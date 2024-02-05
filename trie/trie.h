#ifndef TRIE_H
#define TRIE_H

typedef struct trie * Trie;
typedef struct trie_node * TrieNode;


Trie trie();


const char * insert_to_trie(Trie trie,const char *string,void * end_of_string_pointer);


void * exist_in_trie(Trie trie,const char *string);


void delete_from_trie(Trie trie,const char  *string);


void destroy_trie(Trie * trie);
/**
 * @brief This function is used to destroy the trie, and free the end of string pointer,
 * by using a function pointer that given by the user.
 * 
 * @param trie The trie.
 * @param destroy_ptr The function pointer that is used to free the end of string pointer.
 */
void destroy_trie_with_ptr(Trie * trie, void (*destroy_ptr)(void *));







#endif
