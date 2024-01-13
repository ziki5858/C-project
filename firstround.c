#include "front.h"
#include "tables.h"
#include "trie/trie.h"
#include <stdio.h>
#define WIDTH_OF_WORD 15 /*14?????????????*/
#define MAX_DATA_VALUE 8191
#define MIN_DATA_VALUE -8192

extern int num_of_patterns; /* אולי מהקדם אסמבלר?*/
extern int num_of_symbols;
extern int num_of_entries;
extern int num_of_externals;

extern Trie macro_trie;
char saved_words[26][6] = {"mov", "cmp", "add",   "sub",    "lea", "not", "clr",
                           "inc", "dec", "jmp",   "bne",    "red", "prn", "jsr",
                           "rts", "hlt", "entry", "extern", "r0",  "r1",  "r2",
                           "r3",  "r4",  "r5",    "r6",     "r7"};
int error_flag = 0;
int num_of_codes = 0;
int current_pattern_num = 0;

struct symbol {
  char *label;
  int address;
  enum { CODE, DATA, EXTERN, ENTRY, ENTRY_CODE, ENTRY_DATA } type;
  int line_in_file;
};

struct constant {
  char *label;
  int value;
  int line_in_file;
};

struct external {
  char *label;
  int *addresses;
  int number_of_addresses;
  int line_in_file;
};

struct entry {
  Symbol symbol;
  int line_in_file;
};

struct word_bin {
  char word[WIDTH_OF_WORD]; /**/
};

struct code {
  int num_of_lines;
  WordBin *lines;
  int line_in_file;
};

Trie symbols, constant, externals, entries; /*אולי להוסיף עץ בשם לבהירות*/
int IC = 0, DC = 0;
Code *code, data;

Symbol *symbol_table;
int num_of_symbols = 0;

External *external_table;
int num_of_externals = 0;

Entry *entry_table;
int num_of_entries = 0;

Symbol create_symbol(char *label, int address, int type, int line_in_file) {
  Symbol s = (Symbol)calloc(1, sizeof(struct symbol));
  if (!s) {
	print_error_memory(line_in_file);
    return NULL;
  }
  s->label = label;
  s->address = address;
  s->type = type;
  s->line_in_file = line_in_file;
  if (insert_to_trie(symbols, label, s)) {
    symbol_table[num_of_symbols++] = s;
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
  if (insert_to_trie(constant, label, c))
    return c;
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
    external_table[num_of_externals++] = e;
    return e;
  }
  print_error_msg("fail to add external", line_in_file);
  return NULL;
}

void insert_address_to_external(External e, int address) {
  e->addresses =
      (int *)realloc(e->addresses, sizeof(int) * (e->number_of_addresses + 1));
  e->addresses[e->number_of_addresses++] = address;
}

