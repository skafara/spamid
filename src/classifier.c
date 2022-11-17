/**
 * \file classifier.c
 * \brief Functions declared in classifier.c are implemented in this file.
 *
 * Represents a general naive Bayes classifier.
 * Classifier has variable count of classes and uses the bag-of-words model.
 */


#include <stdio.h>
#include <math.h>

#include "classifier.h"
#include "vector.h"
#include "arrays.h"


/**
 * \brief f_next_str Reads next string from the file stream.
 *                   Allocates a memory for the string and returns the pointer to the string.
 *                   Allocated memory must later be freed.
 * \param fp File handle.
 * \return Pointer to the newly allocated memory, where the string is stored,
 *         or NULL if there is not any more.
 */
char *f_next_str(FILE *fp) {
    vector *v;
    char *str;
    int c;

    if (!fp) {
        return NULL;
    }

    v = vector_create(sizeof(char), NULL);
    if (!v) {
        return NULL;
    }

    c = fgetc(fp);
    if (c == EOF || c == '\r' || c == '\n') {
        goto fail;
    }

    while (c != EOF || c != '\r' || c != '\n') {
        if (c == ' ') {
            break;
        }

        if (!vector_push_back(v, &c)) {
            goto fail;
        }

        c = fgetc(fp);
    }

    c = '\x00';
    if (!vector_push_back(v, &c) || !vector_shrink(v)) {
        goto fail;
    }

    str = (char *) vector_give_up_data(v);

    vector_free(&v);
    return str;

    fail:
    vector_free(&v);
    return NULL;
}


/**
 * \brief cmp_double_greater Performs a comparison of the two provided values.
 * \param value1 Pointer to the first value.
 * \param value2 Pointer to the second value.
 * \return 1 if value1 is greater than value2, -1 if value1 is less than value2,
 *         0 if value1 is equal to value2.
 */
int cmp_double_greater(const void *value1, const void *value2) {
    if (*((double *) value1) > *((double *) value2)) {
        return 1;
    }
    else if (*((double *) value1) < *((double *) value2)) {
        return -1;
    }
    else {
        return 0;
    }
}


/**
 * \brief nbc_arrays_htabs_free Releases the memory held by the classifier's arrays and hashtables
 *                              and NULLs the pointers to the arrays and hashtables.
 * \param cl Pointer to a classifier.
 */
void nbc_arrays_htabs_free(nbc *cl) {
    if (!cl) {
        return;
    }
    
    array_free((void **) &cl->cls_prob); array_free((void **) &cl->cls_words_cnt);
    htab_free(&cl->words_cnt); htab_free(&cl->words_prob);

    cl->cls_prob = NULL; cl->cls_words_cnt = NULL;
    cl->words_cnt = cl->words_prob = NULL;
}


/**
 * \brief nbc_reset Frees and creates new classifier's arrays and hashtables and sets dictionary size to 0.
 * \param cl Pointer to a classifier.
 * \return 1 if operation was successful, else 0.
 */
int nbc_reset(nbc *cl) {
    double *new_cls_prob;
    size_t *new_cls_words_cnt;
    htab *new_words_cnt, *new_words_prob;

    if (!cl) {
        return 0;
    }

    new_cls_prob = (double *) array_create(cl->cls_cnt, sizeof(double));
    new_cls_words_cnt = (size_t *) array_create(cl->cls_cnt, sizeof(size_t));
    new_words_cnt = htab_create(sizeof(size_t *), (htab_item_value_deallocator) array_free);
    new_words_prob = htab_create(sizeof(size_t *), (htab_item_value_deallocator) array_free);

    if (!new_cls_prob || !new_cls_words_cnt || !new_words_cnt || !new_words_prob) {
        array_free((void **) &new_cls_prob); array_free((void **) &new_cls_words_cnt);
        htab_free(&new_words_cnt); htab_free(&new_words_prob);
        return 0;
    }

    nbc_arrays_htabs_free(cl);
    cl->cls_prob = new_cls_prob; cl->cls_words_cnt = new_cls_words_cnt;
    cl->words_cnt = new_words_cnt; cl->words_prob = new_words_prob;
    cl->dict_size = 0;

    return 1;
}


/**
 * \brief nbc_initialize Initializes the classifier to provided number of classes.
 * \param cl Pointer to a classifier.
 * \param cls_cnt Number of classes.
 * \return 1 if operation was successful, else 0.
 */
