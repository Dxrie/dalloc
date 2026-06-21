# Dalloc - Dxrie's (Trash) Memory Allocator

A custom, arena-based memory allocator written in C. It serves as a fun and educational project to understand memory management, heap internals, and dynamic memory allocation. Is it faster than malloc? Definitely not. Is it safer? Definitely not. But it implements the fundamental concepts of dynamic memory management.

---

## Features

### 1. Arena-based Memory Management
* **Virtual Memory Mapping:** Uses `mmap` with `MAP_PRIVATE | MAP_ANONYMOUS` to request page-aligned memory chunks directly from the operating system, bypassing standard heap allocation APIs.
* **Singly Linked Arenas:** Arenas (`arena_t`) are managed in a singly linked list. When the current arenas run out of space, `dalloc` dynamically requests and appends a new 1MB arena.

### 2. Chunk-based Allocation & Splitting
* **Chunk Headers:** Memory inside each arena is split into chunks, prefixed with a metadata header (`heapchunk_t`) containing the chunk's size, status flags (0: Free, 1: In Use, 2: Isolated Chunk), and a pointer to the next chunk.
* **Chunk Splitting:** To combat internal fragmentation, if a found free chunk is larger than requested plus the overhead of a new chunk header (and at least an 8-byte payload), the chunk is split. The requested portion is allocated, and the remaining space is converted into a new free chunk in the list.

### 3. Allocation Optimization & Constraints
* **8-Byte Alignment:** All requested sizes are aligned to 8-byte boundaries using `(size + 7) & ~7` to ensure compatibility and efficiency on modern architectures.
* **Large Allocation Bypass (Isolated Chunks):** Requests exceeding the standard arena size are allocated as "isolated chunks" directly using dedicated, sized `mmap` calls.

---

## API Status

The following functions are declared in `include/dalloc.h`:

| Function | Description | Status |
|---|---|---|
| `void *dalloc_malloc(size_t size)` | Allocates `size` bytes of memory and returns a pointer to it. | **Implemented** |
| `void dalloc_free(void *ptr)` | Frees the allocated memory pointed to by `ptr`. | Implemented |
| `void *dalloc_realloc(void *ptr, size_t size)` | Changes the size of the memory block pointed to by `ptr`. | *Declared / Pending Implementation* |

---

## Future Roadmap / Planned Features
* [ ] Implement `dalloc_realloc` for dynamic resizing of allocated blocks.
