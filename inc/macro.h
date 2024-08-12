#ifndef MACRO_H
#define MACRO_H
#define PRINT_SDL_ERR(stream, msg) fprintf(stream, "%s\n", msg)
#define PRINT_STR_ERR(stream, context, msg) fprintf(stream, "%s : %s\n", context, msg)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define ABS(x) (((x) < 0) ? -(x) : (x))
#define DIFF(a, b) ABS((a) - (b))
#define Y_OFFSET(x) (x + 50)
#define free_ptr(ptr)                                                                                        \
  if ((ptr) != NULL) {                                                                                       \
    free(ptr);                                                                                               \
    (ptr) = NULL;                                                                                            \
  }

#endif
