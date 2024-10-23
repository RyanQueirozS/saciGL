# saciGL Conventions

## Style 
### Indentation

Use **4** space indentation, tabs are okay but spaces are easier to jump
around. Using 2 space indentation makes it hard to differenciate blocks of
code.

There should be no reason to have over 3 indentation levels in a single piece
of code, **NO REASON**. That's why 4 space indentation shouldn't be too much on
the screen.

Prefere the use of braces in if statements unless it reduces readability or the
if statement affect only one line. Example:

```c
if (condition) DoSomething();
// Only one line

if (otherCondition) {
    DoThis();
    AndThat();
}
```

#### Avoid else

Elses are powerfull and very tempting to use. As said previouslly using over 3
levels of indentation is a sacrilegious act and `else` helps make that happen.
If possible, always return early, always separate logic into reusable code.
Example:

```c
...
    if(something) {
        doSomething();
    } else if (!something && otherThing){
        if(otherThing == "other thing") { 
            doOtherthing("other thing");
        } else {
            doSomethingDifferent();
        }
    } else if (!otherThing) {
        doRandom();
    }
...

// the above code can be refactored to

...
    if (something) {
        doSomething();
        return;
    }

    if (otherThing) {
        if (otherThing === "other thing") {
            doOtherthing("other thing");
        } else { // We can further remove this else but it might not increase readability
            doSomethingDifferent();
        }
        return;
    }

    doRandom();
    
    // You can perhaps create a otherThing validation function and separate even further.
...
```

**Code readability is a must**

### Line limit

Line limit should be kept at 80, the `.clang-format` file does not break at 80, 
instead breaking at 100 (limit). If the line gets less readable when broken
after 80 chars, feel free to leave it to 100, but **NO line should pass 100
chars**, and **TRY TO KEEP IT AT 80**.

### Braces

Braces must be kept after the declaration of whichever statement block (if,
switch, for, while, do, functions, structs, enums, **ALL of them**). Example:

```c
if (x) {
    // code
}

void function(int x) {
    // body
}

typedef enum SomeEnum {
    // enum values
}

// You get the idea
```

## Best Practices

Always use the implemented types, for integers, no `long long`, `short short`,
use `saci_s8` to `saci_s64` and `saci_u8` to `saci_u64`, that goes for all types.

Avoid the use of **magic values**. Only use them when it has implicit meaning
(returning 0, -1 or NULL for fail).

### Error Handling

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
texture loading, in general use `SACI_LOG_PRINT` at `SACI_LOG_SEVERITY_HIGH` instead of `assert`.

### Log

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
| SACI_LOG_WARN  | What may cause issues, the end user shouldn't need to see this                                                |
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

New contexts are welcomed!

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


### Macros and Defines

Do not recreate macros, and use them when possible. Prefere `SACI_SCAST_TO`
than the default casting, prefere `SACI_ARRLEN` than `sizeof(array) /
sizeof(array[0])` 

Use `#define` only when needed

### Variables

- Do not create unused variables.
- Always initialize variables.
- Use const whenever possible.

When creating a local variable, ask yourself if they are really need to be
local, same with global.

### Constants

When creating a local constant, ask yourself if they are really need to be
local, same with global.

### Enum

Only use enums when: 
- `#define` would be anoing (having to `#define` hundreads of constants)
- "Type clarity" matters (the function recieves MyEnum instead of int)

### Functions

Always:
- Use `const` to parameters that won't be modified.
- Ask yourself if this function needs to be public:
    - Does the end developer really need this?
    - Is this safe to be called everywhere?
- Understand if this function really does what it entails.
    - A function that creates, modifies, initializes and does a lot of
      different things will be hard to refactor.
    - Focus on modularity, keep functions - not small - but decent in scope
    - `FunctionThatDoesFoo()` might do `foo` and validate it, but not resize,
      modify already existing`foos`, etc.

About modularity: If, and only IF a function need to do more than one thing,
separate it into different smaller scopes. Example:
```c
sc_Renderer* sc_Renderer_Create(saci_Bool generateDefaults) {
    sc_Renderer* renderer = (sc_Renderer*)malloc(sizeof(sc_Renderer));
    if (!renderer) {
        SACI_LOG_PRINT(SACI_LOG_LEVEL_ERROR, SACI_LOG_CONTEXT_RENDERER,
                       "Renderer could not be initialized");
        return NULL;
    }
    if (generateDefaults) {
        __sc_renderer_initAll(renderer);
    }
    SACI_LOG_PRINT(SACI_LOG_LEVEL_INFO, SACI_LOG_CONTEXT_RENDERER,
                   "Renderer created successfully");
    return renderer;
}
```
and `__sc_renderer_initAll`:

