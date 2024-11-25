#include "filesystem.h"
#include "utils.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static Paths *reallocate_paths_buffer(Paths *old_ptr, const size_t new_size);
static char *allocate_char_buffer(const size_t bufsize);
static int check_path_bounds(size_t size);
static int check_ascii_bounds(unsigned char character);

static int check_ascii_bounds(unsigned char character) {
  return character > 127;
}

static int check_path_bounds(size_t size) { return size > MAX_PATH; }

static char *allocate_char_buffer(const size_t bufsize) {
  char *buf = malloc(bufsize);
  if (!buf) {
    ERRNO_CALLBACK("Could not allocate pointer!", strerror(errno));
    return NULL;
  }

  return buf;
}

static Paths *reallocate_paths_buffer(Paths *old_ptr, const size_t new_size) {
  Paths *temp_buffer = (Paths *)realloc(old_ptr, sizeof(Paths) * new_size);
  if (!temp_buffer) {
    ERRNO_CALLBACK("Could not allocate pointer!", strerror(errno));
    return NULL;
  }

  return temp_buffer;
}

Paths *win_find_directories(size_t *count) {
  WIN32_FIND_DATA find_file_data;
  HANDLE h_find;

  char *music_dir = "Music\\MVSource\\*";
  char *music_dir_no_wc = "Music\\MVSource\\";
  int written = 0;

  size_t total_length =
      get_length(3, strlen("\\"), strlen(music_dir), strlen(vis.home));

  char *search_path = malloc(total_length + 16);
  if (!search_path) {
    ERRNO_CALLBACK("Failed to allocate pointer!", strerror(errno));
    return NULL;
  }

  written =
      snprintf(search_path, total_length + 1, "%s\\%s", vis.home, music_dir);
  if (written <= 0) {
    ERRNO_CALLBACK("Failed to concatenate!", strerror(errno));
    return NULL;
  }

  fprintf(stdout, "SEARCH PATH -> %s\n", search_path);

  size_t default_size = 4;
  Paths *paths = calloc(default_size, sizeof(Paths));
  if (!paths) {
    fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  h_find = FindFirstFile(search_path, &find_file_data);
  if (h_find == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Could not read filesystem! -> %s\n", search_path);
    return NULL;
  }

  *count = 0; // Initialize count
  bool find_file_broken = false;

  do {
    if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      if (strcmp(find_file_data.cFileName, "..") != 0 &&
          strcmp(find_file_data.cFileName, ".") != 0) {
        // Check if we need to resize the buffer
        if (*count >= default_size) {
          size_t new_size = default_size * 2;
          paths = reallocate_paths_buffer(paths, new_size);
          if (!paths) {
            find_file_broken = true;
            break;
          }
          default_size = new_size;
        }

        paths[*count].path = NULL;
        paths[*count].path_length = 0;
        paths[*count].name = NULL;
        paths[*count].name_length = 0;

        size_t file_name_length = strlen(find_file_data.cFileName);
        char *dir_buffer = allocate_char_buffer(file_name_length + 1);
        if (!dir_buffer) {
          find_file_broken = true;
          break;
        }

        strcpy_s(dir_buffer, file_name_length + 1, find_file_data.cFileName);

        size_t path_ttl_length =
            get_length(5, strlen(music_dir_no_wc), strlen(vis.home),
                       strlen("\\"), strlen(dir_buffer), strlen("\\"));

        char *path_buffer = NULL;
        if (check_path_bounds(path_ttl_length)) {
          ERRNO_CALLBACK("Length exceeds MAX!", "No Error");
          free(dir_buffer);
          find_file_broken = true;
          break;
        }

        path_buffer = allocate_char_buffer(path_ttl_length + 1);
        if (!path_buffer) {
          free(dir_buffer);
          find_file_broken = true;
          break;
        }

        written = snprintf(path_buffer, path_ttl_length + 1, "%s\\%s%s\\",
                           vis.home, music_dir_no_wc, dir_buffer);
        if (written <= 0) {
          ERRNO_CALLBACK("Failed to concatenate!", strerror(errno));
          free(dir_buffer);
          free(path_buffer);
          find_file_broken = true;
          break;
        }

        paths[*count].path = path_buffer;
        paths[*count].path_length = path_ttl_length;
        paths[*count].name = dir_buffer;
        paths[*count].name_length = file_name_length;

        (*count)++;
      }
    }

  } while (FindNextFile(h_find, &find_file_data) != 0);

  free(search_path);
  // Tested and works as far as I know.
  if (find_file_broken) {
    for (size_t i = 0; i < *count; i++) {
      if (paths[i].name) {
        free(paths[i].name);
        paths[i].name = NULL;
      }

      if (paths[i].path) {
        free(paths[i].path);
        paths[i].path = NULL;
      }
    }

    free_ptrs(1, paths);
    FindClose(h_find);
    return NULL;
  }

  FindClose(h_find);
  return paths;
}

