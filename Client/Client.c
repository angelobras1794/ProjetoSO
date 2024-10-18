#include <stdio.h>
#include <stdlib.h>
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

void geraId(int Cliente_id,int client_socket){
    char resposta_servidor[5];
    int id = 1;
    printf("%d\n",id);
    while(1){
       printf("O cliente com id %d quer se conectar\n",id);
       send(client_socket,&id,sizeof(id),0);
       recv(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
       printf("O servidor respondeu com %s\n",resposta_servidor);
       if(strcmp(resposta_servidor,"true")==0){
          Cliente_id = id;
          printf("O id do cliente é %d\n",Cliente_id);
          break;
       }
       id++;
    }

}

int main(){
    
    int Cliente_id = 1;
    
    int network_socket;
    network_socket = socket(AF_INET,SOCK_STREAM,0);//socket(dominio,tipo,protocolo) , AF_INET -> IPV4 , sock_stream -> TCP

    //especificacao um endereco para o socket

    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET; //especeficacao do endereco de familia 
    server_address.sin_port = htons(10000); 
    server_address.sin_addr.s_addr = INADDR_ANY;      
    
   int connection_status = connect(network_socket,(struct sockaddr*)&server_address,sizeof(server_address));
   
   if(connection_status==-1){
    printf("houve um erro");
   }
    
    //receber resposta do server
    geraId(Cliente_id,network_socket);


    close(network_socket);
    
    //printf("O servidor respondeu com %s ",server_response); //resposta do server
   
    return 0;
}