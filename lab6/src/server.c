#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <pthread.h>

#include <getopt.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "libhelp/help.h"

static uint64_t Factorial(const fac_args_t* args) {
  uint64_t ans = args->begin;

  for (uint64_t i = args->begin + 1; i < args->end; i++)
    ans = MultModulo(ans, i, args->mod);

  return ans;
}

static void *ThreadFactorial(void *args) {
  fac_args_t* fargs = (fac_args_t*)args;
  return (void *)(uint64_t *)Factorial(fargs);
}

int main(int argc, char **argv) {
  int tnum = -1;
  int port = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"port", required_argument, 0, 0},
                                      {"tnum", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        port = atoi(optarg);
        if (!port) {
          printf("Error: bad port value\n");
          return -1;
        }
        break;
      case 1:
        tnum = atoi(optarg);
        if (!tnum) {
          printf("Error: bad tnum value\n");
          return -1;
        }
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Unknown argument\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (port == -1 || tnum == -1) {
    fprintf(stderr, "Using: %s --port 20001 --tnum 4\n", argv[0]);
    return 1;
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);  
  //AF_INET - IPv4, 
  //SOCK_STREAM - TCP
  if (server_fd < 0) {
    fprintf(stderr, "Can not create server socket!");
    return 1;
  }

  struct sockaddr_in server = create_sockaddr(port, INADDR_ANY);

  int opt_val = 1;
  //Setsocket:
   //server_fd - дескриптор
   //SOL_SOCKET - параметры на уровне библиотеки
   //SO_REUSEADDR - Разрешает повторное использование локальных адресов 
   
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

  //присоединяет сокет к адресу
  int err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
  if (err < 0) {
    return 1;
  }

  // 128 connections - max
  err = listen(server_fd, 128);
  if (err < 0) {
    fprintf(stderr, "Could not listen on socket\n");
    return 1;
  }

  printf("Server listening at %d\n", port);

  while (true) {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

    if (client_fd < 0) {
      fprintf(stderr, "Could not establish new connection\n");
      continue;
    }

    while (true) {
      size_t buffer_size = sizeof(fac_args_t);
      char from_client[buffer_size];

      int read = recv(client_fd, from_client, buffer_size, 0);

      if (!read)
        break;
      if (read < 0) {
        fprintf(stderr, "Client %d read failed, errno=%d\n", client_fd, errno);
        break;
      }
      if (read < buffer_size) {
        fprintf(stderr, "Client send wrong data format\n");
        break;
      }

      pthread_t threads[tnum];
      uint64_t begin = 0;
      uint64_t end = 0;
      uint64_t mod = 0;
      memcpy(&begin, from_client, sizeof(uint64_t));
      memcpy(&end, from_client + sizeof(uint64_t), sizeof(uint64_t));
      memcpy(&mod, from_client + 2 * sizeof(uint64_t), sizeof(uint64_t));

      fprintf(stdout, "Receive: %lu %lu %lu\n", begin, end, mod);

      if (tnum > (end - begin) / 2) {
        tnum = (end - begin) / 2;
        printf("Warning: too much threads. Continue with %d\n", tnum);
      }
      fac_args_t args[tnum];

      float block = (float)(end - begin) / tnum;
      for (uint32_t i = 0; i < tnum; i++) {
        args[i].begin = begin + round(block * (float)i);
        args[i].end = begin + round(block * (i + 1.f));
        args[i].mod = mod;

        if (pthread_create(&threads[i], NULL, ThreadFactorial,
                           (void *)&args[i])) {
          printf("Error: pthread_create failed!\n");
          return 1;
        }
      }

      uint64_t total = 1;
      for (uint32_t i = 0; i < tnum; i++) {
        uint64_t result = 0;
        pthread_join(threads[i], (void **)&result);
        total = MultModulo(total, result, mod);
      }

      printf("Total: %lu\n", total);
      char buffer[sizeof(total)];
      memcpy(buffer, &total, sizeof(total));

      err = send(client_fd, buffer, sizeof(total), 0);
      if (err < 0) {
        fprintf(stderr, "Can't send data to client\n");
        break;
      }
    }

    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
  }

  return 0;
}