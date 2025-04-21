#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"



void commandDelete(char *buff, btree *tree){
  strtok(buff, " ");
  char *id = strtok(NULL, " ");
  id[strcspn(id, "\n")] = 0;
  int int_id = atoi(id);
  printf("NUMBER IS %d\n", int_id);
  deleteKey(int_id, tree->root);
  printf("Value deleted successfully\n");
  return;
}

void commandInsert(char *buff, btree *tree){
  // printf("%s\n", buff);
  int id = rand(); 
  strtok(buff, " "); 
  char *username = strtok(NULL, " ");
  char *password = strtok(NULL, " ");
  Rows tmpRow = createRow(id, username, password);
  insertKey(tmpRow, tree);
  printf("Value inserted successfully\n");
  return;
}

bool getCommand(btree *tree){
  printf("Working in the table, what do you want to do ?\nAvailable command: insert, select, search, delete, exit\n");
  char buff[100];
  fgets(buff, 100, stdin);
  if (!strncmp(buff, "insert", 6)) {
    commandInsert(buff, tree);
  }
  else if (!strncmp(buff, "select", 6)) {
    printf("FUCK YEAH\n");
  }
  else if (!strncmp(buff, "search", 6)) {
    printf("FUCK YEAH\n");
  }
  else if (!strncmp(buff, "delete", 6)) {
    commandDelete(buff, tree);
  }
  else if (!strncmp(buff, "exit", 4)) {
    return 1;
  }
  else {
    printf("Invalid command\n");
  }
  return 0;
}



void useTable(btree *tree){
  while (true) {
    int ret = getCommand(tree);
    if (ret == 1) {
      break;
    }
  }
  return;  
  
}
