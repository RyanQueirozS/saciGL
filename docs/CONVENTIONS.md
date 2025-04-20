# saciGL Conventions

## tl;dr

If you have already read other coding guidelines, and feel like learning saci
standard guidelines by looking at it's code, just these **10 requirements**
mentioned below should be enough to make your changes fit in the saci project:

1. **Use appropriate prefixes (sc_, sl_, sa_) and case styles**.
   > Maintaining naming rules makes code more predictable when reading.
2. **Avoid elses and nesting**.
   > 3 levels or more of nested statements should probably be better subdivided
   > into separate functions.
3. **Log and assert whenever possible (`sa_Log_Print_m`, `sa_Assert_m`)**.
   > Assertions should be used wisely and only to critical portions of code, and
   > it's better to log pretty much everything then not loggin usefull info.
4. **Use predefined types (sa_s32, sa_u8)**.
   > Keeps code standardized.
5. **Prefere pre-allocation over frequent memory allocations**.
   > Frequent allocations affect memory and can lead to other bigger issues that
   > can be mostly avoided.
6. **DO NOT use `goto`, `setjmp` or `longjmp` constructs.**
   > Code should go have a simple control flow.
7. **Keep info in the smallest possible level os scope**.
   > Maintaining the global space clean makes for a good and safe experience for
   > the developer and the end-user of the library.
8. **Try not to use `typedef` when not needed**.
   > Specifying `struct` and `enum` before a data structure is a strong `C`
   > feature that helps readabilty over the cost of verbosity.
9. **Ensure that abstractions provide a clear reason to exist and aren't just
   over-engineering**.
   > Make sure the saciLib doesn't use any dependencies other than saciCore, and
   > saciCore should provide a simple abstraction layer over the dependencies
   > themselves.
10. **Give users freedom while preventing unintended mistakes. Use preprocessors
    or functions to manage 'hidden' values and ensure safe interactions**.
    > The sc_renderer struct is opaque but can be exposed through a
    > `SC_RENDERER_STRUCT_EXPOSE` define and functions that manage it's
    > properties.

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

3. Rules 1 and 2 **do not apply** in **LOCAL** scopes (e.g., inside functions,
   structs, etc.).

4. Prefixes and suffixes should maintain their defined casing and should not
   affect overall casing, `sc_` will not be capitalized even if in a
   `sc_CONST_VALUE`.

5. Prefere `count` or `lenght` over `size`, and `array` or `list` over plural.
   Example:
   ```c
   // BAD:
   key* key_presses;
   int key_presses_size; // Ambiguous, is it the capacity or the amount of items in key_presses?
   // GOOD:
   key* key_press_array;
   int key_press_array_count; // depending on context could just be key_press_count.
   ```

6. Names should use keywords that go from most important to least, and separate
   **what it is RELATED to (`_Scope_`, `_Event_`, `_Renderer_`)**. Example:

```c
// Bad:
typedef void (*sc_FunctionPtrToHandleMousePos_t)(sc_Window_t* window, double posx, double posy);
// We defined a type for a function ptr, that only tells what it is used for in
// the end `HandleMousePos`

// Good:
typedef void (*sc_Event_MousePosHandler_t)(sc_Window_t* window, double posx, double posy);
// We defined a type for a function ptr, that tells the SCOPE first (_Event_),
// and then we explain what it does in simple terms, it's a `mouse pos handler`.
```

---

### Element-Specific Naming Rules

| CONVENTION             | ELEMENTS                          |
| ---------------------- | --------------------------------- |
| `__` + ALL_CAPS + `__` | Header Guards                     |
| ALL_CAPS               | Defines, Consts                   |
| Pascal_Case_Underlined | Functions, Macros                 |
| camelCase              | Enum, Structs                     |
| snake_case             | Variables, params, struct members |

NOTE:

1. `Header guards` should contain the full path related to `/saci/include/`.
   Example: `/saci/include/my-dir/my-file.h` will be `__MY_DIR_MY_FILE_H__`;
2. Even const params should use `snake_case`;
3. `Static` values should use `_s` suffix independent of being constant or
   variable;
4. `Macros` should have a `_m` suffix.

Examples:

