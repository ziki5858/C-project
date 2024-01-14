#include <stdio.h>
#include <string.h>
#include "ctype.h"
#include "front.h"
#include "frontMethodDeclarstion.h"

/* Set to store entry labels */
struct LabelSet {
    char labels[MAX_LABEL_SIZE][MAX_LINE_SIZE];
    int count;
};

/* Initialize the label set */
struct LabelSet entryLabelSet = { .count = 0 };

/* Function to handle the formatting of directives in the assembly code */
int directiveFormat(FILE *file, char *word, struct pattern *data, struct Node **head) {
    if (isValidLabel(word)) {
        fscanf(file, "%49s", word);
        if (strcmp(word, ".string") == 0) {
            return handleStringDirective(file, data, head);
        } else if (strcmp(word, ".data") == 0) {
            return handleDataDirective(file, data, head);
        }
    }

    /* Separate cases for other directives */
    if (strcmp(word, ".entry") == 0) {
        num_of_entries++;
        data->dir.directive_type = ENTRY;

        fscanf(file, "%49s", word);
        if (isValidLabel(word)) {
            strcpy(data->label, word);
            insertNode(head, *data);
            return 1;
        } else {
            isError(data, "Error: Invalid label name", head);
            return 0;
        }
    } else if (strcmp(word, ".extern") == 0) {
        num_of_externals++;
        data->dir.directive_type = EXTERN;

        handleEntryDirective(file, data, head);
    } else if (strcmp(word, ".string") == 0) {
        return handleStringDirective(file, data, head);
    } else if (strcmp(word, ".data") == 0) {
        return handleDataDirective(file, data, head);
    }
    return 0;  /* Not a directive */
}

/* Function to handle the .string directive */
int handleStringDirective(FILE *file, struct pattern *data, struct Node **head) {
    data->dir.directive_type = STRING;
    insertNode(head, *data);
    fscanf(file, "%49s", data->label);
    data->dir.string = (char *) strdup(data->label);
    insertNode(head, *data);
    data->dir.size = countChars(data->label);
    insertNode(head, *data);
    return 1;
}

/* Function to handle the .data directive */
int handleDataDirective(FILE *file, struct pattern *data, struct Node **head) {
    char input[100];
    int size;
    data->dir.directive_type = DATA;
    insertNode(head, *data);

    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    if (checkLastCharacter(input, ',') != 0) {
        isError(data, "Error: Extraneous text after end of command", head);
        return 0;
    }

    if (!miss(0)) {
        isError(data, "Error: missing arguments.", head);
        return 0;
    }

    size = processNumericArguments(input, data, head);
    if (size == -1) {
        return 0;
    }

    return 1;
}

/* Function to process numeric arguments in the .data directive */
int processNumericArguments(char *input, struct pattern *data, struct Node **head) {
    int size = 0;
    char *token = strtok(input, "[^,]");

    /* Allocate memory for the array of strings */
    data->dir.data = (char **)calloc(size, sizeof(char *));

    while (token != NULL) {
        if (*token == '\0') {
            isError(data, "Error: Missing argument.", head);
            return 0;
        }

        if (!isNumeric(token)) {
            isError(data, "Error: Argument is not a real number", head);
            return 0;
        }

        /* Allocate memory for the current string and copy the token */
        data->dir.data[size] = strdup(token);

        /* Check if memory allocation was successful */
        if (data->dir.data[size] == NULL) {
            isError(data, "Error: Memory allocation failed", head);
            /* Clean up previously allocated strings */
            for (int i = 0; i < size; i++) {
                free(data->dir.data[i]);
            }
            /* Free the array itself */
            free(data->dir.data);
            return 0;  // Indicate error
        }

        size++;
        /* Resize the array of strings */
        data->dir.data = realloc(data->dir.data, size * sizeof(char *));
        if (data->dir.data == NULL) {
            isError(data, "Error: Memory allocation failed", head);
            /* Clean up previously allocated strings */
            for (int i = 0; i < size - 1; i++) {
                free(data->dir.data[i]);
            }
            /* Free the array itself */
            free(data->dir.data);
            return 0; // Indicate error
        }

        token = strtok(NULL, "[^,]");
    }
    insertNode(head, *data);
    data->dir.size = size;
    insertNode(head, *data);
    return 1;
}

/* Function to check if the last character of a string is a specific character */
int checkLastCharacter(const char input[], char errorChar) {
    /* Check if the last character is a comma */
    size_t length = strlen(input);

    if (length > 0) {
        char lastChar = input[length - 1];
        if (lastChar == errorChar) {
            return 0;
        }
    }
    return 1;
}

/* Function to handle missing arguments in the .data directive */
int miss(int requireComma) {
    int missing;
    missing = getchar();

    if (missing == '\n') {
        return 0; /* No extraneous text or missing comma */
    }

    if (requireComma && missing != ',') {
        /* Handle extraneous text (clear the input buffer) */
        while (getchar() != '\n');
        return 0;
    }

    return 1;
}

/* Function to count characters in a string */
int countChars(const char *str) {
    int count = 1; /* For the \0 at the end of the string by task instruction */
    /* Iterate through each character until the null terminator is reached */
    while (*str != '\0') {
        count++;
        str++;
    }
    return count;
}

/* Function to check if a label is valid */
int isValidLabel(const char *name) {
    /* Check if the name is not empty */
    if (*name == '\0') {
        return 0; // Invalid: Empty name
    }

    /* Check if the first character is a letter */
    if (!isalpha(*name)) {
        return 0;
    }

    /* Check the remaining characters */
    name++;

    /* Check for alphanumeric or underscore, up to 30 characters */
    int count = 1;
    while (*name != '\0' && count <= 30) {
        if (!isalnum(*name) && *name != '_') {
            return 0;
        }
        name++;
        count++;
    }

    /* Check if the last character is ':' */
    if (*(name - 1) != ':') {
        return 0;
    }

    return 1;
}

/* Function to add a label to the set */
void addToEntryLabelSet(const char *label) {
    if (entryLabelSet.count < MAX_LABEL_SIZE) {
        strcpy(entryLabelSet.labels[entryLabelSet.count], label);
        entryLabelSet.count++;
    }
}

/* Function to check if a label is in the set */
int isEntryLabel(const char *label) {
    for (int i = 0; i < entryLabelSet.count; i++) {
        if (strcmp(entryLabelSet.labels[i], label) == 0) {
            return 1; // Found in the set
        }
    }
    return 0; // Not found in the set
}

/* Function to handle the .entry directive */
int handleEntryDirective(FILE *file, struct pattern *data, struct Node **head) {
    fscanf(file, "%49s", data->label);

    /* Check if the label is already used as .entry */
    if (isEntryLabel(data->label)) {
        isError(data, "Error: Label already used as .entry", head);
        return 0;
    }

    /* Check if the label is valid */
    if (isValidLabel(data->label)) {
        strcpy(data->label, data->label);
        insertNode(head, *data);

        /* Add the label to the entry label set */
        addToEntryLabelSet(data->label);

        return 1;
    } else {
        isError(data, "Error: Invalid label name", head);
        return 0;
    }
}