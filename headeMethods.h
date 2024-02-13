#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#define MAX_ERROR_SIZE 300
#define MAX_LINE_SIZE 81
#define MAX_LABEL_SIZE 31

/* Enumeration for directive types */
enum DirectiveType {
    ENTRY, /**< Entry directive */
    EXTERN, /**< Extern directive */
    STRING, /**< String directive */
    DATA    /**< Data directive */
};

/* Enumeration for instruction types */
enum InstructionType {
    MOV, CMP, ADD, SUB, LEA, NOT, CLR, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, HLT
};

/* Enumeration for registers */
enum Register {
    r0, r1, r2, r3, r4, r5, r6, r7
};

/* Structure to represent a pattern in assembly language */
struct pattern {
    char label[MAX_LABEL_SIZE];  /**< Label associated with the pattern */
    enum { DIRECTIVE, INSTRUCTION, DEFINE, ERROR } type_line; /**< Type of the pattern */

    union {
        char error[MAX_ERROR_SIZE];
        /* Union to store different types of patterns*/
        struct {
            enum DirectiveType directive_type;
            char **data;
            char *string;                    
            int size;
        } dir;

        struct {
            enum InstructionType op_type;     /**< Type of instruction pattern */
            int num_of_operands;              /**< Number of operands */
            struct {
                enum { IMMEDIATE_NUMBER, DIRECT, DIRECT_INDEX, REGISTER } op_type; /**< Type of operand */
                struct {
                    char symbol[MAX_LABEL_SIZE]; /**< Operand value as symbol */
                    char const_num[MAX_LABEL_SIZE]; /**< Operand value as constant number */
                    int value; /**< Operand value as immediate number */
                    enum Register reg; /**< Operand value as register */
                } operand_value;
            } operands[2]; /**< Array to store operands */
            struct code *code; /**< Code associated with the pattern */
        } inst;

        struct {
            int value; /**< Value for define pattern */
        } def;
    } choice;
};

/* Structure for a linked list node representing a pattern */
struct Node {
    struct pattern *data;  /**< Data containing information about the assembly language pattern */
    struct Node *next;    /**< Pointer to the next node in the linked list */
};

/* Structure to map instruction names to their corresponding enum values */
struct InstructionMapping {
    const char *name;            /**< Name of the instruction */
    enum InstructionType type;    /**< Corresponding enum value */
};
extern const struct InstructionMapping instructionMappings[];

/* Structure to map register names to their corresponding enum values */
struct RegisterMapping {
    const char *name;          /**< Name of the register */
    enum Register reg;         /**< Corresponding enum value */
};
extern const struct RegisterMapping registerMappings[];

/* Global variables */
extern int num_of_patterns;
extern int num_of_entries;
extern int num_of_externals;
extern int num_of_constants;
extern int lineNumber;

extern const int true;
extern const int false;
extern const int true_inValid;

/* Function declarations */
/**
 * @brief Categorizes a word based on its type and updates the pattern structure.
 * @param file Pointer to the file being processed.
 * @param word The word to be categorized.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 */
void categorizeWord(FILE *file, char *word, struct pattern *data, struct Node **head);


/* Directive processing functions */
/**
 * @brief Function to check if a word is a directive and return the directive type.
 * @param file Pointer to the file being processed.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return The DirectiveType enumeration if the word is a directive; otherwise, -1.
 */
int directiveFormat(FILE *file, char *word, struct pattern *data, struct Node **head);
/**
 * @brief Function to check if a word is a define and update the pattern structure.
 * @param file Pointer to the file being processed.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the word is a define; otherwise, -1.
 */
int defineFormat(FILE *file, char *word, struct pattern *data, struct Node **head);
/**
 * @brief Function to check if a word is an error and update the pattern structure.
 * @param file Pointer to the file being processed.
 * @param data A pointer to the pattern data structure to be updated.
 * @param errorMessage The error message to be associated with the pattern.
 * @param filename The name of the file being processed.
 * @param head A pointer to the head of the linked list.
 */
void isError(FILE *file, char *word, struct pattern *data, const char *errorMessage, const char *filename, struct Node **head);
/*
 * @brief Processes numeric arguments in a data directive.
 * @param file Pointer to the file being processed.
 * @param input The input string containing numeric arguments.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int processNumericArguments(FILE *file,char *word, char *input, struct pattern *data, struct Node **head);
/**
 * @brief Handles processing of the .string directive.
 * @param file Pointer to the file being processed.
 * @param data A pointer to the pattern data structure to be updated.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int handleStringDirective(FILE *file, struct pattern *data, struct Node **head);
/**
 * @brief Handles processing of the .data directive.
 * @param file Pointer to the file being processed.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int handleDataDirective(FILE *file, char *word, struct pattern *data, struct Node **head);
/**
 * @brief Handles processing of the .extern directive.
 * @param file Pointer to the file being processed.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int handleExternDirective(FILE *file, char *word, struct pattern *data, struct Node **head);
/**
 * @brief Handles processing of the .entry directive.
 * @param file Pointer to the file being processed.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int handleEntryDirective(FILE *file, char *word, struct pattern *data, struct Node **head);
/**
 * @brief Checks for a comma at the end of a line.
 * @param file Pointer to the file being processed.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if a comma is found at the end; otherwise, 0.
 */
int checkCommaAtEnd(FILE *file, char *word, struct pattern *data, struct Node **head);
/**
 * @brief Checks for a comma before a word.
 * @param file Pointer to the file being processed.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if a comma is found before the word; otherwise, 0.
 */
int checkCommaWord(FILE *file, char *word, struct pattern *data, struct Node **head);


/* Linked list functions */
/**
 * @brief Creates a new linked list node with the given data.
 * @param data The pattern data to be stored in the node.
 * @return A pointer to the newly created node.
 */
