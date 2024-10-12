#include "filesystem.h"
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

char **win_find_directories(size_t *count) {
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
    char search_path[260];
    snprintf(search_path, sizeof(search_path), "%s\\%s", home, music_dir);

    fprintf(stdout, "SEARCH PATH -> %s\n", search_path);

    h_find = FindFirstFile(search_path, &find_file_data);
    if (h_find == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Could not read filesystem! -> %s\n", search_path);
        free(home); 
        return NULL;
    }

    
    size_t default_size = 6;
    char **dir_buffer = (char **)malloc(sizeof(char *) * default_size);
    if (!dir_buffer) {
        FindClose(h_find);
        free(home);
        fprintf(stderr, "Could not allocate pointer buffer! -> %s\n", strerror(errno));
        return NULL;
    }

    *count = 0;  // Initialize count

    do {
        if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(find_file_data.cFileName, "..") != 0 &&
                strcmp(find_file_data.cFileName, ".") != 0) {
                // Check if we need to resize the buffer
                if (*count >= default_size) {
                    size_t new_size = default_size * 2;
                    char **temp_buffer = (char **)realloc(dir_buffer, sizeof(char *) * new_size);
                    if (!temp_buffer) {
                        fprintf(stderr, "Could not reallocate pointer buffer! -> %s\n", strerror(errno));
                        for (size_t i = 0; i < *count; ++i) {
                            free(dir_buffer[i]);
                        }
                        free(dir_buffer);
                        FindClose(h_find);
                        free(home);
                        return NULL;
                    }

                    dir_buffer = temp_buffer;
                    default_size = new_size;
                }

                size_t file_name_length = strlen(find_file_data.cFileName);
                dir_buffer[*count] = (char *)malloc((file_name_length + 1) * sizeof(char));
                if (!dir_buffer[*count]) {
                    fprintf(stderr, "Could not allocate memory for directory name! -> %s\n", strerror(errno));
                    for (size_t i = 0; i < *count; ++i) {
                        free(dir_buffer[i]);
                    }
                    free(dir_buffer);
                    FindClose(h_find);
                    free(home);
                    return NULL;
                }

                strcpy_s(dir_buffer[*count], file_name_length + 1, find_file_data.cFileName);
                (*count)++;
            }
        }
    } while (FindNextFile(h_find, &find_file_data) != 0);

    FindClose(h_find);
    free(home);  
    return dir_buffer;  
}
#endif
#ifdef __LINUX__
char **unix_find_directories(size_t *count) {
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
