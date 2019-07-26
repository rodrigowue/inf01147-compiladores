//Autores: Guilherme Sonaglio Chaves e  Rodrigo Nogueira Wuerdig
#include "hash.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void initializeHashTable(Node** ht)
{
    int i;

    for(i=0; i<HASH_SIZE; i++)
        ht[i] = 0; 

}


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

Node* searchHash(const char* key) {

    int hash = computeHash(key);
    Node* elemento = hashTable[hash];
    while (elemento != NULL) {
        if (strcmp(elemento->text, key) == 0) {
            return elemento;
        }
        elemento = elemento -> next;
    } 
    return NULL;
}

// insere elemento na tabela hash
Node* insertHash(char* key, int type, int line)
{
    Node* newNode;
    int hash = computeHash(key);
    Node* elemento = hashTable[hash];

    if(searchHash(key) != NULL)
    	return newNode;

    newNode = calloc(1, sizeof(Node));
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
    return newNode;
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

void hashCheckUndeclared(){
    int i;
    Node *node;
    for(i = 0; i< HASH_SIZE; i++){
        for(node = hashTable[i]; node; node = node->next){
            if(node->type == SYMBOL_IDENTIFIER){
                fprintf(stderr, "ERRO semantico: variavel '%s' nao foi declarada.\n", node->text);          
            }   
        }
    }
}

Node* makeTemp(){
    static int serialNumber = 0;
    static char buffer[128];

    sprintf(buffer, "_temp%d_", serialNumber++);
    return insertHash(buffer, SYMBOL_IDENTIFIER,0);
}

Node* makeLabel(){
    static int serialNumber = 0;
    static char buffer[128];

    sprintf(buffer, "_label%d_", serialNumber++);
    return insertHash(buffer, SYMBOL_IDENTIFIER,0);
}
