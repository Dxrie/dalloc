#include "dalloc.h"
#include "internal.h"
#include <stddef.h>
#include <sys/mman.h>

static heaparena_t g_arena = {0};

void *dalloc_malloc(size_t size) {
  if (size == 0) {
    return NULL;
  }

  size = (size + 7) & ~7; // alignment
  heapchunk_t *current = g_arena.head;

  while (current) {
    if (current->flag == 0 && current->size >= size) {
      current->flag = 1;
      return (void *)(current + 1); // return the user data chunk
    }
  }
}

void dalloc_free(void *ptr) { (void)ptr; }

void *dalloc_realloc(void *ptr, size_t size) {
  (void)ptr;
  (void)size;
  return NULL;
}
