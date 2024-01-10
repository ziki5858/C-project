/**
 * @file preprossesor.c
 * @author shlomo-weisz
 * @brief this program is a preprossesor for the assembler
 * @version 0.1
 * @date 2024-01-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */



#include "./trie/trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SKIP_SPACES(line) while(*line == ' ' || *line == '\t') line++
#define SPACES " \t\n\r\f\v"
#define MAX_LINE_LENGTH 81
#define EXT_BEFORE ".as"
#define EXT_AFTER ".am"

typedef struct macro * Macro ;


/**
 * @brief struct for macro, contains the name of the macro, the value of the macro and the number of lines in the macro
 * 
 */
struct macro {
	char name[32];
	char *value;
	int number_of_lines;
};

/**
 * @brief trie for the macros
 * 
 */
Trie macro_trie;

/**
 * @brief this function determaine which line is it, if it is a macro, a macro definition, a macro end or a normal line
 * 
 * @param line 
 * @return int 
 */
int determaine_which_line_is_it(char *line);

/**
 * @brief this function creats a macro
 * 
 * @param line 
 * @return Macro 
 */
Macro creat_macro(char *line);

/**
 * @brief this function searchs for a macro in the trie
 * 
 * @param line 
 * @return Macro 
 */
Macro search_macro(char *line);

/**
 * @brief this function inserts a line to a macro
 * 
 * @param m 
 * @param line 
 */
void insert_line_to_macro(Macro m, char *line);

/**
 * @brief this function is the main function of the preprossesor, it opens the file, reads it line by line and determaines which line is it
 * 
 * @param name_of_file 
 * @return int 
 */
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
	int index; 
	strcpy(copy_line, line);
	SKIP_SPACES(copy_line);


	/*
	if the line is empty or a comment line or a line with only spaces, return 1
	*/
	if (*copy_line == '\0' || *copy_line == '\n' || *copy_line == ';')
		return 1;
	
	
	/* now we will check if the line is a macro definition, a macro end or a macro call
	first we will check the first word in the line, if it is a macro name, it is a macro definition
	*/
	temp = strpbrk(copy_line, SPACES);
	if (temp != NULL) /*there are spaces */
	{
		/*we will copy the first word in the line to a temp string*/
		index = temp - copy_line;
		temp = malloc(index + 1);
		strncpy(temp, copy_line, index);
		temp[index] = '\0';
	}
	else {
		/* there arn't spaces, so temp equal to all line */
		temp = malloc(strlen(copy_line) + 1);
		strcpy(temp, copy_line);
	}
	free(copy_line);
		
	/* now we will check if the line is a macro definition	*/
	if (strcmp(temp , start_of_macro) == 0)
		index = 2;

	/* now we will check if the line is a macro end	*/
	else if (strcmp(temp, end_of_macro)== 0)
		index = 3;

	/* now we will check if the line is a macro call	*/
	else if (search_macro(temp) != NULL)
		index = 4;

	/* the line is a normal line	*/
	else
		index = 0;

	/* free the temp string	*/
	free(temp);
	return index;
}



Macro creat_macro(char *line){
	char const *result;
	char *def = strtok(line, " "); /* the first word */
	char *name = strtok(NULL, " "); /* the second word */
	Macro m = (Macro)malloc(sizeof(struct macro)); /* alocate memory for macro */
	if (m == NULL)
		return NULL;
	strcpy(m->name, name); /* copy the name of the macro to the macro struct */
	m->value = calloc(MAX_LINE_LENGTH, sizeof(char)); /* alocate memory of one line for the value of the macro */
	
	/* insert the macro to the trie	*/
	result = insert_to_trie(macro_trie, m->name, (void*)m);
	if (result == NULL)
		return NULL;
	return m;

}


Macro search_macro(char *line){
	Macro m;
	char *temp;
	int index;
	temp = strpbrk(line, SPACES); /*looking for spaces*/
	if (temp != NULL)
	{
		/*we will copy the first word in the line to a temp string*/
		index = temp - line;
		temp = malloc(index + 1);
		strncpy(temp, line, index);
		temp[index] = '\0';
	}
	else {
		/* there arn't spaces, so temp equal to all line */
		temp = malloc(strlen(line) + 1);
		strcpy(temp, line);
	}

	/* search for the macro in the trie	*/
	m = (Macro)exist_in_trie(macro_trie, temp);
	/* free the temp string	*/
	free(temp);
	return m;
}



void insert_line_to_macro(Macro m, char *line){
	/* alocate memory for the new line	*/
	char *temp = realloc(m->value, (m->number_of_lines + 1) * MAX_LINE_LENGTH);
	if (temp == NULL)
		return;
	/* increase the number of lines in the macro	*/
	m->number_of_lines++;
	/* copy the line to the macro	*/
	if (m->value == NULL)
		strcpy(temp, line);
	else
		strcat(temp, line);
	m->value = temp;
	
}



int preprocess(char *name_of_file){
	/* pointers to the original file and the preprocessed file */
	FILE *ptr_original;
	FILE *ptr_preprocessed;

	char line[MAX_LINE_LENGTH];
	char *temp;
	char *temp_original;

	/* a pointer to the macro, ant for a macro trie	*/
	Macro m = NULL;
	macro_trie = trie();

	/* make the name of the original and the preprocessed files	*/
	temp = malloc(strlen(name_of_file) * sizeof(char) + 4);
	strcpy(temp, name_of_file);
	strcat(temp, EXT_BEFORE);
	ptr_original = fopen(temp, "r");
	if (ptr_original == NULL)
		return 1;
	/* make the name of the preprocessed file	*/
	temp[strlen(temp) - 3] = '\0';
	strcat(temp, EXT_AFTER);
	ptr_preprocessed = fopen(temp, "w");

	/* free the temp string	*/
	free(temp);

	/* read the original file line by line	*/
	while (fgets(line, MAX_LINE_LENGTH, ptr_original) != NULL){
		temp = malloc(strlen(line) + 1); /* alocate memory for the line	*/
		temp_original = temp; /* save the original pointer to be free later	*/
		strcpy(temp, line);
		SKIP_SPACES(temp);

		/* determaine which line is it	*/
		switch (determaine_which_line_is_it(temp)){
		case 1: /* empty line or comment line or line with only spaces	*/
			break;
		case 2: /* macro definition	*/
			m = creat_macro(temp);
			if (m == NULL)
				return 1;
			break;
		case 3: /* macro end	*/
			m = NULL;
			break;
		case 4: /* macro call	*/
			m = search_macro(temp);
			if (m == NULL)
				return 1;
			fputs(m->value, ptr_preprocessed); /* insert the macro value to the preprocessed file	*/
			m = NULL;
			break;
		case 0: /* normal line	*/
			if (m != NULL) /* if there is a macro, insert the line to the macro	*/
				insert_line_to_macro(m, temp);
			else /* if there isn't a macro, insert the line to the preprocessed file	*/
				fputs(temp, ptr_preprocessed);
			break;
		}
		free(temp_original);
	}

	
	return 0;
}

