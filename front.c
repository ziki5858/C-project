#include "front.h"
#include "frontMethodDeclarstion.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct Node *createNode(struct pattern data) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

void insertNode(struct Node **head, struct pattern data) {
    struct Node *newNode = createNode(data);
    if (*head == NULL) {
        *head = newNode;
    } else {
        struct Node *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}


struct Node *processAssemblyText(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    struct Node *head = NULL;
    char word[50];

    processLine(file, &head);

    fclose(file);
    return head;
    freeLinkedList(head);
}


void processLine(FILE *file, struct Node **head) {
    char word[50];
    lineNumber=0;
    while (fscanf(file, "%49s", word) == 1) {
        struct pattern data;
        categorizeWord(file,word, &data, head);
        insertNode(head, data);
        lineNumber++;
    }
}


int categorizeWord(FILE *file, char *word, struct pattern *data, struct Node **head) {
    enum InstructionType *instType;

    if(strcmp(word, ".define") == 0){
        data->type_line = DEFINE;
        if (!defineFormat(file, word, data, head)) {
            data->type_line = ERROR;
            return 0;  // Propagate error if the method fails
        }
        insertNode(head, *data);
    }
    else if (directiveFormat(file, word, data, head)) {
        data->type_line = DIRECTIVE;
        insertNode(head, *data);
        num_of_entries++;  /* Increment here if it's ENTRY, adjust for other directives */
        /* ENTRY, EXTERN, STRING, DATA */
    }
    else {
        instType = isInstruction(word, data);
        if (instType != NULL) {
            data->type_line = INSTRUCTION;
        }
        else {
            return 0; /* word not at assembly language table */
        }
    }

    return 1;
}



int defineFormat(FILE *file, char *word, struct pattern *data, struct Node **head) {
    fscanf(file, "%49s", word);
    if (isValidConstantName(word)) {
        strcpy(data->label, word);
        insertNode(head, *data);
    } else {
        isError(data, "Invalid constant name",head);
        return 0;
    }


    fscanf(file, "%49s", word);
    if(strcmp(word,"=")!=0){
        isError(data, "Invalid constant name",head);
        return 0;
    }

    fscanf(file, "%49s", word);
    if (isNumeric(word)) {
        data->def.value = atoi(word);
        insertNode(head, *data);
    } else {
        isError(data, "Invalid numeric value",head);
        return 0;
    }

    return 1;
}


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


int directiveFormat(FILE *file, char *word, struct pattern *data, struct Node **head) {
    if (isValidLabel(word)) {
        fscanf(file, "%49s", word);
        if (strcmp(word, ".string") == 0) {
            return handleStringDirective(file, data, head);
        } else if (strcmp(word, ".data") == 0) {
            return handleDataDirective(file, data, head);
        }
    }

    // Separate cases for other directives
    if (strcmp(word, ".entry") == 0) {
        return 1;
    } else if (strcmp(word, ".extern") == 0) {
        return 1;
    } else if (strcmp(word, ".string") == 0) {
        return handleStringDirective(file, data, head);
    } else if (strcmp(word, ".data") == 0) {
        return handleDataDirective(file, data, head);
    } else {
        return 0;  /* Not a directive */
    }

}

int handleStringDirective(FILE *file, struct pattern *data, struct Node **head) {
    data->dir.directive_type = STRING;
    insertNode(head, *data);
    fscanf(file, "%49s", data->label);
    data->dir.data = (char **)strdup(data->label);
    insertNode(head, *data);
    data->dir.size = countChars(data->label);
    insertNode(head, *data);
    return 1;
}

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

    data->dir.data = (char **)calloc(size,  sizeof(int*));
    //need to insert at a loop the numbers to the array
    data->dir.size = size;
    insertNode(head, *data);
    return 1;
}

void isError(struct pattern *data, const char *errorMessage, struct Node **head)
{
    data->type_line = ERROR;
    // Insert data into the linked list
    insertNode(head, *data);
    // Update the error message with the line number
    snprintf(data->error, sizeof(data->error), "Error: %s, File: front.c, Line: %d", errorMessage, lineNumber);
    // Insert the updated data into the linked list
    insertNode(head, *data);
}

int processNumericArguments(char *input, struct pattern *data, struct Node **head) {
    int size = 0;
    char *token = strtok(input, "[^,]");

    while (token != NULL) {
        if (*token == '\0') {
            isError(data, "Error: Missing argument.",head);
            return -1;  // Indicate error
        }

        if (!isNumeric(token)) {
            isError(data, "Error: Argument is not a real number",head);
            return -1;  // Indicate error
        }
        size++;
        token = strtok(NULL, "[^,]");
    }
    return size;
}

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


int miss(int requireComma) {
    int missing;
    missing = getchar();

    if (missing == '\n') {
        return 0; /* No extraneous text or missing comma*/
    }

    if (requireComma && missing != ',') {
        /* Handle extraneous text (clear the input buffer)*/
        while (getchar() != '\n');
        return 0;
    }

    return 1;
}

int countChars(const char *str) {
    int count = 1;/*for the \0 at the end of string by task instruction*/
    // Iterate through each character until the null terminator is reached
    while (*str != '\0') {
        count++;
        str++;
    }
    return count;
}


int isValidLabel(const char *name) {
    // Check if the name is not empty
    if (*name == '\0') {
        return 0; // Invalid: Empty name
    }

    // Check if the first character is a letter
    if (!isalpha(*name)) {
        return 0; // Invalid: Name must start with a letter
    }

    // Check the remaining characters
    name++;

    // Check for alphanumeric or underscore, up to 30 characters
    int count = 1;
    while (*name != '\0' && count <= 30) {
        if (!isalnum(*name) && *name != '_') {
            return 0; // Invalid: Name must be alphanumeric or underscore
        }
        name++;
        count++;
    }

    // Check if the last character is ':'
    if (*(name - 1) != ':') {
        return 0; // Invalid: Label definition must end with ':'
    }

    return 1; // Valid label
}



enum InstructionType* isInstruction(const char *word, struct pattern *data) {
    /* Iterate through the array to find a match */
    for (int i = 0; instructionMappings[i].name != NULL; ++i) {
        if (strcmp(word, instructionMappings[i].name) == 0) {
            /* It's an instruction */
            data->type_line = INSTRUCTION;
            data->inst.op_type = instructionMappings[i].type;
            /* Update other properties of the data structure as needed */
            return &(data->inst.op_type);
        }
    }
    /* Not an instruction */
    return NULL;
}




void printLinkedList(struct Node *head) {
    struct Node *current = head;
    while (current != NULL) {
        printf("Type: %d\n", current->data.type_line);
        current = current->next;

    }
}

void freeLinkedList(struct Node *head) {
    struct Node *current = head;
    while (current != NULL) {
        struct Node *temp = current;
        current = current->next;
        // Free the allocated memory for the data in each node
        free(temp);
    }
}

int main() {
    // Replace "your_input_file.asm" with the actual file name you want to process
    const char *filename = "code.am";

    // Process the assembly text and build a linked list
    struct Node *head = processAssemblyText(filename);

    // Print the linked list for verification
    printLinkedList(head);

    // Free the allocated memory for the linked list
    freeLinkedList(head);

    return 0;
}
