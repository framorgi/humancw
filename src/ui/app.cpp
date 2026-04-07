#include "humancw/ui/app.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include <atomic>
#include <cmath>
#include <memory>
#include <vector>

namespace humancw::ui {

static constexpr double SAMPLE_RATE    = 44100.0;
static constexpr double TWO_PI         = 6.283185307179586;

App::App()
    : screen_(ftxui::ScreenInteractive::Fullscreen()) {
    if (!engine_.init()) {
        status_ = std::string("PortAudio init failed: ") + engine_.last_error();
    } else {
        status_ = "Ready.";
    }
}

void App::transmit() {
    if (input_text_.empty()) {
        status_ = "Nothing to transmit.";
        return;
    }

    engine_.stop();

    auto symbols = encoder_.encode(input_text_);

    // Pre-render symbols to a PCM buffer.
    std::vector<float> pcm;
    pcm.reserve(static_cast<std::size_t>(input_text_.size() * 4000));

    for (const auto& sym : symbols) {
        const auto n = static_cast<std::size_t>(sym.duration_ms * SAMPLE_RATE / 1000.0);
        for (std::size_t i = 0; i < n; ++i) {
            float s = 0.0f;
            if (sym.tone) {
                double t = static_cast<double>(i) / SAMPLE_RATE;
                s = 0.5f * static_cast<float>(
                        std::sin(TWO_PI * cw::Encoder::FREQ_HZ * t));
            }
            pcm.push_back(s);
        }
    }

    status_ = "Transmitting: " + input_text_;

    auto shared_pcm = std::make_shared<std::vector<float>>(std::move(pcm));
    auto pos        = std::make_shared<std::atomic<std::size_t>>(0u);

    engine_.start_output(
        [shared_pcm, pos](float* out, std::size_t frames) {
            std::size_t p = pos->load(std::memory_order_relaxed);
            for (std::size_t i = 0; i < frames; ++i) {
                out[i] = (p < shared_pcm->size()) ? (*shared_pcm)[p++] : 0.0f;
            }
            pos->store(p, std::memory_order_relaxed);
        },
        SAMPLE_RATE);
}

void App::run() {
    auto input = ftxui::Input(&input_text_, "Type text to send as CW…");

    auto btn_transmit = ftxui::Button(" Transmit ", [this] { transmit(); });
    auto btn_stop     = ftxui::Button(
        " Stop ", [this] { engine_.stop(); status_ = "Stopped."; });
    auto btn_quit = ftxui::Button(" Quit ", screen_.ExitLoopClosure());

    auto buttons = ftxui::Container::Horizontal({btn_transmit, btn_stop, btn_quit});
    auto layout  = ftxui::Container::Vertical({input, buttons});

    auto renderer = ftxui::Renderer(layout, [&] {
        return ftxui::vbox({
                   ftxui::text("humancw – Continuous Wave (Morse) Encoder")
                       | ftxui::bold | ftxui::center,
                   ftxui::separator(),
                   ftxui::hbox({ftxui::text("Input : "), input->Render() | ftxui::flex}),
                   ftxui::separator(),
                   ftxui::hbox({
                       btn_transmit->Render(),
                       ftxui::text("  "),
                       btn_stop->Render(),
                       ftxui::text("  "),
                       btn_quit->Render(),
                   }) | ftxui::center,
                   ftxui::separator(),
                   ftxui::text(status_) | ftxui::center,
               })
               | ftxui::border;
    });

    screen_.Loop(renderer);
    engine_.stop();
}

} // namespace humancw::ui
