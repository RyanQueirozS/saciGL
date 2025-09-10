# Choose compiler (default: gcc)
COMPILER ?= gcc
CC = $(COMPILER)
ifeq ($(COMPILER),clang)
	COMPILER = clang	
endif

# Compiler Flags
CFLAGS = -Wall -Wextra -O0 -g \
	-I../saci/include \
	-I../saci/src \
	-I../saci/deps/stb \
	-I../saci/deps/memory \
	-I../saci/deps/model-loading \
	-I../saci/deps/ryanqueirozs \
	-DSACI_DEBUG_MODE_ALL \
	-DSACI_LIB_PATH=\"./\" \
	-DSACI_DEFAULT_CONFIG_PATH=\"../saci/saci-config.lua\"

# Sources for the saci library (adjust paths if needed)
SACI_SRC = \
	../saci/src/saci-backend/**.c \
	../saci/src/saci-utils/**.c \
	../saci/src/saci-main/**.c \

# Example directories
EXAMPLES = \
	2d/shapes \
	3d/rotating-cube \
	3d/stanford-bunny-model \
	renderer-config/empty-renderer \
	texture/basic-img

# Sources for each example (just main.c for now)
EXAMPLE_SRCS = $(addsuffix /main.c, $(EXAMPLES))

# Targets per example
TARGETS = $(EXAMPLES:%=build/%/main)

.PHONY: all clean run

# Default target: build all examples or a specified one
ifeq ($(TARGET),)
all: $(TARGETS)
else
all: build/$(TARGET)/main
endif

# Rule to build each example independently, compiling example + library sources
build/%/main: %/main.c $(SACI_SRC)
	@echo "\nBuilding example $< with $(CC)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(SACI_SRC) -o $@ $(LIBS)
	@echo "Build succeeded for $<\n"

# Libraries to link against
LIBS = -llua5.4 -lm -Wl,-rpath,$(PWD)/../saci/deps/glfw/lib/GLFW

# Run a specific example
run: all
	@echo "\nAvailable examples to run:"
	@$(foreach ex, $(EXAMPLES), echo $(ex);)
	@read -p "Enter example path to run (e.g., 2d/shapes): " ex && \
		export LD_LIBRARY_PATH=../saci/deps/glfw/lib/GLFW:$$LD_LIBRARY_PATH && \
		./build/$$ex/main

# Clean all build files
clean:
	rm -rf build
	@echo "\nCleaned all build files."
