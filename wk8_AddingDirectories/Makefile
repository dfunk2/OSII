.PHONY: all clean pristine test ls

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99
CTEST_FLAGS = -DCTEST_ENABLE

# Output files
TARGET = testfs
LIB = libvvsfs.a

# Source and object files
SRCS = image.c block.c free.c inode.c pack.c dir.c
OBJS = $(SRCS:.c=.o)

TEST_MAIN = testfs.c
TEST_OBJ = $(TEST_MAIN:.c=.o)

LS_SRC = ls.c
LS_OBJ = $(LS_SRC:.c=.o)

# Header dependencies
HEADERS = image.h block.h free.h inode.h pack.h dir.h ctest.h

# Default build
all: $(TARGET)

# Build static library
$(LIB): $(OBJS)
	ar rcs $@ $^

# Compile testfs main with testing flags
$(TEST_OBJ): $(TEST_MAIN)
	$(CC) $(CFLAGS) $(CTEST_FLAGS) -c -o $@ $<

# Compile ls
$(LS_OBJ): $(LS_SRC)
	$(CC) $(CFLAGS) -c -o $@ $<

# Link final testfs executable
$(TARGET): $(TEST_OBJ) $(LIB)
	$(CC) -o $@ $^

# Ls binary for testing directory output
ls: $(LS_OBJ) $(LIB)
	$(CC) -o $@ $^

# Run test binary
test: $(TARGET)
	./$(TARGET)

# Remove intermediate object files
clean:
	rm -f *.o

# Remove all build outputs
pristine: clean
	rm -f $(TARGET) $(LIB) test.txt ls
