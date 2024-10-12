#include "filesystem.h"
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

char **win_find_directories(size_t *count) {

  WIN32_FIND_DATA find_file_data;
  HANDLE h_find;

  char *home = getenv("USERPROFILE");
  char *music_dir = "Music\\MVSource\\*";

  char search_path[260];

  snprintf(search_path, sizeof(search_path), "%s%s%s", home, "\\", music_dir);

  fprintf(stdout, "SEARCH PATH -> %s\n", search_path);

  h_find = FindFirstFile(search_path, &find_file_data);
  if (h_find == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "Could not read filesystem! -> %s\n", search_path);
    return NULL;
  }
  size_t default_size = 6;

  char **dir_buffer = malloc(sizeof(char *) * default_size);
  if (!dir_buffer) {
    fprintf(stderr, "Could not allocate pointer! -> %s\n", strerror(errno));
  }
  memset(dir_buffer, '0', sizeof(char) * default_size);

  while (FindNextFile(h_find, &find_file_data) != 0) {
    if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      if (strcmp(find_file_data.cFileName, "..") != 0 &&
          strcmp(find_file_data.cFileName, ".") != 0) {
        if (*count >= default_size) {
          dir_buffer = realloc(dir_buffer, sizeof(char *) * *count);
          if (!dir_buffer) {
            fprintf(stderr, "Could not allocate pointer! -> %s\n",
                    strerror(errno));
            return NULL;
          }
        }

        size_t file_name_length = strlen(find_file_data.cFileName);
        dir_buffer[*count] = malloc((file_name_length + 1) * sizeof(char));
        if (!dir_buffer[*count]) {
          fprintf(stderr, "Could not allocate pointer! -> %s\n",
                  strerror(errno));
          return NULL;
        }

        memset(dir_buffer[*count], '0', sizeof(char) * (file_name_length + 1));
        strcpy_s(dir_buffer[*count], sizeof(char) * (file_name_length + 1),
                 find_file_data.cFileName);

        *count += 1;
      }
    }
  }

  FindClose(h_find);

  return dir_buffer;
}

char *unix_find_directories() {}