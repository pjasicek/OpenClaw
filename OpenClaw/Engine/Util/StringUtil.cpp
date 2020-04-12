#include <memory.h>
#include "StringUtil.h"

using std::string;

// Remove all leading whitespace
void TrimLeft(std::string &s)
{
    // find first non-space character
    int i = 0;
    int len = (int)s.length();
    while (i <  len)
    {
        char ch = s[i];
        int white =
#ifdef UNICODE
            iswspace(ch);
#else
            isspace(ch);
#endif
        if (!white)
            break;
        ++i;
    }

    if (i<len)
        s = s.substr(i);
}

// The following function was found on http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html, where it was attributed to 
// the C/C++ Users Journal, written by Mike Cornelison. It is a little ugly, but it is FAST. Use this as an excercise in not reinventing the
// wheel, even if you see gotos. 

bool WildcardMatch(const char *pat, const char *str) {
    int i, star;

new_segment:

    star = 0;
    if (*pat == '*') {
        star = 1;
        do { pat++; } while (*pat == '*'); /* enddo */
    } /* endif */

test_match:

    for (i = 0; pat[i] && (pat[i] != '*'); i++) {
        //if (mapCaseTable[str[i]] != mapCaseTable[pat[i]]) {
        if (str[i] != pat[i]) {
            if (!str[i]) return 0;
            if ((pat[i] == '?') && (str[i] != '.')) continue;
            if (!star) return 0;
            str++;
            goto test_match;
        }
    }
    if (pat[i] == '*') {
        str += i;
        pat += i;
        goto new_segment;
    }
    if (!str[i]) return 1;
    if (i && pat[i - 1] == '*') return 1;
    if (!star) return 0;
    str++;
    goto test_match;
}

string ToStr(int num, int base)
{
    return std::to_string(num);
}

string ToStr(std::string str)
{
    return str;
}

string ToStr(unsigned int num, int base)
{
    return std::to_string(num);
}

string ToStr(unsigned long num, int base)
{
    return std::to_string(num);
}

string ToStr(float num)
{
    return std::to_string(num);
}

string ToStr(double num)
{
    return std::to_string(num);
}

string ToStr(bool val)
{
    return (string((val == true ? "true" : "false")));
}

string ToStr(char* val)
{
    if (val == NULL)
    {
        return "";
    }
    
    return string(val);
}

string ToStr(const char* val)
{
    if (val == NULL)
    {
        return "";
    }

    return string(val);
}

//---------------------------------------------------------------------------------------------------------------------
// This is basically like the Perl split() function.  It splits str into substrings by cutting it at each delimiter.  
// The result is stored in vec.
//---------------------------------------------------------------------------------------------------------------------
void Split(const string& str, std::vector<std::string>& vec, char delimiter)
{
    vec.clear();
    size_t strLen = str.size();
    if (strLen == 0)
        return;

    size_t startIndex = 0;
    size_t indexOfDel = str.find_first_of(delimiter, startIndex);
    while (indexOfDel != string::npos)
    {
        vec.push_back(str.substr(startIndex, indexOfDel - startIndex));
        startIndex = indexOfDel + 1;
        if (startIndex >= strLen)
            break;
        indexOfDel = str.find_first_of(delimiter, startIndex);
    }
    if (startIndex < strLen)
        vec.push_back(str.substr(startIndex));
}

unsigned long HashName(char const * pIdentStr)
{
    // Relatively simple hash of arbitrary text string into a
    // 32-bit identifier Output value is
    // input-valid-deterministic, but no guarantees are made
    // about the uniqueness of the output per-input
    //
    // Input value is treated as lower-case to cut down on false
    // separations cause by human mistypes. Sure, it could be
    // construed as a programming error to mix up your cases, and
    // it cuts down on permutations, but in Real World Usage
    // making this text case-sensitive will likely just lead to
    // Pain and Suffering.
    //
    // This code lossely based upon the adler32 checksum by Mark
    // Adler and published as part of the zlib compression
    // library sources.

    // largest prime smaller than 65536
    unsigned long BASE = 65521L;

    // NMAX is the largest n such that 255n(n+1)/2 +
    // (n+1)(BASE-1) <= 2^32-1
    unsigned long NMAX = 5552;

#define DO1(buf,i)  {s1 += tolower(buf[i]); s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

    if (pIdentStr == NULL)
        return 0;

    unsigned long s1 = 0;
    unsigned long s2 = 0;

    for (size_t len = strlen(pIdentStr); len > 0;)
    {
        unsigned long k = len < NMAX ? len : NMAX;

        len -= k;

        while (k >= 16)
        {
            DO16(pIdentStr);
            pIdentStr += 16;
            k -= 16;
        }

        if (k != 0) do
        {
            s1 += tolower(*pIdentStr++);
            s2 += s1;
        } while (--k);

        s1 %= BASE;
        s2 %= BASE;
    }

#pragma warning(push)
#pragma warning(disable : 4312)

    return ((s2 << 16) | s1);

#pragma warning(pop)
#undef DO1
#undef DO2
#undef DO4
#undef DO8
#undef DO16
}

std::string GetBaseName(const std::string& path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

std::string RemoveExtension(const std::string& fileName)
{
    std::string::size_type const p(fileName.find_last_of('.'));
    return fileName.substr(0, p);
}

std::string StripPathAndExtension(const std::string& fullFilePath)
{
    std::string path = GetBaseName(fullFilePath);
    return RemoveExtension(path);
}
