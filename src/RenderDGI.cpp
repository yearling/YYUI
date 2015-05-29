#include "RenderDGI.h"

extern "C"
{
    extern unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, \
        int *comp, int req_comp);
    extern void     stbi_image_free(void *retval_from_stbi_load);

};

namespace YUI
{

    static COLORREF PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
    {
        return RGB (GetRValue (clrSrc) * src_darken + GetRValue (clrDest) * dest_darken, 
            GetGValue (clrSrc) * src_darken + GetGValue (clrDest) * dest_darken, 
            GetBValue (clrSrc) * src_darken + GetBValue (clrDest) * dest_darken);

    }

    static BOOL WINAPI AlphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC, \
        int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
    {
        HDC hTempDC = ::CreateCompatibleDC(hDC);
        if (NULL == hTempDC)
            return FALSE;

        //Creates Source DIB
        LPBITMAPINFO lpbiSrc = NULL;
        // Fill in the BITMAPINFOHEADER
        lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
        if (lpbiSrc == NULL)
        {
            ::DeleteDC(hTempDC);
            return FALSE;
        }
        lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        lpbiSrc->bmiHeader.biWidth = dwWidth;
        lpbiSrc->bmiHeader.biHeight = dwHeight;
        lpbiSrc->bmiHeader.biPlanes = 1;
        lpbiSrc->bmiHeader.biBitCount = 32;
        lpbiSrc->bmiHeader.biCompression = BI_RGB;
        lpbiSrc->bmiHeader.biSizeImage = dwWidth * dwHeight;
        lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
        lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
        lpbiSrc->bmiHeader.biClrUsed = 0;
        lpbiSrc->bmiHeader.biClrImportant = 0;

        COLORREF* pSrcBits = NULL;
        HBITMAP hSrcDib = CreateDIBSection (
            hSrcDC, lpbiSrc, DIB_RGB_COLORS, (void **)&pSrcBits,
            NULL, NULL);

        if ((NULL == hSrcDib) || (NULL == pSrcBits)) 
        {
            delete [] lpbiSrc;
            ::DeleteDC(hTempDC);
            return FALSE;
        }

        HBITMAP hOldTempBmp = (HBITMAP)::SelectObject (hTempDC, hSrcDib);
        ::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
        ::SelectObject (hTempDC, hOldTempBmp);

        //Creates Destination DIB
        LPBITMAPINFO lpbiDest = NULL;
        // Fill in the BITMAPINFOHEADER
        lpbiDest = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
        if (lpbiDest == NULL)
        {
            delete [] lpbiSrc;
            ::DeleteObject(hSrcDib);
            ::DeleteDC(hTempDC);
            return FALSE;
        }

        lpbiDest->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        lpbiDest->bmiHeader.biWidth = dwWidth;
        lpbiDest->bmiHeader.biHeight = dwHeight;
        lpbiDest->bmiHeader.biPlanes = 1;
        lpbiDest->bmiHeader.biBitCount = 32;
        lpbiDest->bmiHeader.biCompression = BI_RGB;
        lpbiDest->bmiHeader.biSizeImage = dwWidth * dwHeight;
        lpbiDest->bmiHeader.biXPelsPerMeter = 0;
        lpbiDest->bmiHeader.biYPelsPerMeter = 0;
        lpbiDest->bmiHeader.biClrUsed = 0;
        lpbiDest->bmiHeader.biClrImportant = 0;

        COLORREF* pDestBits = NULL;
        HBITMAP hDestDib = CreateDIBSection (
            hDC, lpbiDest, DIB_RGB_COLORS, (void **)&pDestBits,
            NULL, NULL);

        if ((NULL == hDestDib) || (NULL == pDestBits))
        {
            delete [] lpbiSrc;
            ::DeleteObject(hSrcDib);
            ::DeleteDC(hTempDC);
            return FALSE;
        }

        ::SelectObject (hTempDC, hDestDib);
        ::BitBlt (hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
        ::SelectObject (hTempDC, hOldTempBmp);

        double src_darken;
        BYTE nAlpha;

        for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
        {
            nAlpha = LOBYTE(*pSrcBits >> 24);
            src_darken = (double) (nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
            if( src_darken < 0.0 ) src_darken = 0.0;
            *pDestBits = PixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
        } //for

        ::SelectObject (hTempDC, hDestDib);
        ::BitBlt (hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
        ::SelectObject (hTempDC, hOldTempBmp);

        delete [] lpbiDest;
        ::DeleteObject(hDestDib);

        delete [] lpbiSrc;
        ::DeleteObject(hSrcDib);

        ::DeleteDC(hTempDC);
        return TRUE;
    }

   

   std::shared_ptr<ImageInfo> RenderGDI::LoadImage(const YString& bitmap, LPCTSTR type /*= NULL*/, DWORD mask /*= 0*/)
    {
        LPBYTE pData = NULL;
        DWORD dwSize = 0;

        do 
        {
            if( type == NULL ) {
                YString sFile = PaintManagerUI::GetResourcePath();
                if( PaintManagerUI::GetResourceZip().empty() ) {
                    sFile += bitmap;
                    HANDLE hFile = ::CreateFile(sFile.c_str(), 
                                                GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
                                                FILE_ATTRIBUTE_NORMAL, NULL);
                    if( hFile == INVALID_HANDLE_VALUE )
                        break;
                    dwSize = ::GetFileSize(hFile, NULL);
                    if( dwSize == 0 ) break;

                    DWORD dwRead = 0;
                    pData = new BYTE[ dwSize ];
                    ::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
                    ::CloseHandle( hFile );

                    if( dwRead != dwSize ) {
                        delete[] pData;
                        pData = NULL;
                        break;
                    }
                }
                else {
                    return NULL;
#if 0
                    sFile += PaintManagerUI::GetResourceZip();
                    HZIP hz = NULL;
                    if( CPaintManagerUI::IsCachedResourceZip() ) hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
                    else hz = OpenZip((void*)sFile.GetData(), 0, 2);
                    if( hz == NULL ) break;
                    ZIPENTRY ze; 
                    int i; 
                    if( FindZipItem(hz, bitmap.m_lpstr, true, &i, &ze) != 0 ) break;
                    dwSize = ze.unc_size;
                    if( dwSize == 0 ) break;
                    pData = new BYTE[ dwSize ];
                    int res = UnzipItem(hz, i, pData, dwSize, 3);
                    if( res != 0x00000000 && res != 0x00000600) {
                        delete[] pData;
                        pData = NULL;
                        if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
                        break;
                    }
                    if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
#endif
                }
            }
            else {
                HRSRC hResource = ::FindResource(PaintManagerUI::GetResourceDll(), bitmap.c_str(), type);
                if( hResource == NULL ) break;
                HGLOBAL hGlobal = ::LoadResource(PaintManagerUI::GetResourceDll(), hResource);
                if( hGlobal == NULL ) {
                    FreeResource(hResource);
                    break;
                }

                dwSize = ::SizeofResource(PaintManagerUI::GetResourceDll(), hResource);
                if( dwSize == 0 ) break;
                pData = new BYTE[ dwSize ];
                ::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
                ::FreeResource(hResource);
            }
        } while (0);

        while (!pData)
        {
            //读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
            HANDLE hFile = ::CreateFile(bitmap.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
                FILE_ATTRIBUTE_NORMAL, NULL);
            if( hFile == INVALID_HANDLE_VALUE ) break;
            dwSize = ::GetFileSize(hFile, NULL);
            if( dwSize == 0 ) break;

            DWORD dwRead = 0;
            pData = new BYTE[ dwSize ];
            ::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
            ::CloseHandle( hFile );

            if( dwRead != dwSize ) {
                delete[] pData;
                pData = NULL;
            }
            break;
        }
        if (!pData)
        {
            //::MessageBox(0, _T("读取图片数据失败！"), _T("抓BUG"), MB_OK);
            return NULL;
        }

        LPBYTE pImage = NULL;
        int x,y,n;
        pImage = stbi_load_from_memory(pData, dwSize, &x, &y, &n, 4);
        delete[] pData;
        if( !pImage ) {
            //::MessageBox(0, _T("解析图片失败"), _T("抓BUG"), MB_OK);
            return NULL;
        }

        BITMAPINFO bmi;
        ::ZeroMemory(&bmi, sizeof(BITMAPINFO));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = x;
        bmi.bmiHeader.biHeight = -y;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = x * y * 4;

        bool bAlphaChannel = false;
        LPBYTE pDest = NULL;
        HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
        if( !hBitmap ) {
            //::MessageBox(0, _T("CreateDIBSection失败"), _T("抓BUG"), MB_OK);
            return NULL;
        }

        for( int i = 0; i < x * y; i++ ) 
        {
            pDest[i*4 + 3] = pImage[i*4 + 3];
            if( pDest[i*4 + 3] < 255 )
            {
                pDest[i*4] = (BYTE)(DWORD(pImage[i*4 + 2])*pImage[i*4 + 3]/255);
                pDest[i*4 + 1] = (BYTE)(DWORD(pImage[i*4 + 1])*pImage[i*4 + 3]/255);
                pDest[i*4 + 2] = (BYTE)(DWORD(pImage[i*4])*pImage[i*4 + 3]/255); 
                bAlphaChannel = true;
            }
            else
            {
                pDest[i*4] = pImage[i*4 + 2];
                pDest[i*4 + 1] = pImage[i*4 + 1];
                pDest[i*4 + 2] = pImage[i*4]; 
            }

            if( *(DWORD*)(&pDest[i*4]) == mask ) {
                pDest[i*4] = (BYTE)0;
                pDest[i*4 + 1] = (BYTE)0;
                pDest[i*4 + 2] = (BYTE)0; 
                pDest[i*4 + 3] = (BYTE)0;
                bAlphaChannel = true;
            }
        }

        stbi_image_free(pImage);

        auto data = std::shared_ptr<ImageInfo>(new ImageInfo,&ImageInfo::ImageInfoDeleter);
        data->m_hBitmap = hBitmap;
        data->m_nX = x;
        data->m_nY = y;
        data->m_alphaChannel = bAlphaChannel;
        return data;
    }

  

    void RenderGDI::DrawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint, const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, BYTE uFade /*= 255*/, bool hole /*= false*/, bool xtiled /*= false*/, bool ytiled /*= false*/)
    {
        assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

        typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
        static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");

        if( lpAlphaBlend == NULL ) lpAlphaBlend = AlphaBitBlt;
        if( hBitmap == NULL ) return;

        HDC hCloneDC = ::CreateCompatibleDC(hDC);
        HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
        ::SetStretchBltMode(hDC, HALFTONE);

        RECT rcTemp = {0};
        RECT rcDest = {0};
        if( lpAlphaBlend && (alphaChannel || uFade < 255) ) {
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };
            // middle
            if( !hole ) {
                rcDest.left = rc.left + rcCorners.left;
                rcDest.top = rc.top + rcCorners.top;
                rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
                rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    if( !xtiled && !ytiled ) {
                        rcDest.right -= rcDest.left;
                        rcDest.bottom -= rcDest.top;
                        lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                            rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                            rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
                            rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
                    }
                    else if( xtiled && ytiled ) {
                        LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                        LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                        int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                        int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                        for( int j = 0; j < iTimesY; ++j ) {
                            LONG lDestTop = rcDest.top + lHeight * j;
                            LONG lDestBottom = rcDest.top + lHeight * (j + 1);
                            LONG lDrawHeight = lHeight;
                            if( lDestBottom > rcDest.bottom ) {
                                lDrawHeight -= lDestBottom - rcDest.bottom;
                                lDestBottom = rcDest.bottom;
                            }
                            for( int i = 0; i < iTimesX; ++i ) {
                                LONG lDestLeft = rcDest.left + lWidth * i;
                                LONG lDestRight = rcDest.left + lWidth * (i + 1);
                                LONG lDrawWidth = lWidth;
                                if( lDestRight > rcDest.right ) {
                                    lDrawWidth -= lDestRight - rcDest.right;
                                    lDestRight = rcDest.right;
                                }
                                lpAlphaBlend(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, 
                                    lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, 
                                    rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth, lDrawHeight, bf);
                            }
                        }
                    }
                    else if( xtiled ) {
                        LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                        int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                        for( int i = 0; i < iTimes; ++i ) {
                            LONG lDestLeft = rcDest.left + lWidth * i;
                            LONG lDestRight = rcDest.left + lWidth * (i + 1);
                            LONG lDrawWidth = lWidth;
                            if( lDestRight > rcDest.right ) {
                                lDrawWidth -= lDestRight - rcDest.right;
                                lDestRight = rcDest.right;
                            }
                            lpAlphaBlend(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
                                hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                                lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
                        }
                    }
                    else { // ytiled
                        LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                        int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                        for( int i = 0; i < iTimes; ++i ) {
                            LONG lDestTop = rcDest.top + lHeight * i;
                            LONG lDestBottom = rcDest.top + lHeight * (i + 1);
                            LONG lDrawHeight = lHeight;
                            if( lDestBottom > rcDest.bottom ) {
                                lDrawHeight -= lDestBottom - rcDest.bottom;
                                lDestBottom = rcDest.bottom;
                            }
                            lpAlphaBlend(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
                                hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                                rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, bf);                    
                        }
                    }
                }
            }

            // left-top
            if( rcCorners.left > 0 && rcCorners.top > 0 ) {
                rcDest.left = rc.left;
                rcDest.top = rc.top;
                rcDest.right = rcCorners.left;
                rcDest.bottom = rcCorners.top;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, bf);
                }
            }
            // top
            if( rcCorners.top > 0 ) {
                rcDest.left = rc.left + rcCorners.left;
                rcDest.top = rc.top;
                rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
                rcDest.bottom = rcCorners.top;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
                        rcCorners.left - rcCorners.right, rcCorners.top, bf);
                }
            }
            // right-top
            if( rcCorners.right > 0 && rcCorners.top > 0 ) {
                rcDest.left = rc.right - rcCorners.right;
                rcDest.top = rc.top;
                rcDest.right = rcCorners.right;
                rcDest.bottom = rcCorners.top;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, bf);
                }
            }
            // left
            if( rcCorners.left > 0 ) {
                rcDest.left = rc.left;
                rcDest.top = rc.top + rcCorners.top;
                rcDest.right = rcCorners.left;
                rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
                        rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
                }
            }
            // right
            if( rcCorners.right > 0 ) {
                rcDest.left = rc.right - rcCorners.right;
                rcDest.top = rc.top + rcCorners.top;
                rcDest.right = rcCorners.right;
                rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
                        rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
                }
            }
            // left-bottom
            if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
                rcDest.left = rc.left;
                rcDest.top = rc.bottom - rcCorners.bottom;
                rcDest.right = rcCorners.left;
                rcDest.bottom = rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, bf);
                }
            }
            // bottom
            if( rcCorners.bottom > 0 ) {
                rcDest.left = rc.left + rcCorners.left;
                rcDest.top = rc.bottom - rcCorners.bottom;
                rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
                rcDest.bottom = rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
                        rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, bf);
                }
            }
            // right-bottom
            if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
                rcDest.left = rc.right - rcCorners.right;
                rcDest.top = rc.bottom - rcCorners.bottom;
                rcDest.right = rcCorners.right;
                rcDest.bottom = rcCorners.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;
                if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                        rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
                        rcCorners.bottom, bf);
                }
            }
        }
        else 
        {
            if (rc.right - rc.left == rcBmpPart.right - rcBmpPart.left \
                && rc.bottom - rc.top == rcBmpPart.bottom - rcBmpPart.top \
                && rcCorners.left == 0 && rcCorners.right == 0 && rcCorners.top == 0 && rcCorners.bottom == 0)
            {
                if( ::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
                    ::BitBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, \
                        hCloneDC, rcBmpPart.left + rcTemp.left - rc.left, rcBmpPart.top + rcTemp.top - rc.top, SRCCOPY);
                }
            }
            else
            {
                // middle
                if( !hole ) {
                    rcDest.left = rc.left + rcCorners.left;
                    rcDest.top = rc.top + rcCorners.top;
                    rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
                    rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
                    rcDest.right += rcDest.left;
                    rcDest.bottom += rcDest.top;
                    if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                        if( !xtiled && !ytiled ) {
                            rcDest.right -= rcDest.left;
                            rcDest.bottom -= rcDest.top;
                            ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                                rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                                rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
                                rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
                        }
                        else if( xtiled && ytiled ) {
                            LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                            LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                            int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                            int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                            for( int j = 0; j < iTimesY; ++j ) {
                                LONG lDestTop = rcDest.top + lHeight * j;
                                LONG lDestBottom = rcDest.top + lHeight * (j + 1);
                                LONG lDrawHeight = lHeight;
                                if( lDestBottom > rcDest.bottom ) {
                                    lDrawHeight -= lDestBottom - rcDest.bottom;
                                    lDestBottom = rcDest.bottom;
                                }
                                for( int i = 0; i < iTimesX; ++i ) {
                                    LONG lDestLeft = rcDest.left + lWidth * i;
                                    LONG lDestRight = rcDest.left + lWidth * (i + 1);
                                    LONG lDrawWidth = lWidth;
                                    if( lDestRight > rcDest.right ) {
                                        lDrawWidth -= lDestRight - rcDest.right;
                                        lDestRight = rcDest.right;
                                    }
                                    ::BitBlt(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, \
                                        lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, \
                                        rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, SRCCOPY);
                                }
                            }
                        }
                        else if( xtiled ) {
                            LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
                            int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                            for( int i = 0; i < iTimes; ++i ) {
                                LONG lDestLeft = rcDest.left + lWidth * i;
                                LONG lDestRight = rcDest.left + lWidth * (i + 1);
                                LONG lDrawWidth = lWidth;
                                if( lDestRight > rcDest.right ) {
                                    lDrawWidth -= lDestRight - rcDest.right;
                                    lDestRight = rcDest.right;
                                }
                                ::StretchBlt(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
                                    hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                                    lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
                            }
                        }
                        else { // ytiled
                            LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
                            int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                            for( int i = 0; i < iTimes; ++i ) {
                                LONG lDestTop = rcDest.top + lHeight * i;
                                LONG lDestBottom = rcDest.top + lHeight * (i + 1);
                                LONG lDrawHeight = lHeight;
                                if( lDestBottom > rcDest.bottom ) {
                                    lDrawHeight -= lDestBottom - rcDest.bottom;
                                    lDestBottom = rcDest.bottom;
                                }
                                ::StretchBlt(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
                                    hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
                                    rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, SRCCOPY);                    
                            }
                        }
                    }
                }

                // left-top
                if( rcCorners.left > 0 && rcCorners.top > 0 ) {
                    rcDest.left = rc.left;
                    rcDest.top = rc.top;
                    rcDest.right = rcCorners.left;
                    rcDest.bottom = rcCorners.top;
                    rcDest.right += rcDest.left;
                    rcDest.bottom += rcDest.top;
                    if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                        rcDest.right -= rcDest.left;
                        rcDest.bottom -= rcDest.top;
                        ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                            rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, SRCCOPY);
                    }
                }
                // top
                if( rcCorners.top > 0 ) {
                    rcDest.left = rc.left + rcCorners.left;
                    rcDest.top = rc.top;
                    rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
                    rcDest.bottom = rcCorners.top;
                    rcDest.right += rcDest.left;
                    rcDest.bottom += rcDest.top;
                    if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                        rcDest.right -= rcDest.left;
                        rcDest.bottom -= rcDest.top;
                        ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                            rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
                            rcCorners.left - rcCorners.right, rcCorners.top, SRCCOPY);
                    }
                }
                // right-top
                if( rcCorners.right > 0 && rcCorners.top > 0 ) {
                    rcDest.left = rc.right - rcCorners.right;
                    rcDest.top = rc.top;
                    rcDest.right = rcCorners.right;
                    rcDest.bottom = rcCorners.top;
                    rcDest.right += rcDest.left;
                    rcDest.bottom += rcDest.top;
                    if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                        rcDest.right -= rcDest.left;
                        rcDest.bottom -= rcDest.top;
                        ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                            rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, SRCCOPY);
                    }
                }
                // left
                if( rcCorners.left > 0 ) {
                    rcDest.left = rc.left;
                    rcDest.top = rc.top + rcCorners.top;
                    rcDest.right = rcCorners.left;
                    rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
                    rcDest.right += rcDest.left;
                    rcDest.bottom += rcDest.top;
                    if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                        rcDest.right -= rcDest.left;
                        rcDest.bottom -= rcDest.top;
                        ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                            rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
                            rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
                    }
                }
                // right
                if( rcCorners.right > 0 ) {
                    rcDest.left = rc.right - rcCorners.right;
                    rcDest.top = rc.top + rcCorners.top;
                    rcDest.right = rcCorners.right;
                    rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
                    rcDest.right += rcDest.left;
                    rcDest.bottom += rcDest.top;
                    if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                        rcDest.right -= rcDest.left;
                        rcDest.bottom -= rcDest.top;
                        ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                            rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
                            rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
                    }
                }
                // left-bottom
                if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
                    rcDest.left = rc.left;
                    rcDest.top = rc.bottom - rcCorners.bottom;
                    rcDest.right = rcCorners.left;
                    rcDest.bottom = rcCorners.bottom;
                    rcDest.right += rcDest.left;
                    rcDest.bottom += rcDest.top;
                    if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                        rcDest.right -= rcDest.left;
                        rcDest.bottom -= rcDest.top;
                        ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                            rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, SRCCOPY);
                    }
                }
                // bottom
                if( rcCorners.bottom > 0 ) {
                    rcDest.left = rc.left + rcCorners.left;
                    rcDest.top = rc.bottom - rcCorners.bottom;
                    rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
                    rcDest.bottom = rcCorners.bottom;
                    rcDest.right += rcDest.left;
                    rcDest.bottom += rcDest.top;
                    if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                        rcDest.right -= rcDest.left;
                        rcDest.bottom -= rcDest.top;
                        ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                            rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
                            rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, SRCCOPY);
                    }
                }
                // right-bottom
                if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
                    rcDest.left = rc.right - rcCorners.right;
                    rcDest.top = rc.bottom - rcCorners.bottom;
                    rcDest.right = rcCorners.right;
                    rcDest.bottom = rcCorners.bottom;
                    rcDest.right += rcDest.left;
                    rcDest.bottom += rcDest.top;
                    if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
                        rcDest.right -= rcDest.left;
                        rcDest.bottom -= rcDest.top;
                        ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                            rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
                            rcCorners.bottom, SRCCOPY);
                    }
                }
            }
        }

        ::SelectObject(hCloneDC, hOldBitmap);
        ::DeleteDC(hCloneDC);
    }

    static bool DrawImage(HDC hDC, std::shared_ptr<PaintManagerUI>&pManager, 
        const RECT& rc, const RECT& rcPaint, const YString& sImageName, 
        const YString& sImageResType, RECT rcItem, RECT rcBmpPart, RECT rcCorner, DWORD dwMask, BYTE bFade, 
        bool bHole, bool bTiledX, bool bTiledY)
    {
        if (sImageName.empty()) {
            return false;
        }
        std::shared_ptr<const ImageInfo> data;
        if( sImageResType.empty() ) {
            data = pManager->GetImageEx(sImageName, NULL, dwMask);
        }
        else {
            assert(0 && "!!fixme ");
            data = pManager->GetImageEx(sImageName, sImageResType.c_str(), dwMask);
        }
        if( !data ) return false;    

        if( rcBmpPart.left == 0 && rcBmpPart.right == 0 && rcBmpPart.top == 0 && rcBmpPart.bottom == 0 ) {
            rcBmpPart.right = data->m_nX;
            rcBmpPart.bottom = data->m_nY;
        }
        if (rcBmpPart.right > data->m_nX) rcBmpPart.right = data->m_nX;
        if (rcBmpPart.bottom > data->m_nY) rcBmpPart.bottom = data->m_nY;

        RECT rcTemp;
        if( !::IntersectRect(&rcTemp, &rcItem, &rc) ) return true;
        if( !::IntersectRect(&rcTemp, &rcItem, &rcPaint) ) return true;

        RenderGDI::DrawImage(hDC, data->m_hBitmap, rcItem, rcPaint, rcBmpPart, rcCorner, data->m_alphaChannel, bFade, bHole, bTiledX, bTiledY);

        return true;
    }
 bool RenderGDI::DrawImageString(HDC hDC, std::shared_ptr<PaintManagerUI>& pManager, const RECT& rcItems, const RECT& rcPaint, const YString & StrImage, const YString & pStrModify /*= _T("")*/)
    {
        if ((pManager == NULL) || (hDC == NULL)) return false;

        // 1、aaa.jpg
        // 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
        // mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'

        YString sImageName = StrImage;
        YString sImageResType;
        RECT rcItem = rcItems;
        RECT rcBmpPart = {0};
        RECT rcCorner = {0};
        DWORD dwMask = 0;
        BYTE bFade = 0xFF;
        bool bHole = false;
        bool bTiledX = false;
        bool bTiledY = false;
        LPCTSTR pStrImage = StrImage.c_str();
        int image_count = 0;

        YString sItem;
        YString sValue;
        LPTSTR pstr = NULL;

        for( int i = 0; i < 2; ++i,image_count = 0 ) {
            if( i == 1)
                pStrImage = pStrModify.c_str();

            if( !pStrImage ) continue;

            while( *pStrImage != _T('\0') ) {
                sItem.clear();
                sValue.clear();
                while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
                while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
                    LPTSTR pstrTemp = ::CharNext(pStrImage);
                    while( pStrImage < pstrTemp) {
                        sItem += *pStrImage++;
                    }
                }
                while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
                if( *pStrImage++ != _T('=') ) break;
                while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
                if( *pStrImage++ != _T('\'') ) break;
                while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
                    LPTSTR pstrTemp = ::CharNext(pStrImage);
                    while( pStrImage < pstrTemp) {
                        sValue += *pStrImage++;
                    }
                }
                if( *pStrImage++ != _T('\'') ) break;
                if( !sValue.empty() ) {
                    if( sItem == _T("file") || sItem == _T("res") ) {
                        if( image_count > 0 )
                            YUI::DrawImage(hDC, pManager, rcItems, rcPaint, sImageName, sImageResType,
                            rcItem, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);

                        sImageName = sValue;
                        if( sItem == _T("file") )
                            ++image_count;
                    }
                    else if( sItem == _T("restype") ) {
                        if( image_count > 0 )
                            YUI::DrawImage(hDC, pManager, rcItems, rcPaint, sImageName, sImageResType,
                            rcItem, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);

                        sImageResType = sValue;
                        ++image_count;
                    }
                    else if( sItem == _T("dest") ) {
                        rcItem.left = rcItems.left + _tcstol(sValue.c_str(), &pstr, 10);  assert(pstr);    
                        rcItem.top = rcItems.top + _tcstol(pstr + 1, &pstr, 10);    assert(pstr);
                        rcItem.right = rcItems.left + _tcstol(pstr + 1, &pstr, 10);  assert(pstr);
                        if (rcItem.right > rcItems.right) rcItem.right = rcItems.right;
                        rcItem.bottom = rcItems.top + _tcstol(pstr + 1, &pstr, 10); assert(pstr);
                        if (rcItem.bottom > rcItems.bottom) rcItem.bottom = rcItems.bottom;
                    }
                    else if( sItem == _T("source") ) {
                        rcBmpPart.left = _tcstol(sValue.c_str(), &pstr, 10);  assert(pstr);    
                        rcBmpPart.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
                        rcBmpPart.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
                        rcBmpPart.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);  
                    }
                    else if( sItem == _T("corner") ) {
                        rcCorner.left = _tcstol(sValue.c_str(), &pstr, 10);  assert(pstr);    
                        rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
                        rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
                        rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
                    }
                    else if( sItem == _T("mask") ) {
                        if( sValue[0] == _T('#')) dwMask = _tcstoul(sValue.c_str() + 1, &pstr, 16);
                        else dwMask = _tcstoul(sValue.c_str(), &pstr, 16);
                    }
                    else if( sItem == _T("fade") ) {
                        bFade = (BYTE)_tcstoul(sValue.c_str(), &pstr, 10);
                    }
                    else if( sItem == _T("hole") ) {
                        bHole = (_tcscmp(sValue.c_str(), _T("true")) == 0);
                    }
                    else if( sItem == _T("xtiled") ) {
                        bTiledX = (_tcscmp(sValue.c_str(), _T("true")) == 0);
                    }
                    else if( sItem == _T("ytiled") ) {
                        bTiledY = (_tcscmp(sValue.c_str(), _T("true")) == 0);
                    }
                }
                if( *pStrImage++ != _T(' ') ) break;
            }
        }

        YUI::DrawImage(hDC, pManager, rcItems, rcPaint, sImageName, sImageResType,
            rcItem, rcBmpPart, rcCorner, dwMask, bFade, bHole, bTiledX, bTiledY);

        return true;
    }
    void RenderGDI::DrawColor(HDC hDC, const RECT& rc, DWORD color)
    {
        if( color <= 0x00FFFFFF ) return;
        if( color >= 0xFF000000 )
        {
            ::SetBkColor(hDC, RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
            ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
        }
        else
        {
            // Create a new 32bpp bitmap with room for an alpha channel
            BITMAPINFO bmi = { 0 };
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = 1;
            bmi.bmiHeader.biHeight = 1;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biSizeImage = 1 * 1 * sizeof(DWORD);
            LPDWORD pDest = NULL;
            HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
            if( !hBitmap ) return;

            *pDest = color;

            RECT rcBmpPart = {0, 0, 1, 1};
            RECT rcCorners = {0};
            DrawImage(hDC, hBitmap, rc, rc, rcBmpPart, rcCorners, true, 255);
            ::DeleteObject(hBitmap);
        }
    }

    void RenderGDI::DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps)
    {
        typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
        static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");
        if( lpAlphaBlend == NULL ) lpAlphaBlend = AlphaBitBlt;
        typedef BOOL (WINAPI *PGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
        static PGradientFill lpGradientFill = (PGradientFill) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "GradientFill");

        BYTE bAlpha = (BYTE)(((dwFirst >> 24) + (dwSecond >> 24)) >> 1);
        if( bAlpha == 0 ) return;
        int cx = rc.right - rc.left;
        int cy = rc.bottom - rc.top;
        RECT rcPaint = rc;
        HDC hPaintDC = hDC;
        HBITMAP hPaintBitmap = NULL;
        HBITMAP hOldPaintBitmap = NULL;
        if( bAlpha < 255 ) {
            rcPaint.left = rcPaint.top = 0;
            rcPaint.right = cx;
            rcPaint.bottom = cy;
            hPaintDC = ::CreateCompatibleDC(hDC);
            hPaintBitmap = ::CreateCompatibleBitmap(hDC, cx, cy);
            assert(hPaintDC);
            assert(hPaintBitmap);
            hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
        }
        if( lpGradientFill != NULL ) 
        {
            TRIVERTEX triv[2] = 
            {
                { rcPaint.left, rcPaint.top, GetBValue(dwFirst) << 8, GetGValue(dwFirst) << 8, GetRValue(dwFirst) << 8, 0xFF00 },
                { rcPaint.right, rcPaint.bottom, GetBValue(dwSecond) << 8, GetGValue(dwSecond) << 8, GetRValue(dwSecond) << 8, 0xFF00 }
            };
            GRADIENT_RECT grc = { 0, 1 };
            lpGradientFill(hPaintDC, triv, 2, &grc, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
        }
        else 
        {
            // Determine how many shades
            int nShift = 1;
            if( nSteps >= 64 ) nShift = 6;
            else if( nSteps >= 32 ) nShift = 5;
            else if( nSteps >= 16 ) nShift = 4;
            else if( nSteps >= 8 ) nShift = 3;
            else if( nSteps >= 4 ) nShift = 2;
            int nLines = 1 << nShift;
            for( int i = 0; i < nLines; i++ ) {
                // Do a little alpha blending
                BYTE bR = (BYTE) ((GetBValue(dwSecond) * (nLines - i) + GetBValue(dwFirst) * i) >> nShift);
                BYTE bG = (BYTE) ((GetGValue(dwSecond) * (nLines - i) + GetGValue(dwFirst) * i) >> nShift);
                BYTE bB = (BYTE) ((GetRValue(dwSecond) * (nLines - i) + GetRValue(dwFirst) * i) >> nShift);
                // ... then paint with the resulting color
                HBRUSH hBrush = ::CreateSolidBrush(RGB(bR,bG,bB));
                RECT r2 = rcPaint;
                if( bVertical ) {
                    r2.bottom = rc.bottom - ((i * (rc.bottom - rc.top)) >> nShift);
                    r2.top = rc.bottom - (((i + 1) * (rc.bottom - rc.top)) >> nShift);
                    if( (r2.bottom - r2.top) > 0 ) ::FillRect(hDC, &r2, hBrush);
                }
                else {
                    r2.left = rc.right - (((i + 1) * (rc.right - rc.left)) >> nShift);
                    r2.right = rc.right - ((i * (rc.right - rc.left)) >> nShift);
                    if( (r2.right - r2.left) > 0 ) ::FillRect(hPaintDC, &r2, hBrush);
                }
                ::DeleteObject(hBrush);
            }
        }
        if( bAlpha < 255 ) {
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, bAlpha, AC_SRC_ALPHA };
            lpAlphaBlend(hDC, rc.left, rc.top, cx, cy, hPaintDC, 0, 0, cx, cy, bf);
            ::SelectObject(hPaintDC, hOldPaintBitmap);
            ::DeleteObject(hPaintBitmap);
            ::DeleteDC(hPaintDC);
        }
    }

    void RenderGDI::DrawLine(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor, int nStyle /*= PS_SOLID*/)
    {
        assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

        LOGPEN lg;
        lg.lopnColor = RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor));
        lg.lopnStyle = nStyle;
        lg.lopnWidth.x = nSize;
        HPEN hPen = CreatePenIndirect(&lg);
        HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
        POINT ptTemp = { 0 };
        ::MoveToEx(hDC, rc.left, rc.top, &ptTemp);
        ::LineTo(hDC, rc.right, rc.bottom);
        ::SelectObject(hDC, hOldPen);
        ::DeleteObject(hPen);
    }

    void RenderGDI::DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor)
    {
        assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
        HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
        HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
        ::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
        ::Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
        ::SelectObject(hDC, hOldPen);
        ::DeleteObject(hPen);
    }

    void RenderGDI::DrawRoundRect(HDC hDC, const RECT& rc, int width, int height, int nSize, DWORD dwPenColor)
    {
        assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
        HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
        HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
        ::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
        ::RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, width, height);
        ::SelectObject(hDC, hOldPen);
        ::DeleteObject(hPen);
    }


    void RenderGDI::DrawText(HDC hDC, std::shared_ptr<PaintManagerUI>& pManager, RECT& rc, const YString& pstrText, DWORD dwTextColor, const YString& strFont, UINT uStyle)
    {
        assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
        if( pstrText.empty() || (!pManager) ) return;
        ::SetBkMode(hDC, TRANSPARENT);
        ::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
        HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(strFont));
        ::DrawText(hDC, pstrText.c_str(), -1, &rc, uStyle | DT_NOPREFIX);
        ::SelectObject(hDC, hOldFont);
    }

    void RenderGDI::DrawHtmlText(HDC hDC, std::shared_ptr<PaintManagerUI>& pManager, RECT& rc, const YString& pstrText, DWORD dwTextColor, RECT* pLinks, YString* sLinks, int& nLinkRects, UINT uStyle)
    {
       
    }

    HBITMAP RenderGDI::GenerateBitmap(std::shared_ptr<PaintManagerUI>& pManager, std::shared_ptr<ControlUI>& pControl, RECT rc)
    {
        int cx = rc.right - rc.left;
        int cy = rc.bottom - rc.top;

        HDC hPaintDC = ::CreateCompatibleDC(pManager->GetPaintDC());
        HBITMAP hPaintBitmap = ::CreateCompatibleBitmap(pManager->GetPaintDC(), rc.right, rc.bottom);
        assert(hPaintDC);
        assert(hPaintBitmap);
        HBITMAP hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
        pControl->DoPaint(hPaintDC, rc);

        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = cx;
        bmi.bmiHeader.biHeight = cy;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = cx * cy * sizeof(DWORD);
        LPDWORD pDest = NULL;
        HDC hCloneDC = ::CreateCompatibleDC(pManager->GetPaintDC());
        HBITMAP hBitmap = ::CreateDIBSection(pManager->GetPaintDC(), &bmi, DIB_RGB_COLORS, (LPVOID*) &pDest, NULL, 0);
        assert(hCloneDC);
        assert(hBitmap);
        if( hBitmap != NULL )
        {
            HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
            ::BitBlt(hCloneDC, 0, 0, cx, cy, hPaintDC, rc.left, rc.top, SRCCOPY);
            ::SelectObject(hCloneDC, hOldBitmap);
            ::DeleteDC(hCloneDC);  
            ::GdiFlush();
        }

        // Cleanup
        ::SelectObject(hPaintDC, hOldPaintBitmap);
        ::DeleteObject(hPaintBitmap);
        ::DeleteDC(hPaintDC);

        return hBitmap;
    }

    SIZE RenderGDI::GetTextSize(HDC hDC, std::shared_ptr<PaintManagerUI>& pManager , const YString& pstrText, const YString& strFont, UINT uStyle)
    {
        SIZE size = {0,0};
        assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
        if( pstrText.empty() || pManager ) return size;
        ::SetBkMode(hDC, TRANSPARENT);
        HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(strFont));
        GetTextExtentPoint32(hDC, pstrText.c_str(), _tcslen(pstrText.c_str()) , &size);
        ::SelectObject(hDC, hOldFont);
        return size;
    }


    void RenderClip::GenerateClip(HDC hDC, RECT rc, RenderClip& clip)
    {
        RECT rcClip = { 0 };
        ::GetClipBox(hDC, &rcClip);
        clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
        clip.hRgn = ::CreateRectRgnIndirect(&rc);
        ::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
        clip.hDC = hDC;
        clip.rcItem = rc;
    }

    void RenderClip::GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, RenderClip& clip)
    {
        RECT rcClip = { 0 };
        ::GetClipBox(hDC, &rcClip);
        clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
        clip.hRgn = ::CreateRectRgnIndirect(&rc);
        HRGN hRgnItem = ::CreateRoundRectRgn(rcItem.left, rcItem.top, rcItem.right + 1, rcItem.bottom + 1, width, height);
        ::CombineRgn(clip.hRgn, clip.hRgn, hRgnItem, RGN_AND);
        ::ExtSelectClipRgn(hDC, clip.hRgn, RGN_AND);
        clip.hDC = hDC;
        clip.rcItem = rc;
        ::DeleteObject(hRgnItem);
    }

    void RenderClip::UseOldClipBegin(HDC hDC, RenderClip& clip)
    {
        ::SelectClipRgn(hDC, clip.hOldRgn);
    }

    void RenderClip::UseOldClipEnd(HDC hDC, RenderClip& clip)
    {
         ::SelectClipRgn(hDC, clip.hRgn);
    }

    RenderClip::RenderClip()
    {

    }

    RenderClip::~RenderClip()
    {
        assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);
        assert(::GetObjectType(hRgn)==OBJ_REGION);
        assert(::GetObjectType(hOldRgn)==OBJ_REGION);
        ::SelectClipRgn(hDC, hOldRgn);
        ::DeleteObject(hOldRgn);
        ::DeleteObject(hRgn);
    }

}



