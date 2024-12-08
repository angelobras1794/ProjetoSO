#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <strings.h> // For bzero


struct confCliente
{
   char ip_server[INET_ADDRSTRLEN];  
   int porta; 
   int id;   

};


void jogoAutonomo(int client_socket,int id_cliente,int opcaoSala){
    char mensagem[100];
    int * tabuleiro[9][9]; // Tabuleiro como array bidimensional
    int linhaRandom, colunaRandom;
    int sala=opcaoSala - 1;
    srand(time(NULL));   // Inicializar a semente aleatória
    
    while (1) {
        // Receber o tabuleiro do servidor
        recv(client_socket,tabuleiro,sizeof(tabuleiro),0);
        mostra_grid(&tabuleiro); // Exibir o tabuleiro atualizado

        // Procurar uma posição vazia
        do {
            linhaRandom = rand() % 9;  // De 0 a 8
            colunaRandom = rand() % 9; // De 0 a 8
        } while (tabuleiro[linhaRandom][colunaRandom] != 0);

        // Escolher o valor (aqui, você pode implementar lógica adicional)
        int valorEscolhido = (rand() % 9) + 1; // Exemplo: sempre jogar 1

        // Enviar a jogada formatada
        sprintf(mensagem, "jogo:%d,%d,%d,%d:%d",sala,linhaRandom, colunaRandom, valorEscolhido, id_cliente);
        send(client_socket, mensagem, sizeof(mensagem), 0);
        printf("Jogada enviada: linha=%d, coluna=%d, valor=%d\n", linhaRandom, colunaRandom, valorEscolhido);

        usleep(2000000); // fica 2 segundos parado

        recv(client_socket,mensagem,sizeof(mensagem),0);
        printf("%s\n",mensagem);

        usleep(2000000); // fica 2 segundos parado
        // system("clear"); //limpar a consola

        //verificar se o jogo acabou
        recv(client_socket,mensagem,sizeof(mensagem),0);
        if(strcmp(mensagem,"end")==0){
            printf("acabei o jogo lol\n");
            break;
        }
    }
    //o jogo acabou
    recv(client_socket,mensagem,sizeof(mensagem),0);
    printf("%s\n",mensagem);

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
    fprintf(ficheiro, "%d [%s] %s\n",id_user,time_str, mensagem);
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
    char mensagem[100];

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
        mostraMenuJogar(client_socket,Cliente_id);
        break;
    case 2:
        printf("esta na opcao estatisticas");
        break;
    case 3:
        printf("A sair do jogo");
        sprintf(mensagem,"sair:SAIR:%d",Cliente_id);
        send(client_socket,&mensagem,sizeof(mensagem),0);    
    default:
        printf("input invalido");
        break;
    }

    }while(menu_option != 3);


}

void mostraMenuJogar(int client_socket,int Cliente_id) {
    int menu_option;
    char mensagem[100];
    int entraJogo;
    

    do {
        printf("Bem vindo Cliente N%d\n",Cliente_id);
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
                    printf("Debug: opcaoSala = %d, Cliente_id = %d\n", opcaoSala,Cliente_id);
                    sprintf(mensagem,"entrar_em_sala:%d:%d",opcaoSala,Cliente_id);
                    
                    send(client_socket,&mensagem,sizeof(mensagem),0); //+1 ?
                    recv(client_socket,&mensagem,sizeof(mensagem),0);
                    printf("%s\n",mensagem); //resposta do servidor (sucesso na criacao ou nao)
                    memset(mensagem, 0, sizeof(mensagem));
                    recv(client_socket,&mensagem,sizeof(mensagem),0);
                     printf("%s\n",mensagem);
                    if(strcmp(mensagem,"true") == 0){
                    printf("ola tudo bem amigo\n");
                    jogoAutonomo(client_socket,Cliente_id,opcaoSala);
                    }
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




int main(int argc,int *argv[]){
    struct confCliente *configuracao = malloc(sizeof(struct confCliente)); //alocar espaco para a struct
    ler_ficheiroConf(configuracao,argv[1]);

    int Cliente_id = configuracao->id;
    printf("id_cliente: %d\n",Cliente_id);
    
    int network_socket;
    
    network_socket = socket(AF_INET,SOCK_STREAM,0);//socket(dominio,tipo,protocolo) , AF_INET -> IPV4 , sock_stream -> TCP

    //especificacao um endereco para o socket

    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET; //especeficacao do endereco de familia 
    server_address.sin_port = htons(configuracao->porta);
    inet_pton(AF_INET, configuracao->ip_server, &server_address.sin_addr); //passa
    
    
   int connection_status = connect(network_socket,(struct sockaddr*)&server_address,sizeof(server_address));
   
   if(connection_status==-1){
        printf("houve um erro");
    
   }else{
        // geraId(Cliente_id,network_socket);
        mostraMenuPrincipal(network_socket,Cliente_id);
   }
   close(network_socket);
    
   return 0;
}

void ler_ficheiroConf(struct confCliente * cliente,char * nomeFicheiro){
    char linha[256];
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
            
            strcpy(cliente->ip_server, linha + 10);
            printf("%s\n",cliente->ip_server);
            
        }
        // Verificar se a linha contém "PORTA"
        else if (strncmp(linha, "PORTA:", 6) == 0) {
            // Converter a porta de string para inteiro
            cliente->porta = atoi(linha + 6);
            printf("%d\n",cliente->porta);
        }
        else if (strncmp(linha, "ID:", 3) == 0) {
            // Converter a porta de string para inteiro
            cliente->id = atoi(linha + 3);
            printf("%d\n",cliente->id);
        }
    }

    fclose(ficheiro);
    
}




