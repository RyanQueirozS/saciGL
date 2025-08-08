# Choose compiler (default: gcc)
COMPILER ?= gcc
CC = $(COMPILER)

# Compiler Flags
CFLAGS = -Wall -Wextra -O0 -g \
	-I../saci/include \
	-I../saci/deps/glad/include \
	-I../saci/deps/glfw/include \
	-I../saci/deps/stb \
	-I../saci/deps/memory \
	-I../saci/deps/model-loading \
	-I../saci/deps/ryanqueirozs \
	-DSACI_LIB_PATH=\"./\" \
	-DDYLILO_IMPL

# Clang-specific tweaks (optional)
ifeq ($(COMPILER),clang)
	COMPILER = clang	
endif

# Static library path
SACI_LIB_PATH = ../saci/build/lib
SACI_LIB = -L$(SACI_LIB_PATH) -lsaci

# Linking libraries and flags
LIBS = $(SACI_LIB) -L../saci/deps/glfw/lib/GLFW/ -lglfw -llua5.4 -lm \
    -Wl,-rpath,$(PWD)/../saci/deps/glfw/lib/GLFW

# Example directories
EXAMPLES = \
	2d/shapes \
	3d/rotating-cube \
	3d/stanford-bunny-model \
	renderer-config/empty-renderer \
	texture/basic-img

# Targets for all example builds
TARGETS = $(EXAMPLES:%=build/%/main)

# If a specific target is passed, use it, else default to all
ifeq ($(TARGET),)
    TARGETS := $(TARGETS)
else
    TARGETS := build/$(TARGET)/main
endif

.PHONY: all clean run

# Default target to build all examples or the specified one
all: $(TARGETS)

# Rule to compile each example
build/%/main: %/main.c
	@echo "\nBuilding $< with $(CC)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)
	@echo "Build succeeded for $<\n"

# Run a specific example
run: all
	@echo "\nAvailable examples to run:"
	@$(foreach ex, $(EXAMPLES), echo $(ex);)
	@read -p "Enter example path to run (e.g., 2d/shapes): " ex && \
		export LD_LIBRARY_PATH=../saci/deps/glfw/lib/GLFW:$$LD_LIBRARY_PATH && \
		./build/$$ex/main

# Clean build files
clean:
	rm -rf build
	@echo "\nCleaned all build files."
