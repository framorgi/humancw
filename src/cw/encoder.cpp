#include "humancw/cw/encoder.hpp"

#include <cctype>
#include <unordered_map>

namespace humancw::cw {

// ITU-R M.1677-1 Morse code table.
static const std::unordered_map<char, const char*> MORSE_TABLE = {
    {'A', ".-"},    {'B', "-..."},  {'C', "-.-."},  {'D', "-.."},
    {'E', "."},     {'F', "..-."},  {'G', "--."},   {'H', "...."},
    {'I', ".."},    {'J', ".---"},  {'K', "-.-"},   {'L', ".-.."},
    {'M', "--"},    {'N', "-."},    {'O', "---"},   {'P', ".--."},
    {'Q', "--.-"},  {'R', ".-."},   {'S', "..."},   {'T', "-"},
    {'U', "..-"},   {'V', "...-"},  {'W', ".--"},   {'X', "-..-"},
    {'Y', "-.--"},  {'Z', "--.."},
    {'0', "-----"}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
    {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."},
    {'8', "---.."}, {'9', "----."},
    {'.', ".-.-.-"},{',', "--..--"},{'?', "..--.."},{'\'',".--.-."},
    {'!', "-.-.--"},{'/', "-..-."}, {'(', "-.--."}, {')', "-.--.-"},
    {'&', ".-..."},  {':', "---..."},{';', "-.-.-."},{'-', "-....-"},
    {'_', "..--.-"},{'"', ".-..-."},{'+', ".-.-."},{'=', "-...-"},
};

const char* Encoder::morse_pattern(char upper_ch) {
    auto it = MORSE_TABLE.find(upper_ch);
    return (it != MORSE_TABLE.end()) ? it->second : nullptr;
}

std::vector<Symbol> Encoder::encode(const std::string& text) const {
    std::vector<Symbol> symbols;
    bool first_char = true;

    for (char raw : text) {
        if (raw == ' ') {
            // Word gap = 7 dots total.  3 dots are added as the inter-letter
            // gap after the preceding character, so we only add 4 more here.
            if (!first_char) {
                symbols.push_back({false, 4.0f * DOT_MS});
            }
            first_char = true;
            continue;
        }

        const unsigned char uc      = static_cast<unsigned char>(raw);
        const char          upper   = static_cast<char>(std::toupper(uc));
        const char*         pattern = morse_pattern(upper);
        if (!pattern) continue;

        if (!first_char) {
            symbols.push_back({false, 3.0f * DOT_MS}); // inter-letter gap
        }
        first_char = false;

        bool first_element = true;
        for (const char* p = pattern; *p != '\0'; ++p) {
            if (!first_element) {
                symbols.push_back({false, DOT_MS}); // intra-letter gap
            }
            first_element = false;
            symbols.push_back({true, (*p == '.') ? DOT_MS : DASH_MS});
        }
    }

    return symbols;
}

} // namespace humancw::cw
