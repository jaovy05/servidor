#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "mensagens.h"
#include "utils.h"
#include "queue.h"

#define BUFLEN 101  //Max length of buffer
//#define C_DEBUG 1

Roteador        *roteadores = NULL;
RoteadorNucleo  nucleo;
EntradaVetor    *vetorDistancia;
RegistroVizinho *historico;
ConfigInfo      config;

Queue           qSender;
pthread_mutex_t mutexFila;
sem_t           semItens;

Queue           qReceiver;
pthread_mutex_t mutexFilaReceiver;
sem_t           semItensReceiver;

void *sender(void *socket);
void *queueSender(void *queue);
void *receiver(void *sockect);
void *packet_handler(void *socket);
void init_vetor_distancia();
void init_historico_vizinhos();
void atualiza_vetor_distancia();
void *vetor_sender(void *arg);
void tratar_controle(Mensagem *msg);

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

    config = readIni();

    init_vetor_distancia();
    init_historico_vizinhos();

    // Cria o socket configurado.
    int sock = makeSocket(nucleo.endereco.porta);
    
    init_queue(&qSender);
    pthread_mutex_init(&mutexFila, NULL);
    sem_init(&semItens, 0, 0);

    init_queue(&qReceiver);
    pthread_mutex_init(&mutexFilaReceiver, NULL);
    sem_init(&semItensReceiver, 0, 0);

    // Cria 1 thread para cada serviço.
    pthread_t t_receiver, t_sender, t_queue_sender, t_packet_handler, t_vetor_sender;

    // Seta as respectivas funções 
    pthread_create(&t_receiver, NULL, receiver, &sock);
    pthread_create(&t_sender, NULL, sender, &sock);
    pthread_create(&t_queue_sender, NULL, queueSender, &sock);
    pthread_create(&t_packet_handler, NULL, packet_handler, &sock);
    pthread_create(&t_vetor_sender, NULL, vetor_sender, &sock);

    // O programa espera até a thread do sender encerrar 
    pthread_join(t_sender, NULL);    
    
    sem_destroy(&semItens);
    close(sock);
    return 0;
}

void *sender(void *socket) {
    // Recupera o descritor de socket passado como argumento
    // int s = *(int *)socket;

    Mensagem msg;
    msg.fonte = nucleo.endereco; // Define o endereço de origem da mensagem como o núcleo

    int id, op;
    char buffer[BUFLEN + 3]; // Buffer para entrada de usuário (id + espaço + mensagem)

    while (1){
        printf("+--------------------------------------+\n");
        printf("| %-39s|\n", "Escolha uma opção");
        printf("| %-37s|\n", "0 - Digitar uma mensagem");
        printf("| %-37s|\n", "1 - Sair");
        printf("| %-37s|\n", "2 - Mostrar vetores recebidos");
        printf("+--------------------------------------+\n");

        // Lê a opção do usuário
        scanf("%d", &op);
        if (op == 1) return NULL;  // Se for 1, encerra a thread

        if (op == 2) {
            for (int i = 0; i < nucleo.qtdVizinhos; i++) {
                printf("Vizinho %d: [", historico[i].idVizinho);
                for (int j = 0; j < config.totalRoteadores; j++) {
                    printf("(%d, %d)%s",
                        historico[i].vetor[j].idDestino,
                        historico[i].vetor[j].custo,
                        (j < config.totalRoteadores - 1) ? ", " : "]\n");
                }
            }
            continue;
        }
        
        // Remove os resíduos do buffer de entrada
        while (getchar() != '\n'); 
        // Remove os resíduos da msg anterior
        memset(&msg, 0, sizeof(Mensagem));

        printf("Informe a mensagem no formato: <id> <msg>\n");
        fflush(stdout);  // Garante que a mensagem seja impressa imediatamente

        // Lê a linha completa do stdin
        fgets(buffer, BUFLEN + 3, stdin);

        // Tenta extrair o id e a mensagem da linha
        if (sscanf(buffer, "%d %[^\n]", &id, msg.data) != 2) {
            perror("Formato inválido. Use: <id> <mensagem>\n");
            continue;
        }

        // Seta o id do destino
        msg.destinoId = id;
        // Seta o id da fonte
        msg.fonteId = nucleo.id;

        // Procura o roteador informado
        Roteador *r = findById(id);
        if(!r) printf("Roteador não existe ou não é vizinho");

        // Seta o tipo da mensagem e o endereço do destino
        msg.tipoMensagem = DADOS;

        msg.destino = r->endereco;

        // Seta o id do destino
        msg.destinoId = id;
        // Seta o id da fonte
        msg.fonteId = nucleo.id;
        
        // protege a fila antes de alterar 
        pthread_mutex_lock(&mutexFila);
        enqueue(&qSender, msg);
        pthread_mutex_unlock(&mutexFila);

        // incrementa o semaforo avisando que tem +1 item na fila
        sem_post(&semItens);
    }

    return NULL;
}

