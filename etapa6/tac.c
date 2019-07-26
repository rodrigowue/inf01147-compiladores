#include <stdio.h>
#include <stdlib.h>
#include "tac.h"

extern char* toNumeral(char *valor);

TAC* tacCreate(int type, Node *res, Node *op1, Node *op2)
{
	TAC *n = (TAC*) calloc(1, sizeof(TAC));
	n->type = type;
	n->res = res;
	n->op1 = op1;
	n->op2 = op2;
	n->prev = 0;
	n->next = 0;	
	return n;
}

TAC* tacJoin(TAC* l1, TAC* l2)
{
	TAC* tac;
	if(!l1) return l2;
	if(!l2) return l1;
	tac = l2;
	while(tac->prev){
		tac = tac->prev;	
	}
	tac->prev = l1;
	return l2;
}

void tacPrintBack(TAC *last)
{
	fprintf(stderr, "\n================================\nGenerated Intermediary Code Below:\n================================\n");
	
	TAC *tac;

	for(tac = last; tac; tac = tac->prev){
		tacPrintOne(tac);
	}

	/*if(last)
	{
		tacPrintOne(last);
		tacPrintBack(last->prev);
	}*/


}

void tacPrintFoward(TAC* first)
{
	fprintf(stderr, "\n================================\nGenerated Intermediary Code Below:\n================================\n");
	
	TAC *tac;

	for(tac = first; tac; tac = tac->next){
		tacPrintOne(tac);
	}
	
	/*if(first)
	{
		tacPrintOne(first);
		tacPrintFoward(first->next);
	}*/
}

TAC* tacReverse(TAC* tac)
{
	TAC* t;
	for(t = tac; t->prev; t = t->prev)
		t->prev->next = t;
	return t;
}

void tacPrintOne(TAC *tac)
{
		if(tac->type == TAC_SYMBOL) return;
		fprintf(stderr, "TAC(");
		switch(tac->type){
			case TAC_SYMBOL: fprintf(stderr, "TAC_SYMBOL"); break;
			case TAC_ADD: fprintf(stderr, "TAC_ADD"); break;
			case TAC_SUB: fprintf(stderr, "TAC_SUB"); break;
			case TAC_MUL: fprintf(stderr, "TAC_MUL"); break;
			case TAC_DIV: fprintf(stderr, "TAC_DIV"); break;
			case TAC_L: fprintf(stderr, "TAC_L"); break;
			case TAC_G: fprintf(stderr, "TAC_G"); break;
			case TAC_LE: fprintf(stderr, "TAC_LE"); break;
			case TAC_GE: fprintf(stderr, "TAC_GE"); break;
			case TAC_EQ: fprintf(stderr, "TAC_EQ"); break;
			case TAC_NE: fprintf(stderr, "TAC_NE"); break;
			case TAC_AND: fprintf(stderr, "TAC_AND"); break;
			case TAC_OR: fprintf(stderr, "TAC_OR"); break;
			case TAC_NOT: fprintf(stderr, "TAC_NOT"); break;
			case TAC_MOVE: fprintf(stderr, "TAC_MOVE"); break;
			case TAC_READ: fprintf(stderr, "TAC_READ"); break;
			case TAC_RET: fprintf(stderr, "TAC_RET"); break;
			case TAC_IFZ: fprintf(stderr, "TAC_IFZ"); break;
			case TAC_LABEL: fprintf(stderr, "TAC_LABEL"); break;
			case TAC_JUMP: fprintf(stderr, "TAC_JUMP"); break;
			case TAC_PRINT: fprintf(stderr, "TAC_PRINT"); break;
			case TAC_PARPOP: fprintf(stderr, "TAC_PARPOP"); break;
			case TAC_FUNCALL: fprintf(stderr, "TAC_FUNCALL"); break;
			case TAC_BEGINFUN: fprintf(stderr, "TAC_BEGINFUN"); break;
			case TAC_ENDFUN: fprintf(stderr, "TAC_ENDFUN"); break;
			case TAC_AREAD: fprintf(stderr, "TAC_AREAD"); break;
			case TAC_AWRITE: fprintf(stderr, "TAC_AWRITE"); break;			
			case TAC_AINIPUSH: fprintf(stderr, "TAC_AINIPUSH"); break;
			case TAC_ASIZE: fprintf(stderr, "TAC_ASIZE"); break;
			case TAC_PARPUSH: fprintf(stderr, "TAC_PARPUSH"); break;
			case TAC_VEC_WRITE: fprintf(stderr, "TAC_VEC_WRITE"); break;
			case TAC_VEC_READ: fprintf(stderr, "TAC_VEC_READ"); break;
			case TAC_ARG: fprintf(stderr, "TAC_ARG"); break;
			case TAC_LISTLITERAIS: fprintf(stderr, "TAC_LISTLITERAIS"); break;
			case TAC_VEC_DEC_INIT: fprintf(stderr, "TAC_VEC_DEC_INIT"); break;
			
			default: fprintf(stderr, "TAC_UNKNOWN"); break;
		}
		if(tac->res) fprintf(stderr, ",%s", tac->res->text); else fprintf(stderr, ",0");
		if(tac->op1) fprintf(stderr, ",%s", tac->op1->text); else fprintf(stderr, ",0");
		if(tac->op2) fprintf(stderr, ",%s", tac->op2->text); else fprintf(stderr, ",0");
		fprintf(stderr, ")\n");
}

