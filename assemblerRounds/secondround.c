#include "../front.h"
#include "../tables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIDTH_OF_WORD 15


extern int IC;
extern int DC;
extern int error_flag;
extern int num_of_codes;

char **binary_table;


/**
 * @brief  check if all the entries are defined
 * 
 */
void validate_entreis() {
  Entry e;
  int i;
  /* loop over the entry table and check if the symbol is defined */
  for (i = 0; i < num_of_entries_in_table; i++) {
    e = entry_table[i];
    if (e->symbol->address == 0) {

      printf("error: in line:%d entry %s definition not found\n", e->line_in_file, e->symbol->label);
      error_flag = 1;
    }
  }
}



void secondround(struct Node *head) {
  char *result, *temp_res;
  int type_ARE, value, i, words =0, j = 0;
  Symbol s;
  validate_entreis();
  
  while (head) {
    if (head->data->type_line == INSTRUCTION) {

      if (head->data->choice.inst.operands[0].op_type == DIRECT ||
          head->data->choice.inst.operands[0].op_type == DIRECT_INDEX ||( head->data->choice.inst.num_of_operands == 2 &&
          (head->data->choice.inst.operands[1].op_type == DIRECT ||
          head->data->choice.inst.operands[1].op_type == DIRECT_INDEX))) {

        for (i = 0; i < head->data->choice.inst.num_of_operands; i++, j = 0) {

          if ((head->data->choice.inst.operands[i].op_type == DIRECT) ||
              (head->data->choice.inst.operands[i].op_type == DIRECT_INDEX)) {
            s = (Symbol)exist_in_trie(
                symbols, head->data->choice.inst.operands[i].operand_value.symbol);
            if (s) {
              if (s->type == S_EXTERN) {
                type_ARE = 1;
                value = 0;

              } else {
                type_ARE = 2;
                value = s->address;
              }
              result = (char*)calloc(WIDTH_OF_WORD, sizeof(char));
              if (result == NULL) {
                printf("error in allocation memory\n");
                error_flag = 1;
                return;
              }
			  temp_res = toBinaryString(value, 12);
              strcpy(result, temp_res);
			  free(temp_res);
			  temp_res = toBinaryString(type_ARE, 2);
              strcat(result, temp_res);
			  free(temp_res);
              if (i == 1)
                strcpy((char *)head->data->choice.inst.code->lines[1], result);
              else {

                if (head->data->choice.inst.num_of_operands == 2) {

                  if (head->data->choice.inst.operands[1].op_type == DIRECT_INDEX)
                    j = 2;
                  else
                    j = 1;
                }
                strcpy((char *)head->data->choice.inst.code->lines[1 + j], result);
                if (type_ARE == 1) {
                  insert_address_to_external(exist_in_trie(externals, s->label),
                                             words + 1 + j);
                }
              }
			  free(result);
            } else {
              printf("error: symbol %s not found\n",
                     head->data->choice.inst.operands[i].operand_value.symbol);
              error_flag = 1;
              return;
            }
          }
        }
      }
      words += head->data->choice.inst.code->num_of_lines;
    }
    head = head->next;
  }
}



void to_binary_table() {
  int i, j, k;
  char **result = (char **)calloc(DC + IC, sizeof(char *));
  if (result == NULL) {
    printf("error in allocation memory\n");
    error_flag = 1;
  }
  for (i = 0, k = 0; i < num_of_codes; i++) {
    for (j = 0; j < code[i]->num_of_lines; j++) {
      result[k] = calloc(WIDTH_OF_WORD, sizeof(char));
      if (result[k] == NULL) {
        printf("error in allocation memory\n");
        error_flag = 1;
      }
      strcat(result[k], (char *)code[i]->lines[j]);
      k++;
    }
  }
  for (i = 0; i < data->num_of_lines; i++) {
    result[i + k] = calloc(WIDTH_OF_WORD, sizeof(char));
    if (result[i + k] == NULL) {
      printf("error in allocation memory\n");
      error_flag = 1;
    }
    strcpy(result[i + k], (char *)data->lines[i]);
  }
  binary_table = result;
}
