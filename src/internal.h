#ifndef INTERNAL_H
#define INTERNAL_H

#include <stddef.h>
#include <stdint.h>

typedef struct heapchunk_t heapchunk_t;
typedef struct arena_t arena_t;

struct heapchunk_t {
  size_t size;
  uint8_t flag; // 0: free, 1: in use, 2: isolated chunk
  heapchunk_t *next;
};

struct arena_t {
  void *memory;
  size_t size;
  heapchunk_t *head;
  arena_t *next;
};

#endif
