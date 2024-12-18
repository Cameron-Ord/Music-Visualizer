#include "renderer.h"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

void render_seek_bar(const uint32_t *position, const uint32_t *length,
                     const int w, const SDL_Color *col, SDL_Renderer *r) {
  SDL_RenderSetViewport(r, NULL);
  float normalized = (float)*position / *length;
  int screen_location = normalized * w;

  int box_width = 14;
  int box_height = 8;

  int y = 0 - (box_height * 0.5);
  int x = screen_location - (box_width * 0.5);

  SDL_Rect box = {x, y, box_width, box_height};
  SDL_SetRenderDrawColor(r, col->r, col->g, col->b, col->a);
  SDL_RenderFillRect(r, &box);
}

static float get_max(const float *even, const float *odd) {
  if (!odd) {
    return *even;
  }

  if (*even > *odd) {
    return *even;
  } else {
    return *odd;
  }
}

static int isg(size_t l, size_t cmp) {
  if (cmp > l - 1) {
    return 1;
  }
  return 0;
}

static int check(const size_t *len, const size_t *even, const size_t *odd) {
  if (isg(*len, *even) || isg(*len, *odd)) {
    return 0;
  }

  return 1;
}

static int cmp_y(int base, int cmp) {
  if (base > cmp) {
    return 1;
  }

  return 0;
}

static SDL_Rect create_rect(const int i, const float smpl, const int h,
                            const int cell_width) {
  // This can overflow, might handle this at some point. But it's annoying
  const int x_pos = (i * (cell_width + cell_width / 2));
  const int y_pos = h - (int)(smpl * (h * 0.9));
  // Height is set afterwards
  SDL_Rect box = {x_pos, y_pos, cell_width, 0};
  return box;
}

static void set_colour(SDL_Renderer *r, const SDL_Color *col) {
  SDL_SetRenderDrawColor(r, col->r, col->g, col->b, col->a);
}

static void fill_rect(SDL_Renderer *r, const SDL_Rect *rect) {
  SDL_RenderFillRect(r, rect);
}

int check_args(RenderArgs *a) {
  if (!a) {
    return 0;
  }

  const int size = 3;
  const void *args[] = {a->length, a->smear, a->smooth};
  for (int i = 0; i < size; i++) {
    if (!args[i]) {
      return 0;
    }
  }

  if (*a->length <= 0) {
    return 0;
  }

  return 1;
}

static int get_cw(const int target, const int w) { return w / target; }

const int min_c_width = 4;
static void determine_ds(int *ds, const size_t *i, const int *w) {
  const size_t iter = *i / *ds;
  const int cell_width = *w / (int)iter;
  if (cell_width < min_c_width) {
    *ds *= 2;
    determine_ds(ds, i, w);
  }
}

void render_draw_music(RenderArgs *args, const int w, const int h,
                       SDL_Renderer *r, const SDL_Color *p, SDL_Color *s) {
  if (!check_args(args)) {
    return;
  }

  SDL_RenderSetViewport(r, NULL);

  const size_t *len = args->length;
  const float *smear = args->smear;
  const float *smoothed = args->smooth;

  int ds = 1;
  determine_ds(&ds, len, &w);
  const int target = *len / ds;
  int cell_width = get_cw(target, w);
  for (int i = 0; i < target; i++) {
    SDL_Rect sample_frame = {0};
    SDL_Rect smear_frame = {0};

    if (ds > 1) {
      // Access by even and odd indexes. If the DS is 2 then basically most of
      // the data is transfered over since we do a comparison and grab the max,
      // if ds is greater than 2, then information is lost as it begins to skip
      // parts.
      const size_t even = i * 2;
      const size_t odd = i * 2 + 1;
      // If one of these accesses out of bounds, literally just use the
      // last(current) iteration as an access and use that value
      if (!check(len, &even, &odd)) {
        sample_frame = create_rect(i, smoothed[i], h, cell_width);
        smear_frame = create_rect(i, smear[i], h, cell_width);

        smear_frame.h = sample_frame.y - smear_frame.y;
        sample_frame.h = smoothed[i] * (h * 0.9);

      } else {
        const float sample_max = get_max(&smoothed[even], &smoothed[odd]);
        const float smear_max = get_max(&smear[even], &smear[odd]);

        sample_frame = create_rect(i, sample_max, h, cell_width);
        smear_frame = create_rect(i, smear_max, h, cell_width);

        smear_frame.h = sample_frame.y - smear_frame.y;
        sample_frame.h = sample_max * (h * 0.9);
      }
    } else {
      sample_frame = create_rect(i, smoothed[i], h, cell_width);
      smear_frame = create_rect(i, smear[i], h, cell_width);

      smear_frame.h = sample_frame.y - smear_frame.y;
      sample_frame.h = smoothed[i] * (h * 0.9);
    }

    if (cmp_y(sample_frame.y, smear_frame.y)) {
      set_colour(r, s);
      fill_rect(r, &smear_frame);
    }

    set_colour(r, p);
    fill_rect(r, &sample_frame);
  }
}
