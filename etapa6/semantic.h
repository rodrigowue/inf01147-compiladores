#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"

#define FUNC_DEC 	1
#define FUNC_CALL 	2
#define FUNC_RETURN 3


typedef struct Func
{
	char *nome;
	Ast* listParam;
	struct Func *next;
}Functions;

Functions *funcoes;
Functions *chamadaFuncoes;
Functions *retornoFuncao;

int erroSemantico;

void insertFunction(Ast* node, int type, char *nome);
void checkCallFunction();
int checkParameters(Ast *listParamCall, Ast *listParamFunc, char *nome, Ast *call, Ast *func, int param, int parada);

void semantic(Ast *node);


void assignDataType(Ast *node);
void setAndCheckRedeclared(Ast *node);
void checkUndeclared(Ast *node);
void checkOperands(Ast *node);
void checkIDType(Ast *node);

void printExpression(Ast *ast);