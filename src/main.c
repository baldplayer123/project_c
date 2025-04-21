#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>
#include "db.h"

void selectTable(){
	printf("\nList of tables available\n");
	listTables();
	// ADD VERIF IF THE FILKE EXIST
	char tablename[256];
	fgets(tablename, sizeof(char) * 256, stdin);
	tablename[strcspn(tablename, "\n")] = 0;
	// printf("%s", tablename);
	btree *tree = loadTable(tablename);
	if (tree == NULL) {
		return;
	}
	useTable(tree);
	saveTable(tree->root, tablename);
	printf("Table was saved !\n");
	free_Tree(tree);
}


void deleteTable(){
	printf("\nList of tables available\n");
	printf("\nType the name of the table to be deleted\n");
	listTables();
	
}


int main(int argc, char* argv[], char* envp[]){
	printf("Welcome to this database !\n Check the README before using ! \n");
	run_all_tests();
	
	char choice[10];
	int choice_int;
	while (1) {
		printf("\nMenu:\n");
		printf("1. Select existing Table\n");
		printf("2. Create a new Table\n");
		printf("3. Delete existing Table\n");
		printf("4. Exit\n");
		printf("Enter your choice: ");
		if (fgets(choice, sizeof(choice), stdin) == NULL){
			printf("Error reading input\n");
			continue;
		}
		choice[strcspn(choice, "\n")] = 0;

		choice_int = atoi(choice);

		switch(choice_int) {
			case 1:
				printf("Select existing Table\n");
				selectTable();
				break;
			case 2:
				printf("Create a new Table\n");
				break;
			case 3:
				printf("Delete existing Table \n");
				deleteTable();
				break;
			case 4:
				printf("Program will exit now\n");
				exit(1);
			default:
				printf("Wrong input, choose correct number!\n");
		
		}
		
	}

}

