#pragma once

#include <portaudio.h>

#include <cstddef>
#include <functional>

namespace humancw::audio {

/// Thin RAII wrapper around a PortAudio output stream.
class AudioEngine {
public:
    /// Callback invoked by PortAudio to fill each audio buffer.
    /// @param out    Float32 mono output buffer (frames samples).
    /// @param frames Number of samples to fill.
    using OutputCallback = std::function<void(float* out, std::size_t frames)>;

    AudioEngine();
    ~AudioEngine();

    // Non-copyable, non-movable (owns raw PaStream*).
    AudioEngine(const AudioEngine&)            = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;

    /// Initialize the PortAudio library.  Must be called before start_output().
    /// @return true on success.
    bool init();

    /// Release the PortAudio library.
    void terminate();

    /// Open and start a mono float32 output stream.
    /// @param callback        Called from the audio thread to fill samples.
    /// @param sample_rate     Desired sample rate in Hz (default 44 100).
    /// @param frames_per_buf  Buffer size in frames (default 256).
    /// @return true on success.
    bool start_output(OutputCallback callback,
                      double          sample_rate    = 44100.0,
                      int             frames_per_buf = 256);

    /// Stop and close the current stream (no-op if not running).
    void stop();

    bool        is_running() const { return stream_ != nullptr; }
    const char* last_error() const { return Pa_GetErrorText(last_err_); }

private:
    static int pa_callback(const void*                     in,
                           void*                           out,
                           unsigned long                   frames,
                           const PaStreamCallbackTimeInfo* time_info,
                           PaStreamCallbackFlags           flags,
                           void*                           user_data);

    PaStream*      stream_   = nullptr;
    PaError        last_err_ = paNoError;
    OutputCallback callback_;
    bool           initialised_ = false;
};

} // namespace humancw::audio
