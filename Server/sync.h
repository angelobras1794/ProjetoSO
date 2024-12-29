#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdbool.h>



struct mutex_threads{
    pthread_mutex_t  criar_sala;   //TRINCO PARA CRIAR SALA
    pthread_mutex_t  entrar_sala;  //TRINCO PARA ENTRAR NA SALA
    pthread_mutex_t  jogar_sala;
    pthread_mutex_t sair_sala;
    
    
};

struct simple_barrier_t {
    pthread_mutex_t  mutex;
    sem_t barreira;
    int count;
    int threshold;
} ;

struct ClientRequest {
    int clientId;
    int socket;
    int line, column, value; // Play details
    struct ClientRequest* next;
};

struct PriorityQueue {
    struct ClientRequest* head;
    pthread_mutex_t lock;
    bool iniciarJogo;
    bool atendedorOn;
};




//FUNCOES DA BARREIRA
void simple_barrier_init(struct simple_barrier_t *barrier, int threshold);
void simple_barrier_wait(struct simple_barrier_t *barrier);

void mutexes_init(struct mutex_threads *mutexes);

//FUNCOES DA PRIORIDADE

void createPriorityQueue(struct PriorityQueue* fila);
void enqueue(struct PriorityQueue* pq, int clientId, int socket, int line, int column, int value); 
struct ClientRequest* dequeue(struct PriorityQueue* pq);  
int countNodes(struct ClientRequest* head);
void clean_list(struct ClientRequest** head) ;







