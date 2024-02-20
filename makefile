CFLAGS = -Wall -ansi -pedantic -fsanitize=address -g
assembler:  firstround.o secondround.o back.o assembler.o trie.o memory.o front.o instrucstion.o directive.o define.o preprossesor.o structs_func.o encoder.o
	gcc ${CFLAGS}  firstround.o secondround.o back.o assembler.o trie.o memory.o front.o instrucstion.o directive.o define.o preprossesor.o structs_func.o encoder.o -o assembler

firstround.o: ./assemblerRounds/firstround.c
	gcc -c ${CFLAGS} ./assemblerRounds/firstround.c -o firstround.o

secondround.o: ./assemblerRounds/secondround.c
	gcc -c ${CFLAGS} ./assemblerRounds/secondround.c -o secondround.o

back.o: ./back/back.c
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

preprossesor.o: ./preprocessor/preprossesor.c
	gcc -c ${CFLAGS}  ./preprocessor/preprossesor.c -o preprossesor.o

structs_func.o: ./assemblerRounds/structs_func.c ./assemblerRounds/structs_func.h
	gcc -c ${CFLAGS} ./assemblerRounds/structs_func.c -o structs_func.o

encoder.o: ./assemblerRounds/encoder.c ./assemblerRounds/encoder.h
	gcc -c ${CFLAGS} ./assemblerRounds/encoder.c -o encoder.o
clean:
	rm -f *.o 
