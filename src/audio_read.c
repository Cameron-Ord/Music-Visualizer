
#include "../inc/audio.h"
#include "../inc/music_visualizer.h"

int
read_to_buffer(FileContext* FC, SongState* SS, FourierTransform* FT) {

  char* home = getenv(get_platform_env());
  if (home == NULL) {
    PRINT_STR_ERR(stderr, "Err getting home DIR", strerror(errno));
    return -1;
  }

  FileState* FS  = FC->file_state;
  AudioData* Aud = SS->audio_data;
  reset_playback_variables(Aud, SS->pb_state, FT->fft_data);

  char combined_path[PATH_MAX];
  char path[PATH_MAX];

  snprintf(path, PATH_MAX, "%s%sMusic%sfftmplayer%s%s%s", home, get_slash(), get_slash(), get_slash(),
           FS->selected_dir, get_slash());

  sprintf(combined_path, "%s%s", path, FS->files[FS->file_index]);
  printf("READING FILE : %s OF PATH : %s\n", FS->files[FS->file_index], path);

  SNDFILE* sndfile;
  SF_INFO  sfinfo;

  sndfile = sf_open(combined_path, SFM_READ, &sfinfo);
  if (!sndfile) {
    PRINT_STR_ERR(stderr, "Err opening file for reading", strerror(errno));
    return -1;
  }

  if (sfinfo.channels != 2) {
    fprintf(stderr, "Must be a 2 channel audio file!\n");
    return -1;
  }

  Aud->channels = sfinfo.channels;
  Aud->sr       = sfinfo.samplerate;
  Aud->format   = sfinfo.format;

  printf("--CHANNELS : %d\n", Aud->channels);
  printf("--SAMPLE RATE : %d\n", Aud->sr);
  printf("--FORMAT : %x\n", Aud->format);
#ifdef _WIN32
  printf("--FRAMES :  %lld\n", sfinfo.frames);
#endif

#ifdef __linux__
  printf("--FRAMES :  %ld\n", sfinfo.frames);
#endif

  Aud->samples     = sfinfo.frames * sfinfo.channels;
  Aud->total_bytes = Aud->samples * sizeof(f32);

  Aud->buffer = realloc(Aud->buffer, Aud->samples * sizeof(float));
  if (Aud->buffer == NULL) {
    return -1;
  }

  memset(Aud->buffer, 0, Aud->samples * sizeof(float));
  printf("--BUFFER : %p\n", Aud->buffer);

  sf_count_t num_read = sf_read_float(sndfile, Aud->buffer, Aud->samples);
  if (num_read < 0) {
    PRINT_STR_ERR(stderr, "Err reading audio data", strerror(errno));
    free_ptr(Aud->buffer);
    sf_close(sndfile);
    return -1;
  }

  Aud->wav_len = Aud->samples;

  printf("\n..Done reading. Closing file\n\n");
  sf_close(sndfile);

  return 0;
}
