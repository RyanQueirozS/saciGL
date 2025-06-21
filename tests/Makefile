# Compiler flags
CFLAGS = -Wall -Wextra -O0 -g \
	-I../saci/src \
	-I../saci/include \
	-I../saci/deps/glad/include \
	-I../saci/deps/glfw/include \
	-I../saci/deps/stb \
	-I../saci/deps/model-loading \
	-I./deps \
	-DSACI_DEBUG_MODE_ALL \
	-DSC_RENDERER_STRUCT_EXPOSE \
	-DSACI_TEST_BUILD \
	-DSACI_RENDERING_DISABLED

COMPILER ?= gcc
CC = $(COMPILER)
ifeq ($(COMPILER),clang)
	COMPILER = clang	
endif

# Sources for the saci library
SACI_SRC = \
	../saci/src/saci-backend/**.c \
	../saci/src/saci-utils/**.c \
	../saci/src/saci-main/**.c \
	../saci/deps/glad/src/glad.c

SRC_DIR = saci-core
# Sources for the current project
SOURCES = $(wildcard $(SRC_DIR)/*.c)

TARGETS= $(SOURCES:%=build/%)

.PHONY: all clean run

all: $(TARGETS)

# Libraries to link against
LIBS = -L../saci/deps/glfw/lib/GLFW/ -lglfw -llua5.4 -lm \
    -Wl,-rpath,$(PWD)/../saci/deps/glfw/lib/GLFW


# Rule to compile each example
build/%: % $(SACI_SRC)
	@echo "\nBuilding $< with $(CC)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(SACI_SRC) -o $@ $(LIBS)
	@echo "Build succeeded for $<\n"

# Run a specific example
run: all
	@echo "\nAvailable tests to run:"
	@$(foreach ex, $(EXAMPLES), echo $(ex);)
	@read -p "Enter test path to run: " ex && \
		export LD_LIBRARY_PATH=../saci/deps/glfw/lib/GLFW:$$LD_LIBRARY_PATH && \
		./build/$$ex/main

list:
	@echo "Sources: $(SOURCES)"
	@echo "Targets: $(TARGET)"

# Clean build files
clean:
	rm -rf build
	@echo "\nCleaned all build files."
