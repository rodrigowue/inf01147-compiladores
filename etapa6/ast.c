#include "ast.h"
#include <stdlib.h>
#include <stdio.h>


Ast *astCreate(int type, Node *symbol, Ast *s0, Ast *s1, Ast *s2, Ast *s3)
{
	Ast *newNode;

	newNode = (Ast*) calloc(1, sizeof(Ast));
	newNode->dataType = 0;
	newNode->type = type;
	newNode->symbol = symbol;
	newNode->son[0] = s0;
	newNode->son[1] = s1;
	newNode->son[2] = s2;
	newNode->son[3] = s3;

	return newNode;
}


void astPrint(Ast *node, int nivel)
{
	int aux = nivel;

	if(node != 0)
	{
		while(aux>0)
		{
			printf("   ");
			aux--;
		}

		switch(node->type)
		{
			case 1: printf("%s\n", node->symbol->text); break;
			case 2: printf("AST_VECTOR\n"); break;
			case 3: printf("AST_EXP\n"); break;
			case 4: printf("AST_ATRIBUICAO\n"); break;
			case 5: printf("AST_ATRIBUICAO_VEC\n"); break;
			case 6: printf("AST_FUNCAO\n"); break;
			case 7: printf("AST_LISTEXP\n"); break;
			case 8: printf("AST_CALL_FUNCTION\n"); break;
			case 9: printf("AST_EQUAL\n"); break;
			case 10: printf("AST_IF\n"); break;
			case 11: printf("AST_ELSE\n"); break;
			case 12: printf("AST_OR\n"); break;
			case 13: printf("AST_AND\n"); break;
			case 14: printf("AST_LE\n"); break;
			case 15: printf("AST_GE\n"); break;
			case 16: printf("AST_ADD\n"); break;
			case 17: printf("AST_SUB\n"); break;
			case 18: printf("AST_DIV\n"); break;
			case 19: printf("AST_MULT\n"); break;
			case 20: printf("AST_LESS\n"); break;
			case 21: printf("AST_GREATER\n"); break;
			case 22: printf("AST_ASSOCIATIVIDADE\n"); break;
			case 23: printf("AST_NEG\n"); break;
			case 24: printf("AST_RETURN\n"); break;
			case 25: printf("AST_READ\n"); break;
			case 26: printf("AST_LEAP\n"); break;
			case 27: printf("AST_LOOP\n"); break;
			case 28: printf("AST_BLOCO\n"); break;
			case 29: printf("AST_LISTCMD\n"); break;
			case 30: printf("AST_PRINT\n"); break;
			case 31: printf("AST_PRINTLIST\n"); break;
			case 32: printf("AST_PARAM\n"); break;
			case 33: printf("AST_BYTE\n"); break;
			case 34: printf("AST_INT\n"); break;
			case 35: printf("AST_FLOAT\n"); break;
			case 36: printf("AST_LISTPARAM\n"); break;
			case 37: printf("AST_CABECALHO\n"); break;
			case 38: printf("AST_VARIAVEL_DEC\n"); break;
			case 39: printf("AST_VEC_DEC\n"); break;
			case 40: printf("AST_LISTLITERAIS\n"); break;
			case 41: printf("AST_VEC_DEC_INIT\n"); break;
			case 42: printf("AST_LISTDEC\n"); break;
		}
		astPrint(node->son[0], nivel+1);
		astPrint(node->son[1], nivel+1);
		astPrint(node->son[2], nivel+1);
		astPrint(node->son[3], nivel+1);
	}
}