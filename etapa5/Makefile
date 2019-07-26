CC=gcc
LC=lex
YC=yacc

etapa5: lex.yy.o hash.o y.tab.o ast.o semantic.o main.o tac.o
	${CC} -o etapa5 lex.yy.o hash.o ast.o semantic.o y.tab.o main.o tac.o
main.o: main.c lex.yy.c
	${CC} -c main.c
hash.o: hash.c
	${CC} -c hash.c
ast.o: ast.c
	${CC} -c ast.c
tac.o: tac.c
	${CC} -c tac.c
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
	rm lex.* y.* lex.yy.o hash.o ast.o semantic.o y.tab.o main.o tac.o etapa5