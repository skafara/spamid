/**
 * \file spamid.c
 * \brief Main file of the spam identifier application.
 * \version 1, 02-01-2023
 * \author Stanislav Kafara, skafara@students.zcu.cz
 *
 * Processes input arguments,
 * teaches classifier provided files,
 * classifies provided files,
 * outputs results into provided file.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "classifier.h"
#include "utilities/utils.h"

/** \brief Required program input arguments count. */
#define REQUIRED_ARGS_CNT 7
/** \brief Spam, ham classifier classes count. */
#define CLASSIFIER_CLS_CNT 2
/** \brief Format of one line in classification result file. */
#define RESULT_LINE_FORMAT "%s\t%s\n"

/** \brief Directory containing the files to be learnt and classified. */
#define DATA_DIR "data"
/** \brief Suffix of files to be learnt and classified. */
#define FILE_SUFFIX ".txt"

/** \brief class SPAM, HAM enum */
typedef enum class_ {SPAM, HAM} class;
/** \brief Class description put in the classification result file. */
const char *RESULT_CLASS_DESCRIPTION[CLASSIFIER_CLS_CNT] = {"S", "H"};


/**
 * \brief print_nl Prints new line.
 */
void print_nl() {
    printf("\n");
}


/**
 * \brief print_indented Prints indented message.
 * \param message Message to be printed.
 */
void print_indented(const char message[]) {
    printf("\t%s\n", message);
}


/**
 * \brief print_err Prints an error message.
 * \param message Error message.
 */
void print_err(const char message[]) {
    printf("spamid[ERROR]: %s\n", message);
}


/**
 * \brief print_man Prints a manual.
 */
void print_man() {
    printf("Spam/Ham Naive Bayes Classifier (23-01-02)\n");
    printf("Seminar Work of KIV/PC - \"Programming in C\"\n");
    printf("Stanislav Kafara, skafara@students.zcu.cz\n");
    printf("University of West Bohemia, Pilsen\n");
    print_nl();
    printf("Usage:\n");
    print_indented("spamid <spam> <spam-cnt> <ham> <ham-cnt> <test> <test-cnt> <out-file>");
    print_nl();
    print_indented("<spam>     - Training spam files pattern.");
    print_indented("<spam-cnt> - Training spam files count.");
    print_indented("<ham>      - Training ham files pattern.");
    print_indented("<ham-cnt>  - Training ham files count.");
    print_indented("<test>     - Tested files pattern.");
    print_indented("<test-cnt> - Tested files count.");
    print_indented("<out-file> - Output file name.");
    print_nl();
    printf("Example:\n");
    print_indented("spamid spam 1234 ham 1234 test 12 result.txt");
    print_nl();
    print_indented("Classifier learns 1234 spam files (\"spam1.txt\" ... \"spam1234.txt\").");
    print_indented("Classifier learns 1234 ham files (\"ham1.txt\" ... \"ham1234.txt\").");
    print_indented("Classifier classifies 12 tested files (\"test1.txt\" ... \"test12.txt\").");
    print_indented("Output is printed to file \"result.txt\".");
}


/**
 * \brief is_valid_count Checks whether provided string is a valid count.
 *                       String is a valid count if it is an integer > 0 and does not start with 0.
 * \param str String to be checked for validity.
 * \return 1 if string is a valid count, else 0.
 */
int is_valid_count(const char *str) {
    size_t c;

    if (!str || strlen(str) == 0) {
        return 0;
    }

    if (strlen(str) > 1 && str[0] == '0') {
        return 0;
    }

    for (c = 0; c < strlen(str); c++) {
        if (!isdigit(str[c])) {
            return 0;
        }
    }
    return 1;
}


/**
 * \brief f_paths_free Releases the memory held by the array of file paths and NULLs the pointer to it.
 * \param f_paths Pointer to an array of file paths.
 * \param f_paths_cnt Number of file paths.
 */
void f_paths_free(char **f_paths[], size_t f_paths_cnt) {
    size_t f;

    if (!f_paths || !(*f_paths) || f_paths_cnt == 0) {
        return;
    }

    for (f = 0; f < f_paths_cnt; f++) {
        free((*f_paths)[f]);
    }
    free(*f_paths);
    *f_paths = NULL;
}


/**
 * \brief f_path_make Prints the file path to the provided buffer.
 * \param dest Buffer.
 * \param dir Directory.
 * \param pattern File pattern.
 * \param number File number.
 * \param suffix File suffix.
 * \return 1 if operation was successful, else 0.
 */
int f_path_make(char *dest, const char dir[], const char pattern[], const size_t number, const char suffix[]) {
    if (!dest || !pattern || number <= 0) {
        return 0;
    }
    
    if (dir) {
        if (sprintf(dest, "%s/%s%lu%s", dir, pattern, number, suffix) <= 0) {
            return 0;
        }
    }
    else {
        if (sprintf(dest, "%s%lu%s", pattern, number, suffix) <= 0) {
            return 0;
        }
    }

    return 1;
}


