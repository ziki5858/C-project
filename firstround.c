#include "./trie/trie.h"
#include "front.h"
#include "tables.h"
#define WIDTH_OF_WORD 15

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
  char *label;
  int address;
};

struct word_bin {
	char word[WIDTH_OF_WORD];
};

struct code {
	int num_of_lines;
	WordBin *lines;
};

Trie symbols, constant, externals, entries ;
int IC = 100, DC = 0;
Code *code, *data;

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

External create_external(char *label, int address) {
  External e = (External)malloc(sizeof(struct external));
  e->label = label;
  e->addresses = (int *)malloc(sizeof(int));
  e->addresses[0] = address;
  e->number_of_addresses = 1;
  if (insert_to_trie(externals, label, e))
    {
	  external_table[num_of_externals++] = e;
	  return e;
	}
	
  return NULL;
}


Entry create_entry(char *label, int address) {
  Entry e = (Entry)malloc(sizeof(struct entry));
  e->label = label;
  e->address = address;
  if(insert_to_trie(entries, label, e))
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






void first_round(FILE *fp) {
  symbols = trie();
  externals = trie();
  entries = trie();
  constant = trie();
  
  extern int num_of_patterns;
  extern int num_of_symbols;
  extern int num_of_entries;
  extern int num_of_externals;
  
  
  code = (Code *)calloc(num_of_patterns,sizeof(Code));
  data = (Code *)calloc(num_of_patterns,sizeof(Code));

  symbol_table = (Symbol *)calloc(num_of_symbols,sizeof(Symbol));
  entry_table = (Entry *)calloc(num_of_entries,sizeof(Entry));
  external_table = (External *)calloc(num_of_externals,sizeof(External));
  
  
  
  
  int error_flag = 0;
}
  int main() { return 0; }