#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MSG_SIZE 256

void create_sensor_packet(char *buffer,
                          int id,
                          int temp,
                          int hum,
                          int press);

void create_ack_packet(char *buffer,
                       int id);

void create_ping_packet(char *buffer);

void create_pong_packet(char *buffer);

int parse_ack_packet(const char *buffer);

int is_ping_packet(const char *buffer);

int is_pong_packet(const char *buffer);

int parse_sensor_packet(
    const char *buffer);

#endif