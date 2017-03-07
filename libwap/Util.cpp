#include <stdexcept>
#include "IO.h"
#include "Util.h"

char* StdStringToCharArray(std::string source)
{
    char* ret = new char[source.length() + 1];
    strcpy(ret, source.c_str());
    return ret;
}

char* ReadAndAllocateString(InputStream &stream, size_t len)
{
    std::string str;
    str.resize(len);
    stream.read_buffer(&str[0], str.size());
    return StdStringToCharArray(str);
}

char* ReadNullTerminatedString(InputStream &stream)
{
    std::string str;
    char c = '\0';
    while (true)
    {
        stream.read(c);
        if (c == 0)
        {
            break;
        }
        str += c;
    }

    return StdStringToCharArray(str);
}

std::vector<char> ReadWholeFile(char* filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    if (!file.good())
    {
        throw std::exception();
    }

    return buffer;
}