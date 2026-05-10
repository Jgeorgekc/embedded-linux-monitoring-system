#include <stdio.h>
#include <string.h>

#include "message_queue.h"

void queue_init(message_queue_t *q)
{
    q->front = 0;
    q->rear = 0;
    q->count = 0;

    pthread_mutex_init(&q->mutex,
                       NULL);

    pthread_cond_init(&q->cond,
                      NULL);
}

void enqueue(message_queue_t *q,
             const char *msg)
{
    pthread_mutex_lock(&q->mutex);

    if(q->count == QUEUE_SIZE)
    {
        printf("[QUEUE] Queue full\n");

        pthread_mutex_unlock(&q->mutex);

        return;
    }

    strcpy(q->messages[q->rear],
           msg);

    q->rear = (q->rear + 1) % QUEUE_SIZE;

    q->count++;

    pthread_cond_signal(&q->cond);

    pthread_mutex_unlock(&q->mutex);
}

void dequeue(message_queue_t *q,
             char *msg)
{
    pthread_mutex_lock(&q->mutex);

    while(q->count == 0)
    {
        pthread_cond_wait(&q->cond,
                          &q->mutex);
    }

    strcpy(msg,
           q->messages[q->front]);

    q->front = (q->front + 1) % QUEUE_SIZE;

    q->count--;

    pthread_mutex_unlock(&q->mutex);
}
