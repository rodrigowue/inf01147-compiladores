//Autores: Guilherme Sonaglio Chaves e  Rodrigo Nogueira Wuerdig

#ifndef HASH_H
#define HASH_H

/******* Definicao da Hash ***********/

/* tamanho da tabela */
#define HASH_SIZE 997

#define SYMBOL_INTEGER 		1
#define SYMBOL_CHAR 		2
#define SYMBOL_FLOAT 		3
#define SYMBOL_STRING 		4
#define SYMBOL_IDENTIFIER 	5

/* definicao do elemento da tabela*/
typedef struct element
{
    char* text;
    int type;
    int line;
    struct element *next;
} Node;

/* definicao da tabela hash */

//Node* hashTable[HASH_SIZE] = { NULL };
Node* hashTable[HASH_SIZE];

void initializeHashTable(Node** ht);
void insertHash(char* key, int type, int line);
Node* searchHash(const char* key);

/******* TODO: implementar esses metodos ***********/

//int searchHash(Node** ht, Node* elem);


#endif // HASH_H
