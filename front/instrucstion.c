/*
 * ----------------------------------------------------------------------------
 * File: instruction.c
 * Author: Ishay Abakiyev - ziki, Shlomo Wsisz
 * Date: 28/01/2024
 * Description: This file contains functions related to processing instructions
 *              in the assembly language. It includes functions for handling
 *              operands, processing two operands, one operand, and no operands,
 *              and formatting instructions based on predefined mappings.
 *
 * Functions:
 *   - processOperands: Processes operands in the assembly code, validating and
 *                      updating the data structure accordingly.
 *   - processTwoOperands: Processes instructions with two operands.
 *   - processOneOperand: Processes instructions with one operand.
 *   - processNoOperands: Processes instructions without operands.
 *   - instructionFormat: Formats instructions based on predefined mappings,
 *                        including handling different operand counts.
 *   - registerOperand: Processes a register operand and updates the data structure.
 *   - directIndexOperand: Processes a direct index operand and updates the data structure.
 *   - immediateNumberOperand: Processes an immediate number operand and updates the data structure.
 *   - directLabelOperand: Processes a direct label operand and updates the data structure.
 * ----------------------------------------------------------------------------
 */
#include "headeMethods.h"

int registerOperand(FILE *file, char *word,  char *token, struct pattern *data, int operandCount, int i, struct Node **head) {
    int y;
    int con = 0;
    /* Check if the word is a register (starts with 'r' and has a digit following it) */
    if (word[0] == 'r' && word[1] >= '0' && word[1] <= '7') {
        /* Set operand type to REGISTER */

        /* Map the register string to the corresponding enum value */
        for (y = 0; registerMappings[y].name != NULL; ++y) {
            if (strcmp(token, registerMappings[y].name) == 0) {
                data->choice.inst.operands[operandCount - 1 - i].operand_value.reg = registerMappings[y].reg;
                data->choice.inst.operands[operandCount - 1 - i].op_type = REGISTER;
                con = 1;
                break;
            } else if (y == 7) {
                isError(file, word, data, "Error: Invalid register", "instruction.h", head);
                return false;
            }
        }
        /* Continue to the next iteration if the register was processed */
        if (con == 1) {
            return true;
        }
    }
    return false;
}

int directIndexOperand(FILE *file,  char *word,  char *token, struct pattern *data, int operandCount, int i, struct Node **head) {
    int len = (token != NULL && token[0] != '\0') ? strlen(token) : 0;
    
    /* Check if the word ends with ']' for direct indexing */
    if (len - 1 > 0 && token[len - 1] == ']') {
        /* Set operand type to DIRECT_INDEX */

        data->choice.inst.operands[operandCount - i - 1].op_type = DIRECT_INDEX;

        /* Tokenize the word to extract content inside square brackets */
        token = strtok(word, "[^ [ ]");

        if (isValidLabel(token, data, 1, 1)) {
            strcpy(data->choice.inst.operands[operandCount - i - 1].operand_value.symbol, token);
        } else {
            isError(file,word,data, "Error: Invalid label", "instruction.h", head);
            return false;
        }

        /* Move the file pointer back to the start of the word */
        fseek(file, -strlen(word), SEEK_CUR);

        /* Read the next word after the square brackets */
        fscanf(file, "%s", word);
        token = strtok(word, "[]");
        strcpy(data->choice.inst.operands[operandCount - i - 1].operand_value.const_num, token);
        return true;
    }
    

    return false;
}

int immediateNumberOperand( char *word, struct pattern *data, int operandCount, int i) {
    /* Check if the word starts with '#' for immediate numbers */
    if (word[0] == '#') {
        /* Set operand type to IMMEDIATE_NUMBER */

        data->choice.inst.operands[operandCount - 1 - i].op_type = IMMEDIATE_NUMBER;

        /* Convert the string after '#' to an integer and assign it to the operand */
        if (isNumeric(word + 1)) {
            data->choice.inst.operands[operandCount - 1 - i].operand_value.value = atoi(word + 1);
        } else {
            strcpy(data->choice.inst.operands[operandCount - 1 - i].operand_value.const_num, word + 1);
        }

        return true;
    }

    return false;
}

