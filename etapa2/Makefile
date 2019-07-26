CC=gcc
LC=lex
YC=yacc

etapa2: y.tab.o lex.yy.o 
	${CC} -o etapa2 lex.yy.o y.tab.o
main.o: main.c
	${CC} -c main.c
hash.o: hash.c
	${CC} -c hash.c
lex.yy.o: lex.yy.c
	${CC} -c lex.yy.c
lex.yy.c: scanner.l
	${LC} --header-file=lex.yy.h scanner.l 
y.tab.o: y.tab.c
	${CC} -c y.tab.c
y.tab.c: parser.y
	${YC} -d -v parser.y
clean:
	rm lex.* y.* etapa2
