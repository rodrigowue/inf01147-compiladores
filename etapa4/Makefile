CC=gcc
LC=lex
YC=yacc

etapa4: lex.yy.o hash.o y.tab.o ast.o semantic.o main.o
	${CC} -o etapa4 lex.yy.o hash.o ast.o semantic.o y.tab.o main.o
main.o: main.c lex.yy.c
	${CC} -c main.c
hash.o: hash.c
	${CC} -c hash.c
ast.o: ast.c
	${CC} -c ast.c
semantic.o: semantic.c
	${CC} -c semantic.c
lex.yy.o: lex.yy.c
	${CC} -c lex.yy.c
lex.yy.c: scanner.l y.tab.o
	${LC} --header-file=lex.yy.h scanner.l 
y.tab.o: y.tab.c
	${CC} -c y.tab.c
y.tab.c: parser.y
	${YC} -d -v parser.y
clean:
	rm lex.* y.* ast.o hash.o etapa4