#include "test_main.h"

using namespace std;

#if defined KEWB_PLATFORM_LINUX
    #define KEWB_PATH_SEP   '/'
#elif defined KEWB_PLATFORM_WINDOWS
    #define KEWB_PATH_SEP   '\\'
#endif


//--------------
//
vector<string>
LoadFileLines(string const& filename)
{
    string          line;
    vector<string>  lines;
    ifstream        in(filename, ios::in | ios::binary);

    lines.reserve(1000);

    if (in)
    {
        while (!in.eof())
        {
            getline(in, line);
            if (line.size() > 0)
            {
                lines.push_back(line);
            }
        }
        in.close();
    }
    return lines;
}

string
LoadFile(string const& filename)
{
    string      line;
    ifstream    in(filename, ios::in | ios::binary);

    if (in)
    {
        in.seekg(0, ios_base::end);
        line.resize((size_t) in.tellg());   //- Cast is there to make 32-bit Windows happy
        in.seekg(0, ios_base::beg);
        in.read(&line[0], line.size());
        in.close();
    }
    return line;
}

string
MakeFilePath(std::string const& dir, std::string const& filename)
{
    string  path;

    if (dir.size() > 0)
    {
        path.assign(dir);

        if (path.back() != KEWB_PATH_SEP)
        {
            path.append(1u, KEWB_PATH_SEP);
        }
    }
    path.append(filename);

    return path;
}

void
MakeFileList(file_list& files)
{
    files.clear();

    files.emplace_back("english_wiki.txt");
    files.emplace_back("chinese_wiki.txt");
    files.emplace_back("hindi_wiki.txt");
    files.emplace_back("japanese_wiki.txt");
    files.emplace_back("korean_wiki.txt");
    files.emplace_back("portuguese_wiki.txt");
    files.emplace_back("russian_wiki.txt");
    files.emplace_back("swedish_wiki.txt");
    files.emplace_back("stress_test_0.txt");
    files.emplace_back("stress_test_1.txt");
    files.emplace_back("stress_test_2.txt");
    files.emplace_back("hindi_wiki_in_english.txt");
    files.emplace_back("hindi_wiki_in_russian.txt");
    files.emplace_back("kermit.txt");
    files.emplace_back("z1_kosme.txt");
    files.emplace_back("z1_ascii.txt");
}

void
PrintHelp()
{
    printf("usage: utf_utils_test [option]... \n");
    printf("  -h              Print help\n");
    printf("  -dd <data_dir>  Specify directory containing test input files\n");
    printf("  -rx <reps>      Specify reps: power-of-two (if < 32) or exact count (if >= 32)\n");
    printf("  -t16            Run UTF-8 to UTF-16 conversion tests\n");
    printf("  -t32            Run UTF-8 to UTF-32 conversion tests\n");
    printf("  -tct            Run big -vs- small lookup table comparison tests\n");
    printf("  -tm             Run miscellaneous conformance tests\n");
}

//--------------
//
int main(int argc, char* argv[])
{
    string      dataDir;
    uint32_t    repShift   = 28;
    bool        testAll    = false;
    bool        testMisc   = false;
    bool        test32     = false;
    bool        test16     = false;
    bool        testTblCmp = false;
    file_list   files;

    for (int i = 1;  i < argc;  ++i)
    {
        string  arg(argv[i]);

        if (arg == "-dd")
        {
            if (++i < argc)
            {
                dataDir = argv[i];
            }
        }
        else if (arg == "-rx")
        {
            if (++i < argc)
            {
                repShift = (uint32_t) std::max(atoi(argv[i]), 0);
            }
        }
        else if (arg == "-tm")
        {
            testMisc = true;
        }
        else if (arg == "-t32")
        {
            test32 = true;
        }
        else if (arg == "-t16")
        {
            test16 = true;
        }
        else if (arg == "-tct")
        {
            testTblCmp = true;
        }
        else if (arg == "-h")
        {
            PrintHelp();
            return 0;
        }
    }

    testAll = !testMisc && !test32 && !test16;

    if (testAll || testMisc)
    {
        TestTrace();
        TestBadSequences();
        TestRoundTripping();
    }

    if (testAll || test32 || test16)
    {
        MakeFileList(files);
    }

    if (testAll || test32)
    {
        TestFiles32(dataDir, repShift, files, testTblCmp);
    }

    if (testAll || test16)
    {
        TestFiles16(dataDir, repShift, files, testTblCmp);
    }
    return 0;
}


//--------------------------------------------------------------------------------------------------
//
#if 0
#include <random>
void
GenerateStressFiles()
{
    using dist_type = std::uniform_int_distribution<char32_t>;

    std::mt19937    rng(time(0));
    dist_type       genA(32, 126);
    dist_type       genB(0x4E00, 0x9FFF);

    size_t const            cpsize = 100000u;
    std::vector<char32_t>   stressIn0(cpsize);
    std::vector<char32_t>   stressIn1(cpsize);
    std::vector<char32_t>   stressIn2(cpsize);

    for (size_t i = 0;  i < cpsize;  ++i)
    {
        stressIn0[i] = genA(rng);
        stressIn1[i] = genB(rng);
        stressIn2[i] = ((i % 2) == 1) ? ' ' : genB(rng);
    }

    std::vector<char8_t>    stressOut0(4*cpsize);
    std::vector<char8_t>    stressOut1(4*cpsize);
    std::vector<char8_t>    stressOut2(4*cpsize);

    char8_t*    pStressOut0 = stressOut0.data();
    char8_t*    pStressOut1 = stressOut1.data();
    char8_t*    pStressOut2 = stressOut2.data();

    for (size_t i = 0;  i < cpsize;  ++i)
    {
        uu::UtfUtils::GetCodeUnits(stressIn0[i], pStressOut0);
        uu::UtfUtils::GetCodeUnits(stressIn1[i], pStressOut1);
        uu::UtfUtils::GetCodeUnits(stressIn2[i], pStressOut2);
    }

    stressOut0.resize((size_t)(pStressOut0 - stressOut0.data()));
    stressOut1.resize((size_t)(pStressOut1 - stressOut1.data()));
    stressOut2.resize((size_t)(pStressOut2 - stressOut2.data()));

    FILE*   fp0 = fopen("stress_test_0.txt", "wb");
    fwrite(stressOut0.data(), sizeof(char8_t), stressOut0.size(), fp0);
    fclose(fp0);

    FILE*   fp1 = fopen("stress_test_1.txt", "wb");
    fwrite(stressOut1.data(), sizeof(char8_t), stressOut1.size(), fp1);
    fclose(fp1);

    FILE*   fp2 = fopen("stress_test_2.txt", "wb");
    fwrite(stressOut2.data(), sizeof(char8_t), stressOut2.size(), fp1);
    fclose(fp2);
}
#endif
