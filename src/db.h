#ifndef DB_H
#define DB_H
#include <stdbool.h>

#define ORDER_TREE 4
#define MAX_KEY (ORDER_TREE - 1)
#define MIN_KEY ((ORDER_TREE /2) - 1)

  // Btree
  // ---------------------

typedef struct btree_node{
  bool leaf; // IS this a leaf
  int *keys; // Array of keys
  int nb_keys; // nb of keys
  struct btree_node **Children; // Array of child pointers, each valye of tgus array points to a child
} btree_node;

typedef struct btree{
  struct btree_node *root;
} btree;


// Creation & Initialization
btree_node *create_newNode(void);
btree *createBtree(void);

// Insertion
void insertKey(int key, btree *tree);
void insertNonFullKey(btree_node *node, int key);
void insertSplitChild(btree_node *parent, int index);

// Deletion
void deleteKey(int key, btree_node* node);
void deleteFromLeaf(btree_node *node, int index);
void deleteFromNonLeaf(btree_node* node, int index);

// Search
void SearchKey(btree_node *node, int key);
void traverseTree(btree_node *node);

// Utilities
int UTILS_getPred(btree_node *node);
int UTILS_GetSuc(btree_node *node);
void UTILS_merge(btree_node *node, int index);
  
// Cleanup
void free_Node(btree_node *node);
void free_Tree(btree *tree);


  // Tests
  // ---------------------

// Btree tests
void btree_test_Insert100values();
void btree_test_InsertAndDelete();
int btree_test_traversalreturn(btree_node *node, int *lastvalue);


void run_all_tests();


#endif
