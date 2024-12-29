#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//PROBLEMA DA BARBEARIA 
struct barberShop{
    pthread_mutex_t trinco;
    sem_t clientes;
    sem_t barbeiro;
    sem_t clienteCompleto;
    sem_t barbeiroCompleto;
    int nClientes;
    int maxclientes;
    int barbeariaAberta;
    int barbeiroChegou;
};

struct prodCons{
   struct FixedStringArray  * buffer;
   pthread_mutex_t trinco;
   sem_t itens; //Quando positivo indica o numero de intens no buffer
   sem_t spaces; //Numero de espacos disponiveis no buffer
   int indexProd;
   int indexCons;
   
    
};

struct FixedStringArray {
    char** array;    // Array of string pointers
    size_t size;     // Current number of strings in the array
    size_t capacity; // Total allocated slots in the array 
} ;

void barberShop_init(struct barberShop *barberShop,int maxClientes);

void prodConsInit(struct prodCons * prodCons,int tamBuffer);
void criaConsumidor(struct prodCons * prodCons);

void  consConsome(void* arg);
void prodProduz(struct prodCons * prodCons,char * mensagem);
void escrever_logs(char *mensagem);

struct FixedStringArray* create_fixed_string_array(size_t capacity);

void print_fixed_string_array(struct FixedStringArray* str_array);
void add_to_fixed_string_array(struct FixedStringArray* str_array, const char* str,int index);


