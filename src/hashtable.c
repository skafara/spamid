/**
 * \file hashtable.c
 * \brief Functions declared in hashtable.h are implemented in this file.
 * 
 * Hashtable is implemented as an array (buckets)
 * of linked lists of hashtable links (entries) containing the key-value pair.
 * Hashtable key is a pointer to a copy of the provided key.
 * Hashtable value is a pointer to a copy of the provided value (either a pointer or a direct value).
 */


#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "primes.h"
#include "arrays.h"


/**
 * \brief htab_link_create Creates a new hashtable entry (link) with set copies
 *                         of provided key and value and next set to NULL.
 *                         Does not check arguments validity.
 * \param ht Pointer to a hashtable.
 * \param key Char array to be copied into entry (link).
 * \param value Pointer to the value to be copied into entry (link).
 * \return Pointer to a newly created hashtable entry (link) with aformentioned properties.
 */
htab_link *htab_link_create(const htab *ht, const char *key, const void *value) {
    htab_link *new_htl;

    new_htl = (htab_link *) malloc(sizeof(htab_link));
    if (!new_htl) {
        return NULL;
    }

    new_htl->key = strdup(key);
    if (!new_htl->key) {
        free(new_htl);
        return NULL;
    }

    new_htl->value = (void *) malloc(ht->item_value_size);
    if (!new_htl->value) {
        free((void *) new_htl->key);
        free(new_htl);
        return NULL;
    }
    memcpy(new_htl->value, value, ht->item_value_size);

    new_htl->next = NULL;

    return new_htl;
}


/**
 * \brief htab_link_free Releases the memory held by the hashtable entry (link)
 *                       - frees htab_link struct
 *                       -- frees htab_link item value, if htab_item_value_deallocator was provided
 *                       --- frees key (char array copy)
 *                                 and value (copy of the value,
 *                                            where either a pointer or a direct value is stored)
 *                       Does not check arguments validity.
 * \param ht Pointer to a hashtable.
 * \param htl Pointer to a hashtable entry (link).
 */
void htab_link_free(htab *ht, htab_link *htl) {
    free((void *) htl->key);
    if (ht->item_value_deallocator) {
        (ht->item_value_deallocator)(&htl->value);
    }
    free(htl->value);
    free(htl);
}


/**
 * \brief htab_clear_buckets NULLs the buckets of the hashtable.
 *                           Does not check arguments validity.
 * \param ht Pointer to a hashtable.
 */
void htab_clear_buckets(htab *ht) {
    array_clear(ht->buckets, ht->buckets_cnt, sizeof(htab_link *));
}


htab *htab_create(const size_t item_value_size, const htab_item_value_deallocator item_value_deallocator) {
    htab *ht;

    if (item_value_size == 0) {
        return NULL;
    }

    ht = (htab *) malloc(sizeof(htab));
    if (!ht) {
        return NULL;
    }

    ht->buckets_cnt = H_DEF_BUCKETS_CNT;
    ht->buckets = (htab_link **) array_create(ht->buckets_cnt, sizeof(htab_link *));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }

    ht->items_cnt = 0;
    *((size_t *) &ht->item_value_size) = item_value_size;
    *((htab_item_value_deallocator *) &ht->item_value_deallocator) = item_value_deallocator;

    return ht;
}


void htab_free(htab **ht) {
    size_t b;
    htab_link *htl, *htl_next;

    if (!ht || !(*ht)) {
        return;
    }

    for (b = 0; b < (*ht)->buckets_cnt; b++) {
        htl = (*ht)->buckets[b];
        while (htl) {
            htl_next = htl->next;
            htab_link_free(*ht, htl);
            htl = htl_next;
        }
    }

    array_free((void **) &(*ht)->buckets);
    free(*ht);
    *ht = NULL;
}


/**
 * \brief htab_hcode Computes and returns the hashcode
 *                   of provided char array modulated by provided divisor.
 *                   Does not check arguments validity.
 * \param key Char array the hashcode to be computed of.
 * \param divisor Number the hashcode to be modulated by.
 * \return Char array hashcode modulated by divisor.
 */
size_t htab_hcode(const char *key, const size_t divisor) {
    size_t key_len, c;
    size_t hcode;
    
    key_len = strlen(key);
    for (c = 0, hcode = 0; c < key_len; c++) {
        hcode = (hcode * 256 + key[c]) % divisor;
    }

    return hcode;
}


/**
 * \brief htab_link_find Searches for the item with provided key in the hashtable
 *                       and if found, returns a pointer to the hashtable entry (link).
 * \param ht Pointer to a hashtable.
 * \param key Key of an item to be searched for in the hashtable.
 * \return Pointer to a hashtable item (link) having the same key if found, else NULL.
 */
htab_link *htab_link_find(const htab *ht, const char *key) {
    htab_link *htl;

    if (!ht || !key) {
        return NULL;
    }

    htl = ht->buckets[htab_hcode(key, ht->buckets_cnt)];
    while (htl) {
        if (strcmp(key, htl->key) == 0) {
            return htl;
        }

        htl = htl->next;
    }

    return NULL;
}


size_t htab_items_cnt(const htab *ht) {
    if (!ht) {
        return 0;
    }

    return ht->items_cnt;
}