TAC* tacGenerate(Ast *node)
{
	float temp, temp1; 
	float resposta = 0;
	int i = 0;
	TAC *code[MAX_SONS];
	TAC *result = 0;
	Node *label;

	if(!node) return 0;
	
	for(i = 0; i < MAX_SONS; i++)
	{
		code[i] = tacGenerate(node->son[i]);
	}

	switch(node->type)
	{
		case AST_SYMBOL: 	result = tacCreate(TAC_SYMBOL, node->symbol, 0, 0); break;
		case AST_VECTOR: 	result = tacJoin(tacJoin(code[0], code[1]), tacCreate(TAC_VEC_READ, makeTemp(node->son[0]->symbol->datatype, resposta), node->son[0]->symbol, code[1]?code[1]->res:0)); break;

		case AST_ADD: 	if((code[0]->res->text)[0] != '_')
							if(code[0]->res->atribuicao)
								temp = atof(toNumeral(code[0]->res->atribuicao));
							else
								temp = atof(toNumeral(code[0]->res->text));
						else
							temp = (float)((code[0]->res->line));
						if((code[1]->res->text)[1] != '_')
							if(code[1]->res->atribuicao)
								temp1 = atof(toNumeral(code[1]->res->atribuicao));
							else
								temp1 = atof(toNumeral(code[1]->res->text));
						else
							temp1 = (float)((code[1]->res->line));

						resposta = temp+temp1;
						return makeBinOp(code[0],code[1],TAC_ADD, resposta);
						break;

		case AST_SUB: 	if((code[0]->res->text)[0] != '_')
							if(code[0]->res->atribuicao)
								temp = atof(toNumeral(code[0]->res->atribuicao));
							else
								temp = atof(toNumeral(code[0]->res->text));
						else
							temp = (float)((code[0]->res->line));
						if((code[1]->res->text)[1] != '_')
							if(code[1]->res->atribuicao)
								temp1 = atof(toNumeral(code[1]->res->atribuicao));
							else
								temp1 = atof(toNumeral(code[1]->res->text));
						else
							temp1 = (float)((code[1]->res->line));

						resposta = temp-temp1;
						return makeBinOp(code[0],code[1],TAC_SUB, resposta);
						break;

		case AST_MULT: if((code[0]->res->text)[0] != '_')
							if(code[0]->res->atribuicao)
								temp = atof(toNumeral(code[0]->res->atribuicao));
							else
								temp = atof(toNumeral(code[0]->res->text));
						else
							temp = (float)((code[0]->res->line));
						if((code[1]->res->text)[1] != '_')
							if(code[1]->res->atribuicao)
								temp1 = atof(toNumeral(code[1]->res->atribuicao));
							else
								temp1 = atof(toNumeral(code[1]->res->text));
						else
							temp1 = (float)((code[1]->res->line));

						resposta = temp*temp1;
						return makeBinOp(code[0],code[1],TAC_MUL, resposta);
						break; 

		case AST_DIV: 	if((code[0]->res->text)[0] != '_')
							if(code[0]->res->atribuicao)
								temp = atof(toNumeral(code[0]->res->atribuicao));
							else
								temp = atof(toNumeral(code[0]->res->text));
						else
							temp = (float)((code[0]->res->line));
						if((code[1]->res->text)[1] != '_')
							if(code[1]->res->atribuicao)
								temp1 = atof(toNumeral(code[1]->res->atribuicao));
							else
								temp1 = atof(toNumeral(code[1]->res->text));
						else
							temp1 = (float)((code[1]->res->line));

						resposta = temp/temp1;
						return makeBinOp(code[0],code[1],TAC_DIV, resposta);
						break; 

		case AST_LESS: 		return makeBinOp(code[0],code[1],TAC_L, resposta);break;
		case AST_GREATER: 	return makeBinOp(code[0],code[1],TAC_G, resposta);break;
		case AST_LE: 		return makeBinOp(code[0],code[1],TAC_LE, resposta);break;
		case AST_GE: 		return makeBinOp(code[0],code[1],TAC_GE, resposta);break; 
		case AST_EQUAL: 	return makeBinOp(code[0],code[1],TAC_EQ, resposta);break; 
		case AST_AND: 		return makeBinOp(code[0],code[1],TAC_AND, resposta);break; 
		case AST_OR: 		return makeBinOp(code[0],code[1],TAC_OR, resposta);break; 
		case AST_NEG: 		result = tacJoin(tacJoin(code[0], code[1]),tacCreate(TAC_NOT, makeTemp(0, resposta), code[0]?code[0]->res:0, code[1]?code[1]->res:0)); break; 

		case AST_ATRIBUICAO_VEC: result = tacJoin(tacJoin(tacJoin(code[0],code[1]), code[2]),tacCreate(TAC_VEC_WRITE,code[0]?code[0]->res:0,code[1]?code[1]->res:0,code[2]?code[2]->res:0)); break;
		case AST_ATRIBUICAO: result = tacJoin(tacJoin(code[0],code[1]),tacCreate(TAC_MOVE,code[0]?code[0]->res:0,code[1]?code[1]->res:0,0)); break;
		case AST_READ: 		result = tacCreate(TAC_READ, node->son[0]->symbol, 0, 0); break;
		case AST_RETURN: 	result = tacJoin(code[0], tacCreate(TAC_RET, code[0]?code[0]->res:0, 0, 0)); break;
		case AST_IF: 		result = makeIfThenElse(code[0], code[1], code[2]); break;

		case AST_LOOP: 		result = makeWhile(code[0], code[1]); break;
		case AST_PRINTLIST: 		result = tacJoin(tacJoin(code[0], tacCreate(TAC_PRINT, code[0]?code[0]->res:0, 0, 0)), code[1]); break;

		case AST_CALL_FUNCTION: return tacJoin(code[1], tacCreate(TAC_FUNCALL,makeTemp(node->son[0]->symbol->datatype, resposta),node->son[0]->symbol,0)); break;
		case AST_LISTEXP: return tacJoin(tacJoin(code[0],tacCreate(TAC_ARG,node->son[0]->symbol,0,0)),code[1]); break;

		case AST_FUNCAO: 	result = makeFunction(tacCreate(TAC_SYMBOL, node->son[0]->son[1]->symbol, 0, 0), code[0], code[1]); break;
		case AST_PARAM: 	result = tacCreate(TAC_PARPOP, node->son[1]->symbol, 0, 0); break;
		case AST_LISTPARAM: 		result = tacJoin(code[0], code[1]); break;
		//case AST_LISTPARAM: result = tacJoin(tacJoin(code[0],tacCreate(TAC_PARPOP,node->son[0]->symbol,0,0)),code[1]); break;

		case AST_LISTLITERAIS: return tacJoin(tacJoin(code[0],tacCreate(TAC_LISTLITERAIS,node->son[0]->symbol,0,0)),code[1]); break;
		//case AST_LISTLITERAIS: 	result = tacJoin(code[0], code[1]); break;
		case AST_VEC_DEC_INIT:	result = tacJoin(tacCreate(TAC_VEC_DEC_INIT,code[1]?code[1]->res:0,code[2]?code[2]->res:0,0), code[3]); break;
		
		default: result = tacJoin(tacJoin(tacJoin(code[0], code[1]), code[2]), code[3]) ; break;
	}

	return result;
}

