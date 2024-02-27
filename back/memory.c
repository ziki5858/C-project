#include "../front.h"
#include "../trie/trie.h"
#include <stdlib.h>

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

void destroy_constant(void *constant_i) {
  Constant c = (Constant)constant_i;
  free(c);
}



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
 * @brief This function used to free all the memory that was allocated during the initialization of the memory.
 * 
 */
void free_memory(struct Node *head) {
  int i,j;

  /* free the code array, and all the lines in it */
  
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

  /* free the tries */
  destroy_trie(&symbols);
  destroy_trie_with_ptr(&constant, destroy_constant);
  destroy_trie_with_ptr(&externals, destroy_extern);
  destroy_trie(&entries);
  destroy_trie_with_ptr(&macro_trie, destroy_macro);

   for (i = 0; i < num_of_symbols; i++) {
     free(symbol_table[i]);
   }
  /* free the tables */
  free(symbol_table);
  
  free(symbol_table_of_entries);

   for (i = 0; i < num_of_entries; i++) {
     free(entry_table[i]);
   }
  free(entry_table);
  
  
/*   extern int num_of_constants_in_table;
   extern Constant *constant_table;
   for (i = 0; i < num_of_constants_in_table; i++) {
     free(constant_table[i]);
   }
   free(constant_table);

   for (i = 0; i < num_of_externals; i++) {
     free(external_table[i]->addresses);
     free(external_table[i]);
   }*/
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
  /* free the linked list */
  destroy_linked_list(head);
}

