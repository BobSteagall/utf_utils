#include "test_main.h"

using namespace std;
using namespace uu;

using TestFn16 = ptrdiff_t (*)(string const&, size_t, u16string&);

//--------------
//
ptrdiff_t
Convert16_Iconv(string const& src, size_t reps, u16string& dst)
{
    static  bool    init = false;
    static  iconv_t jdsc = (iconv_t)(-1);

    if (!init)
    {
        jdsc = iconv_open("UTF-16LE", "UTF-8");

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
        dstLen  = srcLen*sizeof(char16_t);
        pSrcBuf = (char*) &src[0];
        pDstBuf = (char*) &dst[0];
        iconv(jdsc, &pSrcBuf, &srcLen, &pDstBuf, &dstLen);
    }
    dstLen = (char16_t*) pDstBuf - &dst[0];

    return dstLen;
}

//--------------
//
ptrdiff_t
Convert16_Llvm(string const& src, size_t reps, u16string& dst)
{
    size_t          srcLen = src.size();    //- Number of code units in source string
    size_t          dstLen = dst.size();    //- Number of code points in destination string
    uchar const*    pSrcBuf;                //- Pointer to source buffer
    UTF16*          pDstBuf;                //- Pointer to destination buffer

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        pDstBuf = (UTF16*) &dst[0];
        pSrcBuf = (uchar const*) &src[0];
        ConvertUTF8toUTF16(&pSrcBuf, pSrcBuf+srcLen, &pDstBuf, pDstBuf+dstLen, strictConversion);
    }
    dstLen = pDstBuf - (UTF16*) &dst[0];

    return dstLen;
}

#ifdef KEWB_COMPILER_MSVC
//--------------
//
ptrdiff_t
Convert16_MS(string const& src, size_t reps, u16string& dst)
{
    ptrdiff_t   tmpLen  = 0;
    int         srcLen  = (int) src.size();
    int         dstLen  = (int) dst.size();
    char const* pSrcBuf = &src[0];
    wchar_t*    pDstBuf = (wchar_t*)(void*) &dst[0];

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        tmpLen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, pSrcBuf, srcLen, pDstBuf, dstLen);
    }
    
    return tmpLen;
}
#endif

//--------------
//
ptrdiff_t
Convert16_Codecvt(string const& src, size_t reps, u16string& dst)
{
    using Utf8to16 = wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>;

    Utf8to16    utf16conv;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dst = utf16conv.from_bytes(src);
    }
    
    return (ptrdiff_t) dst.size();
}

#ifdef KEWB_COMPILER_MSVC
    __PURE_APPDOMAIN_GLOBAL std::locale::id     std::codecvt<char16_t, char, struct _Mbstatet>::id;
#endif

//--------------
//
std::ptrdiff_t
BoostTextConvert(char8_t const* pSrc, char8_t const* pSrcEnd, char16_t* pDst) noexcept
{
    char16_t *pDstOrig = pDst;

    auto first = boost::text::utf8::to_utf16_iterator<char8_t const*>(pSrc, pSrc, pSrcEnd);
    auto last =  boost::text::utf8::to_utf16_iterator<char8_t const*>(pSrc, pSrcEnd, pSrcEnd);

    pDst = std::copy(first, last, pDst);

    return pDst - pDstOrig;
}

