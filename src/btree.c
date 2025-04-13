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


void insertNonFullKey(btree_node *node, int key){
  int i = node->nb_keys - 1;
  if (node->leaf == true){
    // Leaf donc insert directement  Easy
    while (i >= 0 && key < node->keys[i]) {
      node->keys[i + 1] = node->keys[i];
      i--;
    }
    i++;
    node->keys[i] = key;
    node->nb_keys += 1;
  }
  else {
    // Node n'est pas une feuille donc split child and insert
    while (i >= 0 && key < node->keys[i] ) {
      i--;
    }
    i++;
    if (node->Children[i]->nb_keys == max_size_keys) {
      insertSplitChild(node->Children[i], i);
      // Now i have two child but i have to choose in which child i do insert
      if (node->keys[i] > key) {
        i++;
      }
    }
    insertNonFullKey(node->Children[i], key);


    

  }    
}


void insertKey(int key, btree *tree){
  btree_node *root = tree->root;
  // Case where root is ful!!!
  if (root->nb_keys == max_size_keys) {
    btree_node *NewRoot = create_newNode();
    NewRoot->leaf = false;
    NewRoot->Children[0] = root;
    insertSplitChild(NewRoot, 0);
    // Update the bew tree root
    tree->root = NewRoot;
    insertNonFullKey(NewRoot, key);

    
  } 
  // Case where root is not full
  else {
    insertNonFullKey(root, key);
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
  btree_node *root = tree->root;
  free(tree);
}


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
  printf("%d\n", mytree->root->Children[0]->keys[0]);
  printf("%d\n", mytree->root->Children[1]->keys[0]);
  printf("%d\n", mytree->root->Children[1]->keys[1]);
  // printf("%d\n", mytree->root->keys[2]);
  // printf("Root node keys are : %d", )
  free_Tree(mytree);

  
  
} 

