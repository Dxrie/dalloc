#include "dalloc.h"
#include "internal.h"
#include <stddef.h>
#include <sys/mman.h>

#define ARENA_SIZE (1024 * 1024)

static arena_t global_arena = {0};
static arena_t *arena_list_head = NULL;

static void arena_init() {
  if (global_arena.memory) {
    return;
  }

  global_arena.memory = mmap(NULL, ARENA_SIZE, PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (global_arena.memory == MAP_FAILED) {
    return;
  }

  global_arena.size = ARENA_SIZE;
  global_arena.next = NULL;
  arena_list_head = &global_arena;

  heapchunk_t *free_chunk = (heapchunk_t *)global_arena.memory;
  free_chunk->size = ARENA_SIZE - sizeof(heapchunk_t);
  free_chunk->flag = 0;
  free_chunk->next = NULL;
  free_chunk->prev = NULL;

  global_arena.head = free_chunk;
}

static arena_t *allocate_new_arena() {
  void *memory = mmap(NULL, ARENA_SIZE, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (memory == MAP_FAILED) {
    return NULL;
  }

  arena_t *new_arena = (arena_t *)memory;

  new_arena->memory = (char *)memory + sizeof(arena_t);
  new_arena->size = ARENA_SIZE - sizeof(arena_t);
  new_arena->next = NULL;

  heapchunk_t *free_chunk = (heapchunk_t *)new_arena->memory;
  free_chunk->size = new_arena->size - sizeof(heapchunk_t);
  free_chunk->flag = 0;
  free_chunk->next = NULL;

  new_arena->head = free_chunk;

  arena_t *current = arena_list_head;
  while (current->next) {
    current = current->next;
  }
  current->next = new_arena;

  return new_arena;
}

void *dalloc_malloc(size_t size) {
  if (size == 0) {
    return NULL;
  }

  arena_init();

  size = (size + 7) & ~7; // alignment

  if (size > (ARENA_SIZE - sizeof(arena_t) - sizeof(heapchunk_t))) {
    size_t total = size + sizeof(heapchunk_t);

    void *memory = mmap(NULL, total, PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if (memory == MAP_FAILED) {
      return NULL;
    }

    heapchunk_t *isolated_chunk = (heapchunk_t *)memory;
    isolated_chunk->size = size;
    isolated_chunk->flag = 2;
    isolated_chunk->next = NULL;
    isolated_chunk->prev = NULL;

    return (void *)(isolated_chunk + 1);
  }

  arena_t *current_arena = arena_list_head;

  while (current_arena) {
    heapchunk_t *current_chunk = current_arena->head;

    while (current_chunk) {
      if (current_chunk->flag == 0 && current_chunk->size >= size) {
        if (current_chunk->size >= size + sizeof(heapchunk_t) + 8) {
          heapchunk_t *next_free_chunk =
              (heapchunk_t *)((char *)current_chunk + sizeof(heapchunk_t) +
                              size);

          next_free_chunk->size =
              current_chunk->size - size - sizeof(heapchunk_t);
          next_free_chunk->flag = 0;
          next_free_chunk->next = current_chunk->next;
          next_free_chunk->prev = current_chunk;

          if (current_chunk->next) {
            current_chunk->next->prev = next_free_chunk;
          }

          current_chunk->size = size;
          current_chunk->next = next_free_chunk;
        }

        current_chunk->flag = 1;
        return (void *)(current_chunk + 1);
      }
      current_chunk = current_chunk->next;
    }
    current_arena = current_arena->next;
  }

  // out of space reallocate new chunk
  arena_t *new_arena = allocate_new_arena();

  if (!new_arena) {
    return NULL;
  }

  heapchunk_t *new_chunk = new_arena->head;

  if (new_chunk->size >= size + sizeof(heapchunk_t) + 8) {
    heapchunk_t *next_free_chunk =
        (heapchunk_t *)((char *)new_chunk + sizeof(heapchunk_t) + size);
    next_free_chunk->size = new_chunk->size - size - sizeof(heapchunk_t);
    next_free_chunk->flag = 0;
    next_free_chunk->next = NULL;
    next_free_chunk->prev = new_chunk;

    new_chunk->next = next_free_chunk;
  }

  new_chunk->size = size;
  new_chunk->flag = 1;
  return (void *)(new_chunk + 1);
}

void dalloc_free(void *ptr) {
  if (!ptr) {
    return;
  }

  heapchunk_t *chunk = (heapchunk_t *)ptr - 1;

  if (chunk->flag == 2) {
    size_t total = sizeof(heapchunk_t) + chunk->size;
    munmap(chunk, total);
    return;
  }

  if (chunk->flag == 1) {
    chunk->flag = 0;

    while (chunk->next && chunk->next->flag == 0) {
      heapchunk_t *next_chunk = chunk->next;
      chunk->size += next_chunk->size + sizeof(heapchunk_t);
      chunk->next = next_chunk->next;
    }
  }
}
