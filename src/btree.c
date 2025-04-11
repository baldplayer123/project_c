#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "db.h"

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
  return tree; // Return the tree pointer 
}

void insertNonFullKey(){
  
}
void insertFullKey(){
  
}

void insertSplitChild(){
  
}

  // Inside NewNode is memory address on the heap, and Inside *NewNode is the actual struct so returning it means copying the value
}

void free_Node(btree_node node){
  free(node->Children);
  free(node->keys);
  free(node);
}


int main(){
  printf("Program begins \n");
  btree_node DaNode = *create_newNode();
  printf("%d", DaNode->keys[0]);
  printf("%d", DaNode->keys[1]);
  free_Node(DaNode);
  
}

void free_Node(btree_node *node){

    free(node->Children);
    free(node->keys);
    free(node);
}


int main(){
  printf("Program begins \n");
  // createBtree();
  struct btree *mytree = createBtree(); // Address of my tree in the heap
  // printf("%p\n", mytree); // If i understand well, they give back the same address because btree address and root start at the same point
  // printf("%p\n", &mytree->order);
  // printf("%p\n", mytree->root);
  insertKey(3, mytree);
  
} 
