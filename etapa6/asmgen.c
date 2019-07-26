#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include "asmgen.h"


char* append(char* s, char c) {
    
    int len = strlen(s);
    char aux[100];
    int i;

    for(i=0; i<=len; i++)
    {
    	aux[i] = s[i];
    }

    aux[len] = c;
    aux[len+1] = '\0';

    s = aux;

    return s;
}


char* toNumeral(char *valor)
{
	int i = 0;
	char digito = valor[i];
	char algaritmo;
	char *final = 0;
	char aux[1];

	//char *final = calloc(1, sizeof(char*));

	while(digito != '\0')
	{
		switch(digito)
		{
			case 'I': algaritmo = '1'; break;
			case 'O': algaritmo = '0'; break;
			case 'H': algaritmo = '2'; break;
			case 'G': algaritmo = '3'; break;
			case 'F': algaritmo = '4'; break;
			case 'E': algaritmo = '5'; break;
			case 'D': algaritmo = '6'; break;
			case 'C': algaritmo = '7'; break;
			case 'B': algaritmo = '8'; break;
			case 'A': algaritmo = '9'; break;
			case '.': algaritmo = '.'; break;
		}

		if(!final)
		{
			aux[0] = algaritmo;
			final = strdup(aux);
		}
		else
			final = strdup(append(final, algaritmo));

		digito = valor[++i];
	}

	//printf("%s\n", final);

	return final;
}



