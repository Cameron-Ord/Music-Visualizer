#include "../inc/font.h"
#include "../inc/renderer.h"
#include "../inc/sys.h"
#include "../inc/table.h"
#include "../inc/window.h"

static int char_add(const int cw, const int pad) { return cw + pad; }
static int offset(const int mw, const int cw, const int pad) {
  return mw - (char_add(cw, pad) * 2);
}

void render_node_text(const Paths *p) {
  int max_width = get_window()->width, max_height = get_window()->height;
  int y = max_height * 0.5;
  int padding = 4;

  for (size_t i = p->cursor; i < p->size; i++) {

    int x = 0;
    int h = 0;
    for (size_t k = 0; k < p[i].name.length; k++) {

      const Character *_badchar = atlas_lookup_char('?');
      const Character *_lastchar = atlas_lookup_char('~');
      const Character *_c = atlas_lookup_char(p[i].name.path[k]);

      const int access = p->cursor == (int)i ? 1 : 0;

      switch ((int)(_c != NULL)) {

      case 1: {
        const int chw = char_add(_c->w[access], padding);
        const int offsw = offset(max_width, _c->w[access], padding);

        switch ((int)(x + chw >= offsw)) {
        default:
          break;

        case 0: {
          SDL_Rect char_rect = {x, y, _c->w[access], _c->h[access]};
          SDL_RenderCopy(get_renderer()->r, _c->texture[access], NULL,
                         &char_rect);
          x += _c->w[access] + padding;
          if (_c->h[access] > h) {
            h = _c->h[access];
          }
        } break;

        case 1: {
          if (x + chw < max_width - char_add(_lastchar->w[access], padding)) {
            SDL_Rect char_rect = {x, y, _lastchar->w[access],
                                  _lastchar->h[access]};
            SDL_RenderCopy(get_renderer()->r, _lastchar->texture[access], NULL,
                           &char_rect);
            x += _lastchar->w[0] + padding;
            if (_lastchar->h[0] > h) {
              h = _lastchar->h[0];
            }
          }
        } break;
        }

      } break;

      case 0: {
        const int chw = char_add(_badchar->w[access], padding);
        const int offsw = offset(max_width, _badchar->w[access], padding);

        switch ((int)(x + chw >= offsw)) {
        default:
          break;

        case 0: {
          SDL_Rect char_rect = {x, y, _badchar->w[access], _badchar->h[access]};
          SDL_RenderCopy(get_renderer()->r, _badchar->texture[access], NULL,
                         &char_rect);
          x += _badchar->w[access] + padding;
          if (_badchar->h[access] > h) {
            h = _badchar->h[access];
          }
        } break;

        case 1: {
          if (x + chw < max_width - char_add(_lastchar->w[access], padding)) {
            SDL_Rect char_rect = {x, y, _lastchar->w[access],
                                  _lastchar->h[access]};
            SDL_RenderCopy(get_renderer()->r, _lastchar->texture[access], NULL,
                           &char_rect);
            x += _lastchar->w[0] + padding;
            if (_lastchar->h[0] > h) {
              h = _lastchar->h[0];
            }
          }
        } break;
        }

      } break;
      }
    }
    y += h + padding;

    if (y >= max_height - (h + padding)) {
      return;
    }
  }
}
