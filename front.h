#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ERROR_SIZE 300
#define MAX_LINE_SIZE 300
#define MAX_LABEL_SIZE 31

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

struct node {
  struct pattern *data;
  struct Node *next;
};

node *create_stracts(char *file_name);
