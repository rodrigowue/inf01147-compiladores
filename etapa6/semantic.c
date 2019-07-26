#include "semantic.h"

void semantic(Ast *node)
{
	erroSemantico =0;
	funcoes = 0;
	chamadaFuncoes = 0;
	//astPrint(node, 0);
	setAndCheckRedeclared(node);
	checkUndeclared(node);
	checkOperands(node);
	checkCallFunction();
	checkIDType(node);

	if(erroSemantico)
		exit(4);

}



void deleteList(Functions *node)
{
	Functions *current = node;
	Functions *next;

	while(current != 0)
	{
		next = current->next;
		free(current);
		current = next;
	}

	node = 0;
}



void assignDataType(Ast *node)
{
	if(node->son[0]->type == AST_BYTE)
		node->son[1]->symbol->datatype = SYMBOL_DATATYPE_BYTE;
	else if(node->son[0]->type == AST_INT)
		node->son[1]->symbol->datatype = SYMBOL_DATATYPE_INT;
	else
		node->son[1]->symbol->datatype = SYMBOL_DATATYPE_FLOAT;
}



void insertFunctions(Ast *node, int type, char *nome)
{
	Functions *elementoReturn = retornoFuncao;
	Functions *elementoCall = chamadaFuncoes;
	Functions *elemento = funcoes;
	Functions *newNode;
	newNode = (Functions*) calloc(1, sizeof(Functions));
	newNode->listParam = node;
	newNode->nome = nome;
	newNode->next = 0;

	if(type == FUNC_DEC)
	{
		if(funcoes == 0) 
	        funcoes = newNode;
	    else
	    {
	        while(elemento->next != 0)
	            elemento = elemento->next;

	        elemento->next = newNode;
	    }
	}
	else if(type == FUNC_CALL)
	{
		if(chamadaFuncoes == 0) 
	        chamadaFuncoes = newNode;
	    else
	    {
	        while(elementoCall->next != 0)
	            elementoCall = elementoCall->next;

	        elementoCall->next = newNode;
	    }
	}
	else
	{
		if(retornoFuncao == 0) 
	        retornoFuncao = newNode;
	    else
	    {
	        while(elementoReturn->next != 0)
	            elementoReturn = elementoReturn->next;

	        elementoReturn->next = newNode;
	    }
	}
}



void checkCallFunction()
{
	int i, j;

	Functions *call = chamadaFuncoes;
	Functions *func = funcoes;

	int isCallWrong = 0;

	while(call != 0)
	{
		while(func != 0)
		{
			if(strcmp(call->nome, func->nome) == 0)
			{
	 			isCallWrong = checkParameters(call->listParam, func->listParam, call->nome, call->listParam, func->listParam, 0, 0);
				if(isCallWrong != 0)
				{
					fprintf(stderr, "		Funcao -> %s(", func->nome);
					printExpression(func->listParam);
					fprintf(stderr, ")\n");
					fprintf(stderr, "		Call -> %s(", call->nome);
					printExpression(call->listParam);
					fprintf(stderr, ")\n");
				}
				break;
			}
			func = func->next;
		}
		func = funcoes;
		call = call->next;
		isCallWrong = 0;
	}
	deleteList(call);
	deleteList(func);
}



void differentArgumentMessage(Ast *listParamCall, Ast *listParamFunc, char *nome)
{
	fprintf(stderr, "ERRO SEMANTICO: Chamada de Funcao com argumentos nao-compativeis com a declaracao\n");
	fprintf(stderr, "		Funcao -> %s(", nome);
	printExpression(listParamFunc);
	fprintf(stderr, ")\n");
	fprintf(stderr, "		Call -> %s(", nome);
	printExpression(listParamCall);
	fprintf(stderr, ")\n");
	erroSemantico = 1;
}


