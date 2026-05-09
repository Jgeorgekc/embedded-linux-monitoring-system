#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#include "shared_mem.h"

sensor_data_t *create_shared_memory()
{
    int fd;

    fd = shm_open(SHM_NAME,
                  O_CREAT | O_RDWR,
                  0666);

    if(fd < 0)
    {
        perror("shm_open");
        return NULL;
    }

    if(ftruncate(fd,
                 sizeof(sensor_data_t)) < 0)
    {
        perror("ftruncate");
        return NULL;
    }

    sensor_data_t *data;

    data = mmap(NULL,
                sizeof(sensor_data_t),
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd,
                0);

    if(data == MAP_FAILED)
    {
        perror("mmap");
        return NULL;
    }

    memset(data,
           0,
           sizeof(sensor_data_t));

    pthread_mutexattr_t mattr;

    pthread_mutexattr_init(&mattr);

    pthread_mutexattr_setpshared(&mattr,
                                 PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&data->mutex,
                       &mattr);

    return data;
}

sensor_data_t *get_shared_memory()
{
    int fd;

    fd = shm_open(SHM_NAME,
                  O_RDWR,
                  0666);

    if(fd < 0)
    {
        perror("shm_open");
        return NULL;
    }

    sensor_data_t *data;

    data = mmap(NULL,
                sizeof(sensor_data_t),
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd,
                0);

    if(data == MAP_FAILED)
    {
        perror("mmap");
        return NULL;
    }

    return data;
}
