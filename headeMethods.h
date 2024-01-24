/** @file
 *  @brief Brief description of the purpose of the file.
 *
 *  Detailed description of the purpose and contents of the file.
 *
 *  @note
 *  Additional notes or considerations, if any.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ctype.h"

#define MAX_ERROR_SIZE 300
#define MAX_LINE_SIZE 81
#define MAX_LABEL_SIZE 31
struct pattern;
struct code;

/* Global variables*/
extern int num_of_patterns;
extern int num_of_symbols;
extern int num_of_entries;
extern int num_of_externals;
extern int num_of_constants;
extern int lineNumber;

/**
 * @enum DirectiveType
 * @brief Enumeration for directive types.
 */
enum DirectiveType {
    ENTRY, /**< Entry directive */
    EXTERN, /**< Extern directive */
    STRING, /**< String directive */
    DATA    /**< Data directive */
};

/**
 * @enum InstructionType
 * @brief Enumeration for instruction types.
 */
enum InstructionType {
    MOV, CMP, ADD, SUB, LEA, NOT, CLR, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, HLT
};

enum Register {
    r0,
    r1,
    r2,
    r3,
    r4,
    r5,
    r6,
    r7
};

/**
 * @struct pattern
 * @brief Structure to represent a pattern in assembly language.
 */
struct pattern {
     /**< Error message, if any */
    char label[MAX_LABEL_SIZE];  /**< Label associated with the pattern */
    enum { DIRECTIVE, INSTRUCTION, DEFINE, ERROR } type_line; /**< Type of the pattern */
	
	union {
	char error[MAX_ERROR_SIZE];
    /* Union to store different types of patterns*/
    struct {
        enum DirectiveType directive_type; /**< Type of directive pattern */
        char **data;
		char * string;                        /**< Data for certain directive patterns */
        int size;
    } dir;

    struct {
        enum InstructionType op_type;        /**< Type of instruction pattern */
        int num_of_operands;                 /**< Number of operands */
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

/**
 * @struct Node
 * @brief Linked list node representing a pattern.
 */
struct Node {
    struct pattern data;  /**< Data containing information about the assembly language pattern */
    struct Node *next;    /**< Pointer to the next node in the linked list */
};

/**
 * @struct InstructionMapping
 * @brief Structure to map instruction names to their corresponding enum values.
 */
struct InstructionMapping {
    const char *name;            /**< Name of the instruction */
    enum InstructionType type;/**< Corresponding enum value */
};
extern const struct InstructionMapping instructionMappings[];


struct RegisterMapping {
    const char *name;          /**< Name of the register */
    enum Register reg;         /**< Corresponding enum value */
};

extern const struct RegisterMapping registerMappings[];



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
 * @param data A pointer to the pattern data structure to be updated.
 * @param errorMessage The error message to be associated with the pattern.
 * @param filename The name of the file being processed.
 * @param head A pointer to the head of the linked list.
 */
void isError(struct pattern *data, const char *errorMessage, const char *filename, struct Node **head);
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

/**
 * @brief Function to check if a word is a valid constant name.
 * @param name The word to be checked.
 * @return 1 if the word is a valid constant name; otherwise, 0.
 */
int isValidConstantName(const char *name);

/**
 * @brief Function to check if a word is numeric.
 * @param str The word to be checked.
 * @return 1 if the word is numeric; otherwise, 0.
 */
int isNumeric(char *str);

/**
 * @brief Function to check if a label is valid.
 * @param name The label to be checked.
 * @param data Pointer to the data structure where the label is stored if valid.
 * @param needColon Flag indicating whether the label requires a colon at the end.
 * @return 1 if the label is valid; otherwise, 0.
 */
int isValidLabel(char *name,struct pattern *data, int needColon );

/**
 * @brief Function to count characters in a string.
 * @param str The string to be processed.
 * @return The number of characters in the string.
 */
int countChars(const char *str);

/**
 * @brief Function to check for missing elements in a data directive.
 * @param requireComma Flag indicating if a comma is required.
 * @return 0 if there is no extraneous text or missing comma; otherwise, 1.
 */
int miss(int requireComma, FILE *file);

/**
 * @brief Function to check if the last character of a string is a specific character.
 * @param input The string to be checked.
 * @param errorChar The specific character to check for.
 * @return 0 if the last character is the specified character; otherwise, 1.
 */
int checkLastCharacter(const char input[], char errorChar);

/**
 * @brief Function to process numeric arguments in a data directive.
 * @param input The input string containing numeric arguments.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int processNumericArguments(char *input, char *word, struct pattern *data, struct Node **head);

/**
 * @brief Function to handle the .string directive.
 * @param file Pointer to the file being processed.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int handleStringDirective(FILE *file, struct pattern *data);

/**
 * @brief Function to handle the .data directive.
 * @param file Pointer to the file being processed.
 * @param data A pointer to the pattern data structure to be updated.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int handleDataDirective(FILE *file, char *word, struct pattern *data, struct Node **head);

/**
 * @brief Function to check if a label is already used as .entry.
 * @param label The label to be checked.
 * @return 1 if the label is used as .entry; otherwise, 0.
 */
int isEntryLabel(const char *label);

/**
 * @brief Function to add a label to the .entry label set.
 * @param label The label to be added.
 */
void addToEntryLabelSet(const char *label);

/**
 * @brief Function to handle the .entry directive.
 * @param file Pointer to the file being processed.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the processing is successful; otherwise, 0.
 */
int handleEntryDirective(FILE *file, struct pattern *data, struct Node **head);

/**
 * @brief Categorizes a word based on its type and updates the pattern structure.
 * @param file Pointer to the file being processed.
 * @param word The word to be categorized.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the categorization is successful; otherwise, 0.
 */
int categorizeWord(FILE *file, char *word, struct pattern *data, struct Node **head);


/**
 * @brief Checks if a word is an instruction and updates the pattern structure.
 * @param file The file pointer to the input assembly file.
 * @param word The word to be checked.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if the word is an instruction; otherwise, 0.
 */
int instructionFormat(FILE *file, const char *word, struct pattern *data, struct Node **head);

/**
 * @brief Handles processing of instructions without operands.
 * @param data A pointer to the pattern data structure to be updated.
 * @return 1 if processing is successful; otherwise, 0.
 */
int processNoOperands(struct pattern *data);

/**
 * @brief Handles processing of instructions with one operand.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if processing is successful; otherwise, 0.
 */
int processOneOperand(FILE  *file, struct pattern *data, struct Node **head);

/**
 * @brief Handles processing of instructions with two operands.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @return 1 if processing is successful; otherwise, 0.
 */
int processTwoOperands(FILE  *file, struct pattern *data, struct Node **head);


/**
 * @brief Handles processing of operands for instructions.
 * @param file The file pointer to the input assembly file.
 * @param data A pointer to the pattern data structure to be updated.
 * @param head A pointer to the head of the linked list.
 * @param operandCount The expected count of operands.
 * @return 1 if operand processing is successful; otherwise, 0.
 */
int processOperands(FILE *file, struct pattern *data, struct Node **head, int operandCount);

