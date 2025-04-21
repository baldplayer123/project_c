## Compitaleur 
CC= gcc

## Options de compilation
CO= -Wall -Wextra

## Source
SRC = src/main.c src/btree.c src/test.c src/savetable.c src/db.h

projectdb: 
	$(CC) $(CO) $(SRC) -o projectdb

clean:
	rm projectdb

