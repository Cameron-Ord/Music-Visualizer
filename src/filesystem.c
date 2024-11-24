#include "filesystem.h"
#include "audio.h"
#include "main.h"
#include "utils.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *seek_path(TextBuffer *buf, const size_t *cursor, Paths *contents) {
  const char *search_key = buf[*cursor].text->name;
  const char *path_str = find_pathstr(search_key, contents);
  return path_str;
}

void fill_file_contents(Paths **file_contents, TextBuffer **file_text_buffer,
                        size_t *file_count, const char *path_str,
                        Paths *(*find_files)(size_t *, const char *)) {

  *file_contents = free_paths(*file_contents, file_count);
  *file_text_buffer = free_text_buffer(*file_text_buffer, file_count);

  if (!path_str) {
    return;
  }

  *file_contents = find_files(file_count, path_str);
  if (*file_count > 0 && *file_contents) {
    *file_text_buffer = create_fonts(*file_contents, file_count);
    if (*file_text_buffer) {
      key.file_cursor = 0;
      vis.last_state = vis.current_state;
      vis.current_state = SONGS;
    } else {
      *file_contents = free_paths(*file_contents, file_count);
    }
  }
}

void select_file(AudioDataContainer *adc, const char *path_str) {
  if (path_str && read_audio_file(path_str, adc)) {
    if (load_song(adc)) {
      vis.last_state = vis.current_state;
      vis.current_state = PLAYBACK;
    } else {
      vis.last_state = vis.current_state;
      vis.current_state = SONGS;
    }
  } else {
    pause_device();
    SDL_CloseAudioDevice(vis.dev);
    vis.last_state = vis.current_state;
    vis.current_state = SONGS;
  }
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

static Paths *reallocate_paths_buffer(Paths *old_ptr, const size_t new_size);
static char *allocate_char_buffer(const size_t bufsize);
static int check_path_bounds(size_t size);
static int check_ascii_bounds(unsigned char character);

static int check_ascii_bounds(unsigned char character) {
  for (size_t i = 0; i < 127; i++) {
    if (character == i) {
      return 0;
    }
  }

  return 1;
}

static int check_path_bounds(size_t size) { return size > PATH_MAX; }

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

Paths *unix_find_directories(size_t *count) {
  char *music_dir = "Music/MVSource";
  char *search_path = NULL;
  int written = 0;

  size_t total_length =
      get_length(3, strlen("/"), strlen(music_dir), strlen(vis.home));

  search_path = malloc(total_length + 16);
  if (!search_path) {
    ERRNO_CALLBACK("Could not allocate pointer", strerror(errno));
    return NULL;
  }

  written =
      snprintf(search_path, total_length + 1, "%s/%s", vis.home, music_dir);
  if (written <= 0) {
    ERRNO_CALLBACK("Could not concatenate char buffer!", strerror(errno));
    free(search_path);
    return NULL;
  }

  printf("Search path -> %s\n", search_path);
  DIR *dir = opendir(search_path);
  if (!dir) {
    free(search_path);
    ERRNO_CALLBACK("Failed to open directory!", strerror(errno));
    return NULL;
  }

  size_t default_size = 4;

  Paths *dpaths = calloc(default_size, sizeof(Paths));
  if (!dpaths) {
    fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  bool search_broken = false;

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".") != 0) {

      if (*count >= default_size) {
        size_t new_size = default_size * 2;

        dpaths = reallocate_paths_buffer(dpaths, new_size);
        if (!dpaths) {
          search_broken = true;
          break;
        }

        default_size = new_size;
      }

      dpaths[*count].path = NULL;
      dpaths[*count].path_length = 0;
      dpaths[*count].name = NULL;
      dpaths[*count].name_length = 0;

      size_t file_name_length = strlen(entry->d_name);
      char *dir_buf = allocate_char_buffer(file_name_length + 1);
      if (!dir_buf) {
        search_broken = true;
        break;
      }

      strcpy(dir_buf, entry->d_name);

      const size_t music_dirpath_len = strlen(music_dir);
      const size_t home_path_len = strlen(vis.home);
      const size_t dir_buf_len = strlen(dir_buf);
      const size_t slash_len = strlen("/");

      size_t path_ttl_length = get_length(5, music_dirpath_len, home_path_len,
                                          dir_buf_len, slash_len, slash_len);

      char *path_buf = NULL;
      if (check_path_bounds(path_ttl_length)) {
        ERRNO_CALLBACK("Path exceeds MAX_PATH!", "No Error");
        free(dir_buf);
        search_broken = true;
        break;
      }

      path_buf = allocate_char_buffer(path_ttl_length + 1);
      if (!path_buf) {
        free(dir_buf);
        search_broken = true;
        break;
      }

      snprintf(path_buf, path_ttl_length + 1, "%s/%s/%s", vis.home, music_dir,
               dir_buf);

      dpaths[*count].path = path_buf;
      dpaths[*count].path_length = path_ttl_length;
      dpaths[*count].name = dir_buf;
      dpaths[*count].name_length = file_name_length;

      (*count)++;
    }
  }

  closedir(dir);
  free(search_path);
  if (search_broken) {
    for (size_t i = 0; i < *count; i++) {
      if (dpaths[i].name) {
        free(dpaths[i].name);
        dpaths[i].name = NULL;
      }

      if (dpaths[i].path) {
        free(dpaths[i].path);
        dpaths[i].path = NULL;
      }
    }

    free_ptrs(1, dpaths);
    return NULL;
  }

  return dpaths;
}

