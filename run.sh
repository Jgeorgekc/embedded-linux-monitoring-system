#!/bin/bash

echo "How many clients do you want?"

read CLIENTS

echo "Starting sensor process..."

gnome-terminal -- ./sensor_process

sleep 1

echo "Starting monitor server..."

gnome-terminal -- ./monitor_server

sleep 1

for ((i=1; i<=CLIENTS; i++))
do
    echo "Starting client $i"

    gnome-terminal -- ./client

    sleep 1
done
