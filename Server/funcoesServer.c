#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include "funcoesServer.h"
#include "cJSON.h"

#define TAM 6
#define N_CLIENTES 100
#define MAX_SALAS  3
#define MAX_JOGADORES 1


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

void jogo(struct jogoSoduku * Jogo,int client_socket){
    int linha, coluna,valor,id_cliente;
    int jogadas = 0;
    int acabaJogo=0;

    FILE * ficheiro = NULL;
    char mensagem[100];
    recv(client_socket,&id_cliente,sizeof(id_cliente),0);
    strcpy(mensagem, "\n Bem vindo ao jogo do Soduku\n");
    send(client_socket,&mensagem,sizeof(mensagem),0);
   //printf("\n Bem vindo ao jogo do Soduku\n");
    while (verificaFimJogo(Jogo))
    {
    //envia o jogo para o cliente    
    send(client_socket,&Jogo->tabuleiro,sizeof(Jogo->tabuleiro),0);
    printf("%d\n\n",Jogo->tabuleiro[0][1]);
    mostra_grid(Jogo->tabuleiro);
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
    escrever_logs(id_cliente,"2 - O servidor recebe a resposta(linha,coluna)");
    escrever_logs(id_cliente,"3 - O servidor verifica a resposta(linha,coluna)");
    coluna--;
    printf("%d\n",coluna);
    if(Jogo->tabuleiro[linha][coluna] == 0){
        escrever_logs(id_cliente,"3 - O servidor envia a resposta");
        strcpy(mensagem,"true");
        send(client_socket,&mensagem,sizeof(mensagem),0);
        strcpy(mensagem, "\nPorfavor insira o valor\n");
        send(client_socket,&mensagem,sizeof(mensagem),0); //Acontece ate aqui
        recv(client_socket,&valor,sizeof(valor),0);
        escrever_logs(id_cliente,"2 - O servidor recebe a resposta");
        printf("%d\n",valor);
        escrever_logs(id_cliente,"3 - O servidor verifica a resposta(valor)");
        if(verifica_jogada(Jogo,linha,coluna,valor)){
            strcpy(mensagem, "true");
            send(client_socket,mensagem,sizeof(mensagem),0);
            Jogo->tabuleiro[linha][coluna] = valor;
            jogadas++;
            strcpy(mensagem, "\nJogada efetuada com sucesso\n Faca A sua proxima jogada\n"); //
            send(client_socket,&mensagem,sizeof(mensagem),0); 
            escrever_logs(id_cliente,"4 - O servidor envia a resposta"); //
            //send(client_socket,&Jogo->tabuleiro,sizeof(Jogo->tabuleiro),0); 
            if(verificaFimJogo(Jogo)==0){
            acabaJogo=0;
            send(client_socket,&acabaJogo,sizeof(int),0);
            }else{
                acabaJogo=1;
            send(client_socket,&acabaJogo,sizeof(int),0);    
            }
        }else{
           strcpy(mensagem,"false");
           send(client_socket,mensagem,sizeof(mensagem),0);
           sprintf(mensagem, "Erro: O valor %d ja existe na linha ou coluna ou quadrado\n", valor);
           escrever_logs(id_cliente,mensagem);
           send(client_socket,&mensagem,sizeof(mensagem),0); 
           escrever_logs(id_cliente,"4 - O servidor envia a resposta");
           send(client_socket,&Jogo->tabuleiro,sizeof(Jogo->tabuleiro),0);
        }
    }else{
        escrever_logs(id_cliente,"3 - O servidor envia a resposta");
        strcpy(mensagem,"false");
        send(client_socket,mensagem,sizeof(mensagem),0);
        sprintf(mensagem,"\nErro: A posicao (%d,%d) ja esta preenchida. Tenta outra posicao\n",linha+1,coluna+1);
        escrever_logs(id_cliente,mensagem);
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



// Função para resolver o Sudoku (backtracking)
int solveSudoku(int *grid[9][9], int row, int col) {
    // If we have filled all rows, return 1 (solution found)
    if (row == 9) {
        return 1;
    }

    // If we have filled all columns in the current row, go to the next row
    if (col == 9) {
        return solveSudoku(grid, row + 1, 0);
    }

    // If the current cell already has a value, move to the next cell
    if (grid[row][col] != 0) {
        return solveSudoku(grid, row, col + 1);
    }

    // Try all possible values (1-9) for the current cell
    for (int num = 1; num <= 9; num++) {
        // Check if it's a valid number
        if (isValid(grid, row, col, num)) {
            // Place the number and recursively try to solve the next cells
            grid[row][col] = num;
            if (solveSudoku(grid, row, col + 1)) {
                return 1;
            }
            // Backtrack if placing num doesn't lead to a solution
            grid[row][col] = 0;
        }
    }

    // If no valid number can be placed, return 0 (no solution)
    return 0;
}





bool isValid(int *grid[9][9], int linha, int coluna, int valor){
    //verificar se o valor esta na linha
    for (int i = 0; i < 9; i++)
    {
        if(grid[linha][i] == valor){
            printf("O valor ja existe na linha\n Tenta outro valor\n");
            return false;
        }
    }
    //verificar se o valor esta na coluna
    for (int i = 0; i < 9; i++)
    {
        if(grid[i][coluna] == valor){
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
            if(grid[i][j] == valor){
                printf("O valor ja existe no quadrado\nTenta outro valor\n");
                return false;
            }
        }
    }
    return true;

}




void processarMensagem(char mensagem[100], int client_socket,struct jogoSoduku *salas,char salasDisponiveis[][100],int *totalSalas) {
    char acao[20];
    char parametro[90];
    char resposta_servidor[100];
    int id_cliente;
    char escreveLog[100];

    // Dividir a mensagem em ação e parâmetro
    int scanned = sscanf(mensagem, "%[^:]:%[^:]:%d", acao, parametro, &id_cliente);
    printf("Scanned: %d, Ação: '%s', Parâmetro: '%s', ID Cliente: '%d'\n", scanned, acao, parametro, id_cliente);
    printf("Jogadores na SALA1: %d , socket : %d , id : %d, : tabuleiro[0][1] : %d \n ",salas[1].nJogadores,salas[1].jogadores[0].client_socket,salas[1].jogadores[0].id,salas[1].tabuleiro[0][1]);

    if (strcmp(acao,"entrar_em_sala") == 0) {
        printf("ENTAR SALA\n");
        int sala_encontrada = 0;
        int sala_escolhida = atoi(parametro);
        for (int i = 0; i < MAX_SALAS; i++) {
            if (i==sala_escolhida-1) {
                sala_encontrada = 1;
                sprintf(escreveLog,"O user com id %d esta a tentar entrar na sala %d",id_cliente,sala_escolhida);
                printf("salas[i].nJogadores : %d\n", salas[i].nJogadores );
                escrever_logs(id_cliente,escreveLog);
                if(salas[i].nJogadores < MAX_JOGADORES){
                salas[i].nJogadores++;
                printf("Client_Socket:%d\n",client_socket);
                entraClienteSala(client_socket,i,salas,id_cliente);
                strcpy(resposta_servidor,"Entrada na sala com sucesso");
                send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
                printf("Jogador entrou na sala '%s'\n", salas[i].nome);
                sprintf(escreveLog,"O user com id %d conseguiu entrar na sala %d",id_cliente,sala_escolhida);
                escrever_logs(id_cliente,escreveLog);
                //VERIFICAR SE PODE COMECAR O jogo
                if(salas[i].nJogadores == MAX_JOGADORES){
                 send(client_socket,"true",4,0);  
                 jogo(&salas[i],client_socket);

                }
                send(client_socket,"fals",4,0);
                }else{
                strcpy(resposta_servidor,"Sala atingiu o limite maximo de jogadores");
                send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);   
                sprintf(escreveLog,"O user com id %d nao conseguiu entrar na sala %d porque esta cheia",id_cliente,sala_escolhida);
                escrever_logs(id_cliente,escreveLog);
                }
                break;
            }
        }
        if (!sala_encontrada) {
            strcpy(resposta_servidor,"Sala nao encontrada");
            send(client_socket,&resposta_servidor,sizeof(resposta_servidor),0);
            sprintf(escreveLog,"A sala (%d) que o o user %d tentou entrar nao foi encontrada",id_cliente,sala_escolhida);
            escrever_logs(id_cliente,escreveLog);
        }
     } 
   else if (strcmp(acao,"criar_sala") == 0) {
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
           /// 
        }else if (*totalSalas < MAX_SALAS) {
            printf("criar SALAS4242\n");
            printf("o parametro :%s\n",parametro);
            strcpy(salas[(*totalSalas)].nome,parametro); 
            printf("o parametro depois :%s\n",parametro);
            verifica_ID(client_socket,salas);
            (*totalSalas)++;
            printf("\ntotal Salas: %d\n",*totalSalas);
            strcpy(resposta_servidor,"Sala criada com sucesso");
            printf("%s\n", resposta_servidor);
            send(client_socket,&resposta_servidor,sizeof(resposta_servidor), 0);
            printf("Sala '%s' criada\n", parametro);
            sprintf(escreveLog,"O user %d criou a sala com o nome %s",id_cliente,parametro);
            escrever_logs(id_cliente,escreveLog);
            int indice = *totalSalas;
            printf("indixe : %d\n",indice);
            
            if(load_sudoku_game("Jogos.json",salas,indice)){
                printf("deu certo \n");
            }
        } else {
            strcpy(resposta_servidor,"Limite de salas atingido\n");
            send(client_socket,&resposta_servidor,sizeof(resposta_servidor), 0);
            printf(escreveLog,"O user %d nao conseguiu criar a sala com nome %s, porque foi atingido o limite de salas",id_cliente,parametro);
            escrever_logs(id_cliente,escreveLog);
        }
        
    }
    else if(strcmp(acao,"salasCriadas") == 0){
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
          
    }else{
        send(client_socket,"Comando invalido\n",18, 0);
    }
    return;
}


void entraClienteSala(int client_socket,int i,struct jogoSoduku* salas,int id){
   for(int y=0;y<MAX_JOGADORES;y++){
    if(salas[i].jogadores[y].id == 0){
       salas[i].jogadores[y].id = id;
       salas[i].jogadores[y].client_socket = client_socket;  
       salas[i].jogadores[y].pontos =0;  
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
    mostra_grid(game[index].tabuleiro);

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

    
    cJSON_Delete(root);
    printf("Error: 'tabuleiroInic' is not an array4\n");
    return 1;  // Successfully loaded the game
}

int verificaFimJogo(struct jogoSoduku* game){
    for(int i =0;i<9;i++){
        for(int j = 0 ;j<9;j++){
            if(game->tabuleiro[i][j]==0){return 1;}
        }
    }
    return 0;
}




