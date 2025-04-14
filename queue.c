#include "queue.h"
#include <stdio.h>

void init_queue(Queue *q) {
    q->front = 0;
    q->rear = 0;
}

bool queue_empty(Queue *q) {
    return (q->front == q->rear);
}

bool queue_full(Queue *q) {
    return (((q->rear + 1) & (SIZE_QUEUE - 1)) == q->front);
}

bool enqueue(Queue *q, Mensagem newItem) {
    if(queue_full(q)) {
        printf("Queue is full.\n");
        return false;
    }

    q->queue[q->rear] = newItem;
    q->rear = (q->rear + 1) & (SIZE_QUEUE - 1);
    return true;
}

bool dequeue(Queue *q) {
    if(queue_empty(q)) {
        printf("Queue is empty.\n");
        return false;
    }

    q->front = (q->front + 1) & (SIZE_QUEUE - 1);
    return true;
}

int current_size(Queue *q) {
    return (q->rear - q->front) & (SIZE_QUEUE - 1);
}
