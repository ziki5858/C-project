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
    if (isValidLabel(word,data,0)) {
        fscanf(file, "%s", word);
        if (strcmp(word, ".string") == 0) {
            return handleStringDirective(file, data);
        } else if (strcmp(word, ".data") == 0) {
            return handleDataDirective(file,word, data, head);
        }
    }

    /* Separate cases for other directives */
    if (strcmp(word, ".entry") == 0) {
        num_of_entries++;
        data->choice.dir.directive_type = ENTRY;

        fscanf(file, "%s", word);
        if (isValidLabel(word,data,1)) {
            addToEntryLabelSet(data->label);
            return 1;
        } else {
            isError(data, "Error: Invalid label name", head);
            return 0;
        }

    } else if (strcmp(word, ".extern") == 0) {
        num_of_externals++;
        data->choice.dir.directive_type = EXTERN;
       return handleEntryDirective(file, data, head);

    } else if (strcmp(word, ".string") == 0) {
        return handleStringDirective(file, data);
    } else if (strcmp(word, ".data") == 0) {
        return handleDataDirective(file,word, data, head);
    }
    return 0;  /* Not a directive */
}

/* Function to handle the .string directive */
int handleStringDirective(FILE *file, struct pattern *data) {
    char word[MAX_LINE_SIZE];
    data->choice.dir.directive_type = STRING;
    fscanf(file, "%s", word);
    data->choice.dir.string = (char *) strdup(word);
    data->choice.dir.size = countChars(word);
    return 1;
}

/* Function to handle the .data directive */
int handleDataDirective(FILE *file, char *word, struct pattern *data, struct Node **head) {
    char *token;
    data->choice.dir.directive_type = DATA;

    fscanf(file, "%s", word);
    int len = strlen(word);

    if(getc(file)!='\n'){
        if (len > 0 && word[len - 1] != ',') {
            isError(data, "Error: Missing comma", head);
            return 0;
        }
    }
    fseek(file, ftell(file) - 1, SEEK_SET);



    token = strtok(word, " ,");

    while (token != NULL) {
        if (checkLastCharacter(token, ',') == 0) {
            isError(data, "Error: Extraneous text after end of command", head);
            return 0;
        }

        if (!processNumericArguments(token,word, data, head)) {
            return 0;
        } else{
            if(getc(file)=='\n')
                break;
            else
                fseek(file, ftell(file) - 1, SEEK_SET);
        }

        fscanf(file, "%s", word);
        int len = strlen(word);
        if (len > 0 && word[len - 1] != ',') {
            if(getc(file)=='\n')
                break;
            isError(data, "Error: Missing comma", head);
            return 0;
        }
        token = strtok(word, " ,");
    }

    return 1;
}

/* Function to process numeric arguments in the .data directive */
int processNumericArguments(char *input, char *word, struct pattern *data, struct Node **head) {

    /* Allocate memory for the array of strings */
    data->choice.dir.data = NULL;

    if (word == NULL) {
        isError(data, "Error: No numeric arguments found", head);
        return 0;
    }

    if (*word == '\0') {
        isError(data, "Error: Missing argument.", head);
        return 0;
    }

    /* Check if the token is a valid number or constant*/
    if (!isNumeric(word) && isEntryLabel(word)) {
        isError(data, "Error: Argument is not a real number", head);
        return 0;
    }

    /* Resize the array of strings */
    char **temp = realloc(data->choice.dir.data, ( data->choice.dir.size + 1) * sizeof(char *));
    if (temp == NULL) {
        isError(data, "Error: Memory allocation failed", head);
        /* Clean up previously allocated strings */
        for (int i = 0; i <  data->choice.dir.size; i++) {
            free(data->choice.dir.data[i]);
        }
        /* Free the array itself */
        free(data->choice.dir.data);
        return 0; // Indicate error
    }
    data->choice.dir.data = temp;

    /* Allocate memory for the current string and copy the token */
    data->choice.dir.data[ data->choice.dir.size] = strdup(word);

    /* Check if memory allocation was successful */
    if (data->choice.dir.data[ data->choice.dir.size] == NULL) {
        isError(data, "Error: Memory allocation failed", head);
        /* Clean up previously allocated strings */
        for (int i = 0; i <=  data->choice.dir.size; i++) {
            free(data->choice.dir.data[i]);
        }
        /* Free the array itself */
        free(data->choice.dir.data);
        return 0;  // Indicate error
    }
    data->choice.dir.size =  data->choice.dir.size++;

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
    return 1;/* Last character is not a comma */
}

/* Function to handle missing arguments in the .data directive */
int miss(int requireComma, FILE *file) {
    int missing;
    missing = fgetc(file);

    if (missing == '\n') {
        return 0; /* No extraneous text or missing comma */
    }

    if (requireComma && missing != ',') {
        /* Handle extraneous text (clear the input buffer) */
        while (fgetc(file) != '\n');
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

int isValidLabel(char *name, struct pattern *data, int needColon) {
    /* Check if the name is not empty */
    if (*name == '\0') {
        return 0; // Invalid: Empty name
    }

    /* Check if the first character is a letter */
    if (!isalpha(*name)) {
        return 0;
    }

    /* Check for alphanumeric or underscore, up to 30 characters */
    int count = 0;
    char *lastChar = name + strlen(name) - 1;

    if(needColon!=1) {
        /* Check if the last character is ':'*/
        if (*lastChar == ':') {
            *lastChar = '\0'; // Remove the last character
        } else {
            return 0;
        }
    }

    while (*name != '\0' && count <= MAX_LABEL_SIZE) {
        if (!isalnum(*name) && *name != '_'&&!isupper(*name)) {
            return 0;
        }
        name++;
        count++;
    }

    // Copy the modified label to the data structure
    strcpy(data->label, lastChar);
    num_of_symbol++;
    return 1;
}



/* Function to add a label to the set */
void addToEntryLabelSet(const char *label) {
        strcpy(entryLabelSet.labels[entryLabelSet.count], label);
        entryLabelSet.count++;
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

int handleEntryDirective(FILE *file, struct pattern *data, struct Node **head) {
    char tempLabel[MAX_LABEL_SIZE];

    if (fscanf(file, "%s", tempLabel) == 1) {
        /* Check if the label is already used as .entry */
        if (isEntryLabel(tempLabel)) {
            isError(data, "Error: Label already used as .entry", head);
            return 0;
        }

        /* Check if the label is valid */
        if (isValidLabel(tempLabel,data,1)) {
            strcpy(tempLabel, data->label);
            num_of_symbol++;
            /* Add the label to the entry label set */
            addToEntryLabelSet(data->label);
            return 1;
        } else {
            isError(data, "Error: Invalid label name", head);
            return 0;
        }
    } else {
        /* Handle fscanf failure */
        isError(data, "Error: Unable to read label from file", head);
        return 0;
    }
}