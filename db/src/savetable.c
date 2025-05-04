#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include "db.h"


// LOAD TABLE
// ---------------------

// Load a table from disk and reconstruct its B-Tree
btree *loadTable(char *tablename) {
  char path[64];
  strcpy(path, "tables/");
  strcat(path, tablename);

  FILE *file = fopen(path, "r");
  if (file == NULL) {
    printf("[!] Table not found\n");
    return NULL;
  }

  char line[256];
  btree *tree = createBtree();

  while ((fgets(line, sizeof(line), file)) != NULL) {
    line[strcspn(line, "\r\n")] = 0;           // Remove newline characters
    if (strlen(line) == 0) continue;

    Rows tmpRow = Unserialize(line);           // Parse line into a Row
    insertKey(tmpRow, tree);                   // Insert into the B-Tree
  }

  fclose(file);
  return tree;
}


// SAVE TREE RECURSIVELY
// ---------------------

// Recursively write all rows of a B-Tree to a file in CSV format
void saveTreeRecursive(btree_node *node, FILE *file) {
  int i;
  for (i = 0; i < node->nb_keys; i++) {
    if (!node->leaf) {
      saveTreeRecursive(node->Children[i], file);
    }

    char *csvRow = Serialize(node->keys[i]);
    size_t len = strlen(csvRow);
    if (len == 0 || csvRow[len - 1] != '\n') {
      fprintf(file, "%s\n", csvRow);
    } else {
      fprintf(file, "%s", csvRow);
    }
    free(csvRow);
  }

  if (!node->leaf) {
    saveTreeRecursive(node->Children[i], file);
  }
}


// SAVE TABLE
// ---------------------

// Save a full table to disk using in-order traversal
void saveTable(btree_node *node, char *tablename) {
  char path[64];
  strcpy(path, "tables/");
  strcat(path, tablename);

  FILE *file = fopen(path, "w");
  if (file == NULL) {
    printf("[!] Could not save table\n");
    return;
  }

  saveTreeRecursive(node, file);     // Write tree content to file
  fclose(file);
}


// LIST TABLES
// ---------------------

// Show all saved tables in the 'tables/' directory
void listTables() {
  if (dirExist() == 1) {
    printf("[!] Directory 'tables/' not found\n");
    return;
  }

  DIR *dir = opendir("tables");
  struct dirent *entry;
  int count = 0;

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
      if (count == 0) {
        printf("\n[*] Available tables:\n");
        printf("╭────────────────────────────╮\n");
      }
      printf("│ %-26s │\n", entry->d_name);
      count++;
    }
  }

  if (count > 0) {
    printf("╰────────────────────────────╯\n");
  } else {
    printf("[*] No tables found\n");
  }

  closedir(dir);
}


// CHECK DIRECTORY
// ---------------------

// Return 0 if directory 'tables/' exists, 1 otherwise
bool dirExist() {
  DIR *dir = opendir("tables");
  if (dir) {
    closedir(dir);
    return 0;
  } else {
    return 1;
  }
}
