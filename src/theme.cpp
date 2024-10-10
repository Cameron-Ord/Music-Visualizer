#include "../include/theme.hpp"
#include <cmath>
#include <cstdint>

ProgramThemes::ProgramThemes() {
  SDL_Color primary = {166, 172, 205, 255};
  SDL_Color secondary = {68, 66, 103, 255};
  SDL_Color background = {41, 45, 62, 255};
  SDL_Color text = {103, 110, 149, 255};
  SDL_Color text_bg = {113, 124, 180, 255};

  set_color(primary, PRIMARY);
  set_color(secondary, SECONDARY);
  set_color(background, BACKGROUND);
  set_color(text, TEXT);
  set_color(text_bg, TEXT_BG);
}

ProgramThemes::~ProgramThemes() {}

const SDL_Color *ProgramThemes::get_primary() { return &primary; }

const SDL_Color *ProgramThemes::get_secondary() { return &secondary; }

const SDL_Color *ProgramThemes::get_background() { return &background; }

const SDL_Color *ProgramThemes::get_text() { return &text; }

const SDL_Color *ProgramThemes::get_textbg() { return &text_bg; }

HSL_TO_RGB phase_hue_effect(const float *phased_hue) {
  float C = 0.5;
  float X = C * (1 - fabs(fmod(*phased_hue / 60.f, 2) - 1));
  float m = 0.5;

  HSL_TO_RGB converted;

  float r_prime = 0.0;
  float g_prime = 0.0;
  float b_prime = 0.0;

  if (*phased_hue < 60) {
    r_prime = C;
    g_prime = X;
    b_prime = 0;
  } else if (*phased_hue < 120) {
    r_prime = X;
    g_prime = C;
    b_prime = 0;
  } else if (*phased_hue < 180) {
    r_prime = 0;
    g_prime = C;
    b_prime = X;
  } else if (*phased_hue < 240) {
    r_prime = 0;
    g_prime = X;
    b_prime = C;
  } else if (*phased_hue < 300) {
    r_prime = X;
    g_prime = 0;
    b_prime = C;
  } else {
    r_prime = C;
    g_prime = 0;
    b_prime = X;
  }

  converted.r = static_cast<uint8_t>((r_prime + m) * UINT8_MAX);
  converted.g = static_cast<uint8_t>((g_prime + m) * UINT8_MAX);
  converted.b = static_cast<uint8_t>((b_prime + m) * UINT8_MAX);

  return converted;
}

const float *ProgramThemes::get_hue(int TYPE) {
  switch (TYPE) {
  case PRIMARY: {
    return &primary_hue;
  }
  case SECONDARY: {
    return &secondary_hue;
  }
  case BACKGROUND: {
    return &background_hue;
  }
  case TEXT: {
    return &text_hue;
  }
  case TEXT_BG: {
    return &text_bg_hue;
  }
  default: {
    return NULL;
  }
  }
}

float ProgramThemes::convert_rgba_to_hsl(int TYPE) {
  SDL_Color tmp;

  switch (TYPE) {
  default: {
    return 0.0;
  }
  case PRIMARY: {
    tmp = primary;
    break;
  }
  case SECONDARY: {
    tmp = secondary;
    break;
  }
  case TEXT: {
    tmp = text;
    break;
  }
  case TEXT_BG: {
    tmp = text_bg;
    break;
  }
  case BACKGROUND: {
    tmp = background;
    break;
  }
  }

  float r_prime = tmp.r / 255.0;
  float g_prime = tmp.g / 255.0;
  float b_prime = tmp.b / 255.0;

  float max = fmaxf(r_prime, fmaxf(g_prime, b_prime));
  float min = fminf(r_prime, fminf(g_prime, b_prime));

  float delta = max - min;
  float hue = 0.0;
  float saturation = 0.0;
  float lightness = (max + min) / 2.0;

  if (delta == 0.0) {
    hue = 0.0;
  } else {

    if (max == r_prime) {
      hue = fmodf((g_prime - b_prime) / delta, 6.0);
    } else if (max == g_prime) {
      hue = (b_prime - r_prime) / delta + 2.0;
    } else {
      hue = (r_prime - g_prime) / delta + 4.0;
    }

    hue *= 60.0;
    if (hue < 0) {
      hue += 360.0;
    }
  }

  return hue;
}

void ProgramThemes::set_hue_from_rgba(int TYPE) {
  switch (TYPE) {
  case PRIMARY: {
    primary_hue = convert_rgba_to_hsl(PRIMARY);
    break;
  }

  case SECONDARY: {
    secondary_hue = convert_rgba_to_hsl(SECONDARY);
    break;
  }

  case BACKGROUND: {
    background_hue = convert_rgba_to_hsl(BACKGROUND);
    break;
  }

  case TEXT: {
    text_hue = convert_rgba_to_hsl(TEXT);
    break;
  }

  case TEXT_BG: {
    text_bg_hue = convert_rgba_to_hsl(TEXT_BG);
    break;
  }

  default: {
    break;
  }
  }
}

void ProgramThemes::set_color(SDL_Color to_color, int TYPE) {
  switch (TYPE) {
  case PRIMARY: {
    primary = to_color;
    break;
  }
  case SECONDARY: {
    secondary = to_color;
    break;
  }
  case BACKGROUND: {
    background = to_color;
    break;
  }
  case TEXT: {
    text = to_color;
    break;
  }
  case TEXT_BG: {
    text_bg = to_color;
    break;
  }
  default: {
    break;
  }
  }
}
