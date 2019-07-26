%option nounput yylineno

%{

#include "main.c"
#include "y.tab.h"
#include "hash.c"

int lineNumbers = 1;
int running = 1;
void initMe()
{
	 
}

int getLineNumber()
{
	if(yylineno - lineNumbers == 0)
		return yylineno;
	return lineNumbers;
}

int isRunning()
{
	return running;
} 

%}

%x COMMENT

%%

ZZ.*							
XXX								{BEGIN(COMMENT);}
<COMMENT>.
<COMMENT>XXX					{BEGIN(INITIAL);}
<COMMENT>\n						{lineNumbers = yylineno;}			
"byte"							{return KW_BYTE;}
"int"							{return KW_INT;}
"float"							{return KW_FLOAT;}
"if"							{return KW_IF;}
"then"							{return KW_THEN;}
"else"							{return KW_ELSE;}
"loop"							{return KW_LOOP;}
"leap"							{return KW_LEAP;}
"or"							{return OPERATOR_OR;}
"and"							{return OPERATOR_AND;}
"not"							{return OPERATOR_NOT;}
"read"							{return KW_READ;}
"print"							{return KW_PRINT;}
"return"						{return KW_RETURN;}
[-,;:)([\]}{*/><=&$#+]			{return yytext[0];}
"<="							{return OPERATOR_LE;}
">="							{return OPERATOR_GE;}
"=="							{return OPERATOR_EQ;}
"!="							{return OPERATOR_DIF;}
[_a-z0-9]+						{lineNumbers = yylineno; insertHash(yytext, 0, getLineNumber());     return TK_IDENTIFIER;}
[A-IO]+							{lineNumbers = yylineno; insertHash(yytext, 0, getLineNumber());     return LIT_INTEGER;}
([A-IO])+"."([A-IO])+			{lineNumbers = yylineno; insertHash(yytext, 0, getLineNumber());     return LIT_FLOAT;}
'.'								{lineNumbers = yylineno; insertHash(yytext, 0, getLineNumber());     return LIT_CHAR;}
\"([^\"]|\n)+\"					{insertHash(yytext, 0, getLineNumber());     return LIT_STRING;}
[ \t]
"\n"							{lineNumbers = yylineno;}
.								{return TOKEN_ERROR;}

%%


int yywrap()
{
	//printTable();
	running = 0;
	return 1;
}













