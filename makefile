CC=gcc
CFLAGS=-pthread -Wall -Wextra -Wconversion

main: main.c
	clear
	$(CC) main.c -o main.exe $(CFLAGS)
	./main.exe
