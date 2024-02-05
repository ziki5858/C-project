CFLAGS = -Wall -ansi -pedantic -fsanitize=address -g
exem:  firstround.o secondround.o back.o assembler.o trie.o memory.o front.o instrucstion.o directive.o define.o preprossesor.o
	gcc -g  firstround.o secondround.o back.o assembler.o trie.o memory.o front.o instrucstion.o directive.o define.o preprossesor.o -o exem

firstround.o: firstround.c
	gcc -c -g firstround.c -o firstround.o

secondround.o: secondround.c
	gcc -c -g secondround.c -o secondround.o

back.o: back.c
	gcc -c -g back.c -o back.o

assembler.o: assembler.c	
	gcc -c -g assembler.c -o assembler.o

trie.o: trie/trie.c
	gcc -c -g trie/trie.c -o trie.o

memory.o: memory.c
	gcc -c -g memory.c -o memory.o

front.o: front.c
	gcc -c -g front.c -o front.o

instrucstion.o: instrucstion.c
	gcc -c -g instrucstion.c -o instrucstion.o

directive.o: directive.c
	gcc -c -g directive.c -o directive.o

define.o: define.c
	gcc -c -g define.c -o define.o

preprossesor.o: preprossesor.c
	gcc -c -g preprossesor.c -o preprossesor.o

clean:
	rm -f *.o 
