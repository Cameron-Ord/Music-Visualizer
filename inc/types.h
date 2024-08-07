#ifndef TYPES_H
#define TYPES_H
#include "macdef.h"
#include "macro.h"
#include <complex.h>
#include <inttypes.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef float    f32;
typedef double   f64;
typedef float _Complex f32c;

typedef struct PathContainer     PathContainer;
typedef struct AppContext        AppContext;
typedef struct VolBar            VolBar;
typedef struct SDLMouse          SDLMouse;
typedef struct SDLContainer      SDLContainer;
typedef struct SDLContext        SDLContext;
typedef struct FourierTransform  FourierTransform;
typedef struct FileContext       FileContext;
typedef struct FontContext       FontContext;
typedef struct SongState         SongState;
typedef struct DirState          DirState;
typedef struct TTFData           TTFData;
typedef struct FontState         FontState;
typedef struct FTransformData    FTransformData;
typedef struct AudioData         AudioData;
typedef struct FTransformBuffers FTransformBuffers;
typedef struct PlaybackState     PlaybackState;
typedef struct FileState         FileState;
typedef struct SeekBar           SeekBar;
typedef struct ActiveSong        ActiveSong;
typedef struct FontData          FontData;
typedef struct SDLContext        SDLContext;
typedef struct Positions         Positions;
typedef struct SettingsGear      SettingsGear;
typedef struct SDLColours        SDLColours;
typedef struct SDLViewports      SDLViewports;
typedef struct ListLimiter       ListLimiter;
typedef struct SDLSprites        SDLSprites;
typedef struct PlayIcon          PlayIcon;
typedef struct PauseIcon         PauseIcon;
typedef struct StopIcon          StopIcon;
typedef struct SeekIcon          SeekIcon;
typedef struct Theme             Theme;
#endif
