#ifndef TABLE_HPP
#define TABLE_HPP
#include "fontdefs.hpp"

#define DEFAULT_TABLE_SIZE 1024

struct Node;

typedef struct Node Node;

struct Node {
  int key;
  Text *text_data;
  Node *next;
};

Text *search_key(Node **table, int key, size_t length);
bool insert_node(Node **hash_table, int key, Text *value, int length);
int hash_fn(int key, int length);
#endif
