#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

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

// struct Semaphore{
//     int value, wakeups ;
//     pthread_mutex_t  mutex ;
//     pthread_cond_t  cond ;
//  };




//FUNCOES DA BARREIRA
void simple_barrier_init(struct simple_barrier_t *barrier, int threshold);
void simple_barrier_wait(struct simple_barrier_t *barrier);

void mutexes_init(struct mutex_threads *mutexes);




//FUNCOES DO SEMAFORO

// struct Semaphore * make_semaphore ( int value );
// void sem_wait(struct Semaphore * semaphore);
// void sem_signal(struct Semaphore * semaphore);



