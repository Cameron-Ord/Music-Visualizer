#include "../include/audio.hpp"
#include "../include/fft.hpp"
#include "../include/macdefs.hpp"

SDL2Audio::SDL2Audio() {
    next_song_flag = 0;
    audio_streaming = 0;
}

void callback(void *data, uint8_t *stream, int len) {
    USERDATA *userdata = static_cast<USERDATA *>(data);

    const uint32_t length = userdata->ad->get_audio_data()->length;
    uint32_t *pos = &userdata->ad->get_audio_data()->position;

    const uint32_t uint32_len = static_cast<uint32_t>(len);
    const uint32_t samples = uint32_len / sizeof(float);
    const uint32_t remaining = (length - *pos);

    const uint32_t copy = (samples < remaining) ? samples : remaining;

    float *f32_stream = reinterpret_cast<float *>(stream);
    for (uint32_t i = 0; i < copy; i++) {
        f32_stream[i] = userdata->ad->get_audio_data()->buffer[i + *pos] * 1.0;
    }

    userdata->fft->fft_push(*pos, userdata->ad->get_audio_data()->buffer,
                            copy * sizeof(float));
    *pos += copy;

    if (*pos >= length) {
        userdata->sdl2_ad->set_flag(1, userdata->sdl2_ad->get_next_song_flag());
    }
}

void SDL2Audio::set_audio_spec(USERDATA *userdata) {
    spec.userdata = userdata;
    spec.callback = callback;
    spec.channels = userdata->ad->get_audio_data()->channels;
    spec.freq = userdata->ad->get_audio_data()->SR;
    spec.format = AUDIO_F32;
    spec.samples = BUFF_SIZE;
}

bool SDL2Audio::open_audio_device() {
    dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
    if (!dev) {
        fprintf(stderr, "Could not open audio device! -> %s\n", SDL_GetError());
        return false;
    }
    return true;
}

void SDL2Audio::close_audio_device() {
    SDL_CloseAudioDevice(dev);
}

void SDL2Audio::pause_audio() {
    int status = SDL_GetAudioDeviceStatus(dev);
    if (status) {
        SDL_PauseAudioDevice(dev, true);
        audio_streaming = 0;
    }
}

void SDL2Audio::resume_audio() {
    int status = SDL_GetAudioDeviceStatus(dev);
    if (status) {
        SDL_PauseAudioDevice(dev, false);
        audio_streaming = 1;
    }
}

void SDL2Audio::set_flag(int flag, int *mutable_value) {
    *mutable_value = flag;
}

int *SDL2Audio::get_next_song_flag() {
    return &next_song_flag;
}

int SDL2Audio::get_stream_flag() {
    return audio_streaming;
}

SDL_AudioDeviceID *SDL2Audio::get_device() {
    return &dev;
}
SDL_AudioSpec *SDL2Audio::get_spec() {
    return &spec;
}

SDL2Audio::~SDL2Audio() {}
