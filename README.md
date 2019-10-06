# UTF-16 <-> UTF-8 Converter
This project contains two small functions written in raw C (no C++ features) that can convert in-memory UTF-8 strings to UTF-16 and vice-versa.

It is written in standard C with no OS-specific functions and built & tested with CMake.

The `converter` folder contains a library with the conversion functions themselves.
For more information on how to use the functions, consult the documentation comment above each function in its header (`converter/include/converter.h`).

The `tester` folder contains an executable that can be used to test the conversions,
along with a suite of CTest test cases.
For more information on how to use the tester program, consult the [`README.md` in its folder](./tester/README.md).

## Building
First, install CMake version 3.10 or higher and any required build tools for your platform (Visual Studio or Cygwin on Windows, gcc or clang on Linux, etc).

Then, just run (`sh` or `cmd`, one command per line):

**Unix-like (gcc/clang/Cygwin)**
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
ctest
```

**Visual Studio**
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
ctest -C Release
```

This will build the project in a `build` directory, then run all tests with CTest. The `cmake --build` comamnd should tell you where to find the compiled libraries and executables.

For more information on how to customize the build process, check out [CMake's documentation](https://cmake.org/cmake/help/latest/). 
This is a very simple project with no "magic" in the build process,
so you shouldn't have trouble changing it to suit your needs.

Alternatively, you can just copy `converter.h` and `converter.c` into your project.
The conversion functions are self-contained and use standard C functions and syntax.

