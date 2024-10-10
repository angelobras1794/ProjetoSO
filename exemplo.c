#include <stdio.h>
#include <stdlib.h>
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

void escreveLogServer(char *log)
{
    FILE * ficheiro;
    ficheiro=fopen("logs.txt","ab");

    fprintf(ficheiro, "%s\n", log);
    fclose(ficheiro);
}





int main(){
   mostra_grid("530070000600195000098000060800060003400803001700020006060000280000419005000080079");
   return 0;
}