#include "../inc/music_visualizer.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef __linux__
#include <linux/limits.h>
#endif

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <limits.h>
#endif

char*
get_slash() {
#ifdef __linux__
  return "/";
#endif

#ifdef _WIN32
  return "\\";
#endif
  return "";
}

char*
get_platform_env() {
#ifdef __linux__
  return "HOME";
#endif

#ifdef _WIN32
  return "USERPROFILE";
#endif

  return "";
}

int
make_directory(char* path, mode_t mode) {
#ifdef __linux__
  if (mkdir(path, mode) == 0) {
    return 0;
  }
  return -1;
#endif

#ifdef _WIN32
  if (mkdir(path) == 0) {
    return 0;
  }
#endif
  return -1;
}

int
chmod_dir(char* path, mode_t mode) {
  if (chmod(path, mode) == 0) {
    return 0;
  };
  return -1;
}

void
setup_dirs() {

  char* home = getenv(get_platform_env());
  if (home == NULL) {
    PRINT_STR_ERR(stderr, "Error getting home ENV", strerror(errno));
    return;
  }

  char   path[PATH_MAX];
  mode_t mode = S_IRWXU;

  snprintf(path, PATH_MAX * sizeof(char), "%s%sMusic%sfftmlogs%s", home, get_slash(), get_slash(),
           get_slash());

  if (make_directory(path, mode) == 0) {
    if (chmod_dir(path, mode) != 0) {
      perror("chmod");
      return;
    }
  } else {
    if (errno != EEXIST) {
      PRINT_STR_ERR(stderr, "Failed to create DIR", strerror(errno));
      return;
    }
  }

  snprintf(path, PATH_MAX * sizeof(char), "%s%sMusic%sfftmplayer%s", home, get_slash(), get_slash(),
           get_slash());

  if (make_directory(path, mode) == 0) {
    if (chmod_dir(path, mode) != 0) {
      perror("chmod");
      return;
    }
  } else {
    if (errno != EEXIST) {
      PRINT_STR_ERR(stderr, "Failed to create DIR", strerror(errno));
      perror("mkdir");
      return;
    }
  }

  snprintf(path, PATH_MAX * sizeof(char), "%s%sMusic%sfftmlogs%s%s", home, get_slash(), get_slash(),
           get_slash(), "log.txt");

  if (freopen(path, "w", stdout) == NULL) {
    PRINT_STR_ERR(stderr, "Error redirecting STDOUT", strerror(errno));
    return;
  }

  snprintf(path, PATH_MAX * sizeof(char), "%s%sMusic%sfftmlogs%s%s", home, get_slash(), get_slash(),
           get_slash(), "errlog.txt");

  if (freopen(path, "w", stderr) == NULL) {
    PRINT_STR_ERR(stdout, "Could not redirect STDERR!", strerror(errno));
    return;
  }
} /*setup_dirs*/

int
fetch_dirs(DirState* DS) {

  char***        sub_dirs = &DS->directories;
  DIR*           directory;
  struct dirent* entry;
  int            dir_count = 0;
  char*          home      = getenv(get_platform_env());

  if (home == NULL) {
    PRINT_STR_ERR(stderr, "Failed to get home ENV", strerror(errno));
    return -1;
  }

  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s%sMusic%sfftmplayer%s", home, get_slash(), get_slash(), get_slash());

  directory = opendir(path);
  if (directory == NULL) {
    PRINT_STR_ERR(stderr, "Could not open directory", strerror(errno));
    return -1;
  }

  while ((entry = readdir(directory)) != NULL) {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {

      char* duped_name = malloc(PATH_MAX * sizeof(char*));
      if (duped_name == NULL) {
        closedir(directory);
        return -1;
      }

      const size_t size_offset = strlen(entry->d_name);

      char   full_path[PATH_MAX + size_offset];
      size_t required_len = strlen(path) + strlen(entry->d_name);

      if (required_len < PATH_MAX + size_offset) {
        snprintf(full_path, sizeof(char) * (PATH_MAX + size_offset), "%s%s", path, entry->d_name);
      } else {
        fprintf(stderr, "Exceeded maximum path length!\n");
        return -1;
      }

      struct stat statbuf;

      if (stat(full_path, &statbuf) != 0) {
        fprintf(stderr, "stat failed on %s: %s\n", full_path, strerror(errno));
        break;
      }

      if (S_ISDIR(statbuf.st_mode)) {
        int has_ws = 0;
        strcpy(duped_name, entry->d_name);
        int i, j;

        for (i = 0, j = 0; duped_name[i] != '\0'; i++) {
          if (duped_name[i] == ' ' || duped_name[i] == ',') {
            has_ws        = 1;
            duped_name[i] = '_';
          }

          if (duped_name[i] != ' ') {
            duped_name[j++] = duped_name[i];
          }
        }
        duped_name[j] = '\0';

        if (has_ws) {
          char input_path[PATH_MAX];
          char output_path[PATH_MAX];

          /*It really do be the easiest way*/

          snprintf(input_path, PATH_MAX, "%s%sMusic%sfftmplayer%s%s", home, get_slash(), get_slash(),
                   get_slash(), entry->d_name);
          snprintf(output_path, PATH_MAX, "%s%sMusic%sfftmplayer%s%s", home, get_slash(), get_slash(),
                   get_slash(), duped_name);

          rename(input_path, output_path);
        }

        *sub_dirs = realloc(*sub_dirs, (dir_count + 1) * sizeof(char*));
        if (*sub_dirs == NULL) {
          closedir(directory);
          return 0;
        }

        (*sub_dirs)[dir_count] = duped_name;
        dir_count++;
      }
    }
  }

  closedir(directory);
  return dir_count;
} /*read_music_dir*/

