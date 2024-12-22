#define _XOPEN_SOURCE 900
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
// #include <bits/pthreadtypes.h>

#include "sync.h"

#define MAX_Jogadores 2
#define MAX_SALAS  3




struct Cliente
{
    int id;                 // ID do cliente
    int client_socket;      // ID do socket
    int pontos;

};


struct jogoSoduku
{
    int idSala;
    char nome[100];
    int nJogadores;
    int nJogadoresEspera;
    struct Cliente jogadores[MAX_Jogadores];
    int idTabuleiro;
    int *tabuleiroJogavel[9][9]; // Tabuleiro que vai ser jogado
    int *tabuleiro[9][9];      // Tabuleiro como array bidimensional
    int *solucTabuleiro[9][9];  // Solução do tabuleiro
    struct simple_barrier_t barreira;
    // struct  Semaphore  sem;
    sem_t sem;
    pthread_mutex_t trinco;


    
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
// int solveSudoku(int *grid[9][9], int row, int col);
// bool isValid(int *grid[9][9], int linha, int coluna, int valor);


void processarMensagem(char mensagem[100], int client_socket,struct jogoSoduku *salas,char salasDisponiveis[][100],int *totalSalas,struct mutex_threads *mutexes,int *totalClientes);
void entraClienteSala(int client_socket,int i,struct jogoSoduku* salas,int id);
void gerarSalasDisponiveis(int *totalSalas, struct jogoSoduku* salas, char salasDisponiveis[][100]);
int load_sudoku_game(const char *filename, struct jogoSoduku *game,int index);
int verificaFimJogo(struct jogoSoduku* game);
bool is_playable(int * board[9][9]);
void jogo3(int linha,int coluna,int valor,struct jogoSoduku * Jogo,int client_socket,int id_cliente);
void atualizaPontos(struct jogoSoduku *game,int id_cliente,int client_socket);
void resetaSala(struct jogoSoduku *game);
void init(struct jogoSoduku *game);
