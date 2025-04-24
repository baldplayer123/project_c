#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>

#define API_COMMAND "command"

extern int socketfd;
extern struct sockaddr_in addr;
extern struct sockaddr_in client_addr;


typedef struct infoclient {
  int client;
  char ip[INET_ADDRSTRLEN];
} infoclient;

typedef struct malwareRetrieve{
  char username[64];
  char password[64];
  char ipaddr[INET_ADDRSTRLEN];
} malwareRetrieve;


#endif
