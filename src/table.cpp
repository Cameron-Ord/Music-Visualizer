#include "../include/table.hpp"

Text *search_key(Node **table, size_t key, size_t length) {
  size_t index = hash_fn(key, length);
  Node *current = table[index];

  while (current != NULL) {
    if (current->key == key) {
      return current->text_data;
    }

    current = current->next;
  }

  return NULL;
}
size_t hash_fn(size_t key, size_t length) { return key % length; }
bool insert_node(Node **table, size_t key, Text *value, size_t length) {
  Node *node = (Node *)malloc(sizeof(Node));
  if (!node) {
    std::cerr << "Could not allocate pointer! -> " << strerror(errno)
              << std::endl;
    return false;
  }

  size_t index = hash_fn(key, length);
  node->key = key;
  node->text_data = value;

  node->next = table[index];
  table[index] = node;

  return true;
}
