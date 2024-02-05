#include "front.h"
#include "tables.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIDTH_OF_WORD 15 /*14?????????????*/
#define MAX_DATA_VALUE 8191
#define MIN_DATA_VALUE -8192

extern int num_of_patterns; /* אולי מהקדם אסמבלר?*/

// extern Trie macro_trie;
char saved_words[26][6] = {"mov", "cmp", "add",   "sub",    "lea", "not", "clr",
                           "inc", "dec", "jmp",   "bne",    "red", "prn", "jsr",
                           "rts", "hlt", "entry", "extern", "r0",  "r1",  "r2",
                           "r3",  "r4",  "r5",    "r6",     "r7"};
int error_flag = 0;
int num_of_codes = 0;
int current_pattern_num = 0;

Trie symbols, constant, externals, entries; /*אולי להוסיף עץ בשם לבהירות*/
int IC = 0, DC = 0;
Code *code, data;

Symbol *symbol_table;
int num_of_symbols = 0, size_of_symbol_table = 4;

External *external_table;
int num_of_externals_in_table = 0;

Entry *entry_table;
int num_of_entries_in_table = 0;

Symbol *symbol_table_of_entries;
int num_of_symbols_in_entries = 0;

// Constant *constant_table;
// int num_of_constants_in_table = 0;
// int size_in_constant_table = 4;

Symbol create_symbol(char *label, int address, int type, int line_in_file) {
  Symbol s = (Symbol)calloc(1, sizeof(struct symbol));
  if (!s) {
    print_error_memory(line_in_file);
    return NULL;
  }
  strcpy(s->label, label);
  s->address = address;
  s->type = type;
  s->line_in_file = line_in_file;

  if (insert_to_trie(symbols, label, s)) {
    if (num_of_symbols == size_of_symbol_table) {
      symbol_table = (Symbol *)realloc(
          symbol_table, sizeof(Symbol) * (size_of_symbol_table * 2));
      size_of_symbol_table *= 2;
    }
    (symbol_table[num_of_symbols]) = s;
    num_of_symbols++;
    return s;
  }
  print_error_msg("fail to add symbol", line_in_file);
  return NULL;
}

Constant create_constant(char *label, int value, int line_in_file) {
  Constant c = (Constant)calloc(1, sizeof(struct constant));
  if (!c) {
    print_error_memory(line_in_file);
    return NULL;
  }
  c->label = label;
  c->value = value;
  c->line_in_file = line_in_file;
  if (insert_to_trie(constant, label, c)) {
    // if (num_of_constants_in_table == size_in_constant_table)
    //   constant_table = (Constant *)realloc(
    //       constant_table, sizeof(Constant) * (size_in_constant_table *= 2));
    // constant_table[num_of_constants_in_table++] = c;
    // size_in_constant_table *= 2;
    return c;
  }
  print_error_msg("fail to add constant", line_in_file);
  return NULL;
}

External create_external(char *label, int line_in_file) {
  External e = (External)calloc(1, sizeof(struct external));
  if (!e) {
    print_error_memory(line_in_file);
    return NULL;
  }
  e->label = label;
  e->line_in_file = line_in_file;
  e->addresses = (int *)calloc(1, sizeof(int));
  //   e->addresses[0] = address;
  //   e->number_of_addresses = 1;
  if (insert_to_trie(externals, label, e)) {
    external_table[num_of_externals_in_table++] = e;
    return e;
  }
  print_error_msg("fail to add external", line_in_file);
  return NULL;
}

void insert_address_to_external(External e, int address) {
  e->addresses =
      (int *)realloc(e->addresses, sizeof(int) * (e->number_of_addresses + 1));
  e->addresses[e->number_of_addresses] = address;
  e->number_of_addresses++;
}

Entry create_entry(Symbol symbol, int line_in_file) { /*למ הוגדר ככה בסימבול?*/
  Entry e = (Entry)calloc(1, sizeof(struct entry));
  e->symbol = symbol;
  e->line_in_file = line_in_file;
  if (insert_to_trie(entries, symbol->label, e)) {
    entry_table[num_of_entries_in_table++] = e;
    return e;
  }
  return NULL;
}

void print_error_memory(int line) {
  printf("error: fail to allocate memory in line %d\n", line);
  error_flag = 1;
}

void print_error_msg(char *msg, int line) {
  printf("error: %s in line %d\n", msg, line);
  error_flag = 1;
}

void add_IC_to_symbol_table(int IC) {
  int i;
  Symbol s;
  for (i = 0; i < num_of_symbols; i++) {
    s = symbol_table[i];
    if (s->type == S_DATA || s->type == ENTRY_DATA)
      s->address += (IC + 100);
    else if (s->type == CODE || s->type == ENTRY_CODE)
      s->address += 100;
  }
}

