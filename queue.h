#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "mensagens.h"

#define SIZE_QUEUE 16

/**
 * @brief Fila circular de tamanho fixo para armazenar mensagens.
 *
 * Esta estrutura implementa uma fila circular utilizando um vetor com dois índices: 
 * `front` aponta para o primeiro elemento (o mais antigo), 
 * e `rear` aponta para o final da fila.
 *
 * A fila é implementada com um tamanho 16 e
 * as operações de rotação (circularidade) são feitas com operadores
 * binários (bitwise AND).
 */
typedef struct queue {
    int front;              /* Índice do primeiro elemento da fila */
    int rear;               /* Índice para o final da fila */   
    Mensagem queue[SIZE_QUEUE];  /* Vetor circular fixo que armazena as mensagens */
} Queue;

/**
 * Inicializa a fila atribuindo 0 para front e rear.
 *
 * @param q Ponteiro para a fila a ser inicializada.
 */
void init_queue(Queue *q);

/**
 * Determina se a fila está fazia
 * 
 * @param q Ponteiro para a fila.
 * @return true se a fila estiver vazia, falso caso contrário.
 */
bool queue_empty(Queue *q);

/**
 * Determina se a fila está cheia.
 * 
 * @param q Ponteiro para a fila.
 * @return true se a fila estiver cheia, falso caso contrário.
 */
bool queue_full(Queue *q);

/**
 * Adiciona nova mensagem no final da fila.
 * 
 * @param q Ponteiro para a fila.
 * @param newItem A mensagem a ser inserida.
 * @return true se a mensagem foi inserida, falso caso contrário. 
 */
bool enqueue(Queue *q, int newItem);

/**
 * Remove a mensagem no início da fila.
 * 
 * @param q Ponteiro para a fila.
 * @return true se a mensagem foi removida, falso caso contrário. 
 */
bool dequeue(Queue *q);

/**
 * Retorna a quantidade atual de elementos na fila.
 * 
 * @param q Ponteiro para a fila.
 * @return Número de elementos na fila.
 */
int current_size(Queue *q);

#endif //QUEUE_H