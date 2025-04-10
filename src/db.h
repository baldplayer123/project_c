#ifndef DB_H
#define DB_H

#define MINIMUM_DEGREE 2
// MAX KEYS = 2 * MINIMUM_DEGREE - 1
// MIN CHILD = MINIMUM_DEGREE
// MAX CHILD = 2 * MINIMUM DEGREE

/*
bool leaf -> Is the current node a leaf
int *keys -> Array of the keys inside the node [key1, key2, key3]
int nkeys -> Counter of the number of keys inside the node so increases until it is at Max keys (But implement it)
struct btree_node **Children -> This will store the Children nodes,
  - We use struct btree_node because we are working with node, so the "return value" is a struct_node
   - **Children, It is a pointer to an array of btree_node, i can also write it as *Children[]
   The first * point to the FIRST value of the array of btree_node
   the Second * points to the FIRST struct btree_node, not the memory adress but the actual value, Meaning  i deference the address
*/

// typedef struct btree_node{
//   bool leaf; // IS this a leaf
//   int *keys; // Array of keys
//   int nb_keys; // nb of keys
//   struct btree_node **Children; // Array of child pointers, each valye of tgus array points to a child
// } btree_node;





#endif
