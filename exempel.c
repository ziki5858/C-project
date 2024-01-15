#include "front.h"
#include "tables.h"
#include <string.h>
int num_of_patterns = 7;
int num_of_symbols = 0;
int num_of_entries = 0;
int num_of_externals = 0;
int main() {
  Pattern p = (struct pattern *)calloc(7 ,sizeof(struct pattern));
  	
	

  p[0].type_line = INSTRUCTION;
  p[0].inst.op_type = MOV;
  p[0].inst.num_of_operands = 2;
  p[0].inst.operands[0].op_type = DIRECT;
  strcpy(p[0].inst.operands[0].operand_value.symbol, "symbol1");
  p[0].inst.operands[1].op_type = DIRECT;
  strcpy(p[0].inst.operands[1].operand_value.symbol, "symbol2");

  p[1].type_line = DEFINE;
  p[1].def.value = 5;
  strcpy(p[1].label, "define1");

  p[2].type_line = DIRECTIVE;
  p[2].dir.directive_type = DATA;
  p[2].dir.size = 3;
  strcpy(p[2].dir.data[0], "1");
  strcpy(p[2].dir.data[1], "define1");
  strcpy(p[2].dir.data[2], "55");

  p[3].type_line = DIRECTIVE;
  p[3].dir.directive_type = EXTERN;
  strcpy(p[3].label, "extern2");

  p[4].type_line = DIRECTIVE;
  p[4].dir.directive_type = DATA;
  p[4].dir.size = 3;
  strcpy(p[4].dir.data[0], "1");
  strcpy(p[4].dir.data[1], "444");
  strcpy(p[4].dir.data[2], "4441");
  strcpy(p[4].label, "symbol1");

  p[5].type_line = DIRECTIVE;
  p[5].dir.directive_type = EXTERN;
  strcpy(p[5].label, "extern1");

  p[6].type_line = INSTRUCTION;
  p[6].inst.op_type = MOV;
  p[6].inst.num_of_operands = 2;
  p[6].inst.operands[0].op_type = DIRECT;
  strcpy(p[6].inst.operands[0].operand_value.symbol, "symbol1");
  p[6].inst.operands[1].op_type = DIRECT;
  strcpy(p[6].inst.operands[1].operand_value.symbol, "extern1");
  strcpy(p[6].label, "symbol2");
  num_of_symbols = 4;
  num_of_entries = 0;
  num_of_externals = 2;
printf("%d\n",sizeof(struct pattern));
  struct node *head = NULL;
  struct node *tail = NULL;
  struct node *temp = NULL;
  int i;
  for (i = 0; i < 7; i++) {
    temp = (struct node *)malloc(sizeof(struct node));
    temp->data = &p[i];
    temp->next = NULL;
    if (head == NULL) {
      head = temp;
      tail = temp;
    } else {
      tail->next = temp;
      tail = temp;
    }
  }
  tail->next = NULL;
  assembler("ssss", head);
  return 0;
}