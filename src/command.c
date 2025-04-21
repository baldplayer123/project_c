#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"




void commandInsert(char *buff){
  printf("%s\n", buff);
  int id = rand();
  char *username = strtok(buff, " ");
  char *password = strtok(NULL, " ");
  createRow(id, username, password);
}

void getCommand(){
  printf("Give me command!\n");
  char buff[100];
  fgets(buff, 100, stdin);
  // printf("%d\n", buff[6]);
  if (!strncmp(buff, "insert", 6)) {
    printf("FUCK YEAH\n");
    commandInsert(buff);
  }
  else if (!strncmp(buff, "select", 6)) {
    printf("FUCK YEAH\n");
  }
  else if (!strncmp(buff, "search", 6)) {
    printf("FUCK YEAH\n");
  }
  else if (!strncmp(buff, "delete", 6)) {
    printf("FUCK YEAH\n");
  }
  else {
    printf("Invalid command\n");
  }
  

}



int main(){
  getCommand();
}
