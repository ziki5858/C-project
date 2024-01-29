CFLAGS = -fsanitize=address -g -Wall -pedantic -ansi
front: front.o define.o instrucstion.o directive.o
	gcc $(CFLAGS)  front.o define.o instrucstion.o directive.o -o front
front.o: front.c headeMethods.h
	gcc $(CFLAGS) -c front.c headeMethods.h
define.o: define.c
	gcc $(CFLAGS) -c define.c
instrucstion.o: instrucstion.c
	gcc $(CFLAGS) -c instrucstion.c
directive.o: directive.c
	gcc $(CFLAGS) -c directive.c
clean:
	rm -f *.o 
