# Choose compiler (default: gcc)
COMPILER ?= gcc

ifeq ($(COMPILER),emcc)
	CC = emcc
else
	CC = $(COMPILER)
endif

# Compiler Flags
CFLAGS = -Wall -Wextra -O0 -g \
	-I../saci/include \
	-I../saci/src \
	-I../saci/deps/glad/include \
	-I../saci/deps/glfw/include \
	-I../saci/deps/stb \
	-I../saci/deps/memory \
	-I../saci/deps/lua \
	-I../saci/deps/model-loading \
	-DSACI_TEST_BUILD \
	-DSACI_DEBUG_MODE_ALL

# Emscripten-specific flags
ifeq ($(COMPILER),emcc)
	CFLAGS += -s USE_GLFW=3 -s FULL_ES3=1 -s ASYNCIFY -s ALLOW_MEMORY_GROWTH=1
	LIBS =
	OUTPUT_EXT = .html
else
	LIBS = -L../saci/deps/glfw/lib/GLFW/ -lglfw -lm \
		-Wl,-rpath,$(PWD)/../saci/deps/glfw/lib/GLFW
	OUTPUT_EXT =
endif

# Source files from saci-core and saci-utils
SACI_SRC = \
	$(wildcard ../saci/src/saci-backend/*.c) \
	$(wildcard ../saci/src/saci-main/*.c) \
	$(wildcard ../saci/src/saci-utils/*.c) \
	../saci/deps/glad/src/glad.c

# Example directories
EXAMPLES = \
	2d/shapes \
	3d/rotating-cube \
	3d/stanford-bunny-model \
	renderer-config/empty-renderer \
	texture/basic-img

# Targets for all example builds
TARGETS = $(EXAMPLES:%=build/%/main$(OUTPUT_EXT))

# If a specific target is passed, use it, else default to all
ifeq ($(TARGET),)
    TARGETS := $(TARGETS)
else
    TARGETS := build/$(TARGET)/main$(OUTPUT_EXT)
endif

.PHONY: all clean run

# Default target to build all examples or the specified one
all: $(TARGETS)

# Rule to compile each example
build/%/main$(OUTPUT_EXT): %/main.c $(SACI_SRC)
	@echo "\nBuilding $< with $(CC)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(SACI_SRC) -o $@ $(LIBS)
	@echo "Build succeeded for $< -> $@\n"

# Run is not used for emscripten builds, but kept for native
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



