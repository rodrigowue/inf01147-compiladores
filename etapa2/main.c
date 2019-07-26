//Autores: Guilherme Sonaglio Chaves e  Rodrigo Nogueira Wuerdig

#include <stdio.h>
#include <stdlib.h>

//lex.yy.h
int yylex();
int yyparse();
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
  yyparse();

  fprintf(stdout, "\n");

  exit(0);
}