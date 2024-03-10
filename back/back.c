/**
 * @file back.c
 * @author shlomo weisz
 * @brief  This file contains the implementation of the functions that used to handle the back end of the assembler.
 * the file contains the following functions:
 * 1. to_binary_table - a function that converts the code and data tables to binary table.
 * 2. fromBinary - a function that converts a binary word to a number.
 * 3. convert - a function that converts a binary word to encrypted word.
 * 4. translator - a function that translates the binary table to encrypted words.
 * 5. to_files - a function that writes the binary table to the output files.
 * 6. write_obj - a function that writes the obj file.
 * 7. write_ent - a function that writes the ent file.
 * 8. write_ext - a function that writes the ext file.
 * @version 0.1
 * @date 2024-03-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "../front.h"
#include <stdio.h>
#include "back.h"


/**
 * @brief convert the code and data tables to binary table
 * 
 */
int to_binary_table(int IC, int DC, char ***binary_table) {
  int i, j, k;
  /* allocate memory for the binary table */
  char **result = (char **)calloc(DC + IC, sizeof(char *));
  if (result == NULL) {
    printf("error in allocation memory\n");
    return 1;
  }
  /* loop over the code table and copy the lines to the binary table */
  for (i = 0, k = 0; i < num_of_codes; i++) {
    for (j = 0; j < code[i]->num_of_lines; j++) {
      result[k] = calloc(WIDTH_OF_WORD, sizeof(char));
      if (result[k] == NULL) {
        printf("error in allocation memory\n");
        return 1;
      }
      strcat(result[k], (char *)code[i]->lines[j]);
      k++;
    }
  }
  /* loop over the data table and copy the lines to the binary table */
  for (i = 0; i < data->num_of_lines; i++) {
    result[i + k] = calloc(WIDTH_OF_WORD, sizeof(char));
    if (result[i + k] == NULL) {
      printf("error in allocation memory\n");
      return 1;
    }
    strcpy(result[i + k], (char *)data->lines[i]);
  }
  *binary_table = result;
  return 0;
}

/**
 * @brief This function converts a binary word to a number
 * 
 * @param s The binary word
 * @return int The number
 */
int fromBinary(const char *binary_string) { return (int)strtol(binary_string, NULL, 2); }


/**
 * @brief This function converts a binary word to encrypted word
 * 
 * @param word The binary word
 * @return char* The encrypted word
 */
char *convert(char *word) {
  char *part;
  char *result;
  int i, num;
  /* the signs that represent the binary numbers */
	char signs[4] = {'*', '#', '%', '!'};
  part = calloc(3, sizeof(char));
  result = calloc(8, sizeof(char));
  /* we will go through the word, and convert each 2 bits to a sign */
  for (i = 0; i < 7; i++) {
    part[0] = word[i * 2];
    part[1] = word[i * 2 + 1];
    part[2] = '\0';

	/* convert the part to a number */
    num = fromBinary(part);

    result[i] = signs[num];
  }
  free(part);
  result[7] = '\0';
  return result;
}



/**
 * @brief This function translates the binary table to encrypted words
 * 
 */
void translator(int IC, int DC, char **binary_table, char ***binary_table_translated) {
  int i;
  *binary_table_translated = calloc(IC + DC, sizeof(char *));
  for (i = 0; i < IC + DC; i++) {
    (*binary_table_translated)[i] = convert(binary_table[i]);
  }
}


/**
 * @brief This function writes the binary table to the output files
 * 
 * @param file_name The name of the file
 * @param IC The instruction counter
 * @param DC The data counter
 * @param binary_table_translated The binary table
 */
void to_files(char *file_name, int IC, int DC,  char **binary_table_translated) {
  char *ob_name, *ent_name, *ext_name;

  /* create the names of the output files */
  ob_name = calloc(strlen(file_name) + 5, sizeof(char));
  ent_name = calloc(strlen(file_name) + 5, sizeof(char));
  ext_name = calloc(strlen(file_name) + 5, sizeof(char));
  strcpy(ob_name, file_name);
  strcpy(ent_name, file_name);
  strcpy(ext_name, file_name);
  strcat(ob_name, ".ob");
  strcat(ent_name, ".ent");
  strcat(ext_name, ".ext");

  /* write the obj file */
  write_obj(ob_name,  IC,  DC, binary_table_translated);

  /* write the ent file */
  write_ent(ent_name);
 
  /* write the ext file */
  write_ext(ext_name);
  
  free(ext_name);
  free(ent_name);
  free(ob_name);
}


/**
 * @brief This function writes the obj file
 * 
 * @param obj_name The name of the file
 * @param IC The instruction counter
 * @param DC The data counter
 * @param binary_table_translated The binary table
 */
void write_obj(char *obj_name, int IC, int DC, char **binary_table_translated) {
  int i;
  FILE *obj_file = fopen(obj_name, "w");
  fprintf(obj_file, "   %d %d\n", IC, DC);
  for (i = 0; i < IC + DC; i++) {
    fprintf(obj_file, "%04d  %s\n", i + 100, binary_table_translated[i]);
  }
  fclose(obj_file);
}


/**
 * @brief This function writes the ent file. prints the symbols of the entries and their addresses
 * 
 * @param ent_name The name of the file
 */
void write_ent(char *ent_name){
	if (num_of_entries > 0) {
	int i, address;
    FILE *ent = fopen(ent_name, "w");
    for (i = 0; i < num_of_entries_in_table; i++) {
		address = symbol_table_of_entries[i]->address;
      fprintf(ent, "%s    %04d\n", symbol_table_of_entries[i]->label,
              address);
    }
    fclose(ent);
  }
}


/**
 * @brief This function writes the ext file. prints the symbols of the externals and their addresses
 * 
 * @param ext_name The name of the file
 */
void write_ext(char *ext_name){
	if (num_of_externals > 0) {
	int i, j;			
    FILE *ext = fopen(ext_name, "w");
    for (i = 0; i < num_of_externals_in_table; i++) {
      for (j = 0; j < external_table[i]->number_of_addresses; j++) {
        fprintf(ext, "%s\t%04d\n", external_table[i]->label,
                external_table[i]->addresses[j] + 100);
      }
    }
	fclose(ext);
  }
}