int directLabelOperand( char *word,  char *token, struct pattern *data, int operandCount, int i, struct Node **head) {
    /* Check if the word is a valid label and process it */
    if (((operandCount - i == 2) && isValidLabel(token, data, 1, 0)) ||
        ((operandCount - i == 1) && isValidLabel(word, data, 1, 0 /* no need at ':' check */))) {
        /* Set operand type to DIRECT */

        data->choice.inst.operands[operandCount - 1 - i].op_type = DIRECT;

        /* Assign the word as the symbol for the operand */
        strcpy(data->choice.inst.operands[operandCount - 1 - i].operand_value.symbol, word);
        return true;
    }

    return false;
}

int processOperands(FILE *file, struct pattern *data, struct Node **head, int operandCount) {
    char word[MAX_LINE_SIZE];
    char *token=NULL;
    int i;

    for (i = 0; i < operandCount; i++) {
        /* Read the next word from the file */
        if (fscanf(file, "%s", word) != 1) {
            isError(file,word,data, "Error: Missing operand","instruction.h", head);
            return false;
        }

        /* For two operands, check if there's a comma after first operand */
        if(operandCount-i==2)
        {
            int len = strlen(word);
            if (len > 0 && word[len - 1] != ',') {
                isError(file,word,data, "Error: Missing comma","instruction.h", head);
                return false;
            }
            token = strtok(word, " ,");
        }

            /* Check if the word is a register and process it */
        if (registerOperand(file, word, token, data, operandCount, i, head)) {
            continue;
        }
    
         /* Check if the word is a direct index and process it */
        if (directIndexOperand(file, word, token, data, operandCount, i, head)) {
            continue;
        }

            /* Check if the word is an immediate number and process it */
        if (immediateNumberOperand(word, data, operandCount, i)) {
            continue;
        }

        /* Check if the word is a direct label and process it */
        if (directLabelOperand(word, token, data, operandCount, i, head)) {
            continue;
        }

        /* If none of the conditions are met, the operand is invalid */
        isError(file,word,data, "Error: Invalid operand","instruction.h", head);
        return false;
    }
    return true;     /* All operands processed successfully */
}

int processTwoOperands(FILE  *file, struct pattern *data, struct Node **head) {
    data->type_line = INSTRUCTION;
    data->choice.inst.num_of_operands=2;
    if (!processOperands(file, data, head, 2)) {
        return false;
    }
    return true;
}

int processOneOperand(FILE  *file, struct pattern *data, struct Node **head) {
    data->type_line = INSTRUCTION;
    data->choice.inst.num_of_operands=1;
    if (!processOperands(file, data, head, 1)) {
        return false;
    }
    return true;
}

int processNoOperands(struct pattern *data) {
    data->type_line = INSTRUCTION;
    data->choice.inst.num_of_operands=0;
    return 1;
}

int instructionFormat(FILE *file, const char *word, struct pattern *data, struct Node **head) {
    /* There is no need to check if the word is a valid label,
     because it is already checked in categorizeWord */
    int i;
    /* Iterate through the array to find a match */
    for (i = 0; instructionMappings[i].name != NULL; ++i) {
        if (strcmp(word, instructionMappings[i].name) == 0) {
            data->choice.inst.op_type = instructionMappings[i].type;
            switch (i) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4: 
                    return processTwoOperands(file, data, head);
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:    
                    return processOneOperand(file, data, head);
                case 15: 
                case 16:
                case 17:
                    return processNoOperands(data);
                default:
                    /* Invalid instruction name*/
                    isError(file,word,data, "Invalid instruction name", "instruction.h", head);
                    return false;
            }
        }
    }
    return false;
}
