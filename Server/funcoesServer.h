
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
void jogo(struct jogoSoduku * Jogo);
void escrever_logs(int id_user,char *mensagem);
void criarJogo(FILE *ficheiro, struct jogoSoduku *jogos);
