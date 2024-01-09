#include "./trie/trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SKIP_SPACES(line) while(*line == ' ' || *line == '\t') line++
#define MAX_LINE_LENGTH 80
#define NUMBER_OF_SAVED_WORDS 4

typedef struct macro *Macro ;

struct macro {
	char name[32];
	char *value;
};

Trie macro_trie;




int determaine_which_line_is_it(char *line){
	char *copy_line = malloc(strlen(line) + 1);
	char *temp;
	char *start_of_macro = "mcr";
	char *end_of_macro = "endmcr";
	strcpy(copy_line, line);
	SKIP_SPACES(line);
	if (*line == '\0' || *line == '\n')
		return 1;
	temp = strtok(copy_line, " ");
	if (strcmp(temp , start_of_macro) == 0)
		return 2;
	else if (strcmp(temp, end_of_macro)== 0)
		return 3;
	else
		return 4;
}



Macro creat_macro(char *line){
	char *def = strtok(line, " ");
	char *name = strtok(NULL, " ");
	Macro m = malloc(sizeof(struct macro));
	if (m == NULL)
		return NULL;
	strcpy(m->name, name);
	insert_to_trie(macro_trie, m->name, m);
	return m;

}


Macro search_macro(char *line){
	Macro m;
	m = exist_in_trie(macro_trie, line);
	return m;
}

void insert_line_to_macro(Macro m, char *line){
	char *temp = realloc(m->value, m->value == NULL ? strlen(line) + 1 : strlen(m->value) + strlen(line) + 1);
	if (temp == NULL)
		return;
	if (m->value == NULL)
		strcpy(temp, line);
	else
		strcat(temp, line);
	m->value = temp;
}



void preprocess(char *line){
	
	
}


int main(){
	

	macro_trie = trie();
	return 0;
}