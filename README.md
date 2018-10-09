## Overview

This is the repo for my work on high-speed conversion/transcoding of UTF-8 to UTF-32/UTF-16.  The interesting bits are currently implemented in the `UtfUtils` class defined in the files `src/utf_utils.(h|cpp)`.  A small test suite is included in the repo, and instructions are provided below for building and running on Linux and Windows.

I fully expect that this work will evolve and grow over time as new ways to exploit SSE (and AVX) are explored.  I'll try to tag and branch things in a way that makes sense.

## About `UtfUtils`

`UtfUtils` is a traits-style class intended to demonstrate DFA-based techniques for converting strings of UTF-8 code units to strings of UTF-32 code points, as well as transcoding UTF-8 into strings of UTF-16 code units.  Its current focus is on converting _from_ UTF-8 in a highly performant way, although it does include utility member functions for converting a UTF-32 code point into sequences of UTF-8/UTF-16 code units.

It implements conversion from UTF-8 in three different, but related ways:

1. Using a purely DFA-based approach to recognizing valid sequences of UTF-8 code units;
1. Using a DFA-based approach with a short-circuit optimization for ASCII code units; and, 
1. Using a DFA-based approach with an SSE-based optimization for contiguous runs of ASCII code units.

The member functions implement STL-style argument ordering, with source arguments on the left and destination arguments on the right that define the input and output ranges, respectively.  The range-to-range conversion member functions are analogous to `std::copy()` or `std::transform()` in that the first two arguments define the input range and the third argument defines the starting point of the output range.

This class is not quite ready for production usage, as it currently provides only a trivial and not-very-useful mechanism for reporting errors.  This will improve over time.  Also, no checking is done for null pointer arguments; it is assumed that the input and output pointers sensibly point to buffers that already exist, and that the destination buffer is appropriately sized.

## Building and Testing

Solution and project files are provided for building on Windows using Microsoft Visual Studio, as well as on Linux using CMake with Clang and/or GCC.  Of course, you'll need to first use your favorite method for cloning the repo into the work directory of choice.

### On Linux

CMake 3.5 or higher is required to build on Linux.  I'm pretty sure that 3.5 is not absolutely required -- it just happens to be the oldest version I have installed on my various workstations.  If your version of CMake is older, you can always try changing the required version in the `CMakeLists.txt` file and see if it works for you.

As you might expect, to build with GCC, `gcc` and `g++` must be in your executable path.  Likewise, in order to build with Clang, `clang` and `clang++` must be in your executable path.

To build and run: clone the repo, run the provided handy-dandy CMake setup script, run `make`, and then try running the test program.

```
    $ cd <some_work_dir>
    $ git clone https://github.com/BobSteagall/utf_utils.git
    $ cd utf_utils
    $ ./setup_cmake_builds.sh
    $
    $ cd build-release-gcc
    $ make
    $ ./utf_utils_test -h
    $ ./utf_utils_test -dd ../test_data
    $
    $ cd ../build-release-clang
    $ make
    $ ./utf_utils_test -dd ../test_data
```

If you find yourself needing to re-run CMake from one of the build directories, use the `run_cmake.sh` script:

```
    $ cd ../build-release-gcc
    $ rm -rf ./*
    $ ../run_cmake.sh
    $ make
```

### On Windows

I've been building and testing with Visual Studio 2017 on both Windows 8.1 and Windows 10;  the Visual Studio 2015 edition should also work on those two platforms.  I have not tried any other combination of platform and compiler.

To build: clone the repo, open the `UtfUtils.sln` solution file in the project root directory, select the Release/x64 build configuration, and build the UtfUtils project.

```
    C:\blah_dir> cd <some_work_dir>
    C:\some_work_dir> git clone https://github.com/BobSteagall/utf_utils.git
    C:\some_word_dir> cd utf_utils
    [... build with VS ...]
```

To run: open a Command Prompt window, PowerShell window, MinGW shell window, or Cygwin shell window and change directory to the project root.  From there, you can execute the test program with something like this (with Command Prompt):

```
    C:\some_work_dir\utf_utils> build-win\x64\Release\utf_utils_test.exe -dd test_data
```

NB: The repo contains a pre-built version of [libiconv](https://www.gnu.org/software/libiconv/) built with Visual Studio 2015 against the Windows 8.1 SDK.  It includes static libraries for Debug/Release configurations on x86/x64.  The test program uses these libraries as part of its benchmarking.  I've built and tested the test program with all four library configurations with VS2015/2017 on Windows 8.1/10.

## Future Directions

To make this work truly useful for real-world applications, the following functionality needs to be added (at a minimum):
1. Provide meaningful error reporting, including a way of describing the type of error and where in the input/output range it occurred;
1. Provide various common error recovery strategies and a means of choosing them at runtime;
1. Provide member functions for measuring the number of resulting UTF-32 code points or UTF-16 code units without actually doing the conversion (i.e., analogous to `strlen()`);
1. Provide four-argument versions of the conversion functions to fully specify the output range so as to permit error checking for out-of-bounds writes to the output buffer;
1. Provide parametrized member function overloads that can work with STL iterators, and "do the right thing" based on the input and output iterator categories;
1. Provide conversions to both little-endian (LE) and big-endian (BE) representations;
1. Provide a tasteful set of wrapper overloads to remove some of the boilerplate drudgery needed to use the interface as it currently stands.

These items are all on my to-do list to be accomplished in the near future. 

Please let me know if you find any errors or problems, and as always, feedback and suggestions are greatly appreciated.

--Bob

P.S. For the site where this and my other C++ work will be discussed, see [The State Machine](https://bobsteagall.com).