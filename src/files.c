#include "music_visualizer.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void setup_dirs() {

#ifdef __LINUX__
  char* home = getenv("HOME");
  if (home == NULL) {
    PRINT_STR_ERR(stderr, "Error getting home ENV", strerror(errno));
    return;
  } else {
    char   path[PATH_MAX];
    mode_t mode = S_IRWXU;

    snprintf(path, PATH_MAX, "%s/Music/fftmlogs/", home);
    if (mkdir(path, mode) == 0) {
      printf("Log DIR created\n");
      if (chmod(path, mode) != 0) {
        perror("chmod");
        return;
      }
    } else {
      if (errno != EEXIST) {
        PRINT_STR_ERR(stderr, "Failed to create DIR", strerror(errno));
        return;
      }
    }

    snprintf(path, PATH_MAX, "%s/Music/fftmplayer/", home);
    if (mkdir(path, mode) == 0) {
      printf("Song DIR created\n");
      if (chmod(path, mode) != 0) {
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

    int fd;
    snprintf(path, PATH_MAX, "%s/Music/fftmlogs/%s", home, "log.txt");
    fd = open(path, O_RDWR | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
      PRINT_STR_ERR(stderr, "Could not open file", strerror(errno));
      return;
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);

    snprintf(path, PATH_MAX, "%s/Music/fftmlogs/%s", home, "errlog.txt");
    fd = open(path, O_RDWR | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
      PRINT_STR_ERR(stderr, "Could not open file", strerror(errno));
      return;
    }
    dup2(fd, STDERR_FILENO);
    close(fd);
  }
#endif
} /*setup_dirs*/

int fetch_dirs(DirState* DS) {
#ifdef __LINUX__

  char***        sub_dirs = &DS->directories;
  DIR*           directory;
  struct dirent* entry;
  int            dir_count = 0;

  char* home = getenv("HOME");
  if (home == NULL) {
    PRINT_STR_ERR(stderr, "Failed to get home ENV", strerror(errno));
    return -1;
  }

  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/Music/fftmplayer/", home);

  directory = opendir(path);
  if (directory == NULL) {
    PRINT_STR_ERR(stderr, "Could not open directory", strerror(errno));
    return -1;
  }

  while ((entry = readdir(directory)) != NULL) {
    if (entry->d_type == DT_DIR) {
      if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
        char* duped_name = malloc(PATH_MAX * sizeof(char*));
        if (duped_name == NULL) {
          closedir(directory);
          return -1;
        }

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
          snprintf(input_path, PATH_MAX, "%s/Music/fftmplayer/%s", home, entry->d_name);
          snprintf(output_path, PATH_MAX, "%s/Music/fftmplayer/%s", home, duped_name);
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
#endif
  return -1;
} /*read_music_dir*/

int fetch_files(FileState* FS) {
#ifdef __LINUX__

  char***        files        = &FS->files;
  char*          selected_dir = FS->selected_dir;
  DIR*           directory;
  struct dirent* entry;
  int            file_count = 0;

  char* home = getenv("HOME");
  if (home == NULL) {
    PRINT_STR_ERR(stderr, "Failed to get home ENV", strerror(errno));
    return -1;
  }
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/Music/fftmplayer/%s/", home, selected_dir);

  directory = opendir(path);
  if (directory == NULL) {
    PRINT_STR_ERR(stderr, "Could not open directory", strerror(errno));
    return -1;
  }

  while ((entry = readdir(directory)) != NULL) {
    if (entry->d_type == DT_REG) {
      char* duped_name = malloc(PATH_MAX * sizeof(char*));
      if (duped_name == NULL) {
        closedir(directory);
        return -1;
      }

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
        snprintf(input_path, PATH_MAX, "%s/Music/fftmplayer/%s/%s", home, selected_dir,
                 entry->d_name);
        snprintf(output_path, PATH_MAX, "%s/Music/fftmplayer/%s/%s", home, selected_dir,
                 duped_name);
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
#endif

  return -1;
} /*load_dir_songlist*/
