#pragma once

#include <string>
#include <vector>

namespace humancw::cw {

/// A single timed element of a CW signal.
struct Symbol {
    bool  tone;        ///< true = carrier on, false = silence
    float duration_ms; ///< duration in milliseconds
};

/// Converts plain text (A-Z, 0-9, space) into a sequence of CW Symbols.
class Encoder {
public:
    /// CW timing constants (ITU-R M.1677-1, 20 WPM baseline).
    static constexpr float DOT_MS  = 60.0f;          ///< dot duration
    static constexpr float DASH_MS = 3.0f * DOT_MS;  ///< dash duration
    static constexpr float FREQ_HZ = 700.0f;          ///< carrier frequency

    /// Encode @p text into a symbol stream ready for audio synthesis.
    std::vector<Symbol> encode(const std::string& text) const;

private:
    /// Returns the ITU Morse pattern string for @p upper_ch (uppercase letter/digit),
    /// or nullptr if not found.
    static const char* morse_pattern(char upper_ch);
};

} // namespace humancw::cw
