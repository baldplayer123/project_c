#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>
#include "db.h"

int main(int argc, char* argv[], char* envp[]){
	printf("Welcome to this database !\n Check the README before using ! \n");
	run_all_tests();

	
	// char choice[1];
	// int choice_int;
	// while (1) {
	// 	printf("\nMenu:\n");
	// 	printf("1. Create a new database\n");
	// 	printf("2. Load a database from a file\n");
	// 	printf("3. Delete the current database\n");
	// 	printf("4. Display the database\n");
	// 	printf("5. Exit\n");
	// 	printf("Enter your choice: ");
	// 	scanf("%s", choice);		
	// 	choice_int = atoi(choice);

	// 	switch(choice_int) {
	// 		case 1:
	// 			printf("Entering database creation\n");
	// 			break;
	// 		case 2:
	// 			printf("Entering loading of a database from file\n");
	// 			break;
	// 		case 3:
	// 			printf("Entering delete of database\n");
	// 			break;
	// 		case 4:
	// 			printf("Display the database\n");
	// 			break;
	// 		case 5:
	// 			printf("Program will exit now\n");
	// 			exit(1);
	// 		default:
	// 			printf("Wrong input, choose correct number!\n");
		
	// 	}
		
	// }

}

