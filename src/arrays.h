#include <stddef.h>


/**
 * \brief array_clear Sets the arrays item to 0.
 * \param item_cnt Array item count.
 * \param item_size Size of an item.
 */
void array_clear(void *arr, size_t item_cnt, size_t item_size);


/**
 * \brief array_create Creates an array of provided size.
 * \param item_cnt Array item count.
 * \param item_size Size of an item.
 * \return Pointer to the created array.
 */
void *array_create(size_t item_cnt, size_t item_size);


/**
 * \brief array_free Releases the memory heldy by the array and NULLs the pointer to it.
 * \param arr Pointer to a pointer to an array.
 */
void array_free(void **arr);
