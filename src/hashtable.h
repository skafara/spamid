/**
 * \file hashtable.h
 * \brief Header file related to manipulation with a hashtable.
 *
 * Used for manipulation with a hashtable.
 * Hashtable is implemented as an array (buckets)
 * of linked lists of hashtable links (entries) containing the key-value pair.
 * Hashtable key is a pointer to a copy of the provided key.
 * Hashtable value is a pointer to a copy of the provided value (either a pointer or a direct value).
 */


#include <stdlib.h>


#ifndef HASHTABLE_H
#define HASHTABLE_H

/**< Hashtable default buckets count. */
#define H_DEF_BUCKETS_CNT 5
/**< Hashtable average items count per basket. */
#define H_ITEMS_PER_BUCKET 5


/**
 * \brief Pointer to a function, which will perform
 *        releasing the memory held by the value of a hashtable item.
 */
typedef void (*htab_item_value_deallocator)(void **value);


/**
 * \struct htab_link
 * \brief Struct representing an entry (link) in a hashtable.
 */
typedef struct htab_link_ {
    const char *key;            /**< char pointer to copy of the key: Key. */
    void *value;                /**< void pointer to copy of the value
                                     (either a pointer or a direct value): Value. */
    struct htab_link_ *next;    /**< Reference to next entry (link). */
} htab_link;


/**
 * \struct htab
 * \brief Struct representing a hashtable.
 */
typedef struct htab_ {
    htab_link **buckets;            /**< array of pointers to htab_links: Hashtable buckets. */
    size_t buckets_cnt;             /**< Hashtable buckets count. */
    size_t items_cnt;               /**< Hashtable items count. */
    const size_t item_value_size;   /**< Hashtable item value size. */
    const htab_item_value_deallocator item_value_deallocator;
                                    /**< Hashtable item value deallocator. */
} htab;


/**
 * \brief htab_create Creates an empty hashtable with default buckets count
 *                    ready to work with items with values of provided size.
 * \param item_value_size Size of hashtable item value.
 * \param item_value_deallocator Pointer to a function that frees hashtable item values, when hashtable is freed.
 * \return Pointer to a new empty hashtable with aformentioned properties.
 */
htab *htab_create(const size_t item_value_size, const htab_item_value_deallocator item_value_deallocator);


/**
 * \brief htab_free Releases the memory held by the hashtable
 *                  - frees htab struct
 *                  -- frees each htab_link struct in buckets
 *                  --- frees htab_link item value, if htab_item_value_deallocator was provided
 *                  ---- frees htab_link key (char array copy)
 *                                       and value (copy of the value,
 *                                                  where either a pointer or a direct value is stored)
 *                  and NULLs the pointer to the hashtable.
 * \param ht Pointer to a pointer to a hashtable.
 */
void htab_free(htab **ht);


/**
 * \brief htab_contains Checks whether an item with provided key is present in the hashtable.
 * \param ht Pointer to a hashtable.
 * \param key Key of an item to be searched for in the hashtable.
 * \return 1 if the hashtable contains an item with provided key, else 0.
 */
int htab_contains(const htab *ht, const char *key);


/**
 * \brief htab_get Searches for the item with provided key in the hashtable
 *                 and if found, copies the item value to destination.
 * \param ht Pointer to a hashtable.
 * \param key Key of an item to be searched for in the hashtable.
 * \param dest Pointer to destination, where the value will be copied to.
 * \return 1 if the hashtable contains an item with provided key and value was copied to destination, else 0.
 */
int htab_get(const htab *ht, const char *key, void *dest);


/**
 * \brief htab_add Adds an hashtable entry (link) to the hashtable.
 *                 Copies the provided key and value (either a pointer or a direct value) to the entry,
 *                 which is then added to the hashtable.
 * \param ht Pointer to a hashtable.
 * \param key Key to be copied and added to the hashtable.
 * \param value Pointer to the value to be copied and added to the hashtable.
 * \return 1 if the operation was successful, else 0.
 */
int htab_add(htab *ht, const char *key, const void *value);


#endif
