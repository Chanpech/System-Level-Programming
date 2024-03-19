all: czy dzy bitsy 
bitsy: bitsy.h bitsy.c
	gcc -c -Wall bitsy.c -lm
czy: bitsy czy.c
	gcc -Wall -o czy bitsy.o czy.c -lm
dzy: bitsy dzy.c
	gcc -Wall -o dzy bitsy.o dzy.c -lm
clean:
	rm czy dzy bitsy.o -lm
submission:
	gtar czvf prog3.tgz Makefile bitsy.h bitsy.c czy.c dzy.c README
