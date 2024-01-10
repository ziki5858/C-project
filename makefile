pre: preprossesor.c trie/trie.c
	gcc -fsanitize=address preprossesor.c  trie/trie.c -o pre -g 
