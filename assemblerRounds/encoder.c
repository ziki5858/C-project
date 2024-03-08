/**
 * @file encoder.c
 * @author shlomo weisz
 * @brief  This file contains the implementation of the functions that used to encode the instructions and the operands of the assembler.
 * the file contains the following functions:
 * 1. toBinaryString - a function that converts a number to a binary string.
 * 2. encoded_register - a function that encodes a register.
 * 3. encoded_registers - a function that encodes two registers.
 * 4. encoded_data - a function that encodes data.
 * 5. encoded_immidiate_number - a function that encodes an immidiate number.
 * 6. encoded_instruction - a function that encodes an instruction.
 * @version 0.1
 * @date 2024-03-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "encoder.h"
/**
 * @brief Convert number to binary string
 *
 * @param n The number to convert
 * @param num_bits The number of bits
 * @return char* The binary string
 */
char *toBinaryString(int n, int num_bits) {
  int i;
  char *string = malloc(num_bits + 1);
  if (!string) {
    return NULL;
  }
  /* convert the number to binary */
  for (i = num_bits - 1; i >= 0; i--) {
    string[i] = (n & 1) + '0';
    n >>= 1;
  }
  string[num_bits] = '\0';
  return string;
}


/**
 * @brief Encoded register
 * 
 * @param n The register number
 * @return char* The encoded register
 */
char *encoded_register(int n) { return toBinaryString(n, 3); }


/**
 * @brief Encoded registers
 * 
 * @param n1 The first register number
 * @param n2 The second register number
 * @return char* The encoded registers
 */
char *encoded_registers(int n1, int n2) {
  char *result = malloc(WIDTH_OF_WORD),*reg1,*reg2; /*now its 15*/
  int i;
  if (!result) {
    return NULL;
  }
  result[WIDTH_OF_WORD - 1] = '\0';
  

  /* initialize result to 0 */
  for (i = 0; i < 14; i++) {
    result[i] = '0';
  }

  reg1 = encoded_register(n1);
  reg2 = encoded_register(n2);

  for (i = 0; i < 3; i++) {
    result[i + 6] = reg1[i];
    result[i + 9] = reg2[i];
  }
  free(reg1);
  free(reg2);
  return result;
}


/**
 * @brief Encoded data
 * 
 * @param n The data to encode
 * @return char* The encoded data
 */
char *encoded_data(int n) {
  return toBinaryString(n, WIDTH_OF_WORD - 1);
} /*now 15*/


/**
 * @brief Encoded immidiate number
 * 
 * @param n The immidiate number to encode
 * @return char* The encoded immidiate number
 */
char *encoded_immidiate_number(int n) {
  char *base = toBinaryString(n, 12);
  char *result = calloc(WIDTH_OF_WORD, sizeof(char));
  if (!result) {
    print_error_memory(current_pattern_num);
    return NULL;
  }
  strcat(result, base);
  free(base);
  result[WIDTH_OF_WORD - 1] = '\0';
  result[12] = '0'; /* A flag for immidiate number */
  result[13] = '0';
  return result;
}


/**
 * @brief Encoded instruction based on the pattern of the instruction and the operands
 * 
 * @param instruction The instruction to encode
 * @return char* The encoded instruction
 */
char *encoded_instruction(struct pattern *instruction) {
  char *result = malloc(WIDTH_OF_WORD), *op_code;
  int i, j;
  if (!result) {
    print_error_memory(current_pattern_num);
    return NULL;
  }
  result[WIDTH_OF_WORD - 1] = '\0';
  

  /* initialize result to 0 */
  for (i = 0; i < 14; i++) {
    result[i] = '0';
  }

  op_code = toBinaryString(instruction->choice.inst.op_type, 4);

  for (i = 0; i < 4; i++) {
    result[i + 4] = op_code[i];
  }
  free(op_code);
  /*assumming that the first operand is the target operand
   */
  for (i = 0; i < instruction->choice.inst.num_of_operands; i++) {
    char *operand =
        toBinaryString(instruction->choice.inst.operands[i].op_type, 2);
    for (j = 0; j < 2; j++) {
      result[i * -2 + j + 10] = operand[j];
    }
    free(operand);
  }

  return result;
}


/**
 * @brief  encode the direct operand, by converting the value to binary and adding the ARE flag
 * 
 * @param value  the value of the operand
 * @param are_flag  the ARE flag
 * @return char*  the encoded operand
 */
char *encoded_direct(int value, int are_flag){
	char *result, *temp_res;
	result = (char*)calloc(WIDTH_OF_WORD, sizeof(char));
              if (result == NULL) {
                printf("error in allocation memory\n");
                error_flag = 1;
                return NULL;
              }
	temp_res = toBinaryString(value, 12);
    strcpy(result, temp_res);
	free(temp_res);
	temp_res = toBinaryString(are_flag, 2);
    strcat(result, temp_res);
	free(temp_res);
	return result;
}


/**
 * @brief convert the code and data tables to binary table
 * 
 */
void to_binary_table() {
  int i, j, k;
  /* allocate memory for the binary table */
  char **result = (char **)calloc(DC + IC, sizeof(char *));
  if (result == NULL) {
    printf("error in allocation memory\n");
    error_flag = 1;
  }
  /* loop over the code table and copy the lines to the binary table */
  for (i = 0, k = 0; i < num_of_codes; i++) {
    for (j = 0; j < code[i]->num_of_lines; j++) {
      result[k] = calloc(WIDTH_OF_WORD, sizeof(char));
      if (result[k] == NULL) {
        printf("error in allocation memory\n");
        error_flag = 1;
      }
      strcat(result[k], (char *)code[i]->lines[j]);
      k++;
    }
  }
  /* loop over the data table and copy the lines to the binary table */
  for (i = 0; i < data->num_of_lines; i++) {
    result[i + k] = calloc(WIDTH_OF_WORD, sizeof(char));
    if (result[i + k] == NULL) {
      printf("error in allocation memory\n");
      error_flag = 1;
    }
    strcpy(result[i + k], (char *)data->lines[i]);
  }
  binary_table = result;
}