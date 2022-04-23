#!/bin/sh
./server.out --port 8080 --tnum 2 &
./server.out --port 8081 --tnum 3 &
./client.out --k 15 --mod 1111 --servers servers.txt