#ifndef STRUCTDEF_H
#define STRUCTDEF_H

class SDL2INTERNAL;
class SDL2Audio;
class SDL2Renderer;
class SDL2Window;
class SDL2KeyInputs;
class SDL2Fonts;
class AudioData;
class ProgramPath;
class ProgramFiles;
class FourierTransform;
class ProgramThemes;

struct StdClassWrapper{
 AudioData *ad;
 ProgramPath *pathing;
 ProgramFiles *files;
 FourierTransform *fft;
 ProgramThemes *themes;

};

struct SDL2Wrapper{
    SDL2INTERNAL *sdl2;
    SDL2Audio *sdl2_ad;
    SDL2Renderer *rend;
    SDL2Window *win;
    SDL2KeyInputs *key;
    SDL2Fonts *fonts;
};

struct WIN_SIZE {
    int WIDTH;
    int HEIGHT;
};

typedef WIN_SIZE WIN_SIZE;
typedef StdClassWrapper StdClassWrapper;
typedef SDL2Wrapper SDL2Wrapper;

#endif