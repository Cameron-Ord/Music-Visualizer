#include "../inc/sys.h"
#include "../inc/main.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_entry(Paths *p) {
  if (!p) {
    return;
  }

  if (p->name.path) {
    free(p->name.path);
  }

  if (p->path.path) {
    free(p->path.path);
  }
}

static int not_nav(const char *str) {
  if (strcmp("..", str) == 0) {
    return 0;
  }

  if (strcmp(".", str) == 0) {
    return 0;
  }

  return 1;
}

static int not_hidden(const char *str) {
  if (!strlen(str)) {
    return 0;
  }

  if (str[0] != '.') {
    return 1;
  }

  return 0;
}

static Paths *reallocate_paths(Paths **p, const size_t size) {
  Paths *tmp = realloc(*p, size);
  if (!tmp) {
    errno_string("realloc() failed!", strerror(errno));
    return NULL;
  }

  return tmp;
}

#ifdef __linux__
int get_file_type(const int type) {
  if (type == DT_REG) {
    return TYPE_FILE;
  } else if (type == DT_DIR) {
    return TYPE_DIRECTORY;
  } else {
    return UNKNOWN;
  }
}

Paths *unix_fs_search(const char *pathstr) {
  if (pathstr) {
    Paths *p = unix_read_dir(pathstr);
    return p;
  }

  return NULL;
}

Paths *unix_read_dir(const char *path) {
  printf("Search path -> %s\n", path);
  DIR *dir = opendir(path);
  if (!dir) {
    errno_string("opendir()", strerror(errno));
    return NULL;
  }

  size_t size = 4;
  Paths *d = malloc(sizeof(Paths) * size);
  if (!d) {
    errno_string("malloc()", strerror(errno));
    return NULL;
  }

  size_t count = 0;

  struct dirent *e;
  while ((e = readdir(dir)) != NULL) {
    if (not_nav(e->d_name) && not_hidden(e->d_name)) {
      if (count >= size) {
        size_t new_size = size * 2;
        Paths *tmp = reallocate_paths(&d, new_size * sizeof(Paths));
        if (!tmp) {
          closedir(dir);
          d->is_valid = 0;
          return d;
        }
        d = tmp;
        size = new_size;
      }

      Paths *cur = &d[count];

      cur->cursor = 0;
      cur->is_valid = 0;
      memset(&cur->path, 0, sizeof(StrVals));
      memset(&cur->name, 0, sizeof(StrVals));

      size_t entry_size = strlen(e->d_name);
      cur->name.path = malloc(entry_size + 1);
      if (!cur->name.path) {
        errno_string("malloc()", strerror(errno));
        closedir(dir);
        d->is_valid = 0;
        return d;
      }

      if (!strcpy(cur->name.path, e->d_name)) {
        errno_string("malloc()", strerror(errno));
        closedir(dir);
        d->is_valid = 0;
        return d;
      }

      size_t path_size = strlen(path) + strlen("/") + strlen(e->d_name);
      cur->path.path = malloc(path_size + 1);
      if (!cur->path.path) {
        errno_string("malloc()", strerror(errno));
        closedir(dir);
        d->is_valid = 0;
        return d;
      }

      const char *strings[] = {"/", e->d_name};
      const int str_count = 2;

      if (!strcpy(cur->path.path, path)) {
        errno_string("strcpy()", strerror(errno));
        closedir(dir);
        d->is_valid = 0;
        return d;
      }

      for (int k = 0; k < str_count; k++) {
        if (!strcat(cur->path.path, strings[k])) {
          errno_string("strcat()", strerror(errno));
          closedir(dir);
          d->is_valid = 0;
          return d;
        }
      }

      cur->name.length = entry_size;
      cur->path.length = path_size;
      cur->type = get_file_type(e->d_type);
      cur->is_valid = 1;
      count++;
    }
  }

  // store the size at the pointer
  d->size = count;
  return d;
}
#endif
