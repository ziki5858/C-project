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
    if (!isNumeric(word)) {
        isError(data, "Invalid numeric value", head);
        return 0;
    }
    data->choice.def.value = atoi(word);
    return 1;
}

int isNumeric(char *str) {
    if (str == NULL || *str == '\0') {
        return 0; /* Not numeric if the string is empty or NULL*/
    }

    int length = strlen(str);
    int startIndex = 0;

    while (str[startIndex] == ' ' && startIndex < length) {
        startIndex++;
    }
    /* Check for an optional sign at the beginning*/
    if (str[startIndex] == '-' || str[startIndex] == '+') {
        startIndex ++;
    }

    for (int i = startIndex; i < length; i++) {
        if (!isdigit(str[i])) {
            return 0; /* Non-digit character found*/
        }
    }

    return 1;
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
