#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <pthread.h>

#define QUEUE_SIZE 100
#define MSG_SIZE 256

typedef struct
{
    char messages[QUEUE_SIZE][MSG_SIZE];

    int front;
    int rear;
    int count;

    pthread_mutex_t mutex;

    pthread_cond_t cond;

} message_queue_t;

void queue_init(message_queue_t *q);

void enqueue(message_queue_t *q,
             const char *msg);

void dequeue(message_queue_t *q,
             char *msg);

#endif
