# Contributing to SACI

Thank you for considering contributing to SACI! This guide will help you
understand the coding standards, best practices, and workflow to follow when
making changes to the `saci-core` and `saci-lib` modules.

## Table of Contents

- [Code Structure and Organization](#Code-Structure-and-Organization)
    - [Header Files](#Header-Files)
    - [Source Files](#Source-Files)
    - [Examples](#Examples)
- [Coding Standards](#Coding-Standards)
    - [General Guidelines](#General-Guidelines)
        - [Consistency](#Consistency)
        - [Error Handling](#Error-Handling)
        - [Logs](#Logs)
        - [Magic Values](#Magic-Values)
        - [Abstraction](#Abstraction)
    - [Naming Conventions](#Naming-Conventions)
        - [Base naming convention](#Base-naming-convention)
        - [Header guard](#Header-guard)
        - [Structs](#Structs)
        - [Functions](#Functions)
        - [Variables](#Variables)
        - [Constants](#Constants)
        - [Static variables or constants](#Static-variables-or-constants)
        - [Enums](#Enums)
        - [Macros](#Macros)
- [Header File Documentation](#Header-File-Documentation)
- [Header file Declarations](#Header-file-Declarations)
- [Source File Implementations](#Source-File-Implementations)
- [Submitting Contributions](#Submitting-Contributions)


## Code Structure and Organization

When contributing to SACI, ensure you follow the existing code’s structure and
organizational principles:

### Header Files

Header files must

- Be placed in `saci/include/` if part of the library;
- Be place in `saci/deps/` if a dependency to the library;
- Only declare functions, types, constants, and macros. **NO implementation or
  logic.**
- Provide detailed function documentation using [Doxygen
  tags](https://www.doxygen.nl/manual/commands.html).

Example:
`/saci/include/saci-core/sc-camera.h`

### Source files

Source files must:
- Be place in `saci/src/` if part of the library.
- Be place in `saci/deps/` if a dependency to the library.
- Implement functions in source files, including minimal and necessary headers.
- Use helper functions to maintain modularity and clarity.
- Match function definitions with the appropriate source files (e.g., rendering
  logic in `sc-rendering.c`).

Example:
`/saci/src/saci-core/sc-camera.c`

### Examples

Examples must:
- Be placed in `examples/` directory, following the "type" of the example (2d,
  3d, texture, audio), and then the name of the example (basic-img for
  example).
- Contain a CMakeLists.txt to build it, linking it with saciLIB or saciCORE.

## Coding Standards

### General Guidelines:

#### Consistency
Follow the current SACI code style. Code style might evolve, but no major
changes are expected soon. Use the `.clang-format` provided in the root of the
project.

#### Modularity
Keep functions focused on a single responsibility. Use helper functions to
simplify large or complex tasks. Split functions if it improves clarity without
overcomplicating logic.

For example: 
```c
/**
 * @brief Creates the sc_Renderer struct
 *
 * @details
 * This functions creates, initializes and sets all of the sc_Renderer related shaders and
 * OpenGL context.
 *
 * @param generateDefaults A boolean to generate defaulted shaders and OpenGL context.
 * @return A new sc_Renderer* either defaulted or not. Can return null
 */
sc_Renderer* sc_CreateRenderer(saci_Bool generateDefaults);
``` 
A function like `sc_CreateRenderer` might perform multiple tasks (creation and
initialization), but its logic is subdivided into functions
like`__sc_initRenderer`, `__sc_initializeRenderValues`,
`__sc_renderBatch_ResizeInternal` and others. This should apply to the entire
codebase.

#### Error Handling

Address all edge cases and errors, especially for memory allocation and
dependencies. Document any unhandled errors, such as returning `NULL` values.

- Return early;
- Memory allocation should always be checked;
- Ensure resource management;
- Do not check the same thing twice as it can lower down performance;
- OpenGL already logs errors through `glDebugMessageCallback`, adding new error
  checks can be done but are not adviced;

**Important Note on ASSERT**: using `assert` is adviced only in core
functionalities like creation of a `sc_Renderer` but not on a smaller task like
texture loading.

#### Logs 

Use `SACI_LOG_PRINT` for significant actions (e.g., creation,
deletion, configuration changes) and appropriate log levels (INFO, WARN,
ERROR). 
- **EVERY error should be logged.** 
- Use `#if defined(SACI_DEBUG_MODE)` or `#if
  defined(SACI_DEBUG_MODE_{FILE_MAIN_NAME})` to enable debug logs if necessary.
- **DO NOT log unnecessary information**.
- Avoid using plain `printf` or print-related functions.

Logging **Type**:

| Type           | Use                                                                                                           |
|----------------|---------------------------------------------------------------------------------------------------------------|
| SACI_LOG_DEBUG | Information that helps developers understand internal workings or trace execution for debugging purposes.     |
| SACI_LOG_INFO  | Information useful to the end-user regarding application state or normal operations (e.g., resource loading). |
| SACI_LOG_WARN  | Uninitialized values, unexpected parameters                                                                   |
| SACI_LOG_ERROR | Failures, errors or unexpected values                                                                         |


Logging **Level**:

| Level                          | Use                                                           |
|--------------------------------|---------------------------------------------------------------|
| SACI_LOG_SEVERITY_NOTIFICATION | Only informational messages, no action needed.                |
| SACI_LOG_SEVERITY_LOW          | Minor issues, such as deprecation or performance hints.       |
| SACI_LOG_SEVERITY_MEDIUM       | Issues that could cause bugs or notable performance problems. |
| SACI_LOG_SEVERITY_HIGH         | Critical errors that will likely lead to application crashes. |

Logging **context**:

| Contexts                  | Use                                                                     |
|---------------------------|-------------------------------------------------------------------------|
| SACI_LOG_CONTEXT_OPENGL   | Logs related to OpenGL operations (e.g., shader errors).                |
| SACI_LOG_CONTEXT_RENDERER | Logs from the rendering pipeline (e.g., draw call issues, performance). |
| SACI_LOG_CONTEXT_STBI     | Logs from the STBI library for image loading (e.g., texture issues).    |

Example: 
```c
    if (texData->width <= 0 || texData->height <= 0) {
        SACI_LOG_PRINT(
            SACI_LOG_TYPE_ERROR, // Type
            SACI_LOG_LEVEL_MEDIUM, // Level
            SACI_LOG_CONTEXT_STBI, // Context
            "Texture coudn't be loaded: Texture Width or Height is equal to 0"); // Message
    }
```

#### Magic Values
Magic values are unexplained values (strings, numbers, floats), magic values
reduce maintainability, if you cannot remember what you wrote yesterday,
imagine a random value you wrote years ago. **AVOID** magic values whenever
possible.

- Exceptions:
    - The value is universal, for example: returning `NULL`, `-1` and `0` to
      represent failure in functions.
    - The context in which the value is used makes its meaning immediately
      clear, view the following example:

Imagine the following function that does a math operation depending on the amount of vertices of a "shape":

```c
// This is just an example, no function like this exists

void DoSomethingWithShapeVertices() {
    SomeVerticeOperation(3); // BAD
}

```
Whe can improve this function by:

**Most recommended action**:
```c
void DoSomethingWithShapeVertices() {
    const int triangleVertices = 3;
    SomeVerticeOperation(triangleVertices); // OK. Declared what 3 is!
}
```
In this, we define a constant value that represents what the magic number `3`
represents.

**Also works**:
```c
void DoesSomethingWithTriangleVertices() {
    SomeVerticeOperation(3); // OK. With a small context, you know it's 3 vertices of a triangle!
}
```
In this, we explain through the function's name what it does; it's implicit
that `3` represents the vertices of a triangle.

#### Abstraction
The purpose of `saci-core` is to provide a layer of abstraction over
dependencies like OpenGL such that `saci-lib` can remain "changeless"
over time. This abstraction ensures that the underlying functionality can
evolve without requiring changes to the higher-level code. By isolating the
details of the dependencies, we ensure that our codebase remains flexible and
easy to maintain, even if the underlying libraries (e.g., OpenGL) change their
API or behavior over time.

However, the goal of this library is simplicity, and overcomplicating the
abstraction would defeat this purpose. Abstractions should be implemented
carefully and only when necessary. We aim to avoid "over-engineering" while
still providing a clean, maintainable interface.

- `saciCORE` provides an abstration over base OpenGL functionality,
  shader-loading, rendering, loading textures and more.
- `saciLIB` bases itself on `saciCORE` and simplifies it's workflow and reduces
  verbosity
- **DO NOT use dependencies in `saci-lib`.**

### Naming Conventions:

#### Base naming convention:
1. Use the `sc_` prefix for public types, functions, and variables in
`saci-core`; use `sl_` for `saci-lib` and `saci_` for none of the previous.
2. Use the `__sc_` prefix for private/helper functions in `saci-core`, and
`__sl_` for `saci-lib`.
3. Use the `SACI_` prefix with uppercase names for constants and macros.

### File Naming

Files must:
- Be in lowercase;
- Have its words divided by `-`;
- Not repeat, if a Header file with `header-file-name.h` exists, there cannot
  be another `header-file-name.h` in another directory;

Example:
`/saci/include/saci-core/sc-camera.h`

#### Header guard

Header guards must:
- Be in uppercase;
- Begin with `__` and end with `__`;
- Have its words separeted by `_`;
- Contain the whole path from the include/ directory. 

Example: `/saci/include/saci-core/sc-camera.h` should be defined as
`__SACI_CORE_SC_CAMERA_H__`.

#### Structs

Structs follow the base [Naming Conventions](#Naming-Conventions), but must:
- Use PascalCase after the `sc_`, `sl_` or `saci_`, prefix;
- Members follow [variable](#Variables) style

```c
typedef struct sc_Camera {
    saci_Vec3 position;
    saci_Vec3 target;
    saci_Vec3 up;

    float fov;
    float aspectRatio;
    float near;
    float far;
} sc_Camera;
```

#### Functions

**FUNCTIONS MUST** Start with what it modifies or what are most closely
related. Example:
```c
/**
 * @brief Loads texture data from a file into a sc_TextureData* without creating an OpenGL
 * texture.
 *
 * @param path The file path of the texture to load.
 * @param flipImg A boolean indicating whether the image should be flipped vertically.
 * @param texData A pointer to an sc_TextureData structure that will hold the loaded
 * texture information.
 */
void sc_Texture_LoadData(const char* path, saci_Bool flipImg, sc_TextureData* texData);
```
In the code above , it's declared a function that loads texture from a file
into a `sc_TextureData*`. This function is related to **TEXTURES**, so we need
to begin the name of the function with `Texture`.

Other notes:
- If a function recieves no parameters, it should be declared as
  `MyFunction(void)` (with the `void` keyword inside the paramter list)
- Be descriptive! No one should guess what the function does nor have to read
  the whole documentation.


**PUBLIC functions** follow the base [naming conventions](#Naming-Conventions), but should:
- Use PascalCase after the `sc_`, `sl_` or `saci_` prefix;

Example:
```c
sc_Camera sc_Camera_GetDefault2DCamera(void);
```

**PRIVATE functions** follow the base [naming convetions](#Naming-Convetions), but should:
- Begin with `__` prefix
- Use camelCase after the `sc_`, `sl_` or `saci_` prefix;

Example:
```c
void __sc_renderBatch_empty(sc_RenderBatch* renderBatch);
```
`renderBatch` is the context, empty is what the function does to the context.

#### Variables

Variables must:
- Contain `saci_`, `sc_` or `sl_` prefix if public;
- Be in camelCase;
- Be discriptive, **DO NOT use one one letter words**, unless it is in a
  forloop;
```c
    for (saci_u32 i = 0; i < renderer->renderBatch.renderCallCount; ++i) {
    ...
        sc_RenderCall* call = &renderer->renderBatch.renderCalls[i];
        ...
        saci_u32 vertexCount = 0;
        if (call->renderMode == GL_TRIANGLES) {
            vertexCount = 3; // Triangle needs 3 vertices
        } else if (call->renderMode == GL_QUADS) {
            vertexCount = 6; // A quad uses 6 vertices (2 triangles)
        } else if (call->renderMode == GL_LINES) {
            vertexCount = 2; // Line needs 2 vertices
        }
    ...
    }
``` 
Note how the `i` variable in the for loop can remain a single letter variable,
but all of the variables along the code are descriptive;

#### Constants

Global constants must:
- Be in all uppercase;
- Have its words separeteed by `_`;

Private constants follow variable guideline.

### Static variables or constants

Static variables or constants, follow their respective rules, but must:
- Contain a `s` prefix and follow capitalized.

Example:
```c
static struct sc_RenderConfig {
    ...
} sc_sRenderConfig;
```

### Enums

Enum naming follows the same style of structures, and enum values follows the
style of constants.

```c
typedef enum saci_ContextLevel {
    SACI_LOG_CONTEXT_OPENGL = 0,
    SACI_LOG_CONTEXT_RENDERER = 1,
    SACI_LOG_CONTEXT_STBI = 2,
} saci_ContextLevel;
```

Note the use of `saci_` as this is a utility enum (not a part of core or lib).
Also, note how every constant inside the enum follows the style of constants

### Macros

Macros follow the same guide of constants.

## Header File Documentation:

1. Each header should start with a brief description of its purpose using a
`@file` comment block.
    - Keep it simple: "Describe what the code does and why, not how at a
      micro-level" - [LLVM Coding
      Standard](https://llvm.org/docs/CodingStandards.html#file-headers).
2. Follow the [Doxygen commenting
guide](https://www.doxygen.nl/manual/docblocks.html).
    - Use Javadoc-style comment blocks.
    - Use `@` for Doxygen tags.

## Header file Declarations

1. **Only usefull declarations**: Only declare what will be used in another
   file or what will/should be used by the library user.
2. **Private over public**: Private declarations should be prefered if they are:
    - Unimportant to the user;
    - Unsafe for the user; 
    - Achieve minimal functionality; (Helper function)

## Source File Implementations

1. **Order**: Follow an appropriate order of implementation.
    - Place helper functions at the top.
    - Implement functions/structs according to the header declarations.
    - If `Function1` is defined before `Function2` in the header, implement
      `Function1` before `Function2`.
2. **Commenting**: Use minimal comments. Let code naming and logic be
self-explanatory.
3. **Dependencies**: Ensure proper resource management and deletion.
4. **Error Checking**: Ensure robust error and memory management.
5. **Memory Management**: Always clean up allocated memory in destructors or
termination functions to prevent memory leaks.
    - Use `free()` properly and set pointers to `NULL` after freeing memory.

## Submitting Contributions

1. **Branch Naming**:
    - For features, use `feature/{FEATURE_NAME}`;
    - For refactoring, use `refactor/{REFACTOR_NAME}`;
    - For documentation, use `doc/{DOCUMENTATION_NAME}`.

2. **Commit Messages**: Use descriptive commit messages.
    - Make commits feature-focused. Take this with a grain of salt.
    - Messages should start with `doc:` for documentation, `feature:` for
      features, and `refactor:` for refactoring.

3. **Workflow**: After testing a branch, it may be merged into the `develop`
branch and eventually into `main`.