ptrdiff_t
Convert16_BoostText(string const& src, size_t reps, u16string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char16_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
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
Convert16_Hoehrmann(string const& src, size_t reps, u16string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    size_t          srcLen  = src.size();               //- Number of code units
    char16_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
    ptrdiff_t       dstLen  = 0;

    for (uint64_t i = 0;  i < reps;  ++i)
    {
        dstLen = toUtf16(pSrcBuf, srcLen, pDstBuf);
    }

    return dstLen;
}

//--------------------------------------------------------------------------------------------------
//
ptrdiff_t
Convert16_KewbBasicSmTab(string const& src, size_t reps, u16string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char16_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
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
Convert16_KewbFastSmTab(string const& src, size_t reps, u16string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char16_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
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
Convert16_KewbSseSmTab(string const& src, size_t reps, u16string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char16_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
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
Convert16_KewbBasicBgTab(string const& src, size_t reps, u16string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char16_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
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
Convert16_KewbFastBgTab(string const& src, size_t reps, u16string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char16_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
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
Convert16_KewbSseBgTab(string const& src, size_t reps, u16string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char16_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
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
Convert16_KewbBasic(string const& src, size_t reps, u16string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char16_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
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
Convert16_KewbFast(string const& src, size_t reps, u16string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char16_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
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
Convert16_KewbSse(string const& src, size_t reps, u16string& dst)
{
    char8_t const*  pSrcBuf = (char8_t const*) &src[0]; //- Pointer to source buffer
    char8_t const*  pSrcEnd = pSrcBuf + src.size();     //- Pointer to end of source buffer
    char16_t*       pDstBuf = &dst[0];                  //- Pointer to destination buffer
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
TestOneConversion16
(TestFn16 fn, string const& src, size_t reps, u16string const& answer, char const* name)
{
    using tm_pt = chrono::high_resolution_clock::time_point;

    tm_pt       start, finish;
    int64_t     tmdiff;
    ptrdiff_t   dstLen;
    u16string   dst(src.size(), 0u);

    start  = chrono::high_resolution_clock::now();
    dstLen = fn(src, reps, dst);
    finish = chrono::high_resolution_clock::now();
    tmdiff = chrono::duration_cast<chrono::milliseconds>(finish - start).count();

    dst.resize((dstLen >= 0) ? (size_t) dstLen : 0u);

    printf("UTF-8 to UTF-16 took %4u msec (%zu/%zu units/units) (%zu reps) (%s)\n",
            (uint16_t) tmdiff, src.size(), dst.size(), reps, ((name != nullptr) ? name : ""));

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
TestAllConversions16(string const& fname, bool isFile, size_t repShift, bool tblCmp)
{
    size_t      reps;
    string      u8src;
    u16string   u16answer;
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
    u16answer.resize(u8src.size(), 0);
    u16answer.resize((size_t) Convert16_Iconv(u8src, 1, u16answer));

    //- Run the individual tests.
    //
    tdiff = TestOneConversion16(&Convert16_Iconv, u8src, reps, u16answer, "iconv");
    times.push_back(tdiff);
    algos.emplace_back("iconv");

    tdiff = TestOneConversion16(&Convert16_Llvm, u8src, reps, u16answer, "llvm");
    times.push_back(tdiff);
    algos.emplace_back("llvm");

#ifdef KEWB_COMPILER_MSVC
    tdiff = TestOneConversion16(&Convert16_MS, u8src, reps, u16answer, "win32-mbtowc");
    times.push_back(tdiff);
    algos.emplace_back("win32-mbtowc");
#endif

    tdiff = TestOneConversion16(&Convert16_Codecvt, u8src, reps, u16answer, "std::codecvt");
    times.push_back(tdiff);
    algos.emplace_back("std::codecvt");

    tdiff = TestOneConversion16(&Convert16_BoostText, u8src, reps, u16answer, "Boost.Text");
    times.push_back(tdiff);
    algos.emplace_back("Boost.Text");

    tdiff = TestOneConversion16(&Convert16_Hoehrmann, u8src, reps, u16answer, "hoehrmann");
    times.push_back(tdiff);
    algos.emplace_back("Hoehrmann");

    if (tblCmp)
    {
        tdiff = TestOneConversion16(&Convert16_KewbBasicSmTab, u8src, reps, u16answer, "kewb-basic-small-table");
        times.push_back(tdiff);
        algos.emplace_back("kewb-basic-small-table");

        tdiff = TestOneConversion16(&Convert16_KewbBasicBgTab, u8src, reps, u16answer, "kewb-basic-big-table");
        times.push_back(tdiff);
        algos.emplace_back("kewb-basic-big-table");

        tdiff = TestOneConversion16(&Convert16_KewbFastSmTab, u8src, reps, u16answer, "kewb-fast-small-table");
        times.push_back(tdiff);
        algos.emplace_back("kewb-fast-small-table");

        tdiff = TestOneConversion16(&Convert16_KewbFastBgTab, u8src, reps, u16answer, "kewb-fast-big-table");
        times.push_back(tdiff);
        algos.emplace_back("kewb-fast-big-table");

        tdiff = TestOneConversion16(&Convert16_KewbSseSmTab, u8src, reps, u16answer, "kewb-sse-small-table");
        times.push_back(tdiff);
        algos.emplace_back("kewb-sse-small-table");

        tdiff = TestOneConversion16(&Convert16_KewbSseBgTab, u8src, reps, u16answer, "kewb-sse-big-table");
        times.push_back(tdiff);
        algos.emplace_back("kewb-sse-big-table");
    }
    else
    {
        tdiff = TestOneConversion16(&Convert16_KewbBasic, u8src, reps, u16answer, "kewb-basic");
        times.push_back(tdiff);
        algos.emplace_back("kewb-basic");

        tdiff = TestOneConversion16(&Convert16_KewbFast, u8src, reps, u16answer, "kewb-fast");
        times.push_back(tdiff);
        algos.emplace_back("kewb-fast");

        tdiff = TestOneConversion16(&Convert16_KewbSse, u8src, reps, u16answer, "kewb-sse");
        times.push_back(tdiff);
        algos.emplace_back("kewb-sse");
    }

    return tuple<name_list, time_list>(algos, times);
}

//--------------
//
void
TestFiles16(string const& dataDir, size_t repShift, file_list const& files, bool tblCmp)
{
    name_list   algos;
    time_list   times;
    time_table  all_times;

    printf("\n******  UTF-8 to UTF-16 Conversion  ******\n");

    for (auto const& fname : files)
    {
        string  fpath(MakeFilePath(dataDir, fname));

        printf("\nfor file: '%s'\n", fname.c_str());
        fflush(stdout);
        std::tie(algos, times) = TestAllConversions16(fpath, true, repShift, tblCmp);
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
