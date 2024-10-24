#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include "funcoesServer.h"
#define TAM 6


void remove_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}
int is_empty_or_whitespace(const char *str) {
    while (*str) {
        if (!isspace((unsigned char)*str)) {
            return 0;
        }
        str++;
    }
    return 1;
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

void retira(struct jogoSoduku * Jogo){  //retira 4 posicoes
  for ( int i = 0; i < 4; i++)
  {
    int linha_random= rand() % (9);
    int coluna_random= rand() % (9);
    Jogo->tabuleiro[linha_random][coluna_random] = 0;
  }
}

void text_to_grid(char *text, struct jogoSoduku * Jogo) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            Jogo->tabuleiro[i][j] = text[i * 9 + j] - '0'; // Convert char to int
        }
    }
}

bool verifica_jogada(struct jogoSoduku * Jogo, int linha, int coluna, int valor){
    //verificar se o valor esta na linha
    for (int i = 0; i < 9; i++)
    {
        if(Jogo->tabuleiro[linha][i] == valor){
            printf("O valor ja existe na linha\n Tenta outro valor\n");
            return false;
        }
    }
    //verificar se o valor esta na coluna
    for (int i = 0; i < 9; i++)
    {
        if(Jogo->tabuleiro[i][coluna] == valor){
            printf("O valor ja existe na coluna\n Tenta outro valor\n");
            return false;
        }
    }
    //verificar se o valor esta no quadrado
    int linha_quadrado = linha / 3;
    int coluna_quadrado = coluna / 3;
    for (int i = linha_quadrado * 3; i < linha_quadrado * 3 + 3; i++)
    {
        for (int j = coluna_quadrado * 3; j < coluna_quadrado * 3 + 3; j++)
        {
            if(Jogo->tabuleiro[i][j] == valor){
                printf("O valor ja existe no quadrado\nTenta outro valor\n");
                return false;
            }
        }
    }
    return true;

}

void jogo(struct jogoSoduku * Jogo,int client_socket){
    int linha, coluna,valor,id_cliente;
    int jogadas = 0;

    FILE * ficheiro = NULL;
    char mensagem[100];
    recv(client_socket,&id_cliente,sizeof(id_cliente),0);
    strcpy(mensagem, "\n Bem vindo ao jogo do Soduku\n");
    send(client_socket,&mensagem,sizeof(mensagem),0);
   //printf("\n Bem vindo ao jogo do Soduku\n");
    while (jogadas < 4)
    {
    //envia o jogo para o cliente    
    send(client_socket,&Jogo->tabuleiro,sizeof(Jogo->tabuleiro),0);
    escrever_logs(id_cliente,"1 - O servidor envia o jogo");
   
    
    strcpy(mensagem, "\n Escolha uma linha\n");
    send(client_socket,&mensagem,sizeof(mensagem),0);
    // //recebe a linha escolhida pelo cliente
    recv(client_socket,&linha,sizeof(linha),0);
   
    linha--;  
    printf("\n%d\n",linha);
   
    strcpy(mensagem, "\nPorfavor insira a coluna\n");
    send(client_socket,&mensagem,sizeof(mensagem),0); 
   
    //  //recebe a coluna escolhida pelo cliente
    recv(client_socket,&coluna,sizeof(coluna),0);
    escrever_logs(id_cliente,"3 - O servidor verifica a resposta(linha,coluna)");
    coluna--;
    printf("%d\n",coluna);
    if(Jogo->tabuleiro[linha][coluna] == 0){
        escrever_logs(id_cliente,"3 - O servidor verifica a resposta");
        strcpy(mensagem,"true");
        send(client_socket,&mensagem,sizeof(mensagem),0);
        strcpy(mensagem, "\nPorfavor insira o valor\n");
        send(client_socket,&mensagem,sizeof(mensagem),0); //Acontece ate aqui
        recv(client_socket,&valor,sizeof(valor),0);
        escrever_logs(id_cliente,"2 - O servidor recebe a resposta");
        printf("%d\n",valor);
        if(verifica_jogada(Jogo,linha,coluna,valor)){
            escrever_logs(id_cliente,"3 - O servidor verifica a resposta(valor)");
            strcpy(mensagem, "true");
            send(client_socket,mensagem,sizeof(mensagem),0);
            Jogo->tabuleiro[linha][coluna] = valor;
            jogadas++;
            strcpy(mensagem, "\nJogada efetuada com sucesso\n Faca A sua proxima jogada\n");
            send(client_socket,&mensagem,sizeof(mensagem),0); 
            escrever_logs(id_cliente,"4 - O servidor envia a resposta");
            send(client_socket,&Jogo->tabuleiro,sizeof(Jogo->tabuleiro),0); 
        }else{
           escrever_logs(id_cliente,"3 - O servidor verifica a resposta");
           strcpy(mensagem,"false");
           send(client_socket,mensagem,sizeof(mensagem),0);
           strcpy(mensagem, "O valor ja existe na linha ou coluna ou quadrado\n");
           send(client_socket,&mensagem,sizeof(mensagem),0); 
           escrever_logs(id_cliente,"4 - O servidor envia a resposta");
           send(client_socket,&Jogo->tabuleiro,sizeof(Jogo->tabuleiro),0);
        }
    }else{
        escrever_logs(id_cliente,"3 - O servidor verifica a resposta");
        strcpy(mensagem,"false");
        send(client_socket,mensagem,sizeof(mensagem),0);
        strcpy(mensagem,"\nA posicao ja esta preenchida , Tenta outra posicao\n");
        send(client_socket,&mensagem,sizeof(mensagem),0);
        escrever_logs(id_cliente,"4 - O servidor envia a resposta");
        send(client_socket,&Jogo->tabuleiro,sizeof(Jogo->tabuleiro),0);
        
        
     }
   }
    strcpy(mensagem, "\nParabens, Jogo concluido\n");
    send(client_socket,&mensagem,sizeof(mensagem),0);
    send(client_socket,&Jogo->tabuleiro,sizeof(Jogo->tabuleiro),0);
    escrever_logs(id_cliente,"5 - O servidor termina o jogo ");
    

}

