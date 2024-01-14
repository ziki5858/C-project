#include "front.h"
#include "directive.h"
#include "define.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
    processLine(file, &head);
    fclose(file);
    return head;
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
            return 0;
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


void isError(struct pattern *data, const char *errorMessage, struct Node **head)
{
    data->type_line = ERROR;
    /* Insert data into the linked list*/
    insertNode(head, *data);
    /*Update the error message with the line number*/
    snprintf(data->error, sizeof(data->error), "Error: %s, File: front.c, Line: %d", errorMessage, lineNumber);
    insertNode(head, *data);
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
        /* Free the allocated memory for the data in each node*/
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
