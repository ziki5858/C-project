#include "headeMethods.h"

/* Set to store entry labels */
struct LabelSet {
    char labels[MAX_LABEL_SIZE][MAX_LINE_SIZE];
    int count;
};

/* Initialize the label set */
struct LabelSet entryLabelSet = { 0 };

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
        fscanf(file, "%s", word);
        if (isValidLabel(word,data,1)) {
            num_of_entries++;
            data->choice.dir.directive_type = ENTRY;
            return 1;
        } else {
            isError(data, "Error: Invalid label name at .entry","directive.h", head);
            return -1;/*-1 means that it is a directive type but arguments are not valid*/
        }

    } else if (strcmp(word, ".extern") == 0) {
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

    free(data->choice.dir.string);
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
            isError(data, "Error: Missing comma","directive.h", head);
            return 0;
        }
    }
    fseek(file, ftell(file) - 1, SEEK_SET);



    token = strtok(word, " ,");

    while (token != NULL) {
        if (checkLastCharacter(token, ',') == 0) {
            isError(data, "Error: Extraneous text after end of command","directive.h", head);
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
            isError(data, "Error: Missing comma","directive.h", head);
            return 0;
        }
        token = strtok(word, " ,");
    }
    free(data->choice.dir.data);

    return 1;
}

/* Function to process numeric arguments in the .data directive */
int processNumericArguments(char *input, char *word, struct pattern *data, struct Node **head) {
    /* Allocate memory for the array of strings */
    data->choice.dir.data = NULL;

    if (word == NULL) {
        isError(data, "Error: No numeric arguments found", "directive.h", head);
        return 0;
    }

    if (*word == '\0') {
        isError(data, "Error: Missing argument.", "directive.h", head);
        return 0;
    }

    /* Check if the token is a valid number or constant */
    if (!isNumeric(word) && isEntryLabel(word)) {
        isError(data, "Error: Argument is not a real number", "directive.h", head);
        return 0;
    }

    /* Resize the array of strings */
    char **temp = realloc(data->choice.dir.data, (data->choice.dir.size + 1) * sizeof(char *));
    if (temp == NULL) {
        isError(data, "Error: Memory allocation failed", "directive.h", head);
        /* Free the array itself */
        free(data->choice.dir.data);
        return 0; // Indicate error
    }
    data->choice.dir.data = temp;

    /* Allocate memory for the new string and copy the content of word */
    data->choice.dir.data[data->choice.dir.size] = strdup(word);
    if (data->choice.dir.data[data->choice.dir.size] == NULL) {
        isError(data, "Error: Memory allocation failed", "directive.h", head);
        /* Free the array itself */
        free(data->choice.dir.data);
        return 0; // Indicate error
    }
    free(data->choice.dir.data[data->choice.dir.size]);

    data->choice.dir.size++;
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
    int count = 0;   /* Check for alphanumeric or underscore, up to 30 characters */
    char *lastChar = name + strlen(name) - 1;
    char *nameCopy = strdup(name);  /* Make a copy of the name for use in the while loop*/

    /* Check if the name is not empty */
    if (*name == '\0') {
        return 0; // Invalid: Empty name
    }

    /* Check if the first character is a letter */
    if (!isalpha(*name)) {
        return 0;
    }

    if(needColon!=1) {
        /* Check if the last character is ':'*/
        if (*lastChar == ':') {
            *lastChar = '\0'; // Remove the last character
        } else {
            return 0;
        }
    }
        

    while (*nameCopy != '\0' && count < MAX_LABEL_SIZE) {
        if (!isalnum(*nameCopy) && *nameCopy != '_' && !isupper(*nameCopy)) {
            return 0;
        }
        data->label[count] = *nameCopy;
        nameCopy++;
        count++;
    }

    // Copy the modified label to the data structure
    strcpy(data->label, name);
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
    int nextChar;
    // Check if the next character is a newline
    do {
        nextChar = fgetc(file);
    } while (nextChar != EOF && (nextChar == ' ' || nextChar == '\t'));

    // If the next character is not a newline, put it back to the stream
    if (nextChar != EOF && nextChar != '\n') {
        ungetc(nextChar, file);
    } else {
        // Break if the next character is a newline
        isError(data, "Error: no argument at .entry","directive.h", head);
        return -1;
    }

    if (fscanf(file, "%s", tempLabel) == 1) {
        /* Check if the label is already used as .entry */
        if (isEntryLabel(tempLabel)) {
            isError(data, "Error: Label already used as .entry","directive.h", head);
            return -1;/*case -1 means that it is a directive type but arguments are not valid*/
        }

        /* Check if the label is valid */
        if (isValidLabel(tempLabel,data,1)) {
            strcpy(tempLabel, data->label);
            num_of_symbol++;
            /* Add the label to the entry label set */
            addToEntryLabelSet(data->label);
            num_of_externals++;
            return 1;
        } else {
            isError(data, "Error: Invalid label name","directive.h", head);
            return -1;
        }
    } else {
        /* Handle fscanf failure */
        isError(data, "Error: Unable to read label from file","directive.h", head);
        return -1;
    }
}