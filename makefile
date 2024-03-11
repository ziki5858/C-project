CFLAGS = -Wall -ansi -pedantic -fsanitize=address -g
assembler:  rounds.o print.o assembler.o trie.o memory.o parse.o instrucstion.o directive.o define.o preprossesor.o structs_func.o encoder.o validation.o bilder.o
	gcc ${CFLAGS}  rounds.o print.o assembler.o trie.o memory.o parse.o instrucstion.o directive.o define.o preprossesor.o structs_func.o encoder.o validation.o bilder.o -o assembler

rounds.o: ./assemblerRounds/rounds.c ./assemblerRounds/headers/rounds.h
	gcc -c ${CFLAGS} ./assemblerRounds/rounds.c -o rounds.o



print.o: ./end/print.c ./end/print.h
	gcc -c ${CFLAGS} ./end/print.c -o print.o

assembler.o: assembler.c	
	gcc -c ${CFLAGS} assembler.c -o assembler.o

trie.o: trie/trie.c
	gcc -c ${CFLAGS} trie/trie.c -o trie.o

memory.o: ./end/memory.c
	gcc -c ${CFLAGS} ./end/memory.c -o memory.o

parse.o: ./parse/parse.c
	gcc -c ${CFLAGS} ./parse/parse.c -o parse.o

instrucstion.o: ./parse/instrucstion.c
	gcc -c ${CFLAGS} ./parse/instrucstion.c -o instrucstion.o

directive.o: ./parse/directive.c
	gcc -c ${CFLAGS} ./parse/directive.c -o directive.o

define.o: ./parse/define.c
	gcc -c ${CFLAGS} ./parse/define.c -o define.o

preprossesor.o: ./preprocessor/preprossesor.c ./preprocessor/preprossesor.h
	gcc -c ${CFLAGS}  ./preprocessor/preprossesor.c -o preprossesor.o

structs_func.o: ./assemblerRounds/structs_func.c ./assemblerRounds/headers/structs_func.h
	gcc -c ${CFLAGS} ./assemblerRounds/structs_func.c -o structs_func.o

encoder.o: ./assemblerRounds/encoder.c ./assemblerRounds/headers/encoder.h
	gcc -c ${CFLAGS} ./assemblerRounds/encoder.c -o encoder.o

validation.o: ./assemblerRounds/validation.c ./assemblerRounds/headers/validation.h
	gcc -c ${CFLAGS} ./assemblerRounds/validation.c -o validation.o
	
bilder.o: ./assemblerRounds/bilder.c ./assemblerRounds/headers/bilder.h
	gcc -c ${CFLAGS} ./assemblerRounds/bilder.c -o bilder.o

clean:
	rm -f *.o 
