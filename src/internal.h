#ifndef INTERNAL_H
#define INTERNAL_H

#include <stddef.h>
#include <stdint.h>

typedef struct heapchunk_t heapchunk_t;

struct heapchunk_t {
  size_t size;
  uint8_t flag; // 0 for free 1 for in use
  heapchunk_t *next;
};

typedef struct heaparena_t {
  heapchunk_t *head;
} heaparena_t;

#endif
