#include "structs_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Trie symbols, constant, externals, entries; /*אולי להוסיף עץ בשם לבהירות*/

Code *code, data;

Symbol *symbol_table;
int num_of_symbols = 0, size_of_symbol_table = 4;

External *external_table;
int num_of_externals_in_table = 0;

Entry *entry_table;
int num_of_entries_in_table = 0;

Symbol *symbol_table_of_entries;
int num_of_symbols_in_entries = 0;

/* Constant *constant_table;
 int num_of_constants_in_table = 0;
 int size_in_constant_table = 4;*/


/**
 * @brief Create a symbol object
 * 
 * @param label the label of the symbol
 * @param address The address of the symbol in the memory
 * @param type The type of the symbol
 * @param line_in_file The line in the file
 * @return Symbol 
 */
Symbol create_symbol(char *label, int address, int type, int line_in_file) {
  Symbol s = (Symbol)calloc(1, sizeof(struct symbol));
  if (!s) {
    print_error_memory(line_in_file);
    return NULL;
  }
  strcpy(s->label, label);
  s->address = address;
  s->type = type;
  s->line_in_file = line_in_file;

  if (insert_to_trie(symbols, label, s)) {
    if (num_of_symbols == size_of_symbol_table) {
      symbol_table = (Symbol *)realloc(
          symbol_table, sizeof(Symbol) * (size_of_symbol_table * 2));
      size_of_symbol_table *= 2;
    }
    (symbol_table[num_of_symbols]) = s;
    num_of_symbols++;
    return s;
  }
  print_error_msg("fail to add symbol", line_in_file);
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
Constant create_constant(char *label, int value, int line_in_file) {
  Constant c = (Constant)calloc(1, sizeof(struct constant));
  if (!c) {
    print_error_memory(line_in_file);
    return NULL;
  }
  c->label = label;
  c->value = value;
  c->line_in_file = line_in_file;
  if (insert_to_trie(constant, label, c)) {
    return c;
  }
  print_error_msg("fail to add constant", line_in_file);
  return NULL;
}


/**
 * @brief Create a external object
 * 
 * @param label the label of the external
 * @param line_in_file The line in the file
 * @return External 
 */
External create_external(char *label, int line_in_file) {
  External e = (External)calloc(1, sizeof(struct external));
  if (!e) {
    print_error_memory(line_in_file);
    return NULL;
  }
  e->label = label;
  e->line_in_file = line_in_file;
  e->addresses = (int *)calloc(1, sizeof(int));
  
  if (insert_to_trie(externals, label, e)) {
    external_table[num_of_externals_in_table++] = e;
    return e;
  }
  print_error_msg("fail to add external", line_in_file);
  return NULL;
}


/**
 * @brief Insert address to external
 * 
 * @param e the external
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
  e->symbol = symbol;
  e->line_in_file = line_in_file;
  if (insert_to_trie(entries, symbol->label, e)) {
    entry_table[num_of_entries_in_table++] = e;
    return e;
  }
  return NULL;
}