```c
#ifndef __MY_HEADER_GUARD__
#define __MY_HEADER_GUARD__
#endif // __MY_HEADER_GUARD__

const sa_u8 __sc_LINE_COUNT = 10; 

struct __sc_File_Handler {
    char** member_one;
    char* member_two;
}

sa_u8 sc_File_Get_Line_Count(void) {
    int line_offset = 3;
    return sa_File_Get_Ratio_m(__sc_LINE_COUNT - line_offset); // Explanation on why offset should be used
}
...
```

In the above example, just by looking at the code, you can se what everything is
and where it is located project-wise.

- The `__sc` indicates this is a source file of the `saciCore`.
- The `__sc` also indicates that those pieces of code won't be used elsewhere.
- The `sc` in the funtion indicates that function is declared in a header and is
  being implemented in that source file.
- The `_m` suffix after the `sa_File_Get_Ratio_m` indicates it is a macro, and
  the `sa` indicates it is not a part of the `core` or `lib`.

### Naming guidelines

#### Enum Members

Enum members should always begin with the enum name. Example:

```c
enum sa_Log_Level {
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

`if` should contain braces when the code in the `if` statement does more than
one thing. Example:

```c
// BAD: 
// This ifstatement does more than one thing:
if (condition) valueTwo = 2, valueThree = 3;

// GOOOD: 
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

**DO NOT** nest whenever possible. Simple as that.

### Line limit

**Lines should be kept at 80 CHARS** whenever possible. The `.clang-format` file
does not break lines, because sometimes it forces some ugly indentations.
Prefere a **READABLE over a 80 CHAR LINE**, if you need to break at 60, 90, or a
100, do it. **DO NOT GO OVER 110** and whenever possible **BREAK AT 80 CHARS**.

### Whitespace

#### Empty lines

There should **NOT** be a empty line per function in the header files. Example:

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

As seen above. Use braces after functions, ifs, elses, switch and cases, structs
and all of the other things people debate about in regards to braces.

## Best Practices

1. **USE THE ALREADY IMPLEMENTED TYPES**, for integers, no `long long`,
   `short short`, use `sa_s8` to `sa_s64` and `sa_u8` to `sa_u64`, that goes for
   all types;
2. **AVOID MAGIC VALUES**. Only use them when it has implicit meaning (returning
   0, -1 or NULL for fail);
3. **RETURN EARLY**;
4. **CHECK/HANDLE MEMORY ALLOCATION**;
5. Prefere **PRE-ALLOCATING MEMORY**;
6. **NO DEPENDENCIES on SACI LIB**. SaciCORE should provide enough abstraction
   layers;
7. **LOG ALL POSSIBLE ERRORS**;
8. **ASSERT ONLY WHEN NEEDED**. Only for critical operations, like renderer
   creation/library initialization;
9. **DO NOT** use **`goto`**, **`setjmp`** or **`longjmp`** constructs.

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
/* === Event === */
// Event related code


/* === Renderering  === */
// Rendering related code
```

### Assertion

Prefer to use the `sa_Assert_m` whenever critical operations need to be checked.
It is a like the `sa_Log_Print_m` (discused below) but it is specific to
critical, high priority logging information.

### Logging

Use `sa_Log_Print_m` for significant actions (e.g., creation, deletion,
configuration changes) and appropriate log levels (INFO, WARN, ERROR).

- **EVERY error should be logged.**
- Use `#if defined(SA_DEBUG_MODE)` or
  `#if defined(SA_DEBUG_MODE_{FILE_MAIN_NAME})` to enable debug logs if
  necessary.
- **DO NOT log unnecessary information**.
- Avoid using plain `printf` or print-related functions.

Logging **Type**:

| Type         | Use                                                                                                          |
| ------------ | ------------------------------------------------------------------------------------------------------------ |
| sa_LOG_DEBUG | Information that helps developers understand internal workings or trace execution for debugging purposes.    |
| sa_LOG_INFO  | Information useful to the end-user regarding application state or normal operations (e.g. resource loading). |
| sa_LOG_WARN  | What may cause issues, the end user shouldn't need to see this unless they screw up.                         |
| sa_LOG_ERROR | Failures, errors or unexpected values.                                                                       |

Logging **Level**:

