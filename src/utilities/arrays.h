/**
 * \file arrays.h
 * \brief Header file related to manipulation with arrays.
 * \version 1, 28-12-2022
 * \author Stanislav Kafara, skafara@students.zcu.cz
 */


#ifndef ARRAYS_H
#define ARRAYS_H


/**
 * \brief cmp_func Performs a comparison of the two provided values.
 * \param value1 Pointer to the first value.
 * \param value2 Pointer to the second value.
 * \return 1 if value1 is greater than value2, -1 if value1 is less than value2,
 *         0 if value1 is equal to value2.
 */
typedef int (*cmp_func)(const void *value1, const void *value2);


/**
 * \brief array_create Creates an empty array of provided size.
 * \param item_cnt Array item count.
 * \param item_size Size of an item.
 * \return Pointer to the created array.
 */
void *array_create(const size_t item_cnt, const size_t item_size);


/**
 * \brief array_free Releases the memory held by the array and NULLs the pointer to it.
 * \param arr Pointer to a pointer to an array.
 */
void array_free(void **arr);


/**
 * \brief array_clear Sets the arrays item to 0.
 * \param item_cnt Array item count.
 * \param item_size Size of an item.
 */
void array_clear(void *arr, const size_t item_cnt, const size_t item_size);


/**
 * \brief array_extreme Returns a pointer to the extremal value in the provided array.
 * \param arr Array to be searched in for the extremal value.
 * \param cmp Function to be used to compare the values of the array.
 * \param item_cnt Array items count.
 * \param item_size Size of an item of the array.
 */
void *array_extreme(const void *arr, const cmp_func cmp, const size_t item_cnt, const size_t item_size);


#endif
