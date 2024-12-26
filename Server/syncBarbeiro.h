#include <pthread.h>
#include <semaphore.h>

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

void barberShop_init(struct barberShop *barberShop,int maxClientes);