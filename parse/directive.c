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
        return handleExternDirective(file,word, data, head);
    }  if (strcmp(word, ".string") == 0) {
        return handleStringDirective(file, data, head);
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

    isError(file,word,data, "Error: Invalid label name at .entry", "directive.h", head);
    return true_inValid;  /*means that it is a directive type but arguments are not valid*/
}

void addToEntryLabelSet(const char *label) {
    strcpy(entryLabelSet.labels[entryLabelSet.count], label);
    entryLabelSet.count++;
}

int isEntryLabel(const char *label) {
    int i;
    for (i = 0; i < entryLabelSet.count; i++) {
        if (strcmp(entryLabelSet.labels[i], label) == 0) {
            return true; /*Found in the set*/ 
        }
    }
    return false; /* Not found in the set*/
}

int handleExternDirective(FILE *file,char *word, struct pattern *data, struct Node **head) {
    char tempLabel[MAX_LABEL_SIZE];
    data->choice.dir.directive_type = EXTERN;

    if (fscanf(file, "%s", tempLabel) != 1) {
        /* Handle fscanf failure */
        isError(file,word,data, "Error: Unable to read label from file", "directive.h", head);
        return true_inValid;
    }
   
    if (isValidLabel(tempLabel, data, 1, 0)) {
        strcpy(data->label, tempLabel);
        /* Add the label to the entry label set */
        addToEntryLabelSet(data->label);
        num_of_externals++;
        return true;
    }

    /* Invalid label name */
    isError(file,word,data, "Error: Invalid label name", "directive.h", head);
    return true_inValid;
}

int handleStringDirective(FILE *file, struct pattern *data, struct Node **head) {
    char word[MAX_LINE_SIZE];
    char *token;
    int len=0,i;
 
    data->choice.dir.directive_type = STRING;
    fscanf(file, "%s", word);
    spareSpace(word);
    len=strlen(word);
    
    if(word[0]==34&&word[len-1]==34){
        len-=2;
    }
    else{
        isError(file,word,data, "Error: Invalid string", "directive.h", head);
        return true_inValid;
    }
    token = &word[1];
    /* Allocate memory for the string, including the possibility of all characters being printable */
    data->choice.dir.string = malloc(len + 1); /* +1 for the null terminator*/
    for (i = 1; i < len; ++i) {
        if (!isprint(word[i])) {
            isError(file,word,data, "Error: Invalid string", "directive.h", head);
            return true_inValid;
        }       
    }
    strncpy(data->choice.dir.string, token, len);
    data->choice.dir.string[len] = '\0';
    data->choice.dir.size = len+1;
    return true;
}

void spareSpace(char *token) {
    int i = 0;
    while (token[i] == ' '||token[i]=='\t'||token[i]=='\r') {
        i++;
    }
    memmove(token, token + i, strlen(token + i) + 1);
    for(i=strlen(token)-1;i>=0;i--){
        if(token[i]==' '||token[i]=='\t'||token[i]=='\r'||token[i]=='\n'){
            token[i]='\0';
        }
        else{
            break;
        }
    }
}

int check_comas_continus(char *line) {
  int i, comas = 0;
  /* start from the start of the line and check if there are multiple consecutive comas*/
  for (i = 0; i < strlen(line); i++) {
    if (line[i] == ',') {
      comas++;
	  /* if there is a char that is not a space or a coma, reset the counter*/
    } else if ((line[i] != ' ') && (line[i] != '\t')) {
      comas = 0;
    }
	/* if there are multiple consecutive comas, return 0*/
    if (comas == 2) {
      return false;
    }
  }

  return true;
}

int handleDataDirective(FILE *file, char *word, struct pattern *data, struct Node **head) {
    char *token;
    char input[MAX_LINE_SIZE];
    data->choice.dir.directive_type = DATA;

    /* Read the input line */
    fgets(input, sizeof(input), file);
    /* Replace newline character with null terminator */
    input[strcspn(input, "\n")] = '\0';
    if(!check_comas_continus(input)){
        isError(file,word,data, "Error: Multiple consecutive commas", "directive.h", head);
        return true_inValid;
    }

    if (checkCommaAtEnd(file, input, data, head)) {/*Comma at the end of line*/
        input[strcspn(input, "\0")] = '\n';
        return true_inValid;
    }

    token = strtok(input, ",");

    while (token != NULL) {
        spareSpace(token);
        if (!processNumericArguments(file,word, token, data, head)) {
            return true_inValid;
        }

        /* Tokenize the word to process the next numeric argument */
        token = strtok(NULL, ",");
    }
    return 2;/*case .data for fseek again*/ 
}

int checkCommaWord(FILE *file, char *word, struct pattern *data, struct Node **head) {
    int len = strlen(word);

    if (len > 0 && word[len - 1] != ',') {
        int check=getc(file);
        /* Check if the next character is a newline character */
        if (check == '\n'||check!=',') {
            return false;  /*Return false if the newline character is found*/ 
        }

        /* Display an error and return false if there is a missing comma */
        isError(file, word, data, "Error: Missing comma", "directive.h", head);
        return false;
    }

    return true;  /* Return true if the last character is a comma*/
}

int checkCommaAtEnd(FILE *file, char *word, struct pattern *data, struct Node **head) {
    int len = strlen(word);

    if (getc(file) != '\n') {
        /* Check if the last character of the word is a comma */
        if (len > 0 && word[len - 1] == ',') {
            /* Move the file pointer back to the start of the word */
            fseek(file, ftell(file) - 1, SEEK_SET);
            isError(file,word,data, "Error: Comma is not allowed at the end of line", "directive.h", head);
            return true;  
        }
     
    }
     /* Move the file pointer back to the start of the word */
    fseek(file, ftell(file) - 1, SEEK_SET);
    return false; 
}

int processNumericArguments(FILE *file,char *word, char *input, struct pattern *data, struct Node **head) {
    char **temp;
    /*data->choice.dir.data = NULL;*/

    if (input == NULL) {
        isError(file,word,data, "Error: No numeric arguments found", "directive.h", head);
        return false;
    }

    /* Check if the token is a valid number or constant */
    if (!isNumeric(input) && !isEntryLabel(input)) {
        isError(file,word,data, "Error: Argument is not a real number", "directive.h", head);
        return false;
    }

    /* Resize the array of strings */
    temp = realloc(data->choice.dir.data, (data->choice.dir.size + 1) * sizeof(char *));
    if (temp == NULL) {
        isError(file,word,data, "Error: Memory allocation failed", "directive.h", head);
        /* Free the array itself */
        free(data->choice.dir.data);
        return false; 
    }
    data->choice.dir.data = temp;

    /* Allocate memory for the new string and copy the content of word */
	data->choice.dir.data[data->choice.dir.size] = (char *)malloc(strlen(input) + 1);
    strcpy(data->choice.dir.data[data->choice.dir.size] , input);
    if (data->choice.dir.data[data->choice.dir.size] == NULL) {
        isError(file,word,data, "Error: Memory allocation failed", "directive.h", head);
        free(data->choice.dir.data);
        return false; 
    }

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




