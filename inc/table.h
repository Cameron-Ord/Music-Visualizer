#ifndef TABLE_H
#define TABLE_H

#define MAX_NODES 16
#define MODES 2

#include <stddef.h>

struct Paths;
struct TextBuffer;

typedef struct Paths Paths;
typedef struct TextBuffer TextBuffer;

typedef struct Node Node;
typedef struct Table Table;

struct Node {
  size_t key;
  Paths *pbuf;
  TextBuffer *tbuf;
  Node *next;
};

struct Table {
  Node *node_buffer[MAX_NODES];
};

Node *search_table(Table *t, size_t i);
int create_node(Table *t, size_t i);
size_t hash(size_t i);
void table_set_paths(Table *t, size_t i, Paths *pbuf);
void table_set_text(Table *t, size_t i, TextBuffer *tbuf);

#endif
