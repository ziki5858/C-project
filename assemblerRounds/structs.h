#ifndef ASSEMBLER_STRUCTURES
#define ASSEMBLER_STRUCTURES

#define MAX_LINE_SIZE 81
#define MAX_LABEL_SIZE 31
#define MAX_ERROR_SIZE 300
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
struct Node {
  struct pattern *data;
  struct Node *next;
};

struct symbol {
  char label[MAX_LABEL_SIZE];
  int address;
  enum { CODE, S_DATA, S_EXTERN, S_ENTRY, ENTRY_CODE, ENTRY_DATA } type;
  int line_in_file;
};

struct constant {
  char *label;
  int value;
  int line_in_file;
};

struct external {
  char *label;
  int *addresses;
  int number_of_addresses;
  int line_in_file;
};
typedef struct symbol *Symbol;
typedef struct constant *Constant;
typedef struct external *External;
typedef struct entry *Entry;
typedef struct code *Code;
typedef struct word_bin *WordBin;
typedef struct pattern *Pattern ;
typedef struct macro * Macro ;
struct macro {
	char name[32];
	char *value;
	int number_of_lines;
};
struct entry {
  Symbol symbol;
  int line_in_file;
};
#endif


void print_error_memory(int line);
void print_error_msg(char *msg, int line);