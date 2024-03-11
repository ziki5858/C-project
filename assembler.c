/**
 * @file assembler.c
 * @author Shlomo Weisz (shlomowaize@gmail.com)
 * @brief This program is an assembler for the assembly language that was given
 * in the course. The program gets a file name as an argument and assembles the file to a binary file.
 * The program can get multiple files as arguments and assemble them all.
 * the process of the assembler is as follows:
 * 1. preprocess the file
 * 2. parse the file into a linked list of nodes
 * 3. first round of the assembler
 * 4. second round of the assembler
 * 5. translate the binary table to the output files
 * The program is written in C language.
 * The program have the following functions:
 * 1. assembler - the main function of the assembler 
 * @version 0.1
 * @date 2024-01-21
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "mainHeader.h"
#include "./end/print.h"
#include "./end/memory.h"
#include "./assemblerRounds/headers/rounds.h"
#include "./parse/headeMethods.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief This function is the main function of the assembler, it calls all the
 * other functions in the program.
 *
 * @param file_name The name of the file that we want to assemble.
 */
void assembler(char *file_name) {
  int fail, error_flag = 0, IC = 0, DC = 0, preprocess(char *file_name);
  struct Node *head;
  char *file_name_with_extension, **binary_table, **binary_table_translated;
;
  file_name_with_extension = calloc(strlen(file_name) + 4, sizeof(char));
  strcpy(file_name_with_extension, file_name);
  strcat(file_name_with_extension, ".as");

  /* check if the file exsists */
  if (access(file_name_with_extension, F_OK) == -1) {
	printf("error: file does not exist\n");
	return;
  }
  /* preprocess the file */
  fail = preprocess(file_name);
  if (fail) {
    printf("error: preprocessor failed\n");
    return;
  }
  /* make the file name with the extension .am */
  file_name_with_extension[0] = '\0';
  strcpy(file_name_with_extension, file_name);
  strcat(file_name_with_extension, ".am");

  /* Goes through the file and parses the syntax into a linked list of nodes */
  head = processAssemblyText(file_name_with_extension);
  free(file_name_with_extension);
  if (head == NULL) {
    printf("error: processAssemblyText failed\n");
    return;
  }

  /* first round of the assembler */
  first_round(head, &error_flag, &IC, &DC);
  /* second round of the assembler */
  secondround(head, &error_flag);

  /* if there are no errors in the file, translate the binary table to the
   * output files */
  if (error_flag == 0 && !(to_binary_table(IC, DC, &binary_table))) {
    
    translator(IC, DC, binary_table, &binary_table_translated);
    to_files(file_name, IC, DC, binary_table_translated);
  } else {
    printf("error: there are errors in the file or int process. no file created!\n");
    
  }
  free_memory(head, error_flag, IC, DC, binary_table, binary_table_translated);
}

/**
 * @brief This is the main function of the program, it calls the assembler
 * function for each file that was given as an argument.
 *
 * @param argc The number of files that were given as arguments.
 * @param argv The names of the files that were given as arguments.
 * @return int
 */
int main(int argc, char *argv[]) {
  int i;

  if (argc < 2) /* no files to assemble */
  {
    printf("error: no files to assemble\n");
    return 0;
  }

  /* for each file, call the assembler function */
  for (i = 1; i < argc; i++) {
    assembler(argv[i]);
  }
  return 0;
}