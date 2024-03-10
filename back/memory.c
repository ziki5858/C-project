/**
 * @file memory.c
 * @author shlomo weisz
 * @brief  This file contains the implementation of the functions that used to handle the memory of the assembler.
 * the file contains the following functions:
 * 1. free_memory - a function that used to free all the memory that was allocated during the initialization of the memory.
 * 2. destroy_linked_list - a function that used to free all the memory that was allocated during the initialization of the linked list of struct node that contain the struct pattern.
 * 3. free_tables_codes - a function that used to free all the memory that was allocated during the initialization of the code and data tables.
 * 4. free_tries - a function that used to free all the memory that was allocated during the initialization of the tries.
 * 5. destroy_extern - a function that used to free all the memory that was allocated during the initialization of the extern.
 * 6. destroy_macro - a function that used to free all the memory that was allocated during the initialization of the macro.
 * 7. destroy_constant - a function that used to free all the memory that was allocated during the initialization of the constant.
 * 8. free_other_tables - a function that used to free all the memory that was allocated during the initialization of the other tables,
 * such as the symbol table, the entry table, the external table, the binary table and the translated binary table.
 * @version 0.1
 * @date 2024-03-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "../front.h"
#include "../trie/trie.h"
#include "memory.h"
#include <stdlib.h>


/**
 * @brief This function used to free all the memory that was allocated during the initialization of the memory.
 * 
 */
void free_memory(struct Node *head, int error_flag, int IC, int DC, char **binary_table, char **binary_table_translated) {

  /* free the code array, and all the lines in it */
  
  free_tables_codes(DC);

  /* free the tries */
  free_tries();

   free_other_tables(error_flag, IC, DC, binary_table, binary_table_translated);
   
  /* free the linked list */
  destroy_linked_list(head);
}


/**
 * @brief This function used to free all the memory that was allocated during the initialization
 * of the linked list of struct node that contain the struct pattern.
 * 
 * @param head The head of the linked list.
 */
void destroy_linked_list(struct Node *head) {
  struct Node *temp;
  int i;
  while (head) {
	temp = head;
	head = head->next;
	if ((int)temp->data->type_line == (int)DIRECTIVE) {
	/* if there is 2d data in the node, free it */
	if ((int)temp->data->choice.dir.directive_type == (int)DATA) {
	for(i = 0; i < temp->data->choice.dir.size; i++) {
		free(temp->data->choice.dir.data[i]);	
	}
	free(temp->data->choice.dir.data);
	}
	/* free the string in the node */
	if ((int)temp->data->choice.dir.directive_type == (int)STRING) {
		free(temp->data->choice.dir.string);
	}
	}
	free(temp->data);
	free(temp);
  }
}


/**
 * @brief This function used to free all the memory that was allocated during the initialization of the code and data tables.
 * 
 * @param DC The data counter
 */
void free_tables_codes(int DC){
	int i,j;
	extern int num_of_codes;
  for (i = 0; i < num_of_codes; i++) {
    for (j = 0; j < code[i]->num_of_lines; j++) {
      free(code[i]->lines[j]);
    }
    free(code[i]->lines);
    free(code[i]);
  }
  free(code);

  /* free the data array, and all the lines in it */
  
  for (i = 0; i < DC; i++) {
    free(data->lines[i]);
  }
  free(data->lines);
  free(data);
  
}


/**
 * @brief This function used to free all the memory that was allocated during the initialization of the tries.
 * 
 */
void free_tries(){
	destroy_trie(&symbols);
  destroy_trie_with_ptr(&constant, destroy_constant);
  destroy_trie_with_ptr(&externals, destroy_extern);
  destroy_trie(&entries);
  destroy_trie_with_ptr(&macro_trie, destroy_macro);
}


/**
 * @brief This function used to free all the memory that was allocated during the initialization of the extern.
 * 
 * @param extern_i The extern that we want to free.
 */
void destroy_extern(void *extern_i) {
	External e = (External)extern_i;
  free(e->addresses);
  free(e);
}


/**
 * @brief This function used to free all the memory that was allocated during the initialization of the macro.
 * 
 * @param macro_i The macro that we want to free.
 */
void destroy_macro(void *macro_i) {
  Macro m = (Macro)macro_i;
  free(m->value);
  free(m);
}


/**
 * @brief This function used to free all the memory that was allocated during the initialization of the constant.
 * 
 * @param constant_i The constant that we want to free.
 */
void destroy_constant(void *constant_i) {
  Constant c = (Constant)constant_i;
  free(c);
}



/**
 * @brief This function used to free all the memory that was allocated during the initialization of the other tables, 
 * such as the symbol table, the entry table, the external table, the binary table and the translated binary table.
 * 
 * @param error_flag The error flag
 * @param IC The instruction counter
 * @param DC The data counter
 * @param binary_table The binary table
 * @param binary_table_translated The translated binary table
 */
void free_other_tables(int error_flag, int IC, int DC, char **binary_table, char **binary_table_translated){
	int i;
  /* free the tables */
  for (i = 0; i < num_of_symbols; i++) {
     free(symbol_table[i]);
   }
  
  free(symbol_table);
  
  free(symbol_table_of_entries);

   for (i = 0; i < num_of_entries; i++) {
     free(entry_table[i]);
   }
  free(entry_table);
	free(external_table);

	if (error_flag == 0) {
	/* free the binary table */
  for (i = 0; i < IC + DC; i++) {
    free(binary_table[i]);
    free(binary_table_translated[i]);
  }
  free(binary_table);
  free(binary_table_translated);
  }
}





