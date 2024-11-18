#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <strings.h> // For bzero


void jogaJogo(int client_socket,int id_cliente){
 int linha, coluna,valor;
 int jogadas = 0;
 char mensagem[100];
 int * tabuleiro[9][9];
 send(client_socket,&id_cliente,sizeof(id_cliente),0);
 recv(client_socket,&mensagem,sizeof(mensagem),0);
 printf("%s",mensagem);
 escrever_logs(id_cliente,"1 - O cliente recebeu o Jogo");
 while(jogadas < 8){
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
 escrever_logs(id_cliente,"2 - O cliente enviou a resposta (linha,coluna)");
 recv(client_socket,&mensagem,sizeof(mensagem),0);
 escrever_logs(id_cliente,"3 - O cliente recebe a resposta (linha,coluna)");
  if(strcmp(mensagem,"true")==0){
     recv(client_socket,&mensagem,sizeof(mensagem),0);
     printf("%s",mensagem);
     scanf("%d", &valor);
     send(client_socket,&valor,sizeof(valor),0);
     escrever_logs(id_cliente,"2 - O cliente enviou a resposta (valor)");
     recv(client_socket,&mensagem,sizeof(mensagem),0);
     escrever_logs(id_cliente,"3 - O cliente recebe a resposta (valor)");
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
 if(jogadas == 8){
   recv(client_socket,&mensagem,sizeof(mensagem),0);
   printf("%s",mensagem);
   recv(client_socket,tabuleiro,sizeof(tabuleiro),0);
   mostra_grid(tabuleiro);
   escrever_logs(id_cliente,"4 - O cliente termina o jogo");

 }
 

}
void escrever_logs(int id_user,char *mensagem){
    FILE *ficheiro = fopen("logs.txt", "a");
    if (ficheiro == NULL) {
        perror("Error opening file");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
    strftime(time_str, sizeof(time_str) - 1, "%H:%M:%S", t);
    fprintf(ficheiro, "%d [%s] '%s'\n",id_user,time_str, mensagem);
    fclose(ficheiro);
     

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


void mostraMenuPrincipal(int client_socket,int Cliente_id){
    int menu_option;

    printf("Bem vindo ao Servido Sodoku!!!\n");
    printf("------------------------------------------\n\n");
    do{
    printf("Menu Principal\n");
    printf("1-Jogar\n");
    printf("2-Estatisticas\n");
    printf("3-Sair\n");
    printf("Porfavor selecione uma opcao: ");
    scanf("%d",&menu_option);

    switch(menu_option){

    case 1:
        printf("esta na opcao de jogar ");
        mostraMenuJogar(client_socket,1);
        break;
    case 2:
        printf("esta na opcao estatisticas");
        break;
    default:
        printf("input invalido");
        break;
    }

    }while(menu_option !=3);


}

void mostraMenuJogar(int client_socket,int Cliente_id) {
    int menu_option;
    char mensagem[100];
    

    do {
        printf("Menu Jogar\n");
        printf("1-Entrar numa sala\n");
        printf("2-Criar sala\n");
        printf("3-Voltar\n");
        printf("Por favor, selecione uma opcao: \n");
        scanf("%d", &menu_option);

        switch (menu_option) {
            case 1: { // Entrar em uma sala
                int opcaoSala=0;
                int totalSalas;
                memset(mensagem, 0, sizeof(mensagem));
                sprintf(mensagem,"salasCriadas:ola:%d",Cliente_id);
                send(client_socket,&mensagem,strlen(mensagem), 0); //faz isto
                memset(mensagem, 0, sizeof(mensagem));
                int verifica = recv(client_socket,&totalSalas,sizeof(totalSalas),0);             //nao faz print 
                if(totalSalas != 0 ){
                    char salas[totalSalas][100]; //fazer a parte das salas para o cliente
                    recv(client_socket,&salas,sizeof(salas),0);
                    printf("Salas Disponiveis: \n");
                    for(int z=0;z<totalSalas;z++){
                        printf("%s\n",salas[z]);
                    }
                    printf("Escolha uma sala: \n");
                    scanf("%d",&opcaoSala);
                    printf("%d\n",opcaoSala);
                    printf("Debug: opcaoSala = %d, Cliente_id = %d\n", opcaoSala,Cliente_id);
                
                
                    sprintf(mensagem,"entrar_em_sala:%d:%d",opcaoSala,Cliente_id);
                    
                    send(client_socket,&mensagem,sizeof(mensagem),0); //+1 ?
                    recv(client_socket,&mensagem,sizeof(mensagem),0);
                    printf("%s\n",mensagem); //resposta do servidor (sucesso na criacao ou nao)
                }else{
                    printf("Nao existe nenhuma SALA \n\n\n");
                }
                
                break;
            }
            case 2: { // Criar uma sala
                char nomeSala[20];
                char resposta_servidor[110];
                printf("Esta na opcao de criar uma sala\n");
                printf("Insira o nome da sala: \n");
                scanf("%s", nomeSala);
                printf(" \nnome da SALA: %s \n\n",nomeSala); //
                // Envia ação e nome da sala ao servidor
                memset(mensagem, 0, sizeof(mensagem));
                sprintf(mensagem,"criar_sala:%s:%d",nomeSala,Cliente_id);
                send(client_socket,&mensagem,strlen(mensagem), 0);
                recv(client_socket,&resposta_servidor,sizeof(resposta_servidor), 0);
                printf("%s\n",resposta_servidor);
                break;
            }
            default:
                printf("Input invalido\n");
                break;
        }

    } while (menu_option != 3);
}



int main(){
    
    int Cliente_id = 1;
    
    int network_socket;
    network_socket = socket(AF_INET,SOCK_STREAM,0);//socket(dominio,tipo,protocolo) , AF_INET -> IPV4 , sock_stream -> TCP

    //especificacao um endereco para o socket

    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET; //especeficacao do endereco de familia 
    server_address.sin_port = htons(10000); 
    inet_pton(AF_INET,"10.2.15.230", &server_address.sin_addr); //passa
    
    
   int connection_status = connect(network_socket,(struct sockaddr*)&server_address,sizeof(server_address));
   
   if(connection_status==-1){
    printf("houve um erro");
    
   }else{
        // geraId(Cliente_id,network_socket);
        mostraMenuPrincipal(network_socket,1);
   }
    
    //receber resposta do server
    //   //geracao do id
    // jogaJogo(network_socket,Cliente_id); //joga o jogo
    

    
    
    //printf("O servidor respondeu com %s ",server_response); //resposta do server
   
    return 0;
}