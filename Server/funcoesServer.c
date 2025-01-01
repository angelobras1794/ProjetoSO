#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h> 
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include "funcoesServer.h"
#include "cJSON.h"
#include <pthread.h>
#include <fcntl.h> // Include this header for O_CREAT

#define TAM 6
#define N_CLIENTES 100
#define MAX_SALAS  3
#define MAX_JOGADORES 3

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




void jogo3(int linha,int coluna,int valor,struct jogoSoduku * Jogo,int client_socket,int id_cliente,struct estatisticaServer *estatistica,struct prodCons *prodCons){
    char mensagem[100];
    char escreveLogs[200];
    if(Jogo->tabuleiroJogavel[linha][coluna] != 0){
         sprintf(escreveLogs, "Erro:O cliente %d com socket %d tentou o valor %d mas a casa (%d , %d)  foi preenchida por outro jogador na sala %d",id_cliente,client_socket,valor,linha+1,coluna+1,Jogo->idSala);
        prodProduz(prodCons,escreveLogs);
        strcpy(mensagem,"Jogada invalida, a casa foi preenchida por outro jogador\n");
        send(client_socket,&mensagem,sizeof(mensagem),0);
        
    }
    else if(verifica_jogada(Jogo,linha,coluna,valor)){
            Jogo->tabuleiroJogavel[linha][coluna] = valor;
             if (Jogo->solucTabuleiro[linha][coluna]==valor) {
                 sprintf(escreveLogs, "O cliente %d com socket %d conseguiu preencher a casa (%d , %d) com o valor %d na sala %d",id_cliente,client_socket,linha+1,coluna+1,valor,Jogo->idSala);
                 prodProduz(prodCons,escreveLogs);
                strcpy(mensagem, "\nJogada efetuada com sucesso, mais 10 Pontos Ganhos\n Faca A sua proxima jogada\n"); //
                send(client_socket,&mensagem,sizeof(mensagem),0);
                atualizaPontos(Jogo,id_cliente,client_socket);
                
            } else {
                 sprintf(escreveLogs, "O cliente %d com socket %d nao conseguiu preencher a casa (%d , %d) com o valor %d na sala %d visto que a solucao nao era a correta",id_cliente,client_socket,linha+1,coluna+1,valor,Jogo->idSala);
                 prodProduz(prodCons,escreveLogs);
                strcpy(mensagem,"Jogada válida, mas a solucao nao 'e a correta. Revertendo...\n");
                send(client_socket,&mensagem,sizeof(mensagem),0); 
                Jogo->tabuleiroJogavel[linha][coluna] = 0; // Reverte a jogada
            }      
    }
    else{
            
            sprintf(mensagem, "Erro: O valor %d ja existe na linha ou coluna ou quadrado\n", valor);
             sprintf(escreveLogs, "Erro:O cliente %d com socket %d tentou o valor %d na casa (%d , %d) mas ele ja existe na linha ou coluna ou quadrado na sala %d",id_cliente,client_socket,valor,linha+1,coluna+1,Jogo->idSala);
            prodProduz(prodCons,escreveLogs);
            send(client_socket,&mensagem,sizeof(mensagem),0);  
    }

    if(verificaFimJogo(Jogo)==0){
            strcpy(mensagem,"end");
            printf("o jogo acabou\n");
            send(client_socket,&mensagem,sizeof(mensagem),0);
            strcpy(mensagem,"\nParabens voce terminou o jogo\n");
            send(client_socket,&mensagem,sizeof(mensagem),0);
            Jogo->nJogadores--;
            sprintf(escreveLogs,"O cliente %d com socket %d terminou o jogo na sala %d",id_cliente,client_socket,Jogo->idSala);
             prodProduz(prodCons,escreveLogs);
            mostra_grid(Jogo->tabuleiroJogavel);
            if(Jogo->nJogadores == 0){
                imprimePontucao(Jogo);
                Jogo->tempoFim = time(NULL);
                pthread_mutex_lock(&estatistica->trincoEstatistica);
                geraEstatisticasSala(Jogo,MAX_JOGADORES);
                resetaSala(Jogo);
                estatistica->tabuleirosResolvidos++;  //rever quando utilizarmos tricos por sala
                estatistica->tabuleirosEmResolucao--;
                sprintf(escreveLogs,"sala %d resetada",Jogo->idSala);
                prodProduz(prodCons,escreveLogs);
                pthread_mutex_unlock(&estatistica->trincoEstatistica);
                if(Jogo->modoJogo==2){
                    Jogo->barberShop.barbeariaAberta = 0;
                }
            }
    }else{
            printf("o jogo continua\n");
            strcpy(mensagem,"continue");
            send(client_socket,&mensagem,sizeof(mensagem),0);
            send(client_socket,&Jogo->tabuleiroJogavel,sizeof(Jogo->tabuleiroJogavel),0);
        }
        

}




