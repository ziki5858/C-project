#include <stdio.h>
#include <string.h>
#include "ctype.h"
#include "front.h"

int defineFormat(FILE *file, char *word, struct pattern *data, struct Node **head) {
    fscanf(file, "%s", word);
    if (isValidConstantName(word)) {
        strcpy(data->label, word);
        num_of_symbol++;
    } else {
        isError(data, "Error: Invalid constant name",head);
        return 0;
    }

    fscanf(file, "%s", word);
    if(strcmp(word,"=")!=0){
        isError(data, "Error: Invalid symbol, need to be =",head);
        return 0;
    }

    fscanf(file, "%s", word);
    if (isNumeric(word)) {
        data->choice.def.value = atoi(word);
    } else {
        isError(data, "Invalid numeric value",head);
        return 0;
    }

    return 1;
}

/* for .data 6,-8
 * think 6 is " 6" with space therFore not numeric*/
int isNumeric(char *str) {
    if (str == NULL || *str == '\0') {
        return 0;  /* Empty string or NULL is not numeric */
    }

    while (*str) {
        if (!isdigit((unsigned char) *str) && *str != '.' && *str != '-') {
            return 0;  /* Non-numeric character found */
        }
        str++;
    }

    return 1;  /* All characters are numeric */
}


/* Function to check if a string is a valid constant name */
int isValidConstantName( const char *name) {
    /* Check if the name is not empty */
    if (*name == '\0') {
        return 0; /* Invalid: Empty name */
    }
    /* Check if the first character is a letter */
    if (!isalpha(*name)) {
        return 0; /* Invalid: Name must start with a letter */
    }
    /* Check the remaining characters */
    while (*name != '\0') {
        /* Check if each character is alphanumeric or underscore */
        if (!isalnum(*name) && *name != '_') {
            return 0; /* Invalid: Name must be alphanumeric or underscore */
        }
        name++;
    }
    return 1; /* Valid constant name */
}
