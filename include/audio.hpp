#ifndef AUDIO_HPP
#define AUDIO_HPP
#include "audiodefs.hpp"
#include "files.hpp"
#include "paths.hpp"
#include "threads.hpp"
#include <SDL2/SDL_audio.h>
#include <sndfile.h>

class AudioData {
  public:
    AudioData();
    ~AudioData();
    void zero_values();
    bool read_audio_file(std::string file_path);

    void set_audio_data_ptr(AudioDataContainer *audio_data);
    AudioDataContainer *get_audio_data();
  private:
    AudioDataContainer *a_data;
};

class SDL2Audio {
  public:
    SDL2Audio();
    ~SDL2Audio();
    void set_audio_spec(AudioDataContainer *a);

    void audio_state_handler(AudioData *ad, ProgramFiles *files, ProgramPath *pathing,ThreadData *FTransformThread);
    void pause_audio();
    void resume_audio();

    bool open_audio_device();
    void close_audio_device();

    void set_flag(int flag, int *mutable_flag);
    int *get_next_song_flag();
    int *get_stream_flag();

    SDL_AudioDeviceID *get_device();
    SDL_AudioSpec *get_spec();

  private:
    int next_song_flag;
    int audio_streaming;
    SDL_AudioSpec spec;
    SDL_AudioDeviceID dev;
};

#endif
