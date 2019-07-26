CC=gcc
LC=lex
YC=yacc

etapa3: lex.yy.o hash.o y.tab.o ast.o
	${CC} -o etapa3 lex.yy.o hash.o ast.o y.tab.o 
main.o: main.c lex.yy.c
	${CC} -c main.c
hash.o: hash.c
	${CC} -c hash.c
ast.o: ast.c
	${CC} -c ast.c
lex.yy.o: lex.yy.c
	${CC} -c lex.yy.c
lex.yy.c: scanner.l y.tab.o
	${LC} --header-file=lex.yy.h scanner.l 
y.tab.o: y.tab.c
	${CC} -c y.tab.c
y.tab.c: parser.y
	${YC} -d -v parser.y
clean:
	rm lex.* y.* ast.o hash.o etapa3
