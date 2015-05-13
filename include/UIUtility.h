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
    class YRect : public RECT
    {
    public:
        YRect();
        YRect(const RECT & src);
        YRect(int nleft, int nTop, int nRight, int nBottom);

        int                             GetWidth() const;
        int                             GetHeight() const;
        void                            Empty();
        bool                            IsEmpty() const;
        void                            Join(const RECT & rc);
        void                            ResetOffset();
        void                            Normalize();
        void                            OffSet(int cx, int cy);
        void                            Inflate(int cx,int cy);
        void                            Defalte(int cx,int cy);
        void                            Union(YRect & rc);
    };
    class YSize: public tagSIZE
    {
    public:
        YSize();
        YSize(const SIZE & src);
        YSize(const RECT rc);
        YSize(int cx, int cy);
    };
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