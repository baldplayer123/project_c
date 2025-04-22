#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include "db.h"


void commandListall(btree *tree){
  traverseTree(tree->root);
}

void commandSelect(char *buff, btree *tree){
  char copy_buff[64];
  strcpy(copy_buff, buff);
  strtok(copy_buff, " ");
  char *id = strtok(NULL, " ");
  id[strcspn(id, "\n")] = 0;
  int int_id = atoi(id);
  SearchKey(tree->root, int_id);
  return;

}

void commandDelete(char *buff, btree *tree){
  char copy_buff[64];
  strcpy(copy_buff, buff);
  strtok(copy_buff, " ");
  char *id = strtok(NULL, " ");
  id[strcspn(id, "\n")] = 0;
  int int_id = atoi(id);
  printf("NUMBER IS %d\n", int_id);
  deleteKey(int_id, tree->root);
  printf("Value deleted successfully\n");
  return;
}

void commandInsert(char *buff, btree *tree){
  char copy_buff[64];
  strcpy(copy_buff, buff);
  // printf("%s\n", buff);
  int id = generateID(tree);
  strtok(copy_buff, " "); 
  char *username = strtok(NULL, " ");
  char *password = strtok(NULL, " ");
  Rows tmpRow = createRow(id, username, password);
  insertKey(tmpRow, tree);
  printf("Value inserted successfully\n");
  return;
}

bool getCommand(btree *tree, char *tablename){
  printf("\n%s> what do you want to do ?\n%s> Available command: insert, select, listall, delete, exit\n", tablename, tablename);
  char buff[100];
  printf(" %s> ", tablename);
  fgets(buff, 100, stdin);
  if (!strncmp(buff, "insert", 6)) {
    commandInsert(buff, tree);
  }
  else if (!strncmp(buff, "select", 6)) {
    commandSelect(buff, tree);
  }
  else if (!strncmp(buff, "listall", 7)) {
    commandListall(tree);
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



void useTable(btree *tree, char *tablename){
  while (true) {
    int ret = getCommand(tree, tablename);
    if (ret == 1) {
      break;
    }
  }
  return;  
  
}
