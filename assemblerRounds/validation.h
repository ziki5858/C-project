#include "../front.h"
#include "structs_func.h"
extern int error_flag;

/**
 * @brief Print error message
 *
 * @param msg The message to print
 * @param line The line in the file
 */
void print_error_memory(int line);


/**
 * @brief Print error message
 *
 * @param msg The message to print
 * @param line The line in the file
 */
void print_error_msg(char *msg, int line);


/**
 * @brief Add IC to symbol table, used for the second round
 *
 * @param IC The IC to add
 */
void add_IC_to_symbol_table(int IC);



/**
 * @brief Check validation of operands number
 *
 * @param instruction The instruction to check
 * @return int 1 if valid, 0 otherwise
 */
int check_validation_of_operands_num(struct pattern *instruction);



/**
 * @brief Check validation of operands type
 *
 * @param instruction The instruction to check
 * @return int 1 if valid, 0 otherwise
 */
int check_validation_of_operands_type(struct pattern *instruction);



/**
 * @brief Extract immidiate number from the instruction
 *
 * @param p The instruction
 * @param num_of_operand The number of the operand
 * @return int The immidiate number
 */
int extract_immidiate_number(struct pattern *p, int num_of_operand, int current_pattern_num);



/**
 * @brief Check if the word is a saved word
 *
 * @param word The word to check
 * @return int 1 if saved word, 0 otherwise
 */
int is_saved_word(char *word);



/**
 * @brief Determaine the sign of the number
 *
 * @param buffer The buffer to check
 * @return int 1 if positive, -1 if negative, 0 otherwise
 */
int determaine_sign(char *buffer);


/**
 * @brief Check if the string contains only digits
 *
 * @param str The string to check
 * @return int 1 if contains only digits, 0 otherwise
 */
int containsOnlyDigits(const char *str);



/**
 * @brief Handle lable error by printing an error message based on the error
 *
 * @param p The pattern to check
 */
void handle_lable_error(struct pattern *p, int current_pattern_num);



/**
 * @brief Handle lable define by adding the lable to the symbol table
 *
 * @param p The pattern to check
 * @param type The type of the lable
 * @param counter The counter to add
 */
void handle_lable_define(struct pattern *p, char *type, int counter, int current_pattern_num);


/**
 * @brief Allocate memory for instructions in the pattern struct, according to the number
 * of operands and the type of the operands in the instruction
 *
 * @param ins The instruction to allocate memory for
 * @param code_i The code to allocate memory for
 */
void allocate_memory_for_instructions(struct pattern *ins, Code *code_i, int current_pattern_num);


/**
 * @brief Allocate memory for data
 *
 * @param data_i The data to allocate memory for
 */
void allocate_memory_for_data(struct pattern *data_i, int current_pattern_num);


/**
 * @brief  check if all the entries are defined
 * 
 */
void validate_entreis();


/**
 * @brief check if there is a direct operand in the instruction.
 * 
 * @param head The head of the linked list
 * @return int 1 if there is a direct operand, 0 otherwise
 */
int there_is_direct(struct Node *head);