#ifndef UTIL_H_
#define UTIL_H_

#include <vector>
#include <stdint.h>
#include <string>
#include <string.h>

class InputStream;

char* StdStringToCharArray(std::string source);
char* ReadAndAllocateString(InputStream &stream, size_t len);
char* ReadNullTerminatedString(InputStream &stream);
std::vector<char> ReadWholeFile(char* filePath);

#endif //UTIL_H_