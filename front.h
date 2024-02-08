#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "trie/trie.h"
#define WIDTH_OF_WORD 15
#define MAX_ERROR_SIZE 300
#define MAX_LINE_SIZE 81
#define MAX_LABEL_SIZE 31
extern int IC, DC, error_flag;

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

struct word_bin {
  char word[WIDTH_OF_WORD]; /**/
};

struct code {
  int num_of_lines;
  WordBin *lines;
  int line_in_file;
};
void print_error_memory(int line);
void print_error_msg(char *msg, int line);
extern int num_of_entries;
extern int num_of_externals;
extern int num_of_symbols;
extern int num_of_patterns;


extern External *external_table;
extern Entry *entry_table;
extern Symbol *symbol_table_of_entries;
extern Symbol *symbol_table;

extern Trie symbols, constant, externals, entries, macro_trie;
extern char **binary_table, **binary_table_translated;


int preprocess(char *name_of_file);
struct Node *processAssemblyText(const char *filename);
void first_round(struct Node *head);
void secondround(struct Node *head);
void to_binary_table();
void translator();
void to_files(char * file_name);
void assembler(char *file_name);
