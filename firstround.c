#include "trie/trie.h"
#include "front.h"
#include "tables.h"
#include <stdio.h>
#define WIDTH_OF_WORD 15  /*14?????????????*/
#define MAX_DATA_VALUE 8191
#define MIN_DATA_VALUE -8192

extern int num_of_patterns;/* אולי מהקדם אסמבלר?*/
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
};

struct constant {
  char *label;
  int value;
};

struct external {
  char *label;
  int *addresses;
  int number_of_addresses;
};

struct entry {
  Symbol symbol;
};

struct word_bin {
  char word[WIDTH_OF_WORD];  /**/
};

struct code {
  int num_of_lines;
  WordBin *lines;
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

Symbol create_symbol(char *label, int address, int type) {
  Symbol s = (Symbol)calloc(1, sizeof(struct symbol));
  if (!s) {
	printf("error: fail to allocate memory\n");
	error_flag = 1;
	return NULL;
  }
  s->label = label;
  s->address = address;
  s->type = type;
  if (insert_to_trie(symbols, label, s)) {
    symbol_table[num_of_symbols++] = s;
    return s;
  }
  printf("error: fail to add symbol %s\n", label);
  error_flag = 1;
  return NULL;
}

Constant create_constant(char *label, int value) {
  Constant c = (Constant)calloc(1, sizeof(struct constant));
  if (!c) {
	printf("error: fail to allocate memory\n");
	error_flag = 1;
	return NULL;
  }
  c->label = label;
  c->value = value;
  if (insert_to_trie(constant, label, c))
    return c;
  printf("error: fail to add constant %s\n", label);
  error_flag = 1;
  return NULL;
}

External create_external(char *label) {
  External e = (External)calloc(1,sizeof(struct external));
  if (!e) {
	printf("error: fail to allocate memory\n");
	error_flag = 1;
	return NULL;
  }
  e->label = label;
  e->addresses = (int *)calloc(1, sizeof(int));
  //   e->addresses[0] = address;
  //   e->number_of_addresses = 1;
  if (insert_to_trie(externals, label, e)) {
    external_table[num_of_externals++] = e;
    return e;
  }
  printf("error: fail to add external %s\n", label);
  error_flag = 1;
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
  char *result = malloc(WIDTH_OF_WORD);  /*now its 15*/
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

char *encoded_data(int n) { return toBinaryString(n, WIDTH_OF_WORD-1); }/*now 15*/

char *encoded_immidiate_number(int n) {
  char *base = toBinaryString(n, 12);
  char *result = calloc(WIDTH_OF_WORD , sizeof(char));
  if (!result) {
    printf("error: fail to allocate memory\n");
    error_flag = 1;
    return NULL;
  }
  strcat(result, base);
  result[WIDTH_OF_WORD - 1] = '\0';
  result[12] = '0';
  result[13] = '0';
  return result;
}

int check_validation_of_operands_num(struct pattern * instruction){
	int i;
  if (instruction->inst.num_of_operands == 0){
	if (instruction.inst.op_type == RTS || instruction.inst.op_type == HLT)
	  return 1;
	else
	  return 0;
  }
  if (instruction->inst.num_of_operands == 1){
	if (instruction.inst.op_type == NOT || instruction.inst.op_type == CLR ||
	  instruction.inst.op_type == INC || instruction.inst.op_type == DEC ||
	  instruction.inst.op_type == JMP || instruction.inst.op_type == BNE ||
	  instruction.inst.op_type == RED || instruction.inst.op_type == PRN ||
	  instruction.inst.op_type == JSR)
	  return 1;
	else
	  return 0;
  }
  if (instruction->inst.num_of_operands == 2){
	if (instruction.inst.op_type == MOV || instruction.inst.op_type == CMP ||
	  instruction.inst.op_type == ADD || instruction.inst.op_type == SUB ||
	  instruction.inst.op_type == LEA)
	  return 1;
	else
	  return 0;
  }
  return 0;

int check_validation_of_operands_type(struct pattern * instruction){
	int i = instruction->inst.op_type;
    if ((i == MOV || (i >= ADD && i <= DEC) || i == RED) && (instruction->inst.operands[0].op_type != IMMEDIATE_NUMBER))
	  return 1;
	if ((i == JMP || i == BNE || i == JSR) && (instruction->inst.operands[0].op_type != DIRECT && instruction->inst.operands[0].op_type != REGISTER))
	  return 1;
	if ((i == LEA) && ((instruction->inst.operands[1].op_type != REGISTER) && (instruction->inst.operands[1].op_type != IMMEDIATE_NUMBER)))
	  return 1;






char *encoded_instruction(struct pattern *instruction) {
  char *result = malloc(WIDTH_OF_WORD);
  if (!result) {
    printf("error: fail to allocate memory\n");
    error_flag = 1;
    return NULL;
  }
  result[WIDTH_OF_WORD-1] = '\0';
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
    char *operand = toBinaryString(instruction->inst.operands[1-i].op_type, 2);
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
      printf("error: constant %s not found\n", buffer);
      error_flag = 1;
      return 0;
    }
  } else {
    return p->inst.operands[num_of_operand].operand_value.value;
  }
  return 0;
}

int is_saved_word(char *word) {
  for (int i = 0; i < 26; i++) {
    if (strcmp(word, saved_words[i]) == 0)
      return 1;
  }
  return 0;
}

void first_round(struct node *head) {
  struct node *current_pattern = head;

  char buffer[MAX_LINE_SIZE];
  int data_element;

  symbols = trie();
  externals = trie();
  entries = trie();
  constant = trie();

  code = (Code *)calloc(num_of_patterns, sizeof(Code));
  data = (Code)calloc(1, sizeof(struct code));

  symbol_table = (Symbol *)calloc(num_of_symbols, sizeof(Symbol));
  entry_table = (Entry *)calloc(num_of_entries, sizeof(Entry));
  external_table = (External *)calloc(num_of_externals, sizeof(External));

  while (current_pattern) {
    if (current_pattern->data->label[0]) {
      if (exist_in_trie(macro_trie, current_pattern->data->label)) {
        printf("error: label %s already exist as macro\n",
               current_pattern->data->label);
        error_flag = 1;
        continue;
      }
      if (is_saved_word(current_pattern->data->label)) {
        printf("error: label %s already exist as saved word\n",
               current_pattern->data->label);
        error_flag = 1;
        continue;
      }
      if (exist_in_trie(constant, current_pattern->data->label)) {
        printf("error: label %s already exist as constant\n",
               current_pattern->data->label);
        error_flag = 1;
        continue;
      }
    }
    switch (current_pattern->data->type_line) {
    case ERROR:
      printf("%s\n", current_pattern->data->error);
      break;

    case DIRECTIVE:
      switch (current_pattern->data->dir.directive_type) {
      case DATA:
        if (current_pattern->label[0]) {
          Symbol s = (Symbol)exist_in_trie(symbols, current_pattern->label);
          if (s && s->type == ENRTY) {
            s->type = ENTRY_DATA;
            s->address = DC;
          } else if (!s) {
            s = create_symbol(current_pattern->label, DC, DATA);
            if (!s) {
              error_flag = 1;
              printf("error: fail to add symbol %s\n", current_pattern->label);
            }
          } else {
            error_flag = 1;
            printf("error: symbol %s already exist\n", current_pattern->label);
          }
        }

        data->lines = (WordBin *)realloc(
            data->lines, sizeof(WordBin) * (data->num_of_lines +
                                            current_pattern->data->dir.size));
        for (int i = 0; i < current_pattern->data->dir.size; i++) {
          buffer = current_pattern->data->dir.data[i];
          if (exist_in_trie(constant, buffer)) {
            data_element = ((Constant)exist_in_trie(constant, buffer))->value;
          } else {
            if (buffer[0] == '-' || buffer[0] == '+') {
              buffer++;
            }
            if (strlen(buffer) != strspn(buffer, "0123456789")) {
              printf("error: data element %s is not a number\n", data_element);
              error_flag = 1;
              continue;
            } else
              data_element = atoi(buffer);
          }
          if (data_element > MAX_DATA_VALUE || data_element < MIN_DATA_VALUE) {
            printf("error: data element %s is out of range\n", data_element);
            error_flag = 1;
            continue;
          }

          data->lines[data->num_of_lines + i].word = encoded_data(data_element);
        }
        data->num_of_lines += current_pattern->data->dir.size;
        DC += current_pattern->data->dir.size;
        break;

      case ENRTY:
        if (exist_in_trie(entries, current_pattern->label)) {
          /* seem to be allowed */
          continue;
        }
        if (exist_in_trie(externals, current_pattern->label)) {
          printf("error: symbol %s already exist as external\n",
                 current_pattern->label);
          error_flag = 1;
          continue;
        }
        if (Symbol s = (Symbol)exist_in_trie(symbols, current_pattern->label)) {
          // Symbol s = (Symbol)exist_in_trie(symbols, current_pattern->label);
          if (s->type == DATA) {
            s->type = ENTRY_DATA;
            Entry e = create_entry(current_pattern->label, s);
          } else if (s->type == CODE) {
            s->type = ENTRY_CODE;
            Entry e = create_entry(current_pattern->label, s);
          } else {
            /* seem to be allowed */
            continue;
          }
        } else {
          Symbol s = create_symbol(current_pattern->label, 0, ENTRY);
          create_entry(s);
        }

      case EXTERN:
        if (exist_in_trie(entries, current_pattern->label)) {
          printf("error: symbol %s already exist as entry\n",
                 current_pattern->label);
          error_flag = 1;
          continue;
        }
        if (exist_in_trie(externals, current_pattern->label)) {
          /* seem to be allowed */
          continue;
        }
        if (symbols s = exist_in_trie(symbols, current_pattern->label)) {
          if (s->type != EXTERN) {
            printf("error: symbol %s already exist as symbol\n",
                   current_pattern->label);
            error_flag = 1;
            continue;
          }
        } else {
          Symbol s = create_symbol(current_pattern->label, 0, EXTERN);
          External e = create_external(current_pattern->label);
        }
        break;
      }
      break;
    case DEFINE:
      if (exist_in_trie(symbols, current_pattern->label)) {
        printf("error: word %s already exist as symbol\n",
               current_pattern->label);
        error_flag = 1;
        continue;
      }
      Constant c = create_constant(current_pattern->label,
                                   current_pattern->data->def.value);
      break;

    case INSTRUCTION:
		if (!check_validation_of_operands_num(current_pattern->data)){
			printf("error: invalid number of operands\n");
			error_flag = 1;
			continue;
		}
		if (!check_validation_of_operands_type(current_pattern->data)){
			printf("error: invalid type of operands\n");
			error_flag = 1;
			continue;
		}
      if (current_pattern->label[0]) {
        Symbol s = (Symbol)exist_in_trie(symbols, current_pattern->label);
        if (s && s->type == ENRTY) {
          s->type = ENTRY_CODE;
          s->address = IC;
        } else if (!s) {
          s = create_symbol(current_pattern->label, IC, CODE);
          if (!s) {
            error_flag = 1;
            printf("error: fail to add symbol %s\n", current_pattern->label);
          }
        } else {
          error_flag = 1;
          printf("error: symbol %s already exist\n", current_pattern->label);
        }
      }
      int num_of_word_nedded = 1;
	  
      code[num_of_codes].lines = (WordBin *)calloc(1, sizeof(WordBin));
      code[num_of_codes].lines[0].word = encoded_instruction(current_pattern->data);
      code[num_of_codes].num_of_lines = 1;

      if (current_pattern->data->inst.num_of_operands >= 1) {
        if (current_pattern->data->inst.num_of_operands == 2 &&
            (current_pattern->data->inst.operands[0].op_type == REGISTER) &&
            (current_pattern->data->inst.operands[1].op_type == REGISTER)) {
          num_of_word_nedded = 2;
          code[num_of_codes].lines =
              (WordBin *)realloc(code[num_of_codes].lines, sizeof(WordBin) * 2);
          code[num_of_codes].lines[1].word = encoded_registers(
              current_pattern->data->inst.operands[1].operand_value.reg,
              current_pattern->data->inst.operands[0].operand_value.reg);

        } else if (current_pattern->data->inst.operands[0].op_type ==
                   REGISTER) {
          num_of_word_nedded = 2;
          code[num_of_codes].lines =
              (WordBin *)realloc(code[num_of_codes].lines, sizeof(WordBin) * 2);
          code[num_of_codes].lines[1].word = encoded_registers(
              current_pattern->data->inst.operands[0].operand_value.reg, 0);
        } else if (current_pattern->data->inst.operands[0].op_type ==
                   IMMEDIATE_NUMBER) {
          num_of_word_nedded = 2;
          int num;
          num = extract_immidiate_number(current_pattern->data, 0);
          code[num_of_codes].lines =
              (WordBin *)realloc(code[num_of_codes].lines, sizeof(WordBin) * 2);
          code[num_of_codes].lines[1].word = encoded_immidiate_number(num);
        } else if (current_pattern->data->inst.operands[0].op_type == DIRECT) {
          num_of_word_nedded = 2;
          code[num_of_codes].lines =
              (WordBin *)realloc(code[num_of_codes].lines, sizeof(WordBin) * 2);
          code[num_of_codes].lines[1].word = encoded_data(0);
		  current_pattern->data->code = code[num_of_codes];
        } else if (current_pattern->data->inst.operands[0].op_type ==
                   DIRECT_INDEX) {
          num_of_word_nedded = 3;
          code[num_of_codes].lines =
              (WordBin *)realloc(code[num_of_codes].lines, sizeof(WordBin) * 3);
          code[num_of_codes].lines[1].word = encoded_data(0);
          num = extract_immidiate_number(current_pattern->data, 0);
          code[num_of_codes].lines[2].word = encoded_immidiate_number(num);
		  current_pattern->data->code = code[num_of_codes];
        }
      } if (current_pattern->data->inst.num_of_operands == 2) {
        switch (current_pattern->data->inst.operands[1].op_type) {
        case REGISTER:
          if ((current_pattern->data->inst.operands[1].op_type == REGISTER) &&
              (current_pattern->data->inst.operands[0].op_type != REGISTER)) {
            num_of_word_nedded += 1;
            code[num_of_codes].lines = (WordBin *)realloc(
                code[num_of_codes].lines, sizeof(WordBin) * num_of_word_nedded);
            code[num_of_codes].lines[num_of_word_nedded - 1].word = encoded_registers(
                0, current_pattern->data->inst.operands[1].operand_value.reg);
          }
          break;
        case IMMEDIATE_NUMBER:
          num_of_word_nedded += 1;
          code[num_of_codes].lines = (WordBin *)realloc(
              code[num_of_codes].lines, sizeof(WordBin) * num_of_word_nedded);
          num = extract_immidiate_number(current_pattern->data, 1);
          code[num_of_codes].lines[num_of_word_nedded - 1].word =
              encoded_immidiate_number(num);
          break;
        case DIRECT:
          num_of_word_nedded += 1;
          code[num_of_codes].lines = (WordBin *)realloc(
              code[num_of_codes].lines, sizeof(WordBin) * num_of_word_nedded);
          code[num_of_codes].lines[num_of_word_nedded - 1].word = encoded_data(0);
		  current_pattern->data->code = code[num_of_codes];
          break;
        case DIRECT_INDEX:
          num_of_word_nedded += 2;
          code[num_of_codes].lines = (WordBin *)realloc(
              code[num_of_codes].lines, sizeof(WordBin) * num_of_word_nedded);
          code[num_of_codes].lines[num_of_word_nedded - 2].word = encoded_data(0);
          num = extract_immidiate_number(current_pattern->data, 1);
          code[num_of_codes].lines[num_of_word_nedded - 1].word =
              encoded_immidiate_number(num);
			  current_pattern->data->code = code[num_of_codes];
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