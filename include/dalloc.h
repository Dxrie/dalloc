#ifndef DALLOC_H
#define DALLOC_H

#include <stddef.h>

void *dalloc_malloc(size_t size);
void dalloc_free(void *ptr);
void *dalloc_realloc(void *ptr, size_t size);

#endif
