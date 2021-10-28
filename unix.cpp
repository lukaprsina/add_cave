#include <unistd.h>
#include <filesystem>

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
    system(std::string(std::string("/usr/bin/xdg-open ") + path.generic_string()).c_str());
}

void open_file_in_editor(std::filesystem::path &path)
{
    system(std::string(std::string("/usr/bin/code ") + path.generic_string()).c_str());
}