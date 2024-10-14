#include "main.h"
#include <assert.h>

void render_draw_text(TextBuffer *list_buf, const size_t *item_count, const size_t *cursor) {
  
  if(!list_buf || !item_count || !cursor || !rend.r){
    return;
  }

  size_t iter_count = rend.title_limit;
  if(iter_count > *item_count){
    iter_count = *item_count;
  }


  int pixel_inc = win.height / (iter_count + 1);
  int p_accumulate = pixel_inc;

  for(size_t i = 0; i < iter_count; i++){
    
    //wrap around
    size_t locn = (*cursor + i) % *item_count; 

    assert(list_buf[locn].text != NULL);
    
    if(!list_buf[locn].text || !list_buf[locn].text->tex){
      return;
    }

    list_buf[locn].text->rect.x = 25;
    list_buf[locn].text->rect.y = p_accumulate;
    p_accumulate += pixel_inc;

    scc(SDL_RenderCopy(rend.r, list_buf[locn].text->tex, NULL, &list_buf[locn].text->rect));

  }   

  size_t bg_locn = (*cursor) % *item_count; 
  
  SDL_Rect text_rect = list_buf[bg_locn].text->rect;
  SDL_Rect bg_rect = {text_rect.x - 5, text_rect.y - 5, text_rect.w + 10, text_rect.h +10};
  
  SDL_SetRenderDrawColor(rend.r, vis.text_bg.r, vis.text_bg.g,vis.text_bg.b, vis.text_bg.a * 0.25);
  SDL_RenderFillRect(rend.r, &bg_rect);
}
