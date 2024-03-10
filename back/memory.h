/**
 * @brief This function used to free all the memory that was allocated during the initialization of the memory.
 * 
 */
void free_memory(struct Node *head, int error_flag, int IC, int DC, char **binary_table, char **binary_table_translated);


/**
 * @brief This function used to free all the memory that was allocated during the initialization
 * of the linked list of struct node that contain the struct pattern.
 * 
 * @param head The head of the linked list.
 */
void destroy_linked_list(struct Node *head);


/**
 * @brief This function used to free all the memory that was allocated during the initialization of the code and data tables.
 * 
 * @param DC The data counter
 */
void free_tables_codes(int DC);


/**
 * @brief This function used to free all the memory that was allocated during the initialization of the tries.
 * 
 */
void free_tries();


/**
 * @brief This function used to free all the memory that was allocated during the initialization of the other tables, 
 * such as the symbol table, the entry table, the external table, the binary table and the translated binary table.
 * 
 * @param error_flag The error flag
 * @param IC The instruction counter
 * @param DC The data counter
 * @param binary_table The binary table
 * @param binary_table_translated The translated binary table
 */
void free_other_tables(int error_flag, int IC, int DC, char **binary_table, char **binary_table_translated);


/**
 * @brief This function used to free all the memory that was allocated during the initialization of the extern.
 * 
 * @param extern_i The extern that we want to free.
 */
void destroy_extern(void *extern_i);


/**
 * @brief This function used to free all the memory that was allocated during the initialization of the macro.
 * 
 * @param macro_i The macro that we want to free.
 */
void destroy_macro(void *macro_i);


/**
 * @brief This function used to free all the memory that was allocated during the initialization of the constant.
 * 
 * @param constant_i The constant that we want to free.
 */
void destroy_constant(void *constant_i);