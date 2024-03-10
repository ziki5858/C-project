/**
 * @brief The first round of the assembler.
 * In this round we will go through the linked list and handle the labels, the directives and the instructions.
 * after this round we will have all the symbols and we can encode the instructions of direct operands
 *
 * @param head The head of the linked list
 */
void first_round(struct Node *head, int *error_flag, int *IC, int *DC);


/**
 * @brief The second round of the assembler.
 * now we have all the symbols and we can encode the instructions of direct operands
 * 
 * @param head The head of the linked list
 */
void secondround(struct Node *head, int *error_flag);