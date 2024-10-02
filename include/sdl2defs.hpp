#ifndef SDL2DEFS_HPP
#define SDL2DEFS_HPP

class AudioData;
class ProgramPath;
class ProgramFiles;
class FourierTransform;
class ProgramThemes;


struct WIN_SIZE {
    int WIDTH;
    int HEIGHT;
};

struct StdClassWrapper {
    AudioData *ad;
    ProgramPath *pathing;
    ProgramFiles *files;
    FourierTransform *fft;
    ProgramThemes *themes;
};

class SDL2INTERNAL;
class SDL2Audio;
class SDL2Renderer;
class SDL2Window;
class SDL2KeyInputs;
class SDL2Fonts;

struct SDL2Wrapper {
    SDL2INTERNAL *sdl2;
    SDL2Audio *sdl2_ad;
    SDL2Renderer *rend;
    SDL2Window *win;
    SDL2KeyInputs *key;
    SDL2Fonts *fonts;
};

#endif