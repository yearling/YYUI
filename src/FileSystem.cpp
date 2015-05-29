#include "YUI.h"
#include "FileSystem.h"

namespace YUI
{


    YUI::YString GetFileExtention(const YString &strFileName)
    {
        auto pos = strFileName.find_last_of(_T('.'));
        return strFileName.substr(pos,strFileName.size());
    }

    YUI::YString GetModulePath(HMODULE hHandle /*= NULL*/)
    {
       TCHAR pBuf[MAX_PATH+1]; 
       ::GetModuleFileName(hHandle,pBuf,MAX_PATH);
       return pBuf;
    }

    YUI::YString GetEXEPathAbsulute()
    {
        TCHAR pBuf[MAX_PATH+1];
        ::GetCurrentDirectory(MAX_PATH,pBuf);
        return pBuf;
    }

    YUI::YString GetEXEName()
    {
        TCHAR pBuf[MAX_PATH+1]; 
        ::GetModuleFileName(nullptr,pBuf,MAX_PATH);
        YString path(pBuf);
        auto pos = path.find_last_of(_T('.'));
        return path.substr(pos,path.size());
    }

    YUI::YString GetCurrentDir()
    {
        TCHAR pBuf[MAX_PATH+1];
        ::GetCurrentDirectory(MAX_PATH,pBuf);
        return pBuf;
    }

    YUI::YString GetRelativePath(const YString &strPath)
    {
        return _T("");
    }

}

