# SACI Tests

Feel free to add more!

## Guide

Use the following "format" for tests:
```c
static void __MySetupFunction(paramType param); // here is a private local function
                                                // use this for helpers.
static void __MyTerminateSetupFunction(void)

static void TestSomeFunctioName(void);      // test function declaration.
static void TestSomeFunctioOthername(void); // another test function declaration.

void saci_TestFunctionNames(void) { // declare the function that calls all test 
    TestSomeFunctioName();          // functions in the file
    TestSomeFunctioOthername();
}

static void TestSomeFunctioName(void) { // The name of the function is 'Test+NameOfFunctionToTest'
    type param = ""; // initialize variables that will be used over the test.
    {
        SACI_TEST_DESC("Should return true when recieving foo"); // always set description.
        param = "foo"; // setup variables that will be used.
        SACI_TEST_ASSERT(SomeFunctionName(param)); // test condition (boolean).
    }
    {
        SACI_TEST_DESC("Should return false when recieving bar");
        param = "bar";
        SACI_TEST_ASSERT(SomeFunctionName(param) == false); // could use (!SomeFunctionName(param))
    }
}  

static void TestSomeFunctioOthername(void){
    {
        SACI_TEST_DESC("Should work if setup"// describe when it fails as well
        paramType param = "value";
        __MySetupFunction(param);
        SACI_TEST_ASSERT(SomeFunctionOtherName());
        __MyTerminateSetupFunction() // tests should initialize and reset it's global values.
    }
    {
        SACI_TEST_DESC("Should not work if given is not ..."); // describe when it fails as well
        SACI_TEST_ASSERT(SomeFunctionOtherName());
    }
}

 ```
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

```sh
mkdir build && cd build
cmake ..
make
./saci_tests
```
