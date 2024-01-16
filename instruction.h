#include <stdio.h>
#include <string.h>
#include "ctype.h"
#include "front.h"


int processOperands(FILE *file, struct pattern *data, struct Node **head, int operandCount) {
    char word[50];
    for (int i = 0; i < operandCount; i++) {
        if (fscanf(file, "%s", word) != 1) {

            isError(data, "Error: Missing operand", head);
            return 0;
        }
        if (word[0] == '#') {
            data->type_line = IMMEDIATE_NUMBER;
            data->choice.inst.operands[i].operand_value.value = atoi(word + 1);
        } else if(word[strlen(word) - 1] == ']'){// shold insert for the 4 check, problam is strlen word and not line
            data->type_line = DIRECT_INDEX;
            strcpy(data->choice.inst.operands[i].operand_value.symbol, word+1);
        } else if(word[0] == 'r' && word[1] >= '0' && word[1] <= '7'){
            data->type_line = REGISTER;
            data->choice.inst.operands[i].operand_value.reg = word[1] - '0';
        } else if(isValidLabel(word, data)){
            data->type_line = DIRECT;
            strcpy(data->choice.inst.operands[i].operand_value.symbol, word);
        } else {
            isError(data, "Error: Invalid operand", head);
            return 0;
        }
    }
    return 1;
}


int processTwoOperands(FILE  *file, struct pattern *data, struct Node **head, int  i) {
    data->type_line = INSTRUCTION;
    data->choice.inst.op_type = instructionMappings[i].type;

    if (!processOperands(file, data, head, 2)) {
        return 0;
    }

    return 1;
}


int processOneOperand(FILE  *file, struct pattern *data, struct Node **head, int  i) {
    data->type_line = INSTRUCTION;
    data->choice.inst.op_type = instructionMappings[i].type;

    if (!processOperands(file, data, head, 1)) {
        return 0;
    }

    return 1;
}

int processNoOperands(struct pattern *data, struct Node **head, int  i) {
    data->type_line = INSTRUCTION;
    data->choice.inst.op_type = instructionMappings[i].type;

    /* Update other properties of the data structure as needed */
    return 1;
}

int instructionFormat(FILE *file, const char *word, struct pattern *data, struct Node **head) {
    int i;
   /*There is no need to check if the word is a valid label because it is already checked in categorizeWord*/

   /* Iterate through the array to find a match */
    for (i = 0; instructionMappings[i].name != NULL; ++i) {
        if (strcmp(word, instructionMappings[i].name) == 0) {
            /* It's an instruction with two operands */
            if (strcmp(word, "mov") == 0 || strcmp(word, "cmp") == 0 || strcmp(word, "add") == 0 ||
                strcmp(word, "sub") == 0 || strcmp(word, "lea") == 0) {
                return processTwoOperands(file, data, head, i);
            }
                /* It's an instruction with one operand */
            else if (strcmp(word, "not") == 0 || strcmp(word, "clr") == 0 || strcmp(word, "inc") == 0 ||
                     strcmp(word, "dec") == 0 || strcmp(word, "jmp") == 0 || strcmp(word, "bne") == 0 ||
                     strcmp(word, "red") == 0 || strcmp(word, "prn") == 0 || strcmp(word, "jsr") == 0) {
                return processOneOperand(file, data, head, i);
            }
                /* It's an instruction without operands */
            else if (strcmp(word, "hlt") == 0 || strcmp(word, "rts") == 0) {
                return processNoOperands(data, head, i);
            } else {
                /* Invalid instruction name */
                data->type_line = ERROR;
                isError(data, "Invalid instruction name", head);
                return 0;
            }
        }
    }
    /* Not an instruction */
    return 0;
}