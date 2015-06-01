//////////////////////////////////////////////////////////////////////////
// Create by yyCom 2015/4/10
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "YUI.h"
#include <windows.h>
#include <string>
#include <memory>

namespace YUI
{
   
    std::string WChar2Ansi(LPCWSTR pwStr);
    
    std::wstring Ansi2Wchar(LPCSTR pStr);
   
    inline std::string W2S(const std::wstring & w) 
    {
        return WChar2Ansi(w.c_str());
    }

    inline std::wstring S2W( const std::string & s)
    {
        return Ansi2Wchar(s.c_str());
    }
#if defined UNICODE || defined _UNICODE
#define ToWString( a )  (a)
#define ToCWSTR(a)  (a.c_str())   
#define CWSTRLength(a) (a.length())
#else
#define ToWString( a )  (S2W(a))
#define ToCWSTR(a)  (S2W(a).c_str())
#define CWSTRLength(a) (S2W(a).length())
#endif

    std::string UTF8ToGBKA( LPCSTR szUFT8 );
    
    std::string GBKToUTF8A(LPCSTR szGBK);
   
    std::wstring UTF8ToGBKW(LPCSTR szUTF8 );
    std::wstring GBKToUTF8W(LPCSTR szGBK);



    //template<class T>
    //std::wstring ToWString(const T & );
    //
    //template<class T>
    //inline std::wstring ToWString(const std::string & str) 
    //{
    //    return Ansi2Wchar(str.c_str());
    //}

    //template<class T>
    //std::wstring ToWString( const std::wstring & str)
    //{
    //    return str;
    //}

    YString FormatGetLastError();
    YString FormatGetLastError(DWORD dwErr);

    //////////////////////////////////////////////////////////////////////////
    //�ļ�������
    //�õ��ļ�����չ��,
    //@strFileNameOrPath,�������ļ������ӣ�Ҳ��������Ի����·��
    YString GetFileExtension( const YString & strFileNameOrPath);

    //�õ��ļ����ڵ�Ŀ¼,����"\/"
    //@strFilePath,�ļ�����Ի����·��
    YString GetFileDir(const YString & strFilePath);
    
    //�õ��ļ���
    //@strFilePath,�ļ�����Ի����·��,����"\/"
    YString GetFileName(const YString & strFilePath);
    //////////////////////////////////////////////////////////////////////////
  
    //////////////////////////////////////////////////////////////////////////
    class STRINGorID
    {
    public:
        STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
        { }
        STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
        { }
        LPCTSTR m_lpstr;
    };

}