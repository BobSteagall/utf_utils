#include "test_main.h"

using namespace std;
using namespace uu;

using TestFn32 = ptrdiff_t (*)(string const&, size_t, u32string&);

//--------------
//
ptrdiff_t
Convert32_Iconv(string const& src, size_t reps, u32string& dst)
{
    static  bool    init = false;
    static  iconv_t jdsc = (iconv_t)(-1);

    if (!init)
    {
        jdsc = iconv_open("UTF-32LE", "UTF-8");

        if (jdsc == (iconv_t)(-1))
        {
            printf("iconv_open() error\n");
            exit(-1);
        }

        init = true;
    }

    size_t      srcLen;     //- Number of code units in source string
    size_t      dstLen;     //- Number of code points in destination string
    char*       pSrcBuf;    //- Pointer to source buffer
    char*       pDstBuf;    //- Pointer to destination buffer

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        srcLen  = src.size();
        dstLen  = srcLen*sizeof(char32_t);
        pSrcBuf = (char*) &src[0];
        pDstBuf = (char*) &dst[0];
        iconv(jdsc, &pSrcBuf, &srcLen, &pDstBuf, &dstLen);
    }
    dstLen = (char32_t*) pDstBuf - &dst[0];

    return dstLen;
}

//--------------
//
ptrdiff_t
Convert32_Llvm(string const& src, size_t reps, u32string& dst)
{
    size_t          srcLen = src.size();    //- Number of code units in source string
    size_t          dstLen = dst.size();    //- Number of code points in destination string
    uchar const*    pSrcBuf;                //- Pointer to source buffer
    UTF32*          pDstBuf;                //- Pointer to destination buffer

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        pDstBuf = (UTF32*) &dst[0];
        pSrcBuf = (uchar const*) &src[0];
        ConvertUTF8toUTF32(&pSrcBuf, pSrcBuf+srcLen, &pDstBuf, pDstBuf+dstLen, strictConversion);
    }
    dstLen = pDstBuf - (UTF32*) &dst[0];

    return dstLen;
}

//--------------
//
ptrdiff_t
Convert32_Av(string const& src, size_t reps, u32string& dst)
{
    ptrdiff_t   tmpLen  = 0;
    size_t      srcLen  = src.size();           //- Number of code units in source string
    size_t      dstLen  = dst.size();           //- Number of code points in destination string
    char const* pSrcBuf = (char const*) &src[0];//- Pointer to source buffer
    int32_t*    pDstBuf = (int32_t*) &dst[0];   //- Pointer to destination buffer

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        tmpLen = (ptrdiff_t) utf8_to_wchar(pSrcBuf, (int) srcLen, pDstBuf, (int) dstLen, 0);
    }

    return tmpLen;
}

//--------------
//
ptrdiff_t
Convert32_Codecvt(string const& src, size_t reps, u32string& dst)
{
    using Utf8to32 = wstring_convert<std::codecvt_utf8<char32_t>, char32_t>;

    Utf8to32    utf32conv;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dst = utf32conv.from_bytes(src);
    }
    
    return (ptrdiff_t) dst.size();
}

#ifdef KEWB_COMPILER_MSVC
    __PURE_APPDOMAIN_GLOBAL std::locale::id     std::codecvt<char32_t, char, struct _Mbstatet>::id;
#endif

//--------------
//
std::ptrdiff_t
BoostTextConvert(char8_t const* pSrc, char8_t const* pSrcEnd, char32_t* pDst) noexcept
{
    char32_t *pDstOrig = pDst;

    auto first = boost::text::utf8::to_utf32_iterator<char8_t const*>(pSrc, pSrc, pSrcEnd);
    auto last =  boost::text::utf8::to_utf32_iterator<char8_t const*>(pSrc, pSrcEnd, pSrcEnd);

    pDst = std::copy(first, last, pDst);

    return pDst - pDstOrig;
}

ptrdiff_t
Convert32_BoostText(string const& src, size_t reps, u32string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char32_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
    ptrdiff_t       dstLen  = 0;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dstLen = BoostTextConvert(pSrcBuf, pSrcEnd, pDstBuf);
    }

    return dstLen;
}

//--------------------------------------------------------------------------------------------------
//
ptrdiff_t
Convert32_Hoehrmann(string const& src, size_t reps, u32string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    size_t          srcLen  = src.size();               //- Number of code units
    char32_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
    ptrdiff_t       dstLen  = 0;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dstLen = toUtf32(pSrcBuf, srcLen, pDstBuf);
    }

    return dstLen;
}

//--------------------------------------------------------------------------------------------------
//
ptrdiff_t
Convert32_KewbBasicSmTab(string const& src, size_t reps, u32string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char32_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
    ptrdiff_t       dstLen  = 0;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dstLen = UtfUtils::BasicSmallTableConvert(pSrcBuf, pSrcEnd, pDstBuf);
    }

    return dstLen;
}