TAC* makeWhile(TAC* code0, TAC* code1)
{
	TAC* whileTAC;
	TAC *whileLabelTAC, *jumpLabelTAC;

	Node *newWhileLabel = makeLabel(), *newJumpLabel = makeLabel();
	TAC* jumpTAC = tacCreate(TAC_JUMP, newWhileLabel, 0, 0);
	whileTAC = tacCreate(TAC_IFZ, newJumpLabel, code0?code0->res:0, 0);

	whileLabelTAC = tacCreate(TAC_LABEL, newWhileLabel, 0, 0);
	jumpLabelTAC = tacCreate(TAC_LABEL, newJumpLabel, 0, 0);	

	return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(whileLabelTAC, code0), whileTAC), code1),jumpTAC), jumpLabelTAC);
}

TAC* makeBinOp(TAC* result0, TAC* result1, int type, float resposta)
{
	if(result0->res->datatype == SYMBOL_DATATYPE_FLOAT)
	{
		return tacJoin(tacJoin(result0,result1),tacCreate(type,makeTemp(SYMBOL_DATATYPE_FLOAT, resposta),result0?result0->res:0,result1?result1->res:0));
	}
	else
		return tacJoin(tacJoin(result0,result1),tacCreate(type,makeTemp(SYMBOL_DATATYPE_INT, resposta),result0?result0->res:0,result1?result1->res:0));
}

