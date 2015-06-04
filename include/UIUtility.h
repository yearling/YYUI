//////////////////////////////////////////////////////////////////////////
// Create by yyCom 2015/4/10
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <string>
#include <memory>
#include <d2d1helper.h>

namespace YUI
{
	//YString 用的就是标准string
#if defined UNICODE || defined _UNICODE
	typedef std::wstring                YString;
#define Ycout std::wcout
#else
	typedef std::string                 YString;
#define  Ycout std::cout
#endif

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
    //文件名操作
    //得到文件的扩展名,
    //@strFileNameOrPath,可以是文件的名子，也可以是相对或绝对路径
    YString GetFileExtension( const YString & strFileNameOrPath);

    //得到文件所在的目录,不带"\/"
    //@strFilePath,文件的相对或绝对路径
    YString GetFileDir(const YString & strFilePath);
    
    //得到文件名
    //@strFilePath,文件的相对或绝对路径,不带"\/"
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
	//////////////////////////////////////////////////////////////////////////
	
#define   YYRGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16) | (((DWORD)(BYTE)(255))<<24)))
#define   YYRGBA(r,g,b,a)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16) | (((DWORD)(BYTE)(a))<<24)))
	struct YYCOLOR
	{
		float r;
		float g;
		float b;
		float a;
		operator COLORREF()
		{
			return (COLORREF)YYRGB((byte)r*255,(byte)g*255,(byte)b*255);
		}
		operator unsigned int()
		{
			return   (((BYTE)(r*255)|((WORD)((BYTE)(g*255))<<8))|(((DWORD)(BYTE)(b*255))<<16) | (((DWORD)(BYTE)(a*255))<<16));
		}
		YYCOLOR(int col)
		{
			r = float((byte)(0xFF & col))/255.0f;
			g = float((byte)((0xFF& (col>>8)))) /255.0f;
			b = float((byte)(0xFF& (col>>16))) /255.0f;
			a = float((byte)(0xFF& (col>>24))) /255.0f;
		}
		YYCOLOR(D2D1::ColorF col ) 
		{
			r= col.r;
			g= col.g;
			b= col.b;
			a= col.a;
		}
		YYCOLOR():r(0.0f),g(0.0f),b(0.0f),a(0.0f) {};
		operator D2D1::ColorF() const
		{
			return D2D1::ColorF(r,g,b,a);
		}
	};
	struct YYRECT
	{
		float left;
		float right;
		float top;
		float bottom;
		YYRECT():left(0.0f),right(0.0f),top(0.0f),bottom(0.0f){};
		YYRECT(int iLeft,int iTop,int iRight ,int iBottom)
			:left(float(iLeft))
			,top(float(iTop))
			,right(float(iRight))
			,bottom(float(iBottom)){};
		YYRECT(float fLeft,float fTop,float fRight ,float fBottom)
			:left(fLeft)
			,top(fTop)
			,right(fRight)
			,bottom(fBottom){};
		YYRECT(const RECT &rc)
			:left((float)rc.left)
			,top((float)rc.top)
			,right((float)rc.right)
			,bottom((float)rc.bottom){};
		YYRECT(const D2D1_RECT_F & rc)
			:left(rc.left)
			,top(rc.top)
			,right(rc.right)
			,bottom(rc.bottom){};
		operator RECT() const{RECT tmp;tmp.left=(int)left;tmp.top= (int)top;tmp.right = (int) right; tmp.bottom = (int)bottom; return tmp;}
		operator D2D1_RECT_F() const { D2D1_RECT_F tmp;tmp.left = left; tmp.top = top; tmp.right = right; tmp.bottom = bottom; return tmp;}

		void Clear() { left= right=top=bottom =0.0f;}
		float GetWidth() { return right - left;}
		float GetHeight() { return bottom - top;}
		void Join(const YYRECT & rc)
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
		void Normalize() 
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
		void OffSet(float cx,float cy)
		{
			left+=cx;
			right +=cx;
			top+=cy;
			bottom+=cy;
		}
		void Inflate(float cx,float cy)
		{
			left -=cx;
			right +=cx;
			top -=cy;
			bottom +=cy;
		}
		void Deflate(float cx,float cy)
		{
			Inflate(-cx,-cy);
		}
		void Union(const YYRECT &rc)
		{
			left = (std::min)(left,rc.left);
			right = (std::max)(right,rc.right);
			top = (std::min)(top,rc.top);
			bottom = (std::max)(bottom,rc.bottom);
		}
		bool Empty() const { return (right ==0.0f && left ==0.0f && top == 0.0f && bottom == 0.0f);} 
	};
	bool IntersectRect(YYRECT *pDest,const YYRECT &lhs,const YYRECT &rhs);
	struct YYSIZE
	{
		float width;
		float height;
		YYSIZE():width(0.0f),height(0.0f){};
		YYSIZE(float fWidth,float fHeight):width(fWidth),height(fHeight){};
		YYSIZE(int iWidth,int iHeight):width((float)iWidth),height((float)iHeight){};
		YYSIZE(unsigned int uWidth,unsigned int uHeight):width((float)uWidth),height((float)uHeight){};
		YYSIZE(const D2D_SIZE_F &d2dSize):width(d2dSize.width),height(d2dSize.height){};
		operator SIZE() const { SIZE tmp;tmp.cx = (int) width;tmp.cy = (int) height; return tmp;}
		operator D2D_SIZE_F() const { D2D_SIZE_F tmp;tmp.width = width; tmp.height = height;return tmp;}
	};
	struct YYPOINT
	{
		float x;
		float y;
		YYPOINT():x(0.0f),y(0.0f){};
		YYPOINT(float fx,float fy):x(fx),y(fy){};
		YYPOINT(int ix,int iy):x(float(ix)),y((float)iy){};
		YYPOINT(unsigned int ix,unsigned int iy):x(float(ix)),y((float)iy){};
		YYPOINT(const D2D_POINT_2F &d2dpoint):x(d2dpoint.x),y(d2dpoint.y){};
		YYPOINT(const POINT& pt ) : x(float(pt.x)),y(float(pt.y)){};
		operator POINT() const { POINT tmp; tmp.x = (LONG) x;tmp.y = (LONG)y; return tmp;}
		operator D2D_POINT_2F () const { D2D_POINT_2F tmp;tmp.x = x; tmp.y =y;return tmp;}
	};
	bool PtInRect(const YYRECT &rc,const YYPOINT &pt);

	struct RelativePosUI
	{
		bool                            m_bRelative;
		YYSIZE                            m_szParent;
		float                             m_nMoveXPercent;
		float                             m_nMoveYPercent;
		float                             m_nZoomXPercent;
		float                             m_nZoomYPercent;
	}; 
}