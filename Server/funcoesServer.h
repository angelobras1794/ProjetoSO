#include <stdbool.h>


struct jogoSoduku
{
    int  idJogo;
    int *tabuleiro[9][9];
    int *solucTabuleiro[9][9];
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