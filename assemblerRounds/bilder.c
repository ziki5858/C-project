#include "bilder.h"

/**
 * @brief  This function is used to handle the case of data definition.
 * it will insert the data to the data table.
 *
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param DC  The data counter.
 */
void data_def(struct Node *current_pattern, int current_pattern_num, int *DC) {
  int sign = 1, i, data_element;
  char *buffer, *temp_data;
  /* check if there is a label */
  if (current_pattern->data->label[0]) {
    handle_lable_define(current_pattern->data, "data", *DC,
                        current_pattern_num);
  }
  /* insert the data to the data table */
  allocate_memory_for_data(current_pattern->data, current_pattern_num);
  for (i = 0; i < current_pattern->data->choice.dir.size; i++) {
    buffer = current_pattern->data->choice.dir.data[i];

    if (isalpha(buffer[0]) &&
        exist_in_trie(constant, buffer)) { /* if the data is a constant */
      data_element = ((Constant)exist_in_trie(constant, buffer))->value;
    } else {                                      /* if the data is a number */
      if (buffer[0] == '-' || buffer[0] == '+') { /* ignore the sign */
        sign = determaine_sign(buffer);           /* determaine the sign */
        buffer++;
      }
      if (!containsOnlyDigits(&buffer[0])) { /* check if the data is a number */
        print_error_msg("data element is not a number", current_pattern_num);
        continue;
      } else
        data_element = atoi(buffer) * sign;
    }
    if (data_element > MAX_DATA_VALUE || data_element < MIN_DATA_VALUE) {
      print_error_msg("data element is out of range", current_pattern_num);
      continue;
    }
    /* insert the data to the data table */
    temp_data = encoded_data(data_element);
    strcpy(data->lines[data->num_of_lines + i]->word, temp_data); /* -> */
    free(temp_data);
  }
  data->num_of_lines += current_pattern->data->choice.dir.size;
  (*DC) += current_pattern->data->choice.dir.size;
}

/**
 * @brief  This function is used to handle the case of string definition.
 * it will insert the data to the data table.
 *
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param DC  The data counter.
 */
void string_def(struct Node *current_pattern, int current_pattern_num,
                int *DC) {
  int i, char_index;
  char *temp_data;
  if (current_pattern->data->label[0]) {
    handle_lable_define(current_pattern->data, "data", *DC,
                        current_pattern_num);
  }
  /* insert the data to the data table */
  allocate_memory_for_data(current_pattern->data, current_pattern_num);
  for (i = 0; i < current_pattern->data->choice.dir.size; i++) {
    char_index = current_pattern->data->choice.dir.string[i];
    temp_data = encoded_data(char_index);
    strcpy(data->lines[data->num_of_lines + i]->word, temp_data);
    free(temp_data);
  }
  data->num_of_lines += current_pattern->data->choice.dir.size;
  (*DC) += current_pattern->data->choice.dir.size;
}

/**
 * @brief  This function is used to handle the case of entry definition.
 * it will insert the entry to the entry table.
 *
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 */
void entry_def(struct Node *current_pattern, int current_pattern_num) {
  Symbol s;
  if (exist_in_trie(entries, current_pattern->data->label)) {
    /* seem to be allowed */
    return;
  }
  if (exist_in_trie(externals, current_pattern->data->label)) {
    print_error_msg("symbol already exist as external", current_pattern_num);
  }
  if ((s = (Symbol)exist_in_trie(symbols, current_pattern->data->label))) {

    if ((int)s->type == (int)DATA || (int)s->type == (int)CODE) {
      s->type = ((int)s->type == (int)DATA) ? ENTRY_DATA : ENTRY_CODE;
      symbol_table_of_entries[num_of_symbols_in_entries++] = s;
      create_entry(s, current_pattern_num);
    } else {
      /* seem to be allowed */
      return;
    }
  } else { /* symbol not found */
    s = create_symbol(current_pattern->data->label, 0, S_ENTRY,
                      current_pattern_num);
    create_entry(s, current_pattern_num);
  }
}

/**
 * @brief  This function is used to handle the case of extern definition.
 * it will insert the extern to the extern table.
 *
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 */
void extern_def(struct Node *current_pattern, int current_pattern_num) {
  Symbol s;
  if (exist_in_trie(entries, current_pattern->data->label)) {
    print_error_msg("symbol already exist as entry", current_pattern_num);
    return;
  }
  if (exist_in_trie(externals, current_pattern->data->label)) {
    /* seem to be allowed */
    return;
  }
  if ((s = exist_in_trie(symbols, current_pattern->data->label))) {
    print_error_msg("symbol already exist as symbol", current_pattern_num);
    return;

  } else {
    s = create_symbol(current_pattern->data->label, 0, S_EXTERN,
                      current_pattern_num);
    create_external(current_pattern->data->label, current_pattern_num);
  }
}

