/**
 * \file vector.h
 * \brief Header file related to manipulaiton with a vector.
 *        Vector is a dynamically resizable array.
 */


#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>


/** \brief Initial vector capacity. */
#define VECTOR_INIT_CAPACITY 4

/** \brief Vector capacity multiplicator. */
#define VECTOR_CAPACITY_MULT 2


/**
 * \brief Pointer to a function, which will perform
 *        releasing the memory held by the item of a vector.
 */
typedef void (*vector_item_deallocator)(void **item);


/**
 * \struct vector
 * Struct representing a vector, a dynamically resizable array.
 */
typedef struct vector_ {
    size_t capacity;                            /**< Capacity of the vector. */
    size_t count;                               /**< Actual number of items in the vector. */
    const size_t item_size;                     /**< Size of an vector item. */
    void *data;                                 /**< Pointer to the array. */
    const vector_item_deallocator deallocator;  /**< Vector item deallocator. */
} vector;


/**
 * \brief vector_create Creates an empty vector with default capacity.
 * \param item_size Size of an vector item.
 * \param deallocator Pointer to a function, which frees the vector items, when vector is freed.
 * \return Pointer to an empty vector with aformentioned properties.
 */
vector *vector_create(const size_t item_size, const vector_item_deallocator deallocator);


/**
 * \brief vector_init Initializes the vector to default capaity.
 * \param v Pointer to a vector.
 * \param item_size Size of an vector item.
 * \param deallocator Pointer to a function, which frees the vector items, when vector is freed.
 * \return 1 if operation was successful, else 0.
 */
int vector_init(vector *v, const size_t item_size, const vector_item_deallocator deallocator);


/**
 * \brief vector_free Releases the memory held by the vector
 *                    - frees vector struct
 *                    -- frees each vector item, if vector item deallocator was provided
 *                    and NULLs the pointer to the vector.
 * \param v Pointer to a pointer to a vector.
 */
void vector_free(vector **v);


/**
 * \brief vector_capacity Returns the capacity of the vector.
 * \param v Pointer to a vector.
 * \return Capacity of the vector.
 */
size_t vector_capacity(const vector *v);


/**
 * \brief vector_count Returns the actual count of items in the vector.
 * \param v Pointer to a vector.
 * \return Actual count of items in the vector.
 */
size_t vector_count(const vector *v);


/**
 * \brief vector_is_empty Returns 1 if vector is empty, else 0.
 * \param v Pointer to a vector.
 * \return 1 if there are not any items in the vector, else 0.
 */
int vector_is_empty(const vector *v);


/**
 * \brief vector_realloc Reallocates the vector to the capacity of provided size.
 *                       New capacity must be >= vector items count.
 * \param v Pointer to a vector.
 * \param capacity New capacity.
 * \return 1 if operation was successful, else 0.
 */
int vector_realloc(vector *v, const size_t capacity);


/**
 * \brief vector_shrink Shrinks the vector capacity
 *                      to the actual count of items in the vector.
 * \param v Pointer to a vector.
 * \return 1 if operation was successful, else 0.
 */
int vector_shrink(vector *v);


/**
 * \brief vector_at Returns a pointer to the item of the vector with provided index.
 * \param v Pointer to a vector.
 * \param index Index of the item.
 * \return Pointer to the item with provided index, if exists, else NULL.
 */
void *vector_at(const vector *v, const size_t index);


/**
 * \brief vector_push_back Appends the item to the end of the vector.
 * \param v Pointer to a vector.
 * \param item Pointer to the item.
 * \return 1 if operation was successful, else 0.
 */
int vector_push_back(vector *v, const void *item);


/**
 * \brief vector_give_up_data Returns the pointer to the given up vector's data
 *                            and initializes the vector with new array of default capacity.
 * \param v Pointer to a vector.
 * \return Pointer to the vector data.
 */
void *vector_give_up_data(vector *v);


#endif
