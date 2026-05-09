#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include <pthread.h>
#include <semaphore.h>

#define SHM_NAME "/sensor_shm"

typedef struct
{
    int temperature;
    int humidity;
    int pressure;

    pthread_mutex_t mutex;

} sensor_data_t;

sensor_data_t *create_shared_memory();
sensor_data_t *get_shared_memory();

#endif
