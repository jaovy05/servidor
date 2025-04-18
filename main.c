
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include "mensagens.h"
#include "utils.h"
#include "queue.h"

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
    readEnlaces(0, f);
    fclose(f);

    qsort(roteadores, nucleo.qtdVizinhos, sizeof(Roteador), cmp);

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

    pthread_join(t_sender, NULL);    // sender decide se o programa encerra 
    
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
    int id, op;
    char buffer[BUFLEN + 3];

    while (1){
        printf("+--------------------------------------+\n");
        printf("| %-37s|\n", "Escolha uma opção");
        printf("| %-37s|\n", "0 - Digitar uma mensagem");
        printf("| %-37s|\n", "1 - Sair");
        printf("+--------------------------------------+\n");

        scanf("%d", &op);
        if(op) return NULL;
        
        while (getchar() != '\n'); // limpa o buffer

        printf("Informe a mensagem no formato: <id> <msg>\n");
        fflush(stdout);    
        fgets(buffer, BUFLEN + 3, stdin);
        if (sscanf(buffer, "%d %[^\n]", &id, msg.data) != 2) {
            perror("Formato inválido. Use: <id> <mensagem>\n");
            continue;
        }

        msg.tipoMensagem = DADOS;

        Roteador *r = findById(id);
        if(!r) printf("Roteador não existe ou não é vizinho");

        msg.destino = r->endereco;
        si_other.sin_port = htons(msg.destino.porta);
        
        if (sendto(s, &msg, sizeof(msg), 0 ,(struct sockaddr *) &si_other, sizeof(si_other))==-1) die("sendto()");
        
    }

    return NULL;
}