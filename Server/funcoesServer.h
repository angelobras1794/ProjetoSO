#define _XOPEN_SOURCE 900
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>



#include "sync.h"
#include "estatistica.h"
#include "syncBarbeiro.h"

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
    sem_t sem;
    struct estatisticaSala estatistica;
    bool JogoAtivo;
    pthread_mutex_t trinco;
    time_t tempoInicio;
    time_t tempoFim;
    struct PriorityQueue fila;
    int modoJogo;
    struct barberShop barberShop;


    
};

struct confServer
{
   char ip_server[INET_ADDRSTRLEN];  
   int porta;    

};

struct argsFila{
    struct estatisticaServer *estatistica;
    struct jogoSoduku *game;
    struct prodCons *prodCons;
};






void mostra_grid(int *tabuleiro[9][9]);
void text_to_grid(char *text, struct jogoSoduku * Jogo);
bool verifica_jogada(struct jogoSoduku * Jogo, int linha, int coluna, int valor);
// void escrever_logs(int id_user,char *mensagem);
void remove_newline(char *str);
int is_empty_or_whitespace(const char *str);
void ler_ficheiroConf(struct confServer * server,char * nomeFicheiro);

void processarMensagem(char mensagem[200], int client_socket,struct jogoSoduku *salas,char salasDisponiveis[][100],int *totalSalas,struct mutex_threads *mutexes,struct estatisticaServer *estatistica,int * prioridade,struct prodCons *prodCons);
void entraClienteSala(int client_socket,int i,struct jogoSoduku* salas,int id);
void gerarSalasDisponiveis(int *totalSalas, struct jogoSoduku* salas, char salasDisponiveis[][100]);
int load_sudoku_game(const char *filename, struct jogoSoduku *game,int index);
int verificaFimJogo(struct jogoSoduku* game);
bool is_playable(int * board[9][9]);
void jogo3(int linha,int coluna,int valor,struct jogoSoduku * Jogo,int client_socket,int id_cliente,struct estatisticaServer *estatistica,struct prodCons *prodCons);
void jogoFIFO(int linha,int coluna,int valor,struct jogoSoduku * Jogo,int client_socket,int id_cliente,struct estatisticaServer *estatistica);
void atualizaPontos(struct jogoSoduku *game,int id_cliente,int client_socket);
void resetaSala(struct jogoSoduku *game);
void init(struct jogoSoduku *game);
void salasInit(struct jogoSoduku *salas);
bool percorrer_arr(struct jogoSoduku *salas,int sala_id);

//ESTATISTICAS

void geraEstatisticasSala(struct jogoSoduku *game,int maxJogadores);
void escreveEstatisticaSala(struct jogoSoduku *game,int clientSocket);
void formatTime(double timeInSeconds, char *buffer, size_t bufferSize);

int jogadorTentaJogar(struct jogoSoduku* sala, int clientId, int socket, int linha, int coluna, int valor,struct estatisticaSala *estatistica,struct prodCons *prodCons);
void  barbeiroAtende(void* arg);
void inicializarBarbeiros(struct jogoSoduku* sala);
const char* bool_to_string(bool value);


void AtendedorFila(struct argsFila* args);
void * filaAtende(void *args);