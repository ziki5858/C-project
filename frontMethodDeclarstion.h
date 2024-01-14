#include "front.h"

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
int directiveFormat(FILE *file, char *word, struct pattern *data, struct Node **head);
/**
 * @brief Checks if a word is an instruction and updates the pattern structure.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @return A pointer to the InstructionType if the word is an instruction; otherwise, NULL.
 */
enum InstructionType* isInstruction(const char *word, struct pattern *data);

/**
 * @brief Function to check if a word is an error and update the pattern structure.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @return 1 if the word is an error; otherwise, 0.
 */
void isError(struct pattern *data, const char *errorMessage,struct Node **head);

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

int isValidLabel(const char *name);

int countChars(const char *str);

int miss(int requireComma);

int checkLastCharacter(const char input[], char errorChar);

int processNumericArguments(char *input, struct pattern *data, struct Node **head) ;

int handleStringDirective(FILE *file, struct pattern *data, struct Node **head);

int handleDataDirective(FILE *file, struct pattern *data, struct Node **head);
