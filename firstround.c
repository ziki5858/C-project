#include "./trie/trie.h"
#include "front.h"
#include "tables.h"
#define WIDTH_OF_WORD 15
#define MAX_DATA_VALUE 8191
#define MIN_DATA_VALUE -8192

extern Trie macro_trie;
char saved_words[26][6] = {"mov", "cmp", "add", "sub", "lea", "not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "hlt", "entry", "extern", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};


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
	char word[WIDTH_OF_WORD];
};

struct code {
	int num_of_lines;
	WordBin *lines;
};

Trie symbols, constant, externals, entries ;
int IC = 0, DC = 0;
Code *code, data;

Symbol *symbol_table;
int num_of_symbols = 0;

External *external_table;
int num_of_externals = 0;

Entry *entry_table;
int num_of_entries = 0;

Symbol create_symbol(char *label, int address, int type) {
  Symbol s = (Symbol)malloc(sizeof(struct symbol));
  s->label = label;
  s->address = address;
  s->type = type;
  if(insert_to_trie(symbols, label, s)){
	symbol_table[num_of_symbols++] = s;
	return s;
  }
  return NULL;
}


Constant create_constant(char *label, int value) {
  Constant c = (Constant)malloc(sizeof(struct constant));
  c->label = label;
  c->value = value;
  if(insert_to_trie(constant, label, c))
    return c;
  return NULL;
}

External create_external(char *label) {
  External e = (External)malloc(sizeof(struct external));
  e->label = label;
  e->addresses = (int *)calloc(1, sizeof(int));
//   e->addresses[0] = address;
//   e->number_of_addresses = 1;
  if (insert_to_trie(externals, label, e))
    {
	  external_table[num_of_externals++] = e;
	  return e;
	}
	
  return NULL;
}

void insert_address_to_external(External e, int address) {
  e->addresses = (int *)realloc(e->addresses, sizeof(int) * (e->number_of_addresses + 1));
  e->addresses[e->number_of_addresses++] = address;
}

Entry create_entry(Symbol symbol) {
  Entry e = (Entry)malloc(sizeof(struct entry));
  e->symbol = symbol;
  if(insert_to_trie(entries, symbol->label, e))
    {
	entry_table[num_of_entries++] = e;	
  	return e;
	}
  return NULL;
}



void add_IC_to_symbol_table(int IC) {
  int i;
  Symbol s;
  for(i=0;i<num_of_symbols;i++) {
	s = symbol_table[i];
	if(s->type == DATA || s->type == ENTRY_DATA)
	  s->address += IC;
  }
}

char* encoded_data(int n) {
  int num_bits = 14;
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
  int error_flag = 0;

  symbols = trie();
  externals = trie();
  entries = trie();
  constant = trie();
  
  extern int num_of_patterns;
  extern int num_of_symbols;
  extern int num_of_entries;
  extern int num_of_externals;
  
  
  code = (Code *)calloc(num_of_patterns,sizeof(Code));
  data = (Code)calloc(1,sizeof(struct code));

  symbol_table = (Symbol *)calloc(num_of_symbols,sizeof(Symbol));
  entry_table = (Entry *)calloc(num_of_entries,sizeof(Entry));
  external_table = (External *)calloc(num_of_externals,sizeof(External));
  

  while (current_pattern) {
	if (current_pattern->data->label[0])
	{
		if(exist_in_trie(macro_trie, current_pattern->data->label))
		{
			printf("error: label %s already exist as macro\n", current_pattern->data->label);
			error_flag = 1;
			continue;
		}
		if (is_saved_word(current_pattern->data->label))
		{
			printf("error: label %s already exist as saved word\n", current_pattern->data->label);
			error_flag = 1;
			continue;
		}
		if (exist_in_trie(constant, current_pattern->data->label))
		{
			printf("error: label %s already exist as constant\n", current_pattern->data->label);
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
	  	if(current_pattern->label[0])
		{
			Symbol s = (Symbol)exist_in_trie(symbols, current_pattern->label);
			if (s && s->type == ENRTY)
			{
				s->type = ENTRY_DATA;
				s->address = DC;
			}
			else if (!s)
			{
				s = create_symbol(current_pattern->label, DC, DATA);
				if (!s)
				{	error_flag = 1;
					printf("error: fail to add symbol %s\n", current_pattern->label);
				}
			}
			else
			{	error_flag = 1;
				printf("error: symbol %s already exist\n", current_pattern->label);
			}
		}

		data->lines = (WordBin *)realloc(data->lines, sizeof(WordBin) * (data->num_of_lines + current_pattern->data->dir.size));
		for (int i = 0; i < current_pattern->data->dir.size; i++)
		{
			buffer = current_pattern->data->dir.data[i];
			if (exist_in_trie(constant, buffer))
			{
				data_element = ((Constant)exist_in_trie(constant, buffer))->value;
			}
			else {
			if (buffer[0] == '-' || buffer[0] == '+')
			{
				buffer++;}
			if (strlen(buffer) != strspn(buffer, "0123456789")){
				printf("error: data element %s is not a number\n", data_element);
				error_flag = 1;	
				continue;
			}
			else
				data_element = atoi(buffer);
			}
			if (data_element > MAX_DATA_VALUE || data_element < MIN_DATA_VALUE)
			{
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
		if(exist_in_trie(entries, current_pattern->label)){
			/* seem to be allowed */
			continue;
		}
		if (exist_in_trie(externals, current_pattern->label))
		{
			printf("error: symbol %s already exist as external\n", current_pattern->label);
			error_flag = 1;
			continue;
		}
		if(Symbol s = (Symbol)exist_in_trie(symbols, current_pattern->label)){
			//Symbol s = (Symbol)exist_in_trie(symbols, current_pattern->label);
			if (s->type == DATA)
			{
				s->type = ENTRY_DATA;
				Entry e = create_entry(current_pattern->label, s);
			}
			else if (s->type == CODE)
			{
				s->type = ENTRY_CODE;
				Entry e = create_entry(current_pattern->label, s);
			}
			else 
			{
				/* seem to be allowed */
				continue;
			}
		}
		else
		{
			Symbol s = create_symbol(current_pattern->label, 0, ENTRY);
			Entry e = create_entry(current_pattern->label, s);
		}	
			
	  case EXTERN:
	  	if (exist_in_trie(entries, current_pattern->label)) {
			printf("error: symbol %s already exist as entry\n", current_pattern->label);
			error_flag = 1;
			continue;
		}
		if(exist_in_trie(externals, current_pattern->label)){
			/* seem to be allowed */
			continue;	  
	  }
		if (symbols s = exist_in_trie(symbols, current_pattern->label))
		{
			if (s->type != EXTERN)
			{
				printf("error: symbol %s already exist as symbol\n", current_pattern->label);
				error_flag = 1;
				continue;
			}
		}
		else
		{
			Symbol s = create_symbol(current_pattern->label, 0, EXTERN);
			External e = create_external(current_pattern->label);
		}
		break;
	  }
	  break;

	case INSTRUCTION:
	  if(current_pattern->label[0])
		{
			Symbol s = (Symbol)exist_in_trie(symbols, current_pattern->label);
			if (s && s->type == ENRTY)
			{
				s->type = ENTRY_CODE;
				s->address = IC;
			}
			else if (!s)
			{
				s = create_symbol(current_pattern->label, IC, CODE);
				if (!s)
				{	error_flag = 1;
					printf("error: fail to add symbol %s\n", current_pattern->label);
				}
			}
			else
			{	error_flag = 1;
				printf("error: symbol %s already exist\n", current_pattern->label);
			}
		}
	  
  }
  
}
  int main() { return 0; }