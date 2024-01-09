#include "front.h"
#include "tables.h"
#include "./trie/trie.h"

struct symbol {
  char *label;
  int address;
  enum {CODE, DATA, EXTERN, ENTRY, ENTRY_CODE, ENTRY_DATA } type;

};

struct external {
  char *label;
  int *addresses;
};

struct entry {
  char *label;
  int address;
};

Trie symbols, externals, entries;
