#include <stdio.h>
#include <stdlib.h>
 
//sockets
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <string.h>

void escreveLogServer(char *log)
{
    FILE * ficheiro;
    ficheiro=fopen("logs.txt","ab");

    fprintf(ficheiro, "%s\n", log);
    fclose(ficheiro);
}

void ler_ficheiroConf(){
    char linha[256];
     int porta;
    char server_ip[INET_ADDRSTRLEN];
    FILE *ficheiro = fopen("server.conf", "r");
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
            
            strcpy(server_ip, linha + 10);
            printf("%s\n",server_ip);
            
        }
        // Verificar se a linha contém "PORTA"
        else if (strncmp(linha, "PORTA:", 6) == 0) {
            // Converter a porta de string para inteiro
            porta = atoi(linha + 6);
            printf("%d\n",porta);
        }
    }



    fclose(ficheiro);
    
}
    







int main(){
   ler_ficheiroConf();
   return 0;
}