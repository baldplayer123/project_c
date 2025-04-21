#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
#include <stdbool.h>
#include <string.h>
#include "db.h"




// So if i want to create a new node using a function, what the function will return is a btree_node,
// Be carefull if i return only NewNode and not *NewNode, i will copy the value on the stack and lose the address in the heap
struct btree_node *create_newNode(){
  struct btree_node *NewNode = malloc(sizeof(btree_node)); // If i dont initialise it as a pointer, it will be stored on the stacj and not the heap so no dynamic memory -> will lead to stack overflow with a lot of node
  NewNode->keys = malloc(sizeof(Rows) * MAX_KEY);
  NewNode->nb_keys = 0;
  NewNode->Children = malloc(sizeof(btree_node*) * ORDER_TREE); // *4 because max 4 children // Children étant tableau de pointer on alloue sizeof(*btreenode)
  return NewNode; // Do not return the pointer to newNode because it will copy the struct on the stack and lose memory reference to the right value on the stack
  // Inside NewNode is memory address on the heap, and Inside *NewNode is the actual struct so returning it means copying the value
}

struct btree *createBtree(){ // This function has to be a pointer because we will return the address of the btree
  struct btree *tree = malloc(sizeof(btree)); // Here i create a pointer to the address that i allocated for the tree
  tree->root = create_newNode(); // Create the root node
  tree->root->leaf = true;
  return tree; // Return the tree pointer 
}

Rows createRow(int id, char *username, char *password){
  Rows row;
  row.id = id;
  strcpy(row.username, username);
  strcpy(row.password, password);
  return row;
}

char *Serialize(Rows row){
  char *csvRow = malloc(sizeof(char) * 256);
  snprintf(csvRow, 256,"%d,%s,%s\n", row.id, row.username, row.password);
  return csvRow;
}

Rows Unserialize(char *line){
  int id = atoi(strtok(line,","));
  char *username = strtok(NULL, ",");
  char *password = strtok(NULL, ",");
  Rows tmpRow = createRow(id, username, password);

  return tmpRow;
}



void SearchKey(btree_node *node, int id){
  int i = 0;
  while (i < node->nb_keys &&  id > node->keys[i].id) {
    i++;
  }
  if ( i < node->nb_keys && node->keys[i].id == id) {
    printf("Found the key good!\n");
  } else if (node->leaf == true) {
    printf("Did not found the correct key in the tree");
  } else {
    SearchKey(node->Children[i], id);
  }
}


void insertSplitChild(btree_node *parent, int index){
  btree_node *oldroot = parent->Children[index];
  int median_index = oldroot->nb_keys / 2;
// Create the new child node and assign the correct leaf value
  btree_node *NewNode = create_newNode();
  NewNode->leaf = oldroot-> leaf;
  int tmp = 0;
// Assign right value of old root to new child (right)
  for (int j = median_index + 1; j < oldroot->nb_keys; j++) {
      NewNode->keys[tmp] = oldroot->keys[j];
      tmp++;
      NewNode->nb_keys += 1;
  }
// If oldroot is not a leaf, copy children as well
// What happens here is t hat we copy all the Children of old root bigger than
// oldroot median+1 index to the new right node
  tmp = 0;
  if (!oldroot->leaf) {
    for (int j = median_index + 1; j <= oldroot->nb_keys; j++) {
        NewNode->Children[tmp] = oldroot->Children[j];
        tmp++;
    }
}
// Make space for inserting the children to new root node
  for (int j = parent->nb_keys; j > index; j--) {
    parent->keys[j] = parent->keys[j-1];
    parent->Children[j+1] = parent->Children[j];
  }
  parent->keys[index] = oldroot->keys[median_index];
  oldroot->nb_keys = median_index;
  parent->nb_keys += 1;
  parent->Children[index + 1] = NewNode;
}


void insertNonFullKey(btree_node *node, Rows row){
  int i = node->nb_keys - 1;
  if (node->leaf == true){
    // Leaf donc insert directement  Easy
    while (i >= 0 && row.id < node->keys[i].id) {
      node->keys[i + 1] = node->keys[i];
      i--;
    }
    i++;
    node->keys[i] = row;
    node->nb_keys += 1;
  }
  else {
    // Node n'est pas une feuille donc split child and insert
    while (i >= 0 && row.id < node->keys[i].id ) {
      i--;
    }
    i++;
    // Maybe rror here odl

      if (node->Children[i]->nb_keys == MAX_KEY) {
          insertSplitChild(node, i);

          if (row.id > node->keys[i].id) {
              i++;
          }
      }

      insertNonFullKey(node->Children[i], row);

        }
  }    



