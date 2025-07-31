# Main executable name
TARGET = bin/ntm

# Symbolic link name (alias for no color)
ALIAS = bin/ntm_nc

# Source code directories
SRC_DIRS = \
src \
src/Module_Arguments \
src/Module_Settings \
src/Module_Date_Search \
src/Module_Protect \
src/Module_Tree \
src/Module_Compression

# Object file output directory
OBJ_DIR = build

# Compiler and flags
CC = gcc
CFLAGS = -Wall -O2 $(addprefix -I, $(SRC_DIRS))
LDFLAGS = -lcrypto

# Source files (explicitly listed)
SRCS = \
src/main.c \
src/Module_Arguments/Arguments.c \
src/Module_Settings/Settings.c \
src/Module_Date_Search/Date_Search.c \
src/Module_Protect/Protect.c \
src/Module_Tree/Tree_Structure.c \
src/Module_Compression/Huffman_Coding.c

# Object files (mirrored structure in build/)
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

# Default rule: build target and symbolic link
all: $(TARGET) $(ALIAS)

# Rule to build the main executable
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to create the symbolic link (overwrites if exists)
$(ALIAS): $(TARGET)
	@mkdir -p $(dir $@)
	ln -sf $(notdir $<) $@

# Compile each .c to .o into the build/ folder
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Cleanup: remove all generated files
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(ALIAS)

.PHONY: all clean
