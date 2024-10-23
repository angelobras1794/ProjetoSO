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

void jogaJogo(int client_socket){
 int linha, coluna,valor;
 int jogadas = 0;
 char mensagem[100];
 int * tabuleiro[9][9];
 recv(client_socket,&mensagem,sizeof(mensagem),0);
 printf("%s",mensagem);
 while(jogadas < 4){
  recv(client_socket,tabuleiro,sizeof(tabuleiro),0);
  mostra_grid(tabuleiro);
 recv(client_socket,&mensagem,sizeof(mensagem),0);
 printf("%s",mensagem);
 scanf("%d",&linha);
 send(client_socket,&linha,sizeof(linha),0);     //envia a linha
 recv(client_socket,&mensagem,sizeof(mensagem),0);
 printf("%s",mensagem);
 scanf("%d",&coluna);
 send(client_socket,&coluna,sizeof(coluna),0);   //envia a coluna
 recv(client_socket,&mensagem,sizeof(mensagem),0);
  if(strcmp(mensagem,"true")==0){
     recv(client_socket,&mensagem,sizeof(mensagem),0);
     printf("%s",mensagem);
     printf("%s","Please enter a value for 'valor':\n");
     scanf("%d", &valor);
     printf("Valor to send: %d\n", valor);
     send(client_socket,&valor,sizeof(valor),0);
     recv(client_socket,&mensagem,sizeof(mensagem),0);
     if(strcmp(mensagem,"true")==0){
      jogadas++;
      recv(client_socket,&mensagem,sizeof(mensagem),0);
      printf("%s",mensagem);
      recv(client_socket,tabuleiro,sizeof(tabuleiro),0);
     }else{
        recv(client_socket,&mensagem,sizeof(mensagem),0);
        printf("%s",mensagem);
        recv(client_socket,tabuleiro,sizeof(tabuleiro),0);
     }
  }else{
      recv(client_socket,&mensagem,sizeof(mensagem),0);
      printf("\n%s",mensagem);
      recv(client_socket,tabuleiro,sizeof(tabuleiro),0);
      
      
  }

 }
 if(jogadas == 4){
   recv(client_socket,&mensagem,sizeof(mensagem),0);
   printf("%s",mensagem);
   recv(client_socket,tabuleiro,sizeof(tabuleiro),0);
   mostra_grid(tabuleiro);

 }
 

}


void mostra_grid(int *tabuleiro[9][9]) {
    for (int i = 0; i < 9; i++) { // linhas
    if (i == 3 || i == 6) {
            printf("-----------\n");
        }
        for (int j = 0; j < 9; j++) { // colunas
            if (j == 3 || j == 6) {
                printf("|");
            }
            printf("%d",tabuleiro[i][j]);
        }
        printf("\n");
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
    jogaJogo(network_socket);


    close(network_socket);
    
    //printf("O servidor respondeu com %s ",server_response); //resposta do server
   
    return 0;
}