void *queueSender(void *socket) {
    // Recupera o descritor de socket passado como argumento
    int s = *(int *)socket;
    Mensagem *msg;
    struct sockaddr_in si_other;

    while (1) {
        // Evita de consumir CPU se a fila estiver vazia
        sem_wait(&semItens); 
        
        // protege a nossa querida fila 
        pthread_mutex_lock(&mutexFila);
        // pega a primeira mensagem da fila
        msg = &qSender.queue[qSender.front];
        // tira da fila
        dequeue(&qSender);
        // libera a fila
        pthread_mutex_unlock(&mutexFila);

        memset(&si_other, 0, sizeof(si_other));
        // configuração do destino, ip4, porta, e ip enviado
        si_other.sin_family = AF_INET;
        si_other.sin_port = htons(msg->destino.porta);
        si_other.sin_addr.s_addr = inet_addr(msg->destino.ip);

        if (sendto(s, msg, sizeof(Mensagem), 0, (struct sockaddr*)&si_other, sizeof(si_other)) == -1)  perror("Erro ao enviar mensagem");
    }

    return NULL;
}

void *receiver(void *socket) {
    // Recupera o descritor de socket passado como argumento
    int s = *(int *)socket;
    Mensagem msg;
    struct sockaddr_in si_other;
    socklen_t slen = sizeof(si_other);

    while (1) {
        memset(&msg, 0, sizeof(msg)); // Limpa a estrutura da mensagem

        // Espera e recebe uma mensagem do socket UDP
        int recv_len = recvfrom(s, &msg, sizeof(msg), 0, (struct sockaddr *)&si_other, &slen);
        if (recv_len == -1) {
            perror("Erro ao receber mensagem");
            continue;
        }

        // Protege a fila de entrada antes de inserir a nova mensagem
        pthread_mutex_lock(&mutexFilaReceiver);
        enqueue(&qReceiver, msg);
        pthread_mutex_unlock(&mutexFilaReceiver);

        // Sinaliza que há uma nova mensagem na fila de entrada
        sem_post(&semItensReceiver);

        #ifdef C_DEBUG
            // Imprime informações sobre a mensagem recebida
            printf("[Receiver] Mensagem recebida de %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        #endif
    }

    return NULL;
}

void *packet_handler(void *socket) {
    while (1) {
        // Aguarda até que haja uma mensagem disponível na fila de entrada
        sem_wait(&semItensReceiver);

        // Protege a fila de entrada antes de acessar a mensagem
        pthread_mutex_lock(&mutexFilaReceiver);
        Mensagem *msg = &qReceiver.queue[qReceiver.front];
        dequeue(&qReceiver); // Remove da fila
        pthread_mutex_unlock(&mutexFilaReceiver);

        // Verifica se a mensagem é destinada ao roteador vizinho ou ao próprio roteador
        Roteador *r = findById(msg->destinoId);

        if (msg->tipoMensagem == CONTROLE) {
            tratar_controle(msg);
            continue;
        }

        if (r) {
            // Se o destino for um roteador vizinho
            printf("[Packet Handler] Mensagem recebida e o destino é um roteador vizinho (ID: %d)\n", msg->destinoId);

            // Protege a fila de saída antes de inserir a mensagem
            pthread_mutex_lock(&mutexFila);
            enqueue(&qSender, *msg);  // Adiciona a mensagem à fila
            pthread_mutex_unlock(&mutexFila);

            // Incrementa o semáforo sinalizando que há uma nova mensagem na fila
            sem_post(&semItens);

        } else if(msg->destinoId == nucleo.id) {
            // Se o destino for o próprio roteador
            printf("[Packet Handler] Mensagem recebida para o roteador destino (ID: %d). Imprimindo mensagem...\n", msg->destinoId);
            
            // Imprime o conteúdo da mensagem
            printf("[Packet Handler] Mensagem para o roteador destino: %s\n", msg->data);
            
        } else {
            // A mensagem não é para este roteador nem para um roteador vizinho
            printf("[Packet Handler] Mensagem recebida e não é o roteador destino nem um vizinho => Erro: Mensagem inválida.\n");
        }
    }

    return NULL;
}

void init_vetor_distancia() {
    vetorDistancia = malloc(sizeof(EntradaVetor) * config.totalRoteadores);

    for (int i = 0; i < config.totalRoteadores; i++) {
        vetorDistancia[i].idDestino = i + 1;
        vetorDistancia[i].custo = (i + 1 == nucleo.id) ? 0 : config.INF;
    }
}

void init_historico_vizinhos() {
    historico = malloc(sizeof(RegistroVizinho) * nucleo.qtdVizinhos);

    for (int i = 0; i < nucleo.qtdVizinhos; i++) {
        historico[i].idVizinho = roteadores[i].id;
        historico[i].vetor = malloc(sizeof(EntradaVetor) * config.totalRoteadores);

        for (int j = 0; j < config.totalRoteadores; j++) {
            historico[i].vetor[j].idDestino = j + 1;
            historico[i].vetor[j].custo = config.INF;
        }
    }
}

void atualiza_vetor_distancia() {
    int mudou = 0;

    for (int d = 0; d < config.totalRoteadores; d++) {
        if (d + 1 == nucleo.id) continue;

        int menor = config.INF;

        for (int i = 0; i < nucleo.qtdVizinhos; i++) {
            int via = historico[i].idVizinho;
            int custoParaVizinho = historico[i].vetor[d].custo;
            int custoDoVizinhoParaD = roteadores[i].enlace;

            if (custoParaVizinho != config.INF && custoDoVizinhoParaD != config.INF) {
                int total = custoParaVizinho + custoDoVizinhoParaD;
                if (total < menor) {
                    menor = total;
                }
            }
        }
        
        if (vetorDistancia[d].custo > menor) {
            vetorDistancia[d].custo = menor;
            mudou = 1;
        }
    }

    if (mudou) {
        time_t now = time(NULL);
        char *ts = ctime(&now);
        ts[strcspn(ts, "\n")] = 0;
        #ifdef C_DEBUG
            printf("[Atualiza Vetor] %s Vetor atualizado: [", ts);
            for (int i = 0; i < config.totalRoteadores; i++) {
                printf("(%d, %d)%s", vetorDistancia[i].idDestino, vetorDistancia[i].custo,
                    (i < config.totalRoteadores - 1) ? ", " : "]\n");
            }
        #endif
    }
}

void *vetor_sender(void *arg) {
    int sock = *(int *)arg;

    while (1) {
        sleep(config.tempoVetor); 

        for (int i = 0; i < nucleo.qtdVizinhos; i++) {
            Roteador *vizinho = &roteadores[i];

            Mensagem msg;
            memset(&msg, 0, sizeof(Mensagem));
            msg.tipoMensagem = CONTROLE;
            msg.fonteId = nucleo.id;
            msg.fonte = nucleo.endereco;
            msg.destinoId = vizinho->id;
            msg.destino = vizinho->endereco;

            memcpy(msg.data, vetorDistancia, sizeof(EntradaVetor) * config.totalRoteadores);

            struct sockaddr_in si_other;
            memset(&si_other, 0, sizeof(si_other));
            si_other.sin_family = AF_INET;
            si_other.sin_port = htons(vizinho->endereco.porta);
            si_other.sin_addr.s_addr = inet_addr(vizinho->endereco.ip);


            sendto(sock, &msg, sizeof(Mensagem), 0, (struct sockaddr *)&si_other, sizeof(si_other));

            #ifdef C_DEBUG
                printf("[Vetor Sender] Vetor enviado para %d\n", vizinho->id);
                printf("[Vetor Sender] Vetor local: [");
                for (int j = 0; j < config.totalRoteadores; j++) {
                    printf("(%d, %d)%s", vetorDistancia[j].idDestino, vetorDistancia[j].custo,
                        (j < config.totalRoteadores - 1) ? ", " : "]\n");
                }
            #endif
        }
    }
    return NULL;
}

void tratar_controle(Mensagem *msg) {
    EntradaVetor *vetorRecebido = (EntradaVetor *)msg->data;

    for (int i = 0; i < nucleo.qtdVizinhos; i++) {
        if (historico[i].idVizinho == msg->fonteId) {
            memcpy(historico[i].vetor, vetorRecebido, sizeof(EntradaVetor) * config.totalRoteadores);
            atualiza_vetor_distancia();
            break;
        }
    }

    #ifdef C_DEBUG
        printf("[Controle] Vetor recebido de %d: [", msg->fonteId);
        for (int i = 0; i < config.totalRoteadores; i++) {
            printf("(%d, %d)%s", vetorRecebido[i].idDestino, vetorRecebido[i].custo,
                (i < config.totalRoteadores - 1) ? ", " : "]\n");
        }
    #endif
}
