#include "front.h"
#include "tables.h"
#include "trie/trie.h"
#include <stdlib.h>
#include <string.h>
int num_of_patterns = 19;
int num_of_symbols = 9;
int num_of_entries = 2;
int num_of_externals = 2;
int main() {
  Pattern p = (struct pattern *)calloc(15, sizeof(struct pattern));
  Pattern s = (struct pattern *)calloc(19, sizeof(struct pattern));
  // .entry LIST
  // .extern W
  // .define sz = 2
  // MAIN: mov r3, LIST[sz]
  // LOOP: jmp W
  //  prn #-5
  //  mov STR[5], STR[2]
  //  sub r1, r4
  //  cmp K, #sz
  //  bne W
  // L1: inc L3
  // .entry LOOP
  //  bne LOOP
  // END: hlt
  // .define len = 4
  // STR: .string “abcdef”
  // LIST: .data 6, -9, len
  // K: .data 22
  // .extern L3

  // .entry LIST
  s[0].type_line = DIRECTIVE;
  s[0].dir.directive_type = ENTRY;
  strcpy(s[0].label, "LIST");

  // .extern W
  s[1].type_line = DIRECTIVE;
  s[1].dir.directive_type = EXTERN;
  strcpy(s[1].label, "W");

  // .define sz = 2
  s[2].type_line = DEFINE;
  s[2].def.value = 2;
  strcpy(s[2].label, "sz");

  // MAIN: mov r3, LIST[sz]
  s[3].type_line = INSTRUCTION;
  s[3].inst.op_type = MOV;
  s[3].inst.num_of_operands = 2;
  s[3].inst.operands[0].op_type = DIRECT_INDEX;
  strcpy(s[3].inst.operands[0].operand_value.symbol, "LIST");
  strcpy(s[3].inst.operands[0].operand_value.const_num, "sz");
  s[3].inst.operands[1].op_type = REGISTER;
  s[3].inst.operands[1].operand_value.reg = r3;
  strcpy(s[3].label, "MAIN");

  // LOOP: jmp W
  s[4].type_line = INSTRUCTION;
  s[4].inst.op_type = JMP;
  s[4].inst.num_of_operands = 1;
  s[4].inst.operands[0].op_type = DIRECT;
  strcpy(s[4].inst.operands[0].operand_value.symbol, "W");
  strcpy(s[4].label, "LOOP");

  // prn #-5
  s[5].type_line = INSTRUCTION;
  s[5].inst.op_type = PRN;
  s[5].inst.num_of_operands = 1;
  s[5].inst.operands[0].op_type = IMMEDIATE_NUMBER;
  s[5].inst.operands[0].operand_value.value = -5;

  // mov STR[5], STR[2]
  s[6].type_line = INSTRUCTION;
  s[6].inst.op_type = MOV;
  s[6].inst.num_of_operands = 2;
  s[6].inst.operands[0].op_type = DIRECT_INDEX;
  strcpy(s[6].inst.operands[0].operand_value.symbol, "STR");
  s[6].inst.operands[0].operand_value.value = 2;
  s[6].inst.operands[1].op_type = DIRECT_INDEX;
  strcpy(s[6].inst.operands[1].operand_value.symbol, "STR");
  s[6].inst.operands[1].operand_value.value = 5;

  // sub r1, r4
  s[7].type_line = INSTRUCTION;
  s[7].inst.op_type = SUB;
  s[7].inst.num_of_operands = 2;
  s[7].inst.operands[0].op_type = REGISTER;
  s[7].inst.operands[0].operand_value.reg = r4;
  s[7].inst.operands[1].op_type = REGISTER;
  s[7].inst.operands[1].operand_value.reg = r1;

  // cmp K, #sz
  s[8].type_line = INSTRUCTION;
  s[8].inst.op_type = CMP;
  s[8].inst.num_of_operands = 2;
  s[8].inst.operands[0].op_type = IMMEDIATE_NUMBER;
  strcpy(s[8].inst.operands[0].operand_value.const_num, "sz");
  s[8].inst.operands[1].op_type = DIRECT;
  strcpy(s[8].inst.operands[1].operand_value.symbol, "K");

  // bne W
  s[9].type_line = INSTRUCTION;
  s[9].inst.op_type = BNE;
  s[9].inst.num_of_operands = 1;
  s[9].inst.operands[0].op_type = DIRECT;
  strcpy(s[9].inst.operands[0].operand_value.symbol, "W");

  // L1: inc L3
  s[10].type_line = INSTRUCTION;
  s[10].inst.op_type = INC;
  s[10].inst.num_of_operands = 1;
  s[10].inst.operands[0].op_type = DIRECT;
  strcpy(s[10].inst.operands[0].operand_value.symbol, "L3");
  strcpy(s[10].label, "L1");

  // .entry LOOP
  s[11].type_line = DIRECTIVE;
  s[11].dir.directive_type = ENTRY;
  strcpy(s[11].label, "LOOP");

  // bne LOOP
  s[12].type_line = INSTRUCTION;
  s[12].inst.op_type = BNE;
  s[12].inst.num_of_operands = 1;
  s[12].inst.operands[0].op_type = DIRECT;
  strcpy(s[12].inst.operands[0].operand_value.symbol, "LOOP");

  // END: hlt
  s[13].type_line = INSTRUCTION;
  s[13].inst.op_type = HLT;
  s[13].inst.num_of_operands = 0;
  strcpy(s[13].label, "END");

  // .define len = 4
  s[14].type_line = DEFINE;
  s[14].def.value = 4;
  strcpy(s[14].label, "len");

  // STR: .string “abcdef”
  s[15].type_line = DIRECTIVE;
  s[15].dir.directive_type = STRING;
  strcpy(s[15].dir.string, "abcdef");
  strcpy(s[15].label, "STR");
  s[15].dir.size = 7;

  // LIST: .data 6, -9, len
  s[16].type_line = DIRECTIVE;
  s[16].dir.directive_type = DATA;
  s[16].dir.size = 3;
  strcpy(s[16].dir.data[0], "6");
  strcpy(s[16].dir.data[1], "-9");
  strcpy(s[16].dir.data[2], "len");
  strcpy(s[16].label, "LIST");

  // K: .data 22
  s[17].type_line = DIRECTIVE;
  s[17].dir.directive_type = DATA;
  s[17].dir.size = 1;
  strcpy(s[17].dir.data[0], "22");
  strcpy(s[17].label, "K");

  // .extern L3
  s[18].type_line = DIRECTIVE;
  s[18].dir.directive_type = EXTERN;
  strcpy(s[18].label, "L3");

  // .define sz = 2
  // MAIN: mov r3, LIST[sz]
  // LOOP: jmp L1
  // prn #-5
  // mov STR[5], STR[2]
  // sub r1, r4
  // cmp r3, #sz
  // bne END
  // END: hlt
  // .define len = 4
  // STR: .string “abcdef”
  // LIST: .data 6, -9, len
  // K: .data 22
  // L1: inc K
  // bne LOOP
  p[0].type_line = DEFINE;
  p[0].def.value = 2;
  strcpy(p[0].label, "sz");

  // p[1] = MAIN: mov r3, LIST[sz]
  p[1].type_line = INSTRUCTION;
  p[1].inst.op_type = MOV;
  p[1].inst.num_of_operands = 2;
  p[1].inst.operands[0].op_type = DIRECT_INDEX;
  strcpy(p[1].inst.operands[0].operand_value.symbol, "LIST");
  strcpy(p[1].inst.operands[0].operand_value.const_num, "sz");
  p[1].inst.operands[1].op_type = REGISTER;
  p[1].inst.operands[1].operand_value.reg = r3;
  strcpy(p[1].label, "MAIN");

  // p[2] = LOOP: jmp L1
  p[2].type_line = INSTRUCTION;
  p[2].inst.op_type = JMP;
  p[2].inst.num_of_operands = 1;
  p[2].inst.operands[0].op_type = DIRECT;
  strcpy(p[2].inst.operands[0].operand_value.symbol, "L1");
  strcpy(p[2].label, "LOOP");

  // p[3] = prn #-5
  p[3].type_line = INSTRUCTION;
  p[3].inst.op_type = PRN;
  p[3].inst.num_of_operands = 1;
  p[3].inst.operands[0].op_type = IMMEDIATE_NUMBER;
  p[3].inst.operands[0].operand_value.value = -5;

  // p[4] = mov STR[5], STR[2]
  p[4].type_line = INSTRUCTION;
  p[4].inst.op_type = MOV;
  p[4].inst.num_of_operands = 2;
  p[4].inst.operands[0].op_type = DIRECT_INDEX;
  strcpy(p[4].inst.operands[0].operand_value.symbol, "STR");
  p[4].inst.operands[0].operand_value.value = 2;
  p[4].inst.operands[1].op_type = DIRECT_INDEX;
  strcpy(p[4].inst.operands[1].operand_value.symbol, "STR");
  p[4].inst.operands[1].operand_value.value = 5;

  // p[5] = sub r1, r4
  p[5].type_line = INSTRUCTION;
  p[5].inst.op_type = SUB;
  p[5].inst.num_of_operands = 2;
  p[5].inst.operands[0].op_type = REGISTER;
  p[5].inst.operands[0].operand_value.reg = r4;
  p[5].inst.operands[1].op_type = REGISTER;
  p[5].inst.operands[1].operand_value.reg = r1;

  // p[6] = cmp r3, #sz
  p[6].type_line = INSTRUCTION;
  p[6].inst.op_type = CMP;
  p[6].inst.num_of_operands = 2;
  p[6].inst.operands[0].op_type = IMMEDIATE_NUMBER;
  strcpy(p[6].inst.operands[0].operand_value.const_num, "sz");
  p[6].inst.operands[1].op_type = REGISTER;
  p[6].inst.operands[1].operand_value.reg = r3;

  // p[7] = bne END
  p[7].type_line = INSTRUCTION;
  p[7].inst.op_type = BNE;
  p[7].inst.num_of_operands = 1;
  p[7].inst.operands[0].op_type = DIRECT;
  strcpy(p[7].inst.operands[0].operand_value.symbol, "END");

  // p[8] = END: hlt
  p[8].type_line = INSTRUCTION;
  p[8].inst.op_type = HLT;
  p[8].inst.num_of_operands = 0;
  strcpy(p[8].label, "END");

  // p[9] = .define len = 4
  p[9].type_line = DEFINE;
  p[9].def.value = 4;
  strcpy(p[9].label, "len");

  // p[10] = STR: .string “abcdef”
  p[10].type_line = DIRECTIVE;
  p[10].dir.directive_type = STRING;
  strcpy(p[10].dir.string, "abcdef");
  strcpy(p[10].label, "STR");

  // p[11] = LIST: .data 6, -9, len
  p[11].type_line = DIRECTIVE;
  p[11].dir.directive_type = DATA;
  p[11].dir.size = 3;
  strcpy(p[11].dir.data[0], "6");
  strcpy(p[11].dir.data[1], "-9");
  strcpy(p[11].dir.data[2], "len");
  strcpy(p[11].label, "LIST");

  // p[12] = K: .data 22
  p[12].type_line = DIRECTIVE;
  p[12].dir.directive_type = DATA;
  p[12].dir.size = 1;
  strcpy(p[12].dir.data[0], "22");
  strcpy(p[12].label, "K");

  // p[13] = L1: inc K
  p[13].type_line = INSTRUCTION;
  p[13].inst.op_type = INC;
  p[13].inst.num_of_operands = 1;
  p[13].inst.operands[0].op_type = DIRECT;
  strcpy(p[13].inst.operands[0].operand_value.symbol, "K");
  strcpy(p[13].label, "L1");

  // p[14] = bne LOOP
  p[14].type_line = INSTRUCTION;
  p[14].inst.op_type = BNE;
  p[14].inst.num_of_operands = 1;
  p[14].inst.operands[0].op_type = DIRECT;
  strcpy(p[14].inst.operands[0].operand_value.symbol, "LOOP");

  //   p[0].type_line = INSTRUCTION;
  //   p[0].inst.op_type = MOV;
  //   p[0].inst.num_of_operands = 2;
  //   p[0].inst.operands[0].op_type = DIRECT;
  //   strcpy(p[0].inst.operands[0].operand_value.symbol, "symbol1");
  //   p[0].inst.operands[1].op_type = DIRECT;
  //   strcpy(p[0].inst.operands[1].operand_value.symbol, "symbol2");

  //   p[1].type_line = DEFINE;
  //   p[1].def.value = 5;
  //   strcpy(p[1].label, "define1");

  //   p[2].type_line = DIRECTIVE;
  //   p[2].dir.directive_type = DATA;
  //   p[2].dir.size = 3;
  //   strcpy(p[2].dir.data[0], "1");
  //   strcpy(p[2].dir.data[1], "define1");
  //   strcpy(p[2].dir.data[2], "55");

  //   p[3].type_line = DIRECTIVE;
  //   p[3].dir.directive_type = EXTERN;
  //   strcpy(p[3].label, "extern2");

  //   p[4].type_line = DIRECTIVE;
  //   p[4].dir.directive_type = DATA;
  //   p[4].dir.size = 3;
  //   strcpy(p[4].dir.data[0], "1");
  //   strcpy(p[4].dir.data[1], "444");
  //   strcpy(p[4].dir.data[2], "4441");
  //   strcpy(p[4].label, "symbol1");

  //   p[5].type_line = DIRECTIVE;
  //   p[5].dir.directive_type = EXTERN;
  //   strcpy(p[5].label, "extern1");

  //   p[6].type_line = INSTRUCTION;
  //   p[6].inst.op_type = MOV;
  //   p[6].inst.num_of_operands = 2;
  //   p[6].inst.operands[0].op_type = DIRECT;
  //   strcpy(p[6].inst.operands[0].operand_value.symbol, "symbol1");
  //   p[6].inst.operands[1].op_type = DIRECT;
  //   strcpy(p[6].inst.operands[1].operand_value.symbol, "extern1");
  //   strcpy(p[6].label, "symbol2");

  printf("%d\n", sizeof(struct pattern));
  struct node *head = NULL;
  struct node *tail = NULL;
  struct node *temp = NULL;
  int i;
  for (i = 0; i < 19; i++) {
    temp = (struct node *)malloc(sizeof(struct node));
    temp->data = &s[i];
    temp->next = NULL;
    if (head == NULL) {
      head = temp;
      tail = temp;
    } else {
      tail->next = temp;
      tail = temp;
    }
  }
  temp = head;
  tail->next = NULL;
  assembler("exe1", head);
  free(s);
  free(p);
  for (i = 0; i < 19; i++) {
    temp = head->next;
    free(head);
	head = temp;
  }
  extern Code *code;
  extern int num_of_codes;
  extern int IC;
  for (i = 0; i < num_of_codes; i++) {
	for (int j = 0; j<code[i]->num_of_lines; j++) {
		free(code[i]->lines[j]);}
	free(code[i]->lines);
	free(code[i]);
	


  }
  free(code);

  extern Code data;
  extern int DC;
  for (i = 0; i < DC; i++) {
	free(data->lines[i]);
  }
  free(data->lines);
  free(data);

  extern Trie symbols, constant, externals, entries, macro_trie;
  destroy_trie(&symbols);
  destroy_trie(&constant);
  destroy_trie(&externals);
  destroy_trie(&entries);
  destroy_trie(&macro_trie);


  extern Symbol *symbol_table;
  for (i = 0; i < num_of_symbols; i++) {
	free(symbol_table[i]);
  }
  free(symbol_table);
  extern Symbol *symbol_table_of_entries;
  free(symbol_table_of_entries);

  extern Entry *entry_table;
  for (i = 0; i < num_of_entries; i++) {
	free(entry_table[i]);
  }
  free(entry_table);
  extern int num_of_constants_in_table;
  extern Constant *constant_table;
  for (i = 0; i < num_of_constants_in_table; i++) {
	free(constant_table[i]);
  }
  free(constant_table);

  extern External *external_table;
  for (i = 0; i < num_of_externals; i++) {
	free(external_table[i]->addresses);
	free(external_table[i]);
  }
  free(external_table);

extern char **binary_table,  **binary_table_translated;
  for (i = 0; i < IC + DC; i++) {
	free(binary_table[i]);
	free(binary_table_translated[i]);
  }
  free(binary_table);
  free(binary_table_translated);




  return 0;
}