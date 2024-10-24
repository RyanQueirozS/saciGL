# Saci

SaciGL (pronounced sa-si) is a OpenGL based Graphics Library that simplifies
the base renderin process by providing low-level abstractions and memory safety

## Table of Contents

- [Installing](#installing)
  - [Building](#building)
    - [Linux](#linux)
  - [Binaries](#binaries)
    - [Linux](#linux-1)
- [Contributing](#contributing)
- [Documentation](#documentation)
- [Warnings](#warnings)
- [License](#license)

## Installing

### Building

The building process has only been tested on Linux, feel free to try on other
OS and perhaps create an issue!

#### Linus

```sh
git clone https://github.com/RyanQueirozS/saciGL
cd saciGL/saci
mkdir build && cd build
cmake ..
```

The static libraries will be built on `/saciGL/saci/build/lib/`

To install: **([NOT RECOMENDED](#Warnings))**

```sh
make install
```

### Binaries

#### Linux

Take a look at **[releases](https://github.com/RyanQueirozS/saciGL/releases)**.

## Contributing

Take a look at [contributing](/docs/CONTRIBUTING.md)!

## Documentation

SaciGL doesn't currently have a documentation, although the source code is well
documented and the [examples](/examples/) provide simple and more complex
examples on how to use the library.

## Warnings

This library is still under development install it at your own risk

## License

[GNU General Public License version 3 (GPL v3)](/docs/LICENSE).
