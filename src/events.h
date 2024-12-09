#ifndef EVENTS_H
#define EVENTS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int node_index(const char *direction, int node_index, int max);
size_t nav_down(const size_t cursor, const size_t max);
size_t auto_nav_down(const size_t cursor, const size_t max);
size_t nav_up(const size_t cursor, const size_t max);
size_t mv_start_pos(const int cursor, const size_t start, const size_t max);
#endif
