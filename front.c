/*
 * ----------------------------------------------------------------------------
 * File: front.c
 * Author: Ishay Abakiyev - ziki, Shlomo Wsisz
 * Date: 28/01/2024
 * Description: This file contains functions related to processing assembly language
 *              code. It includes a categorization function, categorizing words
 *              into predefined types such as .define, directives, and instructions.
 *
 * Functions:
 *   - createNode: Creates a new linked list node with the given pattern data.
 *   - insertNode: Inserts a new node at the end of the linked list.
 *   - processAssemblyText: Processes the entire assembly file and returns the head of the linked list.
 *   - processLine: Processes a single line from the assembly file, categorizes words, and updates the linked list.
 *   - categorizeWord: Categorizes a word in the assembly language, utilizing an array of format processing functions.
 *   - handleReturnValue: Handles the return value from format processing functions, updating the linked list if needed.
 *   - processDefine: Processes the .define directive in the assembly code, updating the data structure accordingly.
 *   - processDirective: Processes directives in the assembly code, updating the data structure based on the directive type.
 *   - processInstruction: Processes instructions in the assembly code, updating the data structure based on the instruction type.
 *   - isError: Handles error cases, updating the linked list with an error message.
 *
 * Main Function:
 *   - Calls processAssemblyText to process the assembly file and obtain the linked list.
 * ----------------------------------------------------------------------------
 */

#include "headeMethods.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>



/* Global variables*/
int num_of_patterns;
int num_of_entries;
int num_of_externals;
int num_of_constants;
int lineNumber;

const int true = 1;
const int false = 0 ;
const int true_inValid = -1;

const struct InstructionMapping instructionMappings[] = {
    {"mov", MOV},
    {"cmp", CMP},
    {"add", ADD},
    {"sub", SUB},
    {"lea", LEA},
    {"not", NOT},
    {"clr", CLR},
    {"inc", INC},
    {"dec", DEC},
    {"jmp", JMP},
    {"bne", BNE},
    {"red", RED},
    {"prn", PRN},
    {"jsr", JSR},
    {"rts", RTS},
    {"hlt", HLT},
    {NULL, (enum InstructionType)0}  /* Sentinel value for the end of the array */
};

const struct RegisterMapping registerMappings[] = {
{"r0", r0},
{"r1", r1},
{"r2", r2},
{"r3", r3},
{"r4", r4},
{"r5", r5},
{"r6", r6},
{"r7", r7},
{NULL, (enum Register)0}   /* Sentinel value for the end of the array */
};

struct Node *createNode(struct pattern * data) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

void insertNode(struct Node **head, struct pattern * data) {
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

void processLine(FILE *file, struct Node **head) {
    char word[MAX_LINE_SIZE]; /* Buffer to store individual words */
    char lineB[MAX_LINE_SIZE+2]; /* Buffer to store the entire line read from file, plus extra space for potential null terminator and newline character */

    while (fgets(lineB, sizeof(lineB), file) != NULL){ /* Read a line from the file until NULL is encountered (end of file) */
        int c;
        struct pattern * data = (struct pattern*)calloc(1, sizeof(struct pattern)); /* Allocate memory for a structure to store pattern data */
        
        if (strlen(lineB) > MAX_LINE_SIZE ) { /* If the length of the line exceeds the maximum allowed size */
            isError(file, word, data, "Error: Line exceeds maximum length of 80 characters", "front.c", head); /* Call a function to handle the error */
        }
        else{
            fseek(file, -strlen(lineB), SEEK_CUR); /* Move the file pointer back to the beginning of the line */
            fscanf(file, "%s", word); /* Read the first word from the line */
            categorizeWord(file, lineB, word, data, head); /* Call a function to categorize/process the word */
        }
        while ((c = fgetc(file)) != '\n' && c != EOF); /* Discard remaining characters in the current line until a newline or end of file is encountered */
        num_of_patterns++; /* Increment the count of patterns processed */
    }
}

struct Node *processAssemblyText(const char *filename) {
    struct Node *head = NULL;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }
    processLine(file, &head);
    fclose(file);
    return head;
}


void categorizeWord(FILE *file, char *lineB, char *word, struct pattern *data, struct Node **head) {
    int return_value;

    if (strcmp(word, ".define") == 0) {
        return_value = processDefine(file, word, data, head);
        if (return_value != 0) {
            return;
        }
    }

    return_value = processDirective(file,lineB, word, data, head);/*Here undle label for all cases*/
    if (return_value != 0) {
        return;
    }

    return_value = processInstruction(file, word, data, head);
    if (return_value != 0) {
        return;
    }

    isError(file, word, data, "Error: word not at assembly language table", "front.c", head);
}

int handleReturnValue(int return_value, struct pattern *data, struct Node **head) {
    if (return_value == -1) {
        return true_inValid;
    } else if (return_value == 1||return_value == 2) {
        insertNode(head, data);
        return true;
    }
    return false;
}

int processDefine(FILE *file, char *word, struct pattern *data, struct Node **head) {
    int return_value;
    data->type_line = DEFINE;
    return_value = defineFormat(file, word, data, head);
    return handleReturnValue(return_value, data, head);
}

int processDirective(FILE *file,char *lineB, char *word, struct pattern *data, struct Node **head) {
    int return_value;
   
    data->type_line = DIRECTIVE;
    return_value = directiveFormat(file, word, data, head);
    if(return_value == 2){
        fseek(file, -strlen(lineB), SEEK_CUR);
    }
    return handleReturnValue(return_value, data, head);
}

int processInstruction(FILE *file, char *word, struct pattern *data, struct Node **head) {
    int return_value;
    data->type_line = INSTRUCTION;
    return_value = instructionFormat(file, word, data, head);
    return handleReturnValue(return_value, data, head);
}

void isError(FILE *file, char *word, struct pattern *data, const char *errorMessage, const char *filename, struct Node **head){
    data->type_line = ERROR;
    /*Update the error message with the line number and filename*/
    sprintf(data->choice.error, "%s, File: %s, Line: %d", errorMessage, filename, num_of_patterns);
    insertNode(head, data);
}

