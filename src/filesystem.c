#include "filesystem.h"
#include "utils.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
Paths *win_find_directories(size_t *count) {
  WIN32_FIND_DATA find_file_data;
  HANDLE h_find;

  size_t size;
  if (getenv_s(&size, NULL, 0, "USERPROFILE") != 0) {
    fprintf(stderr, "Failed to get USERPROFILE size.\n");
    return NULL;
  }

  char *home = (char *)malloc(size);
  if (!home) {
    fprintf(stderr, "Memory allocation for home failed.\n");
    return NULL;
  }

  if (getenv_s(&size, home, size, "USERPROFILE") != 0) {
    fprintf(stderr, "Failed to get USERPROFILE.\n");
    free(home);
    return NULL;
  }

  char *music_dir = "Music\\MVSource\\*";
  char *music_dir_no_wc = "Music\\MVSource\\";
  char search_path[MAX_PATH];

  size_t total_length =
      get_length(3, strlen("\\"), strlen(music_dir), strlen(home));
  if (total_length > MAX_PATH) {
    fprintf(
        stderr,
        "Total path length exceeds allocated path buffer! -> RETURN NULL\n");
    return NULL;
  }

  snprintf(search_path, sizeof(search_path), "%s\\%s", home, music_dir);

  fprintf(stdout, "SEARCH PATH -> %s\n", search_path);

  size_t default_size = 6;
  Paths *paths = calloc(default_size, sizeof(Paths));
  if (!paths) {
    fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  h_find = FindFirstFile(search_path, &find_file_data);
  if (h_find == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Could not read filesystem! -> %s\n", search_path);
    free(home);
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
          Paths *temp_buffer =
              (Paths *)realloc(paths, sizeof(Paths) * new_size);
          if (!temp_buffer) {
            fprintf(stderr, "Could not reallocate pointer buffer! -> %s\n",
                    strerror(errno));
            find_file_broken = true;
            break;
          }

          paths = temp_buffer;
          default_size = new_size;
        }

        size_t file_name_length = strlen(find_file_data.cFileName);
        char *dir_buffer =
            (char *)malloc((file_name_length + 1) * sizeof(char));
        if (!dir_buffer) {
          fprintf(stderr,
                  "Could not allocate memory for directory name! -> %s\n",
                  strerror(errno));
          find_file_broken = true;
          break;
        }

        strcpy_s(dir_buffer, file_name_length + 1, find_file_data.cFileName);

        size_t path_ttl_length =
            get_length(5, strlen(music_dir_no_wc), strlen(home), strlen("\\"),
                       strlen(dir_buffer), strlen("\\"));

        char *path_buffer = NULL;
        if (path_ttl_length < MAX_PATH) {
          path_buffer = (char *)malloc(sizeof(char) * (path_ttl_length + 1));
          if (!path_buffer) {
            fprintf(stderr, "Could not allocate pointer! -> %s\n",
                    strerror(errno));
            find_file_broken = true;
            break;
          }

          snprintf(path_buffer, path_ttl_length + 1, "%s\\%s%s\\", home,
                   music_dir_no_wc, dir_buffer);
        }

        paths[*count].path = path_buffer;
        paths[*count].path_length = path_ttl_length;
        paths[*count].name = dir_buffer;
        paths[*count].name_length = file_name_length;

        (*count)++;
      }
    }
  } while (FindNextFile(h_find, &find_file_data) != 0);

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

    free_ptrs(2, home, paths);
    FindClose(h_find);
    return NULL;
  }

  FindClose(h_find);
  free(home);
  return paths;
}

