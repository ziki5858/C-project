/*
 * ----------------------------------------------------------------------------
 * File: define.h
 * Author: Ishay Abakiyev - ziki, Shlomo Wsisz
 * Date: 28/01/2024
 * Description: This file contains functions related to processing the .define directive
 *              in assembly language code. It includes a function to handle the
 *              .define directive and auxiliary functions for validation.
 * 
 * Functions:
 *   - defineFormat: Processes the .define directive in the assembly code, updating the data structure.
 *   - isNumeric: Checks if a string represents a numeric value.
 *   - isValidConstantName: Checks if a string is a valid constant name.
 * ----------------------------------------------------------------------------
 */

#include "headeMethods.h"

int defineFormat(FILE *file, char *word, struct pattern *data, struct Node **head) {
    if (!fscanf(file, "%s", word)) {
        isError(file,word, data, "Error: Unable to read constant name", "define.h", head);
        return false;
    }

    if (isValidConstantName(word)) {
        strcpy(data->label, word);
        num_of_constants++;
        addToEntryLabelSet(word);
    } else {
        isError(file,word, data, "Error: Invalid constant name", "define.h", head);
        return false;
    }

    if (!fscanf(file, "%s", word) || strcmp(word, "=") != 0) {
        isError(file,word, data, "Error: Invalid symbol, expected =", "define.h", head);
        return false;
    }

    if (!fscanf(file, "%s", word) || !isNumeric(word)) {
        isError(file,word, data, "Error: Invalid numeric value", "define.h", head);
        return false;
    }

    data->choice.def.value = atoi(word);
    return true;
}

int isNumeric(char *str) {
    if (str == NULL || *str == '\0') {
        return 0;  /* Empty string or NULL is not numeric */
    }

    while (*str) {
        if (!isdigit((unsigned char) *str) && *str != '.' && *str != '-') {
            return false;  /* Non-numeric character found */
        }
        str++;
    }

    return true;  /* All characters are numeric */
}

/* Function to check if a string is a valid constant name */
int isValidConstantName( const char *name) {
    /* Check if the name is not empty */
    if (*name == '\0') {
        return false; /* Invalid: Empty name */
    }
    /* Check if the first character is a letter */
    if (!isalpha(*name)) {
        return false; /* Invalid: Name must start with a letter */
    }
    /* Check the remaining characters */
    while (*name != '\0') {
        /* Check if each character is alphanumeric or underscore */
        if (!isalnum(*name) && *name != '_') {
            return false; /* Invalid: Name must be alphanumeric or underscore */
        }
        name++;
    }
    return true; /* Valid constant name */
}
