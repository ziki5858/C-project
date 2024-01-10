#include "front.h"

struct pattern {
  char *error[MAX_ERROR_SIZE];
  char *label[MAX_LINE_SIZE];
  enum { DIRECTIVE, INSTRUCTION, DEFINE, ERROR } type_line;
  struct {
    enum { ENRTY, EXTERN, STRING, DATA } directive_type;
    char label[MAX_LABEL_SIZE];
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
    char *label[MAX_LABEL_SIZE];
    int num_of_operands;
	struct {
    enum { IMMEDIATE_NUMBER, IMMEDIATE_CONSANT, DIRECT, DIRECT_INDEX, REGISTER } op_type;
	union {
    char op[MAX_LABEL_SIZE];
    int value;
    enum { r0, r1, r2, r3, r4, r5, r6, r7 } reg;
	} operand_value;
	  } operands[2];
  } inst;
  struct {
	char label[MAX_LABEL_SIZE];
	int value;
  } def;
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
    /*dont forget to free the linke list*/
}
