#pragma once

#include "humancw/audio/audio_engine.hpp"
#include "humancw/cw/encoder.hpp"

#include <ftxui/component/screen_interactive.hpp>

#include <string>

namespace humancw::ui {

/// Top-level FTXUI application.
/// Owns the AudioEngine and Encoder; manages the interactive terminal loop.
class App {
public:
    App();
    ~App() = default;

    // Non-copyable.
    App(const App&)            = delete;
    App& operator=(const App&) = delete;

    /// Enter the blocking FTXUI event loop.
    void run();

private:
    /// Encode input_text_ and play it as CW audio.
    void transmit();

    audio::AudioEngine       engine_;
    cw::Encoder              encoder_;
    ftxui::ScreenInteractive screen_;

    std::string input_text_;
    std::string status_;
};

} // namespace humancw::ui