Paths *win_find_files(size_t *count, const char *path) {
  WIN32_FIND_DATA find_file_data;
  HANDLE h_find;

  size_t size;
  if (getenv_s(&size, NULL, 0, "USERPROFILE") != 0) {
    fprintf(stderr, "Failed to get USERPROFILE size.\n");
    return NULL;
  }

  char *home = (char *)malloc(size);
  if (!home) {
    fprintf(stderr, "Memory allocation for home failed.\n");
    return NULL;
  }

  if (getenv_s(&size, home, size, "USERPROFILE") != 0) {
    fprintf(stderr, "Failed to get USERPROFILE.\n");
    free(home);
    return NULL;
  }

  size_t default_size = 6;
  Paths *fpaths = calloc(default_size, sizeof(Paths));
  if (!fpaths) {
    fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  size_t path_length = strlen(path);
  if (path_length == 0) {
    fprintf(stderr, "Failed to get char len! -> %s\n", strerror(errno));
    free(home);
    return NULL;
  }

  char *path_cpy = malloc((path_length + 2) * sizeof(char));
  if (!path_cpy) {
    fprintf(stderr, "Failed to allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  strcpy_s(path_cpy, sizeof(char) * (path_length + 2), path);
  strcat_s(path_cpy, sizeof(char) * (path_length + 2), "*");

  fprintf(stdout, "RETRIEVING FILES FROM PATH -> %s\n", path_cpy);

  h_find = FindFirstFile(path_cpy, &find_file_data);
  if (h_find == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Could not read filesystem! -> %s\n", path_cpy);
    free(path_cpy);
    free(home);
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
          Paths *temp_buffer =
              (Paths *)realloc(fpaths, sizeof(Paths) * new_size);
          if (!temp_buffer) {
            fprintf(stderr, "Could not reallocate pointer buffer! -> %s\n",
                    strerror(errno));
            find_file_broken = true;
            break;
          }

          fpaths = temp_buffer;
          default_size = new_size;
        }

        size_t file_name_length = strlen(find_file_data.cFileName);
        char *file_buffer =
            (char *)malloc((file_name_length + 1) * sizeof(char));
        if (!file_buffer) {
          fprintf(stderr,
                  "Could not allocate memory for directory name! -> %s\n",
                  strerror(errno));
          find_file_broken = true;
          break;
        }

        strcpy_s(file_buffer, file_name_length + 1, find_file_data.cFileName);

        size_t path_ttl_length =
            get_length(2, strlen(path), strlen(file_buffer));

        char *path_buffer = NULL;
        if (path_ttl_length < MAX_PATH) {
          path_buffer = (char *)malloc(sizeof(char) * (path_ttl_length + 1));
          if (!path_buffer) {
            fprintf(stderr, "Could not allocate pointer! -> %s\n",
                    strerror(errno));
            find_file_broken = true;
            break;
          }

          snprintf(path_buffer, path_ttl_length + 1, "%s%s", path, file_buffer);
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

    free_ptrs(3, home, path_cpy, fpaths);
    FindClose(h_find);
    return NULL;
  }

  free(path_cpy);
  free(home);

  return fpaths;
}

#endif

// This is not properly implemented yet
#ifdef __linux__
Paths *unix_find_directories(size_t *count) {

  char *home = getenv("HOME");
  if (!home) {
    fprintf(stderr, "Failed to get home path! -> %s\n", strerror(errno));
    return NULL;
  }
  char *music_dir = "Music/MVSource";
  char search_path[PATH_MAX];

  size_t total_length =
      get_length(3, strlen("/"), strlen(music_dir), strlen(home));

  if (total_length > PATH_MAX) {
    fprintf(
        stderr,
        "Total path length exceeds allocated path buffer! -> NULL RETURN\n");
    return NULL;
  }

  snprintf(search_path, sizeof(search_path), "%s/%s", home, music_dir);

  DIR *dir = opendir(search_path);
  if (!dir) {
    fprintf(stderr, "Failed to open directory! -> %s\n", strerror(errno));
    return NULL;
  }

  fprintf(stdout, "SEARCH_PATH -> %s\n", search_path);

  size_t default_size = 6;

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
        Paths *temp_buffer = (Paths *)realloc(dpaths, sizeof(Paths) * new_size);
        if (!temp_buffer) {
          fprintf(stderr, "Could not reallocate pointer buffer! -> %s\n",
                  strerror(errno));
          search_broken = true;
          break;
        }

        dpaths = temp_buffer;
        default_size = new_size;
      }

      size_t file_name_length = strlen(entry->d_name);
      char *dir_buf = (char *)malloc((file_name_length + 1) * sizeof(char));
      if (!dir_buf) {
        fprintf(stderr, "Could not allocate memory for directory name! -> %s\n",
                strerror(errno));
        search_broken = true;
        break;
      }

      strcpy(dir_buf, entry->d_name);

      const size_t music_dirpath_len = strlen(music_dir);
      const size_t home_path_len = strlen(home);
      const size_t dir_buf_len = strlen(dir_buf);
      const size_t slash_len = strlen("/");

      size_t path_ttl_length = get_length(5, music_dirpath_len, home_path_len,
                                          dir_buf_len, slash_len, slash_len);

      char *path_buf = NULL;
      if (path_ttl_length > PATH_MAX) {
        fprintf(stderr, "Path length exceeds max!\n");
        free(dir_buf);
        free(dpaths);
        search_broken = true;
        break;
      }

      path_buf = (char *)malloc(sizeof(char) * (path_ttl_length + 1));
      if (!path_buf) {
        fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
        search_broken = true;
        break;
      }

      snprintf(path_buf, path_ttl_length + 1, "%s/%s/%s", home, music_dir,
               dir_buf);

      dpaths[*count].path = path_buf;
      dpaths[*count].path_length = path_ttl_length;
      dpaths[*count].name = dir_buf;
      dpaths[*count].name_length = file_name_length;

      (*count)++;
    }
  }

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

    free_ptrs(2, home, dpaths);
    closedir(dir);
    return NULL;
  }

  closedir(dir);
  return dpaths;
}