int nbc_initialize(nbc *cl, const int cls_cnt) {
    if (!cl || nbc_is_learnt(cl) || cls_cnt == 0) {
        return 0;
    }

    *((int *) &cl->cls_cnt) = cls_cnt;

    cl->cls_prob = NULL; cl->cls_words_cnt = NULL;
    cl->words_cnt = cl->words_prob = NULL;

    if (!nbc_reset(cl)) {
        return 0;
    }

    return 1;
}


nbc *nbc_create(const int cls_cnt) {
    nbc *cl;

    if (cls_cnt == 0) {
        return NULL;
    }

    cl = (nbc *) malloc(sizeof(nbc));
    if (!cl) {
        return NULL;
    }

    cl->dict_size = 0;
    if (!nbc_initialize(cl, cls_cnt)) {
        nbc_free(&cl);
        return NULL;
    }

    return cl;
}


void nbc_free(nbc **cl) {
    if (!cl || !(*cl)) {
        return;
    }

    nbc_arrays_htabs_free(*cl);
    free(*cl);
    *cl = NULL;
}


/**
 * \brief nbc_set_cls_prob Sets aprior probabilities of classes in learnt files.
 *                         Does not check arguments validity.
 * \param cl Pointer to a classifier.
 * \param f_counts Array of counts of files of the same class.
 */
void nbc_set_cls_prob(nbc *cl, const size_t f_counts[]) {
    size_t f_counts_sum;
    int cls;

    f_counts_sum = 0;
    for (cls = 0; cls < cl->cls_cnt; cls++) {
        f_counts_sum += f_counts[cls];
    }

    for (cls = 0; cls < cl->cls_cnt; cls++) {
        cl->cls_prob[cls] = 1 - ((double) (f_counts_sum - f_counts[cls]) / f_counts_sum);
    }
}


/**
 * \brief nbc_add_words_cnt Adds the counts of the words in the file of the provided class.
 * \param cl Pointer to a classifier.
 * \param fp File handle.
 * \param cls Class to which the file belongs to.
 * \return 1 if counts of words were successfuly added.
 */
int nbc_add_words_cnt(nbc *cl, FILE *fp, const int cls) {
    char *word;
    size_t *word_cnt;

    while ((word = f_next_str(fp))) {
        if (htab_contains(cl->words_cnt, word)) {
            if (!htab_get(cl->words_cnt, word, &word_cnt)) {
                goto fail;
            }
            word_cnt[cls]++;
        }
        else {
            word_cnt = (size_t *) array_create(cl->cls_cnt, sizeof(size_t));
            if (!word_cnt) {
                goto fail;
            }
            word_cnt[cls] = 1;
            if (!htab_add(cl->words_cnt, word, &word_cnt)) {
                array_free((void **) &word_cnt);
                goto fail;
            }
        }
        free(word);
    }

    return 1;

    fail:
    free(word);
    return 0;
}


/**
 * \brief nbc_set_words_cnt Sets counts of words in provided files of classifier's classes.
 * \param cl Pointer to a classifier.
 * \param f_paths Array of paths to files to be learnt.
 * \param f_counts Array of counts of file paths for each class.
 * \return 1 if classifier learnt all the provided files, else 0.
 */
int nbc_set_words_cnt(nbc *cl, const char *f_paths[], const size_t f_counts[]) {
    size_t f, f_offset;
    FILE *fp;
    int cls;

    f_offset = 0;
    for (cls = 0; cls < cl->cls_cnt; cls++) {
        for (f = 0; f < f_counts[cls]; f++) {
            fp = fopen(f_paths[f_offset + f], "r");
            if (!fp) {
                return 0;
            }
            
            if (!nbc_add_words_cnt(cl, fp, cls)) {
                fclose(fp);
                return 0;
            }
            
            if (fclose(fp) == EOF) {
                return 0;
            }
        }
        
        f_offset += f_counts[cls];
    }

    return 1;
}


/**
 * \brief nbc_set_cls_words_cnt Sets count of learnt words (counting duplicities) for each class.
 *                              Does not check arguments validity.
 * \param cl Pointer to a classifier.
 * \return 1 if operation was successful, else 0.
 */
