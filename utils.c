#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include "utils.h"
#include <stdio.h>
#include <string.h> //memset
#include "mensagens.h"

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
    // para entender melhor só descomentar esses printf
    // 2 x (2 char id) + 4 char enlace + 2 char espaço + 1 \n
    char linha[11];

    do {
    
        if(!fgets(linha, 11, file)){ 
            // printf("\nagr to alocando ");

            // printf("%u \n",i);
            // se chegou aqui significa q lemos todo o arquivo, alocamos espaço para todos roteadores vizinhos 
            roteadores = (Roteador *)malloc(sizeof(Roteador) * i);
            // finaliza a recursão 
            nucleo.qtdVizinhos = i;
        };
        // separa a linha lida 
        sscanf(linha, "%u %u %u", &roteador1, &roteador2, &enlace);
        // printf("l: %s",linha);
    } while (roteador1 != nucleo.id && roteador2 != nucleo.id );
    
    // se o roteador atual faz parte do enlace ele para aqui e chama a função +1
    readEnlaces(i + 1, file);
    // printf("es: %u %u %u\n", roteador1, roteador2, enlace);
    roteadores[i].id = roteador1 ^ roteador2 ^ nucleo.id ;
    roteadores[i].enlace = enlace;
}

void readRoteadores(uint max, FILE *file){
    int hit = max;
    uint id, porta;
    // 2 char id + 6 char porta + 15 char ip + 2 char espaco + 1 \n
    char linha[26], ip[16];
    while(fgets(linha, 26, file) && hit){
        sscanf(linha, "%u %u %s", &id, &porta, ip);
        for (uint i = 0; i < max; i++){
            if(roteadores[i].id != id) continue;

            strncpy(roteadores[i].endereco.ip, ip, 16);
            roteadores[i].endereco.porta = porta;
            hit--;
            break;
        }
    }
}