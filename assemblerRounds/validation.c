/**
 * @file validation.c
 * @author shlomo weisz
 * @brief  This file contains the implementation of the functions that used to
 * validate the assembler. the file contains the following functions:
 * 1. print_error_memory - a function that prints an error message when the
 * program fail to allocate memory.
 * 2. print_error_msg - a function that prints an error message.
 * 3. add_IC_to_symbol_table - a function that adds IC to the symbol table, used
 * for the second round.
 * 4. check_validation_of_operands_num - a function that checks validation of
 * operands number.
 * 5. check_validation_of_operands_type - a function that checks validation of
 * operands type.
 * 6. extract_immidiate_number - a function that extract immidiate number from
 * the instruction.
 * 7. is_saved_word - a function that checks if the word is a saved word.
 * 8. determaine_sign - a function that determaine the sign of the number.
 * 9. containsOnlyDigits - a function that checks if the string contains only
 * digits.
 * 10. handle_lable_error - a function that handle lable error by printing an
 * error message based on the error.
 * 11. handle_lable_define - a function that handle lable define by adding the
 * lable to the symbol table.
 * 12. allocate_memory_for_instructions - a function that allocate memory for
 * instructions in the pattern struct, according to the number of operands and
 * the type of the operands in the instruction.
 * 13. allocate_memory_for_data - a function that allocate memory for data.
 * 14. validate_entreis - a function that check if all the entries are defined.
 * 15. there_is_direct - a function that check if there is a direct operand in the instruction.
 * @version 0.1
 * @date 2024-03-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "./headers/validation.h"
#include <stdio.h>

/**
 * @brief Print error message
 *
 * @param msg The message to print
 * @param line The line in the file
 */
void print_error_memory(int line, int *error_flag) {
  printf("error: fail to allocate memory in line %d\n", line);
  *error_flag = 1;
}

/**
 * @brief Print error message
 *
 * @param msg The message to print
 * @param line The line in the file
 */
void print_error_msg(char *msg, int line, int *error_flag) {
  printf("error: %s in line %d\n", msg, line);
  *error_flag = 1;
}

/**
 * @brief Add IC to symbol table, used for the second round
 *
 * @param IC The IC to add
 */
void add_IC_to_symbol_table(int IC) {
  int i;
  Symbol s;
  /* add IC to the symbol table, by adding IC to the address of the symbols */
  for (i = 0; i < num_of_symbols; i++) {
    s = symbol_table[i];
    if ((int)s->type == (int)S_DATA || (int)s->type == (int)ENTRY_DATA)
      s->address +=
          (IC + 100); /* add 100 + IC to the address of the data symbols */
    else if ((int)s->type == (int)CODE || (int)s->type == (int)ENTRY_CODE)
      s->address += 100; /* add 100 to the address of the code symbols, because
                            the code starts at 100 */
  }
}

/**
 * @brief Check validation of operands number
 *
 * @param instruction The instruction to check
 * @return int 1 if valid, 0 otherwise
 */
int check_validation_of_operands_num(struct pattern *instruction) {
  int i = instruction->choice.inst.num_of_operands;

  if (i == 0) {
    if (instruction->choice.inst.op_type == RTS ||
        instruction->choice.inst.op_type == HLT)
      return 1;
    else
      return 0;
  }
  if (i == 1) {
    if (instruction->choice.inst.op_type == NOT ||
        instruction->choice.inst.op_type == CLR ||
        instruction->choice.inst.op_type == INC ||
        instruction->choice.inst.op_type == DEC ||
        instruction->choice.inst.op_type == JMP ||
        instruction->choice.inst.op_type == BNE ||
        instruction->choice.inst.op_type == RED ||
        instruction->choice.inst.op_type == PRN ||
        instruction->choice.inst.op_type == JSR)
      return 1;
    else
      return 0;
  }
  if (i == 2) {
    if (instruction->choice.inst.op_type == MOV ||
        instruction->choice.inst.op_type == CMP ||
        instruction->choice.inst.op_type == ADD ||
        instruction->choice.inst.op_type == SUB ||
        instruction->choice.inst.op_type == LEA)
      return 1;
    else
      return 0;
  }
  return 0;
}

/**
 * @brief Check validation of operands type
 *
 * @param instruction The instruction to check
 * @return int 1 if valid, 0 otherwise
 */
