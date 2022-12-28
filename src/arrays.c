/**
 * \file arrays.c
 * \brief Functions declared in arrays.h are implemented in this file.
 * \version 1, 28-12-2022
 * \author Stanislav Kafara, skafara@students.zcu.cz
 */


#include <stdlib.h>
#include <string.h>

#include "arrays.h"


void *array_create(const size_t item_cnt, const size_t item_size) {
    void *arr = NULL;

    if (item_cnt == 0 || item_size == 0) {
        return NULL;
    }

    arr = malloc(item_cnt * item_size);
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


void array_clear(void *arr, const size_t item_cnt, const size_t item_size) {
    if (!arr) {
        return;
    }

    memset(arr, 0, item_cnt * item_size);
}


void *array_extreme(const void *arr, const cmp_func cmp, const size_t item_cnt, const size_t item_size) {
    char *value1 = NULL, *value2 = NULL, *extreme = NULL;

    if (!arr || item_cnt == 0 || item_size == 0) {
        return NULL;
    }

    value1 = (char *) arr;
    value2 = value1 + item_size;
    extreme = value1;
    while (value2 < (char *) arr + (item_cnt * item_size)) {
        if (cmp(value2, value1) > 0) {
            extreme = value2;
        }

        value1 = value2;
        value2 += item_size;
    }
    return extreme;
}
