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