int check_validation_of_operands_type(struct pattern *instruction) {
  int i = instruction->choice.inst.op_type;
  if ((i == MOV || (i >= ADD && i <= DEC) || i == RED) &&
      (instruction->choice.inst.operands[0].op_type == IMMEDIATE_NUMBER))
    return 0;
  if ((i == JMP || i == BNE || i == JSR) &&
      (instruction->choice.inst.operands[0].op_type == DIRECT_INDEX ||
       instruction->choice.inst.operands[0].op_type == IMMEDIATE_NUMBER))
    return 0;
  if ((i == LEA) &&
      ((instruction->choice.inst.operands[1].op_type == REGISTER) ||
       (instruction->choice.inst.operands[1].op_type == IMMEDIATE_NUMBER)))
    return 0;
  return 1;
}

/**
 * @brief Extract immidiate number from the instruction
 *
 * @param p The instruction
 * @param num_of_operand The number of the operand
 * @return int The immidiate number
 */
int extract_immidiate_number(struct pattern *p, int num_of_operand,
                             int current_pattern_num, int *error_flag) {
  char *buffer =
      p->choice.inst.operands[num_of_operand].operand_value.const_num;
  if (isalpha(buffer[0])) { /* if the immidiate number is a constant */
    Constant c = (Constant)exist_in_trie(constant, buffer);
    if (c)
      return c->value;
    else {
      print_error_msg("constant not found", current_pattern_num, error_flag);

      return 0;
    }
  } else if (isdigit(buffer[0]) || buffer[0] == '-' ||
             buffer[0] == '+') { /* if the immidiate number is a number */
    return atoi(buffer);
  }
  return p->choice.inst.operands[num_of_operand].operand_value.value;

  return 0;
}

/**
 * @brief Check if the word is a saved word
 *
 * @param word The word to check
 * @return int 1 if saved word, 0 otherwise
 */
int is_saved_word(char *word) {
  int i;
  char saved_words[26][6] = {
      "mov", "cmp", "add", "sub", "lea", "not", "clr", "inc",   "dec",
      "jmp", "bne", "red", "prn", "jsr", "rts", "hlt", "entry", "extern",
      "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7"};

  for (i = 0; i < 26; i++) {
    if (strcmp(word, saved_words[i]) == 0)
      return 1;
  }
  return 0;
}

/**
 * @brief Determaine the sign of the number
 *
 * @param buffer The buffer to check
 * @return int 1 if positive, -1 if negative, 0 otherwise
 */
int determaine_sign(char *buffer) {
  if (buffer[0] == '-') {
    return -1;
  } else if (buffer[0] == '+' || isdigit(buffer[0])) {
    return 1;
  } else {
    return 0;
  }
}

/**
 * @brief Check if the string contains only digits
 *
 * @param str The string to check
 * @return int 1 if contains only digits, 0 otherwise
 */
int containsOnlyDigits(const char *str) {
  while (*str) {
    if (!isdigit(*str)) {
      return 0;
    }
    str++;
  }
  return 1;
}

/**
 * @brief Handle lable error by printing an error message based on the error
 *
 * @param p The pattern to check
 */
void handle_lable_error(struct pattern *p, int current_pattern_num, int *error_flag) {
  if (p->label[0]) {
    if (exist_in_trie(macro_trie, p->label)) {
      print_error_msg("label already exist as macro", current_pattern_num,
					  error_flag);
      return;
    }
    if (is_saved_word(p->label)) {
      print_error_msg("label already exist as saved word", current_pattern_num,
					  error_flag);
      return;
    }
    if (exist_in_trie(constant, p->label)) {
      print_error_msg("label already exist as constant", current_pattern_num,
					  error_flag);
      return;
    }
  }
}

/**
 * @brief Handle lable define by adding the lable to the symbol table
 *
 * @param p The pattern to check
 * @param type The type of the lable
 * @param counter The counter to add
 */
void handle_lable_define(struct pattern *p, char *type, int counter,
                         int current_pattern_num, int *error_flag, int IC, int DC, int num_of_data) {
  int type_of_op = (strcmp(type, "data") == 0) ? 1 : 0;

  /* check if the label is already exist in the tries */
  Symbol s = (Symbol)exist_in_trie(symbols, p->label);

  if (s && (int)s->type == (int)S_ENTRY) { /* define as entry without data */
    s->type = (type_of_op) ? ENTRY_DATA : ENTRY_CODE;
    s->address = (type_of_op) ? DC : IC;
	s->num_of_data = num_of_data;
    symbol_table_of_entries[num_of_symbols_in_entries++] = s;

  } else if (!s) {
    s = create_symbol(p->label, counter, (type_of_op) ? S_DATA : CODE,
                      current_pattern_num, error_flag, num_of_data); /* define as data */
    if (!s) {
      print_error_msg("fail to add symbol", current_pattern_num, error_flag);
    }
  } else { /* already exist */
    print_error_msg("symbol already exist", current_pattern_num, error_flag);
  }
}

