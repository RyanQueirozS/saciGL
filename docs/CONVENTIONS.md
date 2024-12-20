# saciGL Conventions
 
## Naming

### General Naming Rules
1. **GLOBAL HEADER** elements must be prefixed with:
   - `sc_` if part of saciCore.
   - `sl_` if part of saciLib.
   - `sa_` if not one of the above.

2. **GLOBAL SOURCE** elements must be prefixed with:
   - `__sc_` if part of saciCore.
   - `__sl_` if part of saciLib.
   - `__sa_` if not one of the above.

3. Rules 1 and 2 **do not apply** in **LOCAL** scopes (e.g., inside functions, structs, etc.).

4. Prefixes and suffixes should maintain their defined casing and should not affect overall casing.
 
5. Names should use keywords that go from most important to least, and separate
   the **what it is RELATED to (SCOPE)**. Example: 
```c
// Bad:
typedef void (*sc_FunctionPtrToHandleMousePos_t)(sc_Window_t* window, double posx, double posy);
// We defined a type for a function ptr, that only tells what it is used for in
// the end `HandleMousePos`

// Good:
typedef void (*sc_Event_MousePosHandler_t)(sc_Window_t* window, double posx, double posy);
// We defined a type for a function ptr, that tells the SCOPE first (separated
// between underlines), and then we explain what it does in simple terms, it's a
// `mouse pos handler`.
```

---

### Element-Specific Naming Rules

| ELEMENT              | Convention             | GLOBAL HEADER             | GLOBAL SOURCE               | LOCAL                  |
|----------------------|------------------------|---------------------------|-----------------------------|------------------------|
| **Header Guard**     | `__` + ALL_CAPS + `__` | `__MY_COMPLETE_PATH_H__`  | **N/A**                     | **N/A**                |
| **Defines**          | ALL_CAPS               | `sa_HEADER_DEFINE`        | `__sa_SOURCE_DEFINE`        | `LOCAL_DEFINE`         |
| **Macros**           | ALL_CAPS + `_m`        | `sa_HEADER_MACRO_m(x)`    | `__sa_SOURCE_MACRO_m(x)`    | `LOCAL_MACRO_m(x)`     |
| **Variables**        | lower_case             | `sc_header_var`           | `__sc_source_var`           | `local_var`            |
| **Constants**        | ALL_CAPS               | `sc_HEADER_CONST`         | `__sc_SOURCE_CONST`         | `LOCAL_CONST`          |
| **Static**           | lower_case + `_s`      | `sc_static_header_var_s`  | `__sc_static_source_var_s`  | `static_local_var_s`   |
| **Enum**             | camelCase + `_e`       | `sa_Header_Enum_e`        | `__sa_Source_Enum_e`        | `Local_Enum_e`         |
| **Enum Members**     | ALL_CAPS               | `sa_HEADER_ENUM_MEMBER`   | `__sa_SOURCE_ENUM_MEMBER`   | `SOURCE_ENUM_MEMBER`   |
| **Structs**          | camelCase + `_c`       | `sl_Header_Struct_c`      | `__sl_Source_Struct_c`      | `Local_Struct_c`       |
| **Struct Members**   | `m_` + lower_case      | **N/A**                   | **N/A**                     | `m_struct_member`      |
| **Functions**        | Pascal_Case            | `sc_Function_Def`         | `__sc_Function_Def`         | **N/A**                |
| **Functions Params** | lower_case             | **N/A**                   | **N/A**                     | `func_param`           |
| **Typedefs**         | camelCase + `_t`       | `sc_Header_Type_t`        | `__sc_Source_Type_t`        | `My_Local_Type_t`      |

NOTE:
1. `Header guards` should contain the full path related to `/saci/include/`.
   Example: `/saci/include/my-dir/my-file.h` will be `__MY_DIR_MY_FILE_H__`
2. `Static` values should be in **ALL CAPS** if a const and **lower case** if a
   variable. The suffix `_s` should remain on either.
3. Struct Members are **Non applicable** in global space, because a member is
   obviously defined locally inside a struct, obviously. Same thing as function
   parameters.
4. Functions are **Non applicable** in local space, because **C** doesn't allow
   it, there cannot be a function defined/implemented inside another. And even
   if it could it would be an aberration.
5. When defining a **STRUCT**, if it is a defined as a **TYPE**, it should be
   suffixed with a `_t`, and not a `_c`. `Types > Classes` when reading.
   
### Naming guidelines

C has many naming conventions with many different reasons to use each one of
them. Try to keep names the most discriptive as possible without
overcomplicating. Common names like `tmp`, `pos`, `dest` are acceptable only
because they are common. Example:

