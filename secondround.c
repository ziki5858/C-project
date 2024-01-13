#include "front.h"
#include "tables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIDTH_OF_WORD 15

extern Code *code;
extern Code data;
extern Trie symbols, constant, externals;
extern int IC;
extern int DC;
extern int error_flag;
extern int num_of_codes;
extern Entry * entry_table;

char **binary_table;
// int words = IC + DC;

void secondround(struct node *head) {
	int type_ARE, value, i, words, j;
	Symbol s;
  while (head) {
    if (head->data->code) {
      

	  if (head->data->inst.operands[0].op_type == DIRECT || head->data->inst.operands[0].op_type == DIRECT_INDEX
		  || head->data->inst.operands[1].op_type == DIRECT || head->data->inst.operands[1].op_type == DIRECT_INDEX) {
     
	  for (i = 0; i < head->data->inst.num_of_operands; i++, j=0) {
		
        if ((head->data->inst.operands[i].op_type == DIRECT) ||
            (head->data->inst.operands[i].op_type == DIRECT_INDEX)) {
          s = (Symbol)exist_in_trie(
              symbols, head->data->inst.operands[i].operand_value.symbol);
          if (s) {
			if (s->type == EXTERN) {
				type_ARE = 1;
				value = 0;
				
			}
			else {
				type_ARE = 2;
				value = s->address;
			}
            char *result = calloc(WIDTH_OF_WORD, sizeof(char));
            if (result == NULL) {
              printf("error in allocation memory\n");
              error_flag = 1;
              return;
            }
            strcpy(result, toBinaryString(value, 12));
            strcat(result, toBinaryString(type_ARE, 2));
            if (i == 1)
              head->data->code->lines[1] = result;
            else {
              
			  if (head->data->inst.num_of_operands == 2) {
				
					if (head->data->inst.operands[1].op_type == DIRECT_INDEX)
                          j = 2;
					else 
						  j = 1;
			  }
              head->data->code->lines[1 + j] =
                  result;
			if(type_ARE == 1){	  
			insert_address_to_external(exist_in_trie(externals, s->label)), words + 2 + j}  
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
      words += head->data->code->num_of_lines;
    }
	head = head->next;
  }
}


void validate_entreis(){
	Entry e;
	e = entry_table[0];
	while (e){
		if (e->symbol->address == 0){
			printf("error: entry %s definition not found\n", e->symbol->label);
			error_flag = 1;
		}
		e = e->next;
	}
}


void to_binary_table(){
	int i,j,k;
	char **result = (char **)calloc(DC-100, sizeof(char*));
	if (result == NULL){
		printf("error in allocation memory\n");
		error_flag = 1;
		
	}
	for (i = 0,k=0; i < num_of_codes; i++){
		for (j = 0; j < code[i]->num_of_lines; j++){
			result[k] = calloc(WIDTH_OF_WORD, sizeof(char));
			if (result[k] == NULL){
				printf("error in allocation memory\n");
				error_flag = 1;
				
			}
			strcat(result[k], code[i]->lines[j]);
			k++;
		}
	}
	for (i = 0; i < data->num_of_lines; i++){
		result[i + k] = calloc(WIDTH_OF_WORD, sizeof(char));
		if (result[i + k] == NULL){
			printf("error in allocation memory\n");
			error_flag = 1;
		}
		strcpy(result[i + k], data->lines[i]);
	}
	binary_table = result;
}
