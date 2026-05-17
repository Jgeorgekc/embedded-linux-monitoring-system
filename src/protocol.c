#include <stdio.h>
#include <string.h>

#include "protocol.h"

void create_sensor_packet(char *buffer,
                          int id,
                          int temp,
                          int hum,
                          int press)
{
    snprintf(buffer,
             MSG_SIZE,
             "{ \"type\":\"sensor\", "
             "\"id\":%d, "
             "\"temp\":%d, "
             "\"hum\":%d, "
             "\"press\":%d }\n",
             id,
             temp,
             hum,
             press);
}

void create_ack_packet(char *buffer,
                       int id)
{
    snprintf(buffer,
             MSG_SIZE,
             "{ \"type\":\"ack\", "
             "\"id\":%d }\n",
             id);
}

void create_ping_packet(char *buffer)
{
    snprintf(buffer,
             MSG_SIZE,
             "{ \"type\":\"ping\" }\n");
}

void create_pong_packet(char *buffer)
{
    snprintf(buffer,
             MSG_SIZE,
             "{ \"type\":\"pong\" }\n");
}

int parse_ack_packet(const char *buffer)
{
    int id;

    if(sscanf(buffer,
              "{ \"type\":\"ack\", \"id\":%d }",
              &id) == 1)
    {
        return id;
    }

    return -1;
}

int is_ping_packet(const char *buffer)
{
    if(strstr(buffer,
              "\"type\":\"ping\""))
    {
        return 1;
    }

    return 0;
}

int is_pong_packet(const char *buffer)
{
    if(strstr(buffer,
              "\"type\":\"pong\""))
    {
        return 1;
    }

    return 0;
}

int parse_sensor_packet(
    const char *buffer)
{
    int id;

    if(sscanf(buffer,
              "{ \"type\":\"sensor\", \"id\":%d",
              &id) == 1)
    {
        return id;
    }

    return -1;
}