TAC* makeFunction(TAC* symbol, TAC* par, TAC* code)
{
	return tacJoin(tacJoin(tacJoin( tacCreate(TAC_BEGINFUN, symbol->res, 0, 0), par) , code ), tacCreate(TAC_ENDFUN, symbol->res, 0, 0));
}

TAC* makeIfThenElse(TAC* code0, TAC* code1, TAC* code2)
{
	TAC* ifTAC;
	TAC* jumpTAC;
	TAC* ifLabelTAC;
	TAC* elseLabelTAC = 0;
	
	Node* newIfLabel = makeLabel();
	Node* newElseLabel = 0;

	if(code2)
	{
		newElseLabel = makeLabel();		
		jumpTAC = tacCreate	(TAC_JUMP, newElseLabel, 0, 0);
		elseLabelTAC = tacCreate(TAC_LABEL, newElseLabel, 0, 0);
	}
	
	ifTAC = tacCreate(TAC_IFZ, newIfLabel, code0?code0->res:0, 0);
	ifLabelTAC = tacCreate(TAC_LABEL, newIfLabel, 0, 0);

	if(code2) 
		return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(code0, ifTAC), code1), jumpTAC),ifLabelTAC),code2), elseLabelTAC);	
	else 
		return tacJoin(tacJoin(tacJoin(code0, ifTAC), code1),ifLabelTAC);
}