void insertKey(Rows row, btree *tree){
  btree_node *root = tree->root;
  // Case where root is ful!!!
  if (root->nb_keys == MAX_KEY) {
    btree_node *NewRoot = create_newNode();
    NewRoot->leaf = false;
    NewRoot->Children[0] = root;
    insertSplitChild(NewRoot, 0);
    // Update the bew tree root
    tree->root = NewRoot;
    insertNonFullKey(NewRoot, row);    
  } 
  // Case where root is not full
  else {
    insertNonFullKey(root, row);
  }
}
  
// This return index of biggedst value
Rows UTILS_getPred(btree_node *node){
  btree_node *tmp = node;
  while (!tmp->leaf) {
    tmp = tmp->Children[tmp->nb_keys - 1]; // Recursive into the child node with the highest value of key and append to tmp that node
  }
  return tmp->keys[tmp->nb_keys - 1];
}


// This return index of biggedt  value
Rows UTILS_GetSuc(btree_node *node){
  btree_node *tmp = node;
  while (!tmp->leaf) {
    tmp = tmp->Children[0]; // Recursive into the child node with the highest value of key and append to tmp that node
  }
  return tmp->keys[0];
}


// This return pointer to node!

void UTILS_merge(btree_node *parent, int index) {
  btree_node *left = parent->Children[index];
  btree_node *right = parent->Children[index + 1];

  int original_nb_keys = left->nb_keys;  
    // Move parent key down into left

  left->keys[original_nb_keys] = parent->keys[index]; 
    // Copy right child's keys into left

  for (int i = 0; i < right->nb_keys; i++) {
      left->keys[original_nb_keys + 1 + i] = right->keys[i];  // offset by +1
  }

    // If not leaf, copy right child's children too

  if (!left->leaf) {
      for (int i = 0; i <= right->nb_keys; i++) {
          left->Children[left->nb_keys + i] = right->Children[i];
      }
  }


    left->nb_keys = original_nb_keys + 1 + right->nb_keys;
    // Shift parent keys and children to fill the gap
    for (int i = index; i < parent->nb_keys - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->Children[i + 1] = parent->Children[i + 2];
    }

    parent->nb_keys--;

    // Free the right node
    free(right->keys);
    free(right->Children);
    free(right);
}


void deleteFromLeaf(btree_node *node, int index){
    for (int j = index; j < node->nb_keys; j++) {
    node->keys[j] = node->keys[j+1];
    }
    node->nb_keys--;  
}


void deleteFromNonLeaf(btree_node* node, int index){
  if (node->Children[index]->nb_keys > MIN_KEY) { // So if predecessor child has at least MIN KEY INSIDE
    Rows pred = UTILS_getPred(node->Children[index]);
    node->keys[index] = pred;
    deleteKey(pred.id, node->Children[index]);
  }
  else if (node->Children[index+1]->nb_keys > MIN_KEY) { // if successor child has at least MIN KEY INSIDE
    Rows suc = UTILS_GetSuc(node->Children[index+1]);
    node->keys[index] = suc;
    deleteKey(suc.id, node->Children[index+1]);
  }
  else { // If both node children are equals to MIN KEY
    Rows deletedKey = node->keys[index];
    UTILS_merge(node, index);
    deleteKey(deletedKey.id, node->Children[index]);
  }
}


void deleteKey(int id, btree_node* node){
  
  // Look for correct place of insertion for node
  int i = 0;
  printf("Checking key %d against target %d\n", node->keys[i].id, id);
  while (i < node->nb_keys && id > node->keys[i].id) {
      i++;
  }
  if (i < node->nb_keys && node->keys[i].id == id) {
// Dont forget to check if min key is respected
    
  // Case 1 -> If the node is a leaf, just delete nothing else to check (Maybe delete the node if node has only one key)
    if (node->leaf) {
      deleteFromLeaf(node, i);
    }
  // Case 2 -> The node is not leaf so internal!
    else {
      deleteFromNonLeaf(node, i);
    }
  }
  // Go down a node to find the correct key
  else if (!node->leaf) { // Case key not found
      deleteKey(id, node->Children[i]);
    }
  // did not find anything
  else {
    printf("Key not found!!!\n");
  }
    

    
  }



void traverseTree(btree_node *node) {
    int i;
    for (i = 0; i < node->nb_keys; i++) {
        if (!node->leaf) {
            traverseTree(node->Children[i]);
        }
        // printf("%d - %s - %s \n", node->keys[i].id, node->keys[i].username, node->keys[i].password);
    }
    if (!node->leaf) {
        traverseTree(node->Children[i]); // Last child
    }
}


  // Inside NewNode is memory address on the heap, and Inside *NewNode is the actual struct so returning it means copying the value

void free_Node(btree_node *node){
  if (node->leaf == false) {
    for (int i = 0; i <= node->nb_keys; i++) { // Recursively go the the leaf and delete everything, working checked on ps aux
      free_Node(node->Children[i]);
    }
  }
  free(node->Children);
  free(node->keys);
  free(node);
}

void free_Tree(btree *tree){
  free_Node(tree->root);
  free(tree);
}

