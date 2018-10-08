cmake_minimum_required (VERSION 3.5)
project (utf_utils)

include_directories(src)

set(Sources
    src/utf_utils.cpp
    src/utf_utils.h

    test/av_utf8.c
    test/av_utf8.h
	test/boost_utf8.hpp
	test/boost_utf8_config.hpp
    test/hoehrmann.cpp
    test/hoehrmann.h
    test/llvm_convert_utf.c
    test/llvm_convert_utf.h
    test/test_basics.cpp
    test/test_conversions_16.cpp
    test/test_conversions_32.cpp
    test/test_main.cpp
    test/test_main.h
)

include_directories(test)
add_executable(utf_utils_test ${Sources})

set(CMAKE_VERBOSE_MAKEFILE 1)

if(CXX_COMPILER STREQUAL clang++)
    set(CMAKE_C_COMPILER   clang)
    set(CMAKE_CXX_COMPILER clang++)
else()
    set(CMAKE_C_COMPILER   gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()

if(CMAKE_CXX_COMPILER STREQUAL clang++)

    set(CMAKE_C_FLAGS_DEBUG   "-g -Wall -pedantic -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++14 -Wall -pedantic -Wextra -Wno-unused-parameter")

    set(CMAKE_C_FLAGS_RELEASE   "-O3 -march=westmere -Wall -pedantic -Wextra")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=westmere -std=c++14 -stdlib=libc++ -Wall -pedantic -Wextra -Wno-unused-parameter")

elseif(CMAKE_CXX_COMPILER STREQUAL g++)

    set(CMAKE_C_FLAGS_DEBUG   "-g -Wall -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++14 -Wall -pedantic -Wextra")

    set(CMAKE_C_FLAGS_RELEASE   "-O3 -march=westmere -Wall -pedantic")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=westmere -std=c++14 -Wall -pedantic -Wextra")

endif()
