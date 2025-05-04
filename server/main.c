#include <stdio.h>
#include "server.h"

int main() {
  printLog("Starting SSL server on port 6000...");

  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();

  const SSL_METHOD *method = TLS_server_method();
  SSL_CTX *ctx = SSL_CTX_new(method);
  if (!ctx) {
    printLog("Failed to create SSL context");
    exit(-1);
  }

  if (!SSL_CTX_use_certificate_file(ctx, "certificate.crt", SSL_FILETYPE_PEM)) {
    printLog("Failed to load certificate");
    exit(-1);
  }
  if (!SSL_CTX_use_PrivateKey_file(ctx, "private.key", SSL_FILETYPE_PEM)) {
    printLog("Failed to load private key");
    exit(-1);
  }

  if (!SSL_CTX_check_private_key(ctx)) {
    printLog("Private key does not match certificate");
    exit(1);
  }

  int socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd == -1) {
    printLog("Socket creation failed");
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(6000);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(socketfd, (const struct sockaddr *)&addr, sizeof(addr)) == -1) {
    printLog("Failed to bind to port 6000");
    exit(-1);
  }
  int opt = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  if (listen(socketfd, 10) == -1) {
    printLog("Failed to listen on socket");
    exit(-1);
  }

  context *fullcontext = malloc(sizeof(context));
  if (fullcontext == NULL) {
    printLog("Failed to allocate fullcontext");
    exit(1);
  }
  fullcontext->fd = socketfd;
  fullcontext->ctx = ctx;

  pthread_t mainLoop;
  if (pthread_create(&mainLoop, NULL, ListenToClient, fullcontext) != 0) {
    printLog("Failed to start listener thread");
    exit(-1);
  }

  printf("interact > ");
  while (1) {
    char buffer[64];
    fgets(buffer, sizeof(buffer), stdin);
  }
}

