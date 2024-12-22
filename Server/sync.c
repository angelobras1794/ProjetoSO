#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "sync.h"



void simple_barrier_init(struct simple_barrier_t *barrier, int threshold) {
    
    sem_init(&barrier->barreira, 0, 0);
    
    if (pthread_mutex_init(&barrier->mutex, NULL) != 0) {
        perror("Failed to initialize mutex");
        exit(EXIT_FAILURE);
    }   
    
    barrier->count = 0;
    barrier->threshold = threshold;
     
    
}

void simple_barrier_wait(struct simple_barrier_t *barrier) {
    printf("contador %d\n",barrier->count);
    printf("threshold %d\n",barrier->threshold);    
    pthread_mutex_lock(&barrier->mutex);
        barrier->count++;
        printf("contador %d\n",barrier->count);
    pthread_mutex_unlock(&barrier->mutex);
    if (barrier->count == barrier->threshold) {
        sem_post(&barrier->barreira);
    }   
    printf("Preso\n"); 
    sem_wait(&barrier->barreira);
    printf("Soltou\n");
    sem_post(&barrier->barreira);
}

void mutexes_init(struct mutex_threads *mutexes) {
    if (pthread_mutex_init(&mutexes->criar_sala, NULL) != 0) {
        perror("Failed to initialize mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&mutexes->entrar_sala, NULL) != 0) {
        perror("Failed to initialize mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&mutexes->jogar_sala, NULL) != 0) {
        perror("Failed to initialize mutex");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(&mutexes->sair_sala, NULL) != 0) {
        perror("Failed to initialize mutex");
        exit(EXIT_FAILURE);
    }
}




//FUNCOES DE Semaforo

// void sem_wait(struct  Semaphore * semaphore )
// {
 
//  pthread_mutex_lock(&semaphore->mutex);
//  semaphore->value--;

// if (semaphore->value < 0) {
//     do {
//     pthread_cond_wait(&semaphore->cond,&semaphore->mutex );
//     } while (semaphore->wakeups < 1);
//     semaphore->wakeups--;
// }
//  pthread_mutex_unlock(&semaphore->mutex);
//  }


//  void sem_signal(struct Semaphore *semaphore) {
//     Lock the semaphore mutex
//     if (pthread_mutex_lock(&semaphore->mutex) != 0) {
//         perror("Failed to lock mutex");
//         return;
//     }

//     Increment the semaphore value
//     semaphore->value++;

//     Wake up waiting threads if needed
//     if (semaphore->value <= 0) {
//         semaphore->wakeups++;
//         if (pthread_cond_signal(&semaphore->cond) != 0) {
//             perror("Failed to signal condition");
//         }
//     }

//     Unlock the semaphore mutex
//     pthread_mutex_unlock(&semaphore->mutex);
       
    
// }

//  struct Semaphore* make_semaphore(int value) {
//     Allocate memory for the semaphore
//     struct Semaphore *semaphore = malloc(sizeof(struct Semaphore));
//     if (semaphore == NULL) {
//         perror("Failed to allocate memory for semaphore");
//         exit(EXIT_FAILURE);
//     }

//     Allocate memory for the mutex and condition variable
//     semaphore->mutex = malloc(sizeof(pthread_mutex_t));
//     semaphore->cond = malloc(sizeof(pthread_cond_t));
//     if (semaphore->mutex == NULL || semaphore->cond == NULL) {
//         perror("Failed to allocate memory for mutex or condition variable");
//         free(semaphore);
//         exit(EXIT_FAILURE);
//     }

//     Initialize the semaphore properties
//     semaphore->value = value;
//     semaphore->wakeups = 0;

//     Initialize the mutex and condition variable
//     pthread_mutex_init(&semaphore->mutex, NULL);
//     pthread_cond_init(&semaphore->cond, NULL);

//     return semaphore;
// }