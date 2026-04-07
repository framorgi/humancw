#include <portaudio.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <string>

int main() {
    // Initialize PortAudio
    PaError pa_err = Pa_Initialize();
    std::string audio_status;
    if (pa_err != paNoError) {
        audio_status = std::string("PortAudio error: ") + Pa_GetErrorText(pa_err);
    } else {
        int device_count = Pa_GetDeviceCount();
        if (device_count < 0) {
            audio_status = std::string("PortAudio error listing devices: ") +
                           Pa_GetErrorText(static_cast<PaError>(device_count));
        } else if (device_count == 0) {
            audio_status = "PortAudio ready – no audio devices found";
        } else {
            audio_status = "PortAudio ready – devices found: " + std::to_string(device_count);
        }
    }

    // Build the interactive terminal UI
    auto screen = ftxui::ScreenInteractive::Fullscreen();

    auto quit_button = ftxui::Button(" Quit ", screen.ExitLoopClosure());

    auto layout = ftxui::Container::Vertical({quit_button});

    auto renderer = ftxui::Renderer(layout, [&] {
        return ftxui::vbox({
                   ftxui::text("humancw – Continuous Wave Encoder/Decoder") |
                       ftxui::bold | ftxui::center,
                   ftxui::separator(),
                   ftxui::text(audio_status) | ftxui::center,
                   ftxui::separator(),
                   quit_button->Render() | ftxui::center,
               }) |
               ftxui::border;
    });

    screen.Loop(renderer);

    // Clean up PortAudio
    if (pa_err == paNoError) {
        Pa_Terminate();
    }

    return 0;
}
