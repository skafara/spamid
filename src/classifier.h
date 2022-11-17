/**
 * \file classifier.h
 * \brief Header file related to manipulation with a naive Bayes classifier.
 *
 * Represents a general naive Bayes classifier.
 * Classifier has variable count of classes and uses the bag-of-words model.
 */


#ifndef CLASSIFIER_H
#define CLASSIFIER_H


#include <stddef.h>
#include <stdio.h>

#include "hashtable.h"


/**
 * \struct nbc
 * \brief Struct representing a naive Bayes classifier.
 */
typedef struct nbc_ {
    const int cls_cnt;      /**< Number of classes. */

    double *cls_prob;       /**< Aprior probabilities of occurences of classes in learnt data. */
    size_t *cls_words_cnt;  /**< Number of words in classes of learnt data. */

    htab *words_cnt;        /**< Number of occurences of words in learnt data. */
    htab *words_prob;       /**< Probabilities of words occurences in learnt data. */
    
    size_t dict_size;       /**< Number of distinct words in learnt data. */
} nbc;


/**
 * \brief nbc_create Creates an untaught classifier ready to work with provided number of classes.
 * \param cls_cnt Number of classes.
 * \return Pointer to an yet untaught classifier with provided number of classes.
 */
nbc *nbc_create(const int cls_cnt);


/**
 * \brief nbc_free Releases the memory held by the classifier
 *                 - frees nbc struct
 *                 -- frees arrays and hashtables
 *                 and NULLs the pointer to the classifier.
 * \param cl Pointer to a pointer to a classifier.
 */
void nbc_free(nbc **cl);


/**
 * \brief nbc_learn Classifier learns the provided files.
 *                  Classifier may be successfully taught only once,
 *                  any other attempts will fail.
 * \param cl Pointer to the classifier to be taught.
 * \param f_paths Array of file paths.
 * \param f_counts Numbers of file paths of classes.
 * \return 1 if classifier successfully learnt the files, 0 otherwise.
 */
int nbc_learn(nbc *cl, const char *f_paths[], const size_t f_counts[]);


/**
 * \brief nbc_is_learnt Finds out whether classifier was successfully taught.
 * \return 1 if classifier was already successfully taught, else 0.
 */
int nbc_is_learnt(const nbc *cl);


/**
 * \brief nbc_classify Classifies the provided file.
 * \param cl Pointer to the classifier to classify the file.
 * \param f_path Path to the file to be classified.
 * \return Class if provided file was successfully classified, -1 otherwise.
 */
int nbc_classify(const nbc *cl, const char f_path[]);


#endif
