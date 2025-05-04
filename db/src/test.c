#include <stdio.h>
#include <stdlib.h>
#include "db.h"


// BTREE TEST: INSERT 100 VALUES
// -----------------------------

// Insert 100 rows into the B-Tree and check ordering/invariants
void btree_test_Insert100values() {
  btree *tree = createBtree();

  for (int i = 0; i <= 100; i++) {
    Rows row = createRow(i, "user", "pass");
    insertKey(row, tree);
  }

  int lastValue = tree->root->keys[0].id - 60;
  int ret = btree_test_traversalreturn(tree->root, &lastValue);

  saveTable(tree->root, "testtable");
  free_Tree(tree);

  if (ret == 1) {
    printf("[OK] Test passed\n");
  } else {
    printf("[FAIL] Test failed\n");
    exit(1);
  }
}


// BTREE TEST: INSERT AND DELETE
// -----------------------------

// Insert rows and delete specific keys, then check tree correctness
void btree_test_InsertAndDelete() {
  btree *tree = createBtree();

  for (int i = 0; i <= 100; i++) {
    Rows row = createRow(i, "user", "pass");
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

  traverseTree(tree->root);
  free_Tree(tree);

  if (ret == 1) {
    printf("[OK] Test passed\n");
  } else {
    printf("[FAIL] Test failed\n");
    exit(1);
  }
}


// NODE VALIDATION HELPERS
// -----------------------------

// Check if node has valid number of keys
int isNodeCorrect(btree_node *node) {
  return (node->nb_keys >= MIN_KEY && node->nb_keys <= MAX_KEY) ? 0 : 1;
}

// Check if internal node has valid child pointers
int isChildrenCorrect(btree_node *node) {
  if (node->leaf) return 0;

  int expectedChildren = node->nb_keys + 1;
  for (int j = 0; j < expectedChildren; j++) {
    if (node->Children[j] == NULL) {
      return 1;
    }
  }
  return 0;
}


// TRAVERSAL VALIDATION
// -----------------------------

// Recursively verify tree order and node constraints
int btree_test_traversalreturn(btree_node* node, int* lastValue) {
  int i;
  for (i = 0; i < node->nb_keys; i++) {
    if (!node->leaf) {
      if (!btree_test_traversalreturn(node->Children[i], lastValue)) {
        if (isNodeCorrect(node->Children[i]) || isChildrenCorrect(node->Children[i])) return 0;
      }
    }
    if (node->keys[i].id <= *lastValue) {
      if (isNodeCorrect(node->Children[i]) || isChildrenCorrect(node->Children[i])) return 0;
    }
    *lastValue = node->keys[i].id;
  }
  if (!node->leaf) {
    if (!btree_test_traversalreturn(node->Children[i], lastValue)) {
      if (isNodeCorrect(node->Children[i]) || isChildrenCorrect(node->Children[i])) return 0;
    }
  }
  return 1;
}


// RUN ALL TESTS
// -----------------------------

// Entry point to run all B-Tree unit tests
void run_all_tests() {
  printf("\n[*] Running internal tests...\n\n");

  printf("[*] Test: Insert 100 values in btree\n");
  btree_test_Insert100values();

  // printf("[*] Test: Insert 100 values and delete some keys\n");
  // btree_test_InsertAndDelete();
}
