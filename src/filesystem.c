#include "filesystem.h"
#include "utils.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static Paths *reallocate_paths(Paths **p, const size_t size);

static Paths *reallocate_paths(Paths **p, const size_t size) {
  Paths *tmp = realloc(*p, size);
  if (!tmp) {
    ERRNO_CALLBACK("realloc() failed!", strerror(errno));
    return NULL;
  }

  return tmp;
}

#ifdef _WIN32

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

static void winapi_err_callback(const char *prefix, DWORD errcode) {
  char *msg = NULL;
  DWORD size =
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
                        FORMAT_MESSAGE_ALLOCATE_BUFFER,
                    NULL, errcode, 0, (LPSTR)&msg, 0, NULL);

  if (!size) {
    fprintf(stderr, "Failed to format error message. Error code: %lu\n",
            errcode);
  } else {
    fprintf(stderr, "Error code: %lu\nMessage: %s -> %s\n", errcode, prefix,
            msg);
  }

  if (msg) {
    LocalFree(msg);
  }
}

int get_file_type(const DWORD attr) {
  if (attr & FILE_ATTRIBUTE_DIRECTORY) {
    return TYPE_DIRECTORY;
  } else if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
    return TYPE_FILE;
  } else {
    return UNKNOWN;
  }
}

Paths *win_fs_search(const char *pathstr) {
  if (pathstr) {
    Paths *p = win_read_dir(pathstr);
    return p;
  }

  return NULL;
}

Paths *win_read_dir(const char *path) {
  const size_t path_length = strlen(path);
  if (!path_length) {
    ERRNO_CALLBACK("bad path size!", "no error");
  }

  char search_buffer[path_length + 64];
  if (!strcpy(search_buffer, path)) {
    ERRNO_CALLBACK("strcpy() failed!", strerror(errno));
    return NULL;
  }

  if (!strcat(search_buffer, "\\")) {
    ERRNO_CALLBACK("strcat() failed!", strerror(errno));
    return NULL;
  }

  if (!strcat(search_buffer, "*")) {
    ERRNO_CALLBACK("strcat() failed!", strerror(errno));
    return NULL;
  }

  printf("Search path -> %s\n", search_buffer);

  WIN32_FIND_DATA ffd;
  HANDLE h_find = FindFirstFile(search_buffer, &ffd);
  if (h_find == INVALID_HANDLE_VALUE) {
    winapi_err_callback("Invalid handle read!", GetLastError());
    return NULL;
  }

  size_t size = 4;
  Paths *d = malloc(sizeof(Paths) * size);
  if (!d) {
    ERRNO_CALLBACK("malloc() failed!", strerror(errno));
    return NULL;
  }

  size_t count = 0;

  do {
    if (not_nav(ffd.cFileName) && not_hidden(ffd.cFileName)) {
      if (count >= size) {
        size_t new_size = size * 2;
        Paths *tmp = reallocate_paths(&d, new_size * sizeof(Paths));
        if (!tmp) {
          FindClose(h_find);
          d->is_valid = 0;
          return d;
        }
        d = tmp;
        size = new_size;
      }

      const char *fn = ffd.cFileName;

      d[count].is_valid = 0;
      d[count].path = NULL;
      d[count].path_length = 0;
      d[count].name = NULL;
      d[count].name_length = 0;

      size_t entry_size = strlen(fn);
      d[count].name = malloc(entry_size + 1);
      if (!d[count].name) {
        ERRNO_CALLBACK("malloc() failed!", strerror(errno));
        FindClose(h_find);
        d->is_valid = 0;
        return d;
      }

      if (!strcpy(d[count].name, fn)) {
        ERRNO_CALLBACK("malloc() failed!", strerror(errno));
        FindClose(h_find);
        d->is_valid = 0;
        return d;
      }

      size_t path_size = get_length(3, strlen(path), strlen("\\"), strlen(fn));
      d[count].path = malloc(path_size + 1);
      if (!d[count].path) {
        ERRNO_CALLBACK("malloc() failed!", strerror(errno));
        FindClose(h_find);
        d->is_valid = 0;
        return d;
      }

      if (!snprintf(d[count].path, path_size + 1, "%s\\%s", path, fn)) {
        ERRNO_CALLBACK("snprintf() failed!", strerror(errno));
        FindClose(h_find);
        d->is_valid = 0;
        return d;
      }

      // This is pretty cool i guess. Works because we just free the allocated
      // stuff and the count variable hasn't been incremented yet so it will
      // just all get overwritten.
      if (get_file_type(ffd.dwFileAttributes) == TYPE_FILE &&
          !check_file_str(d[count].path)) {
        free(d[count].path);
        free(d[count].name);
        continue;
      }

      d[count].name_length = entry_size;
      d[count].path_length = path_size;
      d[count].type = get_file_type(ffd.dwFileAttributes);
      d[count].is_valid = 1;
      count++;
    }
  } while (FindNextFile(h_find, &ffd) != 0);

  d->size = count;
  return d;
}
#endif

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
    ERRNO_CALLBACK("Failed to open directory!", strerror(errno));
    return NULL;
  }

  size_t size = 4;
  Paths *d = malloc(sizeof(Paths) * size);
  if (!d) {
    ERRNO_CALLBACK("malloc() failed!", strerror(errno));
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
      // The current index gets set to invalid by default, this way if the loop
      // errors, we can just return the pointer to the buffer, and decide what
      // to do with the malformed data at that point to maintain a
      // understandable control flow
      d[count].is_valid = 0;
      d[count].path = NULL;
      d[count].path_length = 0;
      d[count].name = NULL;
      d[count].name_length = 0;

      size_t entry_size = strlen(e->d_name);
      d[count].name = malloc(entry_size + 1);
      if (!d[count].name) {
        ERRNO_CALLBACK("malloc() failed!", strerror(errno));
        closedir(dir);
        d->is_valid = 0;
        return d;
      }

      if (!strcpy(d[count].name, e->d_name)) {
        ERRNO_CALLBACK("malloc() failed!", strerror(errno));
        closedir(dir);
        d->is_valid = 0;
        return d;
      }

      size_t path_size =
          get_length(3, strlen(path), strlen("/"), strlen(e->d_name));
      d[count].path = malloc(path_size + 1);
      if (!d[count].path) {
        ERRNO_CALLBACK("malloc() failed!", strerror(errno));
        closedir(dir);
        d->is_valid = 0;
        return d;
      }

      if (!snprintf(d[count].path, path_size + 1, "%s/%s", path, e->d_name)) {
        ERRNO_CALLBACK("snprintf() failed!", strerror(errno));
        closedir(dir);
        d->is_valid = 0;
        return d;
      }

      if(get_file_type(e->d_type) == TYPE_FILE && !check_file_str(d[count].name)){
	free(d[count].path);
	free(d[count].name);
	continue;
      }

      d[count].name_length = entry_size;
      d[count].path_length = path_size;
      d[count].type = get_file_type(e->d_type);
      d[count].is_valid = 1;
      count++;
    }
  }

  // store the size at the pointer
  d->size = count;
  return d;
}
#endif
