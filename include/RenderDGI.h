#pragma once
#include "YUI.h"
#include "PaintManagerUI.h"

namespace YUI
{
    class RenderClip
    {
    public:
        RenderClip();
        ~RenderClip();
    public:
        RECT rcItem;
        HDC hDC;
        HRGN hRgn;
        HRGN hOldRgn;

        static void GenerateClip(HDC hDC, RECT rc, RenderClip& clip);
        static void GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, RenderClip& clip);
        static void UseOldClipBegin(HDC hDC, RenderClip& clip);
        static void UseOldClipEnd(HDC hDC, RenderClip& clip);
    };
   
    class RenderGDI
    {
    public:
      static bool                       DrawImageString(HDC hDC, 
                                        std::shared_ptr<PaintManagerUI>& pManager, 
                                        const RECT& rcItem, 
                                        const RECT& rcPaint, 
                                        const YString & StrImage, 
                                        const YString & pStrModify = _T(""));

      static std::shared_ptr<ImageInfo> LoadImage(const YString& bitmap, 
                                        LPCTSTR type = NULL, 
                                        DWORD mask = 0);


      static void                       DrawImage(HDC hDC, 
                                        HBITMAP hBitmap, 
                                        const RECT& rc, 
                                        const RECT& rcPaint, 
                                        const RECT& rcBmpPart, 
                                        const RECT& rcCorners, 
                                        bool alphaChannel, 
                                        BYTE uFade = 255, 
                                        bool hole = false, 
                                        bool xtiled = false, 
                                        bool ytiled = false);

      static void                       DrawColor(HDC hDC, 
                                        const RECT& rc, 
                                        DWORD color);

      static void                       DrawGradient(HDC hDC, 
                                        const RECT& rc, 
                                        DWORD dwFirst, 
                                        DWORD dwSecond, 
                                        bool bVertical, 
                                        int nSteps);

      // 以下函数中的颜色参数alpha值无效
      static void                       DrawLine(HDC hDC, 
                                        const RECT& rc, 
                                        int nSize, 
                                        DWORD dwPenColor,
                                        int nStyle = PS_SOLID);

      static void                       DrawRect(HDC hDC, 
                                        const RECT& rc, 
                                        int nSize, 
                                        DWORD dwPenColor);

      static void                       DrawRoundRect(HDC hDC, 
                                        const RECT& rc, 
                                        int width, 
                                        int height, 
                                        int nSize, 
                                        DWORD dwPenColor);

      static void                       DrawText(HDC hDC, 
                                        std::shared_ptr<PaintManagerUI>& pManager, 
                                        RECT& rc, 
                                        const YString& pstrText, 
                                        DWORD dwTextColor, 
                                        const YString& strFont, 
                                        UINT uStyle);

      static void                       DrawHtmlText(HDC hDC, 
                                        std::shared_ptr<PaintManagerUI>& pManager, 
                                        RECT& rc, 
                                        const YString& pstrText, 
                                        DWORD dwTextColor, 
                                        RECT* pLinks, 
                                        YString* sLinks, 
                                        int& nLinkRects, 
                                        UINT uStyle);

      static HBITMAP                    GenerateBitmap(std::shared_ptr<PaintManagerUI>& pManager,                                              std::shared_ptr<ControlUI>& pControl, 
                                        RECT rc);

      static SIZE                       GetTextSize(HDC hDC, 
                                        std::shared_ptr<PaintManagerUI>& pManager , 
                                        const YString& pstrText, 
                                        const YString& strFont,
                                        UINT uStyle);

    };
}