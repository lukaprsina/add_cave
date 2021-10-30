#include <unistd.h>
#include <filesystem>
#include <cstring>
#include <iostream>

namespace
{
    std::string exec(const char *cmd)
    {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe)
        {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            result += buffer.data();
        }
        return result;
    }
}

void open_file(std::filesystem::path &path)
{
    std::system(std::string(std::string("/usr/bin/bash -c /usr/bin/xdg-open ") + path.generic_string()).c_str());
}