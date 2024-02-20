#include "../trie/trie.h"

#include "structs.h"
extern int num_of_symbols_in_entries,size_of_symbol_table;



/**
 * @brief Create a symbol object
 * 
 * @param label the label of the symbol
 * @param address The address of the symbol in the memory
 * @param type The type of the symbol
 * @param line_in_file The line in the file
 * @return Symbol 
 */
Symbol create_symbol(char *label, int address, int type, int line_in_file);


/**
 * @brief Create a constant object
 * 
 * @param label the label of the constant
 * @param value The value of the constant
 * @param line_in_file The line in the file
 * @return Constant 
 */
Constant create_constant(char *label, int value, int line_in_file);


/**
 * @brief Create a external object
 * 
 * @param label the label of the external
 * @param line_in_file The line in the file
 * @return External 
 */
External create_external(char *label, int line_in_file);



/**
 * @brief Insert address to external
 * 
 * @param e the external
 * @param address The address to insert
 */
void insert_address_to_external(External e, int address);


/**
 * @brief Create a entry object
 * 
 * @param symbol the symbol of the entry
 * @param line_in_file The line in the file
 * @return Entry 
 */
Entry create_entry(Symbol symbol, int line_in_file);



