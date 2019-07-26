//Autores: Guilherme Sonaglio Chaves e  Rodrigo Nogueira Wuerdig

#ifndef HASH_H
#define HASH_H

/******* Definicao da Hash ***********/

/* tamanho da tabela */
#define HASH_SIZE 997


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
Node* hashTable[HASH_SIZE] = { NULL };

void insertHash(char* key, int type, int line);

/******* TODO: implementar esses metodos ***********/

//int searchHash(Node** ht, Node* elem);


#endif // HASH_H
