#include "main.h"

size_t nav_down(NavListArgs *list_args) {
  int signed_cursor = (int)*list_args->cursor;
  int signed_length = (int)list_args->list[*list_args->list_index].size;
  int signed_list_index = (int)*list_args->list_index;

  //This one is a little simpler.
  signed_cursor++;
  if (signed_cursor > signed_length - 1) {
    if(*list_args->list_index < list_args->list_size - 1){
      signed_list_index++;
      signed_cursor = 0;
    } else {
      signed_list_index = 0;
      signed_cursor = 0;
    }
  }
  
  *list_args->cursor = (size_t)signed_list_index;
  *list_args->cursor = (size_t)signed_cursor;

  return (size_t)signed_cursor;
}

size_t nav_up(NavListArgs *list_args) {
  int signed_cursor = (int)*list_args->cursor;
  int signed_list_index = (int)*list_args->list_index;
  //Initial cursor decrement.
  signed_cursor--;
  if (signed_cursor < 0) {
    //Walk down the list
    signed_list_index--;
    if(signed_list_index < 0){
      //Wrap around
      signed_list_index = (int)(list_args->list_size -1);
    }
    
    signed_cursor = (int)list_args->list[(size_t)signed_list_index].size - 1;
  }

  //Assign the variables to their pointers
  *list_args->list_index = (size_t)signed_list_index;
  *list_args->cursor = (size_t)signed_cursor;
  
  return (size_t)signed_cursor;
}