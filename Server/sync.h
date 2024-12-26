#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

#define MAX_JOGADORES 5

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
    int * priority;
    int socket;
    int line, column, value; // Play details
    struct ClientRequest* next;
};

struct PriorityQueue {
    struct ClientRequest* head;
    pthread_mutex_t lock;
};




//FUNCOES DA BARREIRA
void simple_barrier_init(struct simple_barrier_t *barrier, int threshold);
void simple_barrier_wait(struct simple_barrier_t *barrier);

void mutexes_init(struct mutex_threads *mutexes);

//FUNCOES DA PRIORIDADE

void createPriorityQueue(struct PriorityQueue* fila);
void enqueue(struct PriorityQueue* pq, int clientId, int *priority, int socket, int line, int column, int value); 
struct ClientRequest* dequeue(struct PriorityQueue* pq);  







