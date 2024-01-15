#include "front.h"


void assembler(char *file_name, struct node * head){
	first_round(head);
	secondround(head);
	to_binary_table();
	if (error_flag == 0){
		translator();
		to_files(file_name);
	}
	else{
		printf("error: there are errors in the file\n"); {
	
	}
}}