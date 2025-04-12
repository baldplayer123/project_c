#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
#include <stdbool.h>
// #include "db.h"

int max_size_keys = 3;



typedef struct btree_node{
  bool leaf; // IS this a leaf
  int *keys; // Array of keys
  int nb_keys; // nb of keys
  struct btree_node **Children; // Array of child pointers, each valye of tgus array points to a child
} btree_node;

typedef struct btree{
  struct btree_node *root;
} btree;





// So if i want to create a new node using a function, what the function will return is a btree_node,
// Be carefull if i return only NewNode and not *NewNode, i will copy the value on the stack and lose the address in the heap
struct btree_node *create_newNode(){
  struct btree_node *NewNode = malloc(sizeof(btree_node)); // If i dont initialise it as a pointer, it will be stored on the stacj and not the heap so no dynamic memory -> will lead to stack overflow with a lot of node
  NewNode->leaf = false;
  NewNode->keys = malloc(sizeof(int) * max_size_keys);
  NewNode->nb_keys = 0;
  NewNode->Children = malloc(sizeof(btree_node*) * 4); // *4 because max 4 children // Children étant tableau de pointer on alloue sizeof(*btreenode)
  return NewNode; // Do not return the pointer to newNode because it will copy the struct on the stack and lose memory reference to the right value on the stack
  // Inside NewNode is memory address on the heap, and Inside *NewNode is the actual struct so returning it means copying the value
}

struct btree *createBtree(){ // This function has to be a pointer because we will return the address of the btree
  struct btree *tree = malloc(sizeof(btree)); // Here i create a pointer to the address that i allocated for the tree
  tree->root = create_newNode(); // Create the root node
  tree->root->leaf = true;
  return tree; // Return the tree pointer 
}

void insertNonFullKey(btree_node *node, int key){
  if (node->nb_keys == 0){
    node->keys[0] = key;
    node->nb_keys += 1;
  }
  else {
    int i = node->nb_keys - 1;
    while (i >= 0 && node->keys[i] > key) {
      i--;
    }
    i++; // NEW POSITION IN THE ARRAY
    for (int j = node->nb_keys - 1; j >= i; j--) {
      node->keys[j+1] = node->keys[j];
    }
    node->keys[i] = key;
    node->nb_keys += 1;
  }
 

  
}


void insertFullKey(){
  
}

void insertSplitChild(){
  
}

void insertKey(int key, btree *tree){
  btree_node *root = tree->root;
  // printf("%p\n", root);
  if (root->nb_keys < 3){
    insertNonFullKey(root, key);
  }
  
}

  // Inside NewNode is memory address on the heap, and Inside *NewNode is the actual struct so returning it means copying the value

// void free_Node(btree_node node){
//   free(node.Children);
//   free(node.keys);
//   free(node);
// }



int main(){
  printf("Program begins \n");
  struct btree *mytree = createBtree(); // Address of my tree in the heap
  printf("%p\n", mytree->root);
  // printf("%d\n", mytree->root->leaf);
  insertKey(35, mytree);
  insertKey(15, mytree);
  insertKey(12, mytree);
  insertKey(456, mytree);
  printf("%d\n", mytree->root->keys[0]);
  printf("%d\n", mytree->root->keys[1]);
  printf("%d\n", mytree->root->keys[2]);
  
} 