/**
 * @brief Allocate memory for instructions in the pattern struct, according to
 * the number of operands and the type of the operands in the instruction
 *
 * @param ins The instruction to allocate memory for
 * @param code_i The code to allocate memory for
 */
void allocate_memory_for_instructions(struct pattern *ins, Code *code_i,
                                      int current_pattern_num, int *error_flag) {
  int amount, i;
  WordBin *temp;
  *code_i = (Code)calloc(1, sizeof(struct code));
  if (!code_i) {
    print_error_memory(current_pattern_num, error_flag);
    return;
  }

  /* determain the amount of bytes needed for the instruction */
  if (ins->choice.inst.num_of_operands == 0) {
    amount = 1;
  } else if (ins->choice.inst.num_of_operands >= 1) {
    if (ins->choice.inst.operands[0].op_type == DIRECT_INDEX) {
      amount = 3;
    } else {
      amount = 2;
    }
  }
  if (ins->choice.inst.num_of_operands == 2) {
    if (ins->choice.inst.operands[1].op_type == DIRECT_INDEX) {
      amount += 2;
    } else if (!((ins->choice.inst.operands[1].op_type == REGISTER) &&
                 (ins->choice.inst.operands[0].op_type == REGISTER))) {
      amount += 1;
    }
  }
  temp = (WordBin *)calloc(amount, sizeof(WordBin));
  if (!temp) {
    print_error_memory(current_pattern_num, error_flag);
    return;
  }
  (*code_i)->lines = temp;
  (*code_i)->num_of_lines = amount;
  (*code_i)->line_in_file = current_pattern_num;

  for (i = 0; i < amount; i++) {
    (*code_i)->lines[i] = (WordBin)calloc(1, sizeof(struct word_bin));
    if (!(*code_i)->lines[i]) {
      print_error_memory(current_pattern_num, error_flag);
      return;
    }
  }
}

/**
 * @brief Allocate memory for data
 *
 * @param data_i The data to allocate memory for
 */
void allocate_memory_for_data(struct pattern *data_i, int current_pattern_num, int *error_flag) {
  int i, exist, new;
  WordBin *temp;
  exist = data->num_of_lines;
  new = data_i->choice.dir.size;
  temp = (WordBin *)realloc(data->lines, sizeof(WordBin) * (exist + new));
  if (!temp) {
    print_error_memory(current_pattern_num, error_flag);
    return;
  }
  data->lines = temp;
  for (i = 0; i < new; i++) {
    data->lines[exist + i] = (WordBin)calloc(1, sizeof(struct word_bin));
    if (!data->lines[exist + i]) {
      print_error_memory(current_pattern_num, error_flag);
      return;
    }
  }
}


/**
 * @brief  check if all the entries are defined
 * 
 */
void validate_entreis(int *error_flag) {
  Entry e;
  int i;
  /* loop over the entry table and check if the symbol is defined */
  for (i = 0; i < num_of_entries_in_table; i++) {
    e = entry_table[i];
    if (e->symbol->address == 0) {

      printf("error: in line:%d entry %s definition not found\n", e->line_in_file, e->symbol->label);
      *error_flag = 1;
    }
  }
}


/**
 * @brief check if there is a direct operand in the instruction.
 * 
 * @param head The head of the linked list
 * @return int 1 if there is a direct operand, 0 otherwise
 */
int there_is_direct(struct Node *head) {
  if (head->data->choice.inst.operands[0].op_type == DIRECT ||
    head->data->choice.inst.operands[0].op_type == DIRECT_INDEX ||( head->data->choice.inst.num_of_operands == 2 &&
    (head->data->choice.inst.operands[1].op_type == DIRECT ||
     head->data->choice.inst.operands[1].op_type == DIRECT_INDEX)))
	return 1;
  return 0;
}


/**
 * @brief  check if the index of the direct operand is out of bounds
 * 
 * @param node  the node that contains the instruction
 * @param error_flag  a flag that indicates if there is an error
 * @return int  1 if the index is out of bounds, 0 otherwise
 */
int check_direct_index(struct Node *node, int *error_flag) {
	int num, i;
	Symbol s;
	for (i = 0; i < node->data->choice.inst.num_of_operands; i++) {
		
	if (node->data->choice.inst.operands[i].op_type == DIRECT_INDEX) {
		
		num = extract_immidiate_number(node->data, i, 0, error_flag);
		s = (Symbol)exist_in_trie(symbols, node->data->choice.inst.operands[i].operand_value.symbol);
		if (s->type != S_EXTERN && num >= s->num_of_data) {
			printf("error: in line %d, the index is out of bounds\n", node->num_in_list);
			*error_flag = 1;
			return 1;
		}
	}
	}
	return 0;
}