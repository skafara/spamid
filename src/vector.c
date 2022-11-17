/**
 * \file vector.c
 * \brief Functions declared in vector.h are implemented in this file.
 *        Vector is a dynamically resizable array.
 */


#include <stdlib.h>
#include <string.h>

#include "vector.h"


int vector_realloc(vector *v, const size_t size) {
    void *new_data;

    if (!v || size < vector_count(v)) {
        return 0;
    }

    new_data = realloc(v->data, v->item_size * size);
    if (!new_data) {
        return 0;
    }

    v->capacity = size;
    v->data = new_data;

    return 1;
}


int vector_shrink(vector *v) {
    if (!v) {
        return 0;
    }

    if (!vector_realloc(v, vector_count(v))) {
        return 0;
    }
    return 1;
}


int vector_init(vector *v, const size_t item_size, const vector_item_deallocator deallocator) {
    if (!v || item_size == 0) {
        return 0;
    }

    v->count = 0;
    v->capacity = 0;
    *((size_t *) &v->item_size) = item_size;
    v->data = NULL;
    *((vector_item_deallocator *) &v->deallocator) = deallocator;

    if (!vector_realloc(v, VECTOR_INIT_CAPACITY)) {
        return 0;
    }

    *((char *) v->data) = 0;

    return 1;
}


vector *vector_create(const size_t item_size, const vector_item_deallocator deallocator) {
    vector *v;

    if (item_size == 0) {
        return NULL;
    }

    v = (vector *) malloc(sizeof(vector));
    if (!v) {
        return NULL;
    }

    if (!vector_init(v, item_size, deallocator)) {
        free(v);
        return NULL;
    }

    return v;
}


vector *vector_clone(const vector *v) {
    vector *clone;

    if (!v) {
        return NULL;
    }

    clone = vector_create(v->item_size, v->deallocator);
    if (!clone) {
        return NULL;
    }
    if (vector_capacity(v) != vector_capacity(clone)) {
        if (vector_realloc(clone, vector_capacity(v))) {
            vector_free(&clone);
            return NULL;
        }
    }

    clone->count = v->count;
    memcpy(clone->data, v->data, vector_count(clone) * clone->item_size);

    return clone;
}


/**
 * \brief vector_at_ Returns a pointer to the item with provided index.
 *                   Does not check argument validity.
 * \return Pointer to the item with provided index.
 */
void *vector_at_(const vector *v, const size_t index) {
    return (void *) ((char *) v->data + (index * v->item_size));
}


void *vector_at(const vector *v, const size_t index) {
    if (index >= vector_count(v)) {
        return NULL;
    }

    return vector_at_(v, index);
}


int vector_push_back(vector *v, const void *item) {
    if (!v || !item) {
        return 0;
    }

    if (vector_count(v) >= vector_capacity(v)) {
        if (!vector_realloc(v, v->capacity * VECTOR_CAPACITY_MULT)) {
            return 0;
        }
    }

    memcpy(vector_at_(v, v->count), item, v->item_size);
    v->count++;
    
    return 1;
}


int vector_push_back_many(vector *v, const void *items, const size_t items_cnt) {
    size_t v_old_capacity;
    size_t i;

    v_old_capacity = v->capacity;
    for (i = 0; i < items_cnt; i++) {
        if (!vector_push_back(v, ((char *) items) + (i * v->item_size))) {
            v->count -= i;
            if (vector_capacity(v) != v_old_capacity) {
                vector_realloc(v, v_old_capacity);
            }
            return 0;
        }
    }

    return 1;
}


size_t vector_capacity(const vector *v) {
    if (!v) {
        return 0;
    }
    else {
        return v->capacity;
    }
}


size_t vector_count(const vector *v) {
    if (!v) {
        return 0;
    }
    else {
        return v->count;
    }
}


int vector_is_empty(const vector *v)  {
    return vector_count(v) == 0;
}


void *vector_give_up_data(vector *v) {
    void *data;

    if (vector_is_empty(v)) {
        return NULL;
    }

    data = v->data;

    vector_init(v, v->item_size, v->deallocator);

    return data;
}


void vector_free(vector **v) {
    size_t i;

    if (!v || !(*v)) {
        return;
    }

    if ((*v)->deallocator) {
        for (i = 0; i < vector_count(*v); i++) {
            ((*v)->deallocator)(vector_at_(*v, i));
        }
    }

    if ((*v)->data) {
        free((*v)->data);
    }
    free(*v);
    *v = NULL;
}
