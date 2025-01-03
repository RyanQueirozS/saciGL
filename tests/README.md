# SACI Tests

Feel free to add more!

## Guide

Use the saci standard for the Tests. Without the `sc`, `sl` or `sa` prefix (for
convenience). Global source data should still use `__`.

On `/tests/main.c`

```c
extern void saci_TestFunctionNames(void); // extern the function. 

int main() {
    SACI_TEST_BEGIN();

    {
    saci_TestFunctionNames(); // place it in the main function under the braces.
    
    // saci_TestWindowing();
    // while testing, you can comment functions that don't matter to you. 
    // Remember to uncoment when commiting
    }

    SACI_TEST_END();
}
```

## Building

There are some dependencies, you need to download the `xyzrgb_dragon.ply.gz` on
the [Stanford Edu website](https://graphics.stanford.edu/data/3Dscanrep/), and
drop it in `/tests/assets/`. You Also need to **CONVERT TO OBJ**. There are
some repos that contain the .obj transformed file.

This isn't included because it can really increase the project's size.

```sh
mkdir build && cd build
cmake ..
make
./saci_tests
```

