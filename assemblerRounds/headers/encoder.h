#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "../../mainHeader.h"
#define WIDTH_OF_WORD 15



extern int current_pattern_num;
extern int num_of_codes;
/**
 * @brief Convert number to binary string
 *
 * @param n The number to convert
 * @param num_bits The number of bits
 * @return char* The binary string
 */
char *toBinaryString(int n, int num_bits);


/**
 * @brief Encoded register
 * 
 * @param n The register number
 * @return char* The encoded register
 */
char *encoded_register(int n);


/**
 * @brief Encoded registers
 * 
 * @param n1 The first register number
 * @param n2 The second register number
 * @return char* The encoded registers
 */
char *encoded_registers(int n1, int n2);



/**
 * @brief Encoded data
 * 
 * @param n The data to encode
 * @return char* The encoded data
 */
char *encoded_data(int n);



/**
 * @brief Encoded immidiate number
 * 
 * @param n The immidiate number to encode
 * @return char* The encoded immidiate number
 */
char *encoded_immidiate_number(int n, int *error_flag, int current_pattern_num);



/**
 * @brief Encoded instruction based on the pattern of the instruction and the operands
 * 
 * @param instruction The instruction to encode
 * @return char* The encoded instruction
 */
char *encoded_instruction(struct pattern *instruction, int *error_flag, int current_pattern_num);


/**
 * @brief  encode the direct operand, by converting the value to binary and adding the ARE flag
 * 
 * @param value  the value of the operand
 * @param are_flag  the ARE flag
 * @return char*  the encoded operand
 */
char *encoded_direct(int value, int are_flag, int *error_flag);


