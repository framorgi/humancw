#include "humancw/audio/audio_engine.hpp"

#include <algorithm>

namespace humancw::audio {

AudioEngine::AudioEngine() = default;

AudioEngine::~AudioEngine() {
    stop();
    terminate();
}

bool AudioEngine::init() {
    last_err_    = Pa_Initialize();
    initialised_ = (last_err_ == paNoError);
    return initialised_;
}

void AudioEngine::terminate() {
    if (initialised_) {
        Pa_Terminate();
        initialised_ = false;
    }
}

bool AudioEngine::start_output(OutputCallback callback,
                                double         sample_rate,
                                int            frames_per_buf) {
    stop(); // close any existing stream first
    callback_ = std::move(callback);

    PaStreamParameters out_params{};
    out_params.device = Pa_GetDefaultOutputDevice();
    if (out_params.device == paNoDevice) {
        last_err_ = paInvalidDevice;
        return false;
    }
    out_params.channelCount              = 1; // mono
    out_params.sampleFormat              = paFloat32;
    out_params.suggestedLatency          = Pa_GetDeviceInfo(out_params.device)->defaultLowOutputLatency;
    out_params.hostApiSpecificStreamInfo = nullptr;

    last_err_ = Pa_OpenStream(
        &stream_,
        nullptr,    // no input
        &out_params,
        sample_rate,
        static_cast<unsigned long>(frames_per_buf),
        paClipOff,
        &AudioEngine::pa_callback,
        this);

    if (last_err_ != paNoError) {
        stream_ = nullptr;
        return false;
    }

    last_err_ = Pa_StartStream(stream_);
    if (last_err_ != paNoError) {
        Pa_CloseStream(stream_);
        stream_ = nullptr;
        return false;
    }

    return true;
}

void AudioEngine::stop() {
    if (stream_) {
        Pa_StopStream(stream_);
        Pa_CloseStream(stream_);
        stream_ = nullptr;
    }
}

int AudioEngine::pa_callback(const void* /*in*/,
                              void*                           out,
                              unsigned long                   frames,
                              const PaStreamCallbackTimeInfo* /*time_info*/,
                              PaStreamCallbackFlags           /*flags*/,
                              void*                           user_data) {
    auto* self   = static_cast<AudioEngine*>(user_data);
    auto* output = static_cast<float*>(out);

    if (self->callback_) {
        self->callback_(output, static_cast<std::size_t>(frames));
    } else {
        std::fill(output, output + frames, 0.0f);
    }

    return paContinue;
}

} // namespace humancw::audio
