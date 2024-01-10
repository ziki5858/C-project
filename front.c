#include "front.h"
#include <string.h>
#include <stdlib.h>

int num_of_patterns;
int num_of_symbol;
int num_of_entries;
int num_of_externals;

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
    char *error[MAX_ERROR_SIZE];
    char *label[MAX_LINE_SIZE];
    enum { DIRECTIVE, INSTRUCTION, DEFINE, ERROR } type_line;
    struct {
        enum DirectiveType directive_type;
        int *data;
        char *string;
    } dir;
    struct {
        enum InstructionType op_type;
        int num_of_operands;
        struct {
            enum { IMMEDIATE_NUMBER, IMMEDIATE_CONSTANT, DIRECT, DIRECT_INDEX, REGISTER } op_type;
            union {
                char op[MAX_LABEL_SIZE];
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
int isError(const char *word, struct pattern *data);

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

/**
 * @brief Function to categorize a word and populate the pattern data structure.
 * @param word The word to be categorized.
 * @param data A pointer to the pattern data structure to be populated.
 */
void categorizeWord(const char *word, struct pattern *data);

/**
 * @brief Processes a line of assembly language text, tokenizes words, and inserts nodes into the linked list.
 * @param line The line of assembly language text to be processed.
 * @param head A pointer to the head of the linked list.
 */
void processLine(const char *line, struct Node **head);

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

void categorizeWord(const char *word, struct pattern *data) {
    enum DirectiveType directiveType = isDirective(word);
    enum InstructionType *instType;
    if (directiveType != -1) {
        /* It's a directive */
        data->type_line = DIRECTIVE;
        data->dir.directive_type = directiveType;
        num_of_entries++;  // Increment here if it's ENTRY, you may need to adjust for other directives
    } else {
        instType = isInstruction(word, data);
        if (instType != NULL) {
            /* It's an instruction */
            data->type_line = INSTRUCTION;
            /* Additional properties may already be updated in isInstruction function */
        } else if (isDefine(word, data)) {
            /* It's a define */
            data->type_line = DEFINE;
            /* Additional properties may already be updated in isDefine function */
        } else if (isError(word, data)) {
            /* It's an error */
            data->type_line = ERROR;
            /* Additional properties may already be updated in isError function */
        }
    }
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

    return 0;  // Not a define
}


int isError(const char *word, struct pattern *data) {
    // Add conditions for recognizing error types
    // For example, you might check if the word is an error keyword
    // Update the data structure accordingly

    return 0;  // Not an error
}


void processLine(const char *line, struct Node **head) {
    char *token = strtok((char *)line, " \t\n");  // Split by space, tab, and newline
    while (token != NULL) {
        struct pattern data;
        categorizeWord(token, &data);
        insertNode(head, data);
        token = strtok(NULL, " \t\n");  // Move to the next token
    }
}


struct Node *processAssemblyText(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    struct Node *head = NULL;
    char buffer[MAX_LINE_SIZE];

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        processLine(buffer, &head);
    }

    fclose(file);
    return head;

    // Don't forget to free the linked list
}

void printLinkedList(struct Node *head) {
    struct Node *current = head;
    while (current != NULL) {
        // Print or process the data in each node as needed
        // Example: printf("Type: %d\n", current->data.type_line);
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