//escrever logs server
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

void criarJogo(FILE *ficheiro, struct jogoSoduku *jogos,int client_socket) {
    ficheiro = fopen("conf.txt", "r");
    if (ficheiro == NULL) {
        perror("Error opening file");
        return;
    }
     
    char linha[82];
    char *arr_char[6];
    int i = 0;
    int num_random = rand() % 6;

    while (i<6)
    {
   if (fgets(linha, sizeof(linha), ficheiro) != NULL) {
            remove_newline(linha);
            // Allocate memory for each line and copy the contents
             if (is_empty_or_whitespace(linha)) {
                continue;
            }
            arr_char[i] = malloc(strlen(linha) + 1);
            if (arr_char[i] != NULL) {
                strcpy(arr_char[i], linha);
            }
        }
    i++;
   }
   strcpy(linha,arr_char[num_random]);
   for (int j = 0; j < 6; j++) {
        if (arr_char[j] != NULL) {
            free(arr_char[j]); // Free the allocated memory
        }
    }
   text_to_grid(linha, jogos);
   retira(jogos);
   jogo(jogos,client_socket);
   fclose(ficheiro);
}


void ler_ficheiroConf(struct confServer * server,char * nomeFicheiro){
    char linha[256];
    //  int porta;
    // char server_ip[INET_ADDRSTRLEN];
    FILE *ficheiro = fopen(nomeFicheiro, "r");
    if (ficheiro == NULL) {
        perror("Error opening file");
        return;
    }
       while (fgets(linha, sizeof(linha), ficheiro) != NULL) {
        // Remover nova linha, se existir
        size_t len = strlen(linha);
        if (len > 0 && linha[len - 1] == '\n') {
            linha[len - 1] = '\0';
        }

        // Verificar se a linha contém "SERVER_IP"
        if (strncmp(linha, "SERVER_IP:", 10) == 0) {
            // Copiar o IP após "SERVER_IP:"
            
            strcpy(server->ip_server, linha + 10);
            printf("%s\n",server->ip_server);
            
        }
        // Verificar se a linha contém "PORTA"
        else if (strncmp(linha, "PORTA:", 6) == 0) {
            // Converter a porta de string para inteiro
            server->porta = atoi(linha + 6);
            printf("%d\n",server->porta);
        }
    }



    fclose(ficheiro);
    
}
