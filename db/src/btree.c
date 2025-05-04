#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
#include <stdbool.h>
#include <string.h>
#include "db.h"



// MEMORY HELPERS
// -----------------

// Create and return a new B-Tree node
// Used to initialize an empty node with space for keys and children
struct btree_node *create_newNode(){
  struct btree_node *NewNode = malloc(sizeof(btree_node)); 
  NewNode->keys = malloc(sizeof(Rows) * MAX_KEY); 
  NewNode->nb_keys = 0;
  NewNode->Children = malloc(sizeof(btree_node*) * ORDER_TREE); 
  return NewNode; 
}

// Create and return a new B-Tree structure
// Starts the tree with a single empty root node marked as a leaf
struct btree *createBtree(){  
  struct btree *tree = malloc(sizeof(btree)); 
  tree->root = create_newNode(); 
  tree->root->leaf = true;
  return tree; 
}


// ROW SERIALIZATION
// -----------------

// Create a row object from values
// Used to build a row struct from ID, username, and password
Rows createRow(int id, char *username, char *password){
  Rows row;
  row.id = id;
  strcpy(row.username, username);
  strcpy(row.password, password);
  return row;
}

// Convert row to CSV format
// Used for exporting a row as a single line string (id,username,password)
char *Serialize(Rows row){
  char *csvRow = malloc(sizeof(char) * 256);
  snprintf(csvRow, 256,"%d,%s,%s", row.id, row.username, row.password);
  return csvRow;
}

// Convert CSV line back to row
// Used to parse a line from CSV and reconstruct the original row struct
Rows Unserialize(char *line){
  char *copy = strdup(line);
  char *id_str = strtok(copy, ",");
  char *username = strtok(NULL, ",");
  char *password = strtok(NULL, ",");

  if (!id_str || !username || !password) {
    printf("[ERROR] Malformed row: %s\n", line);
    exit(1);
  }

  Rows row = createRow(atoi(id_str), username, password);
  free(copy);
  return row;
}




// ID GENERATION
// -----------------

// Traverse to find max ID in all nodes
// Used to search the entire tree and find the highest existing row ID
int findMaxID(btree_node *node) {
  if (!node) return -1;

  int max_id = -1;
  for (int i = 0; i < node->nb_keys; i++) {
    if (node->keys[i].id > max_id) {
      max_id = node->keys[i].id;
    }
  }

  if (!node->leaf) {
    for (int i = 0; i <= node->nb_keys; i++) {
      int child_max = findMaxID(node->Children[i]);
      if (child_max > max_id) {
        max_id = child_max;
      }
    }
  }

  return max_id;
}

// Return new ID as max + 1
// Used to generate a unique ID for the next inserted row
int generateID(btree *tree) {
  int max_id = findMaxID(tree->root);
  return max_id + 1;
}


// SEARCH FUNCTION
// -----------------

// Recursively search for a key
// Used to find and print a row by its ID in the B-Tree
void SearchKey(btree_node *node, int id){
  int i = 0;
  while (i < node->nb_keys &&  id > node->keys[i].id) {
    i++;
  }

  if (i < node->nb_keys && node->keys[i].id == id) {
    printf("| %d | %s | %s |\n", node->keys[i].id, node->keys[i].username, node->keys[i].password);
  } else if (node->leaf == true) {
    printf("Did not found the correct key in the tree");
  } else {
    SearchKey(node->Children[i], id);  // Continue search in the correct child
  }
}

// INSERTION LOGIC
// -----------------
// 
// Split a full child node at given index in parent
// Used during insertion when a child node has no space left
void insertSplitChild(btree_node *parent, int index){
  btree_node *oldroot = parent->Children[index];
  int median_index = oldroot->nb_keys / 2;

  btree_node *NewNode = create_newNode();
  NewNode->leaf = oldroot->leaf;

  int tmp = 0;
  for (int j = median_index + 1; j < oldroot->nb_keys; j++) {
    NewNode->keys[tmp] = oldroot->keys[j];
    tmp++;
    NewNode->nb_keys += 1;
  }

  tmp = 0;
  if (!oldroot->leaf) {
    for (int j = median_index + 1; j <= oldroot->nb_keys; j++) {
      NewNode->Children[tmp] = oldroot->Children[j];
      tmp++;
    }
  }

  for (int j = parent->nb_keys; j > index; j--) {
    parent->keys[j] = parent->keys[j - 1];
    parent->Children[j + 1] = parent->Children[j];
  }

  parent->keys[index] = oldroot->keys[median_index];
  oldroot->nb_keys = median_index;
  parent->Children[index + 1] = NewNode;
  parent->nb_keys++;
}

// Insert a row into a node that is not full
// Recursively inserts a row in the correct position if there's space
void insertNonFullKey(btree_node *node, Rows row){
  int i = node->nb_keys - 1;

  if (node->leaf == true){
    while (i >= 0 && row.id < node->keys[i].id) {
      node->keys[i + 1] = node->keys[i];
      i--;
    }
    i++;
    node->keys[i] = row;
    node->nb_keys += 1;
  }
  else {
    while (i >= 0 && row.id < node->keys[i].id ) {
      i--;
    }
    i++;

    if (node->Children[i]->nb_keys == MAX_KEY) {
      insertSplitChild(node, i);

      if (row.id > node->keys[i].id) {
        i++;
      }
    }

    insertNonFullKey(node->Children[i], row);
  }
}

