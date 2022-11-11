#include <stddef.h>


/**
 * \brief cmp_func Performs a comparison of the two provided values.
 * \param value1 Pointer to the first value.
 * \param value2 Pointer to the second value.
 * \return 1 if value1 is greater than value2, -1 if value1 is less than value2,
 *         0 if value1 is equal to value2, -2 if value1 and value2 cannot be compared.
 */
typedef int (*cmp_func)(void *value1, void *value2);


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


/**
 * \brief array_clear Sets the arrays item to 0.
 * \param item_cnt Array item count.
 * \param item_size Size of an item.
 */
void array_clear(void *arr, size_t item_cnt, size_t item_size);


void *array_extreme(void *arr, cmp_func cmp, size_t item_cnt, size_t item_size);
