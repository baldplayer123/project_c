#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "server.h"

#define PORT 7777
#define SERVER_IP "127.0.0.1"

void sendToDatabase(const char *message) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printLog("[!] Failed to create socket");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printLog("[!] Connection to DB server failed");
        close(sock);
        exit(1);
    }

    char fullmsg[1024];
    snprintf(fullmsg, sizeof(fullmsg), "%s\n", message);  
    send(sock, fullmsg, strlen(fullmsg), 0);
    close(sock);
    printLog("[*] Sent to DB: %s", message);
}
