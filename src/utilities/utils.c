/**
 * \file utils.c
 * \brief Functions declared in utils.h are implemented in this file.
 * \version 1, 30-12-2022
 * \author Stanislav Kafara, skafara@students.zcu.cz
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"


char *strdup(const char *str) {
    char *new_str = NULL;

    new_str = (char *) malloc(strlen(str) + 1);
    if (!new_str) {
        return NULL;
    }
    strcpy(new_str, str);

    return new_str;
}
