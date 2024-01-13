#include "tables.h"
extern char ** binary_table;
extern External *external_table;

extern int IC, DC;
char **binary_table_translated;

char signs[4] = {'*', '#', '%', '!'};


char * convert(char * word){
	char * part;
	char * result;
	int i, num;

	part = calloc(3, sizeof(char));
	result = calloc(WIDTH_OF_WORD, sizeof(char));

	for (i = 0; i < 7; i++){
		part[0] = word[i * 2];
		part[1] = word[i * 2 + 1];
		part[2] = '\0';

		num = atoi(part);

		result[i] = signs[num];
	}
	result[WIDTH_OF_WORD - 1] = '\0';
	return result;

}


void translator(){
	int i;
	for (i = 0; i < IC + DC; i++) {
		binary_table_translated[i] = convert(binary_table[i]);
	}
}

void to_files(char * file_name){
	FILE * ob, * ent, * ext;
	int i,j;
	char * ob_name, * ent_name, * ext_name;
	ob_name = calloc(strlen(file_name) + 5, sizeof(char));
	ent_name = calloc(strlen(file_name) + 5, sizeof(char));
	ext_name = calloc(strlen(file_name) + 5, sizeof(char));
	strcpy(ob_name, file_name);
	strcpy(ent_name, file_name);
	strcpy(ext_name, file_name);
	strcat(ob_name, ".ob");
	strcat(ent_name, ".ent");
	strcat(ext_name, ".ext");

	ob = fopen(ob_name, "w");
	fputs("%d %d\n", ob, IC, DC);
	for (i = 0; i < IC + DC; i++){
		fputs("%7d %s\n", ob, i + 100, binary_table_translated[i]);
	}
	fclose(ob);

	if (num_of_entries > 0){
		ent = fopen(ent_name, "w");
		for (i = 0; i < num_of_entries; i++){
			fputs("%s %7d\n", ent, entry_table[i]->symbol->label, entry_table[i]->symbol->address);
		}
		fclose(ent);
	}

	if (num_of_externals > 0){
		ext = fopen(ext_name, "w");
		for (i = 0; i < num_of_externals; i++){
			for (j=0; j<externals_table[i]->number_of_addresses; j++){
				fputs("%s %7d\n", ext, externals_table[i]->symbol->label, externals_table[i]->addresses[j]);
			}
		fclose(ext);
	}

	
}