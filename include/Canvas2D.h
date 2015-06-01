#pragma once
#include "YUI.h"
#include "RenderD2D.h"
#include "ClipRegionDef.h"
namespace YUI
{

    class Canvas2D
    {
       
    public:
        Canvas2D(HWND hWnd);
    public:
        bool                            BeginDraw( );
        void                            EndDraw();
        void                            DrawSolidText(const YString& strText,const FontD2D& font,const YYRECT &rc,const YYCOLOR color);
        void                            DrawBitmap(const YString &strBmp,const YYRECT &rc);
        void                            DrawBitmap(const YString &strBmp,const YYRECT &desRC,const YYRECT &srcRC,float alpha=1.0f);
        void                            DrawLine(YYPOINT start,YYPOINT end,const YYCOLOR &brush,float strokewidth=1.0f);
        void                            DrawRect(const YYRECT rc,const YYCOLOR &brush,float strokewidth=1.0f);
        void                            FillRect(const YYRECT rc,const YYCOLOR &brush);
        void                            OnResize(float width,float height);
        void                            Clear(const YYCOLOR & color);
        void                            ClipRect(ClipRegionDef& clip);
        YYSIZE                          GetSize();
        static  void                    Init();
        CComPtr<ID2D1HwndRenderTarget>  GetHWNDRT(HWND hWnd);
        std::shared_ptr<RenderD2D>      GetRenderD2D() const { return m_RenderEngine;}
    private:
        Canvas2D(const Canvas2D &);
        Canvas2D & operator=(const Canvas2D &);
        static std::shared_ptr<RenderD2D>      m_RenderEngine;
        HWND                            m_hWnd;
    };

}