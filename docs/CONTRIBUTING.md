# Contributing to SACI

Thank you for considering contributing to SACI! This guide will help you
understand the coding standards, best practices, and workflow to follow when
making changes to the `saci-core` and `saci-lib` modules.

## Code Structure and Organization

When contributing to SACI, ensure you follow the existing code’s structure and
organizational principles:

1. Header Files (`*.h`):
    - Place in `saci/include/` if part of the library.
    - Place in `saci/deps/` if a dependency to the library.
    - Only declare functions, types, constants, and macros. **NO implementation
      or logic.**
    - Provide detailed function documentation using [Doxygen
      tags](https://www.doxygen.nl/manual/commands.html).

2. Source Files (`*.c`):
    - Place in `saci/src/` if part of the library.
    - Place in `saci/deps/` if a dependency to the library.
    - Implement functions in source files, including minimal and necessary
      headers.
    - Use helper functions to maintain modularity and clarity.
    - Match function definitions with the appropriate source files (e.g.,
      rendering logic in `sc-rendering.c`).

## Coding Standards

### General Guidelines:
1. **Consistency**: Follow the current SACI code style.
    - Code style might evolve, but no major changes are expected soon.
2. **Modularity**: Keep functions focused on a single responsibility. Use
helper functions to simplify large or complex tasks.
    - For example, a function like `sc_CreateRenderer` might perform multiple
      tasks (creation and initialization), but its logic should be subdivided.
      This applies to the entire codebase.
    - Split functions if it improves clarity without overcomplicating logic.
3. **Error Handling**: Address all edge cases and errors, especially for memory
allocation and OpenGL.
    - Document any unhandled errors, such as returning `NULL` values.
4. **Logs**: Use `SACI_LOG_PRINT` for significant actions (e.g., creation,
deletion, configuration changes) and appropriate log levels (INFO, WARN,
ERROR).
    - Use `#if defined(SACI_DEBUG_MODE)` or `#if
      defined(SACI_DEBUG_MODE_{FILE_MAIN_NAME})` to enable debug logs if
      necessary.
    - **DO NOT log unnecessary information**.
    - Avoid using plain `printf` or print-related functions.
5. **Magic Values**: Avoid magic values whenever possible.
    - While discouraged, it’s acceptable in cases like using 3 vertices for a
      triangle (you don't need to define `SACI_TRIANGLE_VERTICES = 3` for
      triangle-related math).
6. **Abstraction**: `saci-core` provides abstraction over OpenGL and other
dependencies. `saci-lib` abstracts over `saci-core`.
    - **DO NOT use OpenGL or other dependencies in `saci-lib`.**

## Naming Conventions:

Base naming convention:
1. Use the `sc_` prefix for public types, functions, and variables in
`saci-core`; use `sl_` for `saci-lib`.
2. Use the `__sc_` prefix for private/helper functions in `saci-core`, and
`__sl_` for `saci-lib`.
3. Use the `SACI_` prefix with uppercase names for constants and macros.

### File Naming

Files must:
- Be in lowercase;
- Have its words divided by `-`;
- Not repeat, if a Header file with `header-file-name.h` exists, there cannot
  be another `header-file-name.h` in another directory;

### Header guard

Header guards must:
- Be in uppercase;
- Begin with `__` and end with `__`;
- Have its words separeted by `_`;
- Contain the whole path from the include/ directory. Example:
  /saci/include/saci-core/sc-camera.h should be defined as
  `__SACI_CORE_SC_CAMERA_H__`;

### Structs

Structs follow the base [Naming Conventions](#Naming Conventions), but must:
- Use PascalCase after the `sc_` or `sl_` prefix;

### Functions

Functions follow the base [Naming Conventions](#Naming Conventions), but must:
- Use PascalCase after the `sc_` or `sl_` prefix;
- If a function recieves no parameters, it should be declared as
  `MyFunction(void)` (with the `void` keyword inside the paramter list)

### Variables

Variables must:
- Contain `saci_`, `sc_` or `sl_` prefix if public;
- Be in cammelCase;
- Be discriptive, **DO NOT use one one letter words**, unless it is in a
  forloop;

### Constants

Global constants must:
- Be in all uppercase;
- Have its words separeteed by `_`;

Private constants follow variable guideline.

### Static variables or constants

Static variables or constants, follow their respective rules, but must:
- Contain a `s` before;

### Enums

Enum naming follows the same style of structures, and enum values follows the
style of constants.

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
3. **OpenGL**: Ensure proper resource management and deletion.
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

