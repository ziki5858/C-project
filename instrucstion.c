
#include "headeMethods.h"

int processOperands(FILE *file, struct pattern *data, struct Node **head, int operandCount) {
    char word[MAX_LINE_SIZE];
    char *token;
    int con=0;
    int i,y;
    int a;

    for (i = 0; i < operandCount; i++) {
        /* Read the next word from the file */
        if (fscanf(file, "%s", word) != 1) {
            isError(data, "Error: Missing operand","instruction.h", head);
            return 0;
        }

        /* For two operands, check if there's a comma after first operand */
        if(operandCount-i==2)
        {
            int len = strlen(word);
            if (len > 0 && word[len - 1] != ',') {
                isError(data, "Error: Missing comma","instruction.h", head);
                return 0;
            }
            token = strtok(word, " ,");
        }

        /* Check if the word is a register (starts with 'r' and has a digit following it) */
        if(word[0] == 'r' && word[1] >= '0' && word[1] <= '7') {
            /* Set operand type to REGISTER */
            data->choice.inst.operands[operandCount-1-i].op_type = REGISTER;

            /* Map the register string to the corresponding enum value */
            for (y = 0; registerMappings[y].name != NULL; ++y) {
                if ((operandCount-i==2)&& strcmp(token, registerMappings[y].name) == 0||
                        (operandCount-i==1)&& strcmp(word, registerMappings[y].name) == 0) {
                    data->choice.inst.operands[operandCount - 1].operand_value.reg = registerMappings[y].reg;
                    con=1;
                    break;
                } else if(y==7){
                    isError(data, "Error: Invalid register","instruction.h", head);
                    return 0;
                }
            }
            /* Continue to the next iteration if the register was processed */
            if(con==1){
                con=0;

                continue;
            }
        }
        int len = strlen(token);
        if (len-1>0&&token[len-1] == ']') { /* Check if the word ends with ']' for direct indexing */
            /* Set operand type to DIRECT_INDEX */
            data->choice.inst.operands[operandCount - i].op_type = DIRECT_INDEX;
            /* Tokenize the word to extract content inside square brackets */
            token = strtok(word, "[^ [ ]");
            if(isValidLabel(token,data,1,1)){
                strcpy(data->choice.inst.operands[operandCount - i].operand_value.symbol, token);
            }
            else{
                isError(data, "Error: Invalid label","instruction.h", head);
                return 0;
            }
            /* Move the file pointer back to the start of the word */
            fseek(file, -strlen(word), SEEK_CUR);
            /* Read the next word after the square brackets */
            fscanf(file, "%s", word);
            token = strtok(word, "[]");
            strcpy(data->choice.inst.operands[operandCount - i].operand_value.const_num, token);
            continue;
        }

        if (word[0] == '#') {  /* Check if the word starts with '#' for immediate numbers */
            data->choice.inst.operands[operandCount-1-i].op_type = IMMEDIATE_NUMBER;
            /* Convert the string after '#' to an integer and assign it to the operand */
            data->choice.inst.operands[operandCount-1-i].operand_value.value = atoi(word + 1);
            continue;
        }

        if((operandCount-i==2)&& isValidLabel(token,data,1,0)||(operandCount-i==1)&& isValidLabel(word, data,1,0/*no need at ':' check*/)){
            data->choice.inst.operands[operandCount-1-i].op_type = DIRECT;
            num_of_symbols++;
            /* Assign the word as the symbol for the operand */
            strcpy(data->choice.inst.operands[operandCount-1-i].operand_value.symbol, word);
            continue;
        }
        /* If none of the conditions are met, the operand is invalid */
        isError(data, "Error: Invalid operand","instruction.h", head);
        return 0;
    }
    return 1;     /* All operands processed successfully */
}


int processTwoOperands(FILE  *file, struct pattern *data, struct Node **head) {
    data->type_line = INSTRUCTION;
    data->choice.inst.num_of_operands=2;
    if (!processOperands(file, data, head, 2)) {
        return 0;
    }

    return 1;
}


int processOneOperand(FILE  *file, struct pattern *data, struct Node **head) {
    data->type_line = INSTRUCTION;
    data->choice.inst.num_of_operands=1;
    if (!processOperands(file, data, head, 1)) {
        return 0;
    }

    return 1;
}

int processNoOperands(struct pattern *data) {
    data->type_line = INSTRUCTION;
    data->choice.inst.num_of_operands=0;

    /* Update other properties of the data structure as needed */
    return 1;
}

int instructionFormat(FILE *file, const char *word, struct pattern *data, struct Node **head) {
    int i;
    /* There is no need to check if the word is a valid label because it is already checked in categorizeWord */

    /* Iterate through the array to find a match */
    for (i = 0; instructionMappings[i].name != NULL; ++i) {
        if (strcmp(word, instructionMappings[i].name) == 0) {
            data->choice.inst.op_type = instructionMappings[i].type;

            /* It's an instruction with two operands */
            if (i >= 0 && i < 5) {
                return processTwoOperands(file, data, head);
            }
                /* It's an instruction with one operand */
            else if (i >= 5 && i < 15) {
                return processOneOperand(file, data, head);
            }
                /* It's an instruction without operands */
            else if (i >= 15 && i < 18) {
                return processNoOperands(data);
            } else {
                /* Invalid instruction name */
                data->type_line = ERROR;
                isError(data, "Invalid instruction name","instruction.h", head);
                return 0;
            }
        }
    }
    /* Not an instruction */
    return 0;
}
