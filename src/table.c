#include "../inc/table.h"
#include "../inc/main.h"
#include "../inc/sys.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

int current_index = 0;
int last_index = 0;

void set_current_index(const int i) { current_index = i; }
int get_current_index(void) { return current_index; }

void set_last_index(const int i) { last_index = i; }
int get_last_index(void) { return last_index; }

size_t hash(size_t i) { return i % MAX_NODES; }

int create_node(Table *t, size_t i) {
  Node *n = malloc(sizeof(Node));
  if (!n) {
    errno_string("malloc()", strerror(errno));
    return 0;
  }

  n->key = hash(i);
  n->paths = NULL;
  n->next = t->node_buffer[n->key];
  t->node_buffer[n->key] = n;

  return 1;
}

Node *search_table(Table *t, size_t i) {
  Node *n = t->node_buffer[hash(i)];
  while (n != NULL) {
    if (n->key == hash(i)) {
      return n;
    }
    n = n->next;
  }
  return NULL;
}

int table_set_paths(Table *t, Paths *paths) {
  Node *n = search_table(t, current_index);
  if (!n) {
    return 0;
  }

  if (paths->size <= 0) {
    free(paths);
    return 0;
  }

  if (n->paths) {
    for (size_t i = 0; i < n->paths->size; i++) {
      free_entry(&n->paths[i]);
    }
    free(n->paths);
  }

  if (!paths) {
    n->paths = NULL;
    return 0;
  } else if (paths) {
    switch (paths_check_sanity(paths)) {
    case 0: {
      n->paths = NULL;
      for (size_t i = 0; i < paths->size; i++) {
        free_entry(&paths[i]);
      }
      free(paths);
    } break;
    case 1: {
      n->paths = paths;
      return 1;
    } break;
    }
  }
  return 0;
}
