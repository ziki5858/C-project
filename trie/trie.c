/**
 * @file trie.c
 * @author shlomo weisz
 * @brief This file contains the implementation of the trie data structure. 
 * the file contains the following functions:
 * 1. trie - a function that creates a new trie.
 * 2. insert_to_trie - a function that inserts a string to the trie.
 * 3. exist_in_trie - a function that checks if a string is exist in the trie.
 * 4. destroy_trie - a function that destroys the trie.
 * 5. destroy_trie_with_ptr - a function that destroys the trie, and free the end of string pointer,
 * by using a function pointer that given by the user.
 * @version 0.1
 * @date 2024-03-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "trie.h"
#include <stdlib.h>
#include <stdio.h>



/**
 * @brief  This function is used to check if the string is exist in the sub trie.
 * 
 * @param node_i  The node of the trie that we want to check if the string is exist in it.
 * @param string  The string that we want to check if it is exist in the trie.
 * @return TrieNode  The node that the string is exist in it, or NULL if the string is not exist in the trie.
 */
static TrieNode exist_in_internal_trie(TrieNode node_i,const char * string) {
    
	while(node_i) {
		
        if(*string == '\0'){ /* if the string get to the end */
			if (node_i->end_of_string_pointer != NULL)  /* if the end of string pointer is not NULL, then the string is exist in the trie */
            	return node_i;
			else
				return NULL;
        }
		/* if the string is not get to the end, then we will continue to check the next node */
        node_i = node_i->next[(*string) - TRIE_BASE_CHAR];
		string++;
    }
    return NULL;
}



Trie trie() {
    return calloc(1,sizeof(struct trie));
}



const char * insert_to_trie(Trie trie, const char *string, void * end_of_string_pointer) {
    const char * temp = string;
	/* start from the root of the trie, and insert the string to the trie */
    TrieNode* iterator = &trie->next[(*string) - TRIE_BASE_CHAR];
    while(1) {
        if(*iterator == NULL) { /* if the node is NULL, then we will allocate memory for it */
            (*iterator) = calloc(1,sizeof(struct trie_node));
            if(*iterator == NULL)
                return NULL;
        }
        string++;
        if(*string !='\0') /* if the string is not get to the end, then we will continue to insert the string to the trie */
            iterator = &(*iterator)->next[(*string) - TRIE_BASE_CHAR];
        else
            break;
    }
	/* set the end of string pointer */
    (*iterator)->end_of_string_pointer = end_of_string_pointer;
    return temp;
}




void * exist_in_trie(Trie trie,const char *string) {
    TrieNode find_node;
    if(string == NULL)
        return NULL;
	/* start from the root of the trie, and check if the string is exist in the trie */
    find_node = exist_in_internal_trie(trie->next[(*string) - TRIE_BASE_CHAR],string+1);
    return find_node == NULL ? NULL : find_node->end_of_string_pointer;
}

/**
 * @brief This function is used to destroy the sub trie.
 * 
 * @param node_i The node of the trie.
 */
static void trie_destroy_sub(TrieNode node_i) {
    int i;
	/* destroy the trie recursively */
    for(i=0;i<TRIE_CHAR_COUNT;i++) {
        if(node_i->next[i] != NULL) {
            trie_destroy_sub(node_i->next[i]);
            node_i->next[i] = NULL;
			if (node_i->end_of_string_pointer != NULL)
			{
				free(node_i->end_of_string_pointer);
				node_i->end_of_string_pointer = NULL;
			
			}
        }
    }
    free(node_i);
}


void destroy_trie(Trie * trie) {
    int i;
    if(*trie != NULL) {
        Trie root = *trie;
		/* destroy the trie by using trie_destroy_sub function on each node */
        for(i=0;i<TRIE_CHAR_COUNT;i++) {
            if(root->next[i] != NULL) 
                trie_destroy_sub(root->next[i]);
        }
        free(*trie);
        (*trie) = NULL;
    }
}

/**
 * @brief This function is used to destroy the trie, and free the end of string pointer,
 * by using a function pointer.
 * 
 * @param node_i The node of the trie.
 * @param destroy_ptr The function pointer that is used to free the end of string pointer.
 */
static void trie_destroy_sub_with_ptr(TrieNode node_i, void (*destroy_ptr)(void *)) {
	int i;
	/* destroy the trie recursively */
	for(i=0;i<TRIE_CHAR_COUNT;i++) {
		if(node_i->next[i] != NULL) {
			trie_destroy_sub_with_ptr(node_i->next[i],destroy_ptr);
			node_i->next[i] = NULL;
		}
		if (node_i->end_of_string_pointer != NULL){
			destroy_ptr(node_i->end_of_string_pointer); /* free the end of string pointer by using the function pointer */
			node_i->end_of_string_pointer = NULL;
		}
	}
	free(node_i);
}

/**
 * @brief This function is used to destroy the trie, and free the end of string pointer,
 * by using a function pointer that given by the user.
 * 
 * @param trie The trie.
 * @param destroy_ptr The function pointer that is used to free the end of string pointer.
 */
void destroy_trie_with_ptr(Trie * trie, void (*destroy_ptr)(void *)) {
	int i;
	if(*trie != NULL) {
		Trie root = *trie;
		/* destroy the trie by using trie_destroy_sub_with_ptr function on each node */
		for(i=0;i<TRIE_CHAR_COUNT;i++) {
			if(root->next[i] != NULL) 
				trie_destroy_sub_with_ptr(root->next[i],destroy_ptr);
		}
		free(*trie);
		(*trie) = NULL;
	}
}