int nbc_set_cls_words_cnt(nbc *cl) {
    size_t *words_cnt;
    htl_iter *it;
    htab_link *htl;
    int cls;

    words_cnt = array_create(cl->cls_cnt, sizeof(size_t));
    it = htl_iter_create(cl->words_cnt);
    if (!words_cnt || !it) {
        array_free((void **) &words_cnt);
        htl_iter_free(&it);
        return 0;
    }

    while ((htl = htl_iter_next(it))) {
        for (cls = 0; cls < cl->cls_cnt; cls++) {
            words_cnt[cls] += (*((size_t **) (htl->value)))[cls];
        }
    }
    for (cls = 0; cls < cl->cls_cnt; cls++) {
        cl->cls_words_cnt[cls] = words_cnt[cls];
    }

    array_free((void **) &words_cnt);
    htl_iter_free(&it);
    return 1;
}


/**
 * \brief nbc_set_dict_size Sets classifier dictionary size (number of distinct learnt words).
 *                          Does not check arguments validity.
 * \param cl Pointer to a classifier.
 */
void nbc_set_dict_size(nbc *cl) {
    cl->dict_size = htab_items_cnt(cl->words_cnt);
}


/**
 * \brief nbc_set_words_prob Sets words probabilities.
 *                           Does not check arguments validity.
 * \param cl Pointer to a classifier.
 * \return 1 if operation was successful, else 0.
 */
int nbc_set_words_prob(nbc *cl) {
    htl_iter *it;
    htab_link *htl;
    double *word_prob;
    size_t *word_cnt;
    int cls;

    it = htl_iter_create(cl->words_cnt);
    if (!it) {
        return 0;
    }

    while ((htl = htl_iter_next(it))) {
        if (!htab_contains(cl->words_prob, htl->key)) {
            word_prob = (double *) array_create(cl->cls_cnt, sizeof(double));
            if (!word_prob) {
                return 0;
            }
            if (!htab_add(cl->words_prob, htl->key, &word_prob)) {
                array_free((void **) &word_prob);
                return 0;
            }
        }
    }

    htl_iter_reset(it);
    while ((htl = htl_iter_next(it))) {
        for (cls = 0; cls < cl->cls_cnt; cls++) {
            if (!htab_get(cl->words_prob, htl->key, &word_prob)) {
                return 0;
            }
            if (!htab_get(cl->words_cnt, htl->key, &word_cnt)) {
                return 0;
            }
            word_prob[cls] = (double) (1 + word_cnt[cls]) / (cl->cls_words_cnt[cls] + cl->dict_size);
        }
    }

    htl_iter_free(&it);

    return 1;
}


int nbc_learn(nbc *cl, const char *f_paths[], const size_t f_counts[]) {
    if (!cl || nbc_is_learnt(cl) || !f_paths || !f_counts) {
        return 0;
    }

    nbc_set_cls_prob(cl, f_counts);
    if (!nbc_set_words_cnt(cl, f_paths, f_counts)) {
        goto fail;
    }
    if (!nbc_set_cls_words_cnt(cl)) {
        goto fail;
    }
    nbc_set_dict_size(cl);
    if (!nbc_set_words_prob(cl)) {
        goto fail;
    }

    return 1;

    fail:
    nbc_reset(cl);
    return 0;
}


int nbc_is_learnt(const nbc *cl) {
    if (!cl) {
        return 0;
    }

    return cl->dict_size > 0;
}


int nbc_classify(const nbc *cl, const char f_path[]) {
    FILE *fp;
    double *probs;
    int cls;
    char *word;
    double *word_prob;
    double *max_prob;

    if (!nbc_is_learnt(cl)) {
        return -1;
    }

    fp = fopen(f_path, "r");
    if (!fp) {
        return -1;
    }

    probs = array_create(2, sizeof(double));
    if (!probs) {
        goto fail;
    }
    for (cls = 0; cls < cl->cls_cnt; cls++) {
        probs[cls] = log10(cl->cls_prob[cls]);
    }
    while ((word = f_next_str(fp))) {
        if (!htab_contains(cl->words_prob, word)) {
            continue;
        }
        if (!htab_get(cl->words_prob, word, &word_prob)) {
            goto fail;
        }
        for (cls = 0; cls < cl->cls_cnt; cls++) {
            probs[cls] += log10(word_prob[cls]);
        }
    }
    if (fclose(fp) == EOF) {
        fp = NULL;
        goto fail;
    }
    fp = NULL;

    max_prob = array_extreme(probs, (cmp_func) cmp_double_greater, cl->cls_cnt, sizeof(double));
    if (!max_prob) {
        goto fail;
    }

    cls = max_prob - probs;
    array_free((void **) &probs);
    
    return cls;

    fail:
    if (fp) {
        fclose(fp);
        fp = NULL;
    }
    array_free((void **) &probs);
    return -1;
}
