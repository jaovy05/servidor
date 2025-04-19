
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
    // Registra o roteador ativo
    nucleo.id = atoi(argv[1]);
    
    FILE *f;
    if((f = fopen("enlaces.config", "r")) == NULL) die("Erro ao abrir enlaces.config");
    readEnlaces(0, f); // Lê os enlaces. 
    fclose(f); // Fecha o arquivo.

    // Ordena os roteadores pelo id.
    qsort(roteadores, nucleo.qtdVizinhos, sizeof(Roteador), cmp); 

    if((f = fopen("roteador.config", "r")) == NULL) die("Erro ao abrir roteador.config");
    readRoteadores(f); // Salva os endereços dos roteadores.
    fclose(f); // Fecha o arquivo.

    // Cria o socket configurado.
    int sock = makeSocket(nucleo.endereco.porta);

    // Cria 1 thread para cada serviço.
    pthread_t t_receiver, t_sender, t_queue_sender;

    // Seta as respectivas funções 
    pthread_create(&t_receiver, NULL, receiver, &sock);
    pthread_create(&t_sender, NULL, sender, &sock);

    // O programa espera até a thread do sender encerrar 
    pthread_join(t_sender, NULL);    
    
    close(sock);
    return 0;
}

void *sender(void *socket) {
    struct sockaddr_in si_other;

    // Zera a struct do endereço de destino
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;

    // Recupera o descritor de socket passado como argumento
    int s = *(int *)socket;

    Mensagem msg;
    msg.fonte = nucleo.endereco; // Define o endereço de origem da mensagem como o núcleo

    int id, op;
    char buffer[BUFLEN + 3]; // Buffer para entrada de usuário (id + espaço + mensagem

    while (1){
        printf("+--------------------------------------+\n");
        printf("| %-39s|\n", "Escolha uma opção");
        printf("| %-37s|\n", "0 - Digitar uma mensagem");
        printf("| %-37s|\n", "1 - Sair");
        printf("+--------------------------------------+\n");

        // Lê a opção do usuário
        scanf("%d", &op);
        if (op) return NULL;  // Se for 1, encerra a thread
        
        // Remove os resíduos do buffer de entrada
        while (getchar() != '\n'); 

        printf("Informe a mensagem no formato: <id> <msg>\n");
        fflush(stdout);  // Garante que a mensagem seja impressa imediatamente

        // Lê a linha completa do stdin
        fgets(buffer, BUFLEN + 3, stdin);

        // Tenta extrair o id e a mensagem da linha
        if (sscanf(buffer, "%d %[^\n]", &id, msg.data) != 2) {
            perror("Formato inválido. Use: <id> <mensagem>\n");
            continue;
        }

        // Procura o roteador informado
        Roteador *r = findById(id);
        if(!r) printf("Roteador não existe ou não é vizinho");

        memset(&msg, 0, sizeof(Mensagem)); // reset na var antes do uso
        // Seta o tipo da mensagem e o endereço do destino
        msg.tipoMensagem = DADOS;

        msg.destino = r->endereco;
        si_other.sin_port = htons(msg.destino.porta);
        // Envia a mensagem para o roteador
        if (sendto(s, &msg, sizeof(msg), 0 ,(struct sockaddr *) &si_other, sizeof(si_other))==-1) die("sendto()");
        
    }

    return NULL;
}