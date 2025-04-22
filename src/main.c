#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>
#include "db.h"

void selectTable(char *tablename){
	tablename[strcspn(tablename, "\n")] = 0;
	btree *tree = loadTable(tablename);
	if (tree == NULL) {
		return;
	}
	useTable(tree, tablename);
	saveTable(tree->root, tablename);
	printf("Table was saved !\n");
	free_Tree(tree);
}


void deleteTable(char *tablename){
	// printf("\nList of tables available\n");
	// printf("\nType the name of the table to be deleted\n");
	
}


int main(){
	printf("Welcome to this database !\n Check the README before using ! \n");
	run_all_tests();
	printf("\nHere is the list of the currently saved tables.\n");
	listTables();
	
	while (1) {
		printf("\nAvailable commands :\n");
		printf(" - select -> Select a table to load.\n");
		printf(" - create -> Create a table.\n");
		printf(" - delete -> Delete a table.\n");
		printf(" - list   -> List all the tables available.\n");
		printf(" - exit   -> Exit the database program.\n");

		char buffer[100];
		fgets(buffer, 100, stdin);
		  if (!strncmp(buffer, "select", 6)) {
		    // commandInsert(buff, tree);
		    strtok(buffer, " ");
		    selectTable(strtok(NULL, " "));
		  }
		  else if (!strncmp(buffer, "create", 6)) {
		    // commandSelect(buff, tree);
		  }
		  else if (!strncmp(buffer, "delete", 6)) {
		    // printf("FUCK YEAH\n");
		  }
		  else if (!strncmp(buffer, "list", 4)) {
		    // return 1;
		  }
		  else if (!strncmp(buffer, "exit", 4)) {
		    // return 1;
		  }
		  else {
		    printf("Invalid command\n");
		    continue;
		  }
		}

}
		// switch(choice_int) {
		// 	case 1:
		// 		printf("Select existing Table\n");
		// 		selectTable();
		// 		break;
		// 	case 2:
		// 		printf("Create a new Table\n");
		// 		break;
		// 	case 3:
		// 		printf("Delete existing Table \n");
		// 		deleteTable();
		// 		break;
		// 	case 4:
		// 		printf("Program will exit now\n");
		// 		exit(1);
		// 	default:
		// 		printf("Wrong input, choose correct number!\n");
		
