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

#include "preprossesor.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief check if the line length is bigger than the max length
 *
 * @param line  the line to check
 * @return int  1 if the line is too long, 0 otherwise
 */
int is_line_too_long(char *line) {
  int i;
  for (i = 0; i < MAX_LINE_LENGTH; i++)
    if (line[i] == '\n' || line[i] == '\0') /* the line is not too long	*/
      return 0;

  /* the line is too long	*/
  return 1;
}

int determaine_which_line_is_it(char *line) {
  char *first_space, *first_word;
  char *start_of_macro = "mcr";
  char *end_of_macro = "endmcr";
  int index;
  SKIP_SPACES(line);

  /*
  if the line is empty or a comment line or a line with only spaces, return 1
  */
  if (*line == '\0' || *line == '\n' || *line == ';') {
    return 1;
  }

  /* now we will check if the line is a macro definition, a macro end or a macro
  call first we will check the first word in the line, if it is a macro name, it
  is a macro definition
  */
  first_space = strpbrk(line, SPACES);
  if (first_space != NULL) /*there are spaces */
  {
    /*we will copy the first word in the line to a first_word string*/
    index = first_space - line;
    first_word = malloc(index + 1);
    strncpy(first_word, line, index);
    first_word[index] = '\0';
  } else {
    /* there arn't spaces, so first_word equal to all line */
    first_word = malloc(strlen(line) + 1);
    strcpy(first_word, line);
  }

  /* now we will check if the line is a macro definition	*/
  if (strcmp(first_word, start_of_macro) == 0)
    index = 2;

  /* now we will check if the line is a macro end	*/
  else if (strcmp(first_word, end_of_macro) == 0)
    index = 3;

  /* now we will check if the line is a macro call	*/
  else if (search_macro(first_word) != NULL)
    index = 4;

  /* the line is a normal line	*/
  else
    index = 0;

  /* free the first_word string	*/
  free(first_word);
  return index;
}

Macro creat_macro(char *line, int count) {
  char const *result;
  char *name;
  Macro new_macro;

  /* the first word, must be "mcr" */
  strtok(line, " ");

  /* the second word, the name of the macro */
  name = strtok(NULL, " ");
  SKIP_SPACES(name);
  REMOVE_SPACES_FROM_END(name);
  
  if (is_saved_word(name)) {
    printf("Error: in line %d there is a saved word\n", count);
    return NULL;
  }

  /* alocate memory for macro */
  new_macro = (Macro)calloc(1, sizeof(struct macro));
  if (new_macro == NULL)
    return NULL;

  /* copy the name of the macro to the macro struct */
  strcpy(new_macro->name, name);

  /* alocate memory for the value of the macro */
  new_macro->value = calloc(MAX_LINE_LENGTH, sizeof(char));
  new_macro->number_of_lines = 0; /* the number of lines in the macro */

  /* insert the macro to the trie	*/
  result = insert_to_trie(macro_trie, new_macro->name, (void *)new_macro);
  if (result == NULL)
    return NULL;
  return new_macro;
}

Macro search_macro(char *line) {
  char *first_space;
  int index;

  /* skip the spaces in the line	*/
  SKIP_SPACES(line);
  first_space = strpbrk(line, SPACES); /*looking for spaces*/
  if (first_space != NULL) {
    /*we will cat the spaces*/
    index = first_space - line;
    line[index] = '\0';
  }

  /* search for the macro in the trie	*/
  return (Macro)exist_in_trie(macro_trie, line);
}

void insert_line_to_macro(Macro macro_to_insert, char *line) {
  /* alocate memory for the new line	*/
  char *temp =
      realloc(macro_to_insert->value,
              (macro_to_insert->number_of_lines + 1) * MAX_LINE_LENGTH);
  if (temp == NULL)
    return;
  /* increase the number of lines in the macro	*/
  macro_to_insert->number_of_lines++;
  /* copy the line to the macro	*/
  if (macro_to_insert->value == NULL)
    strcpy(temp, line);
  else
    strcat(temp, line);
  macro_to_insert->value = temp;
}

int preprocess(char *name_of_file) {
  /* pointers to the original file and the preprocessed file */
  FILE *ptr_original;
  FILE *ptr_preprocessed;

  char line[MAX_LINE_LENGTH];
  char *temp = (char *)calloc(MAX_LINE_LENGTH, sizeof(char));
  char *temp_original = temp;
  int count = 0; /* the number of lines in the preprocessed file	*/

  /* a pointer to the macro, and for a macro trie	*/
  Macro m = NULL;
  macro_trie = trie();

  /* make the name of the original and the preprocessed files	*/
  strcpy(temp, name_of_file);
  strcat(temp, EXT_BEFORE);

  ptr_original = fopen(temp, "r");
  if (ptr_original == NULL)
    return 1;

  /* make the name of the preprocessed file	*/
  temp[strlen(temp) - 3] = '\0';
  strcat(temp, EXT_AFTER);
  ptr_preprocessed = fopen(temp, "w");
  if (ptr_preprocessed == NULL)
    return 1;

  temp[0] = '\0'; /* clear the temp string	*/

  /* read the original file line by line	*/
  while (fgets(line, MAX_LINE_LENGTH, ptr_original) != NULL) {
    

    strcpy(temp, line);
    SKIP_SPACES(temp);

    /* determaine which line is it	*/
    switch (determaine_which_line_is_it(temp)) {

    /* empty line or comment line or line with only spaces	*/
    case 1:
      break;

    /* macro definition	*/
    case 2:

      m = creat_macro(temp, count);
      if (m == NULL)
        return 1;
      break;

    /* macro end	*/
    case 3:
      m = NULL;
      break;

    /* macro call	*/
    case 4:
      m = search_macro(temp);
      if (m == NULL)
        return 1;
      fputs(m->value, ptr_preprocessed); /* insert the macro value to the
                                            preprocessed file	*/
      m = NULL;
      break;

    /* normal line	*/
    case 0:
      if (m != NULL) /* if there is a macro, insert the line to the macro
                      */
        insert_line_to_macro(m, temp);
      else /* if there isn't a macro, insert the line to the preprocessed file
            */
        fputs(temp, ptr_preprocessed);
      break;
    }
    temp = temp_original; /* restore the original pointer	*/
    temp[0] = '\0';       /* clear the temp string	*/
    count++;
  }

  /* free the temp string	*/
  free(temp);

  /* close the files	*/
  fclose(ptr_original);
  fclose(ptr_preprocessed);
  return 0;
}
