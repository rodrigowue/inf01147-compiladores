/*
 * Guilherme Chaves e Rodrigo Wuerdig
 */
%{

#include <stdio.h>
#include <stdlib.h>


	#include "ast.h"
	#include "hash.h"
	#include "semantic.h"	
	#include "tac.h"
	#include "asmgen.h"


extern void printExpression(Ast *ast);
extern Node* hashTable[HASH_SIZE];

int yyerror(char* msg);
int yylex();
int getLineNumber();
int erro=0;
Ast* root;
%}

%union
{
	Node* symbol;
	Ast* ast;
}

%token KW_BYTE       
%token KW_INT        
%token KW_FLOAT      
%token KW_IF         
%token KW_THEN       
%token KW_ELSE       
%token KW_LOOP       
%token KW_LEAP       
%token KW_READ       
%token KW_RETURN     
%token KW_PRINT      
%token OPERATOR_LE   
%token OPERATOR_GE   
%token OPERATOR_EQ   
%token OPERATOR_DIF  
%token OPERATOR_OR   
%token OPERATOR_AND  
%token OPERATOR_NOT  
%token TK_IDENTIFIER 
%token LIT_INTEGER   
%token LIT_FLOAT     
%token LIT_CHAR      
%token LIT_STRING    
%token TOKEN_ERROR   

%type <ast>literal
%type <ast>exp
%type <ast>identificador
%type <ast>atribuicao
%type <ast>listexp
%type <ast>ifcmd
%type <ast>elsecmd
%type <ast>cmd
%type <ast>loopcmd
%type <ast>bloco
%type <ast>lcmd
%type <ast>things_to_print
%type <ast>top_things_to_print
%type <ast>param
%type <ast>tipo
%type <ast>listparam
%type <ast>cabecalho
%type <ast>funcao
%type <ast>variavel
%type <ast>integer
%type <ast>listliterais
%type <ast>dec
%type <ast>listdec
%type <ast>program

%type <symbol>LIT_INTEGER
%type <symbol>TK_IDENTIFIER
%type <symbol>LIT_FLOAT
%type <symbol>LIT_CHAR
%type <symbol>LIT_STRING


%left OPERATOR_OR 
%left OPERATOR_AND
%left OPERATOR_EQ
%left OPERATOR_NOT

%left '<' '>' OPERATOR_LE OPERATOR_GE
%left '+' '-'
%left '*' '/'		

%start program

%%


program: listdec 														{root=$$;
																		TAC *tac;
																		//astPrint(root,0);  
			  															semantic(root);
			  															if(root != 0)
			  																tacPrintFoward(tac = tacReverse(tacGenerate($1)));
			  															generateAsm(tac, "saida.s");
																		}

listdec: dec listdec													{$$=astCreate(AST_LISTDEC,0,$1,$2,0,0);}
			|															{$$=0;}
			;

dec: variavel ';'														{$$=$1;}
		|funcao ';'														{$$=$1;}
		;

tipo: KW_BYTE															{$$=astCreate(AST_BYTE,0,0,0,0,0);}
		| KW_INT 														{$$=astCreate(AST_INT,0,0,0,0,0);}
		| KW_FLOAT 														{$$=astCreate(AST_FLOAT,0,0,0,0,0);}
		;

listliterais: literal listliterais										{$$=astCreate(AST_LISTLITERAIS,0,$1,$2,0,0);}
					|													{$$=0;}
					;

literal: LIT_CHAR												{$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}
	    |LIT_INTEGER 											{$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}
		| LIT_FLOAT												{$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}
		;

integer: LIT_INTEGER 													{$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}
			;

identificador: TK_IDENTIFIER											{$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}
			;

variavel: tipo identificador '=' literal 								{$$=astCreate(AST_VARIAVEL_DEC,0,$1,$2,$4,0);}
			| tipo identificador '[' integer ']' ':' listliterais		{$$=astCreate(AST_VEC_DEC_INIT,0,$1,$2,$4,$7);}
			| tipo identificador '[' integer ']'						{$$=astCreate(AST_VEC_DEC,0,$1,$2,$4,0);}
			;

funcao: cabecalho bloco 												{$$=astCreate(AST_FUNCAO,0,$1,$2,0,0);}
			;

cabecalho: tipo identificador '(' listparam ')'							{$$=astCreate(AST_CABECALHO,0,$1,$2,$4,0);}
			;

listparam: param listparam												{$$=astCreate(AST_LISTPARAM,0,$1,$2,0,0);}
			| ',' param listparam 										{$$=astCreate(AST_LISTPARAM,0,$2,$3,0,0);}
			|															{$$=0;}
			;

