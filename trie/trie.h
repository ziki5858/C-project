#ifndef TRIE_H
#define TRIE_H
#define TRIE_BASE_CHAR '.'
#define TRIE_CHAR_COUNT 77


typedef struct trie * Trie;
typedef struct trie_node * TrieNode;

/**
 * @brief  This struct is used to represent a trie node.
 * each node has a pointer to the end of the string, and an array of pointers to the next nodes.
 * 
 */
struct trie_node {
    void * end_of_string_pointer;
    TrieNode next[TRIE_CHAR_COUNT];
};

/**
 * @brief  This struct is used to represent a trie root.
 * 
 */
struct trie {
    TrieNode next[TRIE_CHAR_COUNT];
};


/**
 * @brief This function is used to create a new trie.
 * @return Trie The new trie.
 */
Trie trie();


/**
 * @brief This function is used to insert a string into the trie.
 * @param trie The trie.
 * @param string The string to be inserted.
 * @param end_of_string_pointer The pointer that the end of the string points to.
 * @return const char* The string inserted into the trie.
 */
const char * insert_to_trie(Trie trie,const char *string,void * end_of_string_pointer);


/**
 * @brief This function is used to find a string in the trie.
 * @param trie The trie.
 * @param string The string to be found.
 * @return void* The end of string pointer of the string in the trie.
 */
void * exist_in_trie(Trie trie,const char *string);





/**
 * @brief This function is used to destroy the trie.
 * 
 * @param trie The trie.
 */
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
