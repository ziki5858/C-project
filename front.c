#include "headeMethods.h"

/* Global variables*/
int num_of_patterns;
int num_of_symbols;
int num_of_entries;
int num_of_externals;
int num_of_constants;
int lineNumber;

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


void processLine(FILE *file, struct Node **head) {
    char word[MAX_LINE_SIZE];
    lineNumber=0;

    while (fscanf(file, "%s", word) == 1) {
        struct pattern * data = (struct pattern*)calloc(1, sizeof(struct pattern));
        categorizeWord(file,word, data, head);
        lineNumber++;
    }
    num_of_patterns=lineNumber;
}


int categorizeWord(FILE *file, char *word, struct pattern *data, struct Node **head) {
    int return_value;

    if(strcmp(word, ".define") == 0){/*No need to check for label because language doesn't support label for define*/
        return_value= defineFormat(file, word, data, head);
        data->type_line = DEFINE;
        if (return_value == -1) {
            return 0;
        }
        else if(return_value == 1) {
            insertNode(head, *data);
            return 1;
        }
    }

    return_value= directiveFormat(file, word, data, head);
    if (return_value == 1) {
        data->type_line = DIRECTIVE;
        insertNode(head, *data);
        return 1;
    }
    else if (return_value == -1) {
        return 0;
    }

    else {
        return_value= instructionFormat(file, word, data, head);
        if (return_value == 1) {
            data->type_line = INSTRUCTION;
            insertNode(head, *data);
            return 1;
        } else if (return_value == -1) {
            return 0;
        }
        else {
            isError(data, "Error: word not at assembly language table","front.c", head);
            return 0;
        }
    }

}


void isError(struct pattern *data, const char *errorMessage, const char *filename, struct Node **head)
{
    data->type_line = ERROR;
    /*Update the error message with the line number and filename*/
    snprintf(data->choice.error, sizeof(data->choice.error), "Error: %s, File: %s, Line: %d", errorMessage, filename, lineNumber);
    insertNode(head, *data);
}

void printLinkedList(struct Node *head) {
    struct Node *current = head;
    while (current != NULL) {
        printf("\nType: %d\n", current->data.type_line);
		if (current->data.label[0] != '\0') {
			printf("Label: %s\n", current->data.label);
		}
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
    const char *filename = "exampleCheck";
    printf("size of pattern: %lu\n", sizeof(struct pattern));
    struct Node *head = processAssemblyText(filename);

    printLinkedList(head);

    freeLinkedList(head);

    return 0;
}
