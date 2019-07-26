/*
 * Guilherme Chaves e Rodrigo Wuerdig
 */
%{

#include <stdio.h>
#include <stdlib.h>

int yyerror(char* msg);
int yylex();
int getLineNumber();
%}

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


%left '<' '>' OPERATOR_LE OPERATOR_GE OPERATOR_EQ OPERATOR_AND OPERATOR_OR OPERATOR_NOT
%left '+' '-'
%left '*' '/'		

%start program

%%


program: listdec
			;

listdec: dec listdec
			|%empty
			;

dec: variavel ';'
		|funcao ';'
		;

tipo: KW_BYTE
		| KW_INT
		| KW_FLOAT
		;

listliterais: literal listliterais
					|%empty
					;

literal: LIT_INTEGER 
				| LIT_FLOAT
				| LIT_CHAR
				;

variavel: tipo TK_IDENTIFIER '=' literal 
			| tipo TK_IDENTIFIER '[' LIT_INTEGER ']' ':' listliterais
			| tipo TK_IDENTIFIER '[' LIT_INTEGER ']'
			;

funcao: cabecalho bloco 
			;

cabecalho: tipo TK_IDENTIFIER '(' listparam ')'
			;

listparam: param listparam
			| ',' param listparam
			| %empty
			;

param: tipo TK_IDENTIFIER
		;

lcmd: cmd ';' lcmd
	| cmd
	;

bloco: '{' lcmd '}'
	;

top_things_to_print: things_to_print 
	| things_to_print ',' top_things_to_print 
	;

things_to_print: LIT_STRING 
	| exp 

cmd :  atribuicao 
	| ifcmd  
	| KW_PRINT top_things_to_print 
    | bloco 
    | KW_RETURN exp 
    | KW_READ TK_IDENTIFIER 
    | loopcmd 
    | KW_LEAP 
    | %empty
    ;

atribuicao: TK_IDENTIFIER '=' exp
	| TK_IDENTIFIER '[' exp ']' '=' exp
	;

ifcmd : KW_IF '(' exp ')' KW_THEN lcmd elsecmd 
      ;

elsecmd : KW_ELSE lcmd 
	| %empty 
	;

loopcmd : KW_LOOP '(' exp ')' lcmd 
		;

exp : LIT_INTEGER 
	| TK_IDENTIFIER '[' exp ']' 
	| TK_IDENTIFIER '(' listexp ')'
	| TK_IDENTIFIER 
    | LIT_CHAR 
    | LIT_FLOAT 
    | '(' exp ')'
    | exp OPERATOR_EQ exp
    | exp OPERATOR_LE exp
	| exp OPERATOR_GE exp
    | exp '+' exp
    | exp '-' exp
    | exp '/' exp
    | exp '*' exp
    | exp '<' exp
    | exp '>' exp
    | OPERATOR_NOT exp
    | exp OPERATOR_AND exp
    | exp OPERATOR_OR exp 
    ;

listexp: exp listexp
			| ',' exp listexp
			| %empty
			;

%%

int yyerror(char* msg)
{
	int linecounter = getLineNumber(); 
	fprintf(stderr, "Syntax error at line %d\n",linecounter);
	exit(3);
}
