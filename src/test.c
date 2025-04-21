#include <stdio.h>
#include <stdlib.h>
#include "db.h"




void  btree_test_Insert100values(){
  btree *tree = createBtree();
  for (int i = 0; i <= 100; i++) {
    Rows row = createRow(i, "user" , "pass");    
    insertKey(row, tree);
  }
  int lastValue = tree->root->keys[0].id - 60;
  int ret = btree_test_traversalreturn(tree->root, &lastValue);
  // traverseTree(tree->root);
  saveTable(tree->root, "testtable");
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
    Rows row ;
    row.id = i;
    insertKey(row, tree);
  }
  deleteKey(78, tree->root);
  deleteKey(90, tree->root);
  deleteKey(4, tree->root);
  deleteKey(42, tree->root);
  deleteKey(47, tree->root);
  deleteKey(61, tree->root);
  deleteKey(43, tree->root);

  int lastValue = tree->root->keys[0].id - 60;
  int ret = btree_test_traversalreturn(tree->root, &lastValue);
  free_Tree(tree);
  if (ret == 1) {

    traverseTree(tree->root);
    printf("End - Test passed without issues\n");
  }
  else {
    traverseTree(tree->root);
    printf("End - Test failed!\n");
    exit(1);
  }
  
}

int isNodeCorrect(btree_node *node){  
  if (node->nb_keys >= MIN_KEY && node->nb_keys <= MAX_KEY) {
    return 0;
  }
  else{
    return 1;
  }
}

int isChildrenCorrect(btree_node *node){
  int expectedChildren = node->nb_keys + 1;
  if (node->leaf == true){
    return 0;
  }
  else {
    for (int j = 0; j < expectedChildren; j++) {
      if (node->Children[j] == NULL) {
        return 1;
        }
    }
    return 0;
  }
}

int btree_test_traversalreturn(btree_node* node, int* lastValue) {
    int i;
    for (i = 0; i < node->nb_keys; i++) {
        if (!node->leaf) {
            if (!btree_test_traversalreturn(node->Children[i], lastValue)) {
              if (isNodeCorrect(node->Children[i]) == 0 && isChildrenCorrect(node->Children[i]) == 0) {
                return 0;
              }
              else {
                return 1;
              }
            }
        }
        if (node->keys[i].id <= *lastValue) {
              if (isNodeCorrect(node->Children[i]) == 0 && isChildrenCorrect(node->Children[i]) == 0) {
                return 0;
              }
              else {
                return 1;
              }
        }
        *lastValue = node->keys[i].id;
    }
    if (!node->leaf) {
        if (!btree_test_traversalreturn(node->Children[i], lastValue)) {
            if (isNodeCorrect(node->Children[i]) == 0 && isChildrenCorrect(node->Children[i]) == 0) {
              return 0;
            }
            else {
              return 1;
            }
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

