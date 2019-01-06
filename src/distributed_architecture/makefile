CC			= mpicc
CFLAGS		= -Wall -Wextra -Wconversion
OBJFILES	= main.o array_helpers.o print_helpers.o
TARGET		= distributed_relaxation

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -o $(TARGET) $(OBJFILES)
	
clean:
	clear
	rm -rf $(OBJFILES) $(TARGET) *~