Paths *win_find_files(size_t *count, const char *path) {
  WIN32_FIND_DATA find_file_data;
  HANDLE h_find;
  int written = 0;

  size_t default_size = 4;
  Paths *fpaths = calloc(default_size, sizeof(Paths));
  if (!fpaths) {
    fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  size_t path_length = strlen(path);
  if (path_length == 0) {
    fprintf(stderr, "Failed to get char len! -> %s\n", strerror(errno));
    return NULL;
  }

  char *path_cpy = malloc(path_length + 16);
  if (!path_cpy) {
    fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  strcpy_s(path_cpy, path_length + 2, path);
  strcat_s(path_cpy, path_length + 2, "*");

  fprintf(stdout, "RETRIEVING FILES FROM PATH -> %s\n", path_cpy);

  h_find = FindFirstFile(path_cpy, &find_file_data);
  if (h_find == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Could not read filesystem! -> %s\n", path_cpy);
    free(path_cpy);
    return NULL;
  }

  *count = 0;
  bool find_file_broken = false;

  do {
    if (!(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

      if (strcmp(find_file_data.cFileName, "..") != 0 &&
          strcmp(find_file_data.cFileName, ".") != 0) {
        if (*count >= default_size) {
          size_t new_size = default_size * 2;
          fpaths = reallocate_paths_buffer(fpaths, new_size);
          if (!fpaths) {
            find_file_broken = true;
            break;
          }
          default_size = new_size;
        }

        fpaths[*count].path = NULL;
        fpaths[*count].path_length = 0;
        fpaths[*count].name = NULL;
        fpaths[*count].name_length = 0;

        size_t file_name_length = strlen(find_file_data.cFileName);
        char *file_buffer = allocate_char_buffer(file_name_length + 1);
        if (!file_buffer) {
          find_file_broken = true;
          break;
        }

        strcpy_s(file_buffer, file_name_length + 1, find_file_data.cFileName);

        size_t path_ttl_length =
            get_length(2, strlen(path), strlen(file_buffer));

        char *path_buffer = NULL;
        if (check_path_bounds(path_ttl_length)) {
          ERRNO_CALLBACK("Path length exceeds MAX!", "No Error");
          free(file_buffer);
          find_file_broken = true;
          break;
        }

        path_buffer = malloc(path_ttl_length + 1);
        if (!path_buffer) {
          free(file_buffer);
          find_file_broken = true;
          break;
        }

        written = snprintf(path_buffer, path_ttl_length + 1, "%s%s", path,
                           file_buffer);
        if (written <= 0) {
          ERRNO_CALLBACK("Failed to concatenate!", strerror(errno));
          free(file_buffer);
          free(path_buffer);
          find_file_broken = true;
          break;
        }

        fpaths[*count].path = path_buffer;
        fpaths[*count].path_length = path_ttl_length;
        fpaths[*count].name = file_buffer;
        fpaths[*count].name_length = file_name_length;

        (*count)++;
      }
    }

  } while (FindNextFile(h_find, &find_file_data) != 0);

  if (find_file_broken) {
    for (size_t i = 0; i < *count; i++) {
      if (fpaths[i].name) {
        free(fpaths[i].name);
        fpaths[i].name = NULL;
      }

      if (fpaths[i].path) {
        free(fpaths[i].path);
        fpaths[i].path = NULL;
      }
    }

    free_ptrs(2, path_cpy, fpaths);
    FindClose(h_find);
    return NULL;
  }

  free(path_cpy);
  FindClose(h_find);
  return fpaths;
}

#endif

#ifdef __linux__
Paths *unix_fs_search(const char *pathstr, const char *home) {
  if (!pathstr) {
    Paths *p = unix_read_dir(home);
    return p;
  }

  if (!home) {
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
    if (not_nav(e->d_name)) {
      if (count >= size) {
        size_t new_size = size * 2;
        d = reallocate_paths(&d, new_size * sizeof(Paths));
        if (!d) {
          closedir(dir);
          return NULL;
        }
        size = new_size;
      }

      d[count].path = NULL;
      d[count].path_length = 0;
      d[count].name = NULL;
      d[count].name_length = 0;

      size_t entry_size = strlen(e->d_name);
      d[count].name = malloc(entry_size + 1);
      if (!d[count].name) {
        ERRNO_CALLBACK("malloc() failed!", strerror(errno));
        closedir(dir);
        return NULL;
      }

      if (!strcpy(d[count].name, e->d_name)) {
        ERRNO_CALLBACK("malloc() failed!", strerror(errno));
        closedir(dir);
        return NULL;
      }

      size_t path_size =
          get_length(3, strlen(path), strlen("/"), strlen(e->d_name));
      d[count].path = malloc(path_size + 1);
      if (!d[count].path) {
        ERRNO_CALLBACK("malloc() failed!", strerror(errno));
        closedir(dir);
        return NULL;
      }

      if (!snprintf(d[count].path, path_size + 1, "%s/%s", path, e->d_name)) {
        ERRNO_CALLBACK("snprintf() failed!", strerror(errno));
        closedir(dir);
        return NULL;
      }

      d[count].name_length = entry_size;
      d[count].path_length = path_size;
      d[count].type = e->d_type;

      count++;
    }
  }

  // store the size at the pointer
  d->size = count;
  return d;
}

int not_nav(const char *str) {
  if (strcmp("..", str) == 0) {
    return 0;
  }

  if (strcmp(".", str) == 0) {
    return 0;
  }

  return 1;
}

#endif
