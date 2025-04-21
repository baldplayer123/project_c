#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "db.h"




void saveTreeRecursive(btree_node *node, FILE *file) {
    int i;
    for (i = 0; i < node->nb_keys; i++) {
        if (!node->leaf) {
            saveTreeRecursive(node->Children[i], file);
        }
        fprintf(file, "%d - %s - %s\n", node->keys[i].id, node->keys[i].username, node->keys[i].password);
    }
    if (!node->leaf) {
        saveTreeRecursive(node->Children[i], file);
    }
}

void saveTable(btree_node *node, char *tablename) {
    char path[64];
    strcpy(path, "tables/");
    strcat(path, tablename);

    FILE *file = fopen(path, "w");  
    if (file == NULL) {
        printf("Failed to open file for writing.\n");
        return;
    }

    saveTreeRecursive(node, file);

    fclose(file);  
}


void listTables(){
  int checkDir = dirExist();
  if (checkDir == 1) {
    printf("Directory does not exist!\n Please create 'tables' directory at the root of the project\n");
    return;
  }
  DIR *dir = opendir("tables");
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (!(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)) { // Do nothing if . and ..
      printf("- %s\n", entry->d_name);
    }
    else {
    }
  }
  closedir(dir);
}


bool dirExist(){
  DIR *dir = opendir("tables");
  if (dir) {
    closedir(dir);
    return 0;
  }
  else {
    return 1;
  }
}

// int main(){
//   listTables();


  
// }
