#include "Utils.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

std::string getEnv(const char* name)
{
#if defined(_WIN32)
    if (uint32_t size = GetEnvironmentVariable(name, nullptr, 0))
    {
        std::vector<char> buffer(size);
        GetEnvironmentVariable(name.c_str(), buffer.data(), size);
        return std::string(buffer.data());
    }
#else
    if (const char* const result = getenv(name))
    {
        return std::string(result);
    }
#endif
    
    return "";
}

void setEnv(const char* name, const char* value)
{
#if defined(_WIN32)
    SetEnvironmentVariable(name, value) != 0;
#else
    setenv(name, value, true);
#endif
}

void split(const std::string& str,
           const char* seps,
           std::vector<std::string>* results)
{
    
}

std::string stringFormat(const char* format, ...)
{
    return "";
}
