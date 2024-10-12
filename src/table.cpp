#include "../include/table.hpp"

Text *search_key(Node **table, int key, int length) {
  size_t index = (size_t)hash_fn(key, length);
  Node *current = table[index];

  while (current != NULL) {
    if (current->key == key) {
      return current->text_data;
    }

    current = current->next;
  }

  return NULL;
}
int hash_fn(int key, int length) { return key % length; }
bool insert_node(Node **table, int key, Text *value, int length) {
  Node *node = (Node *)malloc(sizeof(Node));
  if (!node) {
    std::cerr << "Could not allocate pointer! -> " << strerror(errno)
              << std::endl;
    return false;
  }

  node->text_data = value;
  int index = hash_fn(key, length);

  node->next = table[index];
  table[index] = node;

  return true;
}