```c
void __sc_renderer_initAll(sc_Renderer* renderer) {
    // Initializes to remove garbage numbers
    __sc_Renderer_initializeValues(renderer);

    { // Initializes the vertice and texture buffers with default sizes
        __sc_renderBatch_resize(&renderer->renderBatch,
                                SACI_RENDER_BATCH_DEFAULT_CAPACITY);
        assert(renderer->renderBatch.renderCalls);
    }

    // Initializes OpenGL shaders and objects
    __sc_renderer_initGLVertexAttribContext(renderer);
    __sc_renderer_initShaderProgram(renderer);
#if defined(SACI_DEBUG_MODE) || defined(SACI_DEBUG_MODE_RENDERING)
    SACI_LOG_PRINT(SACI_LOG_LEVEL_DEBUG, SACI_LOG_CONTEXT_RENDERER,
                   "Renderer initialized successfully");
#endif
}
```

## Naming


| ELEMENT          | Convention | Example                       |
|------------------|------------|-------------------------------|
| defines          | ALL_CAPS   | `SACI_8BIT_COLOR_MAX`         |
| Macros           | ALL_CAPS   | `SACI_ARRLEN(x)`              |
| Variables        | camelCase  | `int keyState`                |
| Local variables  | camelCase  | `saci_mathPreferences`        |
| Global variables | camelCase  | There are no Global Variables |
| Constants        | camelCase  | `const int screen_width`      |
| Global Constants | ALL_CAPS   | There are no Global Constants |
| Enum             | PascalCase | `saci_LogLevel`               |
| Enum Members     | ALL_CAPS   | `SACI_LOG_LEVEL_DEBUG`        |
| Structs          | PascalCase | `sc_Camera`                   |
| Struct Members   | camelCase  | `saci_Vec3 position`          |
| Functions        | PascalCase | `sc_Renderer_Create`          |
| Functions Params | camelCase  | `saci_Bool generateDefaults`  |

### General naming:

General naming goes as follows:

Whenever defining **GLOBAL** level code, each element should precede a:
- `sc_` if part of saciCore.
- `sl_` if part of saciLib.
- `saci_` if part of saciUtil.

Whenever defining **LOCAL** level code, each element should precede a:
- `__sc_` if part of saciCore.
- `__sl_` if part of saciLib.
- `__saci_` if part of saciUtil.

### Naming guidelines

C has many naming conventions with many different reasons to use each one of
them. Try to keep names the most discriptive as possible without
overcomplicating.  Example:

```c
int randomNumberGeneratedWithRand = rand(); // Bad
int a = rand(); // AWFUL
int random = rand(); // Good


int temporaryValue = 0; // Bad
int tmp = 0; // BAD
int adwadawhdkawj = 0; // AWFUL
int temporary = 0; // Good
```

Hungarian notation can be used, but avoid using it unnecessarily. One example
is `saci_TextureID` which represents a Signed int32, this is good because it
will be used in multiple places and will always represent the same thing. As a rule:
- **DO NOT create types that will be used in few places**.
- DO NOT overcomplicate the workings of a function
    - Example: 
      ```c
      AppleCount getAppleCount(); // You should just return a integer, it's simpler...
      ```
      
### Enum Members

Enum members should always begin with the enum name. Example: 
```c
typedef enum saci_LogLevel {
    SACI_LOG_LEVEL_DEBUG ...
    SACI_LOG_LEVEL_...
    SACI_LOG_LEVEL_...
    SACI_LOG_LEVEL_...
}
```

### Files

Files should be named:
- In lower-case with dashes(`-`) separating each word.
- Be descriptive of what they try to acomplish.

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
typedef struct saci_Vec2 {
    float x; /**< X compoonent */
    float y; /**< Y compoonent */
} saci_Vec2;

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
 * @def SACI_PI
 * @brief Constant for PI (3.141592653589793).
 */
#define SACI_PI 3.141592653589793f
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
sc_Renderer* sc_Renderer_Create(saci_Bool generateDefaults);
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
    saci_Vec3 position; /**< The position of the camera in world space. */
    saci_Vec3 target;   /**< The point the camera is looking at. */
    saci_Vec3 up;       /**< The up vector that defines the camera's orientation. */

    float fov;         /**< The field of view angle (in degrees) for the camera. */
    float aspectRatio; /**< The aspect ratio of the camera (width / height). */
    float near;        /**< The distance to the near clipping plane. */
    float far;         /**< The distance to the far clipping plane. */
} sc_Camera;
```
