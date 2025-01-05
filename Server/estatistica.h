#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>




struct estatisticaServer{
    int clientesConectados;
    int tabuleirosResolvidos;
    int tabuleirosEmResolucao;
    pthread_mutex_t trincoEstatistica;

};

struct estatisticaSala{
    char jogaodorRecorde[100];
    int pontuacaoRecorde;
    int pontuacaoMinima;
    double maiorTempo;
    double menorTempo;
    double tempoMedio;
    int desistencias;
    int respostasErradas;
    pthread_mutex_t trincoEstatistica;
};


void estatisticaServerInit(struct estatisticaServer * estatistica);
void escreveEstatisticaServer(struct estatisticaServer *estatistica,int clientSocket);
