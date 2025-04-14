/*
    Simple udp server
    Silver Moon (m00n.silv3r@gmail.com)
*/
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "mensagens.h"
#include "utils.h"

#define BUFLEN 101  //Max length of buffer

Roteador *roteadores = NULL;
RoteadorNucleo nucleo;

void *sender(void *socket);

void *receiver(void *sockect){
    while (1){

    }
    return NULL;
}


int main(int argc, char *argv[]){
    if(argc != 2){ 
        char buffer[80];
        sprintf(buffer, "Execute no formato: %s <id do roteador>", argv[0]);
        die(buffer);
    }
    nucleo.id = atoi(argv[1]);

    FILE *f;
    if((f = fopen("enlaces.config", "r")) == NULL) die("Erro ao abrir enlaces.config");
    fclose(f);

    if((f = fopen("roteador.config", "r")) == NULL) die("Erro ao abrir roteador.config");
    readRoteadores(nucleo.qtdVizinhos, f);
    fclose(f);

    struct sockaddr_in si_me;
    int sock = makeSocket(si_me, nucleo.endereco.porta);

    // para o prinft n se cagar inteiro
    setvbuf(stdout, 0, _IONBF, 0) ;

    pthread_t t_receiver, t_sender;

    pthread_create(&t_receiver, NULL, receiver, &sock);
    pthread_create(&t_sender, NULL, sender, &sock);

    pthread_join(t_receiver, NULL); // para n encerrar a thread principal sem as outras encerrarem 
    pthread_join(t_sender, NULL);    
    
    close(sock);
    return 0;
}

void *sender(void *socket) {
    struct sockaddr_in si_other;

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    int s = *(int *)socket;
    Mensagem msg;
   
    msg.fonte = nucleo.endereco;
    si_other.sin_port = htons(34534);

    while (1){
        printf("digita aí vai");
        fflush(stdin);
        scanf("%s", msg.data);
        msg.tipoMensagem = DADOS;

        strcpy(msg.destino.ip, "127.0.0.1");
        msg.destino.porta = 12345;

        if (sendto(s, &msg, sizeof(msg), 0 ,(struct sockaddr *) &si_other, sizeof(si_other))==-1) die("sendto()");
        
    }

    return NULL;
}