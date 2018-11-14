CC			= gcc
CFLAGS		= -Wall -Wextra -Wconversion
LDFLAGS		= -pthread
OBJFILES	= main.o array_helpers.o print_helpers.o
TARGET		= relaxation

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
	
clean:
	rm -rf $(OBJFILES) $(TARGET) *~