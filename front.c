#include "front.h"
#include <string.h>

int num_of_patterns;
int num_of_symbols;
int num_of_entries;
int num_of_externals; 


struct pattern {
  char *error[MAX_ERROR_SIZE];
  char *label[MAX_LINE_SIZE];
  enum { DIRECTIVE, INSTRUCTION, DEFINE, ERROR } type_line;
  struct {
    enum { ENTRY, EXTERN, STRING, DATA } directive_type;
    int *data;
	char *string;
  } dir;
  struct {
    enum {
      MOV,
      CMP,
      ADD,
      SUB,
      LEA,
      NOT,
      CLR,
      INC,
      DEC,
      JMP,
      BNE,
      RED,
      PRN,
      JSR,
      RTS,
      HLT
    } op_type;
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
  struct code * code;
};

/* Linked list node representing a pattern */
struct Node {
    struct pattern data;  /**< Data containing information about the assembly language pattern */
    struct Node *next;    /**< Pointer to the next node in the linked list */
};

/**
 * @brief Creates a new linked list node with the given data.
 * @param data The pattern data to be stored in the node.
 * @return A pointer to the newly created node.
 */
struct Node *createNode(struct pattern data) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

/**
 * @brief Inserts a new node at the end of the linked list.
 * @param head A pointer to the head of the linked list.
 * @param data The pattern data to be stored in the new node.
 */
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

/**
 * @brief Categorizes a word and populates the pattern data structure.
 * @param word The word to be categorized.
 * @param data A pointer to the pattern data structure to be populated.
 */
void categorizeWord(const char *word, struct pattern *data) {
    // TODO: Implement the logic to categorize the word and populate the 'data' structure
}

/**
 * @brief Function to check if a word is a directive and return the directive type.
 * @param word The word to be checked.
 * @return A pointer to the DirectiveType enumeration if the word is a directive; otherwise, NULL.
 * @note The caller is responsible for freeing the memory allocated for the DirectiveType.
 */
enum DirectiveType* isDirective(const char *word) {
    enum DirectiveType *directiveType = malloc(sizeof(enum DirectiveType));
    if (directiveType == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    if (strcmp(word, "entry") == 0) {
        *directiveType = ENTRY;
    } else if (strcmp(word, "extern") == 0) {
        *directiveType = EXTERN;
    } else if (strcmp(word, "string") == 0) {
        *directiveType = STRING;
    } else if (strcmp(word, "data") == 0) {
        *directiveType = DATA;
    } else {
        free(directiveType);
        return NULL;
    }

    return directiveType;
}


/**
 * @brief Function to check if a word is an instruction and update the pattern structure.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @return The type of the instruction if the word is an instruction; otherwise, 0.
 */
enum InstructionType isInstruction(const char *word, struct pattern *data) {
    // Add conditions for recognizing instruction types
    if (strcmp(word, "mov") == 0) {
        data->type_line = INSTRUCTION;
        data->type.instruction_type = MOV;
        // Update other properties of the data structure as needed
        return MOV;
    } else if (strcmp(word, "cmp") == 0) {
        data->type_line = INSTRUCTION;
        data->type.instruction_type = CMP;
        // Update other properties of the data structure as needed
        return CMP;
    } else if (strcmp(word, "add") == 0) {
        data->type_line = INSTRUCTION;
        data->type.instruction_type = ADD;
        // Update other properties of the data structure as needed
        return ADD;
    }
    // Add more conditions as needed

    return 0;  // Not an instruction
}


/**
 * @brief Function to check if a word is a define and update the pattern structure.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @return 1 if the word is a define; otherwise, 0.
 */
int isDefine(const char *word, struct pattern *data) {
    // Add conditions for recognizing define types
    // For example, you might check if the word starts with "#define"
    // Update the data structure accordingly

    return 0;  // Not a define
}

/**
 * @brief Function to check if a word is an error and update the pattern structure.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @return 1 if the word is an error; otherwise, 0.
 */
int isError(const char *word, struct pattern *data) {
    // Add conditions for recognizing error types
    // For example, you might check if the word is an error keyword
    // Update the data structure accordingly

    return 0;  // Not an error
}

/**
 * @brief Processes a line of assembly language text, tokenizes words, and inserts nodes into the linked list.
 * @param line The line of assembly language text to be processed.
 * @param head A pointer to the head of the linked list.
 */
void processLine(const char *line, struct Node **head) {
    char *token = strtok((char *)line, " \t\n");  // Split by space, tab, and newline
    while (token != NULL) {
        struct pattern data;
        categorizeWord(token, &data);
        insertNode(head, data);
        token = strtok(NULL, " \t\n");  // Move to the next token
    }
}

/**
 * @brief Processes an entire file of assembly language text and builds a linked list.
 * @param filename The name of the file to be processed.
 * @return A pointer to the head of the linked list.
 */
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
    /*don't forget to free the linked list*/
}