```c
int randomNumberGeneratedWithRand = rand(); // Bad
int a = rand(); // AWFUL
int random = rand(); // Good


int temporaryValue = 0; // OK
int temporary = 0; // Good
int tmp = 0; // GREAT
int adwadawhdkawj = 0; // AWFUL
```

#### Enum Members

Enum members should always begin with the enum name. Example: 
```c
enum sa_Log_Level_e {
    SA_LEVEL_INFO // BAD
    SA_LOG_LEVEL_DEBUG // GOOD
}
```

#### Functions

Should always explain what it does, not to the minimal level, but what it
accomplishes. `Does_Something`, `Creates_This`, `Does_That`, **SIMPLE**.

#### Function params

If a function param is a output param, use `_out` in the end.

#### Files

Files should be named:
- In lower-case with dashes(`-`) separating each word.
- Be descriptive of what they try to acomplish.
 
   
## Style

### Indentation

Use 4 space indentation. Tabs are bad for navigation, 2 is to little to
differenciate, 8 is a lot. Simple as that.

There should be **NO REASON** to have over 3 indentation levels. **IF and ONLY
IF** 4 indentation or more is needed - again, **highly improbable** - it can be
accepted.

### If and elses

#### Braces

`if` should contain braces whenthe code in the `if` statement does more than one thing. Example:

```c
// This ifstatement does more than one thing:
if (condition) valueTwo = 2, valueThree = 3;

// The correct way:
if (condition) {
    valueTwo = 2; // could be a ; or ,
    valueThree = 3; 
}
```

#### Elses

**AVOID ELSES** whenever possible. Elses are double-eged swords, they can
drastically improve readability in some cases, and infinitely worsen in some.
Just in case, **AVOID IT**. Prefere returning early, and **ONLY USING ELSE WHEN
NEEDED**. 

#### Nesting

**DO NOT** nest `if`s and `else`s. Simple as that.

### Line limit

**Lines should be kept at 80 CHARS** whenever possible. The `.clang-format` file does not
break lines, because sometimes it forces some ugly indentations. Prefere a
**READABLE over a 80 CHAR LINE**, if you need to break at 60, 90, or a 100, do
it. **DO NOT GO OVER 110** and whenever possible **BREAK AT 80 CHARS**.

### Whitespace

#### Empty lines

There should **NOT** be a empty line per function, struct and enum. Example: 
```c
// Without be:
void My_Func();

void Another_Func();

// Should be:
void My_Func();
void Another_Func();
```

**AVOID CREATING UNECESSARY EMPTY LINES**. Example:

```c
// One newline per line. BAD:
void My_Func() {
    int a = 0;
    
    int b = 1;
    
    if (condition) {
        ...
    }
}

// CORRECT:

void My_Func() {
    int a = 0; // could even be a , instead of ;
    int b = 1;
    
    if(condition) { // That whitespace above is acceptable
        ...
    }
}
```

#### Braces

As seen above. Use braces after functions, ifs, elses, switch and cases,
structs and all of the other things people debate about in regards to braces.

## Best Practices

1. **USE THE ALREADY IMPLEMENTED TYPES**, for integers, no `long long`, `short
   short`, use `sa_s8` to `sa_s64` and `sa_u8` to `sa_u64`, that goes
   for all types;
2. **AVOID MAGIC VALUES**. Only use them when it has implicit
   meaning (returning 0, -1 or NULL for fail);
3. **RETURN EARLY**;
4. **CHECK/HANDLE MEMORY ALLOCATION**;
5. **NO DEPENDENCIES on SACI LIB**. SaciCORE should provide enough abstraction
   layers;
6. **LOG ALL POSSIBLE ERRORS**;
7. **ASSERT ONLY WHEN NEEDED**. Only for critical operations, like
   renderer creation/library initialization;

### Public vs Non Public

When creating a global element, ask yourself if it really needs to be global.
The user should not know or see the inner workings of a function or data
structure, **UNLESS NEEDED**. Freedom of the users is a must but safety is far
more valuable. 

A good mindset to have is:
> If the user can screw up, they will.  

**DO NOT LET THE USER SCREW UP**.

### Code organization

Each code portion should be subdivided. Example:
```c
/* === Helper === */
// helper code goes here


/* === Implementations === */
// Implementations should go here
```
When in a HEADER FILE:

```c
/* === Renderering === */
// Rendering related code


/* === Model loading === */
// Model loading related code
```

### Logging

Use `sa_LOG_PRINT_m` for significant actions (e.g., creation,
deletion, configuration changes) and appropriate log levels (INFO, WARN,
ERROR). 
- **EVERY error should be logged.** 
- Use `#if defined(SA_DEBUG_MODE)` or `#if
  defined(SA_DEBUG_MODE_{FILE_MAIN_NAME})` to enable debug logs if necessary.
- **DO NOT log unnecessary information**.
- Avoid using plain `printf` or print-related functions.