void ler_ficheiroConf(struct confServer * server,char * nomeFicheiro){
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


void processarMensagem(char mensagem[100], int client_socket,struct jogoSoduku *salas,char salasDisponiveis[][100],int *totalSalas,struct mutex_threads *mutexes,struct estatisticaServer *estatistica,struct prodCons *prodCons) {
    char acao[20];
    char parametro[90];
    char resposta_servidor[100];
    int id_cliente;
    char escreveLog[100];

    // Dividir a mensagem em ação e parâmetro
    int scanned = sscanf(mensagem, "%[^:]:%[^:]:%d", acao, parametro, &id_cliente);
    printf("Scanned: %d, Ação: '%s', Parâmetro: '%s', ID Cliente: '%d'\n", scanned, acao, parametro, id_cliente);

   
    
    
    if (strcmp(acao,"entrar_em_sala") == 0) {
        
        pthread_mutex_lock(&mutexes->entrar_sala);
        printf("ENTRAR SALA\n");
        int sala_encontrada = 0;
        int valorS;
        int sala_escolhida = atoi(parametro);
        for (int i = 0; i < MAX_SALAS; i++) {
            if (i==sala_escolhida-1 && salas[i].idSala!=0) {
                sala_encontrada = 1;
                sprintf(escreveLog,"O user com id %d esta a tentar entrar na sala %d",id_cliente,sala_escolhida);
                prodProduz(prodCons,escreveLog);
                    
                    entraClienteSala(client_socket,i,salas,id_cliente);
                    strcpy(resposta_servidor,"Entrada na sala com sucesso");
                    send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
                
                    sprintf(escreveLog,"O user %d com socket %d conseguiu entrar na sala %d",id_cliente,client_socket,sala_escolhida);
                    prodProduz(prodCons,escreveLog);

                    salas[i].nJogadoresEspera++;
                    pthread_mutex_unlock(&mutexes->entrar_sala);
                    printf("Jogador entrou do semaforo\n");
                    sem_wait(&salas[i].sem);
                    
                    printf("Jogador saiu no semaforo\n");
                    pthread_mutex_lock(&mutexes->entrar_sala);   
                    salas[i].nJogadores++;
                    salas[i].nJogadoresEspera--;
                    pthread_mutex_unlock(&mutexes->entrar_sala);


                    sprintf(escreveLog, "O cliente %d com socket %d esta na barreira na sala %d\n",id_cliente,client_socket,salas[i].idSala);
                    prodProduz(prodCons,escreveLog);
                    simple_barrier_wait(&salas[i].barreira);

                    pthread_mutex_lock(&mutexes->entrar_sala);
                    if(salas[i].JogoAtivo == false){
                        salas[i].JogoAtivo = true;
                    pthread_mutex_lock(&estatistica->trincoEstatistica);   //ver se da eroo  
                        estatistica->tabuleirosEmResolucao++;
                    pthread_mutex_unlock(&estatistica->trincoEstatistica);    
                        salas[i].tempoInicio = time(NULL); // Record the current time as the start time
                        printf("Jogo Ativo\n");
                        sprintf(escreveLog, "Foi iniciado o jogo na sala %d\n",salas[i].idSala);
                        prodProduz(prodCons,escreveLog);
                    }
                    pthread_mutex_unlock(&mutexes->entrar_sala);
                    
                    //INICIO DO JOGO
                    strcpy(resposta_servidor,"true"); 
                    send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
                    send(client_socket,&salas[i].tabuleiroJogavel,sizeof(salas[i].tabuleiroJogavel),0);
                
                break;
            }
        }
        if (!sala_encontrada) {
            strcpy(resposta_servidor,"false"); 
            send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
            strcpy(resposta_servidor,"Sala nao encontrada");
            send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
            sprintf(escreveLog,"A sala (%d) que o o user %d com socket %d tentou entrar nao foi encontrada",sala_escolhida,id_cliente,client_socket);
            prodProduz(prodCons,escreveLog);
            pthread_mutex_unlock(&mutexes->entrar_sala);
        }
        //DesBloqueia o acesso a sala
        
        
     } 
   else if (strcmp(acao,"criar_sala") == 0) {
        pthread_mutex_lock(&mutexes->criar_sala);
        printf("criar SALAS \n");
        sprintf(escreveLog,"O user %d com client socket %d esta a tentar criar a sala com o nome %s",id_cliente,client_socket,parametro);
        prodProduz(prodCons,escreveLog);
        int sala_ja_existe = 0;
        for (int i = 0; i < *totalSalas; i++) {
            if (strcmp(salas[i].nome,parametro) == 0) {
                sala_ja_existe = 1;
                break;
            }
        }
        
        if (sala_ja_existe) {
            strcpy(resposta_servidor,"Sala ja existe");
            send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
            sprintf(escreveLog,"A sala (%s) que o o user %d tentou criar ja existe",parametro,id_cliente);
            prodProduz(prodCons,escreveLog);
         
        }else if (*totalSalas < MAX_SALAS) {
            const char delimitador[] = ",";
            int  modoJogo;
            char nomeSala[100];
            char *token = strtok(parametro,delimitador); // Primeira parte
            if (token != NULL) strcpy(nomeSala, token);
            printf("Sala: %s\n", nomeSala);
            token = strtok(NULL, delimitador); // segunda parte
            if (token != NULL) modoJogo = atoi(token);
            printf("Modo de Jogo: %d\n", modoJogo);
            
            strcpy(salas[(*totalSalas)].nome,nomeSala); 
            salas[*totalSalas].modoJogo = modoJogo;
            salas[*totalSalas].idSala=(*totalSalas)+1;
           
            (*totalSalas)++;
            
            strcpy(resposta_servidor,"Sala criada com sucesso");
            printf("%s\n", resposta_servidor);
            send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);

            printf("Sala '%s' criada\n", parametro);
            sprintf(escreveLog,"O user %d com sokcet %d criou a sala com o nome %s e com o modo de jogo %d",id_cliente,client_socket,parametro,modoJogo);
            prodProduz(prodCons,escreveLog);
            int indice = *totalSalas;
            
            if(load_sudoku_game("Jogos.json",salas,indice)){
                printf("deu certo \n");
            }
            //INICIALIZACAO DA BARREIRA E DO TRINCO
            int index = indice -1;
            if(modoJogo == 2){
                barberShop_init(&salas[index].barberShop,3);
            }
             sem_init(&salas[index].sem, 0, MAX_JOGADORES);
             init(&salas[index]);   
            
        } else {
            strcpy(resposta_servidor,"Limite de salas atingido\n");
            send(client_socket,&resposta_servidor,sizeof(resposta_servidor), 0);
            printf(escreveLog,"O user %d com socket nao conseguiu criar a sala com nome %s, porque foi atingido o limite de salas",id_cliente,client_socket ,parametro);
            prodProduz(prodCons,escreveLog);
        }
        pthread_mutex_unlock(&mutexes->criar_sala);
        
    }
    else if(strcmp(acao,"salasCriadas") == 0){
          pthread_mutex_lock(&mutexes->criar_sala);
          sprintf(escreveLog,"O user %d com socket %d esta a aceder aos dados das salas",id_cliente,client_socket);
          prodProduz(prodCons,escreveLog);
          printf("\nSALAS CRIADAS\n");
          int verifica = send(client_socket,totalSalas,sizeof(int),0);  //aqui tinha um problema
          if(*totalSalas != 0 ){
          gerarSalasDisponiveis(totalSalas,salas,salasDisponiveis);
          for(int z=0;z<*totalSalas;z++){
            printf("Salas Disponivies %s\n",salasDisponiveis[z]);
          }
          send(client_socket,salasDisponiveis,*totalSalas * 100,0);
          }else{
          sprintf(escreveLog,"O user %d com socket %d nao consegiu aceder aos dados das salas, porque nao ha salas disponiveis",id_cliente,client_socket);
          prodProduz(prodCons,escreveLog);
          }
          pthread_mutex_unlock(&mutexes->criar_sala);
          
    }else if(strcmp(acao,"jogo") == 0){
         
        printf("JOGO\n");
        const char delimitador[] = ",";

        int sala, linha, coluna, valor,prioridade;
         
        
               
        // Tokenizar e converter para inteiro
        char *token = strtok(parametro,delimitador); // Primeira parte
        if (token != NULL) sala = atoi(token);
        printf("Sala: %d\n", sala);
       

        token = strtok(NULL, delimitador); // segunda parte
        if (token != NULL) linha = atoi(token);
        printf("Linha: %d\n", linha);

        token = strtok(NULL, delimitador); // terceira parte
        if (token != NULL) coluna = atoi(token);
        printf("Coluna: %d\n", coluna);

        token = strtok(NULL, delimitador); // Última parte
        if (token != NULL) valor = atoi(token);
        printf("Valor: %d\n", valor);

        token = strtok(NULL, delimitador); // Última parte
        if (token != NULL) prioridade = atoi(token);
        printf("Prio: %d\n", prioridade);
        
        sprintf(escreveLog, "O user %d com socket %d enviou a jogada (%d,%d) para a sala %d",id_cliente,client_socket,linha,coluna,sala+1);
        prodProduz(prodCons,escreveLog);
        
        if(salas[sala].modoJogo == 1){    
            enqueue(&salas[sala].fila, id_cliente,client_socket, linha, coluna, valor);
            sprintf(escreveLog, "A jogada (%d,%d) do user %d com socket %d  da sala %d esta na fila FIFO",id_cliente,client_socket,linha,coluna,sala+1);
            prodProduz(prodCons,escreveLog);
            
            if (salas[sala].fila.iniciarJogo == true && salas[sala].fila.atendedorOn==false) //verificar isto
            {
                salas[sala].fila.atendedorOn==true;
                struct argsFila * args = malloc(sizeof(struct argsFila));
                args->game= &salas[sala];
                args->estatistica = estatistica;
                args->prodCons = prodCons;
                pthread_t thread;
                pthread_create(&thread, NULL, filaAtende, (void *)args);
                pthread_detach(thread);  // Detach para não precisar gerenciar os joins

            }
        } 
        else if (salas[sala].modoJogo == 2){
                char mensagem[100];
             //MODO DE JOGO 2 BArbeiros
                pthread_mutex_lock(&salas[sala].trinco);
                if(salas[sala].barberShop.barbeiroChegou==0){
                    salas[sala].barberShop.barbeiroChegou=1;
                    salas[sala].barberShop.barbeariaAberta=1;
                    inicializarBarbeiros(&salas[sala]);
                    printf("Barbeiro chegou\n");
                    }
                pthread_mutex_unlock(&salas[sala].trinco);
                if(jogadorTentaJogar(&salas[sala], id_cliente,client_socket,linha,coluna,valor,estatistica,prodCons)==1){//JOGADOR DESISTIU DA JOGADA
                    strcpy(mensagem,"desistencia");
                    send(client_socket,&mensagem,sizeof(mensagem),0); 
                    pthread_mutex_lock(&salas[sala].estatistica.trincoEstatistica);
                    salas[sala].estatistica.desistencias++;
                    pthread_mutex_unlock(&salas[sala].estatistica.trincoEstatistica);
                    send(client_socket,&salas[sala].tabuleiroJogavel,sizeof(salas[sala].tabuleiroJogavel),0);
                    sprintf(escreveLog, "O user %d com socket %d que enviou a jogada (%d,%d) para a sala %d desistiu porque a fila do barbeiro estava cheia",id_cliente,client_socket,linha,coluna,sala+1);
                    prodProduz(prodCons,escreveLog);
                }

        }
        else if(salas[sala].modoJogo == 3){
            enqueuePriority(&salas[sala].filaPrioridade, id_cliente,client_socket, linha, coluna, valor,prioridade);
            sprintf(escreveLog, "A jogada (%d,%d) do user %d com socket %d com prioridade %d  da sala %d esta na fila FIFO de prioridades",linha,coluna,id_cliente,client_socket,prioridade,sala+1);
            prodProduz(prodCons,escreveLog);
            
            if (salas[sala].filaPrioridade.iniciarJogo == true && salas[sala].filaPrioridade.atendedorOn==false) //verificar isto
            {
                salas[sala].filaPrioridade.atendedorOn==true;
                struct argsFila * args = malloc(sizeof(struct argsFila));
                args->game= &salas[sala];
                args->estatistica = estatistica;
                args->prodCons = prodCons;
                pthread_t thread;
                pthread_create(&thread, NULL, filaAtendePrio, (void *)args);
                pthread_detach(thread);  // Detach para não precisar gerenciar os joins

            }
            
        }
        

    }
    else if(strcmp(acao,"sair") == 0){
        printf("SAIR\n");
        sprintf(escreveLog,"O user %d com socket %d esta a tentar sair da ligacao",id_cliente,client_socket);
        prodProduz(prodCons,escreveLog);
        pthread_mutex_lock(&mutexes->sair_sala);
        (estatistica->clientesConectados)--;
        pthread_mutex_unlock(&mutexes->sair_sala);
        printf("Clientes conectados: %d\n", estatistica->clientesConectados);
        close(client_socket);

    }
    else if(strcmp(acao,"estatisticas") == 0){
        printf("ESTATISTICA\n");
        sprintf(escreveLog,"O user %d com socket %d esta a tentar ver as estatisticas",id_cliente,client_socket);
        prodProduz(prodCons,escreveLog);
        if(strcmp(parametro,"servidor") == 0){
            printf("Estatistica do Servidor\n");
            pthread_mutex_lock(&estatistica->trincoEstatistica);    
            escreveEstatisticaServer(estatistica,client_socket);
            pthread_mutex_unlock(&estatistica->trincoEstatistica);
            sprintf(escreveLog,"O user %d com socket %d esta a ver as estatisticas do server",id_cliente,client_socket);
            prodProduz(prodCons,escreveLog);
        }
        else{
            printf("Estatistica da Sala\n");
            int sala = atoi(parametro) - 1;
            if(salas[sala].idSala != 0){
                strcpy(resposta_servidor,"true");
                send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
                pthread_mutex_lock(&salas[sala].trinco);
                escreveEstatisticaSala(&salas[sala],client_socket);
                pthread_mutex_unlock(&salas[sala].trinco);
                sprintf(escreveLog,"O user %d com socket %d esta a ver as estatisticas da sala %d",id_cliente,client_socket,salas[sala].idSala);
                prodProduz(prodCons,escreveLog);

            }else{
               strcpy(resposta_servidor,"false");
                send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
                strcpy(resposta_servidor,"Sala nao encontrada");
                send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
                sprintf(escreveLog,"O user %d com socket %d nao conseguiu ver as estatisticas da sala %d, porque a sala nao foi encontrada",id_cliente,client_socket,sala);
                prodProduz(prodCons,escreveLog);

            }         

        }

    }
    else{
        printf("Ação desconhecida\n");
    }
    return;
}