Paths *unix_find_files(size_t *count, const char *path) {
  int written = 0;
  DIR *dir = opendir(path);
  if (!dir) {
    fprintf(stderr, "Failed to open directory! -> %s\n", strerror(errno));
    return NULL;
  }

  fprintf(stdout, "SEARCH_PATH -> %s\n", path);

  size_t default_size = 4;

  Paths *fpaths = calloc(default_size, sizeof(Paths));
  if (!fpaths) {
    fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  bool search_broken = false;
  *count = 0;

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".") != 0) {
      if (*count >= default_size) {
        size_t new_size = default_size * 2;

        fpaths = reallocate_paths_buffer(fpaths, new_size);
        if (!fpaths) {
          search_broken = true;
          break;
        }

        default_size = new_size;
      }

      fpaths[*count].path = NULL;
      fpaths[*count].path_length = 0;
      fpaths[*count].name = NULL;
      fpaths[*count].name_length = 0;

      size_t file_name_length = strlen(entry->d_name);

      char *file_buf[2];
      file_buf[0] = allocate_char_buffer(file_name_length + 1);
      if (!file_buf[0]) {
        search_broken = true;
        break;
      }

      file_buf[1] = allocate_char_buffer(file_name_length + 1);
      if (!file_buf[1]) {
        free(file_buf[0]);
        search_broken = true;
        break;
      }

      strcpy(file_buf[0], entry->d_name);
      strcpy(file_buf[1], entry->d_name);

      bool contains_non_ascii = false;
      for (size_t f = 0; f < file_name_length; f++) {
        if (check_ascii_bounds(file_buf[1][f])) {
          file_buf[1][f] = 95;
          contains_non_ascii = true;
        }
      }

      const size_t music_dirpath_len = strlen(path);
      const size_t file_buf_len = strlen(file_buf[0]);
      const size_t slash_len = strlen("/");

      size_t path_ttl_length =
          get_length(4, music_dirpath_len, file_buf_len, slash_len, slash_len);

      if (check_path_bounds(path_ttl_length)) {
        ERRNO_CALLBACK("Path length exceeds MAX!", "No Error");
        free(file_buf[0]);
        free(file_buf[1]);
        search_broken = true;
        break;
      }

      char *path_buf[2];
      path_buf[0] = allocate_char_buffer(path_ttl_length + 1);
      if (!path_buf[0]) {
        free(file_buf[0]);
        free(file_buf[1]);
        search_broken = true;
        break;
      }

      path_buf[1] = allocate_char_buffer(path_ttl_length + 1);
      if (!path_buf[1]) {
        free(file_buf[0]);
        free(file_buf[1]);
        free(path_buf[0]);
        search_broken = true;
        break;
      }

      written = snprintf(path_buf[0], path_ttl_length + 1, "%s/%s", path,
                         file_buf[0]);
      if (written <= 0) {
        ERRNO_CALLBACK("Failed to concatenate!", strerror(errno));
        free(file_buf[0]);
        free(file_buf[1]);
        free(path_buf[0]);
        free(path_buf[1]);
        search_broken = true;
        break;
      }

      written = snprintf(path_buf[1], path_ttl_length + 1, "%s/%s", path,
                         file_buf[1]);
      if (written <= 0) {
        ERRNO_CALLBACK("Failed to concatenate!", strerror(errno));
        free(file_buf[0]);
        free(file_buf[1]);
        free(path_buf[0]);
        free(path_buf[1]);
        search_broken = true;
        break;
      }

      char *fn_ptr = file_buf[0];
      char *fpath_ptr = path_buf[0];

      if (contains_non_ascii) {
        rename(path_buf[0], path_buf[1]);
        fn_ptr = file_buf[1];
        fpath_ptr = path_buf[1];

        free(path_buf[0]);
        free(file_buf[0]);
      } else {
        free(file_buf[1]);
        free(path_buf[1]);
      }

      fpaths[*count].path = fpath_ptr;
      fpaths[*count].path_length = path_ttl_length;
      fpaths[*count].name = fn_ptr;
      fpaths[*count].name_length = file_name_length;

      (*count)++;
    }
  }

  closedir(dir);
  if (search_broken) {
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

    free_ptrs(1, fpaths);
    return NULL;
  }

  return fpaths;
}
#endif
