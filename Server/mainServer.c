#include <stdio.h>
#include <stdlib.h>
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include "funcoesServer.h"

#define N_CLIENTES 100


bool percorrer_arr(int arr_clientes[],int cliente_id){
    for(int i=0;i<sizeof(arr_clientes);i++){
        if(arr_clientes[i] == cliente_id){
            return true;
        }
    }
     return false;


}

void insere_id(int arr_clientes[],int client_id){
    for(int i = 0; i<sizeof(arr_clientes);i++){
        if(arr_clientes[i]==0){
            arr_clientes[i] = client_id;
            break;
        }

    }  
}

void handle_client(int server_socket,int client_socket,int arr_clientes[]){
    
    verifica_ID(server_socket,client_socket,arr_clientes);

    
    FILE * ficheiro;
    struct jogoSoduku *jogos = malloc(sizeof(struct jogoSoduku));
    criarJogo(ficheiro,jogos,client_socket);
    
    close(client_socket);
    
    

}


void verifica_ID(int server_socket,int client_socket,int arr_clientes[]){
    int id_cliente = 0;
    while(true){
        recv(client_socket,&id_cliente,sizeof(id_cliente),0);         // Now 'id' holds the correct integer in host byte order
        printf("O cliente com id %d quer se conectar\n",id_cliente);
        if(percorrer_arr(arr_clientes,id_cliente)){
            send(client_socket,"false",sizeof("false"),0);
        }
        else{
            send(client_socket,"true",sizeof("true"),0);
            insere_id(arr_clientes,id_cliente);  
            printf("O cliente com id %d foi inserido",id_cliente);
            break;
        }  
    }    

}






int main(int argc,int *argv[]){
     printf("%s",argv[1]);
     struct confServer *configuracao = malloc(sizeof(struct confServer));
     ler_ficheiroConf(configuracao,argv[1]);








    srand(time(NULL));
    int arr_clientes[N_CLIENTES] = {0};
     
    char server_message[256] = "Conseguiste alcancar o servidor";
    //create the server socket
    int server_socket;
    server_socket = socket(AF_INET,SOCK_STREAM,0);

    // defenir o endereco do servidor 
    
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_size = sizeof(client_address);
    server_address.sin_family=AF_INET; //especeficacao do endereco de familia 
    server_address.sin_port = htons(configuracao->porta);       //carregamento de parametros
    inet_pton(AF_INET, configuracao->ip_server, &server_address.sin_addr);
       //carregamento de parametros

   // ligar o socket para o ip especificado e porta
   bind(server_socket,(struct sockaddr*) &server_address,sizeof(server_address));

   listen(server_socket,10);

   //while(1){} loop principal , para o servidor estar sempre a receber pedidos
    int client_socket;
    while(true){
    client_socket = accept(server_socket,(struct sockaddr*)&client_address, &client_address_size);


    handle_client(server_socket,client_socket,arr_clientes);    
     
    }
                                                                           
    return 0;
}
