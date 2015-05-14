#include "YUI.h"

namespace YUI
{
    YString GetFileExtention(const YString &strFileName);
    YString GetModulePath(HMODULE hHandle = NULL);
    YString GetEXEPathAbsulute();
    YString GetCurrentDir();
    YString GetEXEName();
    YString GetRelativePath(const YString &strPath);
}