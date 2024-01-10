#include "front.h"
int num_of_patterns;
int num_of_symbols;
int num_of_entries;
int num_of_externals;

struct pattern {
  char *error[MAX_ERROR_SIZE];
  char *label[MAX_LINE_SIZE];
  enum { DIRECTIVE, INSTRUCTION, DEFINE, ERROR } type_line;
  struct {
    enum { ENRTY, EXTERN, STRING, DATA } directive_type;
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
    	enum { IMMEDIATE_NUMBER, IMMEDIATE_CONSANT, DIRECT, DIRECT_INDEX, REGISTER } op_type;
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

struct node {
  struct pattern *data;
  struct Node *next;
};