#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ERROR_SIZE 300
#define MAX_LINE_SIZE 81
#define MAX_LABEL_SIZE 31

struct pattern;

struct node;

node *create_stracts(char *file_name);
