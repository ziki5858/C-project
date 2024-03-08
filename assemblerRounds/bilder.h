#include "../front.h"
#include "../trie/trie.h"
#include "encoder.h"
#include "structs.h"
#include "structs_func.h"
#include "validation.h"
#define MAX_DATA_VALUE 8191
#define MIN_DATA_VALUE -8192

extern int num_of_codes;

/**
 * @brief  This function is used to handle the case of directive. 
 * it will call the right function to handle the directive.
 * 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param DC  The data counter.
 */
void directive(struct Node *current_pattern, int current_pattern_num, int *DC);

/**
 * @brief  This function is used to handle the case of extern definition. 
 * it will insert the extern to the extern table.
 * 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 */
void extern_def(struct Node *current_pattern, int current_pattern_num);


/**
 * @brief  This function is used to handle the case of entry definition. 
 * it will insert the entry to the entry table.
 * 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 */
void entry_def(struct Node *current_pattern, int current_pattern_num);


/**
 * @brief  This function is used to handle the case of string definition. 
 * it will insert the data to the data table.
 * 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param DC  The data counter.
 */
void string_def(struct Node *current_pattern, int current_pattern_num, int *DC);


/**
 * @brief  This function is used to handle the case of data definition. 
 * it will insert the data to the data table.
 * 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param DC  The data counter.
 */
void data_def(struct Node *current_pattern, int current_pattern_num, int *DC);


/**
 * @brief  This function is used to handle the case of define.
 * 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 */
void define(struct Node *current_pattern, int current_pattern_num);


/**
 * @brief  This function is used to handle the case of instruction.
 * it will insert the instruction to the code table, and encode the instruction.
 * 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param IC  The instruction counter.
 */
void instruction(struct Node *current_pattern, int current_pattern_num, int *IC);



/**
 * @brief  This function is used to handle the case of target operand.
 * it will encode the target operand, based on the type of the operand. 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 */
void target_operand(struct Node *current_pattern, int current_pattern_num, int *num_of_word_nedded);


/**
 * @brief  This function is used to handle the case of source operand.
 * it will encode the source operand, based on the type of the operand. 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 */
void source_operand(struct Node *current_pattern, int current_pattern_num, int *num_of_word_nedded);



/**
 * @brief  This function is used to handle the case of immediate operand.
 * it will encode the immediate operand, based on the type of the operand. 
 * @param current_pattern  The current pattern that we want to handle.
 * @param operand_num  The number of the operand.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 */
void immediate(struct Node *current_pattern, int operand_num, int *num_of_word_nedded);



/**
 * @brief  This function is used to handle the case of direct index operand.
 * it will encode the direct index operand, based on the type of the operand. 
 * @param current_pattern  The current pattern that we want to handle.
 * @param operand_num  The number of the operand.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 * @param current_pattern_num  The number of the current pattern.
 */
void direct_index(struct Node *current_pattern, int operand_num, int *num_of_word_nedded, int current_pattern_num);



/**
 * @brief  This function is used to handle the case of direct operand.
 * it will encode the direct operand, based on the type of the operand. 
 * @param current_pattern  The current pattern that we want to handle.
 * @param operand_num  The number of the operand.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 */
void direct(struct Node *current_pattern, int operand_num, int *num_of_word_nedded);



/**
 * @brief  This function is used to handle the case of register.
 * it will encode the register, based on the type of the operand. 
 * @param current_pattern  The current pattern that we want to handle.
 * @param operand_num  The number of the operand.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 */
void register_encoder(struct Node *current_pattern, int operand_num, int *num_of_word_nedded);



/**
 * @brief  This function is used to handle the case of tow registers.
 * it will encode the tow registers, and insert them to the code table.
 * 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 */
void tow_registers(struct Node *current_pattern, int current_pattern_num, int *num_of_word_nedded);
