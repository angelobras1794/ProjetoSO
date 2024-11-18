
# Target: mainServer and its dependencies
mainServer: mainServer.o funcoesServer.o cJSON.o
	gcc -o mainServer mainServer.o funcoesServer.o cJSON.o -lpthread

# Compile the main server source file
mainServer.o: mainServer.c
	gcc -g -c mainServer.c

# Compile the funcoesServer source file and include cJSON
funcoesServer.o: funcoesServer.c funcoesServer.h cJSON.h
	gcc -g -c funcoesServer.c

# Compile the cJSON library
cJSON.o: cJSON.c cJSON.h
	gcc -g -c cJSON.c

# Clean up object files and executable
clean:
	rm -f *.o mainServer