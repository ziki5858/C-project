#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define WIDTH_OF_WORD 15
#define MAX_ERROR_SIZE 300
#define MAX_LINE_SIZE 81
#define MAX_LABEL_SIZE 31
extern int IC, DC, error_flag;
struct pattern {
  char error[MAX_ERROR_SIZE];
  char label[MAX_LINE_SIZE];
  enum { DIRECTIVE, INSTRUCTION, DEFINE, ERROR } type_line;
  struct {
    enum { ENTRY, EXTERN, STRING, DATA } directive_type;
    char data[40][MAX_LINE_SIZE];
	char string[MAX_LINE_SIZE];
	int size;
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
    	enum { IMMEDIATE_NUMBER, DIRECT, DIRECT_INDEX, REGISTER } op_type;
		struct {
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

// struct pattern {
//   char label[MAX_LINE_SIZE];
//   enum { DIRECTIVE, INSTRUCTION, DEFINE, ERROR } type_line;
//   union {
//     char error[MAX_ERROR_SIZE];

//     struct {
//       enum { ENTRY, EXTERN, STRING, DATA } directive_type;
//       char data[40][MAX_LINE_SIZE];
//       char string[MAX_LINE_SIZE];
//       int size;
//     } dir;
//     struct {
//       enum {
//         MOV,
//         CMP,
//         ADD,
//         SUB,
//         LEA,
//         NOT,
//         CLR,
//         INC,
//         DEC,
//         JMP,
//         BNE,
//         RED,
//         PRN,
//         JSR,
//         RTS,
//         HLT
//       } op_type;
//       int num_of_operands;
//       struct {
//         enum { IMMEDIATE_NUMBER, DIRECT, DIRECT_INDEX, REGISTER } op_type;
//         struct {
//           char symbol[MAX_LABEL_SIZE];
//           char const_num[MAX_LABEL_SIZE];
//           int value;
//           enum { r0, r1, r2, r3, r4, r5, r6, r7 } reg;
//         } operand_value;
//       } operands[2];
//       struct code *code;
//     } inst;
//     struct {
//       int value;
//     } def;
//   };
// };

struct node {
  struct pattern *data;
  struct node *next;
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
//node *create_stracts(char *file_name);
extern int num_of_entries;
extern int num_of_externals;
extern int num_of_symbols;
extern int num_of_patterns;
extern External *external_table;
extern Entry *entry_table;
extern Symbol *symbol_table_of_entries;



void first_round(struct node *head);
void secondround(struct node *head);
void to_binary_table();
void translator();
void to_files(char * file_name);
void assembler(char *file_name, struct node * head);