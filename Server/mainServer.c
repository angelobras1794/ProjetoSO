#include <stdio.h>
#include <stdlib.h>
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include "funcoesServer.h"
#include "cJSON.h"

#define N_CLIENTES 100
#define MAX_SALAS  3
#define MAX_JOGADORES 1


bool percorrer_arr(struct jogoSoduku *salas,int sala_id){
    for(int i=0;i<sizeof(salas);i++){
        if(salas[i].idSala == sala_id){
            return true;
        }
    }
     return false;


}

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

void handle_client(int server_socket,int client_socket,struct jogoSoduku *salas,char salasDisponiveis[][100],int *totalSalas){
    char mensagem[100];
    // verifica_ID(server_socket,client_socket,arr_clientes);
    printf("%dnknkn :: \n",*totalSalas);
    
    // FILE * ficheiro;
    // struct jogoSoduku *jogos = malloc(sizeof(struct jogoSoduku));
    // criarJogo(ficheiro,jogos,client_socket);
    
    while(true){
      recv(client_socket,mensagem,sizeof(mensagem),0);
      processarMensagem(mensagem,client_socket,salas,salasDisponiveis,totalSalas);
      


    }
    
    

}

int main(int argc,int *argv[]){
     struct confServer *configuracao = malloc(sizeof(struct confServer)); //alocar espaco para a struct
     ler_ficheiroConf(configuracao,argv[1]);

    srand(time(NULL)); //usado para a geracao de numeros aleatorios
    struct jogoSoduku salas[MAX_SALAS];
    for(int i=0;i<MAX_SALAS;i++){
         salas[i].idSala=0;
         salas[i].nJogadores=0;
         salas[i].idTabuleiro=0;

    }
    int totalSalas = 0;
    char salasDisponiveis[MAX_SALAS][100];
    printf("%d\n",totalSalas);
    
     
    
    //criar o server socket
    int server_socket;
    server_socket = socket(AF_INET,SOCK_STREAM,0); //socket(dominio,tipo,protocolo) , AF_INET -> IPV4 , sock_stream -> TCP

    // defenir o endereco do servidor 
    
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_size = sizeof(client_address);
    server_address.sin_family=AF_INET; //especeficacao do endereco de familia 
    server_address.sin_port = htons(configuracao->porta);   //converter de inteiro para ordem de bytes de rede   
    inet_pton(AF_INET, configuracao->ip_server, &server_address.sin_addr); //passar de char para formato ip
       

   // ligar o socket para o ip especificado e porta
   bind(server_socket,(struct sockaddr*) &server_address,sizeof(server_address));

   listen(server_socket,10); //consegue ter pelo menos 10 clientes

   //while(1){} loop principal , para o servidor estar sempre a receber pedidos
    int client_socket;
    while(true){
        
    client_socket = accept(server_socket,(struct sockaddr*)&client_address, &client_address_size); //servidor aceita clientes


    handle_client(server_socket,client_socket,salas,salasDisponiveis,&totalSalas);   //funcao para tratar do cliente 
     
    }
                                                                           
    return 0;
}





 
 







