#include 'front.h'
#include 'tables.h'
#define WIDTH_OF_WORD 15

extern Code code;
extern Code data;
extern Trie symbols;
extern int IC;
extern int DC;
extern int error_flag;

int words = IC + DC;

void secondround(struct node * head)
	while (head) {
		if (head->data->type_line == INSTRUCTION)
		{
			if (head->data->inst.num_of_operands >=1)
			{
				if (head->data->inst.operands[0].op_type == DIRECT)
				{
					Symbol s = (Symbol)exist_in_trie(symbols, head->data->inst.operands[0].operand_value.symbol_const);
					if (s)
					{
						int type_ARE = (s->type == EXTERN) ? 1 : 2;
						
					}
			}
		}



		head = head->next;
	}