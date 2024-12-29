#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "sync.h"

#define MAX_JOGADORES 2




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


void createPriorityQueue(struct PriorityQueue * fila) {
    fila = (struct PriorityQueue*)malloc(sizeof(struct PriorityQueue));
    fila->head = NULL;
    fila->iniciarJogo=false;
    fila->atendedorOn=false;
    pthread_mutex_init(&fila->lock, NULL);
}

//FILA UTILIZANDO LISTAS LIGADAS

void enqueue(struct PriorityQueue* pq, int clientId, int socket, int line, int column, int value) {
    pthread_mutex_lock(&pq->lock);

    struct ClientRequest* newRequest = (struct ClientRequest*)malloc(sizeof(struct ClientRequest));
    newRequest->clientId = clientId;
    // newRequest->priority = priority;
    newRequest->socket = socket;
    newRequest->line = line;
    newRequest->column = column;
    newRequest->value = value;
    newRequest->next = NULL;

    if (!pq->head) {
        newRequest->next = pq->head;
        pq->head = newRequest;
    } else {
        struct ClientRequest* current = pq->head;
        while (current->next) {
            current = current->next;

        }
        newRequest->next = current->next;
        current->next = newRequest;
    }
    if(countNodes(pq->head) == MAX_JOGADORES){
        pq->iniciarJogo=true;
    }

    pthread_mutex_unlock(&pq->lock);
}

struct ClientRequest* dequeue(struct PriorityQueue* pq) {
    pthread_mutex_lock(&pq->lock);

    if (!pq->head) {
        pthread_mutex_unlock(&pq->lock);
        printf("erro FILA VAZIA\n");
        return NULL;
    }

    struct ClientRequest* topRequest = pq->head;
    pq->head = pq->head->next;

    pthread_mutex_unlock(&pq->lock);
    return topRequest;
}

int countNodes(struct ClientRequest* head) {
    int count = 0;
    struct ClientRequest* current = head; // Start at the head of the list
    while (current != NULL) {
        count++;                 // Increment the count for each node
        current = current->next; // Move to the next node
    }
    return count;
}

void clean_list(struct ClientRequest** head) {
    struct ClientRequest* current = *head;  // Start from the head of the list
    struct ClientRequest* next_node;

    while (current != NULL) {
        next_node = current->next; // Save the pointer to the next node
        free(current);             // Free the current node
        current = next_node;       // Move to the next node
    }

    *head = NULL; // Set the head pointer to NULL to indicate the list is empty
}

