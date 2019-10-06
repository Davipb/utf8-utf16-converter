# UTF-16 <-> UTF-8 Converter
This project contains two small functions written in raw C (no C++ features) that can convert in-memory UTF-8 strings to UTF-16 and vice-versa.

It is written in standard C with no OS-specific functions and built & tested with CMake.

The `converter` folder contains a library with the conversion functions themselves.
For more information on how to use the functions, consult the documentation comment above each function in its header (`converter/include/converter.h`).

The `tester` folder contains an executable that can be used to test the conversions,
along with a suite of CTest test cases.
For more information on how to use the tester program, consult the [`README.md` in its folder](./tester/README.md).
