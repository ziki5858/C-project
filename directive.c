/*
 * ----------------------------------------------------------------------------
 * File: directive.c
 * Author: Ishay Abakiyev - ziki, Shlomo Wsisz
 * Date: 28/01/2024
 * Description: This file contains functions related to processing assembly language
 *              code. It includes functions for handling labels, directives, and
 *              numeric arguments, as well as updating a linked list data structure.
 *
 * Structures:
 *   - struct LabelSet: Set to store entry labels.
 *
 * Functions:
 *   - isValidLabel: Checks if a label is valid based on certain criteria.
 *   - directiveFormat: Processes various assembly language directives.
 *   - handleEntryDirective: Handles the .entry directive.
 *   - addToEntryLabelSet: Adds a label to the entry label set.
 *   - isEntryLabel: Checks if a label is present in the entry label set.
 *   - handleExternDirective: Handles the .extern directive.
 *   - handleStringDirective: Handles the .string directive.
 *   - handleDataDirective: Handles the .data directive.
 *   - checkCommaWord: Checks for a missing comma at the end of a line.
 *   - checkCommaAtEnd: Checks for a comma at the end of a line.
 *   - processNumericArguments: Processes numeric arguments.
 *   - checkLastCharacter: Checks the last character in a string.
 *   - countChars: Counts the number of characters in a string.
 * ----------------------------------------------------------------------------
 */

#include "headeMethods.h"

/* Set to store entry labels */
struct LabelSet {
    char labels[MAX_LABEL_SIZE][MAX_LINE_SIZE];
    int count;
};

/* Initialize the label set */
struct LabelSet entryLabelSet = { 0 };

int isValidLabel(char *name, struct pattern *data, int needColon, int symbol) {
    int count = 0;   /* Check for alphanumeric or underscore, up to MAX_LABEL_SIZE characters */
    char *lastChar = name + strlen(name) - 1;

    /* Check if the name is not empty */
    if (*name == '\0') {
        return false; 
    }

    /* Check if the first character is a letter */
    if (!isalpha(*name)) {
        return false;
    }

    /* Remove the last character (colon) if not needed */
    if (needColon != 1 && *lastChar == ':') {
        *lastChar = '\0';  /* Remove the last character (colon) from the name */
    } else if (needColon != 1) {
        return false;  /* Labels without a colon must end with ':' */
    }

    while (*name != '\0' && count < MAX_LABEL_SIZE) {
        if (!isalnum(*name) && *name != '_' && !isupper(*name)) {
            return false;  /* Invalid character in the label */
        }
        name++;
        count++;
    }
    return true;  /* The label is valid */
}

int directiveFormat(FILE *file, char *word, struct pattern *data, struct Node **head) {
    if (isValidLabel(word,data,0,0)) {  /* Check if the provided word is a valid label*/
        strcpy(data->label, word);
        fscanf(file, "%s", word);
    }
    /* Separate cases for other directives */
    if (strcmp(word, ".entry") == 0) {
        return handleEntryDirective(file, word, data, head);
    }  if (strcmp(word, ".extern") == 0) {
        return handleExternDirective(file, data, head);
    }  if (strcmp(word, ".string") == 0) {
        return handleStringDirective(file, data);
    }  if (strcmp(word, ".data") == 0) {
        return handleDataDirective(file,word, data, head);
    }
    return false;  /* Not a directive */
}

int handleEntryDirective(FILE *file, char *word, struct pattern *data, struct Node **head) {
    fscanf(file, "%s", word);
    if (isValidLabel(word, data, 1, 0)) {
        strcpy(data->label, word);
        num_of_entries++;
        data->choice.dir.directive_type = ENTRY;
        return true;
    } 

    isError(data, "Error: Invalid label name at .entry", "directive.h", head);
    return true_inValid;  /*means that it is a directive type but arguments are not valid*/
}

void addToEntryLabelSet(const char *label) {
    strcpy(entryLabelSet.labels[entryLabelSet.count], label);
    entryLabelSet.count++;
}

int isEntryLabel(const char *label) {
    for (int i = 0; i < entryLabelSet.count; i++) {
        if (strcmp(entryLabelSet.labels[i], label) == 0) {
            return true; /*Found in the set*/ 
        }
    }
    return false; /* Not found in the set*/
}

int handleExternDirective(FILE *file, struct pattern *data, struct Node **head) {
    data->choice.dir.directive_type = EXTERN;
    char tempLabel[MAX_LABEL_SIZE];

    if (fscanf(file, "%s", tempLabel) != 1) {
        /* Handle fscanf failure */
        isError(data, "Error: Unable to read label from file", "directive.h", head);
        return true_inValid;
    }
    /* Check if the label is already used as .entry */
    if (isEntryLabel(tempLabel)) {
        isError(data, "Error: Label already used as .entry", "directive.h", head);
        return true_inValid; /* case -1 means that it is a directive type but arguments are not valid */
    }

    if (isValidLabel(tempLabel, data, 1, 0)) {
        strcpy(data->label, tempLabel);
        /* Add the label to the entry label set */
        addToEntryLabelSet(data->label);
        num_of_externals++;
        return true;
    }

    /* Invalid label name */
    isError(data, "Error: Invalid label name", "directive.h", head);
    return true_inValid;
}

