/*#include "../tables.h"*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../front.h"
#include "structs_func.h"
#include "encoder.h"
#define WIDTH_OF_WORD 15 /*14?????????????*/
#define MAX_DATA_VALUE 8191
#define MIN_DATA_VALUE -8192

extern int num_of_patterns; /* אולי מהקדם אסמבלר?*/


char saved_words[26][6] = {"mov", "cmp", "add",   "sub",    "lea", "not", "clr",
                           "inc", "dec", "jmp",   "bne",    "red", "prn", "jsr",
                           "rts", "hlt", "entry", "extern", "r0",  "r1",  "r2",
                           "r3",  "r4",  "r5",    "r6",     "r7"};
int error_flag = 0;
int num_of_codes = 0;
int current_pattern_num = 0;
int IC = 0, DC = 0;


/**
 * @brief Print error message
 * 
 * @param msg The message to print
 * @param line The line in the file
 */
void print_error_memory(int line) {
  printf("error: fail to allocate memory in line %d\n", line);
  error_flag = 1;
}


/**
 * @brief Print error message
 * 
 * @param msg The message to print
 * @param line The line in the file
 */
void print_error_msg(char *msg, int line) {
  printf("error: %s in line %d\n", msg, line);
  error_flag = 1;
}


/**
 * @brief Add IC to symbol table
 * 
 * @param IC The IC to add
 */