int
fetch_files(FileState* FS) {

  char***        files        = &FS->files;
  char*          selected_dir = FS->selected_dir;
  DIR*           directory;
  struct dirent* entry;
  int            file_count = 0;

  char* home = getenv(get_platform_env());
  if (home == NULL) {
    PRINT_STR_ERR(stderr, "Failed to get home ENV", strerror(errno));
    return -1;
  }
  char path[PATH_MAX];

  /*I hate it*/

  snprintf(path, PATH_MAX, "%s%sMusic%sfftmplayer%s%s%s", home, get_slash(), get_slash(), get_slash(),
           selected_dir, get_slash());

  directory = opendir(path);
  if (directory == NULL) {
    PRINT_STR_ERR(stderr, "Could not open directory", strerror(errno));
    return -1;
  }

  while ((entry = readdir(directory)) != NULL) {

    char* duped_name = malloc(PATH_MAX * sizeof(char*));
    if (duped_name == NULL) {
      closedir(directory);
      return -1;
    }

    const size_t size_offset = strlen(entry->d_name);

    char   full_path[PATH_MAX + size_offset];
    size_t required_len = strlen(path) + strlen(entry->d_name);

    if (required_len < PATH_MAX + size_offset) {
      snprintf(full_path, sizeof(char) * (PATH_MAX + size_offset), "%s%s", path, entry->d_name);
    } else {
      fprintf(stderr, "Exceeded maximum path length!\n");
      return -1;
    }

    struct stat statbuf;

    if (stat(full_path, &statbuf) != 0) {
      fprintf(stderr, "stat failed on %s: %s\n", full_path, strerror(errno));
      break;
    }

    if (S_ISREG(statbuf.st_mode)) {

      int has_ws = 0;
      strcpy(duped_name, entry->d_name);
      int i, j;

      for (i = 0, j = 0; duped_name[i] != '\0'; i++) {
        if (duped_name[i] == ' ' || duped_name[i] == ',') {
          has_ws        = 1;
          duped_name[i] = '_';
        }

        if (duped_name[i] != ' ') {
          duped_name[j++] = duped_name[i];
        }
      }
      duped_name[j] = '\0';

      if (has_ws) {
        char input_path[PATH_MAX];
        char output_path[PATH_MAX];

        /*My eyes bleed*/

        snprintf(input_path, PATH_MAX, "%s%sMusic%sfftmplayer%s%s%s%s", home, get_slash(), get_slash(),
                 get_slash(), selected_dir, get_slash(), entry->d_name);
        snprintf(output_path, PATH_MAX, "%s%sMusic%sfftmplayer%s%s%s%s", home, get_slash(), get_slash(),
                 get_slash(), selected_dir, get_slash(), duped_name);

        rename(input_path, output_path);
      }

      *files = realloc(*files, (file_count + 1) * sizeof(char*));
      if (*files == NULL) {
        closedir(directory);
        return 0;
      }

      (*files)[file_count] = duped_name;
      file_count++;
    }
  }

  closedir(directory);
  return file_count;

} /*load_dir_songlist*/

void
clear_dirs(FileContext* FCPtr) {
  i8  dirs_exist = FCPtr->dir_state->dirs_exist;
  int dir_count  = FCPtr->dir_state->dir_count;

  if (dir_count > 0 && dirs_exist) {
    for (int i = 0; i < dir_count; i++) {
      free_ptr(FCPtr->dir_state->directories[i]);
    }
  }

  if (dirs_exist) {
    free_ptr(FCPtr->dir_state->directories);
  }
}

void
clear_files(FileContext* FCPtr) {

  i8  files_exist = FCPtr->file_state->files_exist;
  int file_count  = FCPtr->file_state->file_count;

  if (file_count > 0 && files_exist) {
    for (int i = 0; i < file_count; i++) {
      free_ptr(FCPtr->file_state->files[i]);
    }
  }

  if (files_exist) {
    free_ptr(FCPtr->file_state->files);
  }
}
