#ifndef ASSEMBLER_STRUCTURES
#define ASSEMBLER_STRUCTURES

#define WIDTH_OF_WORD 15
#define MAX_ERROR_SIZE 300
#define MAX_LINE_SIZE 81
#define MAX_LABEL_SIZE 31

/**
 * @brief this struct is used to represent a line in the assembly file.
 * the line type can be a directive, an instruction, a define or an error.
 * if the line is a directive, there is a union with the directive type and the data.
 * if the line is an instruction, there is a union with the instruction type and the operands.
 * if the line is a define, there is a union with the value of the define.
 * if the line is an error, there is a union with the error message.
 * 
 */

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

/**
 * @brief  This struct is used to represent a node in a linked list.
 * each node has a pointer to the data, and a pointer to the next node.
 * 
 */
struct Node {
  struct pattern *data;
  struct Node *next;
  int num_in_list;

};


/**
 * @brief  This struct is used to represent a symbol in the assembly file.
 * each symbol has a label, an address, a type and a line in the file.
 *  
 */
struct symbol {
  char label[MAX_LABEL_SIZE];
  int address;
  enum { CODE, S_DATA, S_EXTERN, S_ENTRY, ENTRY_CODE, ENTRY_DATA } type;
  int line_in_file;
  int num_of_data;
};


/**
 * @brief  This struct is used to represent a constant in the assembly file.
 * each constant has a label, a value and a line in the file.
 *  
 */
struct constant {
  char *label;
  int value;
  int line_in_file;
};


/**
 * @brief  This struct is used to represent a code in the assembly file.
 * each code has a label, an address, a line in the file and a pointer to the next code.
 *  
 */
struct external {
  char *label;
  int *addresses;
  int number_of_addresses;
  int line_in_file;
};

/* make short names for the structs pointers */
typedef struct symbol *Symbol;
typedef struct constant *Constant;
typedef struct external *External;
typedef struct entry *Entry;
typedef struct code *Code;
typedef struct word_bin *WordBin;
typedef struct pattern *Pattern ;
typedef struct macro * Macro ;

/**
 * @brief  This struct is used to represent a macro in the assembly file.
 * each macro has a name, a value and a number of lines.  
 */
struct macro {
	char name[32];
	char *value;
	int number_of_lines;
};


/**
 * @brief  This struct is used to represent a entry in the assembly file.
 * each entry has a symbol and a line in the file.
 *  
 */
struct entry {
  Symbol symbol;
  int line_in_file;
};
struct code {
  int num_of_lines;
  WordBin *lines;
  int line_in_file;
};
struct word_bin {
  char word[WIDTH_OF_WORD]; /**/
};
#endif


void print_error_memory(int line, int *error_flag);
void print_error_msg(char *msg, int line, int *error_flag);