Logging **Type**:

| Type           | Use                                                                                                           |
|----------------|---------------------------------------------------------------------------------------------------------------|
| SA_LOG_DEBUG | Information that helps developers understand internal workings or trace execution for debugging purposes.     |
| SA_LOG_INFO  | Information useful to the end-user regarding application state or normal operations (e.g., resource loading). |
| SA_LOG_WARN  | What may cause issues, the end user shouldn't need to see this                                                |
| SA_LOG_ERROR | Failures, errors or unexpected values                                                                         |


Logging **Level**:

| Level                          | Use                                                           |
|--------------------------------|---------------------------------------------------------------|
| SA_LOG_SEVERITY_NOTIFICATION | Only informational messages, no action needed.                |
| SA_LOG_SEVERITY_LOW          | Minor issues, such as deprecation or performance hints.       |
| SA_LOG_SEVERITY_MEDIUM       | Issues that could cause bugs or notable performance problems. |
| SA_LOG_SEVERITY_HIGH         | Critical errors that will likely lead to application crashes. |

Logging **context**:

| Contexts                  | Use                                                                     |
|---------------------------|-------------------------------------------------------------------------|
| SA_LOG_CONTEXT_OPENGL   | Logs related to OpenGL operations (e.g., shader errors).                |
| SA_LOG_CONTEXT_RENDERER | Logs from the rendering pipeline (e.g., draw call issues, performance). |
| SA_LOG_CONTEXT_STBI     | Logs from the STBI library for image loading (e.g., texture issues).    |

New contexts are welcomed!

---

### Abstraction

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

### Macros and Defines

Do not recreate macros, and use them when possible. Prefere `SA_SCAST_TO`
than the default casting, prefere `SA_ARRLEN` than `sizeof(array) /
sizeof(array[0])`.

Use `#define` only when needed.

### Variables

- Do not create unused variables.
- Always initialize variables.
- Use `const` whenever possible.

### Constants

Same rules as variables.

### Typedefs

Whenever defining a type, prefere the raw `struct` or `enum` form. Only use
`typedef` when **DEFINING A TYPE THAT WILL BE USED FREQUENTLY** and:
- is a opaque type;  
OR
- is a type that adds context for a given functionality;

Example:
```c
typedef int32_t sa_s32; // OK; reausable, common, usefull.

typedef sa_s32 sa_TextureID // GOOD; adds context.

typedef struct {
    float value1, value2;
    const char* STRING;
} sc_My_Internal_Struct_t // BAD. Will rarelly be used by the user.
```

Imagine reading the following:
```c
void sc_My_Func(sc_My_Data data) { // is sc_My_Data a struct, an enum? You cannot tell!
    ...
}

void sc_My_Func(struct sc_My_Data data) { // this is more readable, obviously a struct.
    ...
}
```

### Enum

Only use enums when: 
- `#define` would be anoing (having to `#define` hundreads of constants)
- "Type clarity" matters (the function recieves My_Enum instead of int)

### Functions

**Never**:
- Create overly-complicated functions.
    - Each function should be easy to read and not require a computer science
      degree to understand.
- Do multiple things in a single function. 
    - **ONLY WHEN NEEDED** a function can do multiple things, like
      `sc_Renderer_Create`, check example bellow.

**Always**:
- Use `const` to parameters that won't be modified.
- Understand if this function really does what it entails.
    - A function that creates, modifies, initializes and does a lot of
      different things will be hard to refactor.
    - Focus on modularity, keep functions - not small - but decent in scope
    - `FunctionThatDoesFoo()` might do `foo` and validate it, but not resize,
      modify already existing `foos`, etc.

About modularity:

```c
// See the BAD following function. It does multiple things, and doesn't
// subdivide it. There is a clear reason to do multiple things (creation,
// assertion, memory initialization, opengl initialization), so the best be is
// to refactor the huge code.
sc_Renderer* sc_Renderer_CreateDefault() {
    sc_Renderer* renderer = (sc_Renderer*)malloc(sizeof(sc_Renderer));
    if (!renderer) {
        SA_LOG_PRINT(SA_LOG_LEVEL_ERROR, SA_LOG_CONTEXT_RENDERER,
                       "Renderer could not be initialized");
        return NULL;
    }
    {
        renderer->renderBatch.renderCalls = NULL;
        renderer->renderBatch.renderCallCount = 0;
        renderer->renderBatch.capacity = 0;
        renderer->vao = 0;
        renderer->vbo = 0;
    }
    ArenaInit(&renderer->memoryContext, size);
    sc_RenderBatch* renderBatch = &renderer->renderBatch;
    if (newSize <= 0 || newSize <= renderBatch->renderCallCount) {
        SA_LOG_PRINT(SA_LOG_LEVEL_ERROR, SA_LOG_CONTEXT_RENDERER,
                       "RenderBatch new size is not valid");
        return;
    }
    ...
    // There is a LOT more, but you get the idea.
}

// How it gets improved
sc_Renderer* sc_Renderer_CreateDefault() {
    sc_Renderer* renderer = sc_Renderer_CreateEmpty(); // Creation gets divided.
    // Garbage numbers are inside of Creation function.
    if (!renderer) {
        SA_LOG_PRINT(SA_LOG_LEVEL_ERROR, SA_LOG_CONTEXT_RENDERER,
                       "Renderer could not be initialized");
        return NULL;
    }
    sc_Renderer_InitMemoryContext(renderer, SA_RENDER_BATCH_DEFAULT_CAPACITY);
    // Set a propper name for what the arena init function does to the renderer.

    __sc_Renderer_InitAll(renderer);
    // initializes all of the OpenGL context.
    SA_LOG_PRINT(SA_LOG_LEVEL_INFO, SA_LOG_CONTEXT_RENDERER,
                   "Renderer created successfully");
    return renderer;
}

```

## Code Structure

```
./saci/  
├── deps/  
│   └── **[ALL OF THE DEPENDENCIES]**  
├── include/  
│   ├── saci-core/  
│   │   └── **[ALL OF SACI CORE HEADER FILES]**  
│   ├── saci-utils/  
│   │   └── **[ALL OF SACI UTILITY HEADER FILES]**  
│   ├── saci-core.h -> **We include all of saci-core in a single file**  
│   └── saci-utils.h  -> **We include all of saci-utils in a single file**   
└── src/  
    ├── saci-core/  
    │   └── **[ALL OF SACI CORE SOURCE FILES]**    
    └── saci-utils/  
        └── **[ALL OF SACI UTILITY SOURCE FILES]**  
```

In saci-core:
- `sc-rendering` uses (includes) all of the other files in `saci-core`
- All files in `saci-core` use (include) only the `saci-utils` files, and never
  any other `saci-core` file

### Header Files

Must:
- Be placed in `saci/include` if a part of the library, or `saci/deps` if a dependency.
- Have no function implementation, it should only have declarative code.
- Provide detailed [documentation](#Documentation).

### Source Files

Must:
- Be placed in `saci/src` if a part of the library, or `saci/deps` if a dependency.
- Implement the functions in it's corresponding header file.
- Include minimal and necessary headers.

### Example files

Must:
- Be placed in `examples/`.
- Contain a CMakeLists.txt to build it.

If a example is 2D and defines how the coordinate system works, it should be
placed in: `examples/2d/coodinate-system/`

## Documentation

### Base Guidelines

All documentation blocks should use JavaDOC:

```c
/**
 * It should begin in this line, not the above, and use double `*`.
 */
```

tags should begin with `@` :

```c
/**
 * @brief Begin with @ 
 * \ is bad
 * _ even worse
 */
```

Member documentation should be as follows:
```c
typedef struct sa_Vec2 {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
} sa_Vec2;

// It begins with `/**<` and ends with `*/`
```

### Files

Header files must:
- Begin with a brief description (`@file` and `@brief` tags) outlining its
  purpose and acomplishments.
  - Example: 
  ```c
  /**
   * @file sc-rendering.h
   * @brief This header defines rendering related functions and structs for saciCORE.
   */
  ```
- Every Definition must contain it's documentation.

Source files must:
- Document the helper functions, local variables etc...
- Use little documentation overall, naming and context should be enough.

### Code elements

#### Defines, Global constants and Global Variables

Should contain a `@def` with it's name, and a `@brief` with the description. Example:
```c
/**
 * @def SA_PI
 * @brief Constant for PI (3.141592653589793).
 */
#define SA_PI 3.141592653589793f
```

---

#### Macros and Functions

Should contain a `@brief` with the description. A `@detail` if needed, and all
of the `@param`s and `@return` if needed. Example:

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
sc_Renderer* sc_Renderer_Create(sa_Bool generateDefaults);
```
---

#### Structs and Enums

Should document what it does or is used on and what each member does or is used on:

```c
/**
 * @struct sc_Camera
 * @brief Represents a 3D camera in the saciCORE.
 */
typedef struct sc_Camera {
    sa_Vec3 position; /**< The position of the camera in world space. */
    sa_Vec3 target;   /**< The point the camera is looking at. */
    sa_Vec3 up;       /**< The up vector that defines the camera's orientation. */

    float fov;         /**< The field of view angle (in degrees) for the camera. */
    float aspectRatio; /**< The aspect ratio of the camera (width / height). */
    float near;        /**< The distance to the near clipping plane. */
    float far;         /**< The distance to the far clipping plane. */
} sc_Camera;
```
