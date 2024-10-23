# Contributing to SACI

Thank you for considering contributing to SACI! This guide will help you
understand the coding standards, best practices, and workflow to follow when
making changes to the `saci-core` and `saci-lib` modules.

Take a look at the [conventions](/docs/CONVENTIONS)!

## Project Goal

SACI aims to implement a simple, highly customizable, OpenGL-based graphics
library (saciGL) that works on Linux, Windows, MacOS, and Web.

This library will serve as the foundation for saciEngine, a lightweight but
powerful 3D game engine that is under development.

### Philosophy 

What saciGL is:

- Free and open-source.
- Well-documented.
- Simple to use.
- Desgined for developers of all levels
- Highly customizible and extensible, adaptable even for complex projects.
- Developer first.
- Platform independence

What saciGL is **NOT**:

- A fully-fledged game engine.
- A complete game library - it's customizable for this specific reason.
- A math library.

### Goals of `saciCore`

`saciCore` aims to deliver a fully functioning graphics library that is:

- Memory-safe at its lowest levels, especially in external dependencies.
- Highly extensible.
- Low in abstraction for core features—designed for control rather than ease of
  use.

### Goals of `saciLib`

`saciLib` aims to provide a higher-level abstraction over `saciCore`:
- Memory safe.
- Highly extensible through `saciCore`.
- High on abstraction for ease of use.

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