Entry create_entry(Symbol symbol) { /*למ הוגדר ככה בסימבול?*/
  Entry e = (Entry)calloc(1, sizeof(struct entry));
  e->symbol = symbol;
  if (insert_to_trie(entries, symbol->label, e)) {
    entry_table[num_of_entries++] = e;
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
    if (s->type == DATA || s->type == ENTRY_DATA)
      s->address += (IC + 100);
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
  result[WIDTH_OF_WORD] = '\0';
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
  result[WIDTH_OF_WORD - 1] = '\0';
  result[12] = '0';
  result[13] = '0';
  return result;
}

int check_validation_of_operands_num(struct pattern *instruction) {
  int i;
  if (instruction->inst.num_of_operands == 0) {
    if (instruction->inst.op_type == RTS || instruction->inst.op_type == HLT)
      return 1;
    else
      return 0;
  }
  if (instruction->inst.num_of_operands == 1) {
    if (instruction->inst.op_type == NOT || instruction->inst.op_type == CLR ||
        instruction->inst.op_type == INC || instruction->inst.op_type == DEC ||
        instruction->inst.op_type == JMP || instruction->inst.op_type == BNE ||
        instruction->inst.op_type == RED || instruction->inst.op_type == PRN ||
        instruction->inst.op_type == JSR)
      return 1;
    else
      return 0;
  }
  if (instruction->inst.num_of_operands == 2) {
    if (instruction->inst.op_type == MOV || instruction->inst.op_type == CMP ||
        instruction->inst.op_type == ADD || instruction->inst.op_type == SUB ||
        instruction->inst.op_type == LEA)
      return 1;
    else
      return 0;
  }
  return 0;
}

int check_validation_of_operands_type(struct pattern *instruction) {
  int i = instruction->inst.op_type;
  if ((i == MOV || (i >= ADD && i <= DEC) || i == RED) &&
      (instruction->inst.operands[0].op_type != IMMEDIATE_NUMBER))
    return 1;
  if ((i == JMP || i == BNE || i == JSR) &&
      (instruction->inst.operands[0].op_type != DIRECT &&
       instruction->inst.operands[0].op_type != REGISTER))
    return 1;
  if ((i == LEA) &&
      ((instruction->inst.operands[1].op_type != REGISTER) &&
       (instruction->inst.operands[1].op_type != IMMEDIATE_NUMBER)))
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

  char *op_code = toBinaryString(instruction->inst.op_type, 4);

  for (i = 0; i < 4; i++) {
    result[i + 4] = op_code[i];
  }
  /*assumming that the first operand is the target operand
   */
  for (i = 0; i < instruction->inst.num_of_operands; i++) {
    char *operand =
        toBinaryString(instruction->inst.operands[1 - i].op_type, 2);
    for (j = 0; j < 2; j++) {
      result[i * 2 + j + 8] = operand[j];
    }
  }

  return result;
}

int extract_immidiate_number(struct pattern *p, int num_of_operand) {
  char *buffer = p->inst.operands[num_of_operand].operand_value.const_num;
  if (buffer[0]) {
    Constant c = (Constant)exist_in_trie(constant, buffer);
    if (c)
      return c->value;
    else {
		print_error_msg("constant not found", current_pattern_num);
    
    	return 0;
    }
  } else {
    return p->inst.operands[num_of_operand].operand_value.value;
  }
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

void first_round(struct node *head) {
  struct node *current_pattern = head;

  char buffer[MAX_LINE_SIZE];
  int data_element;
  int sign = 1;

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
  symbol_table = (Symbol *)calloc(num_of_symbols, sizeof(Symbol));
  entry_table = (Entry *)calloc(num_of_entries, sizeof(Entry));
  external_table = (External *)calloc(num_of_externals, sizeof(External));

  while (current_pattern) {
	current_pattern_num++;
    /*
    dealing with labels
    check if the label is already exist in the tries

    */
    if (current_pattern->data->label[0]) {

      /* check if the label is already exist in the trie of macros */
      if (exist_in_trie(macro_trie, current_pattern->data->label)) {
        print_error_msg("label already exist as macro", current_pattern_num);
        continue;
      }
      /* check if the label is already exist in the table of saved words */
      if (is_saved_word(current_pattern->data->label)) {
		print_error_msg("label already exist as saved word", current_pattern_num);
        continue;
      }
      /* check if the label is already exist in the trie of consts */
      if (exist_in_trie(constant, current_pattern->data->label)) {
		print_error_msg("label already exist as constant", current_pattern_num);
       
        continue;
      }
    }
    /* check the type of the line */
    switch (current_pattern->data->type_line) {

    /* dealing with syntax errors */
    case ERROR:
      printf("%s\n", current_pattern->data->error);
      break;

      /* dealing with directives */
    case DIRECTIVE:

      /* check the type of the directive */
      switch (current_pattern->data->dir.directive_type) {

      /* dealing with data directive */
      case DATA:

        /* check if there is a label */
        if (current_pattern->label[0]) {

          /* check if the label is already exist in the trie of symbols */
          Symbol s = (Symbol)exist_in_trie(symbols, current_pattern->label);
          if (s && s->type == ENRTY) { /* define as entry without data */
            s->type = ENTRY_DATA;
            s->address = DC;
          } else if (!s) {
            s = create_symbol(current_pattern->label, DC,
                              DATA, current_pattern_num); /* define as data */
            if (!s) {
				print_error_msg("fail to add symbol", current_pattern_num);
                 }
          } else { /* already exist */
		  	print_error_msg("symbol already exist", current_pattern_num);
            }
        }
        /* insert the data to the data table */
        data->lines = (WordBin *)realloc(
            data->lines, sizeof(WordBin) * (data->num_of_lines +
                                            current_pattern->data->dir.size));
        for (int i = 0; i < current_pattern->data->dir.size; i++) {
          buffer = current_pattern->data->dir.data[i];

          if (exist_in_trie(constant, buffer)) { /* if the data is a constant */
            data_element = ((Constant)exist_in_trie(constant, buffer))->value;
          } else { /* if the data is a number */
            if (buffer[0] == '-' || buffer[0] == '+') { /* ignore the sign */
              sign = determaine_sign(buffer); /* determaine the sign */
              buffer++;
            }
            if (strlen(buffer) !=
                strspn(buffer,
                       "0123456789")) { /* check if the data is a number */
					   print_error_msg("data element is not a number", current_pattern_num);
               continue;
            } else
              data_element = atoi(buffer) * sign;
          }
          if (data_element > MAX_DATA_VALUE || data_element < MIN_DATA_VALUE) {
			print_error_msg("data element is out of range", current_pattern_num);
            continue;
          }
          /* insert the data to the data table */
          data->lines[data->num_of_lines + i].word =
              encoded_data(data_element); /* -> */
        }
        data->num_of_lines += current_pattern->data->dir.size;
		data->line_in_file = current_pattern_num;
        DC += current_pattern->data->dir.size;
        break;

      case ENRTY: /*אולי להשמיט את בדיקת התווית במקרה כזה*/
        if (exist_in_trie(entries, current_pattern->label)) {
          /* seem to be allowed */
          continue;
        }
        if (exist_in_trie(externals, current_pattern->label)) {
			print_error_msg("symbol already exist as external", current_pattern_num);
          continue;
        }
        if (Symbol s = (Symbol)exist_in_trie(symbols, current_pattern->label)) {
          // Symbol s = (Symbol)exist_in_trie(symbols,
          // current_pattern->label);
          if (s->type == DATA || s->type == CODE) {
            s->type = (s->type == DATA) ? ENTRY_DATA : ENTRY_CODE;
            create_entry(s, current_pattern_num);
          } else {
            /* seem to be allowed */
            continue;
          }
        } else { /* symbol not found */
          Symbol s = create_symbol(current_pattern->label, 0, ENTRY, current_pattern_num);
          create_entry(s, current_pattern_num);
        }

      case EXTERN:
        if (exist_in_trie(entries, current_pattern->label)) {
			print_error_msg("symbol already exist as entry", current_pattern_num);
          continue;
        }
        if (exist_in_trie(externals, current_pattern->label)) {
          /* seem to be allowed */
          continue;
        }
        if (symbols s = exist_in_trie(symbols, current_pattern->label)) {
			print_error_msg("symbol already exist as symbol", current_pattern_num);
          continue;

        } else {
          Symbol s = create_symbol(current_pattern->label, 0, EXTERN, current_pattern_num);
          create_external(current_pattern->label, current_pattern_num);
        }
        break;
      }
      break;

    case DEFINE:
      if (exist_in_trie(symbols, current_pattern->label)) {
		print_error_msg("symbol already exist as symbol", current_pattern_num);
        continue;
      }
      Constant c = create_constant(current_pattern->label,
                                   current_pattern->data->def.value);
      break;

    case INSTRUCTION:
	  
	  current_pattern->data->code = code[num_of_codes];
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
      if (current_pattern->label[0]) {
        Symbol s = (Symbol)exist_in_trie(symbols, current_pattern->label);
        if (s && s->type == ENRTY) {
          s->type = ENTRY_CODE;
          s->address = IC;
        } else if (!s) {
          s = create_symbol(current_pattern->label, IC, CODE, current_pattern_num);
          if (!s) {
			print_error_msg("fail to add symbol", current_pattern_num);abel);
            continue;
          }
        } else {
			print_error_msg("symbol already exist", current_pattern_num);el);
          continue;
        }
      }

      int num_of_word_nedded = 1;

      code[num_of_codes].lines = (WordBin *)calloc(1, sizeof(WordBin));
      code[num_of_codes].lines[0].word =
          encoded_instruction(current_pattern->data);
      code[num_of_codes].num_of_lines = 1;
	  code[num_of_codes].line_in_file = current_pattern_num;

      /* check if there ara operands */
      if (current_pattern->data->inst.num_of_operands >= 1) {

        /* check the type of the first operand */
        if (current_pattern->data->inst.num_of_operands ==
            2) { /*there is olso sorce op*/
          if ((current_pattern->data->inst.operands[0].op_type == REGISTER) &&
              (current_pattern->data->inst.operands[1].op_type ==
               REGISTER)) { /*tow registers*/
            num_of_word_nedded += 1;
            code[num_of_codes].lines = (WordBin *)realloc(
                code[num_of_codes].lines, sizeof(WordBin) * num_of_word_nedded);
            code[num_of_codes].lines[1].word = encoded_registers(
                current_pattern->data->inst.operands[1].operand_value.reg,
                current_pattern->data->inst.operands[0].operand_value.reg);

          } else
            switch (current_pattern->data->inst.operands[1].op_type) {
            case REGISTER:
              /*only the sorsce operand is register*/
              num_of_word_nedded += 1;
              code[num_of_codes].lines =
                  (WordBin *)realloc(code[num_of_codes].lines,
                                     sizeof(WordBin) * num_of_word_nedded);
              code[num_of_codes].lines[1].word = encoded_registers(
                  current_pattern->data->inst.operands[1].operand_value.reg, 0);

            case IMMEDIATE_NUMBER:
              num_of_word_nedded += 1;
              int num;
              num = extract_immidiate_number(current_pattern->data, 1);
              code[num_of_codes].lines =
                  (WordBin *)realloc(code[num_of_codes].lines,
                                     sizeof(WordBin) * num_of_word_nedded);
              code[num_of_codes].lines[1].word = encoded_immidiate_number(num);

            case DIRECT:
              num_of_word_nedded += 1;
              code[num_of_codes].lines =
                  (WordBin *)realloc(code[num_of_codes].lines,
                                     sizeof(WordBin) * num_of_word_nedded);
              code[num_of_codes].lines[1].word =
                  encoded_data(0); /*will encoded next round*/
              
            case DIRECT_INDEX:

              num_of_word_nedded += 2;
              code[num_of_codes].lines =
                  (WordBin *)realloc(code[num_of_codes].lines,
                                     sizeof(WordBin) * num_of_word_nedded);
              code[num_of_codes].lines[1].word =
                  encoded_data(0); /*will encoded next round*/
              num = extract_immidiate_number(current_pattern->data, 0);
              code[num_of_codes].lines[2].word = encoded_immidiate_number(num);
              
            }
        }
        switch (current_pattern->data->inst.operands[0].op_type) {
        case REGISTER:
          if ((current_pattern->data->inst.num_of_operands == 2) &&
              (current_pattern->data->inst.operands[1].op_type != REGISTER)) {
            num_of_word_nedded += 1;
            code[num_of_codes].lines = (WordBin *)realloc(
                code[num_of_codes].lines, sizeof(WordBin) * num_of_word_nedded);
            code[num_of_codes].lines[num_of_word_nedded - 1].word =
                encoded_registers(
                    0,
                    current_pattern->data->inst.operands[0].operand_value.reg);
          }
          break;

        case IMMEDIATE_NUMBER:
          num_of_word_nedded += 1;
          code[num_of_codes].lines = (WordBin *)realloc(
              code[num_of_codes].lines, sizeof(WordBin) * num_of_word_nedded);
          num = extract_immidiate_number(current_pattern->data, 0);
          code[num_of_codes].lines[num_of_word_nedded - 1].word =
              encoded_immidiate_number(num);
          break;

        case DIRECT:
          num_of_word_nedded += 1;
          code[num_of_codes].lines = (WordBin *)realloc(
              code[num_of_codes].lines, sizeof(WordBin) * num_of_word_nedded);
          code[num_of_codes].lines[num_of_word_nedded - 1].word =
              encoded_data(0);
          break;

        case DIRECT_INDEX:
          num_of_word_nedded += 2;
          code[num_of_codes].lines = (WordBin *)realloc(
              code[num_of_codes].lines, sizeof(WordBin) * num_of_word_nedded);
          code[num_of_codes].lines[num_of_word_nedded - 2].word =
              encoded_data(0);
          num = extract_immidiate_number(current_pattern->data, 0);
          code[num_of_codes].lines[num_of_word_nedded - 1].word =
              encoded_immidiate_number(num);
          break;
        }
        num_of_codes++;
        IC += num_of_word_nedded;
        break;
      }
    }
    current_pattern = current_pattern->next;
  }
  add_IC_to_symbol_table(IC);
}

int main() { return 0; }