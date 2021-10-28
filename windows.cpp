#pragma once

#include "platform.h"

#include <filesystem>

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

void open_file(std::filesystem::path &path)
{
    ShellExecute(0, 0, path.generic_wstring(), 0, 0, SW_SHOW);
}

void open_file_in_editor(std::filesystem::path &path)
{
    ShellExecute(0, "edit", path.generic_wstring(), 0, 0, SW_SHOW);
}
