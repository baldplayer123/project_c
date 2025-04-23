#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>


// dont forget about too many conn and dos!!


char api[7] = "command";
int socketfd;
struct sockaddr_in addr;
struct sockaddr_in client_addr;
typedef struct infoclient {
  int client;
  char ip[INET_ADDRSTRLEN];
} infoclient;


void *handleClient(void *arg){
  infoclient *client = (infoclient*)  arg;
  printf("id: %d - from: %s - Connexion established and is being handled !\n", client->client, client->ip);
  while (1) {
    // Check if con is alive
    char msg[64] = "check if conn is alive\n";
    if (write(client->client, msg, sizeof(msg)) != sizeof(msg)){
      printf("conn is dead\n");
      break;
    }
    char buffer[512];
    if (recv(client->client, buffer, sizeof(buffer), 0) != -1) {
      printf("id: %d - from: %s - Message received is '%s' \n",client->client, client->ip, buffer);
      if (strncmp(buffer, api, 7) == 0 ) {
        printf("id: %d - from: %s - Successfull connexion to the command api ! \n", client->client, client->ip);
      }
      else {
        printf("id: %d - from: %s - Connexion to the server without api args, exit!!\n", client->client, client->ip);
        break;
      }

      
    }

  }


  
  // char msg[64] = "Successfull connexion, goodbye now!\n";
  // if (write(client->, msg, sizeof(msg)) == sizeof(msg)){
  //   printf("Success, message has been correctly send\n");
  // }
  // else {
  //   printf("Error - Return does not match the size of the message\n");
  // }



  close(client->client);
  free(client);

  
  return NULL;
}

int main(){
  printf("Starting the server program\n");
  // Initialise a socket
  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd == -1) {
    printf("Error code -1 - Failed to initialise a socket ");
  }
  addr.sin_family = AF_INET; // Address on local system
  addr.sin_port = htons(4001); // Port on local system
  addr.sin_addr.s_addr = INADDR_ANY; // Check more available options ??
  if (bind(socketfd, (const struct sockaddr *) &addr, sizeof(addr)) == -1){
    printf("Error code -1 -  in binding the port \n");
    exit(-1);
  }
  if (listen(socketfd, 10) == -1) {
    printf("Error code -1 - Failed to set the socket to listen \n");
    exit(-1);
  }

  printf("Socket binded and listening !\nWaiting for new connexion...\n");
  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int newsocket = accept(socketfd, (struct sockaddr *) &client_addr, &client_len);
    infoclient *client  = malloc(sizeof(infoclient));
    client->client = newsocket; 
    inet_ntop(AF_INET, &client_addr.sin_addr, client->ip , sizeof(client->ip));
    // printf("id: %d - from: %s - New connexion established !\n",client->client ,client->ip);
    pthread_t newthread;
    pthread_create(&newthread, NULL, handleClient, client);
    pthread_detach(newthread);

  //   // char msg[64] = "Successfull connexion, goodbye now!\n";
  //   if (write(newsocket, msg, sizeof(msg)) == sizeof(msg)){
  //     printf("Success, message has been correctly send\n");
  //   }
  //   else {
  //     printf("Error - Return does not match the size of the message\n");
  //   }
  //   close(newsocket);
  }

}
