# Define directories
SRC_DIR = saci-core
OBJ_DIR = build

CFLAGS = -Wall -Wextra -O0 -g \
	-I../saci/src \
	-I../saci/deps/glad/include \
	-I../saci/deps/glfw/include \
	-I../saci/deps/stb \
	-I../saci/deps/model-loading \
	-I./deps \
	-DSACI_DEBUG_MODE_ALL

COMPILER ?= gcc
CC = $(COMPILER)
ifeq ($(COMPILER),clang)
	COMPILER = clang	
endif

# Additional sources from the saci project (SACI_SRC)
SACI_SRC = \
	../saci/src/saci-core/**.c \
	../saci/src/saci-utils/**.c \
	../saci/deps/glad/src/glad.c

# Libraries to link with
LIBS = -L../saci/deps/glfw/lib/GLFW/ -lglfw -llua5.4 -lm \
	-Wl,-rpath,/home/ryan/projects/c/saciGL/saci/deps/glfw/lib/GLFW

# List of source files in the saci-core directory + additional sources from SACI_SRC
SOURCES = $(wildcard $(SRC_DIR)/*.c) $(SACI_SRC)

# Create object files from sources
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Create an executable for each source file (without extension)
EXECUTABLES = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%)

# Default target
all: $(EXECUTABLES)

# Rule to compile the .c files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)   # Create the build directory if it doesn't exist
	$(CC) $(CFLAGS) -c $< -o $@        # Compile the .c file into an .o object file

# Rule to link the object files into an executable
$(OBJ_DIR)/%: $(OBJ_DIR)/%.o
	$(CC) $^ $(LIBS) -o $@   # Link object files and libraries to create the executable

# Clean up the build directory
clean:
	rm -rf $(OBJ_DIR)

# Optional: Print the list of all sources and executables
list:
	@echo "Sources: $(SOURCES)"
	@echo "Executables: $(EXECUTABLES)"