struct Node *createNode(struct pattern * data);
/**
 * @brief Inserts a new node at the end of the linked list.
 * @param head A pointer to the head of the linked list.
 * @param data The pattern data to be stored in the new node.
 */
void insertNode(struct Node **head, struct pattern * data);
/**
 * @brief Processes a line of assembly language text, tokenizes words, and inserts nodes into the linked list.
 * @param file Pointer to the file being processed.
 * @param head A pointer to the head of the linked list.
 */
void processLine(FILE *file, struct Node **head);
/**
 * @brief Processes an entire file of assembly language text and builds a linked list.
 * @param filename The name of the file to be processed.
 * @return A pointer to the head of the linked list.
 */
struct Node *processAssemblyText(const char *filename);

/* Validation functions */
/**
 * @brief Function to check if a string is a valid constant name.
 * @param name The word to be checked.
 * @return 1 if the word is a valid constant name; otherwise, 0.
 */
int isValidConstantName(const char *name);
/**
 * @brief Function to check if a string is numeric.
 * @param str The word to be checked.
 * @return 1 if the word is numeric; otherwise, 0.
 */
int isNumeric(char *str);
/**
 * @brief Function to check if a label is valid.
 * @param name The label to be checked.
 * @param data Pointer to the data structure where the label is stored if valid.
 * @param needColon Flag indicating whether the label requires a colon at the end.
 * @param symbol Flag indicating whether the label is a symbol.
 * @return 1 if the label is valid; otherwise, 0.
 */
int isValidLabel(char *name, struct pattern *data, int needColon, int symbol);
/**
 * @brief Function to check if the last character of a string is a specific character.
 * @param input The string to be checked.
 * @param errorChar The specific character to check for.
 * @return 0 if the last character is the specified character; otherwise, 1.
 */
int checkLastCharacter(const char input[], char errorChar);


/* Operand processing functions */

/**
 * @brief Function to process a register operand.
 * @param file Pointer to the file being processed.
 * @param word The word to be checked.
 * @param token The token to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param operandCount The expected count of operands.
 * @param i The index of the operand.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the word is a valid register operand; otherwise, 0.
 */
int registerOperand(FILE *file, char *word, char *token, struct pattern *data, int operandCount, int i, struct Node **head);
/**
 * @brief Function to process a direct index operand.
 * @param file Pointer to the file being processed.
 * @param word The word to be checked.
 * @param token The token to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param operandCount The expected count of operands.
 * @param i The index of the operand.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int directIndexOperand(FILE *file, char *word, char *token, struct pattern *data, int operandCount, int i, struct Node **head);
/**
 * @brief Function to process an immediate number operand.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param operandCount The expected count of operands.
 * @param i The index of the operand.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int immediateNumberOperand(char *word, struct pattern *data, int operandCount, int i);
/**
 * @brief Function to process a direct label operand.
 * @param word The word to be checked.
 * @param token The token to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param operandCount The expected count of operands.
 * @param i The index of the operand.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int directLabelOperand(char *word, char *token, struct pattern *data, int operandCount, int i, struct Node **head);
/**
 * @brief Function to process the .define directive.
 * @param file Pointer to the file being processed.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int processDefine(FILE *file, char *word, struct pattern *data, struct Node **head);
/**
 * @brief Function to process a generic directive.
 * @param file Pointer to the file being processed.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int processDirective(FILE *file, char *word, struct pattern *data, struct Node **head);
/**
 * @brief Function to process a generic instruction.
 * @param file Pointer to the file being processed.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int processInstruction(FILE *file, char *word, struct pattern *data, struct Node **head);


/* Instruction processing functions */

/**
 * @brief Function to check if a word is an instruction and update the pattern structure.
 * @param file The file pointer to the input assembly file.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the word is an instruction; otherwise, 0.
 */
int instructionFormat(FILE *file,  char *word, struct pattern *data, struct Node **head);
/**
 * @brief Handles processing of instructions without operands.
 * @param data A pointer to the pattern data structure to be updated.
 * @return 1 if processing is successful; otherwise, 0.
 */
int processNoOperands(struct pattern *data);
/**
 * @brief Handles processing of instructions with one operand.
 * @param file Pointer to the file being processed.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if processing is successful; otherwise, 0.
 */
int processOneOperand(FILE *file, struct pattern *data, struct Node **head);
/**
 * @brief Handles processing of instructions with two operands.
 * @param file Pointer to the file being processed.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if processing is successful; otherwise, 0.
 */
int processTwoOperands(FILE *file, struct pattern *data, struct Node **head);
/**
 * @brief Handles processing of operands for instructions.
 * @param file The file pointer to the input assembly file.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @param operandCount The expected count of operands.
 * @return 1 if operand processing is successful; otherwise, 0.
 */
int processOperands(FILE *file, struct pattern *data, struct Node **head, int operandCount);
/**
 * @brief Function to handle the return value of various processing functions.
 * @param return_value The return value from a processing function.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int handleReturnValue(int return_value, struct pattern *data, struct Node **head);

/**
 * @brief Function to add a label to the entry label set.
 * @param label The label to be added.
*/
void addToEntryLabelSet(const char *label);

/**
 * @brief Function to check if a label is in the entry label set.
 * @param label The label to be checked.
*/
int isEntryLabel(const char *label);

/**
 * @brief Function to check if a label is in the external label set.
 * @param label The label to be checked.
*/
void spareSpace(char *token);

/**
 * @brief This function checks if there are multiple consecutive comas
 * @param line The line that the user entered
 * @return int 0 if there are multiple consecutive comas, 1 otherwise
 */
int check_comas_continus(char *line);