// variavel "param" serve apenas para separar a recursao inicial das demais. Para nao fazer a chamada do call->son[1] logo no inicio
// variavel "parada" datermina se a chamada de funcao tem menos parametros do que a declaracao da mesma 
int checkParameters(Ast *listParamCall, Ast *listParamFunc, char *nome, Ast *call, Ast *func, int param, int parada)
{
	int i;
	int isCallWrong;

	if((!call && func->type == AST_PARAM) || (!func && call->son[1] && parada) ||
		(!listParamFunc && listParamCall) || (listParamFunc && !listParamCall))
	{
		fprintf(stderr, "ERRO SEMANTICO: Chamada de Funcao com numero de parametros diferente da declaracao\n");
		erroSemantico = 1;
		return 1;
	}


	if(call == 0 || func == 0)
		return 0; 

	if(func->type == AST_PARAM)
	{
		switch(func->son[0]->type)
		{
			case AST_BYTE:
			case AST_INT:
				if(call->son[0]->symbol != 0)
				{
					if(call->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT || call->son[0]->symbol->type == SYMBOL_FLOAT)
						differentArgumentMessage(listParamCall, listParamFunc, nome);
				}
				else
					if(call->son[0]->dataType == AST_DATATYPE_FLUTUANTE || call->son[0]->dataType == AST_DATATYPE_BOOLEANO)
						differentArgumentMessage(listParamCall, listParamFunc, nome);
				break;

			case AST_FLOAT:
				if(call->son[0]->symbol != 0)
				{
					if((call->son[0]->symbol->datatype != 0 &&call->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT) || 
						call->son[0]->symbol->type == SYMBOL_FLOAT)
					{}
					else
						differentArgumentMessage(listParamCall, listParamFunc, nome);
				}
				else
					if(call->son[0]->dataType != AST_DATATYPE_FLUTUANTE)
						differentArgumentMessage(listParamCall, listParamFunc, nome);
				break;
		}
	}

	for(i=0; i<2; i++)
	{
		if(func->type == AST_LISTPARAM && param == 0 && func->son[i] != 0 && func->son[i]->type != AST_LISTPARAM)
			isCallWrong = checkParameters(listParamCall, listParamFunc, nome, call, func->son[i], 1, 1);
		else if(func->type == AST_LISTPARAM && param != 0  && func->son[i] != 0 &&  func->son[i]->type != AST_LISTPARAM)
		{
			isCallWrong = checkParameters(listParamCall, listParamFunc, nome, call->son[1], func->son[i], 1, 1);
			if(call->son[1] != 0)
				call = call->son[1];
		}
		else
			if(func->type != AST_LISTPARAM)
				isCallWrong = checkParameters(listParamCall, listParamFunc, nome, call, func->son[i], 1, 0);
			else 
				isCallWrong = checkParameters(listParamCall, listParamFunc, nome, call, func->son[i], 1, 1);

		if(isCallWrong)
			return 1;
	}

	return 0;
}



void differentReturnMessage(Ast *decFunction, Functions *elementoReturn)
{
	fprintf(stderr, "ERRO SEMANTICO: Retorno de funcao com tipo diferente da declaracao\n");
	fprintf(stderr, "		Funcao -> ");
	printExpression(decFunction);
	fprintf(stderr, "\n");
	fprintf(stderr, "		Retorno -> ");
	printExpression(elementoReturn->listParam);
	fprintf(stderr, "\n");
	erroSemantico = 1;
}



