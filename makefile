CC=gcc
CFLAGS=-Wall -Wextra -Wconversion

main: main.c
	$(CC) main.c -o main.exe $(CFLAGS)
	./main.exe