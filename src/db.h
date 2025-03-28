#ifndef DB_H
#define DB_H

#define MINIMUM_DEGREE 2
// MAX KEYS = 2 * MINIMUM_DEGREE - 1
// MIN CHILD = MINIMUM_DEGREE
// MAX CHILD = 2 * MINIMUM DEGREE

typedef struct btree_node{
  bool leaf; // IS this a leaf
  int *keys; // Array of keys
  int nb_keys; // nb of keys
  struct btree_node **Children; // Array of child pointers, each valye of tgus array points to a child
} btree_node;



btree_node* createNode(bool leaf);

// Define Rows in the table
// typedef struct Rows {
//   int id;
//   char username[50];
//   char password[50];
//   char ip_address[7];  
// } Rows;

// Define Tables / db
// I think inside this will be the btrees (So for ID Username Password)
// typedef struct Tables {
//   Rows row;
// } Tables;








#endif