//--------------
//
ptrdiff_t
Convert32_KewbFastSmTab(string const& src, size_t reps, u32string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char32_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
    ptrdiff_t       dstLen  = 0;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dstLen = UtfUtils::FastSmallTableConvert(pSrcBuf, pSrcEnd, pDstBuf);
    }

    return dstLen;
}

//--------------
//
ptrdiff_t
Convert32_KewbSseSmTab(string const& src, size_t reps, u32string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char32_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
    ptrdiff_t       dstLen  = 0;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dstLen = UtfUtils::SseSmallTableConvert(pSrcBuf, pSrcEnd, pDstBuf);
    }

    return dstLen;
}

//--------------------------------------------------------------------------------------------------
//
ptrdiff_t
Convert32_KewbBasicBgTab(string const& src, size_t reps, u32string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char32_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
    ptrdiff_t       dstLen  = 0;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dstLen = UtfUtils::BasicBigTableConvert(pSrcBuf, pSrcEnd, pDstBuf);
    }

    return dstLen;
}

//--------------
//
ptrdiff_t
Convert32_KewbFastBgTab(string const& src, size_t reps, u32string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char32_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
    ptrdiff_t       dstLen  = 0;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dstLen = UtfUtils::FastBigTableConvert(pSrcBuf, pSrcEnd, pDstBuf);
    }

    return dstLen;
}

//--------------
//
ptrdiff_t
Convert32_KewbSseBgTab(string const& src, size_t reps, u32string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char32_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
    ptrdiff_t       dstLen  = 0;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dstLen = UtfUtils::SseBigTableConvert(pSrcBuf, pSrcEnd, pDstBuf);
    }

    return dstLen;
}

//--------------------------------------------------------------------------------------------------
//
ptrdiff_t
Convert32_KewbBasic(string const& src, size_t reps, u32string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char32_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
    ptrdiff_t       dstLen  = 0;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dstLen = UtfUtils::BasicConvert(pSrcBuf, pSrcEnd, pDstBuf);
    }

    return dstLen;
}

//--------------
//
ptrdiff_t
Convert32_KewbFast(string const& src, size_t reps, u32string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char32_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
    ptrdiff_t       dstLen  = 0;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dstLen = UtfUtils::FastConvert(pSrcBuf, pSrcEnd, pDstBuf);
    }

    return dstLen;
}

//--------------
//
ptrdiff_t
Convert32_KewbSse(string const& src, size_t reps, u32string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char32_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
    ptrdiff_t       dstLen  = 0;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dstLen = UtfUtils::SseConvert(pSrcBuf, pSrcEnd, pDstBuf);
    }

    return dstLen;
}

//--------------------------------------------------------------------------------------------------
//
int64_t
TestOneConversion32
(TestFn32 fn, string const& src, size_t reps, u32string const& answer, char const* name)
{
    using tm_pt = chrono::high_resolution_clock::time_point;

    tm_pt       start, finish;
    int64_t     tmdiff;
    ptrdiff_t   dstLen;
    u32string   dst(src.size(), 0u);

    start  = chrono::high_resolution_clock::now();
    dstLen = fn(src, reps, dst);
    finish = chrono::high_resolution_clock::now();
    tmdiff = chrono::duration_cast<chrono::milliseconds>(finish - start).count();

    dst.resize((dstLen >= 0) ? (size_t) dstLen : 0u);

    printf("UTF-8 to UTF-32 took %4u msec (%zu/%zu units/points) (%zu reps) (%s)\n",
            (uint32_t) tmdiff, src.size(), dst.size(), reps, ((name != nullptr) ? name : ""));

    if (dst != answer)
    {
        printf("error: result for %s differs from iconv()\n", name);
    }
    fflush(stdout);
    return tmdiff;
}

