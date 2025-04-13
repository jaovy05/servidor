#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include "utils.h"
#include <stdio.h>
#include <string.h> //memset

uint id = 2;

void die(char *s)
{
    perror(s);
    exit(1);
}

int makeSocket(struct sockaddr_in si_me, int port){
    int s;
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) die("socket");
    
    // zera a var
    memset((char *) &si_me, 0, sizeof(si_me));
 
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
 
    //bind socket to port
    if(bind(s, (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)  die("bind");
    return s;
}

void readEnlaces(uint i, FILE *file){
    uint roteador1, roteador2, enlace;
    // 2 x (2 char id) + 4 char enlace + 2 char espaço + 1 \n
    char linha[11];
    // para entender melhor só descomentar esses printf
    do {
    
        if(!fgets(linha, 11, file)){ 
            // printf("\nagr to alocando ");

            // printf("%u \n",i);
            // se chegou aqui significa q lemos todo o arquivo, alocamos espaço para todos roteadores vizinhos 
            roteadores = (Roteador *)malloc(sizeof(Roteador) * i);
            // finaliza a recursão 
            return ;
        };
        // separa a linha lida 
        sscanf(linha, "%u %u %u", &roteador1, &roteador2, &enlace);
        // printf("l: %s",linha);
    } while (roteador1 != id && roteador2 != id);
    
    // se o roteador atual faz parte do enlace ele para aqui e chama a função +1
    readEnlaces(i + 1, file);
    // printf("es: %u %u %u\n", roteador1, roteador2, enlace);
    // após ter a variavel roteadores alocada corretamente saí registrando os enlaces
    roteadores[i].enlace = enlace;
}

// void readRoteadores(uint i, FILE *file){
//     // 2 byte id + 6 byte porta + 15 bytes ip + 2 byte espaco + 1 \n
//     char linha[26];
//     fgets(linha, 26, file);
//     linha[strcspn(linha, "\n")] = 0; // corta pra fora

//     char token = strtok(linha, " ");
//     int id = atoi(token);

//     char token = strtok(linha, " ");
//     int porta = atoi(token);

//     char token = strtok(linha, " ");
//     int ip = atoi(token);
// }