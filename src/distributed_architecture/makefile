CC			= mpicc
CFLAGS		= -Wall -Wextra -Wconversion
LDFLAGS		= -lm
OBJFILES	= main.o array_helpers.o print_helpers.o
TARGET		= distributed_relaxation

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
	
clean:
	clear
	rm -rf $(OBJFILES) $(TARGET) *~