void entraClienteSala(int client_socket,int i,struct jogoSoduku* salas,int id){
   for(int y=0;y<MAX_JOGADORES;y++){
    if(salas[i].jogadores[y].id == 0){
       salas[i].jogadores[y].id = id;
       salas[i].jogadores[y].client_socket = client_socket;  
       salas[i].jogadores[y].pontos =0;  
       break;
    }
   }
 }

void gerarSalasDisponiveis(int *totalSalas, struct jogoSoduku* salas, char salasDisponiveis[][100]) {
    for (int i = 0; i < *totalSalas; i++) {
        // Concatena id_sala e nome no formato "id_sala - nome"
        sprintf(salasDisponiveis[i], "%d - %s", salas[i].idSala, salas[i].nome);
    }
}


int load_sudoku_game(const char *filename, struct jogoSoduku *game,int index) {
    int N=9;
    index--;
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file %s\n", filename);
        return 0;
    }

    // Read the entire file content into a string
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *file_content = (char *)malloc(file_size + 1);
    if (file_content == NULL) {
        printf("Memory allocation error\n");
        fclose(file);
        return 0;
    }
    fread(file_content, 1, file_size, file);
    fclose(file);
    file_content[file_size] = '\0';

    // Parse JSON
    cJSON *root = cJSON_Parse(file_content);
    free(file_content);
    if (!root) {
        printf("Error parsing JSON\n");
        return 0;
    }

    // Get the "games" array from the JSON
    cJSON *games = cJSON_GetObjectItem(root, "games");
    if (!cJSON_IsArray(games)) {
        printf("Error: 'games' is not an array\n");
        cJSON_Delete(root);
        return 0;
    }

    // Get the first game (assuming we want the first game in the array)
     
    int jogoRandom = rand() % 2;
    cJSON *game_json = cJSON_GetArrayItem(games,jogoRandom);
    if (!game_json) {
        printf("Error: No game data found\n");
        cJSON_Delete(root);
        return 0;
    }

    cJSON *game_id = cJSON_GetObjectItem(game_json, "id");
    if (cJSON_IsNumber(game_id)) {
        game[index].idTabuleiro = game_id->valueint;  // Store the ID in the game structure
        printf("Loaded game ID: %d\n", game[index].idTabuleiro);
    } else {
        printf("Error: 'id' is not a valid number\n");
        cJSON_Delete(root);
        return 0;
    }


    // Check if 'tabuleiroInic' is an array
    cJSON *sudoku = cJSON_GetObjectItem(game_json, "tabuleiroInic");
    if (!cJSON_IsArray(sudoku)) {
        printf("Error: 'tabuleiroInic' is not an array\n");
        cJSON_Delete(root);
        return 0;
    }

    // Debug: Print the sudoku data type and structure
    printf("'tabuleiroInic' is an array, checking rows...\n");

    for (int i = 0; i < N; i++) {
        cJSON *row = cJSON_GetArrayItem(sudoku, i);
        if (!cJSON_IsArray(row)) {
            printf("Error: Row %d is not an array\n", i);
            cJSON_Delete(root);
            return 0;
        }

        // Print the row for debugging
        printf("Row %d: ", i);
        for (int j = 0; j < N; j++) {
            int value = cJSON_GetArrayItem(row, j)->valueint;
            game[index].tabuleiro[i][j] = value;  // Store the value
            printf("%d ", game[index].tabuleiro[i][j]);
        }
        printf("\n");
    }

    // Load the solution
     
    cJSON *solution = cJSON_GetObjectItem(game_json, "solucao");
    if (!cJSON_IsArray(solution)) {
        printf("Error: 'solucao' is not an array\n");
        cJSON_Delete(root);
        return 0;
    }
    for (int i = 0; i < N; i++) {
        cJSON *row = cJSON_GetArrayItem(solution, i);
        if (!cJSON_IsArray(row)) {
            printf("Error: Solution row %d is not an array\n", i);
            cJSON_Delete(root);
            return 0;
        }
        printf("Error: 'tabuleiroInic' is not an array1\n");
        for (int j = 0; j < N; j++) {
            
            game[index].solucTabuleiro[i][j] = cJSON_GetArrayItem(row, j)->valueint;
        }
    }

    memcpy(game[index].tabuleiroJogavel, game[index].tabuleiro, sizeof(game[index].tabuleiro));
    mostra_grid(game[index].tabuleiroJogavel);
    cJSON_Delete(root);
    printf("Error: 'tabuleiroInic' is not an array4\n");
    return 1;  // Successfully loaded the game
}

