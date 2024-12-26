#include <stdio.h>
#include <stdlib.h>
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include "funcoesServer.h"

#include "cJSON.h"
#include <pthread.h>



#define N_CLIENTES 100
#define MAX_SALAS  3


pthread_mutex_t mutexClientesConectados;




struct thread_args {
    int server_socket;
    int client_socket;
    struct jogoSoduku *salas;
    char (*salasDisponiveis)[100];
    int *totalSalas;
    int prioridade;
    struct estatisticaServer *estatistica;
    struct mutex_threads *mutexes;
};






void insere_id(struct jogoSoduku *salas,int sala_id){
    for(int i = 0; i<sizeof(salas);i++){
        if(salas[i].idSala==0){
            salas[i].idSala = sala_id;
            break;
        }

    }  
}

void verifica_ID(int client_socket,struct jogoSoduku *salas){
    int id_sala = 1;
    char mensagem[100];
    while(true){
        if(percorrer_arr(salas,id_sala)){
            id_sala++;
        }
        else{
            insere_id(salas,id_sala);      // o cliente 'e inserido no array
            printf("A SALA foi criada com id %d ",id_sala);
            break;
        }  
    }    

}

void * handle_client(void *args){

    
    struct thread_args *threadArgs = (struct thread_args *)args;
    
    int server_socket = threadArgs->server_socket;
    int client_socket = threadArgs->client_socket;
    struct jogoSoduku *salas = threadArgs->salas;
    char (*salasDisponiveis)[100] = threadArgs->salasDisponiveis;
    int *totalSalas = threadArgs->totalSalas;
    int *prioridade = threadArgs->prioridade;
    struct estatisticaServer *estatistica = threadArgs->estatistica;
    struct mutex_threads *mutexes = threadArgs->mutexes;
    

    char mensagem[100];
        
    while (recv(client_socket, mensagem, sizeof(mensagem), 0) > 0) {
        
        processarMensagem(mensagem, client_socket,salas,salasDisponiveis,totalSalas,mutexes,estatistica,&prioridade);   
    }
}

int main(int argc,int *argv[]){
     struct confServer *configuracao = malloc(sizeof(struct confServer)); //alocar espaco para a struct
     ler_ficheiroConf(configuracao,argv[1]);
     pthread_mutex_init(&mutexClientesConectados, NULL);
     

    srand(time(NULL)); //usado para a geracao de numeros aleatorios
    struct jogoSoduku* salas = (struct jogoSoduku*)malloc(MAX_SALAS * sizeof(struct jogoSoduku));
    void salasInit(salas);
    int totalSalas = 0;
    int clientesConectados = 0;
    char salasDisponiveis[MAX_SALAS][100];
    struct mutex_threads *mutexes = malloc(sizeof(struct mutex_threads));
    mutexes_init(mutexes);
    struct estatisticaServer *estatistica = malloc(sizeof(struct estatisticaServer));
    estatisticaServerInit(estatistica);
    printf("Clientes conectados: %d\n", (estatistica->clientesConectados));
    printf("Tabuleiros resolvidos: %d\n", (estatistica->tabuleirosResolvidos));
    printf("Tabuleiros em resolucao: %d\n", (estatistica->tabuleirosEmResolucao));
    
    


    //criar o server socket
    int server_socket;
    server_socket = socket(AF_INET,SOCK_STREAM,0); //socket(dominio,tipo,protocolo) , AF_INET -> IPV4 , sock_stream -> TCP

    // defenir o endereco do servidor 
    
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_size = sizeof(client_address);
    server_address.sin_family=AF_INET; //especeficacao do endereco de familia 
    server_address.sin_port = htons(configuracao->porta);   //converter de inteiro para ordem de bytes de rede   
    inet_pton(AF_INET, configuracao->ip_server, &server_address.sin_addr); //passar de char para formato ip
       
    
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

   // ligar o socket para o ip especificado e porta
   bind(server_socket,(struct sockaddr*) &server_address,sizeof(server_address));

   listen(server_socket,10); //consegue ter pelo menos 10 clientes

    int client_socket;
    
    while (1){
    client_socket = accept(server_socket,(struct sockaddr*)&client_address, &client_address_size); //servidor aceita clientes
    printf("Cliente conectado\n");
    struct thread_args *args = malloc(sizeof(struct thread_args));
    printf("Cliente conectado\n");


    args->server_socket = server_socket;
    args->client_socket = client_socket;
    args->salas = salas;
    args->salasDisponiveis = salasDisponiveis;
    args->totalSalas = &totalSalas;
    args->prioridade = 10;
    args->estatistica = estatistica;
    args->mutexes = mutexes;

    pthread_t t_id;
    pthread_mutex_lock(&args->mutexes->sair_sala);
    args->estatistica->clientesConectados+=1;
    pthread_mutex_unlock(&args->mutexes->sair_sala);
    printf("Clientes conectados: %d\n", (args->estatistica->clientesConectados));
    if (pthread_create(&t_id, NULL, handle_client, (void *)args) != 0) {
        perror("pthread_create failed");
        free(args);
        close(client_socket);
        exit(EXIT_FAILURE);
    }
   

}
                                                                         
    return 0;
}


