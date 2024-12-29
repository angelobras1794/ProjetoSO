#include "syncBarbeiro.h"

// Função que inicializa a estrutura barberShop
void barberShop_init(struct barberShop *barberShop,int maxClientes) {
    pthread_mutex_init(&barberShop->trinco, NULL);
    sem_init(&barberShop->clientes, 0, 0);
    sem_init(&barberShop->barbeiro, 0, 0);
    sem_init(&barberShop->clienteCompleto, 0, 0);
    sem_init(&barberShop->barbeiroCompleto, 0, 0);
    barberShop->nClientes = 0;
    barberShop->maxclientes = maxClientes;
    barberShop->barbeariaAberta = 0;
    barberShop->barbeiroChegou = 0;
    
    
}

void prodConsInit(struct prodCons * prodCons,int tamBuffer){
    pthread_mutex_init(&prodCons->trinco,NULL);
    sem_init(&prodCons->itens,0,0);
    sem_init(&prodCons->spaces,0,tamBuffer);
    prodCons->buffer = create_fixed_string_array(tamBuffer);
    prodCons->indexProd = 0;
    prodCons->indexCons = 0;

}

struct FixedStringArray* create_fixed_string_array(size_t capacity) {
    struct FixedStringArray* str_array = malloc(sizeof(struct FixedStringArray));
    if (!str_array) {
        perror("Failed to allocate FixedStringArray");
    }
    str_array->array = malloc(capacity * sizeof(char*));
    if (!str_array->array) {
        perror("Failed to allocate array");
        free(str_array);
    }
    str_array->size = 0;
    str_array->capacity = capacity;
    return str_array;
}

void print_fixed_string_array(struct FixedStringArray* str_array) {
    for (size_t i = 0; i < str_array->size; i++) {
        printf("[%zu]: %s\n", i, str_array->array[i]);
    }
}

void add_to_fixed_string_array(struct FixedStringArray* str_array, const char* str,int index) {
    
    str_array->array[index] = strdup(str); // Copy the string
    // if (!str_array->array[str_array->size]) {
    //     perror("Failed to copy string");
    //     free(str_array->array);
    //     free(str_array);
    // }
}

void criaConsumidor(struct prodCons * prodCons){
    pthread_t thread;
    pthread_create(&thread, NULL,consConsome,prodCons);
    pthread_detach(thread);  // Detach para não precisar gerenciar os joins
}


void  consConsome(void* arg) {
    struct prodCons* prodCons = (struct prodCons*)arg;
    while(1){
        char  mensagem[300];
       
        sem_wait(&prodCons->itens);
      
        pthread_mutex_lock(&prodCons->trinco);
        
        strcpy(mensagem, prodCons->buffer->array[prodCons->indexCons]);
        
        escrever_logs(mensagem);
        
        prodCons->indexCons = (prodCons->indexCons + 1) % prodCons->buffer->capacity;
        pthread_mutex_unlock(&prodCons->trinco);
        sem_post(&prodCons->spaces);

    } 
    
    
}

void prodProduz(struct prodCons * prodCons,char * mensagem){
    
    sem_wait(&prodCons->spaces);
   
    pthread_mutex_lock(&prodCons->trinco);
   
    add_to_fixed_string_array(prodCons->buffer,mensagem,prodCons->indexProd);
   
    prodCons->indexProd = (prodCons->indexProd + 1) % prodCons->buffer->capacity;
   
    pthread_mutex_unlock(&prodCons->trinco);
    sem_post(&prodCons->itens);
}


void escrever_logs(char *mensagem){
   
    FILE *ficheiro = fopen("logs.txt", "a");
    if (ficheiro == NULL) {
        perror("Error opening file");
        return;
    }
   

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
   
    strftime(time_str, sizeof(time_str) - 1, "%H:%M:%S", t);
   
    fprintf(ficheiro, "[%s] %s\n",time_str, mensagem);
    
    fclose(ficheiro);
     

}