int verificaFimJogo(struct jogoSoduku* game){
    for(int i =0;i<9;i++){
        for(int j = 0 ;j<9;j++){
            if(game->tabuleiroJogavel[i][j]==0){return 1;}
        }
    }
    return 0;
}

void atualizaPontos(struct jogoSoduku *game,int id_cliente,int client_socket){
    for(int i =0;i<MAX_JOGADORES;i++){
        if(game->jogadores[i].client_socket == client_socket){
            game->jogadores[i].pontos+=10;
        }
    }
}

void resetaSala(struct jogoSoduku *game){
    for(int i =0;i<MAX_JOGADORES;i++){
        game->jogadores[i].id = 0;
        game->jogadores[i].client_socket = 0;
        game->jogadores[i].pontos = 0;
    }
    simple_barrier_init(&game->barreira, MAX_JOGADORES);
    game->JogoAtivo = false;
   
    memset(game->tabuleiroJogavel,'\0',sizeof(game->tabuleiroJogavel));
    memcpy(game->tabuleiroJogavel, game->tabuleiro, sizeof(game->tabuleiro));
    createQueue(&game->fila);
    clean_list(&game->fila);
    game->fila.iniciarJogo=false;
    game->fila.atendedorOn=false;
    game->barberShop.barbeiroChegou = 0;

    if(game->modoJogo != 2){
    for (int i = 0; i < MAX_JOGADORES; i++)
    {
        sem_post(&game->sem);
    }
    }
}


