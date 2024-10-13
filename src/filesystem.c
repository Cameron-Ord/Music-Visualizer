#include "filesystem.h"
#include "utils.h"
#include <stdbool.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __LINUX__
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#endif

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
      get_length(strlen("\\"), strlen(music_dir), strlen(home));
  if (total_length > MAX_PATH) {
    fprintf(
        stderr,
        "Total path length exceeds allocated path buffer! -> RETURN NULL\n");
    return NULL;
  }

  snprintf(search_path, sizeof(search_path), "%s\\%s", home, music_dir);

  fprintf(stdout, "SEARCH PATH -> %s\n", search_path);

  Paths *paths = calloc(1, sizeof(Paths));
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

  size_t default_size = 6;
  char **dir_buffer = (char **)calloc(default_size, sizeof(char *));
  if (!dir_buffer) {
    FindClose(h_find);
    free(home);
    fprintf(stderr, "Could not allocate pointer buffer! -> %s\n",
            strerror(errno));
    return NULL;
  }

  char **path_buffer = (char **)calloc(default_size, sizeof(char *));
  if (!dir_buffer) {
    FindClose(h_find);
    free(home);
    fprintf(stderr, "Could not allocate pointer buffer! -> %s\n",
            strerror(errno));
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
          char **temp_buffer =
              (char **)realloc(dir_buffer, sizeof(char *) * new_size);
          if (!temp_buffer) {
            fprintf(stderr, "Could not reallocate pointer buffer! -> %s\n",
                    strerror(errno));
            find_file_broken = true;
            break;
          }

          char **tmp_path_buffer =
              (char **)realloc(path_buffer, sizeof(char *) * new_size);
          if (!tmp_path_buffer) {
            fprintf(stderr, "Could not reallocate pointer buffer! -> %s\n",
                    strerror(errno));
            find_file_broken = true;
            break;
          }

          path_buffer = tmp_path_buffer;
          dir_buffer = temp_buffer;
          default_size = new_size;
        }

        size_t file_name_length = strlen(find_file_data.cFileName);
        dir_buffer[*count] =
            (char *)malloc((file_name_length + 1) * sizeof(char));
        if (!dir_buffer[*count]) {
          fprintf(stderr,
                  "Could not allocate memory for directory name! -> %s\n",
                  strerror(errno));
          find_file_broken = true;
          break;
        }

        strcpy_s(dir_buffer[*count], file_name_length + 1,
                 find_file_data.cFileName);

        size_t path_ttl_length =
            get_length(4, strlen(music_dir_no_wc), strlen(home), strlen("\\"),
                       strlen(dir_buffer[*count]));
        if (path_ttl_length < MAX_PATH) {
          path_buffer[*count] =
              (char *)malloc(sizeof(char) * (path_ttl_length + 1));
          if (!path_buffer[*count]) {
            fprintf(stderr, "Could not allocate pointer! -> %s\n",
                    strerror(errno));
            find_file_broken = true;
            break;
          }

          snprintf(path_buffer[*count], path_ttl_length + 1, "%s\\%s%s", home,
                   music_dir_no_wc, dir_buffer[*count]);
        }

        printf("\n");
        printf("Added directory -> %s\n", dir_buffer[*count]);
        printf("Added path -> %s\n", path_buffer[*count]);
        printf("\n");

        (*count)++;
      }
    }
  } while (FindNextFile(h_find, &find_file_data) != 0);

  //Tested and works as far as I know.
  if (find_file_broken) {
    for (size_t i = 0; i < *count; i++) {
      if (dir_buffer[i]) {
        free(dir_buffer[i]);
        dir_buffer[i] = NULL;
      }

      if (path_buffer[i]) {
        free(path_buffer[i]);
        path_buffer[i] = NULL;
      }
    }

    free_ptrs(4, home, dir_buffer, path_buffer, paths);
    FindClose(h_find);
    return NULL;
  }

  paths->paths = path_buffer;
  paths->names = dir_buffer;

  FindClose(h_find);
  free(home);
  return paths;
}
#endif
#ifdef __LINUX__
char **unix_find_directories(size_t *count, FileSys *files) {
  char *home = getenv("HOME");
  char *music_dir = "Music/MVSource";

  char search_path[260];
  snprintf(search_path, sizeof(search_path), "%s%s%s", home, "/", music_dir);

  DIR *dir = opendir(search_path);

  size_t default_size = 6;

  char **dir_buffer = malloc(sizeof(char *) * default_size);
  if (!dir_buffer) {
    fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
    return NULL;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".") != 0) {
      if (*count >= default_size) {
        dir_buffer = realloc(dir_buffer, sizeof(char *) * (*count));
        if (!dir_buffer) {
          fprintf(stderr, "Could not reallocate pointer! -> %s\n",
                  strerror(errno));
          return NULL;
        }
      }

      size_t file_name_length = strlen(entry->d_name);
      dir_buffer[*count] = malloc((file_name_length + 1) * sizeof(char));
      if (!dir_buffer[*count]) {
        fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
        return NULL;
      }

      strcpy(dir_buffer[*count], entry->d_name);

      (*count)++;
    }
  }

  closedir(dir);
  return dir_buffer;
}
#endif
