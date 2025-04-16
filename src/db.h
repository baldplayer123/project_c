#ifndef DB_H
#define DB_H
#include <stdbool.h>

#define ORDER_TREE 4
#define MAX_KEY (ORDER_TREE - 1)
#define MIN_KEY ((ORDER_TREE /2) - 1)

// #define createRow(...) createRow(__VA_ARGS__, yanis, pass, NULL)
 

  // Btree
  // ---------------------

typedef struct Rows{
  int id;
  char username[64];
  char password[64];
} Rows;


typedef struct btree_node{
  bool leaf; // IS this a leaf
  Rows *keys; // Array of keys
  int nb_keys; // nb of keys
  struct btree_node **Children; // Array of child pointers, each valye of tgus array points to a child
} btree_node;


typedef struct btree{
  struct btree_node *root;
} btree;


// Creation & Initialization
btree_node *create_newNode(void);
btree *createBtree(void);

// Create Rows
Rows createRow(int id, char *username, char *password);


// Insertion
void insertKey(Rows row, btree *tree);
void insertNonFullKey(btree_node *node, Rows row);
void insertSplitChild(btree_node *parent, int index);

// Deletion
void deleteKey(int id, btree_node* node);
void deleteFromLeaf(btree_node *node, int index);
void deleteFromNonLeaf(btree_node* node, int index);

// Search by id
void SearchKey(btree_node *node, int id);
void traverseTree(btree_node *node);

// Utilities
Rows UTILS_getPred(btree_node *node);
Rows UTILS_GetSuc(btree_node *node);
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
