#include <stdio.h>
#include <stdlib.h>
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

int main(){

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
   char server_response[256];
   recv(network_socket,&server_response,sizeof(server_response),0); //receber resposta do server
   
   printf("O servidor respondeu com %s ",server_response); //resposta do server
   
    return 0;
}