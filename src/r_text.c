#include "../inc/font.h"
#include "../inc/renderer.h"
#include "../inc/sys.h"
#include "../inc/table.h"
#include "../inc/window.h"

void render_node_text(const Paths *p) {
  int max_width = get_window()->width, max_height = get_window()->height;
  int y = 0;
  int padding = 4;

  for (size_t i = p->cursor; i < p->size; i++) {
    int x = 0;
    int h = 0;
    for (size_t k = 0; k < p[i].name.length; k++) {
      const Character *_c = atlas_lookup_char(p[i].name.path[k]);

      if (_c) {
        SDL_Rect char_rect = {x, y, _c->w[0], _c->h[0]};
        SDL_RenderCopy(get_renderer()->r, _c->texture[0], NULL, &char_rect);
      }

      x += _c->w[0] + padding;
      if (_c->h[0] > h) {
        h = _c->h[0];
      }
    }
    y += h + padding;
  }
}
