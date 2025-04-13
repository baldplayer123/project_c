#include <stdio.h>
#include <stdlib.h>
#include "db.h"




void  btree_test_Insert100values(){
  btree *tree = createBtree();
  for (int i = 0; i <= 100; i++) {
    insertKey(i, tree);
  }
  int lastValue = tree->root->keys[0] - 60;
  int ret = btree_test_traversalreturn(tree->root, &lastValue);
  free_Tree(tree);
  if (ret == 1) {
    printf("End - Test passed without issues\n");
  }
  else {
    printf("End - Test failed!\n");
    exit(1);
  }
  
}

void btree_test_InsertAndDelete(){
  btree *tree = createBtree();
  for (int i = 0; i <= 100; i++) {
    insertKey(i, tree);
  }
  deleteKey(78, tree->root);
  deleteKey(90, tree->root);
  deleteKey(4, tree->root);
  deleteKey(42, tree->root);
  deleteKey(47, tree->root);
  deleteKey(61, tree->root);
  deleteKey(43, tree->root);

  int lastValue = tree->root->keys[0] - 60;
  int ret = btree_test_traversalreturn(tree->root, &lastValue);
  free_Tree(tree);
  if (ret == 1) {
    printf("End - Test passed without issues\n");
  }
  else {
    printf("End - Test failed!\n");
    exit(1);
  }
  
}


int btree_test_traversalreturn(btree_node* node, int* lastValue) {
    int i;
    for (i = 0; i < node->nb_keys; i++) {
        if (!node->leaf) {
            if (!btree_test_traversalreturn(node->Children[i], lastValue)) {
                return 0;
            }
        }
        if (node->keys[i] <= *lastValue) {
            return 0;
        }
        *lastValue = node->keys[i];
    }
    if (!node->leaf) {
        if (!btree_test_traversalreturn(node->Children[i], lastValue)) {
            return 0;
        }
    }
    return 1;
}

void run_all_tests(){
  printf("Starting tests!\n");

  printf("Start - Insertion of 40 values in btree!\n");
  btree_test_Insert100values();
  
  printf("Start - Deletion of some values in btree!\n");
  btree_test_Insert100values();
}

