#include "front.h"
#include "tables.h"

extern char **binary_table;
extern External *external_table;

extern int IC, DC;
char **binary_table_translated;

char signs[4] = {'*', '#', '%', '!'};

int fromBinary(const char *s) { return (int)strtol(s, NULL, 2); }

char *convert(char *word) {
  char *part;
  char *result;
  int i, num;

  part = calloc(3, sizeof(char));
  result = calloc(8, sizeof(char));

  for (i = 0; i < 7; i++) {
    part[0] = word[i * 2];
    part[1] = word[i * 2 + 1];
    part[2] = '\0';

    num = fromBinary(part);

    result[i] = signs[num];
  }

  result[7] = '\0';
  return result;
}

void translator() {
  int i;
  printf("IC: %d\n", IC);
  printf("DC: %d\n", DC);
  printf("convert of 11001100000011: %s\n", convert("11001100000011"));
  binary_table_translated = calloc(IC + DC, sizeof(char *));
  for (i = 0; i < IC + DC; i++) {
    binary_table_translated[i] = calloc(8, sizeof(char));
    strcpy(binary_table_translated[i], convert(binary_table[i]));
    printf("%s\n", binary_table_translated[i]);
  }
}

void to_files(char *file_name) {
  FILE *ob, *ent, *ext;
  int i, j;
  char *ob_name, *ent_name, *ext_name;
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
  fprintf(ob, "%d %d\n", IC, DC);
  for (i = 0; i < IC + DC; i++) {
    fprintf(ob, "%d  %s\n", i + 100, binary_table_translated[i]);
  }
  fclose(ob);

  if (num_of_entries > 0) {
    ent = fopen(ent_name, "w");
    for (i = 0; i < num_of_entries; i++) {
      fprintf(ent, "%s %7d\n", entry_table[i]->symbol->label,
              entry_table[i]->symbol->address);
    }
    fclose(ent);
  }

  if (num_of_externals > 0) {
    ext = fopen(ext_name, "w");
    for (i = 0; i < num_of_externals; i++) {
      for (j = 0; j < external_table[i]->number_of_addresses; j++) {
        fprintf(ext, "%s %7d\n", external_table[i]->label,
                external_table[i]->addresses[j]);
      }
    }
    fclose(ext);
  }
}