/**
 * \brief f_paths_create Creates an array of file paths constructed from provided arguments.
 * \param dir Directory.
 * \param f_patterns Array of file patterns.
 * \param f_patterns_cnt Number of file patterns.
 * \param f_counts Array of numbers of file paths to be created for a pattern.
 * \param suffix File suffix.
 * \return Array of constructed file paths.
 */
char **f_paths_create(const char dir[], const char *f_patterns[], const size_t f_patterns_cnt, const size_t f_counts[], const char suffix[]) {
    char **f_paths = NULL;
    char *f_path = NULL;
    size_t f_path_len;
    size_t f_count;
    size_t p;
    size_t f;
    size_t f_offset;

    if (!f_patterns || f_patterns_cnt == 0 || !f_counts) {
        return NULL;
    }

    f_count = 0;
    for (p = 0; p < f_patterns_cnt; p++) {
        f_count += f_counts[p];
    }
    f_paths = (char **) calloc(f_count, sizeof(char *));
    if (!f_paths) {
        return NULL;
    } 

    f_offset = 0;
    for (p = 0; p < f_patterns_cnt; p++) {
        f_path_len = (dir ? strlen(dir) : 0) +
                     strlen(f_patterns[p]) + (int) (ceil(log10(f_counts[p] + 1)) + 1) +
                     (suffix ? strlen(suffix) : 0);
        
        f_path = calloc(f_path_len + 1, sizeof(char));
        for (f = 0; f < f_counts[p]; f++) {
            if (!f_path_make(f_path, dir, f_patterns[p], f + 1, suffix)) {
                free(f_path);
                f_paths_free(&f_paths, f_offset + f);
                return NULL;
            }
            f_paths[f + f_offset] = strdup(f_path);
        }

        free(f_path);
        f_offset += f;
    }

    return f_paths;
}


/**
 * \brief load_args Loads program input arguments.
 * \param argc Program input arguments count.
 * \param argv Program input arguments values.
 * \param f_learn_patterns Array of file patterns of files to be learnt.
 * \param f_learn_counts Array of numbers of files of a pattern.
 * \param f_classify_pattern Pointer to a classify pattern.
 * \param f_classify_cnt Pointer to a number of files to be classified.
 * \param f_out Pointer to a classification result file path.
 * \return 1 if all program arguments are provided and valid, else 0.
 */
int load_args(int argc, char **argv,
              char *f_learn_patterns[], size_t f_learn_counts[],
              char **f_classify_pattern, size_t *f_classify_cnt,
              char **f_out) {
    if (argc != REQUIRED_ARGS_CNT + 1) {
        return 0;
    }
    if (!is_valid_count(argv[2]) || !is_valid_count(argv[4]) || !is_valid_count(argv[6])) {
        return 0;
    }

    f_learn_patterns[SPAM] = argv[1];
    f_learn_counts[SPAM] = atoi(argv[2]);
    f_learn_patterns[HAM] = argv[3];
    f_learn_counts[HAM] = atoi(argv[4]);
    *f_classify_pattern = argv[5];
    *f_classify_cnt = atoi(argv[6]);
    *f_out = argv[7];

    return 1;
}


/**
 * \brief unload_f_paths Releases the memory held by the arrays of file paths.
 *                       Does not check argument validity.
 * \param f_learn_paths Pointer to an array of file paths to be learnt.
 * \param f_learn_counts Array of numbers of files of a pattern.
 * \param f_classify_paths Pointer to an array of file paths to be classified.
 * \param f_classify_names Pointer to an array of file names (file name without dir prefix) to be printed to the output file.
 * \param f_classify_cnt Number of classified files.
 */
void unload_f_paths(char **f_learn_paths[], const size_t f_learn_counts[],
                    char **f_classify_paths[], char **f_classify_names[], const size_t f_classify_cnt) {
    f_paths_free(f_learn_paths, f_learn_counts[SPAM] + f_learn_counts[HAM]);
    f_paths_free(f_classify_paths, f_classify_cnt);
    f_paths_free(f_classify_names, f_classify_cnt);
    *f_learn_paths = *f_classify_paths = *f_classify_names = NULL;
}


/**
 * \brief load_f_paths Creates and loads arrays of file paths.
 *                     Does not check argument validity.
 * \param f_learn_paths Pointer to an array of file paths to be learnt.
 * \param f_learn_patterns Array of patterns of files to be learnt.
 * \param f_learn_counts Array of numbers of files of a pattern.
 * \param f_classify_paths Pointer to an array of file paths to be classified.
 * \param f_classify_names Pointer to an array of file names (file name without dir prefix) to be printed to the output file.
 * \param f_classify_pattern Pattern of the classified files.
 * \param f_classify_cnt Number of classified files.
 * \return 1 if operation was successful, else 0.
 */
