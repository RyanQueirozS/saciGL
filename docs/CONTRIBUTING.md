# Contributing to SACI

Thank you for considering contributing to SACI! This guide will help you
understand the coding standards, best practices, and workflow to follow when
making changes to the `saci-core` and `saci-lib` modules.

Take a look at the [conventions](/docs/CONVENTIONS.md)!

## Table of Contents

- [Project Goal](#project-goal)
  - [Philosophy](#philosophy)
    - [What saciGL is](#what-sacigl-is)
    - [What saciGL is NOT](#what-sacigl-is-not)
  - [Goals of saciCore](#goals-of-sacicore)
  - [Goals of saciLib](#goals-of-sacilib)
- [Submitting Contributions](#submitting-contributions)
  - [Branch Naming](#branch-naming)
  - [Commit Messages](#commit-messages)
  - [Workflow](#workflow)


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

3. **Workflow**: The contribution workflow for SACI follows the typical Git
   Flow model to ensure that features, fixes, and improvements are properly
   tested and reviewed before being merged into the main codebase. The process
   should go as follows:
    1. Fork and Clone the Repository
    2. Create a New Branch
    3. Make Changes
    4. Create a Pull Request
    5. Code Review
    6. Merge
    7. Branch Clean Up

