#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include <semaphore.h>
#include <pthread.h>

void olamundo(struct exemplo *ex) {
    printf("Ola Mundo\n");
    sem_wait(&ex->sem);
    
}   
struct exemplo
{
    sem_t *sem;
};



int main() {
    struct exemplo *ex = malloc(sizeof(struct exemplo));
    sem_init(&ex->sem, 0, 5);
    olamundo(ex);
    int sem_value;
    sem_getvalue(&ex->sem, &sem_value);
    printf("Semaphore value: %d\n", sem_value);

    return 0;
}
