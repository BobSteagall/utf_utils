#ifndef TEST_MAIN_H_DEFINED
#define TEST_MAIN_H_DEFINED

#include <algorithm>
#include <array>
#include <chrono>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include <tuple>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iconv.h>

#ifdef _MSC_VER
    #include <Windows.h>
    #undef  max
    #undef  min
    #define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
#endif

#include "av_utf8.h"
#include "boost_utf8.hpp"
#include "hoehrmann.h"
#include "llvm_convert_utf.h"
#include "utf_utils.h"

using uchar      = unsigned char;
using char8_t    = uu::UtfUtils::char8_t;
using file_list  = std::vector<std::string>;
using name_list  = std::vector<std::string>;
using time_list  = std::vector<std::int64_t>;
using time_table = std::vector<time_list>;

name_list       LoadFileLines(std::string const& filename);
std::string     LoadFile(std::string const& filename);
std::string     MakeFilePath(std::string const& dir, std::string const& filename);
void            MakeFileList(file_list& files);

void    TestTrace();
void    TestBadSequences();
void    TestRoundTripping();
void    TestFiles16(std::string const& dataDir, size_t repShift, file_list const& files, bool tblCmp);
void    TestFiles32(std::string const& dataDir, size_t repShift, file_list const& files, bool tblCmp);

#endif  //- TEST_MAIN_H_DEFINED
