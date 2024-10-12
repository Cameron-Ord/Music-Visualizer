#ifndef TABLE_HPP
#define TABLE_HPP
#include "fontdefs.hpp"

#define DEFAULT_TABLE_SIZE 1024

struct Node;

typedef struct Node Node;

struct Node {
  size_t key;
  Text *text_data;
  Node *next;
};

Text *search_key(Node **table, size_t key, size_t length);
bool insert_node(Node **hash_table, size_t key, Text *value, size_t length);
size_t hash_fn(size_t key, size_t length);
#endif
