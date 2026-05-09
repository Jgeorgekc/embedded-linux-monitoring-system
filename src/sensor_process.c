#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "shared_mem.h"

int main()
{
    sensor_data_t *data;

    data = create_shared_memory();

    if(data == NULL)
    {
        return -1;
    }

    srand(time(NULL));

    while(1)
    {
        pthread_mutex_lock(&data->mutex);

        data->temperature = rand() % 10 + 30;
        data->humidity = rand() % 20 + 50;
        data->pressure = rand() % 50 + 1000;

        printf("[SENSOR] TEMP=%d HUM=%d PRESS=%d\n",
               data->temperature,
               data->humidity,
               data->pressure);

        fflush(stdout);

        pthread_mutex_unlock(&data->mutex);

        sleep(5);
    }

    return 0;
}
