/**
 * @file structs_func.c
 * @author shlomo weisz
 * @brief  This file contains the implementation of the functions that used to create the structs of the assembler.
 * the file contains the following functions:
 * 1. create_symbol - a function that creates a symbol.
 * 2. create_constant - a function that creates a constant.
 * 3. create_external - a function that creates an external.
 * 4. insert_address_to_external - a function that inserts an address to an external.
 * 5. create_entry - a function that creates an entry.
 * the file also contains the global variables of the structs:
 * 1. symbols - a trie that contains the symbols of the assembler.
 * 2. constant - a trie that contains the constants of the assembler.
 * 3. externals - a trie that contains the externals of the assembler.
 * 4. entries - a trie that contains the entries of the assembler.
 * 5. code - a struct that contains the code of the assembler.
 * 6. data - a struct that contains the data of the assembler.
 * 7. symbol_table - an array that contains the symbols of the assembler.
 * 8. num_of_symbols - the number of symbols in the symbol table.
 * 9. size_of_symbol_table - the size of the symbol table.
 * 10. external_table - an array that contains the externals of the assembler.
 * 11. num_of_externals_in_table - the number of externals in the external table.
 * 12. entry_table - an array that contains the entries of the assembler.
 * 13. num_of_entries_in_table - the number of entries in the entry table.
 * 14. symbol_table_of_entries - an array that contains the symbols of the entries.
 * 15. num_of_symbols_in_entries - the number of symbols in the symbol table of the entries.
 * @version 0.1
 * @date 2024-03-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "./headers/structs_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Global variables*/
Trie symbols, constant, externals, entries; 

Code *code, data;

Symbol *symbol_table;
int num_of_symbols = 0, size_of_symbol_table = 4;

External *external_table;
int num_of_externals_in_table = 0;

Entry *entry_table;
int num_of_entries_in_table = 0;

Symbol *symbol_table_of_entries;
int num_of_symbols_in_entries = 0;




/**
 * @brief Create a symbol object
 * 
 * @param label the label of the symbol
 * @param address The address of the symbol in the memory
 * @param type The type of the symbol
 * @param line_in_file The line in the file
 * @return Symbol 
 */
Symbol create_symbol(char *label, int address, int type, int line_in_file, int *error_flag, int num_of_data) {
  Symbol s = (Symbol)calloc(1, sizeof(struct symbol));
  if (!s) {
    print_error_memory(line_in_file, error_flag);
    return NULL;
  }
  /* set the values of the symbol */
  strcpy(s->label, label);
  s->address = address;
  s->type = type;
  s->line_in_file = line_in_file;
  s->num_of_data = num_of_data;

  if (insert_to_trie(symbols, label, s)) {
    if (num_of_symbols == size_of_symbol_table) { /* if the symbol table is full, we will allocate more memory */
      symbol_table = (Symbol *)realloc(
          symbol_table, sizeof(Symbol) * (size_of_symbol_table * 2));
      size_of_symbol_table *= 2;
    }
    (symbol_table[num_of_symbols]) = s;
    num_of_symbols++;
    return s;
  }
  print_error_msg("fail to add symbol", line_in_file, error_flag);
  return NULL;
}


/**
 * @brief Create a constant object
 * 
 * @param label the label of the constant
 * @param value The value of the constant
 * @param line_in_file The line in the file
 * @return Constant 
 */
Constant create_constant(char *label, int value, int line_in_file, int *error_flag) {
  Constant c = (Constant)calloc(1, sizeof(struct constant));
  if (!c) {
    print_error_memory(line_in_file, error_flag);
    return NULL;
  }
  /* set the values of the constant */
  c->label = label;
  c->value = value;
  c->line_in_file = line_in_file;
  if (insert_to_trie(constant, label, c)) {
    return c;
  }
  print_error_msg("fail to add constant", line_in_file, error_flag);
  return NULL;
}


/**
 * @brief Create a external object
 * 
 * @param label the label of the external
 * @param line_in_file The line in the file
 * @return External 
 */
External create_external(char *label, int line_in_file, int *error_flag) {
  External e = (External)calloc(1, sizeof(struct external));
  if (!e) {
    print_error_memory(line_in_file, error_flag);
    return NULL;
  }
  /* set the values of the external */
  e->label = label;
  e->line_in_file = line_in_file;
  e->addresses = (int *)calloc(1, sizeof(int));
  
  if (insert_to_trie(externals, label, e)) {
    external_table[num_of_externals_in_table++] = e; /* add the external to the table */
    return e;
  }
  print_error_msg("fail to add external", line_in_file, error_flag);
  return NULL;
}


/**
 * @brief Insert address to external 
 * 
 * @param e the external to insert the address
 * @param address The address to insert
 */
void insert_address_to_external(External e, int address) {
  e->addresses =
      (int *)realloc(e->addresses, sizeof(int) * (e->number_of_addresses + 1));
  e->addresses[e->number_of_addresses] = address;
  e->number_of_addresses++;
}


/**
 * @brief Create a entry object
 * 
 * @param symbol the symbol of the entry
 * @param line_in_file The line in the file
 * @return Entry 
 */
Entry create_entry(Symbol symbol, int line_in_file) { /*למ הוגדר ככה בסימבול?*/
  Entry e = (Entry)calloc(1, sizeof(struct entry));
  /* set the values of the entry */
  e->symbol = symbol;
  e->line_in_file = line_in_file;
  if (insert_to_trie(entries, symbol->label, e)) {
    entry_table[num_of_entries_in_table++] = e; /* add the entry to the table */
    return e;
  }
  return NULL;
}
