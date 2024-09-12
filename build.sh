#!/bin/bash

# Base directory for examples
EXAMPLES_DIR="examples"

# Function to build all examples with CMake
build_example() {
    EXAMPLE_PATH=$1
    echo ""
    echo "Building example in $EXAMPLE_PATH"
    echo ""

    # Navigate to the example directory
    cd $EXAMPLE_PATH

    # Create a build directory if it doesn't exist
    mkdir -p build
    cd build

    # Run CMake to configure the project
    cmake ..

    # Build the project using CMake
    cmake --build .

    # Optionally run the executable if the build succeeds
    if [ $? -eq 0 ]; then
        echo ""
        echo "Build succeeded for $EXAMPLE_PATH."
        echo ""
    else
        echo ""
        echo "Build failed for $EXAMPLE_PATH."
        echo ""
    fi

    # Navigate back to the original directory
    cd ../../..
}

# Loop through all subdirectories inside examples/
find $EXAMPLES_DIR -name CMakeLists.txt | while read cmake_file; do
    # Get the directory containing the CMakeLists.txt
    example_dir=$(dirname $cmake_file)
    
    # Call the function to build this example
    build_example $example_dir
done
