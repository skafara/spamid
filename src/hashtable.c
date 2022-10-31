#include <string.h>

#include "hashtable.h"
#include "primes.h"


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

    new_htl->key = (char *) malloc(strlen(key) + 1);
    if (!new_htl->key) {
        free(new_htl);
        return NULL;
    }
    strcpy((char *) new_htl->key, key);

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
 *                       -- frees key (char array copy)
 *                                and value (void pointer to copy of the value,
                                             where either a pointer or a direct value is stored)
 *                       Does not check arguments validity.
 * \param htl Pointer to a hashtable entry (link).
 */
void htab_link_free(htab_link *htl) {
    free((void *) htl->key);
    free(htl->value);
    free(htl);
}


/**
 * \brief htab_clear_buckets NULLs the buckets of the hashtable.
 *                           Does not check arguments validity.
 * \param ht Pointer to a hashtable.
 */
void htab_clear_buckets(htab *ht) {
    memset(ht->buckets, 0, ht->buckets_cnt * sizeof(htab_link **));
}


htab *htab_create(const size_t item_value_size) {
    htab *ht;

    if (item_value_size == 0) {
        return NULL;
    }

    ht = (htab *) malloc(sizeof(htab));
    if (!ht) {
        return NULL;
    }

    ht->buckets_cnt = H_DEF_BUCKETS_CNT;
    ht->buckets = (htab_link **) malloc(ht->buckets_cnt * sizeof(htab_link *));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }
    htab_clear_buckets(ht);

    ht->items_cnt = 0;
    *((size_t *) &ht->item_value_size) = item_value_size;

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
            htab_link_free(htl);
            htl = htl_next;
        }
    }

    free((*ht)->buckets);
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


int htab_contains(const htab *ht, const char *key) {
    htab_link *htl;

    if (!ht || !key) {
        return 0;
    }

    htl = ht->buckets[htab_hcode(key, ht->buckets_cnt)];
    while (htl) {
        if (strcmp(key, htl->key) == 0) {
            return 1;
        }

        htl = htl->next;
    }

    return 0;
}


int htab_get(const htab *ht, const char *key, void *dest) {
    htab_link *htl;

    if (!ht || !key || !dest) {
        return 0;
    }

    htl = ht->buckets[htab_hcode(key, ht->buckets_cnt)];
    while (htl) {
        if (strcmp(key, htl->key) == 0) {
            memcpy(dest, htl->value, ht->item_value_size);

            return 1;
        }

        htl = htl->next;
    }
    
    return 0;
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

    ht->buckets = (htab_link **) malloc(new_cnt * sizeof(htab_link *));
    if (!ht->buckets) {
        ht->buckets = old_buckets;
        return 0;
    }
    
    ht->buckets_cnt = new_cnt;
    
    htab_clear_buckets(ht);
    old_items_cnt = ht->items_cnt;
    ht->items_cnt = 0;
    for (b = 0; b < old_buckets_cnt; b++) {
        htl = old_buckets[b];
        while (htl) {
            htl_next = htl->next;
            if (!htab_add_link(ht, htl)) {
                free(ht->buckets);
                ht->buckets = old_buckets;
                ht->buckets_cnt = old_buckets_cnt;
                ht->items_cnt = old_items_cnt;
                return 0;
            }
            htl = htl_next;
        }
    }
    free(old_buckets);

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
        htab_link_free(new_htl);
        return 0;
    }

    return 1;
}