void init(struct jogoSoduku *game){
   
    pthread_mutex_init(&game->trinco, NULL);
  
    simple_barrier_init(&game->barreira, MAX_JOGADORES); 
 
}

void salasInit(struct jogoSoduku *salas){
    for(int i=0;i<MAX_SALAS;i++){
        salas[i].idSala=0;
        salas[i].nJogadores=0;
        salas[i].nJogadoresEspera=0;
        salas[i].idTabuleiro=0;
        salas[i].JogoAtivo=false;
        salas[i].estatistica.pontuacaoMinima = 100000;
        salas[i].estatistica.maiorTempo = 0;
        salas[i].estatistica.menorTempo = 0;
        salas[i].estatistica.pontuacaoRecorde = 0;
        salas[i].estatistica.desistencias=0;
        pthread_mutex_init(&salas[i].estatistica.trincoEstatistica,NULL);
        salas[i].modoJogo = 0;
        createQueue(&salas[i].fila);
        createPriorityQueue(&salas[i].filaPrioridade);
        strcpy(salas[i].estatistica.jogaodorRecorde,"");
        for(int j=0;j<MAX_JOGADORES;j++){
            salas[i].jogadores[j].id = 0;
            salas[i].jogadores[j].client_socket = 0;
            salas[i].jogadores[j].pontos = 0;
        }
    }

}