| Level                        | Use                                                             |
| ---------------------------- | --------------------------------------------------------------- |
| sa_LOG_SEVERITY_NOTIFICATION | Only informational messages, no action needed.                  |
| sa_LOG_SEVERITY_LOW          | Minor issues, such as deprecation or minimal performance hints. |
| sa_LOG_SEVERITY_MEDIUM       | Issues that could cause bugs or notable performance problems.   |
| sa_LOG_SEVERITY_HIGH         | Critical errors. **SHOULD CRASH THE APP**                       |

### Abstraction

The purpose of `saci-core` is to provide a layer of abstraction over
dependencies like OpenGL such that `saci-lib` can remain "changeless" over time.
This abstraction ensures that the underlying functionality can evolve without
requiring changes to the higher-level code. By isolating the details of the
dependencies, we ensure that our codebase remains flexible and easy to maintain,
even if the underlying libraries (e.g., OpenGL) change their API or behavior
over time.

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

Do not recreate macros. Prefere `sa_Scast_To` than the default casting, prefere
`sa_Arrlen` than `sizeof(array) / sizeof(array[0])`.

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
  \
  OR
- is a type that adds context for a given functionality;

Example:

```c
typedef int32_t sa_s32; // OK; reausable, common, usefull.

typedef sa_s32 sa_TextureID // GOOD; adds context.

typedef struct {
    float value1, value2;
    const char* STRING;
} sc_My_Internal_Struct; // BAD. Will rarelly be used by the user.
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
- Prefere functions up to 60 lines of code.
- Understand if this function really does what it entails.
  - A function that creates, modifies, initializes and does a lot of different
    things will be hard to refactor.
  - Focus on modularity, keep functions - not small - but decent in scope
  - `Function_That_Does_Foo()` might do `foo` and validate it, but not resize,
    modify already existing `foos`, etc.

### Endifs

Endifs should containt the condition they are ending in a comment. Example::

```c
#ifndef __MY_HEADER_GUARD__
#define __MY_HEADER_GUARD__
#endif // __MY_HEADER_GUARD__
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

### Header Files

Must:

- Be placed in `saci/include` if a part of the library, or `saci/deps` if a
  dependency.
- Have no function implementation, it should only have declarative code.
- Provide detailed [documentation](#Documentation).

### Source Files

Must:

- Be placed in `saci/src` if a part of the library, or `saci/deps` if a
  dependency.
- Implement the functions in it's corresponding header file.
- Include minimal and necessary headers.

### Example files

Must:

- Be placed in `examples/`.
- Be a `main.c`

If a example is 2D and defines how the coordinate system works, it should be
placed in: `examples/2d/coodinate-system/`

## Documentation

### Base Guidelines

Documentation blocks should be simple and discriptive, try to fit a one-line
that explains description for each in their header declaration. Use this doxygen
template:

```c
/**
 * @brief <FUNTION-NAME> <DESCRIPTION>
 *
 * @param[in/ou/inout] <PARAM-NAME> <DESCRIPTION>
 * @param[in/ou/inout] ... use as many params as needed
 *
 * @return <TYPE> <DESCRIPTION>
 * @retval <VALUE> <DESCRIPTION>
 */
```

Use return when there are multiple to infinite possiblities and retval when
there are little. Think of it like memcmp where there are only 3 possible
results (use retval), where as a sqrt function can return almost an infinite set
of numbers (use return).

Example:

NOTE: OLD CODE:

```c
// GOOD:
/**
 * @brief sc_Renderer_Create_Empty Creates an empty sc_renderer
 */
sc_Renderer_t* sc_Renderer_Create_Empty(void);

// Unecessary information
/**
 * @brief sc_Renderer_Create_Empty Creates a default sc_renderer. It already comes with a shader program, ibos, ubos...
 */
sc_Renderer_t* sc_Renderer_Create_Default(void); // Creates a renderer with setup fields.
```

Each member should be documented as follows:

```c
typedef struct sa_vec2 {
    float x; // X compoonent
    float y; // Y compoonent
} sa_vec2;
```

### Files

Header files must:

- A simple documentation describing what the file acomplishes
- Every Declaration must contain a simple documentation.

Source files must:

- Use little documentation overall, naming and context should be enough. But can
  use a simple `//` documentation block above functions or any other declaration