/**
 * @brief  This function is used to handle the case of directive.
 * it will call the right function to handle the directive.
 *
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param DC  The data counter.
 */
void directive(struct Node *current_pattern, int current_pattern_num, int *DC) {

  /* check the type of the directive */
  switch (current_pattern->data->choice.dir.directive_type) {

  /* dealing with data directive */
  case DATA:
    data_def(current_pattern, current_pattern_num, DC);
    break;
  case STRING:
    string_def(current_pattern, current_pattern_num, DC);
    break;

  case ENTRY:
    entry_def(current_pattern, current_pattern_num);
    break;
  case EXTERN:
    extern_def(current_pattern, current_pattern_num);
    break;
  }
}

/**
 * @brief  This function is used to handle the case of define.
 *
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 */
void define(struct Node *current_pattern, int current_pattern_num) {
  if (exist_in_trie(symbols, current_pattern->data->label)) {
    print_error_msg("symbol already exist as symbol", current_pattern_num);
    return;
  }
  if (exist_in_trie(constant, current_pattern->data->label)) {
    print_error_msg("symbol already exist as constant", current_pattern_num);
    return;
  }
  create_constant(current_pattern->data->label,
                  current_pattern->data->choice.def.value, current_pattern_num);
}


/**
 * @brief  This function is used to handle the case of tow registers.
 * it will encode the tow registers, and insert them to the code table.
 * 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 */
void tow_registers(struct Node *current_pattern, int current_pattern_num, int *num_of_word_nedded) {
  char *temp_ins;
  (*num_of_word_nedded) += 1;

  temp_ins = encoded_registers(
      current_pattern->data->choice.inst.operands[1].operand_value.reg,
      current_pattern->data->choice.inst.operands[0].operand_value.reg);
  strcpy(code[num_of_codes]->lines[1]->word, temp_ins);
  free(temp_ins);
}


/**
 * @brief  This function is used to handle the case of register.
 * it will encode the register, based on the type of the operand. 
 * @param current_pattern  The current pattern that we want to handle.
 * @param operand_num  The number of the operand.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 */
void register_encoder(struct Node *current_pattern, int operand_num, int *num_of_word_nedded) {
  char *temp_ins;
  (*num_of_word_nedded) += 1;
  /* encode the register, based on the num of the operand */
  if (operand_num == 1) {
	temp_ins = encoded_registers(current_pattern->data->choice.inst.operands[1].operand_value.reg,0);
    strcpy(code[num_of_codes]->lines[1]->word, temp_ins);
  } else {
	temp_ins = encoded_registers(0, current_pattern->data->choice.inst.operands[0].operand_value.reg);
    strcpy(code[num_of_codes]->lines[(*num_of_word_nedded) - 1]->word, temp_ins);
  }
  free(temp_ins);
}


/**
 * @brief  This function is used to handle the case of direct operand.
 * it will encode the direct operand, based on the type of the operand. 
 * @param current_pattern  The current pattern that we want to handle.
 * @param operand_num  The number of the operand.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 */
void direct(struct Node *current_pattern, int operand_num, int *num_of_word_nedded) {
  char *temp_ins;
  (*num_of_word_nedded) += 1;
  temp_ins = encoded_data(0); /*will encoded next round*/
  if (operand_num == 1) {
    strcpy(code[num_of_codes]->lines[1]->word, temp_ins); 
  } else {
    strcpy(code[num_of_codes]->lines[(*num_of_word_nedded) - 1]->word, temp_ins);
  }
  free(temp_ins);
}


/**
 * @brief  This function is used to handle the case of direct index operand.
 * it will encode the direct index operand, based on the type of the operand. 
 * @param current_pattern  The current pattern that we want to handle.
 * @param operand_num  The number of the operand.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 * @param current_pattern_num  The number of the current pattern.
 */
void direct_index(struct Node *current_pattern, int operand_num, int *num_of_word_nedded, int current_pattern_num) {
    char *first_word, *second_word;
	(*num_of_word_nedded) += 2;
    first_word = encoded_data(0); /*will encoded next round*/
	/* the second word is the immidiate number */
	second_word = encoded_immidiate_number(extract_immidiate_number(current_pattern->data, operand_num, current_pattern_num));
    
	/* insert the words to the code table, based on the num of the operand */
	if (operand_num == 1) {
	  strcpy(code[num_of_codes]->lines[1]->word, first_word);
	  strcpy(code[num_of_codes]->lines[2]->word, second_word);
	} else {
	  strcpy(code[num_of_codes]->lines[(*num_of_word_nedded) - 2]->word, first_word);
	  strcpy(code[num_of_codes]->lines[(*num_of_word_nedded) - 1]->word, second_word);
	}  
	  	  
	  free(first_word);
	  free(second_word);
}

