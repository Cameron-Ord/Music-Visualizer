#ifndef EVENTS_H
#define EVENTS_H

#include <stddef.h>

int node_index(const char *direction, int node_index, int max);
size_t nav_down(const size_t cursor, const size_t max);
size_t auto_nav_down(const size_t cursor, const size_t max);
size_t nav_up(const size_t cursor, const size_t max);
#endif
