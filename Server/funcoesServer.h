#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAX_Jogadores 1


struct Cliente
{
    int id;                 // ID do cliente
    int client_socket;      // ID do socket
    int pontos;

};


struct jogoSoduku
{
    int idSala;
    char nome[80];
    int nJogadores;
    struct Cliente jogadores[MAX_Jogadores];
    int idTabuleiro;
    int *tabuleiro[9][9];
    int *solucTabuleiro[9][9];
    
};





struct confServer
{
   char ip_server[INET_ADDRSTRLEN];  
   int porta;    

};


void mostra_grid(int *tabuleiro[9][9]);
void retira(struct jogoSoduku * Jogo);
void text_to_grid(char *text, struct jogoSoduku * Jogo);
bool verifica_jogada(struct jogoSoduku * Jogo, int linha, int coluna, int valor);
void jogo(struct jogoSoduku * Jogo,int client_socket);
void escrever_logs(int id_user,char *mensagem);
void criarJogo(FILE *ficheiro, struct jogoSoduku *jogos,int client_socket);
void remove_newline(char *str);
int is_empty_or_whitespace(const char *str);
void ler_ficheiroConf(struct confServer * server,char * nomeFicheiro);
void removeNumbers(int *grid[9][9], int count);
int isSolvable(int *grid[9][9]);
int solveSudoku(int *grid[9][9], int row, int col);
bool isValid(int *grid[9][9], int linha, int coluna, int valor);


void processarMensagem(char mensagem[100], int client_socket,struct jogoSoduku *salas,char salasDisponiveis[][100],int *totalSalas);
void entraClienteSala(int client_socket,int i,struct jogoSoduku* salas,int id);
void gerarSalasDisponiveis(int *totalSalas, struct jogoSoduku* salas, char salasDisponiveis[][100]);
int load_sudoku_game(const char *filename,struct jogoSoduku *game);