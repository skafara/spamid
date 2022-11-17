/**
 * \file arrays.c
 * \brief Functions declared in arrays.h are implemented in this file.
 */


#include <stdlib.h>
#include <string.h>

#include "arrays.h"


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


void array_clear(void *arr, size_t item_cnt, size_t item_size) {
    if (!arr) {
        return;
    }

    memset(arr, 0, item_cnt * item_size);
}


void *array_extreme(const void *arr, cmp_func cmp, size_t item_cnt, size_t item_size) {
    char *value1, *value2, *extreme;

    if (!arr || item_cnt == 0 || item_size == 0) {
        return NULL;
    }

    value1 = (char *) arr;
    value2 = value1 + item_size;
    extreme = value1;
    for (; value2 < (char *) arr + (item_cnt * item_size); value1 = value2, value2 += item_size) {
        if (cmp(value2, value1) > 0) {
            extreme = value2;
        }
    }

    return extreme;
}