char *toBinaryString(int n, int num_bits) {
  char *string = malloc(num_bits + 1);
  if (!string) {
    return NULL;
  }
  for (int i = num_bits - 1; i >= 0; i--) {
    string[i] = (n & 1) + '0';
    n >>= 1;
  }
  string[num_bits] = '\0';
  return string;
}

char *encoded_register(int n) { return toBinaryString(n, 3); }

char *encoded_registers(int n1, int n2) {
  char *result = malloc(WIDTH_OF_WORD); /*now its 15*/
  if (!result) {
    return NULL;
  }
  result[WIDTH_OF_WORD - 1] = '\0';
  int i;

  /* initialize result to 0 */
  for (i = 0; i < 14; i++) {
    result[i] = '0';
  }

  char *reg1 = encoded_register(n1);
  char *reg2 = encoded_register(n2);

  for (i = 0; i < 3; i++) {
    result[i + 6] = reg1[i];
    result[i + 9] = reg2[i];
  }
  free(reg1);
  free(reg2);
  return result;
}

char *encoded_data(int n) {
  return toBinaryString(n, WIDTH_OF_WORD - 1);
} /*now 15*/

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
  result[12] = '0';
  result[13] = '0';
  return result;
}

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

char *encoded_instruction(struct pattern *instruction) {
  char *result = malloc(WIDTH_OF_WORD);
  if (!result) {
    print_error_memory(current_pattern_num);
    return NULL;
  }
  result[WIDTH_OF_WORD - 1] = '\0';
  int i, j;

  /* initialize result to 0 */
  for (i = 0; i < 14; i++) {
    result[i] = '0';
  }

  char *op_code = toBinaryString(instruction->choice.inst.op_type, 4);

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

int is_saved_word(char *word) {
  int i;
  for (i = 0; i < 26; i++) {
    if (strcmp(word, saved_words[i]) == 0)
      return 1;
  }
  return 0;
}

int determaine_sign(char *buffer) {
  if (buffer[0] == '-') {
    return -1;
  } else if (buffer[0] == '+' || isdigit(buffer[0])) {
    return 1;
  } else {
    return 0;
  }
}

int containsOnlyDigits(const char *str) {
  while (*str) {
    if (!isdigit(*str)) {
      return 0; // Not a digit
    }
    str++;
  }
  return 1; // All characters are digits
}

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

void handle_lable_define(struct pattern *p, char *type, int counter) {
  int type_of_op = (strcmp(type, "data") == 0) ? 1 : 0;

  Symbol s = (Symbol)exist_in_trie(symbols, p->label);
  if (s && s->type == S_ENTRY) { /* define as entry without data */
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

void allocate_memory_for_instructions(struct pattern *ins, Code *code_i) {
  int amount;
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

  for (int i = 0; i < amount; i++) {
    (*code_i)->lines[i] = (WordBin)calloc(1, sizeof(struct word_bin));
    if (!(*code_i)->lines[i]) {
      print_error_memory(current_pattern_num);
      return;
    }
  }
}

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

void first_round(struct Node *head) {
  struct Node *current_pattern = head;

  char *buffer;
  int data_element;
  int sign = 1, i, char_index, num;
  Symbol s;

  // buffer = (char *)calloc(MAX_LINE_SIZE, sizeof(char));
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
  // constant_table = (Constant *)calloc(size_in_constant_table,
  // sizeof(Constant));

  // while (current_pattern) {
  for (current_pattern_num = 1; current_pattern_num <= num_of_patterns;
       current_pattern = current_pattern->next, current_pattern_num++) {
    /*
    dealing with labels
    check if the label is already exist in the tries

    */
    if (current_pattern->data->label[0]) {
      handle_lable_error(current_pattern->data);

      //   /* check if the label is already exist in the trie of macros */
      //   if (exist_in_trie(macro_trie, current_pattern->data->label)) {
      //     print_error_msg("label already exist as macro",
      //     current_pattern_num); continue;
      //   }
      //   /* check if the label is already exist in the table of saved words */
      //   if (is_saved_word(current_pattern->data->label)) {
      //     print_error_msg("label already exist as saved word",
      //                     current_pattern_num);
      //     continue;
      //   }
      //   /* check if the label is already exist in the trie of consts */
      //   if (exist_in_trie(constant, current_pattern->data->label)) {
      //     print_error_msg("label already exist as constant",
      //     current_pattern_num);

      //     continue;
      //   }
    }
    /* check the type of the line */
    switch (current_pattern->data->type_line) {

    /* dealing with syntax errors */
    case ERROR:
      printf("%s\n", current_pattern->data->choice.error);
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

          //   /* check if the label is already exist in the trie of symbols */
          //   s = (Symbol)exist_in_trie(symbols, current_pattern->data->label);
          //   if (s && s->type == ENTRY) { /* define as entry without data */
          //     s->type = ENTRY_DATA;
          //     s->address = DC;
          //   } else if (!s) {
          //     s = create_symbol(current_pattern->data->label, DC, S_DATA,
          //                       current_pattern_num); /* define as data */
          //     if (!s) {
          //       print_error_msg("fail to add symbol", current_pattern_num);
          //     }
          //   } else { /* already exist */
          //     print_error_msg("symbol already exist", current_pattern_num);
          //   }
        }
        /* insert the data to the data table */
        // data->lines = (WordBin *)realloc(
        //     data->lines, sizeof(WordBin) * (data->num_of_lines +
        //                                     current_pattern->data->dir.size));
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
          char *temp_data = encoded_data(data_element);
          strcpy(data->lines[data->num_of_lines + i]->word, temp_data); /* -> */
          free(temp_data);
        }
        data->num_of_lines += current_pattern->data->choice.dir.size;
        // data->line_in_file = current_pattern_num;
        DC += current_pattern->data->choice.dir.size;
        break;

      case STRING:

        if (current_pattern->data->label[0]) {
          handle_lable_define(current_pattern->data, "data", DC);
          //   s = (Symbol)exist_in_trie(symbols, current_pattern->data->label);
          //   if (s && s->type == ENTRY) { /* define as entry without data */
          //     s->type = ENTRY_DATA;
          //     s->address = DC;
          //   } else if (!s) {
          //     s = create_symbol(current_pattern->data->label, DC, S_DATA,
          //                       current_pattern_num); /* define as data */
          //     if (!s) {
          //       print_error_msg("fail to add symbol", current_pattern_num);
          //     }
          //   } else { /* already exist */
          //     print_error_msg("symbol already exist", current_pattern_num);
          //   }
        }
        /* insert the data to the data table */
        // data->lines = (WordBin *)realloc(
        //     data->lines, sizeof(WordBin) * (data->num_of_lines +
        //                                     current_pattern->data->dir.size));
        allocate_memory_for_data(current_pattern->data);
        for (i = 0; i < current_pattern->data->choice.dir.size; i++) {
          char_index = current_pattern->data->choice.dir.string[i];
          char *temp_data = encoded_data(char_index);
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
          // Symbol s = (Symbol)exist_in_trie(symbols,
          // current_pattern->data->label);
          if (s->type == DATA || s->type == CODE) {
            s->type = (s->type == DATA) ? ENTRY_DATA : ENTRY_CODE;
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
      Constant c = create_constant(current_pattern->data->label,
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
      //   if (current_pattern->data->label[0]) {
      //     Symbol s = (Symbol)exist_in_trie(symbols,
      //     current_pattern->data->label); if (s && s->type == ENTRY) {
      //       s->type = ENTRY_CODE;
      //       s->address = IC;
      //     } else if (!s) {
      //       s = create_symbol(current_pattern->data->label, IC, CODE,
      //                         current_pattern_num);
      //       if (!s) {
      //         print_error_msg("fail to add symbol", current_pattern_num);
      //         continue;
      //       }
      //     } else {
      //       print_error_msg("symbol already exist", current_pattern_num);
      //       continue;
      //     }
      // }

      allocate_memory_for_instructions(current_pattern->data,
                                       &(code[num_of_codes]));
      current_pattern->data->choice.inst.code = code[num_of_codes];
      int num_of_word_nedded = 1;
      // Code codes = (Code)calloc(1, sizeof(struct code));
      //   code[num_of_codes] = codes;

      //   code[num_of_codes]->lines = (WordBin*)calloc(1, sizeof(WordBin));
      //   code[num_of_codes]->lines[0] = (WordBin)calloc(1, sizeof(struct
      //   word_bin));
      char *temp_ins = encoded_instruction(current_pattern->data);
      strcpy(code[num_of_codes]->lines[0]->word, temp_ins);
      free(temp_ins);
      //   code[num_of_codes]->num_of_lines = 1;
      //   code[num_of_codes]->line_in_file = current_pattern_num;

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
            // code[num_of_codes]->lines = (WordBin *)realloc(
            //     code[num_of_codes]->lines, sizeof(WordBin) *
            //     num_of_word_nedded);
            // 	code[num_of_codes]->lines[1] = (WordBin)calloc(1, sizeof(struct
            // word_bin));
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
              //   code[num_of_codes]->lines =
              //       (WordBin *)realloc(code[num_of_codes]->lines,
              //                          sizeof(WordBin) * num_of_word_nedded);
              // code[num_of_codes]->lines[1] = (WordBin)calloc(1, sizeof(struct
              // word_bin));
              char *temp_reg = encoded_registers(
                  current_pattern->data->choice.inst.operands[1]
                      .operand_value.reg,
                  0);
              strcpy(code[num_of_codes]->lines[1]->word, temp_reg);
              free(temp_reg);
              break;
            case IMMEDIATE_NUMBER:
              num_of_word_nedded += 1;
              int num;
              num = extract_immidiate_number(current_pattern->data, 1);
              //   code[num_of_codes]->lines =
              //       (WordBin *)realloc(code[num_of_codes]->lines,
              //                          sizeof(WordBin) * num_of_word_nedded);
              // 	code[num_of_codes]->lines[1] = (WordBin)calloc(1,
              // sizeof(struct word_bin));
              strcpy(code[num_of_codes]->lines[1]->word,
                     encoded_immidiate_number(num));
              break;
            case DIRECT:
              num_of_word_nedded += 1;
              //   code[num_of_codes]->lines =
              //       (WordBin *)realloc(code[num_of_codes]->lines,
              //                          sizeof(WordBin) * num_of_word_nedded);
              // code[num_of_codes]->lines[1] = (WordBin)calloc(1, sizeof(struct
              // word_bin));
              temp_ins = encoded_data(0);
              strcpy(code[num_of_codes]->lines[1]->word,
                     temp_ins); /*will encoded next round*/
              free(temp_ins);
              break;
            case DIRECT_INDEX:

              num_of_word_nedded += 2;
              //   code[num_of_codes]->lines =
              //       (WordBin *)realloc(code[num_of_codes]->lines,
              //                          sizeof(WordBin) * num_of_word_nedded);
              // code[num_of_codes]->lines[1] = (WordBin)calloc(1, sizeof(struct
              // word_bin));
              strcpy(code[num_of_codes]->lines[1]->word,
                     "00000000000000"); /*will encoded next round*/
              num = extract_immidiate_number(current_pattern->data, 1);
              // code[num_of_codes]->lines[2] = (WordBin)calloc(1, sizeof(struct
              // word_bin));
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
            // code[num_of_codes]->lines = (WordBin *)realloc(
            //     code[num_of_codes]->lines, sizeof(WordBin) *
            //     num_of_word_nedded);
            // code[num_of_codes]->lines[num_of_word_nedded-1] =
            // (WordBin)calloc(1, sizeof(struct word_bin));
            strcpy(code[num_of_codes]->lines[num_of_word_nedded - 1]->word,
                   encoded_registers(
                       0, current_pattern->data->choice.inst.operands[0]
                              .operand_value.reg));
          }
          break;

        case IMMEDIATE_NUMBER:
          num_of_word_nedded += 1;
          //   code[num_of_codes]->lines = (WordBin *)realloc(
          //       code[num_of_codes]->lines, sizeof(WordBin) *
          //       num_of_word_nedded);
          // code[num_of_codes]->lines[num_of_word_nedded-1] =
          // (WordBin)calloc(1, sizeof(struct word_bin));
          num = extract_immidiate_number(current_pattern->data, 0);
          temp_ins = encoded_immidiate_number(num);
          strcpy(code[num_of_codes]->lines[num_of_word_nedded - 1]->word,
                 temp_ins);
          free(temp_ins);
          break;

        case DIRECT:
          num_of_word_nedded += 1;
          //   code[num_of_codes]->lines = (WordBin *)realloc(
          //       code[num_of_codes]->lines, sizeof(WordBin) *
          //       num_of_word_nedded);
          //   code[num_of_codes]->lines[num_of_word_nedded - 1] =
          //   (WordBin)calloc(1, sizeof(struct word_bin));
          char *temp_data = encoded_data(0);
          strcpy(code[num_of_codes]->lines[num_of_word_nedded - 1]->word,
                 temp_data);
          free(temp_data);
          break;

        case DIRECT_INDEX:
          num_of_word_nedded += 2;
          //   code[num_of_codes]->lines = (WordBin *)realloc(
          //       code[num_of_codes]->lines, sizeof(WordBin) *
          //       num_of_word_nedded);
          // code[num_of_codes]->lines[num_of_word_nedded-2] =
          // (WordBin)calloc(1, sizeof(struct word_bin));
          // code[num_of_codes]->lines[num_of_word_nedded-1] =
          // (WordBin)calloc(1, sizeof(struct word_bin));
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
    // current_pattern = current_pattern->next;
  }
  add_IC_to_symbol_table(IC);
  // free(buffer);
}
