#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>


int socketfd;
struct sockaddr_in addr;

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


  while (1) {
    printf("Waiting for new conn !\n");
    int newsocket = accept(socketfd, NULL, NULL);
    char msg[64] = "Successfull connexion, goodbye now!\n";
    if (write(newsocket, msg, sizeof(msg)) == sizeof(msg)){
      printf("Success, message has been correctly send\n");
    }
    else {
      printf("Error - Return does not match the size of the message\n");
    }
    close(newsocket);
  }

}