void add_IC_to_symbol_table(int IC) {
  int i;
  Symbol s;
  for (i = 0; i < num_of_symbols; i++) {
    s = symbol_table[i];
    if ((int)s->type == (int)S_DATA || (int)s->type == (int)ENTRY_DATA)
      s->address += (IC + 100);
    else if ((int)s->type == (int)CODE || (int)s->type == (int)ENTRY_CODE)
      s->address += 100;
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
int extract_immidiate_number(struct pattern *p, int num_of_operand) {
  char *buffer =
      p->choice.inst.operands[num_of_operand].operand_value.const_num;
  if (isalpha(buffer[0])) {
    Constant c = (Constant)exist_in_trie(constant, buffer);
    if (c)
      return c->value;
    else {
      print_error_msg("constant not found", current_pattern_num);

      return 0;
    }
  } else if(isdigit(buffer[0]) || buffer[0] == '-' || buffer[0] == '+'){
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
void handle_lable_error(struct pattern *p) {
  if (p->label[0]) {
    if (exist_in_trie(macro_trie, p->label)) {
      print_error_msg("label already exist as macro", current_pattern_num);
      return;
    }
    if (is_saved_word(p->label)) {
      print_error_msg("label already exist as saved word", current_pattern_num);
      return;
    }
    if (exist_in_trie(constant, p->label)) {
      print_error_msg("label already exist as constant", current_pattern_num);
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
void handle_lable_define(struct pattern *p, char *type, int counter) {
  int type_of_op = (strcmp(type, "data") == 0) ? 1 : 0;

  Symbol s = (Symbol)exist_in_trie(symbols, p->label);
  if (s && (int)s->type == (int)S_ENTRY) { /* define as entry without data */
    s->type = (type_of_op) ? ENTRY_DATA : ENTRY_CODE;
    s->address = (type_of_op) ? DC : IC;
    symbol_table_of_entries[num_of_symbols_in_entries++] = s;
  } else if (!s) {
    s = create_symbol(p->label, counter, (type_of_op) ? S_DATA : CODE,
                      current_pattern_num); /* define as data */
    if (!s) {
      print_error_msg("fail to add symbol", current_pattern_num);
    }
  } else { /* already exist */
    print_error_msg("symbol already exist", current_pattern_num);
  }
}


/**
 * @brief Allocate memory for instructions
 * 
 * @param ins The instruction to allocate memory for
 * @param code_i The code to allocate memory for
 */
void allocate_memory_for_instructions(struct pattern *ins, Code *code_i) {
  int amount, i;
  WordBin *temp;
  *code_i = (Code)calloc(1, sizeof(struct code));
  if (!code_i) {
    print_error_memory(current_pattern_num);
    return;
  }
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
    print_error_memory(current_pattern_num);
    return;
  }
  (*code_i)->lines = temp;
  (*code_i)->num_of_lines = amount;
  (*code_i)->line_in_file = current_pattern_num;

  for (i = 0; i < amount; i++) {
    (*code_i)->lines[i] = (WordBin)calloc(1, sizeof(struct word_bin));
    if (!(*code_i)->lines[i]) {
      print_error_memory(current_pattern_num);
      return;
    }
  }
}


/**
 * @brief Allocate memory for data
 * 
 * @param data_i The data to allocate memory for
 */
void allocate_memory_for_data(struct pattern *data_i) {
  int i, exist, new;
  WordBin *temp;
  exist = data->num_of_lines;
  new = data_i->choice.dir.size;
  temp = (WordBin *)realloc(data->lines, sizeof(WordBin) * (exist + new));
  if (!temp) {
    print_error_memory(current_pattern_num);
    return;
  }
  data->lines = temp;
  for (i = 0; i < new; i++) {
    data->lines[exist + i] = (WordBin)calloc(1, sizeof(struct word_bin));
    if (!data->lines[exist + i]) {
      print_error_memory(current_pattern_num);
      return;
    }
  }
}


/**
 * @brief The first round of the assembler
 * 
 * @param head The head of the linked list
 */
void first_round(struct Node *head) {
  struct Node *current_pattern = head;

  char *buffer, *temp_data, *temp_ins, *temp_reg;
  int data_element, num_of_word_nedded;
  int sign = 1, i, char_index, num;
  Symbol s;

  /*
  initialize the tries
  */
  symbols = trie();
  externals = trie();
  entries = trie();
  constant = trie();

  /* initialize the code and data tables */
  code = (Code *)calloc(num_of_patterns, sizeof(Code));
  data = (Code)calloc(1, sizeof(struct code));

  /* initialize the symbol, entry and external tables */
  symbol_table = (Symbol *)calloc(size_of_symbol_table, sizeof(Symbol));
  entry_table = (Entry *)calloc(num_of_entries, sizeof(Entry));
  symbol_table_of_entries = (Symbol *)calloc(num_of_entries, sizeof(Symbol));
  external_table = (External *)calloc(num_of_externals, sizeof(External));
  /* constant_table = (Constant *)calloc(size_in_constant_table,
   sizeof(Constant));*/

  for (current_pattern_num = 1; current_pattern_num <= num_of_patterns;
       current_pattern = current_pattern->next, current_pattern_num++) {
    /*
    dealing with labels
    check if the label is already exist in the tries

    */
    if (current_pattern->data->label[0]) {
      handle_lable_error(current_pattern->data);

 
    }
    /* check the type of the line */
    switch (current_pattern->data->type_line) {

    /* dealing with syntax errors */
    case ERROR:
      printf("%s\n", current_pattern->data->choice.error);
	  error_flag = 1;
      break;

      /* dealing with directives */
    case DIRECTIVE:

      /* check the type of the directive */
      switch (current_pattern->data->choice.dir.directive_type) {

      /* dealing with data directive */
      case DATA:
        sign = 1;
        /* check if there is a label */
        if (current_pattern->data->label[0]) {
          handle_lable_define(current_pattern->data, "data", DC);

         
        }
        /* insert the data to the data table */
        allocate_memory_for_data(current_pattern->data);
        for (i = 0; i < current_pattern->data->choice.dir.size; i++) {
          buffer = current_pattern->data->choice.dir.data[i];

          if (isalpha(buffer[0]) &&
              exist_in_trie(constant, buffer)) { /* if the data is a constant */
            data_element = ((Constant)exist_in_trie(constant, buffer))->value;
          } else { /* if the data is a number */
            if (buffer[0] == '-' || buffer[0] == '+') { /* ignore the sign */
              sign = determaine_sign(buffer); /* determaine the sign */
              buffer++;
            }
            if (!containsOnlyDigits(
                    &buffer[0])) { /* check if the data is a number */
              print_error_msg("data element is not a number",
                              current_pattern_num);
              continue;
            } else
              data_element = atoi(buffer) * sign;
          }
          if (data_element > MAX_DATA_VALUE || data_element < MIN_DATA_VALUE) {
            print_error_msg("data element is out of range",
                            current_pattern_num);
            continue;
          }
          /* insert the data to the data table */
          temp_data = encoded_data(data_element);
          strcpy(data->lines[data->num_of_lines + i]->word, temp_data); /* -> */
          free(temp_data);
        }
        data->num_of_lines += current_pattern->data->choice.dir.size;
        DC += current_pattern->data->choice.dir.size;
        break;

      case STRING:

        if (current_pattern->data->label[0]) {
          handle_lable_define(current_pattern->data, "data", DC);
          
        }
        /* insert the data to the data table */
        allocate_memory_for_data(current_pattern->data);
        for (i = 0; i < current_pattern->data->choice.dir.size; i++) {
          char_index = current_pattern->data->choice.dir.string[i];
          temp_data = encoded_data(char_index);
          strcpy(data->lines[data->num_of_lines + i]->word, temp_data);
          free(temp_data);
        }
        data->num_of_lines += current_pattern->data->choice.dir.size;
        DC += current_pattern->data->choice.dir.size;
        break;

      case ENTRY: /*אולי להשמיט את בדיקת התווית במקרה כזה*/
        if (exist_in_trie(entries, current_pattern->data->label)) {
          /* seem to be allowed */
          continue;
        }
        if (exist_in_trie(externals, current_pattern->data->label)) {
          print_error_msg("symbol already exist as external",
                          current_pattern_num);
          continue;
        }
        if ((s = (Symbol)exist_in_trie(symbols,
                                       current_pattern->data->label))) {
          
          if ((int)s->type == (int)DATA || (int)s->type == (int)CODE) {
            s->type = ((int)s->type == (int)DATA) ? ENTRY_DATA : ENTRY_CODE;
            symbol_table_of_entries[num_of_symbols_in_entries++] = s;
            create_entry(s, current_pattern_num);
          } else {
            /* seem to be allowed */
            continue;
          }
        } else { /* symbol not found */
          s = create_symbol(current_pattern->data->label, 0, S_ENTRY,
                            current_pattern_num);
          create_entry(s, current_pattern_num);
        }
        break;

      case EXTERN:
        if (exist_in_trie(entries, current_pattern->data->label)) {
          print_error_msg("symbol already exist as entry", current_pattern_num);
          continue;
        }
        if (exist_in_trie(externals, current_pattern->data->label)) {
          /* seem to be allowed */
          continue;
        }
        if ((s = exist_in_trie(symbols, current_pattern->data->label))) {
          print_error_msg("symbol already exist as symbol",
                          current_pattern_num);
          continue;

        } else {
          s = create_symbol(current_pattern->data->label, 0, S_EXTERN,
                            current_pattern_num);
          create_external(current_pattern->data->label, current_pattern_num);
        }
        break;
      }
      break;

    case DEFINE:
      if (exist_in_trie(symbols, current_pattern->data->label)) {
        print_error_msg("symbol already exist as symbol", current_pattern_num);
        continue;
      }
	  if (exist_in_trie(constant, current_pattern->data->label)) {
		print_error_msg("symbol already exist as constant", current_pattern_num);
		continue;
	  }
      create_constant(current_pattern->data->label,
                                   current_pattern->data->choice.def.value,
                                   current_pattern_num);
      break;

    case INSTRUCTION:

      /* check the valdiation of the operands type and number */
      if (!check_validation_of_operands_num(current_pattern->data)) {
        print_error_msg("invalid number of operands", current_pattern_num);
        continue;
      }
      if (!check_validation_of_operands_type(current_pattern->data)) {
        print_error_msg("invalid type of operands", current_pattern_num);
        continue;
      }

      /* check if there is a label */
      if (current_pattern->data->label[0]) {
        handle_lable_define(current_pattern->data, "code", IC);
      }
      
      allocate_memory_for_instructions(current_pattern->data,
                                       &(code[num_of_codes]));
      current_pattern->data->choice.inst.code = code[num_of_codes];
      num_of_word_nedded = 1;
     
      temp_ins = encoded_instruction(current_pattern->data);
      strcpy(code[num_of_codes]->lines[0]->word, temp_ins);
      free(temp_ins);
     
      /* check if there ara operands */
      if (current_pattern->data->choice.inst.num_of_operands >= 1) {

        /* check the type of the first operand */
        if (current_pattern->data->choice.inst.num_of_operands ==
            2) { /*there is olso sorce op*/
          if ((current_pattern->data->choice.inst.operands[0].op_type ==
               REGISTER) &&
              (current_pattern->data->choice.inst.operands[1].op_type ==
               REGISTER)) { /*tow registers*/
            num_of_word_nedded += 1;
            
            temp_ins =
                encoded_registers(current_pattern->data->choice.inst.operands[1]
                                      .operand_value.reg,
                                  current_pattern->data->choice.inst.operands[0]
                                      .operand_value.reg);
            strcpy(code[num_of_codes]->lines[1]->word, temp_ins);
            free(temp_ins);

          } else
            switch (current_pattern->data->choice.inst.operands[1].op_type) {
            case REGISTER:
              /*only the sorsce operand is register*/
              num_of_word_nedded += 1;
              
              temp_reg = encoded_registers(
                  current_pattern->data->choice.inst.operands[1]
                      .operand_value.reg,
                  0);
              strcpy(code[num_of_codes]->lines[1]->word, temp_reg);
              free(temp_reg);
              break;
            case IMMEDIATE_NUMBER:
              num_of_word_nedded += 1;
              
              num = extract_immidiate_number(current_pattern->data, 1);
              
              strcpy(code[num_of_codes]->lines[1]->word,
                     encoded_immidiate_number(num));
              break;
            case DIRECT:
              num_of_word_nedded += 1;
              temp_ins = encoded_data(0);
              strcpy(code[num_of_codes]->lines[1]->word,
                     temp_ins); /*will encoded next round*/
              free(temp_ins);
              break;
            case DIRECT_INDEX:

              num_of_word_nedded += 2;
              strcpy(code[num_of_codes]->lines[1]->word,
                     "00000000000000"); /*will encoded next round*/
              num = extract_immidiate_number(current_pattern->data, 1);
              temp_ins = encoded_immidiate_number(num);
              strcpy(code[num_of_codes]->lines[2]->word, temp_ins);
              free(temp_ins);
              break;
            }
        }
        switch (current_pattern->data->choice.inst.operands[0].op_type) {
        case REGISTER:
          if ((current_pattern->data->choice.inst.num_of_operands == 2) &&
              (current_pattern->data->choice.inst.operands[1].op_type !=
               REGISTER)) {
            num_of_word_nedded += 1;
            strcpy(code[num_of_codes]->lines[num_of_word_nedded - 1]->word,
                   encoded_registers(
                       0, current_pattern->data->choice.inst.operands[0]
                              .operand_value.reg));
          }
          break;

        case IMMEDIATE_NUMBER:
          num_of_word_nedded += 1;
          num = extract_immidiate_number(current_pattern->data, 0);
          temp_ins = encoded_immidiate_number(num);
          strcpy(code[num_of_codes]->lines[num_of_word_nedded - 1]->word,
                 temp_ins);
          free(temp_ins);
          break;

        case DIRECT:
          num_of_word_nedded += 1;
          temp_data = encoded_data(0);
          strcpy(code[num_of_codes]->lines[num_of_word_nedded - 1]->word,
                 temp_data);
          free(temp_data);
          break;

        case DIRECT_INDEX:
          num_of_word_nedded += 2;
             temp_data = encoded_data(0);
          strcpy(code[num_of_codes]->lines[num_of_word_nedded - 2]->word,
                 temp_data);
          free(temp_data);
          num = extract_immidiate_number(current_pattern->data, 0);
          temp_data = encoded_immidiate_number(num);
          strcpy(code[num_of_codes]->lines[num_of_word_nedded - 1]->word,
                 temp_data);
          free(temp_data);
          break;
        }
      }
      num_of_codes++;
      IC += num_of_word_nedded;
    }
  }
  add_IC_to_symbol_table(IC);
}
