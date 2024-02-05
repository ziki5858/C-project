/**
 * @file assembler.c
 * @author Shlomo Weisz (shlomowaize@gmail.com)
 * @brief This program is an assembler for the assembly language that was given in the course.
 * @version 0.1
 * @date 2024-01-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */




#include "front.h"

/**
 * @brief This function is the main function of the assembler, it calls all the other functions in the program.
 * 
 * @param file_name The name of the file that we want to assemble.
 */
void assembler(char *file_name){
	int fail;
	struct Node *head;
	char *file_name_with_extension;

	/* preprocess the file */
	fail = preprocess(file_name);
	if (fail)
	{
		printf("error: preprocess failed\n");
		return;
	
	}
	/* make the file name with the extension .am */
	file_name_with_extension = calloc(strlen(file_name) + 3, sizeof(char));
	strcpy(file_name_with_extension, file_name);
	strcat(file_name_with_extension, ".am");

	/* Goes through the file and parses the syntax into a linked list of nodes */
	head = processAssemblyText(file_name_with_extension);
    if(head == NULL){
		printf("error: processAssemblyText failed\n");
		return;
	}

	/* first round of the assembler */
	first_round(head);
	/* second round of the assembler */
	secondround(head);
	
	/* if there are no errors in the file, translate the binary table to the output files */
	if (error_flag == 0){
		to_binary_table();
		translator();
		to_files(file_name);
	}
	else{
		printf("error: there are errors in the file\n"); {
	
	}
}}

/**
 * @brief This is the main function of the program, it calls the assembler function for each file that was given as an argument.
 * 
 * @param argc The number of files that were given as arguments.
 * @param argv The names of the files that were given as arguments.
 * @return int 
 */
int main(int argc, char *argv[]){
	int i;

	if (argc < 2) /* no files to assemble */
	{
		printf("error: no files to assemble\n");
		return 0;
	}

	/* for each file, call the assembler function */
	for (i = 1; i < argc; i++)
	{
		assembler(argv[i]);
	}
	return 0;
}