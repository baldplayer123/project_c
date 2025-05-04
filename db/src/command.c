#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include "db.h"


// COMMAND: LISTALL
// -----------------
// Display all rows stored in the B-Tree
void commandListall(btree *tree){
  traverseTree(tree->root);
}


// COMMAND: SELECT
// -----------------
// Search and display a row by ID from input buffer
void commandSelect(char *buff, btree *tree){
  char copy_buff[64];
  strcpy(copy_buff, buff);
  strtok(copy_buff, " ");
  char *id = strtok(NULL, " ");
  id[strcspn(id, "\n")] = 0;
  int int_id = atoi(id);
  SearchKey(tree->root, int_id);
}


// COMMAND: DELETE
// -----------------
// Delete a row by ID parsed from the buffer
void commandDelete(char *buff, btree *tree){
  char copy_buff[64];
  strcpy(copy_buff, buff);
  strtok(copy_buff, " ");
  char *id = strtok(NULL, " ");
  id[strcspn(id, "\n")] = 0;
  int int_id = atoi(id);
  printf("[*] Deleting id: %d\n", int_id);
  deleteKey(int_id, tree->root);
  printf("[*] Deleted\n");
}


// COMMAND: INSERT
// -----------------
// Insert a new row with username and password parsed from input
void commandInsert(char *buff, btree *tree){
  char copy_buff[64];
  strcpy(copy_buff, buff);
  strtok(copy_buff, " "); 
  char *username = strtok(NULL, " ");
  char *password = strtok(NULL, " ");

  if (!username || !password) {
    printf("[!] Missing username or password\n");
    return;
  }

  int id = generateID(tree);
  Rows tmpRow = createRow(id, username, password);
  insertKey(tmpRow, tree);
  printf("[*] Inserted: %s / %s (id: %d)\n", username, password, id);
}


// COMMAND HANDLER
// -----------------
// Read and execute user command for a given table
bool getCommand(btree *tree, char *tablename){
  printf("\n[%s] What do you want to do?\n", tablename);
  printf("[%s] Commands: insert, select, listall, delete, exit\n", tablename);
  char buff[100];
  printf("[%s] > ", tablename);
  if (!fgets(buff, sizeof(buff), stdin)) {
    printf("[!] Input error\n");
    return 1;
  }

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
    printf("[!] Unknown command\n");
  }
  return 0;
}


// TABLE USAGE LOOP
// -----------------
// Interactive loop to handle commands for the active table
void useTable(btree *tree, char *tablename){
  while (true) {
    int ret = getCommand(tree, tablename);
    if (ret == 1) {
      break;
    }
  }
}
