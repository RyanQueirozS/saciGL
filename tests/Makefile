# Compiler flags
CFLAGS = -Wall -Wextra -O0 -g \
	-I../saci/src \
	-I../saci/include \
	-I../saci/deps/stb \
	-I../saci/deps/model-loading \
	-I../saci/deps/ryanqueirozs/ \
	-I../saci/deps/memory/ \
	-I./deps/ \
	-I./mocks/ \
	-DSACI_DEBUG_MODE_ALL \
	-DSC_RENDERER_STRUCT_EXPOSE \
	-DSACI_TEST_BUILD \
	-DSACI_RENDERING_DISABLED \
	-DSACI_LIB_PATH=$(pwd)

COMPILER ?= gcc
CC = $(COMPILER)
ifeq ($(COMPILER),clang)
	COMPILER = clang	
endif

# Sources for the saci library
SACI_SRC = \
	../saci/src/saci-backend/**.c \
	../saci/src/saci-utils/**.c \
	../saci/src/saci-main/**.c 

# Sources for the current project - FIXED
SOURCES = \
	tests_main.c \
	$(wildcard unit/*.c) \
	$(wildcard unit/**/*.c) \
	$(wildcard integration/*.c) \
	$(wildcard integration/**/*.c)

TARGET = build/test_runner

.PHONY: all clean run

all: $(TARGET)

# Libraries to link against
LIBS = -L../saci/deps/glfw/lib/GLFW/ -lglfw -llua5.4 -lm \
    -Wl,-rpath,$(PWD)/../saci/deps/glfw/lib/GLFW

# Rule to compile all sources into one binary
$(TARGET): $(SOURCES) $(SACI_SRC)
	@echo "\nBuilding test runner with $(CC)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(SOURCES) $(SACI_SRC) -o $@ $(LIBS)
	@echo "Build succeeded for test runner\n"

# Run the test runner
run: $(TARGET)
	export LD_LIBRARY_PATH=../saci/deps/glfw/lib/GLFW:$$LD_LIBRARY_PATH && \
	./$(TARGET)

list:
	@echo "Sources: $(SOURCES)"
	@echo "Target: $(TARGET)"

# Clean build files
clean:
	rm -rf build
	@echo "\nCleaned all build files."
