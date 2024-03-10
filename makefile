CFLAGS = -Wall -ansi -pedantic -fsanitize=address -g
assembler:  rounds.o back.o assembler.o trie.o memory.o front.o instrucstion.o directive.o define.o preprossesor.o structs_func.o encoder.o validation.o bilder.o
	gcc ${CFLAGS}  rounds.o back.o assembler.o trie.o memory.o front.o instrucstion.o directive.o define.o preprossesor.o structs_func.o encoder.o validation.o bilder.o -o assembler

rounds.o: ./assemblerRounds/rounds.c ./assemblerRounds/headers/rounds.h
	gcc -c ${CFLAGS} ./assemblerRounds/rounds.c -o rounds.o



back.o: ./back/back.c ./back/back.h
	gcc -c ${CFLAGS} ./back/back.c -o back.o

assembler.o: assembler.c	
	gcc -c ${CFLAGS} assembler.c -o assembler.o

trie.o: trie/trie.c
	gcc -c ${CFLAGS} trie/trie.c -o trie.o

memory.o: ./back/memory.c
	gcc -c ${CFLAGS} ./back/memory.c -o memory.o

front.o: ./front/front.c
	gcc -c ${CFLAGS} ./front/front.c -o front.o

instrucstion.o: ./front/instrucstion.c
	gcc -c ${CFLAGS} ./front/instrucstion.c -o instrucstion.o

directive.o: ./front/directive.c
	gcc -c ${CFLAGS} ./front/directive.c -o directive.o

define.o: ./front/define.c
	gcc -c ${CFLAGS} ./front/define.c -o define.o

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
