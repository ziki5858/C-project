#ifndef TRIE_H
#define TRIE_H

typedef struct trie * Trie;
typedef struct trie_node * TrieNode;


Trie trie();


const char * insert_to_trie(Trie trie,const char *string,void * end_of_string_pointer);


void * exist_in_trie(Trie trie,const char *string);


void delete_from_trie(Trie trie,const char  *string);


void destroy_trie(Trie * trie);








#endif
