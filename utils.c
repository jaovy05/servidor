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

int makeSocket(int port){
    struct sockaddr_in si_me;
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

void readEnlaces(int i, FILE *file){
    int roteador1, roteador2, enlace;
    // 2 x (2 char id) + 4 char enlace + 2 char espaço + 1 (\0 | \n) = 11.
    char linha[11];

    do {
        // lê os bytes linha do arquivo e verifica o caso base da recursão.
        if(!fgets(linha, 11, file)){ 
            // Agora que se sabe a quantidades de roteadores, aloca memória.
            roteadores = (Roteador *)malloc(sizeof(Roteador) * i);
            // salva essa quantidade no roteador main encerrando a recursão. 
            nucleo.qtdVizinhos = i;
            return ;
        };
        // Trata os bytes lidos para o formato esperado.
        sscanf(linha, "%d %d %d", &roteador1, &roteador2, &enlace);
        // verifica se o enlace lido é um enlace do roteador main.
    } while (roteador1 != nucleo.id && roteador2 != nucleo.id );
    
    // Chamada da recursão adicionando incrementando o numero de roteadores encontrados.
    readEnlaces(i + 1, file);
    // Salva o id do roteador encontrado
    // considerando que o roteador1 ou roteador são o id do nucleo 
    // temos x ^ x ^ y e sabe-se que x ^ x = 0 e 0 ^ y = y.
    roteadores[i].id = roteador1 ^ roteador2 ^ nucleo.id ;
    roteadores[i].enlace = enlace;
}

void readRoteadores(FILE *file){
    // Variável de parada.
    int hit = nucleo.qtdVizinhos;
    int id, porta;
    // 2 char id + 5 char porta + 15 char ip + 2 char espaco + 1 \n.
    char linha[25], ip[16];
    while(hit){
        fgets(linha, 25, file);
        sscanf(linha, "%d %d %s", &id, &porta, ip);

        // Se for o próprio núcleo, atualiza o endereço
        if(nucleo.id == id) {
            strncpy(nucleo.endereco.ip, ip, 16);
            nucleo.endereco.porta = porta;
            continue;
        }
        Roteador *r = findById(id);
        if(!r) continue;

        // Atualiza o roteador encontrado        
        strncpy(r->endereco.ip, ip, 16);
        r->endereco.porta = porta;
        hit--;
    }
}

int cmp(const void *a, const void *b) {
    return ((Roteador *)a)->id - ((Roteador *)b)->id;
}

Roteador *findById(int id) {
    // Cria um roteador temporário com id e faz uma busca binária
    Roteador chave = {.id = id};
    return bsearch(&chave, roteadores, nucleo.qtdVizinhos, sizeof(Roteador), cmp);
}