int htab_contains(const htab *ht, const char *key) {
    if (!htab_link_find(ht, key)) {
        return 0;
    }

    return 1;
}


int htab_get(const htab *ht, const char *key, void *dest) {
    htab_link *htl;

    if (!dest) {
        return 0;
    }

    htl = htab_link_find(ht, key);
    if (!htl) {
        return 0;
    }

    memcpy(dest, htl->value, ht->item_value_size);
    return 1;
}


int htab_add_link(htab *ht, htab_link *new_htl);
/**
 * \brief htab_expand_buckets Expands hashtable buckets.
 *                            - creates new buckets
 *                            - adds all hashtable entries (links) to new buckets
 *                            - frees old buckets
 *                            Does not check arguments validity.
 * \param ht Pointer to a hashtable.
 * \param new_cnt New buckets count.
 * \return 1 if the operation was successful, else 0.
 */
int htab_expand_buckets(htab *ht, const size_t new_cnt) {
    htab_link **old_buckets;
    htab_link *htl, *htl_next;
    size_t old_buckets_cnt;
    size_t old_items_cnt;
    size_t b;
    
    old_buckets = ht->buckets;
    old_buckets_cnt = ht->buckets_cnt;

    ht->buckets = (htab_link **) array_create(new_cnt, sizeof(htab_link *));
    if (!ht->buckets) {
        ht->buckets = old_buckets;
        return 0;
    }
    
    ht->buckets_cnt = new_cnt;
    
    old_items_cnt = ht->items_cnt;
    ht->items_cnt = 0;
    for (b = 0; b < old_buckets_cnt; b++) {
        htl = old_buckets[b];
        while (htl) {
            htl_next = htl->next;
            if (!htab_add_link(ht, htl)) {
                array_free((void **) &ht->buckets);
                ht->buckets = old_buckets;
                ht->buckets_cnt = old_buckets_cnt;
                ht->items_cnt = old_items_cnt;
                return 0;
            }
            htl = htl_next;
        }
    }
    array_free((void **) &old_buckets);

    return 1;
}


/**
 * \brief htab_add_link Adds the provided hashtable entry (link) to the hashtable.
 *                      Requests hashtable buckets expansion if neccessary.
 *                      Does not check arguments validity.
 * \param ht Pointer to a hashtable.
 * \param new_htl Pointer to the hashtable entry (link).
 * \return 1 if the operation was successful, else 0.
 */
int htab_add_link(htab *ht, htab_link *new_htl) {
    size_t hcode;

    if (ht->items_cnt == ht->buckets_cnt * H_ITEMS_PER_BUCKET) {
        if (!htab_expand_buckets(ht, next_prime((2 * ht->buckets_cnt) + 1))) {
            return 0;
        }
    }

    hcode = htab_hcode(new_htl->key, ht->buckets_cnt);

    new_htl->next = ht->buckets[hcode];
    ht->buckets[hcode] = new_htl;

    ht->items_cnt++;

    return 1;
}


int htab_add(htab *ht, const char *key, const void *value) {
    htab_link *new_htl;

    if (!ht || !key || !value) {
        return 0;
    }

    new_htl = htab_link_create(ht, key, value);
    if (!new_htl) {
        return 0;
    }

    if (!htab_add_link(ht, new_htl)) {
        htab_link_free(ht, new_htl);
        return 0;
    }

    return 1;
}


/**
 * \brief htl_iter_set_next_nonempty_bucket Sets next hashtable entry (link) to the first hashtable entry (link)
 *                                          in current bucket, if there is any,
 *                                          otherwise iterates through buckets,
 *                                          until finds a bucket having a hashtable entry (link).
 *                                          Does not check arguments validity.
 * \param it Pointer to an iterator.
 */
void htl_iter_set_next_nonempty_bucket(htl_iter *it) {
    for (; it->bucket_curr < it->ht->buckets_cnt; it->bucket_curr++) {
        if (it->ht->buckets[it->bucket_curr]) {
            it->htl_next = it->ht->buckets[it->bucket_curr];
            break;
        }
    }
}


htl_iter *htl_iter_create(const htab *ht) {
    htl_iter *it;

    if (!ht) {
        return NULL;
    }

    it = (htl_iter *) malloc(sizeof(htl_iter));
    if (!it) {
        return NULL;
    }

    it->ht = ht;
    htl_iter_reset(it);

    return it;
}


void htl_iter_free(htl_iter **it) {
    if (!it || !(*it)) {
        return;
    }

    free(*it);
    *it = NULL;
}


int htl_iter_has_next(const htl_iter *it) {
    if (!it) {
        return 0;
    }

    if (!it->htl_next) {
        return 0;
    }
    return 1;
}


htab_link *htl_iter_next(htl_iter *it) {
    htab_link *htl_tmp;

    if (!it || !htl_iter_has_next(it)) {
        return NULL;
    }

    htl_tmp = it->htl_next;
    
    it->htl_next = it->htl_next->next;
    if (!it->htl_next) {
        it->bucket_curr++;
        htl_iter_set_next_nonempty_bucket(it);
    }
    
    return htl_tmp;
}


void htl_iter_reset(htl_iter *it) {
    if (!it) {
        return;
    }

    it->bucket_curr = 0;
    it->htl_next = NULL;
    htl_iter_set_next_nonempty_bucket(it);
}
