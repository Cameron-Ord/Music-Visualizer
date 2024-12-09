#ifndef FONT_H
#define FONT_H
// These just define some structs
#include "filesysdefs.h"
#include "fontdef.h"
#include "main.h"
void *free_text_buffer(TextBuffer *buf, const size_t *count);
TextBuffer *create_fonts(const Paths *pbuf, SDL_Renderer *r, Font *f,
                         const int w, const SDL_Color *c_text,
                         const SDL_Color *c_sec);
#endif
