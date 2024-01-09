#include "trie.h"
#include <stdlib.h>

#define TRIE_BASE_CHAR '.'
#define TRIE_CHAR_COUNT 77

struct trie_node {
    void * end_of_string_pointer;
    TrieNode next[TRIE_CHAR_COUNT];
};

struct trie {
    TrieNode next[TRIE_CHAR_COUNT];
};


static TrieNode exist_in_internal_trie(TrieNode node_i,const char * string) {
    
	while(node_i) {
		
        if(*string == '\0'){
			if (node_i->end_of_string_pointer != NULL) 
            	return node_i;
			else
				return NULL;
        }
        node_i = node_i->next[(*string) - TRIE_BASE_CHAR];
    }
    return NULL;
}


Trie trie() {
    return calloc(1,sizeof(struct trie));
}

const char *insert_to_trie(Trie trie, const char *string, void * end_of_string_pointer) {
    const char * temp = string;
    TrieNode* iterator = &trie->next[(*string) - TRIE_BASE_CHAR];
    while(1) {
        if(*iterator == NULL) {
            (*iterator) = calloc(1,sizeof(struct trie_node));
            if(*iterator == NULL)
                return NULL;
        }
        string++;
        if(*string !='\0')
            iterator = &(*iterator)->next[(*string) - TRIE_BASE_CHAR];
        else
            break;
    }
    (*iterator)->end_of_string_pointer = end_of_string_pointer;
    return temp;
}
void delete_from_trie(Trie trie,const char *string) {
    TrieNode find_node;
    if(string == NULL)
        return;
    find_node = exist_in_internal_trie(trie->next[(*string) - TRIE_BASE_CHAR],string+1);
    if(find_node)
        find_node->end_of_string_pointer = NULL;
}

void * exist_in_trie(Trie trie,const char *string) {
    TrieNode find_node;
    if(string == NULL)
        return NULL;
    find_node = exist_in_internal_trie(trie->next[(*string) - TRIE_BASE_CHAR],string+1);
    return find_node == NULL ? NULL : find_node->end_of_string_pointer;
}

static void trie_destroy_sub(TrieNode node_i) {
    int i;
    for(i=0;i<TRIE_CHAR_COUNT;i++) {
        if(node_i->next[i] != NULL) {
            trie_destroy_sub(node_i->next[i]);
            node_i->next[i] = NULL;
        }
    }
    free(node_i);
}
void destroy_trie(Trie * trie) {
    int i;
    if(*trie != NULL) {
        Trie root = *trie;
        for(i=0;i<TRIE_CHAR_COUNT;i++) {
            if(root->next[i] != NULL) 
                trie_destroy_sub(root->next[i]);
        }
        free(*trie);
        (*trie) = NULL;
    }
}