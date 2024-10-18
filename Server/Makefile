mainServer: mainServer.o funcoesServer.o
	gcc -o mainServer mainServer.o funcoesServer.o -lpthread

mainServer.o: mainServer.c 
	gcc -g -c mainServer.c
	
funcoesServer.o: funcoesServer.c funcoesServer.h
	gcc -g -c funcoesServer.c

clean:
	rm -f *.o mainServer
