#include "./trie/trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SKIP_SPACES(line) while(*line == ' ' || *line == '\t') line++
#define MAX_LINE_LENGTH 81
#define EXT_BEFORE ".as"
#define EXT_AFTER ".am"

typedef struct macro * Macro ;

struct macro {
	char name[32];
	char *value;
	int number_of_lines;
};

Trie macro_trie;

int determaine_which_line_is_it(char *line);
Macro creat_macro(char *line);
Macro search_macro(char *line);
void insert_line_to_macro(Macro m, char *line);
int preprocess(char *name_of_file);





int main(){
	int i = 0;
	i = preprocess("code");
	printf("%d\n", i);

	
	return 0;
}
int determaine_which_line_is_it(char *line){
	char *copy_line = malloc(strlen(line) + 1);
	char *temp;
	char *start_of_macro = "mcr";
	char *end_of_macro = "endmcr";
	strcpy(copy_line, line);
	SKIP_SPACES(copy_line);
		printf("%s\n", copy_line);

	if (*copy_line == '\0' || *copy_line == '\n' || *copy_line == ';')
		return 1;
	temp = strtok(copy_line, " ");
	if (strcmp(temp , start_of_macro) == 0)
		return 2;
	else if (strcmp(temp, end_of_macro)== 0){
		printf("end\n");
		return 3;}
	else if (search_macro(temp) != NULL)
		return 4;
	else
		return 0;
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
	if (m) printf("found\n");
	return m;
}

void insert_line_to_macro(Macro m, char *line){
	char *temp = realloc(m->value, (m->number_of_lines + 1) * MAX_LINE_LENGTH);
	if (temp == NULL)
		return;
	m->number_of_lines++;
	if (m->value == NULL)
		strcpy(temp, line);
	else
		strcat(temp, line);
	m->value = temp;
}



int preprocess(char *name_of_file){
	FILE *ptr_original;
	FILE *ptr_preprocessed;
	char line[MAX_LINE_LENGTH];
	char *temp;
	Macro m;
	macro_trie = trie();

	temp = malloc(strlen(name_of_file) * sizeof(char) + 4);
	strcpy(temp, name_of_file);
	strcat(temp, EXT_BEFORE);
	ptr_original = fopen(temp, "r");
	if (ptr_original == NULL)
		return 0;
	temp[strlen(temp) - 3] = '\0';
	strcat(temp, EXT_AFTER);
	ptr_preprocessed = fopen(temp, "w");

	while (fgets(line, MAX_LINE_LENGTH, ptr_original) != NULL){
		switch (determaine_which_line_is_it(line)){
		case 1:
			break;
		case 2:
			m = creat_macro(line);
			if (m == NULL)
				return 1;
			break;
		case 3:
			printf("endm\n");
			m = NULL;
			break;
		case 4:
			m = search_macro(line);
			fputs(m->value, ptr_preprocessed);
			break;
		case 0:
			if (m != NULL)
				insert_line_to_macro(m, line);
			else
				fputs(line, ptr_preprocessed);
			break;
		}

	}

	
	
}

