/*#include "../tables.h"*/

#include "../front.h"
#include "encoder.h"
#include "structs_func.h"
#include "validation.h"
#include "bilder.h"
#include <stdio.h>
#include <stdlib.h>
#define WIDTH_OF_WORD 15 


extern int num_of_patterns; 


int error_flag = 0;
int num_of_codes = 0;
int current_pattern_num = 0;
int IC = 0, DC = 0;
char **binary_table;


/**
 * @brief The first round of the assembler
 *
 * @param head The head of the linked list
 */
void first_round(struct Node *head) {
  struct Node *current_pattern = head;

  /*
  initialize the tries
  */
  symbols = trie();
  externals = trie();
  entries = trie();
  constant = trie();

  /* initialize the code and data tables */
  code = (Code *)calloc(num_of_patterns, sizeof(Code));
  data = (Code)calloc(1, sizeof(struct code));

  /* initialize the symbol, entry and external tables */
  symbol_table = (Symbol *)calloc(size_of_symbol_table, sizeof(Symbol));
  entry_table = (Entry *)calloc(num_of_entries, sizeof(Entry));
  symbol_table_of_entries = (Symbol *)calloc(num_of_entries, sizeof(Symbol));
  external_table = (External *)calloc(num_of_externals, sizeof(External));
  

  for (current_pattern_num = 1; current_pattern_num <= num_of_patterns;
       current_pattern = current_pattern->next, current_pattern_num++) {
    /*
    dealing with labels
    check if the label is already exist in the tries

    */
    if (current_pattern->data->label[0]) {
      handle_lable_error(current_pattern->data, current_pattern_num);
    }
    /* check the type of the line */
    switch (current_pattern->data->type_line) {

    /* dealing with syntax errors */
    case ERROR:
      printf("%s\n", current_pattern->data->choice.error);
      error_flag = 1;
      break;

    case DIRECTIVE:
		directive(current_pattern,  current_pattern_num, &DC);
      break;

    case DEFINE:
      define(current_pattern, current_pattern_num);
      break;

    case INSTRUCTION:

      instruction(current_pattern, current_pattern_num, &IC);
		  break;}
  }
  add_IC_to_symbol_table(IC);
}










/**
 * @brief The second round of the assembler.
 * now we have all the symbols and we can encode the instructions of direct operands
 * 
 * @param head The head of the linked list
 */
void secondround(struct Node *head) {
  char *result;
  int type_ARE, value, i, words =0, j = 0;
  Symbol s;
  /* validate that all the entries are defined */
  validate_entreis();
  
  while (head) {
    if (head->data->type_line == INSTRUCTION) {

      if (there_is_direct(head)) {

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
              
			  result = encoded_direct(value, type_ARE);
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