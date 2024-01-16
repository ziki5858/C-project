#include <stdio.h>
#include <string.h>
#include "ctype.h"
#include "front.h"


int processOperands(FILE *file, struct pattern *data, struct Node **head, int operandCount) {
    char word[50];
    for (int i = 0; i < operandCount; i++) {
        if (fscanf(file, "%49s", word) != 1) {

            isError(data, "Error: Missing operand", head);
            return 0;
        }
        if (word[0] == '#') {
            data->type_line = IMMEDIATE_NUMBER;
            insertNode(head, *data);
            data->inst.operands[i].operand_value.value = atoi(word + 1);
            insertNode(head, *data);
        } else if(word[strlen(word) - 1] == ']'){
            data->type_line = DIRECT_INDEX;
            insertNode(head, *data);
            strcpy(data->inst.operands[i].operand_value.symbol, word+1);
            insertNode(head, *data);
        } else if(word[0] == 'r' && word[1] >= '0' && word[1] <= '7'){
            data->type_line = REGISTER;
            insertNode(head, *data);
            data->inst.operands[i].operand_value.reg = word[1] - '0';
            insertNode(head, *data);
        } else if(isValidLabel(word)){
            data->type_line = DIRECT;
            insertNode(head, *data);
            strcpy(data->inst.operands[i].operand_value.symbol, word);
            insertNode(head, *data);
        } else {
            isError(data, "Error: Invalid operand", head);
            return 0;
        }
        insertNode(head, *data);
    }
    return 1;
}


int processTwoOperands(FILE  *file, struct pattern *data, struct Node **head, int  i) {
    data->type_line = INSTRUCTION;
    insertNode(head, *data);
    data->inst.op_type = instructionMappings[i].type;
    insertNode(head, *data);

    if (!processOperands(file, data, head, 2)) {
        return 0;
    }

    return 1;
}


int processOneOperand(FILE  *file, struct pattern *data, struct Node **head, int  i) {
    data->type_line = INSTRUCTION;
    insertNode(head, *data);
    data->inst.op_type = instructionMappings[i].type;
    insertNode(head, *data);

    if (!processOperands(file, data, head, 1)) {
        return 0;
    }

    return 1;
}

int processNoOperands(struct pattern *data, struct Node **head, int  i) {
    data->type_line = INSTRUCTION;
    insertNode(head, *data);
    data->inst.op_type = instructionMappings[i].type;
    insertNode(head, *data);
    /* Update other properties of the data structure as needed */
    return 1;
}

int isInstruction(FILE *file, const char *word, struct pattern *data, struct Node **head) {
    int i;
   /*There is no need to check if the word is a valid label because it is already checked in categorizeWord*/

   /* Iterate through the array to find a match */
    for (i = 0; instructionMappings[i].name != NULL; ++i) {
        if (strcmp(word, instructionMappings[i].name) == 0) {
            /* It's an instruction with two operands */
            if (strcmp(word, "MOV") == 0 || strcmp(word, "CMP") == 0 || strcmp(word, "ADD") == 0 ||
                strcmp(word, "SUB") == 0 || strcmp(word, "LEA") == 0) {
                return processTwoOperands(file, data, head, i);
            }
                /* It's an instruction with one operand */
            else if (strcmp(word, "NOT") == 0 || strcmp(word, "CLR") == 0 || strcmp(word, "INC") == 0 ||
                     strcmp(word, "DEC") == 0 || strcmp(word, "JMP") == 0 || strcmp(word, "BNE") == 0 ||
                     strcmp(word, "RED") == 0 || strcmp(word, "PRN") == 0 || strcmp(word, "JSR") == 0) {
                return processOneOperand(file, data, head, i);
            }
                /* It's an instruction without operands */
            else if (strcmp(word, "HLT") == 0 || strcmp(word, "RTS") == 0) {
                return processNoOperands(data, head, i);
            } else {
                /* Invalid instruction name */
                data->type_line = ERROR;
                insertNode(head, *data);
                isError(data, "Invalid instruction name", head);
                return 0;
            }
        }
    }
    /* Not an instruction */
    return 0;
}