int handleStringDirective(FILE *file, struct pattern *data) {
    char word[MAX_LINE_SIZE];
    data->choice.dir.directive_type = STRING;

    fscanf(file, "%s", word);
    /* Allocate memory for the string and copy the word into it */
    data->choice.dir.string = (char *) strdup(word);
    /* Set the size of the string based on the character count */
    data->choice.dir.size = countChars(word);
    /* Free the memory allocated for the string (consider removing this line if not intended) */
    free(data->choice.dir.string);

    return true;
}

int handleDataDirective(FILE *file, char *word, struct pattern *data, struct Node **head) {
    char *token;
    data->choice.dir.directive_type = DATA;
    fscanf(file, "%s", word);

    if (checkCommaAtEnd(file, word, data, head)){/*Comma at the end of line*/
        return true_inValid;
    }

    /* Tokenize the word to process numeric arguments */
    token = strtok(word, " ,");

    while (token != NULL) {

        /* Process numeric arguments based on the token, Check if the next character is a newline character */
        if (!processNumericArguments(token, word, data, head)||getc(file) == '\n') {
            return true_inValid;
        }

        /* Move the file pointer back to the start of the next word */
        fseek(file, ftell(file) - 1, SEEK_SET);
        /* Read the next word from the file */
        fscanf(file, "%s", word);

        if (!checkCommaWord(file, word, data, head)) {
            return true_inValid;  
        }
        token = strtok(word, " ,");
    }

    /* Free allocated memory for data's .data field */
    free(data->choice.dir.data);
    return true;
}

int checkCommaWord(FILE *file, char *word, struct pattern *data, struct Node **head) {
    int len = strlen(word);

    if (len > 0 && word[len - 1] != ',') {
        /* Check if the next character is a newline character */
        if (getc(file) == '\n') {
            return true;  // Return true if the newline character is found
        }

        /* Display an error and return false if there is a missing comma */
        isError(data, "Error: Missing comma", "directive.h", head);
        return false;
    }

    return true;  // Return true if the last character is a comma
}

int checkCommaAtEnd(FILE *file, char *word, struct pattern *data, struct Node **head) {
    int len = strlen(word);

    if (getc(file) != '\n') {
        /* Check if the last character of the word is a comma */
        if (len > 0 && word[len - 1] != ',') {
            isError(data, "Error: Comma is not allowed at the end of line", "directive.h", head);
            return true;  
        }

        /* Move the file pointer back to the start of the word */
        fseek(file, ftell(file) - 1, SEEK_SET);
    }

    return false; 
}

int processNumericArguments(char *input, char *word, struct pattern *data, struct Node **head) {
    data->choice.dir.data = NULL;

    if (word == NULL) {
        isError(data, "Error: No numeric arguments found", "directive.h", head);
        return false;
    }

    /* Check if the token is a valid number or constant */
    if (!isNumeric(word) && !isEntryLabel(word)) {
        isError(data, "Error: Argument is not a real number", "directive.h", head);
        return false;
    }

    /* Resize the array of strings */
    char **temp = realloc(data->choice.dir.data, (data->choice.dir.size + 1) * sizeof(char *));
    if (temp == NULL) {
        isError(data, "Error: Memory allocation failed", "directive.h", head);
        /* Free the array itself */
        free(data->choice.dir.data);
        return false; 
    }
    data->choice.dir.data = temp;

    /* Allocate memory for the new string and copy the content of word */
    data->choice.dir.data[data->choice.dir.size] = strdup(word);
    if (data->choice.dir.data[data->choice.dir.size] == NULL) {
        isError(data, "Error: Memory allocation failed", "directive.h", head);
        free(data->choice.dir.data);
        return false; 
    }
    free(data->choice.dir.data[data->choice.dir.size]);

    data->choice.dir.size++;
    return true;
}

int checkLastCharacter(const char input[], char errorChar) {
    /* Check if the last character is a comma */
    size_t length = strlen(input);

    if (length > 0) {
        char lastChar = input[length - 1];
        if (lastChar == errorChar) {
            return false;
        }
    }
    return true;/* Last character is not a comma */
}


int countChars(const char *str) {
    int count = 0; /* Initialize count to 0 for correct character count */
    /* Iterate through each character until the null terminator is reached */
    while (*str != '\0') {
        count++;
        str++;
    }
    return count;
}


