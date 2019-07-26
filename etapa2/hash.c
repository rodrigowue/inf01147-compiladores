//Autores: Guilherme Sonaglio Chaves e  Rodrigo Nogueira Wuerdig
#include "hash.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* inicializa tabela hash */
/*void initializeHashTable(Node** ht)
{
    int i;

    //for(i=0; i<HASH_SIZE; i++)
        ht[HASH_SIZE] = {NULL}; 

}*/


/* computa a funcao de hash dada a string key
* --------------------------------
* retorna um valor inteiro entre 0 e M-1
*/
int computeHash(const char* key)
{
    int hash=1;
    int i;

    for (i = 0; i < strlen(key); i++)
        hash = (key[i] * hash) % HASH_SIZE + 1;
    
    return hash-1;
}

char* searchHash(const char* key) {
    int hash = computeHash(key);
    Node* elemento = hashTable[hash];
    while (elemento != NULL) {
        if (strcmp(elemento->text, key) == 0) {
            return elemento->text;
        }
        elemento = elemento -> next;
    } 
    return NULL;
}

// insere elemento na tabela hash
void insertHash(char* key, int type, int line)
{
    int hash = computeHash(key);
    Node* elemento = hashTable[hash];

    if(searchHash(key) != NULL)
        return;

    Node* newNode = calloc(1, sizeof(Node));
    newNode->text = strdup(key);
    newNode->type = type;
    newNode->line = line;
    newNode->next = 0;

    if(elemento == NULL) 
        hashTable[hash] = newNode;
    else{
        while(elemento->next != NULL)
            elemento = elemento->next;

        elemento->next = newNode;

    }
}


void printTable()
{

    Node* elemento;
    int i;

    for(i=0; i<HASH_SIZE; i++)
    {
        elemento = hashTable[i];

        if(elemento != NULL)
        {
            do{
                printf("\n[%d]%s - line: %d", i, elemento->text, elemento->line);
                elemento = elemento->next;
            }while(elemento != NULL);
        }
    }

}
