#ifndef USERDATA_DEF_H
#define USERDATA_DEF_H
#include <stdint.h>
#include <stdlib.h>

class SDL2Audio;
class AudioData;

struct USERDATA {
  class SDL2Audio *sdl2_ad;
  class AudioData *ad;
};

typedef USERDATA USERDATA;
#endif
