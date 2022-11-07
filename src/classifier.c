/**
 * \file classifier.c
 * \brief Functions declared in classifier.c are implemented in this file.
 *
 * Naive Bayse classifier uses the bag-of-words model.
 * ... TODO
 */


#include "classifier.h"


void nbc_free(nbc **cl) {
    if (!cl || !*cl) {
        return;
    }

    free((*cl)->cls_prob); free((*cl)->cls_words_cnt);
    htab_free(&(*cl)->words_cnt); htab_free(&(*cl)->words_prob);
    free(*cl);

    *cl = NULL;
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

    cl->cls_prob = NULL; cl->cls_words_cnt = NULL;
    cl->words_cnt = cl->words_prob = NULL;

    *((int *) &cl->cls_cnt) = cls_cnt;
    cl->dict_size = 0;

    cl->cls_prob = (double *) malloc(cl->cls_cnt * sizeof(double));
    cl->cls_words_cnt = (size_t *) malloc(cl->cls_cnt * sizeof(size_t));
    cl->words_cnt = htab_create(sizeof(size_t *));
    cl->words_prob = htab_create(sizeof(size_t *));
    if (!cl->cls_prob || !cl->cls_words_cnt || !cl->words_cnt || !cl->words_prob) {
        nbc_free(&cl);
        return NULL;
    }

    return cl;
}


int nbc_learn(nbc *cl, const char *f_paths[], const size_t f_counts[]);


int nbc_classify(const nbc *cl, const char f_path[]);
