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
	printf("Table was saved !\n\n");
	free_Tree(tree);
}

void createTables(char *tablename){
	tablename[strcspn(tablename, "\n")] = 0;
	btree *tree = createBtree();
	saveTable(tree->root, tablename);
	free(tree);
	printf("Table created\n\n");
	
}

void deleteTables(char *tablename){
	tablename[strcspn(tablename, "\n")] = 0;
	char path[64];
  strcpy(path, "tables/");
  strcat(path, tablename);
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    printf("This table does not exist\n");
    return;
  }
  fclose(file);
	if (remove(path) == 0){
		printf("File deleted successfully\n");
	}
	else {
		printf("Error in the deletion of the file\n");
	}
	
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
		    strtok(buffer, " ");
		    createTables(strtok(NULL, " "));
		  }
		  else if (!strncmp(buffer, "delete", 6)) {
		    strtok(buffer, " ");
		    deleteTables(strtok(NULL, " "));
		  }
		  else if (!strncmp(buffer, "list", 4)) {
		    listTables();
		  }
		  else if (!strncmp(buffer, "exit", 4)) {
		  	printf("goodbye.\n");
		  	exit(0);
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
		
