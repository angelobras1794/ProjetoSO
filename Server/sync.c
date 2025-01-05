#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "sync.h"

#define MAX_JOGADORES 6




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
    pthread_mutex_lock(&barrier->mutex);
        barrier->count++;
    pthread_mutex_unlock(&barrier->mutex);
    if (barrier->count == barrier->threshold) {
        sem_post(&barrier->barreira);
    }    
    sem_wait(&barrier->barreira);
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


void createQueue(struct Queue * fila) {
    fila = (struct Queue*)malloc(sizeof(struct Queue));
    fila->head = NULL;
    fila->iniciarJogo=false;
    fila->atendedorOn=false;
    pthread_mutex_init(&fila->lock, NULL);
}

//FILA UTILIZANDO LISTAS LIGADAS

void enqueue(struct Queue* pq, int clientId, int socket, int line, int column, int value) {
    pthread_mutex_lock(&pq->lock);

    struct ClientRequest* newRequest = (struct ClientRequest*)malloc(sizeof(struct ClientRequest));
    newRequest->clientId = clientId;
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

struct ClientRequest* dequeue(struct Queue* pq) {
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



//FILAS PRIORIDADE

void createPriorityQueue(struct PriorityQueue * fila) {
    fila = (struct PriorityQueue*)malloc(sizeof(struct PriorityQueue));
    fila->head = NULL;
    fila->iniciarJogo=false;
    fila->atendedorOn=false;
    pthread_mutex_init(&fila->lock, NULL);
}

void enqueuePriority(struct PriorityQueue* pq, int clientId, int socket, int line, int column, int value,int prioridade) {
    pthread_mutex_lock(&pq->lock);

    struct ClientRequestPriority* newRequest = (struct ClientRequestPriority*)malloc(sizeof(struct ClientRequestPriority));
    newRequest->clientId = clientId;
    newRequest->priority = prioridade;
    newRequest->socket = socket;
    newRequest->line = line;
    newRequest->column = column;
    newRequest->value = value;
    newRequest->next = NULL;

    if (!pq->head || pq->head->priority < prioridade) {
        newRequest->next = pq->head;
        pq->head = newRequest;
    } else {
        struct ClientRequestPriority* current = pq->head;
        while (current->next && current->next->priority >= prioridade) {
            current = current->next;

        }
        newRequest->next = current->next;
        current->next = newRequest;
    }
    if(countNodesPriority(pq->head) == MAX_JOGADORES){
        pq->iniciarJogo=true;
    }

    pthread_mutex_unlock(&pq->lock);
}

struct ClientRequestPriority* dequeuePriority(struct PriorityQueue* pq) {
    pthread_mutex_lock(&pq->lock);

    if (!pq->head) {
        pthread_mutex_unlock(&pq->lock);
        printf("erro FILA VAZIA\n");
        return NULL;
    }

    struct ClientRequestPriority* topRequest = pq->head;
    pq->head = pq->head->next;

    pthread_mutex_unlock(&pq->lock);
    return topRequest;
}

int countNodesPriority(struct ClientRequestPriority* head) {
    int count = 0;
    struct ClientRequestPriority* current = head; // Start at the head of the list
    while (current != NULL) {
        count++;                 // Increment the count for each node
        current = current->next; // Move to the next node
    }
    return count;
}