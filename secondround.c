#include "front.h"
#include "tables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIDTH_OF_WORD 15

extern Code *code;
extern Code data;
extern Trie symbols, constant;
extern int IC;
extern int DC;
extern int error_flag;
extern int num_of_codes;
extern Entry * entry_table;

char **binary_table;
// int words = IC + DC;

void secondround(struct node *head) {
  while (head) {
    if (head->code) {
      int i;
      for (i = 0; i < head->data->inst.num_of_operands; i++) {
        if ((head->data->inst.operands[i].op_type == DIRECT) ||
            (head->data->inst.operands[i].op_type == DIRECT_INDEX)) {
          Symbol s = (Symbol)exist_in_trie(
              symbols, head->data->inst.operands[i].operand_value.symbol);
          if (s) {
            int type_ARE = (s->type == EXTERN) ? 1 : 2;
            int value = s->address;
            char *result = calloc(WIDTH_OF_WORD, sizeof(char));
            if (result == NULL) {
              printf("error in allocation memory\n");
              error_flag = 1;
              return;
            }
            strcat(result, toBinaryString(value, 12));
            strcat(result, toBinaryString(type_ARE, 3));
            if (i == 0)
              head->data->code->lines[1] = result;
            else {
              int j = (head->data->inst.operands[1].op_type == DIRECT_INDEX)
                          ? 1
                          : 0;
              head->data->code->lines[head->data->code->num_of_lines - j] =
                  result;
            }
          } else {
            printf("error: symbol %s not found\n",
                   head->data->inst.operands[i].operand_value.symbol);
            error_flag = 1;
            return;
          }
        }
      }
      
    }
	head = head->next;
  }
}


void validate_entreis(){
	Entry e;
	e = entry_table[0];
	while (e){
		if (e->symbol->address == 0){
			printf("error: entry %s not found\n", e->symbol->label);
			error_flag = 1;
		}
		e = e->next;
	}


void to_binary_table(){
	int i,j;
	char **result = calloc(DC-100, sizeof(char*));
	if (result == NULL){
		printf("error in allocation memory\n");
		error_flag = 1;
		
	}
	for (i = 0; i < num_of_codes; i++){
		for (j = 0; j < code[i]->num_of_lines; j++){
			result[i] = calloc(WIDTH_OF_WORD, sizeof(char));
			if (result[i] == NULL){
				printf("error in allocation memory\n");
				error_flag = 1;
				
			}
			strcat(result[i], code[i]->lines[j]);
		}
	}
	for (i = 0; i < data->num_of_lines; i++){
		result[i + num_of_codes] = calloc(WIDTH_OF_WORD, sizeof(char));
		if (result[i + num_of_codes] == NULL){
			printf("error in allocation memory\n");
			error_flag = 1;
		}
		strcat(result[i + num_of_codes], data->lines[i]);
	}
	binary_table = result;
}
