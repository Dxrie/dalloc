#include "dalloc.h"
#include <stdio.h>
#include <assert.h>

int main() {
    int *a = dalloc_malloc(sizeof(int));
    assert(a != NULL);

    *a = 123;
    printf("value = %d\n", *a);

    dalloc_free(a);

    printf("free OK\n");
    return 0;
}
