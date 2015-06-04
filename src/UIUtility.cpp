//////////////////////////////////////////////////////////////////////////
// Create by yyCom, 2015/4/10
//////////////////////////////////////////////////////////////////////////

#include "UIUtility.h"
#include <tchar.h>
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


   

    std::string UTF8ToGBKA(LPCSTR szUFT8)
    {
        int nLen = ::MultiByteToWideChar( CP_UTF8, 0, szUFT8, -1, NULL, 0);
        if( !nLen )
            return std::string();
        std::unique_ptr<wchar_t[]> pwstr( new wchar_t[ nLen ] ); 
        if( !pwstr )
            return std::string();
        ::MultiByteToWideChar( CP_UTF8, 0, szUFT8, -1, pwstr.get(), nLen );
        pwstr[nLen-1] = L'\0';
        nLen = ::WideCharToMultiByte( CP_ACP, 0, pwstr.get(), -1, NULL, 0, NULL, NULL);
        if( !nLen )
            return std::string();
        std::unique_ptr<char[] > pstrDes (new char[ nLen ] );
        if( !pstrDes )
            return std::string();
        WideCharToMultiByte(CP_ACP, 0, pwstr.get(), -1, pstrDes.get(), nLen, NULL, NULL);
        pstrDes[ nLen-1 ] = '\0';

        return std::string(pstrDes.get());
    }


    std::string GBKToUTF8A(LPCSTR szGBK)
    {
        int nLen = ::MultiByteToWideChar( CP_ACP, 0, szGBK, -1, NULL, 0);
        if( !nLen )
            return std::string();
        std::unique_ptr<wchar_t[]> pwstr( new wchar_t[ nLen ] ); 
        if( !pwstr )
            return std::string();
        ::MultiByteToWideChar( CP_ACP, 0, szGBK, -1, pwstr.get(), nLen );
        pwstr[nLen-1] = L'\0';

        nLen = ::WideCharToMultiByte( CP_UTF8, 0, pwstr.get(), -1, NULL, 0, NULL, NULL);
        if( !nLen )
            return std::string();
        std::unique_ptr<char[] > pstrDes (new char[ nLen ] );
        if( !pstrDes )
            return std::string();
        WideCharToMultiByte(CP_UTF8, 0, pwstr.get(), -1, pstrDes.get(), nLen, NULL, NULL);
        pstrDes[ nLen-1 ] = '\0';

        return std::string(pstrDes.get());
    }

    std::wstring UTF8ToGBKW(LPCSTR szUTF8)
    {
        int nLen = ::MultiByteToWideChar( CP_UTF8, 0, szUTF8, -1, NULL, 0);
        if( !nLen )
            return std::wstring();
        std::unique_ptr<wchar_t[]> pwstr( new wchar_t[ nLen ] ); 
        if( !pwstr )
            return std::wstring();
        ::MultiByteToWideChar( CP_UTF8, 0, szUTF8, -1, pwstr.get(), nLen );
        pwstr[nLen-1] = L'\0';
        return std::wstring(pwstr.get());
    }

    std::wstring GBKToUTF8W(LPCSTR szGBK)
    {
        int nLen = ::MultiByteToWideChar( CP_UTF8, 0, szGBK, -1, NULL, 0);
        if( !nLen )
            return std::wstring();
        std::unique_ptr<wchar_t[]> pwstr( new wchar_t[ nLen ] ); 
        if( !pwstr )
            return std::wstring();
        ::MultiByteToWideChar( CP_UTF8, 0, szGBK, -1, pwstr.get(), nLen );
        pwstr[nLen-1] = L'\0'; 
        return std::wstring(pwstr.get());
    }

	bool IntersectRect(YYRECT *pDest,const YYRECT &lhs,const YYRECT &rhs)
	{
		YYRECT NormalLhs= lhs;
		NormalLhs.Normalize();
		YYRECT NormalRhs= rhs;
		NormalRhs.Normalize();
		if(NormalLhs.left > NormalRhs.right ||
		   NormalLhs.right < NormalRhs.left ||
		   NormalRhs.bottom < NormalRhs.top ||
		   NormalRhs.top > NormalRhs.bottom )
		   return false;
		YYRECT tmp;
		tmp.left = (std::max)(NormalLhs.left,NormalRhs.left);
		tmp.right = (std::min)(NormalLhs.right,NormalLhs.right);
		tmp.top = (std::max)(NormalLhs.top,NormalRhs.top);
		tmp.bottom= (std::min)(NormalLhs.bottom,NormalRhs.bottom);
		if( pDest)
			*pDest = tmp;
		return true;
	}

	bool PtInRect(const YYRECT &rc,const YYPOINT &pt)
	{
		YYRECT rcNormal = rc;
		rcNormal.Normalize();
		if( rc.left <= pt.x && pt.x <= rc.right && rc.top <= pt.y && pt.y <= rc.bottom )
			return true;
		else 
			return false;
	}

}