int load_f_paths(char **f_learn_paths[],  const char *f_learn_patterns[], const size_t f_learn_counts[],
                 char **f_classify_paths[], char **f_classify_names[], const char f_classify_pattern[], const size_t f_classify_cnt) {
    *f_learn_paths = f_paths_create(DATA_DIR, f_learn_patterns, CLASSIFIER_CLS_CNT, f_learn_counts, FILE_SUFFIX);
    *f_classify_paths = f_paths_create(DATA_DIR, &f_classify_pattern, 1, &f_classify_cnt, FILE_SUFFIX);
    *f_classify_names = f_paths_create(NULL, &f_classify_pattern, 1, &f_classify_cnt, FILE_SUFFIX);
    if (!f_learn_paths || !f_classify_paths || !f_classify_names) {
        unload_f_paths(f_learn_paths, f_learn_counts, f_classify_paths, f_classify_names, f_classify_cnt);
        return 0;
    }

    return 1;
}


/**
 * \brief process Teaches the classifier provided files, classifies provided files
 *                and outputs the result into provided output file.
 * \param f_learn_paths Array of file paths to be learnt.
 * \param f_learn_counts Array of numbers of file paths to be learnt.
 * \param f_classify_paths Array of file paths to be classified.
 * \param f_classify_names Array of names (file name without dir prefix) to be printed to the output file.
 * \param f_classify_cnt Number of files to be classified.
 * \param f_out Output file path.
 * \return 1 if operation was successful, else 0.
 */
int process(const char *f_learn_paths[], const size_t f_learn_counts[],
            const char *f_classify_paths[], const char *f_classify_names[], const size_t f_classify_cnt,
            const char *f_out) {
    nbc *cl = NULL;
    FILE *fp = NULL;
    size_t f;
    int cls;

    if (!f_learn_paths || !f_learn_counts || !f_classify_paths || !f_classify_cnt || !f_out) {
        return 0;
    }

    cl = nbc_create(CLASSIFIER_CLS_CNT);
    if (!cl || !nbc_learn(cl, f_learn_paths, f_learn_counts)) {
        goto fail;
    }

    fp = fopen(f_out, "w");
    if (!fp) {
        goto fail;
    }
    for (f = 0; f < f_classify_cnt; f++) {
        cls = nbc_classify(cl, f_classify_paths[f]);
        if (cls == -1) {
            goto fail;
        }
        
        fprintf(fp, RESULT_LINE_FORMAT, f_classify_names[f], RESULT_CLASS_DESCRIPTION[cls]);
    }

    if (fclose(fp) == EOF) {
        fp = NULL;
        goto fail;
    }
    nbc_free(&cl);

    return 1;

fail:
    if (fp) {
        fclose(fp);
    }
    nbc_free(&cl);
    return 0;
}


/**
 * \brief main Processes input arguments, teaches classifier provided files,
 *             classifies provided files, outputs results into provided file.
 * \param argc Program input arguments count.
 * \param argv Program input arguments values.
 * \return EXIT_SUCCESS if not any problem occured, else EXIT_FAILURE.
 */
int main(int argc, char **argv) {
    char *f_learn_patterns[CLASSIFIER_CLS_CNT] = {NULL};
    char *f_classify_pattern = NULL;
    size_t f_learn_counts[CLASSIFIER_CLS_CNT] = {0};
    size_t f_classify_cnt = 0;
    char **f_learn_paths = NULL, **f_classify_paths = NULL, **f_classify_names = NULL;
    char *f_out = NULL;

    if (!load_args(argc, argv, f_learn_patterns, f_learn_counts, &f_classify_pattern, &f_classify_cnt, &f_out)) {
        print_err("Invalid arguments count/values.");
        printf("\n");
        print_man();
        return EXIT_FAILURE;
    }

    if (!load_f_paths(&f_learn_paths, (const char **) f_learn_patterns, f_learn_counts, &f_classify_paths, &f_classify_names, f_classify_pattern, f_classify_cnt)) {
        goto fail;
    }

    if (!process((const char **) f_learn_paths, f_learn_counts, (const char **) f_classify_paths, (const char **) f_classify_names, f_classify_cnt, f_out)) {
        goto fail;
    }

    unload_f_paths(&f_learn_paths, f_learn_counts, &f_classify_paths, &f_classify_names, f_classify_cnt);
    return EXIT_SUCCESS;

fail:
    print_err("Unexpected error occured during program execution.");
    unload_f_paths(&f_learn_paths, f_learn_counts, &f_classify_paths, &f_classify_names, f_classify_cnt);
    return EXIT_FAILURE;
}
