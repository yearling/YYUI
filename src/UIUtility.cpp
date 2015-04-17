//////////////////////////////////////////////////////////////////////////
// Create by yyCom, 2015/4/10
//////////////////////////////////////////////////////////////////////////

#include "UIUtility.h"

namespace YUI
{


    std::string WChar2Ansi(LPCWSTR pwStr)
    {
        int nLen = ::WideCharToMultiByte( CP_ACP, 0, pwStr, -1, NULL, 0, NULL, NULL);
        if( !nLen )
            return std::string();
        std::unique_ptr<char[] > pstrDes (new char[ nLen ] );
        if( !pstrDes )
            return std::string();
        WideCharToMultiByte(CP_ACP, 0, pwStr, -1, pstrDes.get(), nLen, NULL, NULL);
        pstrDes[ nLen-1 ] = '\0';

        return std::string(pstrDes.get());
    }

    std::wstring Ansi2Wchar(LPCSTR pStr)
    {
        int nLen = ::MultiByteToWideChar( CP_ACP, 0, pStr, -1, NULL, 0);
        if( !nLen )
            return std::wstring();
        std::unique_ptr<wchar_t[]> pwstr( new wchar_t[ nLen ] ); 
        if( !pwstr )
            return std::wstring();
        ::MultiByteToWideChar( CP_ACP, 0, pStr, -1, pwstr.get(), nLen );
        pwstr[nLen-1] = L'\0';

        return std::wstring(pwstr.get());
    }

    YString FormatGetLastError()
    {
        std::unique_ptr<TCHAR[]> pStr(new TCHAR[1024]);
        if( !pStr )
            return YString();
        ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM ,
                        NULL,
                        GetLastError(),
                        MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                        (LPTSTR)pStr.get(),
                        0,
                        NULL);
        return YString(pStr.get());
    }

    YString FormatGetLastError(DWORD dwErr)
    {
        std::unique_ptr<TCHAR[]> pStr(new TCHAR[1024]);
        if( !pStr )
            return YString();
        ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM ,
            NULL,
            dwErr,
            MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
            (LPTSTR)pStr.get(),
            0,
            NULL);
        return YString(pStr.get());
    }


    YRect::YRect()
    {
        left = top = right = bottom = 0;
    }

    YRect::YRect(const RECT & src)
    {
        left = src.left;
        right = src.right;
        top = src.top;
        bottom = src.bottom;
    }

    YRect::YRect(int nleft, int nTop, int nRight, int nBottom)
    {
        left = nleft;
        top = nTop;
        right = nRight;
        bottom = nBottom;
    }

    void YRect::Empty()
    {
        left = right = top = bottom = 0;
    }

    int YRect::GetWidth() const
    {
        return right - left;
    }

    int YRect::GetHeight() const
    {
        return bottom - top;
    }

    void YRect::Join(const RECT & rc) //Ê¹·¶Î§±ä´ó
    {
        if( rc.left < left )
            left = rc.left;
        if( rc.top < top )
            top = rc.top;
        if( rc.right > right )
            right = rc.right;
        if( rc.bottom > bottom )
            bottom = rc.bottom;
    }

    void YRect::ResetOffset()
    {
        ::OffsetRect(this, -left, -top);
    }

    void YRect::Normalize()
    {
        if( left > right )
        {
            std::swap(left,right);
        }
        if( top > bottom )
        {
            std::swap(top,bottom);
        }
    }

    void YRect::OffSet(int cx, int cy)
    {
        ::OffsetRect(this, cx, cy);
    }

    void YRect::Inflate(int cx,int cy)
    {
        ::InflateRect(this, cx, cy);
    }

    void YRect::Defalte(int cx,int cy)
    {
        ::InflateRect(this, -cx, -cy);
    }

    void YRect::Union(YRect & rc)
    {
        ::UnionRect(this,this,&rc);
    }

    bool YRect::IsEmpty() const
    {
        return ( left == 0 && right == 0 && top == 0 && bottom == 0);
    }

    YUI::YString GetFileExtension(const YString & str)
    {
        auto pos = str.find_last_of(_T('.'));
        if( pos == YString::npos)
            return _T("");

        return str.substr(pos+1);
    }

    YUI::YString GetFileDir(const YString & strFilePath)
    {
        auto pos = strFilePath.find_last_of(_T("/\\"));
        if( pos == YString::npos )
            return YString();

        return strFilePath.substr(0,pos);
    }

    YUI::YString GetFileName(const YString & strFilePath)
    {
        auto pos = strFilePath.find_last_of(_T("/\\"));
        if( pos == YString::npos )
            return YString();

        return strFilePath.substr(pos+1);
    }


    YSize::YSize()
    {
        cx = cy = 0 ;
    }

    YSize::YSize(const SIZE & src)
    {
        cx = src.cx;
        cy = src.cy;
    }

    YSize::YSize(const RECT rc)
    {
        cx = rc.right - rc.left;
        cy = rc.bottom - rc.top;
    }

    YSize::YSize(int x, int y)
    {
         cx = x;
         cy = y;
    }

}