bool percorrer_arr(struct jogoSoduku *salas,int sala_id){
    for(int i=0;i<sizeof(salas);i++){
        if(salas[i].idSala == sala_id){
            return true;
        }
    }
     return false;


}

void geraEstatisticasSala(struct jogoSoduku *game,int maxJogadores){
    int maxPontos = game->estatistica.pontuacaoRecorde;
    int minPontos = game->estatistica.pontuacaoMinima;
    for(int i = 0; i<maxJogadores;i++){
        if(game->jogadores[i].pontos>=maxPontos){
            maxPontos = game->jogadores[i].pontos;
            strcpy(game->estatistica.jogaodorRecorde," ");
            strcpy(game->estatistica.jogaodorRecorde,"Jogador ");
            char buffer[10];
            sprintf(buffer,"%d",game->jogadores[i].id);
            strcat(game->estatistica.jogaodorRecorde,buffer);
        }
        if(game->jogadores[i].pontos<=minPontos){
            minPontos = game->jogadores[i].pontos;
        }
    }
    game->estatistica.pontuacaoMinima = minPontos;
    game->estatistica.pontuacaoRecorde = maxPontos;
    double tempo = difftime(game->tempoFim,game->tempoInicio);
    if (tempo > game->estatistica.maiorTempo) {
        game->estatistica.maiorTempo = tempo;
    }
    if (tempo < game->estatistica.menorTempo || game->estatistica.menorTempo == 0) {
        game->estatistica.menorTempo = tempo;
    }
       
}

