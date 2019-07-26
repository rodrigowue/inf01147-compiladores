//Autores: Guilherme Sonaglio Chaves e  Rodrigo Nogueira Wuerdig

#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"

//lex.yy.h
int yylex();
extern char *yytext;
extern FILE *yyin;


int isRunning(void);
void initMe(void);

int main(int argc, char** argv)
  {
  FILE *gold = 0;
  int token = 0;
  int answar = 0;
  int nota = 0;
  int i=1;
      fprintf(stderr,"Rodando main do prof. \n");

  if (argc < 1)
    {
    printf("call: ./etapa1 input.txt output.txt \n");
    exit(1);
    }
  if (0==(yyin = fopen(argv[1],"r")))
    {
    printf("Cannot open file %s... \n",argv[1]);
    exit(1);
    }
  initMe();
  while (isRunning())
    {
    token = yylex();
    
    if (!isRunning())
      break;


  switch(token)
  	{
	  	case KW_BYTE: fprintf(stdout,"KW_BYTE "); break;
	  	case KW_INT: fprintf(stdout, "KW_INT "); break;
	  	case KW_FLOAT: fprintf(stdout, "KW_FLOAT "); break;
	  	case KW_IF: fprintf(stdout, "KW_IF "); break;
	  	case KW_THEN: fprintf(stdout, "KW_THEN "); break;
	  	case KW_ELSE: fprintf(stdout, "KW_ELSE "); break;
	  	case KW_LOOP: fprintf(stdout, "KW_LOOP "); break;
	  	case KW_LEAP: fprintf(stdout, "KW_LEAP "); break;
	  	case KW_READ: fprintf(stdout, "KW_READ "); break;
	  	case KW_RETURN: fprintf(stdout, "KW_RETURN "); break;
	  	case KW_PRINT: fprintf(stdout, "KW_PRINT "); break;
	  	case OPERATOR_LE: fprintf(stdout, "OPERATOR_LE "); break;
	  	case OPERATOR_GE: fprintf(stdout, "OPERATOR_GE "); break;
	  	case OPERATOR_EQ: fprintf(stdout, "OPERATOR_EQ "); break;
	  	case OPERATOR_DIF: fprintf(stdout, "OPERATOR_DIF "); break;
	  	case OPERATOR_OR: fprintf(stdout, "OPERATOR_OR "); break;
	  	case OPERATOR_AND: fprintf(stdout, "OPERATOR_AND "); break;
	  	case OPERATOR_NOT: fprintf(stdout, "OPERATOR_NOT "); break;
	  	case TK_IDENTIFIER: fprintf(stdout, "TK_IDENTIFIER "); break;
	  	case LIT_INTEGER: fprintf(stdout, "LIT_INTEGER "); break;
	  	case LIT_FLOAT: fprintf(stdout, "LIT_FLOAT "); break;
	  	case LIT_CHAR: fprintf(stdout, "LIT_CHAR "); break;
	  	case LIT_STRING: fprintf(stdout, "LIT_STRING "); break;
	  	case TOKEN_ERROR: fprintf(stdout, "TOKEN_ERROR "); break;
	  	default: fprintf(stdout, "token<%c> ", token); break;
	}

  }
  fprintf(stdout, "\n");
}