/**
 * @brief  This function is used to handle the case of immediate operand.
 * it will encode the immediate operand, based on the type of the operand. 
 * @param current_pattern  The current pattern that we want to handle.
 * @param operand_num  The number of the operand.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 */
void immediate(struct Node *current_pattern, int operand_num, int *num_of_word_nedded) {
    char *temp_ins;
    (*num_of_word_nedded) += 1;
	/* extract the immidiate number and encode it, based on the num of the operand */
	if (operand_num == 1) {
	  temp_ins = encoded_immidiate_number(extract_immidiate_number(current_pattern->data, 1, current_pattern_num));
	  strcpy(code[num_of_codes]->lines[1]->word, temp_ins);
	} else {
	  temp_ins = encoded_immidiate_number(extract_immidiate_number(current_pattern->data, 0, current_pattern_num));
	  strcpy(code[num_of_codes]->lines[(*num_of_word_nedded) - 1]->word, temp_ins);
	}
	free(temp_ins);
}

/**
 * @brief  This function is used to handle the case of source operand.
 * it will encode the source operand, based on the type of the operand. 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 */
void source_operand(struct Node *current_pattern, int current_pattern_num, int *num_of_word_nedded){
	switch (current_pattern->data->choice.inst.operands[1].op_type) {
        case REGISTER:
          /*only the sorsce operand is register*/
          register_encoder(current_pattern, 1, num_of_word_nedded);
          break;

        case IMMEDIATE_NUMBER:
          immediate(current_pattern, 1, num_of_word_nedded);
          break;

        case DIRECT:
          direct(current_pattern, 1, num_of_word_nedded);
          break;
        case DIRECT_INDEX:

          direct_index(current_pattern, 1, num_of_word_nedded, current_pattern_num);
          break;
        }
}

/**
 * @brief  This function is used to handle the case of target operand.
 * it will encode the target operand, based on the type of the operand. 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param num_of_word_nedded  The number of word nedded to encode the operand.
 */
void target_operand(struct Node *current_pattern, int current_pattern_num, int *num_of_word_nedded){
	switch (current_pattern->data->choice.inst.operands[0].op_type) {
    case REGISTER:
	/*only the target operand is register*/
      if ((current_pattern->data->choice.inst.num_of_operands == 2) &&
          (current_pattern->data->choice.inst.operands[1].op_type != REGISTER)) {
        register_encoder(current_pattern, 0, num_of_word_nedded);
      }
      break;

    case IMMEDIATE_NUMBER:
      immediate(current_pattern, 0, num_of_word_nedded);
      break;

    case DIRECT:
      direct(current_pattern, 0, num_of_word_nedded);
      break;

    case DIRECT_INDEX:
      direct_index(current_pattern, 0, num_of_word_nedded, current_pattern_num);
      break;
    }
}


/**
 * @brief  This function is used to handle the case of instruction.
 * it will insert the instruction to the code table, and encode the instruction.
 * 
 * @param current_pattern  The current pattern that we want to handle.
 * @param current_pattern_num  The number of the current pattern.
 * @param IC  The instruction counter.
 */
void instruction(struct Node *current_pattern, int current_pattern_num, int *IC) {
  int num_of_word_nedded = 1;
  char *temp_ins;

  /* check the valdiation of the operands type and number */
  if (!check_validation_of_operands_num(current_pattern->data)) {
    print_error_msg("invalid number of operands", current_pattern_num);
    return;
  }
  if (!check_validation_of_operands_type(current_pattern->data)) {
    print_error_msg("invalid type of operands", current_pattern_num);
    return;
  }

  /* check if there is a label */
  if (current_pattern->data->label[0]) {
    handle_lable_define(current_pattern->data, "code", *IC, current_pattern_num);
  }

  allocate_memory_for_instructions(current_pattern->data, &(code[num_of_codes]),
                                   current_pattern_num);
  current_pattern->data->choice.inst.code = code[num_of_codes];

  temp_ins = encoded_instruction(current_pattern->data);
  strcpy(code[num_of_codes]->lines[0]->word, temp_ins);
  free(temp_ins);

  /* check if there ara operands */
  if (current_pattern->data->choice.inst.num_of_operands >= 1) {

    /* check the type of the first operand */
    if (current_pattern->data->choice.inst.num_of_operands == 2) { /*there is olso sorce op*/
      if ((current_pattern->data->choice.inst.operands[0].op_type ==
           REGISTER) &&
          (current_pattern->data->choice.inst.operands[1].op_type ==
           REGISTER)) { /*tow registers*/
        tow_registers(current_pattern, current_pattern_num,
                      &num_of_word_nedded);

      } else
        source_operand(current_pattern, current_pattern_num, &num_of_word_nedded);
    }
	target_operand(current_pattern, current_pattern_num, &num_of_word_nedded);
    
  }
  num_of_codes++;
  (*IC) += num_of_word_nedded;
}