void escreveEstatisticaSala(struct jogoSoduku * game,int clientSocket){
    char mensagem[200];
    char formattedTime[10];
    sprintf(mensagem,"Estatisticas da Sala %d : ",game->idSala);
    char buffer[50];
    sprintf(buffer, "Pontuacao Recorde: %s %d\n",game->estatistica.jogaodorRecorde ,game->estatistica.pontuacaoRecorde);
    strcat(mensagem, buffer);

    sprintf(buffer, "Pontuacao Minima: %d\n", game->estatistica.pontuacaoMinima);
    strcat(mensagem, buffer);
    if(game->modoJogo==2){
    sprintf(buffer,"Numero de Desistencias: %d\n", game->estatistica.desistencias);
    strcat(mensagem,buffer);
    }

    formatTime(game->estatistica.maiorTempo, formattedTime, sizeof(formattedTime));
    snprintf(buffer, sizeof(buffer), "Maior Tempo: %s\n", formattedTime);
    strcat(mensagem, buffer);

    formatTime(game->estatistica.menorTempo, formattedTime, sizeof(formattedTime));
    snprintf(buffer, sizeof(buffer), "Menor Tempo: %s\n", formattedTime);
    strcat(mensagem, buffer);
    send(clientSocket,&mensagem,sizeof(mensagem),0);

}

void formatTime(double timeInSeconds, char *buffer, size_t bufferSize) {
    int minutes = (int)timeInSeconds / 60;
    int seconds = (int)timeInSeconds % 60;
    snprintf(buffer, bufferSize, "%d:%02d", minutes, seconds);
}

//FUNCOES DO BARBEIRO

void  barbeiroAtende(void* arg) {
    struct jogoSoduku* sala = (struct jogoSoduku*)arg;

    while (sala->barberShop.barbeariaAberta) {
        sem_wait(&sala->barberShop.clientes);
        sem_post(&sala->barberShop.barbeiro);
        printf("Barbeiro atendendo cliente.\n");
        sem_wait(&sala->barberShop.barbeiroCompleto);
        sem_post(&sala->barberShop.clienteCompleto);
    }
    printf("Barbeiro terminou o trabalho.\n");
    for (size_t i = 0; i < MAX_JOGADORES; i++)
    {
        sem_post(&sala->sem);
    }
    
}

