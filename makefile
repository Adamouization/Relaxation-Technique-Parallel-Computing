CC=gcc
CFLAGS=-pthread -Wall -Wextra -Wconversion

main: main.c
	clear
	$(CC) main.c array_helpers.c print_helpers.c -o main.exe $(CFLAGS)
	./main.exe
