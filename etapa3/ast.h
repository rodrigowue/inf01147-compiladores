#ifndef AST_H
#define AST_H

#include "hash.h"

#define MAX_SONS 4

#define AST_SYMBOL 				1
#define AST_VECTOR 				2
#define AST_EXP 				3
#define AST_ATRIBUICAO 			4
#define AST_ATRIBUICAO_VEC 		5
#define AST_FUNCAO		 		6
#define AST_LISTEXP		 		7
#define AST_CALL_FUNCTION		8
#define AST_EQUAL				9
#define AST_IF					10
#define AST_ELSE				11
#define AST_OR					12
#define AST_AND					13
#define AST_LE					14
#define AST_GE					15
#define AST_ADD					16
#define AST_SUB					17
#define AST_DIV					18
#define AST_MULT				19
#define AST_LESS				20
#define AST_GREATER				21
#define AST_ASSOCIATIVIDADE		22
#define AST_NEG					23
#define AST_RETURN				24
#define AST_READ				25
#define AST_LEAP				26
#define AST_LOOP				27
#define AST_BLOCO				28
#define AST_LISTCMD				29
#define AST_PRINT				30
#define AST_PRINTLIST			31
#define AST_PARAM				32
#define AST_BYTE				33
#define AST_INT 				34
#define AST_FLOAT 				35
#define AST_LISTPARAM 			36
#define AST_CABECALHO 			37
#define AST_VARIAVEL_DEC		38
#define AST_VEC_DEC 			39
#define AST_LISTLITERAIS 		40
#define AST_VEC_DEC_INIT 		41
#define AST_LISTDEC 	 		42


typedef struct ast_node
{
	int type;
	Node *symbol;
	struct ast_node *son[MAX_SONS];
} Ast;

Ast *astCreate(int type, Node* symbol, Ast *s0, Ast *s1, Ast *s2, Ast *s3);

void astPrint(Ast *node, int nivel);
 
#endif