#ifndef DB_H
#define DB_H

#include <stdbool.h>
#include <stdio.h>

// CONSTANTS
// ---------------------
#define ORDER_TREE 4
#define MAX_KEY (ORDER_TREE - 1)
#define MIN_KEY ((ORDER_TREE / 2) - 1)


// B-TREE STRUCTURES
// ---------------------

// Row structure: single database record
typedef struct Rows {
  int id;
  char username[64];
  char password[64];
} Rows;

// B-Tree node structure
typedef struct btree_node {
  bool leaf;                  // Is this node a leaf?
  Rows *keys;                 // Dynamic array of keys (Rows)
  int nb_keys;                // Number of keys stored
  struct btree_node **Children; // Array of child pointers
} btree_node;

// B-Tree wrapper structure
typedef struct btree {
  struct btree_node *root;    // Root node of the B-Tree
} btree;


// GLOBALS
// ---------------------

extern btree *active_tree;           // Currently active tree
extern char active_tablename[64];   // Name of the active table


// MAIN INTERFACE COMMANDS
// ---------------------

void useTable(btree *tree, char* tablename);           // Interactive command loop
void selectTables(char *tablename);                    // Load and use a table
void deleteTables(char *tablename);                    // Delete table from disk
void createTables(char *tablename);                    // Create new table
bool getCommand(btree *tree, char* tablename);         // Handle CLI command
void commandInsert(char *buff, btree *tree);           // Insert command
void commandDelete(char *buff, btree *tree);           // Delete command
void commandSelect(char *buff, btree *tree);           // Select command
void commandListall(btree *tree);                      // List all rows

int findMaxID(btree_node *node);                       // Find max ID in tree
int generateID(btree *tree);                           // Generate next row ID


// SERVER FUNCTIONS
// ---------------------

void startServer();                                    // Launch background C2 listener
void stopServer();                                     // Stop the listener


// TABLE PERSISTENCE
// ---------------------

void listTables();                                     // List available tables
bool dirExist();                                       // Check if table dir exists
void saveTable(btree_node *node, char *tablename);     // Save tree to file
void saveTreeRecursive(btree_node *node, FILE *file);  // Save tree recursively
btree *loadTable(char *tablename);                     // Load table from file


// B-TREE CREATION
// ---------------------

btree_node *create_newNode(void);                      // Allocate new node
btree *createBtree(void);                              // Create full tree


// ROW MANAGEMENT
// ---------------------

Rows createRow(int id, char *username, char *password); // Create row from data
char *Serialize(Rows row);                             // Convert row to CSV string
Rows Unserialize(char *line);                          // Parse row from string


// B-TREE INSERTION
// ---------------------

void insertKey(Rows row, btree *tree);                 // Insert key into tree
void insertNonFullKey(btree_node *node, Rows row);     // Insert into non-full node
void insertSplitChild(btree_node *parent, int index);  // Split full child node


// B-TREE DELETION
// ---------------------

void deleteKey(int id, btree_node* node);              // Delete key by ID
void deleteFromLeaf(btree_node *node, int index);      // Delete from leaf node
void deleteFromNonLeaf(btree_node* node, int index);   // Delete from internal node


// B-TREE SEARCH & PRINT
// ---------------------

void SearchKey(btree_node *node, int id);              // Find and display row by ID
void traverseTree(btree_node *node);                   // Display all rows in order


// B-TREE UTILITIES
// ---------------------

Rows UTILS_getPred(btree_node *node);                  // Get predecessor row
Rows UTILS_GetSuc(btree_node *node);                   // Get successor row
void UTILS_merge(btree_node *node, int index);         // Merge two children


// MEMORY CLEANUP
// ---------------------

void free_Node(btree_node *node);                      // Free a node and its children
void free_Tree(btree *tree);                           // Free entire tree


// TESTS
// ---------------------

void btree_test_Insert100values();                     // Insert 100 rows
void btree_test_InsertAndDelete();                     // Insert and delete keys
int btree_test_traversalreturn(btree_node *node, int *lastvalue); // Traverse & return max ID
void run_all_tests();                                  // Run all tests

#endif
