#include <stdio.h>
#include <stdlib.h>
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <funcoesServer.h>



int main(){

    char server_message[256] = "Conseguiste alcancar o servidor";
    //create the server socket
    int server_socket;
    server_socket = socket(AF_INET,SOCK_STREAM,0);

    // defenir o endereco do servidor 
    
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_size = sizeof(client_address);
    server_address.sin_family=AF_INET; //especeficacao do endereco de familia 
    server_address.sin_port = htons(10000); 
    server_address.sin_addr.s_addr = INADDR_ANY;   

   // ligar o socket para o ip especificado e porta
   bind(server_socket,(struct sockaddr*) &server_address,sizeof(server_address));

   listen(server_socket,10);

   //while(1){} loop principal , para o servidor estar sempre a receber pedidos

   int client_socket;
   client_socket = accept(server_socket,(struct sockaddr*)&client_address, &client_address_size); 
   //(struct sockaddr*)&client_address converte o apontador de client_adress para um apontador de aockaddr
   // This is necessary because the accept() function is designed to accept a pointer to struct sockaddr, allowing it to handle different address types (IPv4, IPv6, etc.).
   //enviar mensagem                                                                                
   send(client_socket, server_message, sizeof(server_message),0);

   

    return 0;
}