void generateAsm(TAC* tac, char* file)
{
	int i;
	int arg = 0;
	int param = 0;
	int not = 0;
	int lc = 4;
	int aux_eq = 0;

	const char *tempChar;
	FILE *fout;
	fout = fopen(file, "w");

	if(!fout)
	{
		fprintf(stderr, "Nao foi possivel abrir o arquivo %s para escrita\n", file);
		exit(5);
	}


	fprintf(fout, "## VARIAVEIS -> TABELA HASH\n");
	fprintf(fout, ".data\n");

	fprintf(fout, "UM:	.long	1065353216\n");

	for(i=0; i<HASH_SIZE; i++)
    {
        Node *elemento = hashTable[i];

        if(elemento != NULL)
        {
        	//printf("%s\t%d\t%d\n", elemento->text, elemento->datatype, elemento->type);
            do{
            	if(elemento->type != SYMBOL_FUNC)
            	{
                	if(elemento->type < 5 && elemento->quantidade >= 0)
                	{
                		if(elemento->datatype == SYMBOL_DATATYPE_FLOAT || elemento->type == SYMBOL_FLOAT)
                		{
                			float temp = atof((const char*)toNumeral(elemento->text));
                			fprintf(fout, "..%s: .long	 %d\n", elemento->text, *((int*)(&temp)));   //*((int*)(&a))
                		}
                		else if(elemento && elemento->type == SYMBOL_CHAR)
                		{
                			fprintf(fout, "..%s: .long	 %d\n", (elemento->text), (elemento->text)[1]);
                		}
                		else if(elemento && elemento->type == SYMBOL_STRING)
                		{
                			fprintf(fout, ".LC%d: .string	%s\n", lc, elemento->text);
                			fprintf(fout, "..%cS%d: .quad	.LC%d\n", (elemento->text)[1], lc, lc);
                			elemento->lc = lc;
                			lc++;
                		}
                		else{
                			char *teste = toNumeral(elemento->text);
                			fprintf(fout, "..%s: .long	 %s\n", elemento->text, teste);////////////////////////
                		}
                	}
                	else if(elemento->type >= 5 && elemento->atribuicao != 0)
                	{
                		if(elemento->datatype == SYMBOL_DATATYPE_FLOAT || elemento->type == SYMBOL_FLOAT)
                		{
                			float temp = atof((const char*)toNumeral(elemento->atribuicao));
                			fprintf(fout, "..%s: .long	 %d\n", elemento->text, *((int*)(&temp)));   //*((int*)(&a))
                		}
                		else{
                			char *teste = toNumeral(elemento->atribuicao);
                			fprintf(fout, "..%s: .long	 %s\n", elemento->text, teste);
                		}
                	}
                	else if(elemento->type == SYMBOL_VEC)
                	{
                		int temp = atoi((const char*)toNumeral(elemento->vector_size));
                		fprintf(fout, ".comm	..%s,%d\n", elemento->text, temp*4);
                	}
                	else if(elemento->type > 5 && elemento->atribuicao == 0)
                	{
                		fprintf(fout, "..%s: .long	 0\n", (elemento->text));
                	}
            	}
                
                elemento = elemento->next;
            }while(elemento != NULL);
        }
    }



	fprintf(fout, ".LC0:\t.string	 \"%%f\\n\"\n");
	fprintf(fout, ".LC1:\t.string	 \"%%c\\n\"\n");
	fprintf(fout, ".LC2:\t.string	 \"%%d\\n\"\n");

	fprintf(fout, ".R0:\t.string	 \"%%f\"\n");
	fprintf(fout, ".R1:\t.string	 \"%%c\"\n");
	fprintf(fout, ".R2:\t.string	 \"%%d\"\n");


	int temp;
 
	for( ; tac; tac = tac->next)
	{
		switch(tac->type)
		{

			case TAC_PRINT: fprintf(fout, "## PRINT\n");

							if(tac->res && tac->res->datatype == SYMBOL_DATATYPE_FLOAT || tac->res->type == SYMBOL_FLOAT)
							{
								fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
										"\tcvtss2sd	%%xmm0, %%xmm0\n"
										"\tmovl	$.LC0, %%edi\n"
										"\tmovl	$1, %%eax\n"
										"\tcall	printf\n",
									tac->res?tac->res->text:"");
							}
							else if(tac->res && tac->res->type == SYMBOL_CHAR)
							{
								fprintf(fout, "\tmovzbl	..%s(%%rip), %%eax\n"
										"\tmovl	%%eax, %%esi\n"
										"\tmovl	$.LC1, %%edi\n"
										"\tmovl	$1, %%eax\n"
										"\tcall	printf\n",
									tac->res?((tac->res->text)):"");
							}
							else if(tac->res && tac->res->type == SYMBOL_STRING)
							{
								fprintf(fout, "\tmovq	..%cS%d(%%rip), %%rax\n"
										"\tmovq	%%rax, %%rdi\n"
										"\tmovl	$1, %%eax\n"
										"\tcall	puts\n",
									tac->res?((tac->res->text)[1]):0, tac->res?(tac->res->lc):0);
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
										"\tmovl	%%eax, %%esi\n"
										"\tmovl	$.LC2, %%edi\n"
										"\tmovl	$1, %%eax\n"
										"\tcall	printf\n",
									tac->res?tac->res->text:"");
							}
				break;

			case TAC_BEGINFUN: fprintf(fout, "## BEGIN FUNCTION\n"
										".globl	%s\n"
										"%s:\n"
										"\t.cfi_startproc\n"
										"\tpushq	%%rbp\n",
									tac->res?tac->res->text:"", tac->res?tac->res->text:"");
				break;

			case TAC_ENDFUN: fprintf(fout, "## END FUNCTION\n"
										"\tpopq	%%rbp\n"
										"\tret\n"
										"\t.cfi_endproc\n");
				break;

			case TAC_MOVE:	fprintf(fout, "## MOVE\n");
							if(tac->op1 && tac->op1->type == SYMBOL_CHAR)
							{
								fprintf(fout, "\tmovzbl	..%s(%%rip), %%eax\n"
										"\tmovsbl	%%al, %%eax\n"
										"\tmovl	%%eax, ..%s(%%rip)\n", 
									tac->op1?((tac->op1->text)):0, tac->res?tac->res->text:"");
							}
							else if(tac->res && tac->res->datatype == SYMBOL_DATATYPE_FLOAT || tac->res->type == SYMBOL_FLOAT)
							{
								fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
										"\tmovss	%%xmm0, ..%s(%%rip)\n",
										tac->op1?tac->op1->text:"", tac->res?tac->res->text:"");
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
										"\tmovl	%%eax, ..%s(%%rip)\n",
										tac->op1?tac->op1->text:"", tac->res?tac->res->text:"");
							}
				break;

			case TAC_VEC_WRITE: fprintf(fout, "## WRITE VECTOR\n");
								
								temp = tac->op1?(int)((tac->op1->line)*4):0;

								if((tac->op1->text)[0] != '_')
									temp = tac->op1?((atoi(toNumeral(tac->op1->text)))*4):0;
																
								if(tac->res && (tac->res->datatype == SYMBOL_DATATYPE_FLOAT || tac->res->type == SYMBOL_FLOAT)) 
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovss	%%xmm0, ..%s+%d(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->res?tac->res->text:"", 
											temp); //*((int*)(&a))
								}
								else if(tac->op2 && tac->op2->type == SYMBOL_CHAR)
								{
									fprintf(fout, "\tmovzbl	..%s(%%rip), %%eax\n"
											"\tmovsbl	%%al, %%eax\n"
											"\tmovl	%%eax, ..%s+%d(%%rip)\n", 
											tac->op2?((tac->op2->text)):0, tac->res?tac->res->text:"",
											temp);
								}
								else
								{
									fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tmovl	%%eax, ..%s+%d(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->res?tac->res->text:"",
											temp);
								}

				break;

			case TAC_VEC_READ: 	fprintf(fout, "## READ VECTOR\n");

								temp = tac->op2?(int)((tac->op2->line)*4):0;

								if((tac->op2->text)[0] != '_')
									temp = tac->op2?((atoi(toNumeral(tac->op2->text)))*4):0;

								if(tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)) 
								{
									fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");
									fprintf(fout, "\tmovss	..%s+%d(%%rip), %%xmm0\n"
											"\tmovss	%%xmm0, ..%s(%%rip)\n",
											tac->op1?tac->op1->text:"", temp, 
											tac->res?tac->res->text:""); //*((int*)(&a))
								}
								else
								{
									fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");
									fprintf(fout, "\tmovl	..%s+%d(%%rip), %%eax\n"
										"\tmovl	%%eax, ..%s(%%rip)\n",
										tac->op1?tac->op1->text:"", temp,
										tac->res?tac->res->text:"");
								}
				break;

			case TAC_VEC_DEC_INIT: fprintf(fout, "..%s:\n", tac->res?tac->res->text:"");
				break;

			case TAC_LISTLITERAIS: 	if(tac->res && (tac->res->datatype == SYMBOL_DATATYPE_FLOAT || tac->res->type == SYMBOL_FLOAT))
									{
										float temp = atof((const char*)toNumeral(tac->res->text));
										fprintf(fout, "\t.long %d\n", tac->res?*((int*)(&temp)):0);
									}
									else
									{
										fprintf(fout, "\t.long %d\n", tac->res?atoi((const char*)toNumeral(tac->res->text)):0);
									}
				break;

			case TAC_ADD:	fprintf(fout, "## SOMA\n");
							fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");

							if(tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)) 
							{
								if(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT))
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm1\n"
											"\tmovss	..%s(%%rip), %%xmm0\n"
											"\taddss	%%xmm1, %%xmm0\n"
											"\tmovss	%%xmm0, ..%s(%%rip)\n",
											tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
											tac->res?tac->res->text:"");
									tac->res->datatype = SYMBOL_DATATYPE_FLOAT;
								}
								else
								{
									fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n\n"
											"\tpxor	%%xmm0, %%xmm0\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\taddss	%%xmm1, %%xmm0\n"
											"\tmovss	%%xmm0, ..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
									tac->res->datatype = SYMBOL_DATATYPE_FLOAT;
								}
							}
							else if(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)) 
							{

								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
										"\tpxor	%%xmm0, %%xmm0\n"
										"\tcvtsi2ss	%%eax, %%xmm0\n"
										"\tmovss	..%s(%%rip), %%xmm1\n"
										"\taddss	%%xmm1, %%xmm0\n"
										"\tmovss	%%xmm0, ..%s(%%rip)\n",
										tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
										tac->res?tac->res->text:"");
								tac->res->datatype = SYMBOL_DATATYPE_FLOAT;
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%edx\n"
									"\tmovl	..%s(%%rip), %%eax\n"
									"\taddl	%%edx, %%eax\n"
									"\tmovl	%%eax, ..%s(%%rip)\n",
									tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
									tac->res?tac->res->text:"");
							}				
				break;

			case TAC_SUB:	fprintf(fout, "## SUBTRACAO\n");
							fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");

							if(tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)) 
							{
								if(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT))
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tsubss	%%xmm1, %%xmm0\n"
											"\tmovss	%%xmm0, ..%s(%%rip)\n",
											tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
											tac->res?tac->res->text:"");
									tac->res->datatype = SYMBOL_DATATYPE_FLOAT;
								}
								else
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm1\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tsubss	%%xmm0, %%xmm1\n"
											"\tmovaps	%%xmm1, %%xmm0\n"
											"\tmovss	%%xmm0, ..%s(%%rip)\n",
											tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
											tac->res?tac->res->text:"");
									tac->res->datatype = SYMBOL_DATATYPE_FLOAT;
								}
							}
							else if(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)) 
							{

								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
										"\tcvtsi2ss	%%eax, %%xmm0\n"
										"\tmovss	..%s(%%rip), %%xmm1\n"
										"\tsubss	%%xmm1, %%xmm0\n"
										"\tmovss	%%xmm0, ..%s(%%rip)\n",
										tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
										tac->res?tac->res->text:"");
								tac->res->datatype = SYMBOL_DATATYPE_FLOAT;
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%edx\n"
										"\tmovl	..%s(%%rip), %%eax\n"
										"\tsubl	%%eax, %%edx\n"
										"\tmovl	%%edx, %%eax\n"
										"\tmovl	%%eax, ..%s(%%rip)\n",
									tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
									tac->res?tac->res->text:"");
							}
				break;

			case TAC_MUL:	fprintf(fout, "## MULTIPLICACAO\n");
							fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");

							if(tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)) 
							{
								if(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT))
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm1\n"
											"\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmulss	%%xmm1, %%xmm0\n"
											"\tmovss	%%xmm0, ..%s(%%rip)\n",
											tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
											tac->res?tac->res->text:"");
									tac->res->datatype = SYMBOL_DATATYPE_FLOAT;
								}
								else
								{
									fprintf(fout, "\tmovl	 ..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tmulss	%%xmm1, %%xmm0\n"
											"\tmovss	%%xmm0, ..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
									tac->res->datatype = SYMBOL_DATATYPE_FLOAT;
								}
							}
							else if(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)) 
							{

								fprintf(fout, "\tmovl	 ..%s(%%rip), %%eax\n"
										"\tcvtsi2ss	%%eax, %%xmm0\n"
										"\tmovss	..%s(%%rip), %%xmm1\n"
										"\tmulss	%%xmm1, %%xmm0\n"
										"\tmovss	%%xmm0, ..%s(%%rip)\n",
										tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
										tac->res?tac->res->text:"");
								tac->res->datatype = SYMBOL_DATATYPE_FLOAT;
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%edx\n"
										"\tmovl	..%s(%%rip), %%eax\n"
										"\timull	%%edx, %%eax\n"
										"\tmovl	%%eax, ..%s(%%rip)\n",
									tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
									tac->res?tac->res->text:"");
							}				
				break;

			case TAC_DIV:	fprintf(fout, "## DIVISAO\n");
							fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");

							if(tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)) 
							{
								if(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT))
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\t\tmovss	..%s(%%rip), %%xmm1\n"
											"\tdivss	%%xmm1, %%xmm0\n"
											"\tmovss	%%xmm0, ..%s(%%rip)\n",
											tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
											tac->res?tac->res->text:"");
									tac->res->datatype = SYMBOL_DATATYPE_FLOAT;
								}
								else
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm1\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tdivss	%%xmm0, %%xmm1\n"
											"\tmovaps	%%xmm1, %%xmm0\n"
											"\tmovss	%%xmm0, ..%s(%%rip)\n",
											tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
											tac->res?tac->res->text:"");
									tac->res->datatype = SYMBOL_DATATYPE_FLOAT;
								}
							}
							else if(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)) 
							{

								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
										"\tcvtsi2ss	%%eax, %%xmm0\n"
										"\tmovss	..%s(%%rip), %%xmm1\n"
										"\tdivss	%%xmm1, %%xmm0\n"
										"\tmovss	%%xmm0, ..%s(%%rip)\n",
										tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
										tac->res?tac->res->text:"");
								tac->res->datatype = SYMBOL_DATATYPE_FLOAT;
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
										"\tmovl	..%s(%%rip), %%ecx\n"
										"\tcltd\n"
										"\tidivl	%%ecx\n"
										"\tmovl	%%eax, ..%s(%%rip)\n",
									tac->op1?tac->op1->text:"", tac->op2?tac->op2->text:"",
									tac->res?tac->res->text:"");
							}				
				break;

			case TAC_LABEL:	fprintf(fout, "## LABEL\n");
							fprintf(fout, ".%s:\n", tac->res?tac->res->text:"");
				break;

			case TAC_L: 	fprintf(fout, "##LESSER\n");
							fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");
							
							if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)) &&
								(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tucomiss	%%xmm1, %%xmm0\n"
											"\tseta 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tucomiss	%%xmm1, %%xmm0\n"
											"\tseta 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else if((tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tucomiss	%%xmm1, %%xmm0\n"
											"\tseta	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tucomiss	%%xmm1, %%xmm0\n"
											"\tseta 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
				break;


			case TAC_G:		fprintf(fout, "##GREATER\n");
							fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");
							
							if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)) &&
								(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tucomiss	%%xmm0, %%xmm1\n"
											"\tseta 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tucomiss	%%xmm0, %%xmm1\n"
											"\tseta 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else if((tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tucomiss	%%xmm0, %%xmm1\n"
											"\tseta 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tucomiss	%%xmm0, %%xmm1\n"
											"\tseta 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
				break;

			case TAC_LE:	fprintf(fout, "##LESS EQUAL\n");
							fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");
							
							if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)) &&
								(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tucomiss	%%xmm1, %%xmm0\n"
											"\tsetae 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tucomiss	%%xmm1, %%xmm0\n"
											"\tsetae 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else if((tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tucomiss	%%xmm1, %%xmm0\n"
											"\tsetae 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tucomiss	%%xmm1, %%xmm0\n"
											"\tsetae 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
				break;

			case TAC_GE:	fprintf(fout, "##GREATER EQUAL\n");
							fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");
							
							if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)) &&
								(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tucomiss	%%xmm0, %%xmm1\n"
											"\tsetae 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tucomiss	%%xmm0, %%xmm1\n"
											"\tsetae 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else if((tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tucomiss	%%xmm0, %%xmm1\n"
											"\tsetae 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tucomiss	%%xmm0, %%xmm1\n"
											"\tsetae 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
				break;

			case TAC_EQ:	fprintf(fout, "##EQUAL\n");
							fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");
							
							if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)) &&
								(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tucomiss	%%xmm0, %%xmm1\n"
											"\tsete 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tucomiss	%%xmm0, %%xmm1\n"
											"\tsete 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else if((tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tucomiss	%%xmm0, %%xmm1\n"
											"\tsete 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tucomiss	%%xmm0, %%xmm1\n"
											"\tsete 	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"");
							}
				break;	

			case TAC_IFZ:	
							if(tac->prev->type == TAC_L)
								fprintf(fout, "\tjbe	.%s\n", tac->res?tac->res->text:"");

							else if(tac->prev->type == TAC_G)
								fprintf(fout, "\tjbe	.%s\n", tac->res?tac->res->text:"");

							else if(tac->prev->type == TAC_LE)
								fprintf(fout, "\tjb	.%s\n", tac->res?tac->res->text:"");

							else if(tac->prev->type == TAC_GE)
								fprintf(fout, "\tjb	.%s\n", tac->res?tac->res->text:"");

							else if(tac->prev->type == TAC_EQ)
								fprintf(fout, "\tjne	.%s\n", tac->res?tac->res->text:"");

							else if(tac->prev->type == TAC_AND)
								fprintf(fout, "\tjz	.%s\n", tac->res?tac->res->text:"");

							else if(tac->prev->type == TAC_OR)
								fprintf(fout, "\tjz	.%s\n", tac->res?tac->res->text:"");

							else if(tac->prev->type == TAC_NOT)
								fprintf(fout, "\tjz	.%s\n", tac->res?tac->res->text:"");

							else if(atoi(toNumeral(tac->op1->text)) == 0)
								fprintf(fout, "\tjmp	.%s\n", tac->res?tac->res->text:"");


				break;

			case TAC_AND:	fprintf(fout, "##AND\n");
							fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");

							if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)) &&
								(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tpand	%%xmm0, %%xmm1\n"
											"\tmovss	%%xmm1, ..%s(%%rip)\n"
											"\tcmp		$0, ..%s(%%rip)\n"
											"\tsetnz	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"", tac->res?tac->res->text:"", tac->res?tac->res->text:"");
							}
							else if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tpand	%%xmm0, %%xmm1\n"
											"\tmovss	%%xmm1, ..%s(%%rip)\n"
											"\tcmp		$0, ..%s(%%rip)\n"
											"\tsetnz	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"", tac->res?tac->res->text:"", tac->res?tac->res->text:"");
							}
							else if((tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tpand	%%xmm0, %%xmm1\n"
											"\tmovss	%%xmm1, ..%s(%%rip)\n"
											"\tcmp		$0, ..%s(%%rip)\n"
											"\tsetnz	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"", tac->res?tac->res->text:"", tac->res?tac->res->text:"");
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tpand	%%xmm0, %%xmm1\n"
											"\tmovss	%%xmm1, ..%s(%%rip)\n"
											"\tcmp		$0, ..%s(%%rip)\n"
											"\tsetnz	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"", tac->res?tac->res->text:"", tac->res?tac->res->text:"");
							}
				break;
    
    		case TAC_OR:	fprintf(fout, "##OR\n");
    						fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");

							if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)) &&
								(tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tpor	%%xmm0, %%xmm1\n"
											"\tmovss	%%xmm1, ..%s(%%rip)\n"
											"\tcmp		$0, ..%s(%%rip)\n"
											"\tsetnz	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"", tac->res?tac->res->text:"", tac->res?tac->res->text:"");
							}
							else if((tac->op1 && (tac->op1->datatype == SYMBOL_DATATYPE_FLOAT || tac->op1->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovss	..%s(%%rip), %%xmm1\n"
											"\tpor	%%xmm0, %%xmm1\n"
											"\tmovss	%%xmm1, ..%s(%%rip)\n"
											"\tcmp		$0, ..%s(%%rip)\n"
											"\tsetnz	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"", tac->res?tac->res->text:"", tac->res?tac->res->text:"");
							}
							else if((tac->op2 && (tac->op2->datatype == SYMBOL_DATATYPE_FLOAT || tac->op2->type == SYMBOL_FLOAT)))
							{
								fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tpor	%%xmm0, %%xmm1\n"
											"\tmovss	%%xmm1, ..%s(%%rip)\n"
											"\tcmp		$0, ..%s(%%rip)\n"
											"\tsetnz	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"", tac->res?tac->res->text:"", tac->res?tac->res->text:"");
							}
							else
							{
								fprintf(fout, "\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm0\n"
											"\tmovl	..%s(%%rip), %%eax\n"
											"\tcvtsi2ss	%%eax, %%xmm1\n"
											"\tpor	%%xmm0, %%xmm1\n"
											"\tmovss	%%xmm1, ..%s(%%rip)\n"
											"\tcmp		$0, ..%s(%%rip)\n"
											"\tsetnz	..%s(%%rip)\n",
											tac->op2?tac->op2->text:"", tac->op1?tac->op1->text:"",
											tac->res?tac->res->text:"", tac->res?tac->res->text:"", tac->res?tac->res->text:"");
							}
				break;

			case TAC_NOT:	fprintf(fout, "##NOT\n");
    						fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");

    						fprintf(fout, "\tmovl 	..%s(%%rip), %%eax\n"
    									"\tcmp 	$0, %%eax\n"
    									"\tjnz	.not_um%d\n"
    									"\tnot	%%eax\n"
										"\tmovl	%%eax, ..%s(%%rip)\n"
										"\tjmp	.fim_not%d\n"
										".not_um%d:\n"
										"\tmovl	$0, ..%s(%%rip)\n"
										".fim_not%d:\n",
										tac->op1?tac->op1->text:"", not, tac->res?tac->res->text:"", not, not, tac->res?tac->res->text:"", not);
    							not++;

				break;

			case TAC_JUMP:	fprintf(fout, "##JUMP\n");
							fprintf(fout, "\tjmp  .%s\n", tac->res?tac->res->text:"");
				break;



			case TAC_ARG:	
							if((tac->res && (tac->res->datatype == SYMBOL_DATATYPE_FLOAT || tac->res->type == SYMBOL_FLOAT)))
							{
								if(arg == 0)
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm0\n", tac->res?tac->res->text:""); 
									arg++;
								}
								else if(arg == 1)
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm1\n", tac->res?tac->res->text:""); 
									arg++;
								}
								else if(arg == 2)
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm2\n", tac->res?tac->res->text:""); 
									arg++;
								}
								else if(arg == 3)
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm3\n", tac->res?tac->res->text:""); 
									arg++;
								}
								else if(arg == 4)
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm4\n", tac->res?tac->res->text:""); 
									arg++;
								}
								else if(arg == 5)
								{
									fprintf(fout, "\tmovss	..%s(%%rip), %%xmm5\n", tac->res?tac->res->text:""); 
									arg = (8+(arg-6)*8)*-1;
								}
							}
							else
							{
								if(arg == 0)
								{
									fprintf(fout, "\tmovl	..%s(%%rip), %%edi\n", tac->res?tac->res->text:""); 
									arg++;
								}
								else if(arg == 1)
								{
									fprintf(fout, "\tmovl	..%s(%%rip), %%esi\n", tac->res?tac->res->text:""); 
									arg++;
								}
								else if(arg == 2)
								{
									fprintf(fout, "\tmovl	..%s(%%rip), %%edx\n", tac->res?tac->res->text:""); 
									arg++;
								}
								else if(arg == 3)
								{
									fprintf(fout, "\tmovl	..%s(%%rip), %%ecx\n", tac->res?tac->res->text:""); 
									arg++;
								}
								else if(arg == 4)
								{
									fprintf(fout, "\tmovl	..%s(%%rip), %%r8d\n", tac->res?tac->res->text:""); 
									arg++;
								}
								else if(arg == 5)
								{
									fprintf(fout, "\tmovl	..%s(%%rip), %%r9d\n", tac->res?tac->res->text:""); 
									arg++;
								}
							}
								/*else
								{
									fprintf(fout, "\tpushq	..%s(%%rip)\n", tac->res?tac->res->text:""); 
									arg++;
								}*/
				break;

			case TAC_PARPOP:	if((tac->res && (tac->res->datatype == SYMBOL_DATATYPE_FLOAT || tac->res->type == SYMBOL_FLOAT)))
								{
									if(param == 0)
									{
										fprintf(fout, "\tmovss	%%xmm0, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
										param++;
									}
									else if(param == 1)
									{
										fprintf(fout, "\tmovss	%%xmm1, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
										param++;
									}
									else if(param == 2)
									{
										fprintf(fout, "\tmovss	%%xmm2, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
										param++;
									}
									else if(param == 3)
									{
										fprintf(fout, "\tmovss	%%xmm3, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
										param++;
									}
									else if(param == 4)
									{
										fprintf(fout, "\tmovss	%%xmm4, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
										param++;
									}
									else if(param == 5)
									{
										fprintf(fout, "\tmovss	%%xmm5, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
										param = 0;
									}
								}
								else
								{
									if(param == 0)
									{
										fprintf(fout, "\tmovl	%%edi, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
										param++;
									}
									else if(param == 1)
									{
										fprintf(fout, "\tmovl	%%esi, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
										param++;
									}
									else if(param == 2)
									{
										fprintf(fout, "\tmovl	%%edx, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
										param++;
									}
									else if(param == 3)
									{
										fprintf(fout, "\tmovl	%%ecx, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
										param++;
									}
									else if(param == 4)
									{
										fprintf(fout, "\tmovl	%%r8d, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
										param++;
									}
									else if(param == 5)
									{
										fprintf(fout, "\tmovl	%%r9d, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
										param = 0;
									}
								}	


								/*else
								{
									fprintf(fout, "\tmovl	%d(%%rbp), %%eax\n", param); 
									fprintf(fout, "\tmovl	%%eax, ..%s(%%rip)\n", tac->res?tac->res->text:""); 
									param = param-8;
								}*/
				break;

			case TAC_RET:	if((tac->res && (tac->res->datatype == SYMBOL_DATATYPE_FLOAT || tac->res->type == SYMBOL_FLOAT)))
								fprintf(fout, "\tmovss 	..%s(%%rip), %%xmm0\n", tac->res?tac->res->text:"");
							else
								fprintf(fout, "\tmovl 	..%s(%%rip), %%eax\n", tac->res?tac->res->text:"");


				break;

			case TAC_FUNCALL:	if((tac->res && (tac->res->datatype == SYMBOL_DATATYPE_FLOAT || tac->res->type == SYMBOL_FLOAT)))
								{
									fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");
									fprintf(fout, "\tcall 	%s\n", tac->op1?tac->op1->text:"");
									fprintf(fout, "\tmovss 	%%xmm0, ..%s(%%rip)\n", tac->res?tac->res->text:"");
								}
								else
								{
									fprintf(fout, ".comm	..%s,4\n", tac->res?tac->res->text:"");
									fprintf(fout, "\tcall 	%s\n", tac->op1?tac->op1->text:"");
									fprintf(fout, "\tmovl 	%%eax, ..%s(%%rip)\n", tac->res?tac->res->text:"");
								}

				break;

			case TAC_READ:	fprintf(fout, "##READ\n");
							if(tac->res && tac->res->datatype == SYMBOL_DATATYPE_FLOAT || tac->res->type == SYMBOL_FLOAT) //lc0
							{
								fprintf(fout, "\tmovl	$..%s, %%esi\n"
										"\tmovl	$.R0, %%edi\n"
										"\tcall	__isoc99_scanf\n",
									tac->res?tac->res->text:"");
							}
							else if(tac->res && tac->res->type == SYMBOL_CHAR)//lc1
							{
								fprintf(fout, "\tmovl	$..%s, %%esi\n"
										"\tmovl	$.R1, %%edi\n"
										"\tcall	__isoc99_scanf\n",
									tac->res?((tac->res->text)):"");
							}
							else
							{
								fprintf(fout, "\tmovl	$..%s, %%esi\n"
										"\tmovl	$.R2, %%edi\n"
										"\tcall	__isoc99_scanf\n",
									tac->res?tac->res->text:"");
							}
				break;

		}


	}







	fclose(fout);


}

