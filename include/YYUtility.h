//////////////////////////////////////////////////////////////////////////
// Create by yyCom 2015/4/10
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "YYUI.h"
#include <windows.h>
#include <string>
#include <memory>

namespace YYCOM
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
}