# Choose compiler (default: gcc)
COMPILER ?= gcc
CC = $(COMPILER)
ifeq ($(COMPILER),clang)
	COMPILER = clang	
endif

# Detect Emscripten
ifneq (,$(findstring emcc,$(COMPILER)))
	TARGET_PLATFORM = emscripten
	CC = emcc
else
	TARGET_PLATFORM = native
endif

# Base Compiler Flags
CFLAGS_BASE = -x c -std=c11 -Wall -Wextra -O0 -g \
	-I../saci/include \
	-I../saci/src \
	-I../saci/deps/stb \
	-I../saci/deps/memory \
	-I../saci/deps/model-loading \
	-I../saci/deps/ryanqueirozs \
	-DSACI_DEBUG_MODE_ALL \
	-DSACI_LIB_PATH=\"./\" \
	-DSACI_DEFAULT_CONFIG_PATH=\"../saci/saci-config.lua\"

# Platform-specific flags
ifeq ($(TARGET_PLATFORM),emscripten)
	CFLAGS = $(CFLAGS_BASE) \
		-s USE_WEBGL2=1 \
		-s FULL_ES3=1 \
		-s MIN_WEBGL_VERSION=2 \
		-s WASM=1 \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s ASSERTIONS=1 \
		-s GL_ASSERTIONS=1 \
		-s GL_DEBUG=1 \
		--shell-file shell.html \
		-DSACI_PLATFORM_EMSCRIPTEN
else
	CFLAGS = $(CFLAGS_BASE) \
		-DSACI_PLATFORM_NATIVE
endif

# Sources for the saci library (adjust paths if needed)
# Exclude emsdk files for native build, exclude glfw files for emscripten build
ifeq ($(TARGET_PLATFORM),emscripten)
	SACI_SRC := $(shell find ../saci/src/saci_platform -name '*.c' ! -name 'glfw.c' ! -name 'dependency.c' ! -name 'gl4.c')
else
	SACI_SRC := $(shell find ../saci/src/saci_platform -name '*.c' ! -name 'emsdk.c' ! -name 'gles3.c')
endif
SACI_SRC += $(shell find ../saci/src/saci_util ../saci/src/saci ../saci/src/saci_core -name '*.c')

# Example directories
EXAMPLES = \
	2d/shapes \
	3d/rotating-cube \
	3d/stanford-bunny-model \
	renderer-config/empty-renderer \
	texture/basic-img

# Sources for each example (just main.c for now)
EXAMPLE_SRCS = $(addsuffix /main.c, $(EXAMPLES))

# Targets per example - different extensions for emscripten
ifeq ($(TARGET_PLATFORM),emscripten)
	TARGETS = $(EXAMPLES:%=build/%/index.html)
else
	TARGETS = $(EXAMPLES:%=build/%/main)
endif

.PHONY: all clean run serve

# Default target: build all examples or a specified one
ifeq ($(TARGET),)
all: $(TARGETS)
else
ifeq ($(TARGET_PLATFORM),emscripten)
all: build/$(TARGET)/index.html
else
all: build/$(TARGET)/main
endif
endif

# Rule to build native examples
build/%/main: %/main.c $(SACI_SRC)
	@echo "\nBuilding native example $< with $(CC)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(SACI_SRC) -o $@ $(LIBS)
	@echo "Build succeeded for $<\n"

# Rule to build Emscripten examples
build/%/index.html: %/main.c $(SACI_SRC)
	@echo "\nBuilding Emscripten example $< with $(CC)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(SACI_SRC) -o $@ $(EMSCRIPTEN_LIBS)
	@echo "Build succeeded for $<\n"

# Libraries to link against
LIBS = -llua5.4 -lm -Wl,-rpath,$(PWD)/../saci/deps/glfw/lib/GLFW

# Emscripten-specific libraries and flags (GLFW removed)
EMSCRIPTEN_LIBS = \
	-s LLD_REPORT_UNDEFINED

# Run a specific example (native only)
run: all
ifeq ($(TARGET_PLATFORM),emscripten)
	@echo "Emscripten targets cannot be run directly. Use 'make serve' instead."
else
	@echo "\nAvailable examples to run:"
	@$(foreach ex, $(EXAMPLES), echo $(ex);)
	@read -p "Enter example path to run (e.g., 2d/shapes): " ex && \
		export LD_LIBRARY_PATH=../saci/deps/glfw/lib/GLFW:$$LD_LIBRARY_PATH && \
		./build/$$ex/main
endif

# Serve Emscripten builds locally
serve: all
ifeq ($(TARGET_PLATFORM),emscripten)
	@echo "Starting local server for Emscripten builds..."
	@python3 -m http.server 8000 --directory build
else
	@echo "Serve target is only for Emscripten builds. Use 'make run' for native."
endif

# Clean all build files
clean:
	rm -rf build
	@echo "\nCleaned all build files."

# Special target to build for Emscripten
emscripten:
	@$(MAKE) COMPILER=emcc

# Special target to build for WebAssembly with optimization
wasm-release:
	@$(MAKE) COMPILER=emcc CFLAGS="$(subst -O0,-O3,$(CFLAGS_BASE)) -s USE_WEBGL2=1 -s FULL_ES3=1 -s MIN_WEBGL_VERSION=2 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -DSACI_PLATFORM_EMSCRIPTEN"
