#include <stdlib.h>
#include <string.h>


void array_clear(void *arr, size_t item_cnt, size_t item_size) {
    if (!arr) {
        return;
    }

    memset(arr, 0, item_cnt * item_size);
}


void *array_create(size_t item_cnt, size_t item_size) {
    void *arr;
    size_t alloc_size;

    if (item_cnt == 0 || item_size == 0) {
        return NULL;
    }

    alloc_size = item_cnt * item_size;
    arr = malloc(alloc_size);
    if (!arr) {
        return NULL;
    }

    array_clear(arr, item_cnt, item_size);
    
    return arr;
}


void array_free(void **arr) {
    if (!arr || !(*arr)) {
        return;
    }

    free(*arr);
    *arr = NULL;
}
