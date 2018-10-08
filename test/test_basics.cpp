#include "test_main.h"

using namespace std;
using namespace uu;

void
TestTrace()
{
    string      sample{u8R"('kosme' : "κόσμε")"};
    char8_t*    pSrc = (char8_t*) &sample[0];
    u32string   dst(sample.size(), '0');

    printf("\ntracing a sample string [%s]...\n", sample.c_str());
    UtfUtils::ConvertWithTrace(pSrc, pSrc + sample.size(), &dst[0]);
    printf("    ... done\n");
}

//--------------
//
void
TestBadSequences()
{
    vector<array<uchar,8>>    seqs =
    {
        { { 0xC0, 0xAF, ' ' } },
        { { 0xE0, 0x80, 0xAF, ' ' } },
        { { 0xF0, 0x80, 0x80, 0xAF, ' ' } },
        { { 0xF8, 0x80, 0x80, 0x80, 0xAF, ' ' } },
        { { 0xFC, 0x80, 0x80, 0x80, 0x80, 0xAF, ' ' } },
        { { 0xC1, 0xBF, ' ' } },
        { { 0xE0, 0x9F, 0xBF, ' ' } },
        { { 0xF0, 0x8F, 0xBF, 0xBF, ' ' } },
        { { 0xF8, 0x87, 0xBF, 0xBF, 0xBF, ' ' } },
        { { 0xFC, 0x83, 0xBF, 0xBF, 0xBF, 0xBF, ' ' } },
    };
    size_t  errors = 0;

    printf("\ntesting bad octet sequences...\n");

    for (auto seq : seqs)
    {
        char32_t    cdpt;

        if (UtfUtils::GetCodePoint(seq.data(), seq.data()+8u, cdpt))
        {
            printf("conversion error: octet sequence ");
            for (int i = 0;  seq[i] != ' ';  ++i)
            {
                printf(" 0x%02X", seq[i]);
            }
            printf(" is invalid\n");
            ++errors;
        }
    }

    if (errors == 0) printf("    ... no errors found\n");
}

//--------------
//
void
TestRoundTripping()
{
    size_t const    bufSize = 8;

    size_t      icFlag;             //- Result flag from iconv() call
    size_t      srcLen;             //- Size of source buffer in call to iconv()
    size_t      dstLen;             //- Size of destination buffer in call to iconv()
    size_t      tstLen;             //- Size of test buffer for UtfUtils call
    char        srcBuf[bufSize];    //- Source buffer for call to iconv()
    char        dstBuf[bufSize];    //- Destination buffer for call to iconv()
    char8_t     tstBuf[bufSize];    //- Test buffer for UtfUtils call
    char*       pSrcBuf;            //- Pointer to first byte in source buffer
    char*       pDstBuf;            //- Pointer to first byte in destination buffer
    char8_t*    pTstBuf;            //- Pointer to first octet in test buffer
    char32_t    tstCdpt;            //- Test code point for round-trip conversion attempt
    size_t      errors = 0;
    iconv_t     jdsc = iconv_open("UTF-8", "UTF-32LE");

    printf("\ntesting round-trip conversions...\n");

    for (char32_t cdpt = 0;  cdpt < 0x110000;  ++cdpt)
    {
        if (0xD800 <= cdpt  &&  cdpt <= 0xDFFF)  continue;

        //- Reset the buffers.
        //
        memset(srcBuf, 0, bufSize);
        memset(dstBuf, 0, bufSize);
        memset(tstBuf, 0, bufSize);

        //- Copy the code point into the input buffer.
        //
        memcpy(srcBuf, &cdpt, sizeof(cdpt));

        //- Convert the code point in the input buffer into UTF-8 code units in the output buffer.
        //
        srcLen  = sizeof(cdpt);
        dstLen  = bufSize;
        pSrcBuf = &srcBuf[0];
        pDstBuf = &dstBuf[0];
        icFlag  = iconv(jdsc, &pSrcBuf, &srcLen, &pDstBuf, &dstLen);
        dstLen  = bufSize - dstLen;

        //- Fill the test output buffer with blank spaces, and then try the UtfUtils conversion
        //  from code point to code unit(s).
        //
        pTstBuf = &tstBuf[0];
        tstLen  = UtfUtils::GetCodeUnits(cdpt, pTstBuf);

        //- Verify that the output buffer and test output buffer are the same size and contain
        //  the same sequence of octets.
        //
        if (dstLen != tstLen  ||  memcmp(dstBuf, tstBuf, dstLen) != 0  ||  icFlag == (size_t)(-1))
        {
            printf("difference going to UTF-8 at code point 0x%X (%d)  dstLen=%d  tstLen=%d\n",
                   (uint32_t) cdpt, (uint32_t) cdpt, (uint32_t) dstLen, (uint32_t) tstLen);
            ++errors;
            continue;
        }

        //- Convert the code unit buffer into a code point.
        //
        tstCdpt = char32_t(~0u);
        pTstBuf = &tstBuf[0];
        UtfUtils::GetCodePoint(pTstBuf, pTstBuf + tstLen, tstCdpt);

        //- Verify that the round-tripped code point is the same as the original code point.
        //
        if (tstCdpt != cdpt)
        {
            printf("difference going to UTF-32 at code point 0x%X (%d)  test-point=%d\n",
                   (uint32_t) cdpt, (uint32_t) cdpt, (uint32_t) tstCdpt);
            ++errors;
        }
    }

    if (errors == 0) printf("    ... no errors found\n");
}