Paths *unix_find_files(size_t *count, const char *path) {
  DIR *dir = opendir(path);
  if (!dir) {
    fprintf(stderr, "Failed to open directory! -> %s\n", strerror(errno));
    return NULL;
  }

  fprintf(stdout, "SEARCH_PATH -> %s\n", path);

  size_t default_size = 6;

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
        Paths *temp_buffer = realloc(fpaths, sizeof(Paths) * new_size);
        if (!temp_buffer) {
          fprintf(stderr, "Could not reallocate pointer buffer! -> %s\n",
                  strerror(errno));
          search_broken = true;
          break;
        }

        fpaths = temp_buffer;
        default_size = new_size;
      }

      size_t file_name_length = strlen(entry->d_name);
      char *dir_buf = (char *)malloc((file_name_length + 1) * sizeof(char));
      if (!dir_buf) {
        fprintf(stderr, "Could not allocate memory for directory name! -> %s\n",
                strerror(errno));
        search_broken = true;
        break;
      }

      strcpy(dir_buf, entry->d_name);

      const size_t music_dirpath_len = strlen(path);
      const size_t dir_buf_len = strlen(dir_buf);
      const size_t slash_len = strlen("/");

      size_t path_ttl_length =
          get_length(4, music_dirpath_len, dir_buf_len, slash_len, slash_len);

      if (path_ttl_length > 4096) {
        fprintf(stderr, "Path length exceeds max!\n");
        free(dir_buf);
        free(fpaths);
        search_broken = true;
        break;
      }

      char *path_buf = (char *)malloc(sizeof(char) * (path_ttl_length + 1));
      if (!path_buf) {
        fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
        search_broken = true;
        break;
      }

      snprintf(path_buf, path_ttl_length + 1, "%s/%s", path, dir_buf);

      fpaths[*count].path = path_buf;
      fpaths[*count].path_length = path_ttl_length;
      fpaths[*count].name = dir_buf;
      fpaths[*count].name_length = file_name_length;

      (*count)++;
    }
  }

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
    closedir(dir);
    return NULL;
  }

  closedir(dir);
  return fpaths;
}
#endif
