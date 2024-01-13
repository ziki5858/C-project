#include "front.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
int num_of_patterns;
int num_of_symbol;
int num_of_entries;
int num_of_externals;
int lineNumber;

/** Enumeration for directive types */
enum DirectiveType {
    ENTRY,
    EXTERN,
    STRING,
    DATA
};

/** Enumeration for instruction types */
enum InstructionType {
    MOV, CMP, ADD, SUB, LEA, NOT, CLR, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, HLT
};

/** Structure to represent a pattern */
struct pattern {
    char error[MAX_ERROR_SIZE];
    char label[MAX_LINE_SIZE];
    enum { DIRECTIVE, INSTRUCTION, DEFINE, ERROR } type_line;
    struct {
        enum DirectiveType directive_type;
        char **data;
        int size;
    } dir;
    struct {
        enum InstructionType op_type;
        int num_of_operands;
        struct {
            enum { IMMEDIATE_NUMBER, DIRECT, DIRECT_INDEX, REGISTER } op_type;
            union {
                char symbol[MAX_LABEL_SIZE];
                char const_num[MAX_LABEL_SIZE];
                int value;
                enum { r0, r1, r2, r3, r4, r5, r6, r7 } reg;
            } operand_value;
        } operands[2];
    } inst;
    struct {
        int value;
    } def;
    struct code *code;
};


/** Linked list node representing a pattern */
struct Node {
    struct pattern data;  /**< Data containing information about the assembly language pattern */
    struct Node *next;    /**< Pointer to the next node in the linked list */
};

/**
 * @var instructionMappings
 * @brief Array to map instruction names to their corresponding enum values.
 */
const struct InstructionMapping {
    const char *name;            /**< Name of the instruction */
    enum InstructionType type;   /**< Corresponding enum value */
} instructionMappings[] = {
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

/**
 * @brief Function to check if a word is a directive and return the directive type.
 * @param word The word to be checked.
 * @return The DirectiveType enumeration if the word is a directive; otherwise, -1.
 */
enum DirectiveType isDirective(const char *word);

/**
 * @brief Checks if a word is an instruction and updates the pattern structure.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @return A pointer to the InstructionType if the word is an instruction; otherwise, NULL.
 */
enum InstructionType* isInstruction(const char *word, struct pattern *data);

/**
 * @brief Function to check if a word is a define and update the pattern structure.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @return 1 if the word is a define; otherwise, 0.
 */
int isDefine(const char *word, struct pattern *data);

/**
 * @brief Function to check if a word is an error and update the pattern structure.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @return 1 if the word is an error; otherwise, 0.
 */
void isError(struct pattern *data, const char *errorMessage);

/**
 * @brief Creates a new linked list node with the given data.
 * @param data The pattern data to be stored in the node.
 * @return A pointer to the newly created node.
 */
struct Node *createNode(struct pattern data);

/**
 * @brief Inserts a new node at the end of the linked list.
 * @param head A pointer to the head of the linked list.
 * @param data The pattern data to be stored in the new node.
 */
void insertNode(struct Node **head, struct pattern data);



int categorizeWord(FILE *file, char *word, struct pattern *data, struct Node **head);
/**
 * @brief Processes a line of assembly language text, tokenizes words, and inserts nodes into the linked list.
 * @param line The line of assembly language text to be processed.
 * @param head A pointer to the head of the linked list.
 */
void processLine(FILE *file, struct Node **head);

/**
 * @brief Processes an entire file of assembly language text and builds a linked list.
 * @param filename The name of the file to be processed.
 * @return A pointer to the head of the linked list.
 */
struct Node *processAssemblyText(const char *filename);

/**
 * @brief Prints the content of the linked list.
 * @param head A pointer to the head of the linked list.
 */
void printLinkedList(struct Node *head);

/**
 * @brief Frees the memory allocated for the linked list.
 * @param head A pointer to the head of the linked list.
 */
void freeLinkedList(struct Node *head);

int isValidConstantName(const char *name);

int isNumeric(char *str);

int defineFormat(FILE *file, char *word, struct pattern *data, struct Node **head);

struct Node *processAssemblyFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    struct Node *head = NULL;
    char buffer[MAX_LINE_SIZE];

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        processLine(file, &head);
    }


    fclose(file);
    return head;
}

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

int categorizeWord(FILE *file, char *word, struct pattern *data, struct Node **head) {
    enum DirectiveType directiveType = isDirective(word);
    enum InstructionType *instType;

    if (directiveType != -1) {
        data->type_line = DIRECTIVE;
        data->dir.directive_type = directiveType;
        num_of_entries++;  /* Increment here if it's ENTRY, adjust for other directives */
        /* ENTRY, EXTERN, STRING, DATA */
    } else {
        instType = isInstruction(word, data);
        if (instType != NULL) {
            data->type_line = INSTRUCTION;
        } else if (isDefine(word, data)) {
            // Call the new method to read constant and numeric values
            if (!defineFormat(file, word, data, head)) {
                data->type_line = ERROR;
                return 0;  // Propagate error if the method fails
            }
            data->type_line = DEFINE;
            insertNode(head, *data);
        } else {
            return 0; /* word not at assembly language table */
        }
    }

    return 1;
}

void isError(struct pattern *data, const char *errorMessage)
{
    snprintf(data->error, sizeof(data->error),
             "Error: %s, File: front.c, Line: %d", errorMessage, lineNumber);
}

int defineFormat(FILE *file, char *word, struct pattern *data, struct Node **head) {

    fscanf(file, "%49s", word);
    if (isValidConstantName(word)) {
        strcpy(data->label, word);
        insertNode(head, *data);
    } else {
        isError(data, "Invalid constant name");
        return 0;
    }


    fscanf(file, "%49s", word);

    if(strcmp(word,"=")!=0){
        isError(data, "Invalid constant name");
        return 0;
    }

    fscanf(file, "%49s", word);

    if (isNumeric(word)) {
        data->def.value = atoi(word);
        insertNode(head, *data);
    } else {
        isError(data, "Invalid numeric value");
        return 0;
    }

    return 1;
}

/**
 * Checks if a given string is numeric.
 *
 * @param str The string to be checked.
 * @return 1 if the string is numeric, 0 otherwise.
 */
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


enum DirectiveType isDirective(const char *word) {
    if (strcmp(word, "entry") == 0) {
        return ENTRY;
    } else if (strcmp(word, "extern") == 0) {
        return EXTERN;
    } else if (strcmp(word, "string") == 0) {
        return STRING;
    } else if (strcmp(word, "data") == 0) {
        return DATA;
    } else {
        return -1;  /* Not a directive */
    }
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


int isDefine(const char *word, struct pattern *data) {
    // Add conditions for recognizing define types
    // For example, you might check if the word starts with "#define"
    // Update the data structure accordingly
    if (strcmp(word, ".define") == 0) {
        data->type_line = DEFINE;
        return 1;  // Indicates that it is a define
    }

    return 0;  // Not a define
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




struct Node *processAssemblyText(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    struct Node *head = NULL;
    char word[50];

    while (fscanf(file, "%49s", word) == 1) {
        struct pattern data;
        categorizeWord(file, word, &data, &head);
    }

    fclose(file);
    return head;

    // Don't forget to free the linked list
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
