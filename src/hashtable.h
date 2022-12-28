/**
 * \file hashtable.h
 * \brief Header file related to manipulation with a hashtable.
 * \version 1, 28-12-2022
 * \author Stanislav Kafara, skafara@students.zcu.cz
 *
 * Used for manipulation with a hashtable.
 * Hashtable is implemented as an array (buckets)
 * of linked lists of hashtable links (entries) containing the key-value pair.
 * Hashtable key is a pointer to a copy of the provided key.
 * Hashtable value is a pointer to a copy of the provided value (either a pointer or a direct value).
 */


#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>


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
    const htab_item_value_deallocator item_value_deallocator; /**< Hashtable item value deallocator. */
} htab;


/**
 * \struct htl_iter
 * \brief Struct representing an iterator over hashtable entries (links).
 */
typedef struct htl_iter_ {
    const htab *ht;         /**< Pointer to a hashtable to be iterated through. */
    size_t bucket_curr;     /**< Bucket of the next hashtable entry (link). */
    htab_link *htl_next;    /**< Pointer to the next hashtable entry (link). */
} htl_iter;


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
 *                  --- frees htab_link key (char array copy)
 *                                      and value (copy of the value,
 *                                                 where either a pointer or a direct value is stored)
 *                  ---- frees htab_link item value, if htab_item_value_deallocator was provided
 *                       and NULLs the pointer to the hashtable.
 * \param ht Pointer to a pointer to a hashtable.
 */
void htab_free(htab **ht);


/**
 * \brief htab_items_cnt Returns hashtable item count.
 * \param ht Pointer to a hastable.
 * \return Hashtable items count.
 */
size_t htab_items_cnt(const htab *ht);


/**
 * \brief htab_contains Checks whether an item with provided key is present in the hashtable.
 * \param ht Pointer to a hashtable.
 * \param key Key of an item to be searched for in the hashtable.
 * \return 1 if the hashtable contains an item with provided key, else 0.
 */
int htab_contains(const htab *ht, const char *key);


/**
 * \brief htab_ptrget Searches for the item with provided key in the hashtable
 *                    and if found, returns a pointer to the item value.
 * \param ht Pointer to a hashtable.
 * \param key Key of an item to be searched for in the hashtable.
 * \return Pointer to the item value if the hashtable contains an item with provided key, else NULL.
 */
void *htab_ptrget(const htab *ht, const char *key);


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


/**
 * \brief htl_iter_create Creates an iterator over entries (links) of provided hashtable.
 * \param ht Pointer to a hashtable to be iterated through.
 * \return Pointer to an iterator over hashtable entries (links).
 */
htl_iter *htl_iter_create(const htab *ht);


/**
 * \brief htl_iter_free Releases the memory held by the hashtable iterator
 *                      - frees htl_iter struct
 *                      and NULLs the pointer to the hashtable iterator.
 * \param it Pointer to a pointer to an iterator.
 */
void htl_iter_free(htl_iter **it);


/**
 * \brief htl_iter_has_next Finds out whether the iterator has next hashtable entry (link).
 * \param it Pointer to an iterator.
 * \return 1 if iterator has next hashtable entry (link), else 0.
 */
int htl_iter_has_next(const htl_iter *it);


/**
 * \brief htl_iter_next Returns next hashtable entry (link), if there is any.
 * \param it Pointer to an iterator.
 * \return Pointer to next hashtable entry (link), if there is any, else NULL.
 */
htab_link *htl_iter_next(htl_iter *it);


/**
 * \brief htl_iter_reset Returns the iterator to the beginning.
 */
void htl_iter_reset(htl_iter *it);


#endif
