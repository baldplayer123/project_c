#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "db.h"

// Creation d'une fonction qui retourne un pointer vers un nouveau node
btree_node* createNode(bool leaf){
    btree_node* node = malloc(sizeof(btree_node)); // Malloc size for the node to be stored
// Each new node is supposed to be a leaf
    node->leaf = leaf;
// Each key is an int and we known the calc of how many keys will be inside each node so
    node->keys = malloc(sizeof(int) * (2 * MINIMUM_DEGREE - 1));
// Number of keys will be 0 at the creation of the new node
    node->nb_keys = 0;
// We know the max number of child so we can malloc
    node->Children = malloc(sizeof(btree_node*) * (2 * MINIMUM_DEGREE));
    return node;
};