void inicializarBarbeiros(struct jogoSoduku* sala) {
        pthread_t thread;
        pthread_create(&thread, NULL, barbeiroAtende, sala);
        pthread_detach(thread);  // Detach para não precisar gerenciar os joins
    
}




int jogadorTentaJogar(struct jogoSoduku* sala, int clientId, int socket, int linha, int coluna, int valor,struct estatisticaSala *estatistica,struct prodCons *prodCons) {
    // Tentar sentar na fila de espera
    pthread_mutex_lock(&sala->barberShop.trinco);
    if (sala->barberShop.nClientes < sala->barberShop.maxclientes) {
        sala->barberShop.nClientes++;
        pthread_mutex_unlock(&sala->barberShop.trinco);
        sem_post(&sala->barberShop.clientes);
        printf("Jogador %d sentou na fila de espera.\n", clientId);
        sem_wait(&sala->barberShop.barbeiro);

        printf("Jogador %d está jogando.\n", clientId);
        // Simula a jogada
        jogo3(linha,coluna, valor,sala,socket,clientId,estatistica,prodCons); //verificar isto
        printf("Jogador %d acabou de jogar.\n", clientId);
        //  usleep(100000);
        // Libera o barbeiro
        sem_post(&sala->barberShop.barbeiroCompleto);
        sem_wait(&sala->barberShop.clienteCompleto);
        printf("Jogador %d terminou de jogar.\n", clientId);
        pthread_mutex_lock(&sala->barberShop.trinco);
        sala->barberShop.nClientes--;
        pthread_mutex_unlock(&sala->barberShop.trinco);
        return 0;
    } else {
        pthread_mutex_unlock(&sala->barberShop.trinco);
        printf("Jogador %d não encontrou espaço na fila de espera.\n", clientId);
        return 1;
    }
}


const char* bool_to_string(bool value) {
    return value ? "true" : "false";
}

void AtendedorFila(struct argsFila* args) {
        pthread_t thread;
        pthread_create(&thread, NULL, filaAtende, (void *)args);
        pthread_detach(thread);  // Detach para não precisar gerenciar os joins
    
}

void * filaAtende(void *args){
    struct argsFila * argus = (struct argsFila *)args;
    struct jogoSoduku *sala = argus->game;
    struct estatisticaServer* estatistica = argus->estatistica;
    struct prodCons *prodCons = argus->prodCons;

    while(sala->fila.head!=NULL){
        pthread_mutex_lock(&sala->trinco);
        printf("TOU DENTRO \n");    
        printf("b1: %s\n", bool_to_string(sala->fila.iniciarJogo)); 
        if (sala->fila.iniciarJogo == true) {   
        struct ClientRequest* request = dequeue(&sala->fila);
        if (request) {
        jogo3(request->line, request->column, request->value, sala, request->socket, request->clientId, estatistica,prodCons);
        free(request);
        }
        }
        pthread_mutex_unlock(&sala->trinco);
    }

}

//FILAS PRIORIDADE

void AtendedorFilaPrio(struct argsFila* args) {
        pthread_t thread;
        pthread_create(&thread, NULL, filaAtendePrio, (void *)args);
        pthread_detach(thread);  // Detach para não precisar gerenciar os joins
    
}

void * filaAtendePrio(void *args){
    struct argsFila * argus = (struct argsFila *)args;
    struct jogoSoduku *sala = argus->game;
    struct estatisticaServer* estatistica = argus->estatistica;
    struct prodCons *prodCons = argus->prodCons;
    printf("TOU DENTRO 1\n"); 
    while(sala->filaPrioridade.head!=NULL){
        pthread_mutex_lock(&sala->trinco);
        printf("TOU DENTRO \n");    
        printf("b1: %s\n", bool_to_string(sala->filaPrioridade.iniciarJogo)); 
        if (sala->filaPrioridade.iniciarJogo == true) {   
        struct ClientRequestPriority* request = dequeuePriority(&sala->filaPrioridade);
        if (request) {
        jogo3(request->line, request->column, request->value, sala, request->socket, request->clientId, estatistica,prodCons);
        free(request);
        }
        }
        pthread_mutex_unlock(&sala->trinco);
    }

}

void imprimePontucao(struct jogoSoduku * game){
    for(int i =0;i<MAX_JOGADORES;i++){
        printf("O jogador com socket %d teve %d pontos \n",game->jogadores[i].client_socket,game->jogadores[i].pontos);
    }
}