#include "common.h"
#include "functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 500
#define indexPokemon 40 //tamanho máximo de pokemons
#define nomePokemon 11 //tamanho máximo do nome do pokemon

char pokedex[indexPokemon][nomePokemon];
int indexPokedex = 0;

void processMessage(char *message, char *response);
void sendResponse(int csock, char *response);
int receiveMessage(int csock, char *buf);

void usage(int argc, char **argv){
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    int isWrongUsage = argc < 3;
    if (isWrongUsage){
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)){
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1){
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))){
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))){
        logexit("bind");
    }

    if (0 != listen(s, 10)){
        logexit("listen");
    }

    printf("[log] server is listening on %s:%s\n", argv[1], argv[2]);

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    while (1){
        printf("[log] accepting connections\n");
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1){
            logexit("accept");
        }

        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n\n\n", caddrstr);

        char buf[BUFSZ];
        size_t count;
        while (1){
            printf("[log] receiving message\n");
            memset(buf, 0, BUFSZ); 
            count = receiveMessage(csock, buf);

            if (strcmp(buf, "kill") == 0){
                printf("[log] kill message received.\n");
                fflush(stdout);
                break;
            }

            printf("[log] received %li bytes\n", count);
            printf("[msg] %s", buf);
            fflush(stdout);

            char response[BUFSZ];
            memset(response, 0, BUFSZ);

            processMessage(buf, response);

            if (strcmp(response, "kill") == 0){
                break;
            }

            printf("[log] sending response: %s\n\n\n", response);

            memcpy(buf, response, BUFSZ);
            strcat(buf, "\n");
            sendResponse(csock, buf);
        }
        sendResponse(csock, "\n");
        close(csock);
    }

    exit(EXIT_SUCCESS);
}

void sendResponse(int csock, char *response){
    size_t count = 0;

    while (count < strlen(response)){
        count += send(csock, response + count, strlen(response) - count, 0);
    }
}

int receiveMessage(int csock, char *buf){
    size_t count = 0;
    memset(buf, 0, BUFSZ);

    while (buf[strlen(buf) - 1] != '\n'){
        count += recv(csock, buf + count, BUFSZ - count, 0);
    }

    return count;
}

void processMessage(char *message, char *response)
{
    if (strstr(message, "add")){
        //handleAdd(message, response);
    }
    else if (strstr(message, "remove")){
        //handleRemove(message, response);
    }
    else if (strstr(message, "exchange")){
        //handleExchange(message, response);
    }
    else{
        memcpy(response, "kill", strlen("kill"));
    }
}