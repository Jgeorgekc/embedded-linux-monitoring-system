# Embedded Linux Monitoring System

Simple polling-based Embedded Linux monitoring system using:

- POSIX Shared Memory
- pthread mutex
- TCP sockets
- Client-server communication

## Build

make clean
make

## Run Order

Terminal 1:
./sensor_process

Terminal 2:
./monitor_server

Terminal 3:
./client