//--------------
//
tuple<name_list, time_list>
TestAllConversions32(string const& fname, bool isFile, size_t repShift, bool tblCmp)
{
    size_t      reps;
    string      u8src;
    u32string   u32answer;
    int64_t     tdiff;
    name_list   algos;
    time_list   times;

    //- Load the source text.
    //
    u8src = (isFile) ? LoadFile(fname) : fname;

    if (u8src.size() == 0)
    {
        if (isFile)
        {
            printf("file '%s' is non-existent or empty\n", fname.c_str());
        }
        return tuple<name_list, time_list>(algos, times);
    }

    //- Figure out the number of reps to perform.  If repShift is less than 32, then the reps
    //  are computed such that the total amount of input text that is processed is approximately
    //  equal to 2^^repShift.  For example, if repShift is 28, then the reps will be such that
    //  ~256MB of input text is processed.
    //
    //  OTOH, if repShift is bigger than 32, then repShift becomes the number of reps.  This is
    //  useful if you want to run a certain number of reps regardless of the total amount of
    //  input text.
    //
    reps = (repShift < 32) ? ((1ull << repShift) / u8src.size()) : repShift;

    //- Prepare the gold standard conversion answer (from iconv).
    //
    u32answer.resize(u8src.size(), 0);
    u32answer.resize((size_t) Convert32_Iconv(u8src, 1, u32answer));

    //- Run the individual tests.
    //
    tdiff = TestOneConversion32(&Convert32_Iconv, u8src, reps, u32answer, "iconv");
    times.push_back(tdiff);
    algos.emplace_back("iconv");

    tdiff = TestOneConversion32(&Convert32_Llvm, u8src, reps, u32answer, "llvm");
    times.push_back(tdiff);
    algos.emplace_back("llvm");

    tdiff = TestOneConversion32(&Convert32_Av, u8src, reps, u32answer, "av");
    times.push_back(tdiff);
    algos.emplace_back("av");

    tdiff = TestOneConversion32(&Convert32_Codecvt, u8src, reps, u32answer, "std::codecvt");
    times.push_back(tdiff);
    algos.emplace_back("std::codecvt");

    tdiff = TestOneConversion32(&Convert32_BoostText, u8src, reps, u32answer, "Boost.Text");
    times.push_back(tdiff);
    algos.emplace_back("Boost.Text");

    tdiff = TestOneConversion32(&Convert32_Hoehrmann, u8src, reps, u32answer, "hoehrmann");
    times.push_back(tdiff);
    algos.emplace_back("Hoehrmann");

    if (tblCmp)
    {
        tdiff = TestOneConversion32(&Convert32_KewbBasicSmTab, u8src, reps, u32answer, "kewb-basic-small-table");
        times.push_back(tdiff);
        algos.emplace_back("kewb-basic-small-table");

        tdiff = TestOneConversion32(&Convert32_KewbBasicBgTab, u8src, reps, u32answer, "kewb-basic-big-table");
        times.push_back(tdiff);
        algos.emplace_back("kewb-basic-big-table");

        tdiff = TestOneConversion32(&Convert32_KewbFastSmTab, u8src, reps, u32answer, "kewb-fast-small-table");
        times.push_back(tdiff);
        algos.emplace_back("kewb-fast-small-table");

        tdiff = TestOneConversion32(&Convert32_KewbFastBgTab, u8src, reps, u32answer, "kewb-fast-big-table");
        times.push_back(tdiff);
        algos.emplace_back("kewb-fast-big-table");

        tdiff = TestOneConversion32(&Convert32_KewbSseSmTab, u8src, reps, u32answer, "kewb-sse-small-table");
        times.push_back(tdiff);
        algos.emplace_back("kewb-sse-small-table");

        tdiff = TestOneConversion32(&Convert32_KewbSseBgTab, u8src, reps, u32answer, "kewb-sse-big-table");
        times.push_back(tdiff);
        algos.emplace_back("kewb-sse-big-table");
    }
    else
    {
        tdiff = TestOneConversion32(&Convert32_KewbBasic, u8src, reps, u32answer, "kewb-basic");
        times.push_back(tdiff);
        algos.emplace_back("kewb-basic");

        tdiff = TestOneConversion32(&Convert32_KewbFast, u8src, reps, u32answer, "kewb-fast");
        times.push_back(tdiff);
        algos.emplace_back("kewb-fast");

        tdiff = TestOneConversion32(&Convert32_KewbSse, u8src, reps, u32answer, "kewb-sse");
        times.push_back(tdiff);
        algos.emplace_back("kewb-sse");
    }

    return tuple<name_list, time_list>(algos, times);
}

//--------------
//
void
TestFiles32(string const& dataDir, size_t repShift, file_list const& files, bool tblCmp)
{
    name_list   algos;
    time_list   times;
    time_table  all_times;

    printf("\n******  UTF-8 to UTF-32 Conversion  ******\n");

    for (auto const& fname : files)
    {
        string  fpath(MakeFilePath(dataDir, fname));

        printf("\nfor file: '%s'\n", fname.c_str());
        fflush(stdout);
        std::tie(algos, times) = TestAllConversions32(fpath, true, repShift, tblCmp);
        all_times.emplace_back(std::move(times));
    }

    printf("\ntabular summary:\nfile\\algo");
    for (auto const& algo : algos)
    {
        printf(", %s", algo.c_str());
    }
    printf("\n");

    for (size_t i = 0;  i < files.size();  ++i)
    {
        printf("%s", files[i].c_str());

        for (size_t j = 0;  j < all_times[i].size();  ++j)
        {
            printf(", %u", (uint32_t) all_times[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    fflush(stdout);
}
