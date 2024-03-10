#include "../trie/trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SKIP_SPACES(line) while(*line == ' ' || *line == '\t' || *line == '\r') line++
#define REMOVE_SPACES_FROM_END(line)                                           \
  while (line[strlen(line) - 1] == ' ' || line[strlen(line) - 1] == '\n' ||    \
         line[strlen(line) - 1] == '\t'|| line[strlen(line) - 1] == '\r') {    \
    line[strlen(line) - 1] = '\0';                                             \
  }
#define SPACES " \t\n\r\f\v"
#define MAX_LINE_LENGTH 81
#define EXT_BEFORE ".as"
#define EXT_AFTER ".am"


/**
 * @brief struct for macro, contains the name of the macro, the value of the macro and the number of lines in the macro
 * 
 */
struct macro {
	char name[32];
	char *value;
	int number_of_lines;
};
typedef struct macro * Macro ;





/**
 * @brief this function determaine which line is it, if it is a macro, a macro definition, a macro end or a normal line
 * 
 * @param line  the line to determaine
 * @return int  the type of the line
 */
int determaine_which_line_is_it(char *line);

/**
 * @brief this function creats a macro 
 * 
 * @param line  the line to creat the macro from
 * @param count  the number of the line in the file
 * @return Macro  the macro that was created
 */
Macro creat_macro(char *line, int count);

/**
 * @brief this function searchs for a macro in the trie
 * 
 * @param line  the line to search the macro in
 * @return Macro  the macro that was found, if not found returns NULL
 */
Macro search_macro(char *line);

/**
 * @brief this function inserts a line to a macro
 * 
 * @param m  the macro to insert the line to
 * @param line  the line to insert
 */
void insert_line_to_macro(Macro macro_to_insert, char *line);

/**
 * @brief this function is the main function of the preprossesor, it opens the file, reads it line by line and determaines which line is it
 * 
 * @param name_of_file  the name of the file to preprosses
 * @return int  0 if the preprosses was successful, 1 if not
 */
int preprocess(char *name_of_file);


/**
 * @brief Check if the word is a saved word
 * 
 * @param word The word to check
 * @return int 1 if saved word, 0 otherwise
 */
int is_saved_word(char *word);


