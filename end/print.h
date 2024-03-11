extern int num_of_codes;
extern int num_of_entries_in_table;
extern int num_of_externals_in_table;
/**
 * @brief convert the code and data tables to binary table
 * 
 */
int to_binary_table(int IC, int DC, char ***binary_table);


/**
 * @brief This function converts a binary word to a number
 * 
 * @param s The binary word
 * @return int The number
 */
int fromBinary(const char *binary_string);


/**
 * @brief This function converts a binary word to encrypted word
 * 
 * @param word The binary word
 * @return char* The encrypted word
 */
char *convert(char *word);


/**
 * @brief This function translates the binary table to encrypted words
 * 
 */
void translator(int IC, int DC, char **binary_table, char ***binary_table_translated);


/**
 * @brief This function writes the obj file
 * 
 * @param obj_name The name of the file
 * @param IC The instruction counter
 * @param DC The data counter
 * @param binary_table_translated The binary table
 */
void write_obj(char *obj_name, int IC, int DC, char **binary_table_translated);


/**
 * @brief This function writes the ent file. prints the symbols of the entries and their addresses
 * 
 * @param ent_name The name of the file
 */
void write_ent(char *ent_name);



/**
 * @brief This function writes the ext file. prints the symbols of the externals and their addresses
 * 
 * @param ext_name The name of the file
 */
void write_ext(char *ext_name);



/**
 * @brief This function writes the binary table to the output files
 * 
 * @param file_name The name of the file
 * @param IC The instruction counter
 * @param DC The data counter
 * @param binary_table_translated The binary table
 */
void to_files(char *file_name, int IC, int DC,  char **binary_table_translated);