param: tipo identificador												{$$=astCreate(AST_PARAM,0,$1,$2,0,0);}
		;

lcmd: cmd ';' lcmd 														{$$=astCreate(AST_LISTCMD,0,$1,$3,0,0);}
	| cmd																{$$=astCreate(AST_LISTCMD,0,$1,0,0,0);}
	;

bloco: '{' lcmd '}'														{$$=astCreate(AST_BLOCO,0,$2,0,0,0);}
	;

top_things_to_print: things_to_print 									{$$=astCreate(AST_PRINTLIST,0,$1,0,0,0);}
	| things_to_print ',' top_things_to_print 							{$$=astCreate(AST_PRINTLIST,0,$1,$3,0,0);}
	;

things_to_print: LIT_STRING 											{$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}
	| exp 																{$$=$1;}

cmd :  atribuicao 														{$$=$1;}
	| KW_PRINT top_things_to_print 										{$$=astCreate(AST_PRINT,0,$2,0,0,0);}
	| ifcmd																{$$=$1;}
    | bloco 															{$$=$1;}
    | KW_RETURN exp 													{$$=astCreate(AST_RETURN,0,$2,0,0,0);}
    | KW_READ identificador 											{$$=astCreate(AST_READ,0,$2,0,0,0);}
    | KW_LEAP 															{$$=astCreate(AST_LEAP,0,0,0,0,0);}
    | loopcmd															{$$=$1;}
    | 																	{$$=0;}
    ;

atribuicao: identificador '=' exp										{$$=astCreate(AST_ATRIBUICAO,0,$1,$3,0,0);}
	| identificador '[' exp ']' '=' exp									{$$=astCreate(AST_ATRIBUICAO_VEC,0,$1,$3,$6,0);}
	;

ifcmd : KW_IF '(' exp ')' KW_THEN cmd elsecmd							{$$=astCreate(AST_IF,0,$3,$6,$7,0);}
      ;

elsecmd : KW_ELSE cmd													{$$=astCreate(AST_ELSE,0,$2,0,0,0);}
	|																	{$$=0;}
	;

loopcmd : KW_LOOP '(' exp ')' cmd										{$$=astCreate(AST_LOOP,0,$3,$5,0,0);}
		;

exp : LIT_CHAR															{$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}	
	| LIT_FLOAT															{$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}	
	| LIT_INTEGER														{$$=astCreate(AST_SYMBOL,$1,0,0,0,0);}
	| identificador '[' exp ']' 										{$$=astCreate(AST_VECTOR,0,$1,$3,0,0);}
	| identificador '(' listexp ')'										{$$=astCreate(AST_CALL_FUNCTION,0,$1,$3,0,0);}
	| identificador 													{$$=$1;}
    | '(' exp ')'														{$$=astCreate(AST_ASSOCIATIVIDADE,0,$2,0,0,0);}
    | exp OPERATOR_EQ exp												{$$=astCreate(AST_EQUAL,0,$1,$3,0,0);}
    | exp OPERATOR_LE exp												{$$=astCreate(AST_LE,0,$1,$3,0,0);}
	| exp OPERATOR_GE exp												{$$=astCreate(AST_GE,0,$1,$3,0,0);}
    | exp '+' exp														{$$=astCreate(AST_ADD,0,$1,$3,0,0);}
    | exp '-' exp														{$$=astCreate(AST_SUB,0,$1,$3,0,0);}
    | exp '/' exp														{$$=astCreate(AST_DIV,0,$1,$3,0,0);}
    | exp '*' exp														{$$=astCreate(AST_MULT,0,$1,$3,0,0);}
    | exp '<' exp														{$$=astCreate(AST_LESS,0,$1,$3,0,0);}
    | exp '>' exp														{$$=astCreate(AST_GREATER,0,$1,$3,0,0);}
    | OPERATOR_NOT exp													{$$=astCreate(AST_NEG,0,$2,0,0,0);}
    | exp OPERATOR_AND exp												{$$=astCreate(AST_AND,0,$1,$3,0,0);}
    | exp OPERATOR_OR exp 												{$$=astCreate(AST_OR,0,$1,$3,0,0);}
    ;

listexp: exp listexp													{$$=astCreate(AST_LISTEXP,0,$1,$2,0,0);}
			| ',' exp listexp 											{$$=astCreate(AST_LISTEXP,0,$2,$3,0,0);}
			|															{$$=0;}
			;

%%
Ast* getAst(){
	return root;
}

int yyerror(char* msg)
{
	int linecounter = getLineNumber(); 
	fprintf(stderr, "ERRO SINTATICO: Na linha %d\n",linecounter);
	exit(3);
}

