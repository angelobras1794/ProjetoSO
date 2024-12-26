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
#define MAX_JOGADORES 2

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




void jogo3(int linha,int coluna,int valor,struct jogoSoduku * Jogo,int client_socket,int id_cliente,struct estatisticaServer *estatistica,int * prioridade){
    char mensagem[100];
    printf("A prioridade desta thread (INICIO): %d\n",*prioridade);
    if(Jogo->tabuleiroJogavel[linha][coluna] != 0){
        strcpy(mensagem,"Jogada invalida, a casa foi preenchida por outro jogador\n");
        send(client_socket,&mensagem,sizeof(mensagem),0);
        
    }
    else if(verifica_jogada(Jogo,linha,coluna,valor)){
            Jogo->tabuleiroJogavel[linha][coluna] = valor;
             if (Jogo->solucTabuleiro[linha][coluna]==valor) {
                strcpy(mensagem, "\nJogada efetuada com sucesso, mais 10 Pontos Ganhos\n Faca A sua proxima jogada\n"); //
                send(client_socket,&mensagem,sizeof(mensagem),0);
                atualizaPontos(Jogo,id_cliente,client_socket);
                if (Jogo->modoJogo == 1){
                (*prioridade)--;
                }
                
            } else {
                strcpy(mensagem,"Jogada válida, mas a solucao nao 'e a correta. Revertendo...\n");
                send(client_socket,&mensagem,sizeof(mensagem),0); 
                Jogo->tabuleiroJogavel[linha][coluna] = 0; // Reverte a jogada
            }      
    }
    else{
            if (Jogo->modoJogo == 1){
                (*prioridade)++;
                }
            sprintf(mensagem, "Erro: O valor %d ja existe na linha ou coluna ou quadrado\n", valor);
            escrever_logs(id_cliente,mensagem);
            send(client_socket,&mensagem,sizeof(mensagem),0);  
    }

    if(verificaFimJogo(Jogo)==0){
            strcpy(mensagem,"end");
            printf("o jogo acabou\n");
            send(client_socket,&mensagem,sizeof(mensagem),0);
            strcpy(mensagem,"\nParabens voce terminou o jogo\n");
            send(client_socket,&mensagem,sizeof(mensagem),0);
            Jogo->nJogadores--;
            if(Jogo->nJogadores == 0){
                Jogo->tempoFim = time(NULL);
                pthread_mutex_lock(&estatistica->trincoEstatistica);
                geraEstatisticasSala(Jogo,MAX_JOGADORES);
                resetaSala(Jogo);
                estatistica->tabuleirosResolvidos++;  //rever quando utilizarmos tricos por sala
                estatistica->tabuleirosEmResolucao--;
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
   printf("A prioridade desta thread (FINAL): %d\n",*prioridade);
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
    fprintf(ficheiro, "%d [%s] %s\n",id_user,time_str, mensagem);
    fclose(ficheiro);
     

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


void processarMensagem(char mensagem[100], int client_socket,struct jogoSoduku *salas,char salasDisponiveis[][100],int *totalSalas,struct mutex_threads *mutexes,struct estatisticaServer *estatistica,int * prioridade) {
    char acao[20];
    char parametro[90];
    char resposta_servidor[100];
    int id_cliente;
    char escreveLog[100];

    // Dividir a mensagem em ação e parâmetro
    int scanned = sscanf(mensagem, "%[^:]:%[^:]:%d", acao, parametro, &id_cliente);
    printf("Scanned: %d, Ação: '%s', Parâmetro: '%s', ID Cliente: '%d'\n", scanned, acao, parametro, id_cliente);
    printf("Prioridade %d\n",*prioridade);

   
    
    
    if (strcmp(acao,"entrar_em_sala") == 0) {
        
        pthread_mutex_lock(&mutexes->entrar_sala);
        printf("ENTAR SALA\n");
        int sala_encontrada = 0;
        int valorS;
        int sala_escolhida = atoi(parametro);
        for (int i = 0; i < MAX_SALAS; i++) {
            if (i==sala_escolhida-1 && salas[i].idSala!=0) {
                sala_encontrada = 1;
                sprintf(escreveLog,"O user com id %d esta a tentar entrar na sala %d",id_cliente,sala_escolhida);
                escrever_logs(id_cliente,escreveLog);
                    
                    entraClienteSala(client_socket,i,salas,id_cliente);
                    printf("Jogador %d\n",salas[i].jogadores[0].id);
                    printf("Jogador %d\n",salas[i].jogadores[1].id);
                    strcpy(resposta_servidor,"Entrada na sala com sucesso");
                    send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
                
                    sprintf(escreveLog,"O user com id %d conseguiu entrar na sala %d",id_cliente,sala_escolhida);
                    escrever_logs(id_cliente,escreveLog);

                    salas[i].nJogadoresEspera++;
                    pthread_mutex_unlock(&mutexes->entrar_sala);
                    printf("Jogador entrou do semaforo\n");
                    sem_wait(&salas[i].sem);
                    
                    printf("Jogador saiu no semaforo\n");
                    pthread_mutex_lock(&mutexes->entrar_sala);   
                    salas[i].nJogadores++;
                    salas[i].nJogadoresEspera--;
                    pthread_mutex_unlock(&mutexes->entrar_sala);


                    //BARREIRA PARA ESPERAR QUE TODOS OS JOGADORES ENTRAM NA SALA
                    simple_barrier_wait(&salas[i].barreira);

                    pthread_mutex_lock(&mutexes->entrar_sala);
                    if(salas[i].JogoAtivo == false){
                        salas[i].JogoAtivo = true;
                        estatistica->tabuleirosEmResolucao++;
                        salas[i].tempoInicio = time(NULL); // Record the current time as the start time
                        printf("Jogo Ativo\n");
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
            sprintf(escreveLog,"A sala (%d) que o o user %d tentou entrar nao foi encontrada",id_cliente,sala_escolhida);
            escrever_logs(id_cliente,escreveLog);
            pthread_mutex_unlock(&mutexes->entrar_sala);
        }
        //DesBloqueia o acesso a sala
        
        
     } 
   else if (strcmp(acao,"criar_sala") == 0) {
        pthread_mutex_lock(&mutexes->criar_sala);
        printf("criar SALAS \n");
        sprintf(escreveLog,"O user %d esta a tentar criar a sala com o nome %s",id_cliente,parametro);
        escrever_logs(id_cliente,escreveLog);
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
            escrever_logs(id_cliente,escreveLog);
         
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
            sprintf(escreveLog,"O user %d criou a sala com o nome %s",id_cliente,parametro);
            escrever_logs(id_cliente,escreveLog);
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
            printf(escreveLog,"O user %d nao conseguiu criar a sala com nome %s, porque foi atingido o limite de salas",id_cliente,parametro);
            escrever_logs(id_cliente,escreveLog);
        }
        pthread_mutex_unlock(&mutexes->criar_sala);
        
    }
    else if(strcmp(acao,"salasCriadas") == 0){
          pthread_mutex_lock(&mutexes->criar_sala);
          sprintf(escreveLog,"O user %d esta a tentar entrar numa sala",id_cliente);
          escrever_logs(id_cliente,escreveLog);
          printf("\nSALAS CRIADAS\n");
          int verifica = send(client_socket,totalSalas,sizeof(int),0);  //aqui tinha um problema
          if(*totalSalas != 0 ){
          gerarSalasDisponiveis(totalSalas,salas,salasDisponiveis);
          for(int z=0;z<*totalSalas;z++){
            printf("Salas Disponivies %s\n",salasDisponiveis[z]);
          }
          send(client_socket,salasDisponiveis,*totalSalas * 100,0);
          }else{
          sprintf(escreveLog,"O user %d nao consegiu entrar em nenhuma sala, porque nao ha salas disponiveis",id_cliente);
          escrever_logs(id_cliente,escreveLog);
          }
          pthread_mutex_unlock(&mutexes->criar_sala);
          
    }else if(strcmp(acao,"jogo") == 0){
         
        printf("JOGO\n");
        const char delimitador[] = ",";

        int sala, linha, coluna, valor;
         
        
               
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
        
        
        
        if(salas[sala].modoJogo == 1){
            
        
        printf("A prioridade da thread : %d %d\n",*prioridade,&prioridade);
        enqueue(&salas[sala].fila, id_cliente,prioridade,client_socket, linha, coluna, valor);
        
    
        struct ClientRequest* request = dequeue(&salas[sala].fila);
        //FILA DE Prioridade
        
        if (request) {
            pthread_mutex_lock(&salas[sala].trinco);
            jogo3(request->line, request->column, request->value, &salas[sala], request->socket,request->clientId,estatistica,request->priority);
            free(request);
            usleep(5000000);
            pthread_mutex_unlock(&salas[sala].trinco);
        }
        } 
        else{
           //MODO DE JOGO 2 BArbeiros
           pthread_mutex_lock(&salas[sala].trinco);
           if(salas[sala].barberShop.barbeiroChegou==0){
                salas[sala].barberShop.barbeiroChegou=1;
                salas[sala].barberShop.barbeariaAberta=1;
                inicializarBarbeiros(&salas[sala]);
                printf("Barbeiro chegou\n");
           }
            pthread_mutex_unlock(&salas[sala].trinco);
            jogadorTentaJogar(&salas[sala], id_cliente,client_socket,linha,coluna,valor,estatistica,prioridade);//rever isto tambem

        }

    }
    else if(strcmp(acao,"sair") == 0){
        printf("SAIR\n");
        sprintf(escreveLog,"O user %d esta a tentar sair da ligacao",id_cliente);
        escrever_logs(id_cliente,escreveLog);
        pthread_mutex_lock(&mutexes->sair_sala);
        (estatistica->clientesConectados)--;
        pthread_mutex_unlock(&mutexes->sair_sala);
        printf("Clientes conectados: %d\n", estatistica->clientesConectados);
        close(client_socket);

    }
    else if(strcmp(acao,"estatisticas") == 0){
        printf("ESTATISTICA\n");
        sprintf(escreveLog,"O user %d esta a tentar ver as estatisticas",id_cliente);
        escrever_logs(id_cliente,escreveLog);
        if(strcmp(parametro,"servidor") == 0){
            printf("Estatistica do Servidor\n");
            pthread_mutex_lock(&estatistica->trincoEstatistica);    
            escreveEstatisticaServer(estatistica,client_socket);
            pthread_mutex_unlock(&estatistica->trincoEstatistica);
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

            }else{
               strcpy(resposta_servidor,"false");
                send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
                strcpy(resposta_servidor,"Sala nao encontrada");
                send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);

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
        if(game->jogadores[i].id == id_cliente){
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
        salas[i].modoJogo = 0;
        createPriorityQueue(&salas[i].fila);
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
        printf("Barbeiro SAIU.\n");
        pthread_detach(thread);  // Detach para não precisar gerenciar os joins
    
}




void jogadorTentaJogar(struct jogoSoduku* sala, int clientId, int socket, int linha, int coluna, int valor,struct estatisticaSala *estatistica,int prioridade) {
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
        jogo3(linha,coluna, valor,sala,socket,clientId,estatistica,&prioridade); //verificar isto
        printf("Jogador %d acabou de jogar.\n", clientId);
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