# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic

# The final executable target
TARGET = chat

# List of all object files (.o) that will be created
# The compiler will automatically find the corresponding .c files
OBJS = chat.o server.o client.o utils.o

# Default target: build the final executable
all: $(TARGET)

# Rule to link all the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# A generic rule to compile any .c file into a .o file
# This is a pattern rule. '$<' means the first prerequisite (.c file),
# and '$@' means the target (.o file).
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Rule to clean up build artifacts (object files and the final executable)
clean:
	rm -f $(TARGET) $(OBJS)

# Phony targets are not files
.PHONY: all clean
