#include "headeMethods.h"

int defineFormat(FILE *file, char *word, struct pattern *data, struct Node **head) {
    fscanf(file, "%s", word);
    if (isValidConstantName(word)) {
        strcpy(data->label, word);
        num_of_constants++;
    } else {
        isError(data, "Error: Invalid constant name","define.h",head);
        return true_inValid;
    }

    fscanf(file, "%s", word);
    if(strcmp(word,"=")!=0){
        isError(data, "Error: Invalid symbol, need to be =","define.h",head);
        return true_inValid;
    }

    fscanf(file, "%s", word);
    if (!isNumeric(word)) {
        isError(data, "Invalid numeric value","define.h", head);
        return true_inValid;
    }
    data->choice.def.value = atoi(word);
    return true;
}

int isNumeric(char *str) {
    int length = strlen(str);
    int startIndex = 0;
    int i;

    if (str == NULL || *str == '\0') {
        return false; /* Not numeric if the string is empty or NULL*/
    }

    while (str[startIndex] == ' ' && startIndex < length) {
        startIndex++;
    }
    /* Check for an optional sign at the beginning*/
    if (str[startIndex] == '-' || str[startIndex] == '+') {
        startIndex ++;
    }

    for (i = startIndex; i < length; i++) {
        if (!isdigit(str[i])) {
            return false; /* Non-digit character found*/
        }
    }

    return true;
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
