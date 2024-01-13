#include "front.h"

struct pattern p[10];
p[0].type_line = INSTRUCTION;
p[0].inst.op_type = MOV;
p[0].inst.num_of_operands = 2;
p[0].inst.operands[0].op_type = DIRECT;
strcpy(p[0].inst.operands[0].operand_value.symbol, "symbol1");
p[0].inst.operands[1].op_type = DIRECT;
strcpy(p[0].inst.operands[1].operand_value.symbol, "symbol2");

p[1].type_line = DEFINE;
p[1].def.value = 5;
p[1].label = "define1";

p[2].type_line = DIRECTIVE;
p[2].dir.directive_type = DATA;
p[2].dir.size = 3;
p[2].dir.data = calloc(3, sizeof(char *));
p[2].dir.data[0] = "1";
p[2].dir.data[1] = "define1";
p[2].dir.data[2] = "symbol1";

p[3].type_line = DIRECTIVE;
p[3].dir.directive_type = ENTRY;
p[3].label = "entry1";

p[4].type_line = DIRECTIVE;
p[4].dir.directive_type = data;
p[4].dir.size = 3;
p[4].dir.data = calloc(3, sizeof(char *));
p[4].dir.data[0] = "1";
p[4].dir.data[1] = "j";
p[4].dir.data[2] = "4441";
p[4].label = "symbol1";

p[5].type_line = DIRECTIVE;
p[5].dir.directive_type = EXTERN;
p[5].label = "extern1";

p[6].type_line = INSTRUCTION;
p[6].inst.op_type = MOV;
p[6].inst.num_of_operands = 2;
p[6].inst.operands[0].op_type = DIRECT;
strcpy(p[6].inst.operands[0].operand_value.symbol, "symbol1");
p[6].inst.operands[1].op_type = DIRECT;
strcpy(p[6].inst.operands[1].operand_value.symbol, "extern1");
p[6].label = "symbol2";