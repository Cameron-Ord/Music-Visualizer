#include "table.h"
#include "filesystem.h"
#include "font.h"
#include "utils.h"
#include <errno.h>

size_t hash(size_t i) { return i % MAX_NODES; }

int create_node(Table *t, size_t i) {
  Node *n = malloc(sizeof(Node));
  if (!n) {
    ERRNO_CALLBACK("malloc() failed!", strerror(errno));
    return 0;
  }

  n->key = hash(i);
  n->pbuf = NULL;
  n->tbuf = NULL;
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

void table_set_text(Table *t, size_t i, TextBuffer *tbuf) {
  Node *n = search_table(t, i);
  if (!n) {
    return;
  }

  if (!tbuf) {
    n->tbuf = NULL;
    // We do the check on the validity, then free or assign memory as per it's
    // value. Easier to manage this way.
  } else if (tbuf && !tbuf->info.is_valid) {
    // returns null
    n->tbuf = free_text_buffer(tbuf, &tbuf->info.size);
  } else {
    n->tbuf = tbuf;
  }
}

void table_set_paths(Table *t, size_t i, Paths *pbuf) {
  Node *n = search_table(t, i);
  if (!n) {
    return;
  }

  if (!pbuf) {
    n->pbuf = NULL;
    // We do the check on the validity, then free or assign memory as per it's
    // value. Easier to manage this way.
  } else if (pbuf && !pbuf->is_valid) {
    // returns null
    n->pbuf = free_paths(pbuf, &pbuf->size);
  } else {
    n->pbuf = pbuf;
  }
}
