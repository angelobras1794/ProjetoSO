#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>


#include <netinet/in.h>
#define TAM 6

struct jogoSoduku
{
    int  idJogo;
    int *tabuleiro[9][9];
    int *solucTabuleiro[9][9];
};
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

void criarJogo(FILE *ficheiro, struct jogoSoduku *jogos) {
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
   jogo(jogos);
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
            Jogo->solucTabuleiro[i][j] = text[i * 9 + j] - '0'; // Convert char to int
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

void jogo(struct jogoSoduku * Jogo){
    int linha, coluna,valor;
    int jogadas = 0;
    FILE * ficheiro = NULL;
    
    printf("\n Bem vindo ao jogo do Soduku\n");
    while (jogadas < 4)
    {
    mostra_grid(Jogo->tabuleiro);
    printf("Porfavor insira a linha\n");
    scanf("%d", &linha);
    linha--;   
    printf("Porfavor insira a coluna\n");
    scanf("%d", &coluna);
    coluna--;
    if(Jogo->tabuleiro[linha][coluna] == 0){
        printf("Porfavor insira o valor\n");
        scanf("%d", &valor);
        if(verifica_jogada(Jogo, linha, coluna, valor)){
            Jogo->tabuleiro[linha][coluna] = valor;
            jogadas++;
            printf("Jogada efetuada com sucesso\n Faca A sua proxima jogada\n");
        }
    }else{
            printf("A posicao ja esta preenchida , Tenta outra posicao\n");
     }
    
   }
    printf("Parabens, Jogo concluido\n");
    mostra_grid(Jogo->tabuleiro);
    free(Jogo);
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

int main(){
    srand(time(NULL));
    FILE * ficheiro;
    struct jogoSoduku *jogos = malloc(sizeof(struct jogoSoduku));
    if (jogos == NULL) {
        perror("Error allocating memory");
        return 1;
    }
    criarJogo(ficheiro, jogos);
    return 0;
}

// 1 - enviar o jogo
// 2 - receber a jogada do cliente
// 3 - verificar a jogada
// 4 - enviar a resposta ao cliente
