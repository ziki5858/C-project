#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "trie/trie.h"
#include "./assemblerRounds/headers/structs.h"




void print_error_memory(int line, int *error_flag);
void print_error_msg(char *msg, int line, int *error_flag);
extern int num_of_entries;
extern int num_of_externals;
extern int num_of_symbols;
extern int num_of_patterns;
extern int num_of_entries_in_table;


extern External *external_table;
extern Entry *entry_table;
extern Symbol *symbol_table_of_entries;
extern Symbol *symbol_table;
extern Code *code, data;
extern Trie symbols, constant, externals, entries, macro_trie;


/**
 * @brief This function is the main function of the assembler, it calls all the
 * other functions in the program.
 *
 * @param file_name The name of the file that we want to assemble.
 */
void assembler(char *file_name);
