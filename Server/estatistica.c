#include "estatistica.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void estatisticaServerInit(struct estatisticaServer * estatistica){
    estatistica->clientesConectados = 0;
    estatistica->tabuleirosResolvidos = 0;
    estatistica->tabuleirosEmResolucao = 0;

}



void escreveEstatisticaServer(struct estatisticaServer *estatistica,int clientSocket){
     char mensagem[200];
    sprintf(mensagem,"Estatisticas do Servidor \n");
    char buffer[50];
    sprintf(buffer, "Clientes Conectados: %d\n",estatistica->clientesConectados);
    printf("%d\n",estatistica->clientesConectados);
    strcat(mensagem, buffer);

    sprintf(buffer, "Tabuleiros Resolvidos: %d\n", estatistica->tabuleirosResolvidos);
    strcat(mensagem, buffer);

    sprintf(buffer, "Tabuleiros em Resolucao: %d\n", estatistica->tabuleirosEmResolucao);
    strcat(mensagem, buffer);
    send(clientSocket,&mensagem,sizeof(mensagem),0);


    
}