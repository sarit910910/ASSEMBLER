# Makefile for the assembler project

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -std=c11 -Iinclude

# Source files
SRC = source/Array.c source/Errors.c source/First_pass.c source/Global_Func.c source/Main_program.c source/Opcodes_table.c source/Reseved_words.c source/Symbols.c

# Object files
OBJ = $(SRC:.c=.o)

# Executable file
EXEC = assembler

# Default target
all: $(EXEC)

# Link object files to create executable
$(EXEC): $(OBJ)
    $(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

# Compile source files into object files
%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

# Clean object files and executable
clean:
    rm -f $(OBJ) $(EXEC)

# Phony targets
.PHONY: all clean 
