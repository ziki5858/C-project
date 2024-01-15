#include "./trie/trie.h"

// Trie symbols, externals, entries;
// Trie macro_trie;
struct symbol;
struct constant;
struct external;
struct entry;
struct code;
struct word_bin;

typedef struct symbol *Symbol;
typedef struct constant *Constant;
typedef struct external *External;
typedef struct entry *Entry;
typedef struct code *Code;
typedef struct word_bin *WordBin;
typedef struct pattern *Pattern ;

char *toBinaryString(int n, int num_bits);
void insert_address_to_external(External e, int address);