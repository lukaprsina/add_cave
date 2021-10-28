#include "platform.h"

#include <filesystem>
#include <iostream>

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
#include <shellapi.h>

void open_file(std::filesystem::path &path)
{
    ShellExecute(0, 0, path.generic_wstring().c_str(), 0, 0, SW_SHOW);
    /* ShellExecuteW(_In_opt_ HWND hwnd, _In_opt_ LPCWSTR lpOperation, _In_ LPCWSTR lpFile, _In_opt_ LPCWSTR lpParameters,
        _In_opt_ LPCWSTR lpDirectory, _In_ INT nShowCmd); */
}

void open_file_in_editor(std::filesystem::path &path)
{
    ShellExecute(0, L"edit", path.generic_wstring().c_str(), 0, 0, SW_SHOW);
}
