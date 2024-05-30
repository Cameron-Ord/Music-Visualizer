#ifndef TYPES_H
#define TYPES_H
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

typedef struct ThreadWrapper     ThreadWrapper;
typedef struct LogThread         LogThread;
typedef struct HannThread        HannThread;
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

#endif
