exem:  firstround.o secondround.o back.o assembler.o trie.o exempel.o
	gcc -g  firstround.o secondround.o back.o assembler.o trie.o exempel.o -o exem

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

exempel.o: exempel.c
	gcc -c -g exempel.c -o exempel.o

clean:
	rm -f *.o exem
