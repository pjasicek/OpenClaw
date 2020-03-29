#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_

#include <vector>
#include <string>

#define MAX_DIGITS_IN_INT 12  // max number of digits in an int (-2147483647 = 11 digits, +1 for the '\0')

// Removes leading white space
extern void TrimLeft(std::string &s);

// Counts the number of lines in a block of text
extern int CountLines(const std::wstring &s);

// Does a classic * & ? pattern match on a file name - this is case sensitive!
extern bool WildcardMatch(const char *pat, const char *str);

extern std::string ToStr(int num, int base = 10);
extern std::string ToStr(unsigned int num, int base = 10);
extern std::string ToStr(unsigned long num, int base = 10);
extern std::string ToStr(float num);
extern std::string ToStr(double num);
extern std::string ToStr(bool val);
extern std::string ToStr(char* val);
extern std::string ToStr(const char* val);
extern std::string ToStr(std::string str);

// Splits a string by the delimeter into a vector of strings.  For example, say you have the following string:
// std::string test("one,two,three");
// You could call Split() like this:
// Split(test, outVec, ',');
// outVec will have the following values:
// "one", "two", "three"
void Split(const std::string& str, std::vector<std::string>& vec, char delimiter);

unsigned long HashName(char const* identStr);

std::string GetBaseName(const std::string& path);
std::string RemoveExtension(const std::string& fileName);
std::string StripPathAndExtension(const std::string& fullFilePath);

inline bool StringContains(std::string& str, std::string& what)
{
    return str.find(what) != std::string::npos;
}

#endif