// Insert a row into the B-Tree
// Entry point for inserting a row, handles root split if needed
void insertKey(Rows row, btree *tree){
  btree_node *root = tree->root;

  if (root->nb_keys == MAX_KEY) {
    btree_node *NewRoot = create_newNode();
    NewRoot->leaf = false;
    NewRoot->Children[0] = root;

    insertSplitChild(NewRoot, 0);
    tree->root = NewRoot;

    insertNonFullKey(NewRoot, row);
  } 
  else {
    insertNonFullKey(root, row);
  }
}

// DELETE HELPER ROUTINES
// -----------------

// Return the predecessor (largest key in left subtree)
// Walks down the rightmost path to find the largest value
Rows UTILS_getPred(btree_node *node){
  btree_node *tmp = node;
  while (!tmp->leaf) {
    tmp = tmp->Children[tmp->nb_keys - 1];
  }
  return tmp->keys[tmp->nb_keys - 1];
}

// Return the successor (smallest key in right subtree)
// Walks down the leftmost path to find the smallest value
Rows UTILS_GetSuc(btree_node *node){
  btree_node *tmp = node;
  while (!tmp->leaf) {
    tmp = tmp->Children[0];
  }
  return tmp->keys[0];
}

// Merge two children of a parent node
// Used when both children have the minimum number of keys
void UTILS_merge(btree_node *parent, int index) {
  btree_node *left = parent->Children[index];
  btree_node *right = parent->Children[index + 1];

  int original_nb_keys = left->nb_keys;

  left->keys[original_nb_keys] = parent->keys[index];

  for (int i = 0; i < right->nb_keys; i++) {
    left->keys[original_nb_keys + 1 + i] = right->keys[i];
  }

  if (!left->leaf) {
    for (int i = 0; i <= right->nb_keys; i++) {
      left->Children[left->nb_keys + i] = right->Children[i];
    }
  }

  left->nb_keys = original_nb_keys + 1 + right->nb_keys;

  for (int i = index; i < parent->nb_keys - 1; i++) {
    parent->keys[i] = parent->keys[i + 1];
    parent->Children[i + 1] = parent->Children[i + 2];
  }

  parent->nb_keys--;

  free(right->keys);
  free(right->Children);
  free(right);
}

// DELETION LOGIC
// -----------------

// Delete key from a leaf node
// Removes a key directly by shifting elements to the left
void deleteFromLeaf(btree_node *node, int index){
  for (int j = index; j < node->nb_keys; j++) {
    node->keys[j] = node->keys[j+1];
  }
  node->nb_keys--;
}

// Delete key from an internal (non-leaf) node
// Handles deletion by using predecessor, successor, or merging children
void deleteFromNonLeaf(btree_node* node, int index){
  if (node->Children[index]->nb_keys > MIN_KEY) {            // Case 1: predecessor has enough keys
    Rows pred = UTILS_getPred(node->Children[index]);
    node->keys[index] = pred;
    deleteKey(pred.id, node->Children[index]);
  }
  else if (node->Children[index+1]->nb_keys > MIN_KEY) {     // Case 2: successor has enough keys
    Rows suc = UTILS_GetSuc(node->Children[index+1]);
    node->keys[index] = suc;
    deleteKey(suc.id, node->Children[index+1]);
  }
  else {                                                     // Case 3: merge both children
    Rows deletedKey = node->keys[index];
    UTILS_merge(node, index);
    deleteKey(deletedKey.id, node->Children[index]);
  }
}

// Main deletion function to remove a key by ID
// Searches for the key and applies proper deletion case
void deleteKey(int id, btree_node* node){
  int i = 0;
  printf("Checking key %d against target %d\n", node->keys[i].id, id);

  while (i < node->nb_keys && id > node->keys[i].id) {
    i++;
  }

  if (i < node->nb_keys && node->keys[i].id == id) {
    if (node->leaf) {
      deleteFromLeaf(node, i);
    }
    else {
      deleteFromNonLeaf(node, i);
    }
  }
  else if (!node->leaf) {
    deleteKey(id, node->Children[i]);
  }
  else {
    printf("Key not found!!!\n");
  }
}

// TREE TRAVERSAL
// -----------------

// Traverse the B-Tree and print all entries
// Recursively visits all keys and prints them in sorted order

void traverseTreeRecursive(btree_node *node) {
  int i;
  for (i = 0; i < node->nb_keys; i++) {
    if (!node->leaf) traverseTreeRecursive(node->Children[i]);
    printf("│ %-3d│ %-24s│ %-24s│\n",
           node->keys[i].id,
           node->keys[i].username,
           node->keys[i].password);
  }

  if (!node->leaf) traverseTreeRecursive(node->Children[i]);
}

void traverseTree(btree_node *node) {
  printf("\n╭─────┬──────────────────────────┬──────────────────────────╮\n");
  printf("│ ID  │ Username                 │ Password                 │\n");
  printf("├─────┼──────────────────────────┼──────────────────────────┤\n");

  traverseTreeRecursive(node);

  printf("╰─────┴──────────────────────────┴──────────────────────────╯\n");
}

// MEMORY CLEANUP
// -----------------

// Recursively free all nodes in the B-Tree
// Traverses down to free every node and its dynamic data
void free_Node(btree_node *node){
  if (node->leaf == false) {
    for (int i = 0; i <= node->nb_keys; i++) {
      free_Node(node->Children[i]);
    }
  }
  free(node->Children);
  free(node->keys);
  free(node);
}

// Free the entire B-Tree structure
// Frees all nodes and the tree wrapper itself
void free_Tree(btree *tree){
  free_Node(tree->root);
  free(tree);
}
