CC=gcc
LC=lex

etapa1: lex.yy.o 
	${CC} -o etapa1 lex.yy.o
main.o: main.c
	${CC} -c main.c
hash.o: hash.c
	${CC} -c hash.c
lex.yy.o: lex.yy.c
	${CC} -c lex.yy.c
lex.yy.c: scanner.l
	${LC} --header-file=lex.yy.h scanner.l 
clean:
	rm *.o lex.yy.c etapa1