void checkReturn(Ast *decFunction)
{
	Node *node = decFunction->son[1]->symbol;
	Functions *elementoReturn = retornoFuncao;

	while(elementoReturn != 0)
	{
		switch(node->datatype)
		{
			case SYMBOL_DATATYPE_INT:
			case SYMBOL_DATATYPE_BYTE:
				if(elementoReturn->listParam->son[0]->symbol != 0)
				{
					if(elementoReturn->listParam->son[0]->symbol->type == SYMBOL_FLOAT || elementoReturn->listParam->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
					{
						differentReturnMessage(decFunction, elementoReturn);
					}
				}
				else
				{
					if(elementoReturn->listParam->son[0]->dataType == AST_DATATYPE_FLUTUANTE || elementoReturn->listParam->son[0]->dataType == AST_DATATYPE_BOOLEANO)
					{
						differentReturnMessage(decFunction, elementoReturn);
					}
				}
				break;

			case SYMBOL_DATATYPE_FLOAT:
				if(elementoReturn->listParam->son[0]->symbol != 0)
				{
					if(elementoReturn->listParam->son[0]->symbol->type == SYMBOL_FLOAT || elementoReturn->listParam->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
					{}
					else
					{
						differentReturnMessage(decFunction, elementoReturn);
					}
				}
				else
				{
					if(elementoReturn->listParam->son[0]->dataType == AST_DATATYPE_FLUTUANTE)
					{}
					else
					{
						differentReturnMessage(decFunction, elementoReturn);
					}
				}
				break;
		}
		elementoReturn = elementoReturn->next;
	}
	deleteList(elementoReturn);
	retornoFuncao = 0;
}



void setAndCheckRedeclared(Ast *node)
{
	int i;

	if(node==0)
		return;

	for(i=0; i<MAX_SONS; i++)
		setAndCheckRedeclared(node->son[i]);

	switch(node->type)
	{
		case AST_VEC_DEC_INIT: 
		case AST_VEC_DEC:
			if(node->son[1]->symbol->type == SYMBOL_IDENTIFIER)
			{
				node->son[1]->symbol->vector_size = node->son[2]->symbol->text;
				node->son[1]->symbol->type = SYMBOL_VEC;
				assignDataType(node);
			}
			else{
				fprintf(stderr, "ERRO SEMANTICO: Identificador %s redeclarado. Primeira declaracao na linha %d\n", node->son[1]->symbol->text, node->son[1]->symbol->line);
				erroSemantico = 1;
			}
			break;

		case AST_VARIAVEL_DEC:
		case AST_PARAM:
			if(node->type == AST_VARIAVEL_DEC)
			{
				node->son[1]->symbol->atribuicao = node->son[2]->symbol->text;
				node->son[2]->symbol->quantidade = node->son[2]->symbol->quantidade - 1;
			}

			if(node->son[1]->symbol->type == SYMBOL_IDENTIFIER)
			{
				node->son[1]->symbol->type = SYMBOL_VAR;
				assignDataType(node);
			}
			else{
				fprintf(stderr, "ERRO SEMANTICO: Identificador %s redeclarado. Primeira declaracao na linha %d\n", node->son[1]->symbol->text, node->son[1]->symbol->line);
				erroSemantico = 1;
			}
			break;

		case AST_CABECALHO:
			if(node->son[1]->symbol->type == SYMBOL_IDENTIFIER)
			{
				node->son[1]->symbol->type = SYMBOL_FUNC;
				assignDataType(node);
				insertFunctions(node->son[2], FUNC_DEC, node->son[1]->symbol->text);
			}
			else{
				fprintf(stderr, "ERRO SEMANTICO: Identificador %s redeclarado. Primeira declaracao na linha %d\n", node->son[1]->symbol->text, node->son[1]->symbol->line);
				erroSemantico = 1;
			}
			break;	

		case AST_CALL_FUNCTION:
			//insertFunctions(node->son[1], FUNC_CALL, node->son[0]->symbol->text);
			break;
	}
}



void checkOperands(Ast *node)
{
	int i;

	if(node == 0)
		return;

	for(i=0; i<MAX_SONS; i++)
		checkOperands(node->son[i]);

	switch(node->type)
	{
		case AST_DIV:
		case AST_MULT:
		case AST_SUB:
		case AST_ADD:
			if(node->son[0]->type == AST_NEG || (node->son[0]->type == AST_ASSOCIATIVIDADE && node->son[0]->dataType == AST_DATATYPE_BOOLEANO))
			{
				fprintf(stderr, "ERRO SEMANTICO: Primeiro operando, da operacao aritimetica, invalido\n");
				fprintf(stderr, "		-> ");
				printExpression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
				break;
			}
			if(node->son[1]->type == AST_NEG || (node->son[1]->type == AST_ASSOCIATIVIDADE && node->son[1]->dataType == AST_DATATYPE_BOOLEANO))
			{
				fprintf(stderr, "ERRO SEMANTICO: Segundo operando, da operacao aritimetica, invalido\n");
				fprintf(stderr, "		-> ");
				printExpression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
				break;
			}

			if(node->son[0]->type == AST_SUB || node->son[0]->type == AST_DIV || node->son[0]->type == AST_MULT || node->son[0]->type == AST_ADD || 
				node->son[0]->type == AST_ASSOCIATIVIDADE || node->son[1]->type == AST_SUB || node->son[1]->type == AST_DIV || node->son[1]->type == AST_MULT || 
				node->son[1]->type == AST_ADD || node->son[1]->type == AST_ASSOCIATIVIDADE )
			{
				if((node->son[0]->type != AST_ASSOCIATIVIDADE && node->son[0]->dataType == AST_DATATYPE_FLUTUANTE) ||
					(node->son[1]->type != AST_ASSOCIATIVIDADE && node->son[1]->dataType == AST_DATATYPE_FLUTUANTE) ||
					(node->son[0]->type == AST_ASSOCIATIVIDADE && node->son[0]->dataType == AST_DATATYPE_FLUTUANTE) || 
					(node->son[1]->type == AST_ASSOCIATIVIDADE && node->son[1]->dataType == AST_DATATYPE_FLUTUANTE))
				{
					node->dataType = AST_DATATYPE_FLUTUANTE;
					break;
				}				
			}

			if((node->son[0]->symbol != 0 && (node->son[0]->symbol->type == SYMBOL_FLOAT || node->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)) || 
				(node->son[1]->symbol != 0 && (node->son[1]->symbol->type == SYMBOL_FLOAT || node->son[1]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)))
			{	
				node->dataType = AST_DATATYPE_FLUTUANTE;
				break;
			}

			if(node->son[0]->type == AST_VECTOR || node->son[0]->type == AST_CALL_FUNCTION ||
				node->son[1]->type == AST_VECTOR || node->son[1]->type == AST_CALL_FUNCTION)
			{
				if(node->son[0]->dataType == AST_DATATYPE_FLUTUANTE || node->son[1]->dataType == AST_DATATYPE_FLUTUANTE)
				{
					node->dataType = AST_DATATYPE_FLUTUANTE;
					break;
				}
			}

			node->dataType = AST_DATATYPE_INTEIRO;
			break;

		case AST_ASSOCIATIVIDADE:
			if(node->son[0]->symbol != 0 && (node->son[0]->symbol->type == SYMBOL_FLOAT || node->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT))
				node->dataType = AST_DATATYPE_FLUTUANTE;
			else
				node->dataType = node->son[0]->dataType;
			break;

		case AST_VECTOR:
			if(node->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
				node->dataType = AST_DATATYPE_FLUTUANTE;
			else 
				node->dataType = AST_DATATYPE_INTEIRO;
		case AST_ATRIBUICAO_VEC:
			if(node->son[1]->symbol != 0)
			{
				if(node->son[1]->symbol->type == SYMBOL_FLOAT || node->son[1]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
				{
					fprintf(stderr, "ERRO SEMANTICO: Indice do vetor deve ser um valor inteiro\n");
					fprintf(stderr, "		-> ");
					printExpression(node);
					fprintf(stderr, "\n");
					erroSemantico = 1;
				}
			}
			else
			{
				if(node->son[1]->dataType == AST_DATATYPE_FLUTUANTE || node->son[1]->dataType == AST_DATATYPE_BOOLEANO)
				{
					fprintf(stderr, "ERRO SEMANTICO: Indice do vetor deve ser um valor inteiro\n");
					fprintf(stderr, "		-> ");
					printExpression(node);
					fprintf(stderr, "\n");
					erroSemantico = 1;
				}
			}

			break;

		case AST_CALL_FUNCTION:
			if(node->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
				node->dataType = AST_DATATYPE_FLUTUANTE;
			else 
				node->dataType = AST_DATATYPE_INTEIRO;
			break;

		case AST_EQUAL:
		case AST_LE:
		case AST_GE:
		case AST_LESS:
		case AST_GREATER:
		case AST_AND:
		case AST_OR:
		case AST_NEG:
			node->dataType = AST_DATATYPE_BOOLEANO;
			break;

		case AST_FUNCAO:
			if(retornoFuncao != 0)
				checkReturn(node->son[0]);
			break;

		case AST_RETURN:
			insertFunctions(node, FUNC_RETURN, "");
			break;
	}
}



void checkUndeclared(Ast *node)
{
	int i;

	if(node==0)
		return;

	if(node->symbol == 0){

	}
	else if(node->symbol->type == SYMBOL_IDENTIFIER)
	{
		fprintf(stderr, "ERRO SEMANTICO: Identificador %s nao-declarado na linha %d\n", node->symbol->text, node->symbol->line);
		erroSemantico = 1;
	}

	for(i=0; i<MAX_SONS; i++)
		checkUndeclared(node->son[i]);
}



int checkListLiterais(Ast *node, int dataType)
{
	int erro = 0;

	if(node->son[1] != 0 && node->son[1]->symbol != 0)
	{
		if(node->son[1]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
			erro = checkListLiterais(node->son[3], SYMBOL_DATATYPE_FLOAT);
		else
			erro = checkListLiterais(node->son[3], SYMBOL_DATATYPE_INT);
	}

	if(dataType == SYMBOL_DATATYPE_INT)
	{
		if(node->son[0] != 0 && node->son[0]->symbol != 0 && node->son[0]->symbol->type == SYMBOL_FLOAT)
		{
			fprintf(stderr, "ERRO SEMANTICO: Vetor deve receber um valor inteiro\n");
			fprintf(stderr, "		-> ");
			erroSemantico = 1;
			erro = 1;
		}
	}
	else
	{
		if(node->son[0] != 0 && node->son[0]->symbol != 0 && node->son[0]->symbol->type != SYMBOL_FLOAT)
		{
			fprintf(stderr, "ERRO SEMANTICO: Vetor deve receber um valor flutuante\n");
			fprintf(stderr, "		-> ");
			erroSemantico = 1;
			erro = 1;
		}
	}

	if(erro == 0 && node->son[1] != 0)
		erro = checkListLiterais(node->son[1], dataType);

	return erro;
}



void checkIDType(Ast *node)
{
	int i;
	Ast *listLiterais;

	if(node == 0)
		return;

	for(i=0; i<MAX_SONS; i++)
		checkIDType(node->son[i]);

	if(node->type != AST_VECTOR && node->type != AST_VEC_DEC_INIT && node->type != AST_VEC_DEC && node->type != AST_ATRIBUICAO_VEC)
	{
		if((node->son[0] != 0 && node->son[0]->symbol != 0 &&  node->son[0]->symbol->type == SYMBOL_VEC) ||
			(node->son[1] != 0 && node->son[1]->symbol != 0 &&  node->son[1]->symbol->type == SYMBOL_VEC) || 
			(node->son[2] != 0 && node->son[2]->symbol != 0 &&  node->son[2]->symbol->type == SYMBOL_VEC) ||
			(node->son[3] != 0 && node->son[3]->symbol != 0 &&  node->son[3]->symbol->type == SYMBOL_VEC))
		{
			fprintf(stderr, "ERRO SEMANTICO: Vetor precisa ser indexado\n");
			fprintf(stderr, "		-> ");
			printExpression(node);
			fprintf(stderr, "\n");
			erroSemantico = 1;
		}
	}
	else
	{
		if(node->son[0]->symbol != 0 &&  node->son[0]->symbol->type != SYMBOL_VEC)
		{
			fprintf(stderr, "ERRO SEMANTICO: Variavel nao pode ser indexada\n");
			fprintf(stderr, "		-> ");
			printExpression(node);
			fprintf(stderr, "\n");
			erroSemantico = 1;
		}
	}

	if(node->type != AST_CABECALHO && node->type != AST_CALL_FUNCTION)
	{
		if((node->son[0] != 0 && node->son[0]->symbol != 0 &&  node->son[0]->symbol->type == SYMBOL_FUNC) ||
			(node->son[1] != 0 && node->son[1]->symbol != 0 &&  node->son[1]->symbol->type == SYMBOL_FUNC) || 
			(node->son[2] != 0 && node->son[2]->symbol != 0 &&  node->son[2]->symbol->type == SYMBOL_FUNC) ||
			(node->son[3] != 0 && node->son[3]->symbol != 0 &&  node->son[3]->symbol->type == SYMBOL_FUNC))
		{
			fprintf(stderr, "ERRO SEMANTICO: Funcoes apenas devem ser usadas com chamadas\n");
			fprintf(stderr, "		-> ");
			printExpression(node);
			fprintf(stderr, "\n");
			erroSemantico = 1;
		}
	}
	else
	{
		if(node->son[0]->symbol != 0 &&  node->son[0]->symbol->type != SYMBOL_FUNC && node->son[0]->symbol->type != SYMBOL_VEC)
		{
			fprintf(stderr, "ERRO SEMANTICO: Variavel nao pode ser usada como funcao\n");
			fprintf(stderr, "		-> ");
			printExpression(node);
			fprintf(stderr, "\n");
			erroSemantico = 1;
		}
	}

	if(node->type == AST_ATRIBUICAO)
	{
		if(node->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
		{
			if(node->son[1]->symbol != 0 && (node->son[1]->symbol->datatype == SYMBOL_DATATYPE_FLOAT || node->son[1]->symbol->type == SYMBOL_FLOAT))
			{}
			else if(node->son[1]->symbol == 0 && node->son[1]->dataType == AST_DATATYPE_FLUTUANTE)
			{}
			else
			{
				fprintf(stderr, "ERRO SEMANTICO: Variavel deve receber um valor flutuante\n");
				fprintf(stderr, "		-> ");
				printExpression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
			}
		}
		else
		{
			if((node->son[1]->symbol != 0 && (node->son[1]->symbol->datatype == SYMBOL_DATATYPE_FLOAT || node->son[1]->symbol->type == SYMBOL_FLOAT)) ||
				(node->son[1]->symbol == 0 && (node->son[1]->dataType == AST_DATATYPE_FLUTUANTE || node->son[1]->dataType == AST_DATATYPE_BOOLEANO)))
			{
				fprintf(stderr, "ERRO SEMANTICO: Variavel deve receber um valor inteiro\n");
				fprintf(stderr, "		-> ");
				printExpression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
			}
		}
	}
	else if(node->type == AST_ATRIBUICAO_VEC)
	{
		if(node->son[0]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
		{
			if(node->son[2]->symbol != 0 && (node->son[2]->symbol->datatype == SYMBOL_DATATYPE_FLOAT || node->son[2]->symbol->type == SYMBOL_FLOAT))
			{}
			else if(node->son[2]->symbol == 0 && node->son[2]->dataType == AST_DATATYPE_FLUTUANTE)
			{}
			else
			{
				fprintf(stderr, "ERRO SEMANTICO: Variavel deve receber um valor flutuante\n");
				fprintf(stderr, "		-> ");
				printExpression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
			}
		}
		else
		{
			if((node->son[2]->symbol != 0 && (node->son[2]->symbol->datatype == SYMBOL_DATATYPE_FLOAT || node->son[2]->symbol->type == SYMBOL_FLOAT)) ||
				(node->son[2]->symbol == 0 && (node->son[2]->dataType == AST_DATATYPE_FLUTUANTE || node->son[2]->dataType == AST_DATATYPE_BOOLEANO)))
			{
				fprintf(stderr, "ERRO SEMANTICO: Variavel deve receber um valor inteiro\n");
				fprintf(stderr, "		-> ");
				printExpression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
			}
		}
	}
	else if(node->type == AST_VARIAVEL_DEC)
	{
		if(node->son[1]->symbol->datatype == SYMBOL_DATATYPE_FLOAT)
		{
			if(node->son[2]->symbol->type == SYMBOL_FLOAT)
			{}
			else
			{
				fprintf(stderr, "ERRO SEMANTICO: Variavel deve receber um valor flutuante\n");
				fprintf(stderr, "		-> ");
				printExpression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
			}
		}
		else
		{
			if(node->son[2]->symbol->type == SYMBOL_FLOAT)
			{
				fprintf(stderr, "ERRO SEMANTICO: Variavel deve receber um valor inteiro\n");
				fprintf(stderr, "		-> ");
				printExpression(node);
				fprintf(stderr, "\n");
				erroSemantico = 1;
			}
		}
	}
	else if(node->type == AST_VEC_DEC_INIT)
	{
		if(checkListLiterais(node, 0))
		{
			printExpression(node);
			fprintf(stderr, "\n");
			erroSemantico = 1;
		}
	}
}



void printExpression(Ast *ast){
  if(!ast) return;
  if(ast){
    //printf("DEBUG-Node Type: %d\n",ast->type);
    switch(ast->type){      
                case AST_SYMBOL:
                    fprintf(stderr,"%s",ast->symbol->text);
                    break;
                case AST_AND:
                    printExpression(ast->son[0]); fprintf(stderr," and "); printExpression(ast->son[1]);
                    break;
                case AST_OR:
                    printExpression(ast->son[0]); fprintf(stderr," or "); printExpression(ast->son[1]);
                    break;
                case AST_ADD:
                    printExpression(ast->son[0]); fprintf(stderr,"+"); printExpression(ast->son[1]);
                    break;
                case AST_SUB:
                    printExpression(ast->son[0]); fprintf(stderr,"-"); printExpression(ast->son[1]);
                    break;
                case AST_MULT:
                    printExpression(ast->son[0]); fprintf(stderr,"*"); printExpression(ast->son[1]);
                    break;
                case AST_DIV:
                    printExpression(ast->son[0]); fprintf(stderr,"/"); printExpression(ast->son[1]);
                    break;
                case AST_LESS:
                    printExpression(ast->son[0]); fprintf(stderr,"<"); printExpression(ast->son[1]);
                    break;
                case AST_GREATER:
                    printExpression(ast->son[0]); fprintf(stderr,">"); printExpression(ast->son[1]);
                    break;
                case AST_LE:
                    printExpression(ast->son[0]); fprintf(stderr,"<="); printExpression(ast->son[1]);
                    break;
                case AST_GE:
                    printExpression(ast->son[0]); fprintf(stderr,">="); printExpression(ast->son[1]);
                    break;
                case AST_EQUAL:
                    printExpression(ast->son[0]); fprintf(stderr,"=="); printExpression(ast->son[1]);
                    break;
                case AST_NEG:
                    fprintf(stderr," not");printExpression(ast->son[0]);  
                    break;
                case AST_READ:
                    fprintf(stderr,"read ");printExpression(ast->son[0]);
                    break;
                case AST_RETURN:
                    fprintf(stderr,"return "); printExpression(ast->son[0]);
                    break;
                case AST_LOOP:
                    fprintf(stderr,"loop ("); printExpression(ast->son[0]); fprintf(stderr,")"); printExpression(ast->son[1]);
                    break;
                case AST_LEAP:
                    fprintf(stderr,"leap");
                    break;
                case AST_CALL_FUNCTION:
                    printExpression(ast->son[0]); fprintf(stderr,"("); printExpression(ast->son[1]); fprintf(stderr,")");
                    break;
                case AST_ASSOCIATIVIDADE:
                    fprintf(stderr,"("); printExpression(ast->son[0]); fprintf(stderr,")");
                    break;
                case AST_BLOCO:
                    fprintf(stderr,"{\n"); fprintf(stderr," "); printExpression(ast->son[0]); fprintf(stderr,"}");
                    break;
                case AST_PARAM:
                    printExpression(ast->son[0]); printExpression(ast->son[1]);
                    break; 
                case AST_ATRIBUICAO:
                    printExpression(ast->son[0]); fprintf(stderr,"="); printExpression(ast->son[1]);
                    break;
                case AST_VECTOR:
                    printExpression(ast->son[0]); fprintf(stderr,"["); printExpression(ast->son[1]); fprintf(stderr,"]");
                    break;
                case AST_PRINT:
                    fprintf(stderr,"print "); printExpression(ast->son[0]); 
                    break;
                case AST_PRINTLIST:
                    if(ast->son[1] != NULL){
                          printExpression(ast->son[0]); fprintf(stderr,","); printExpression(ast->son[1]);
                    }else
                        printExpression(ast->son[0]);
                    break;
                case AST_LISTCMD:
                    if(ast->son[1] != NULL){
                          printExpression(ast->son[0]); fprintf(stderr,";\n"); printExpression(ast->son[1]);
                    }else
                          printExpression(ast->son[0]);
                    break;
                
                case AST_LISTDEC:    
                  if(ast->son[1] != NULL){
                            printExpression(ast->son[0]); fprintf(stderr,"\n"); printExpression(ast->son[1]);
                        }
                  else
                            printExpression(ast->son[0]);
        
                    break;
                case AST_LISTPARAM:
                    if(ast->son[1] != NULL){
                          printExpression(ast->son[0]); fprintf(stderr,","); printExpression(ast->son[1]);
                    }else
                          printExpression(ast->son[0]);
                    break;
                case AST_LISTLITERAIS:
                  if(ast->son[1] != NULL){
                          printExpression(ast->son[0]); fprintf(stderr," "); printExpression(ast->son[1]);
                    }else
                          printExpression(ast->son[0]);
                    break;
                case AST_VARIAVEL_DEC:
                    printExpression(ast->son[0]); printExpression(ast->son[1]); fprintf(stderr,"="); printExpression(ast->son[2]);fprintf(stderr,";");
                    break;
                case AST_BYTE:
                    fprintf(stderr,"byte "); 
                    break;
                case AST_INT:
                    fprintf(stderr,"int "); 
                    break;
                case AST_CABECALHO:
                     printExpression(ast->son[0]); printExpression(ast->son[1]);fprintf(stderr,"(");printExpression(ast->son[2]);fprintf(stderr,")");
                    break;
                case AST_FUNCAO:
                     printExpression(ast->son[0]); printExpression(ast->son[1]); fprintf(stderr,";");
                    break;
                case AST_LISTEXP:
                    if(ast->son[1] != NULL){
                    printExpression(ast->son[0]); fprintf(stderr,","); printExpression(ast->son[1]);;
                    }else
                          printExpression(ast->son[0]);
                    break;
                case AST_FLOAT:
                    fprintf(stderr,"float ");
                    break;
                case AST_ATRIBUICAO_VEC:
                    printExpression(ast->son[0]);fprintf(stderr,"[");printExpression(ast->son[1]);fprintf(stderr,"]");fprintf(stderr,"=");printExpression(ast->son[2]);
                    break;
                case AST_IF:
                    fprintf(stderr,"if ("); printExpression(ast->son[0]); fprintf(stderr,") then ");fprintf(stderr," \n"); printExpression(ast->son[1]);
                    if(ast->son[2] != NULL){
                    	printExpression(ast->son[2]);
                    }
                    break;
                case AST_ELSE:
                    fprintf(stderr," else \n"); printExpression(ast->son[0]);
                    break;  
                case AST_VEC_DEC:
                    printExpression(ast->son[0]); printExpression(ast->son[1]); fprintf(stderr,"[");  printExpression(ast->son[2]); fprintf(stderr,"]");
                    fprintf(stderr,";");
                    break;
                case AST_VEC_DEC_INIT:
                    printExpression(ast->son[0]); printExpression(ast->son[1]); fprintf(stderr,"[");  printExpression(ast->son[2]); fprintf(stderr,"]");
                    if(ast->son[3]!=NULL)
                    fprintf(stderr,":"); printExpression(ast->son[3]);
                    // fprintf(stderr,";");
                    fprintf(stderr,";");
                    break;
                default:
                  printf("ERRO\n");
                break;
    }
  }
  return;
}