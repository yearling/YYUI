#pragma once
#include "YUI.h"

namespace YUI
{

    class WindowProperty
    {
    public:
        WindowProperty(void);
        ~WindowProperty(void);
        void                            Init(HWND hWnd) { m_hWnd = hWnd;}
    public:
        HWND                            GetHWND() const { return m_hWnd;}
        YYPOINT                         GetMousePos() const;
        YYSIZE                          GetClientSize() const;
        YYRECT                          GetClientRect() const;
        YYSIZE                          GetInitSize() const;
        void SetInitSize(float cx, float cy);
        YYRECT                          GetSizeBox() const ;
        void                            SetSizeBox(const RECT &rcSizeBox);
        YYRECT                          GetCaptionRect() const ;
        void                            SetCaptionRect( const RECT & rcCaption);
        YYSIZE                          GetMinInfo() const;
        void                            SetMinInfo(float cx, float cy);
        YYSIZE                          GetMaxInfo() const;
        void SetMaxInfo(float cx, float cy);
        float GetTransparent() const;
        void                            SetTransparent(float nOpacity);
        void                            SetBackgroundTransparent(bool bTrans);
        void                            SetDefaultDisabledColor(YYCOLOR color);
        YYCOLOR                         GetDefaultDisabledColor() const;
        void                            SetDefaultFontColor(YYCOLOR color);
        YYCOLOR                         GetDefaultFontColor() const;
        void                            SetDefaultLinkFontColor(YYCOLOR color);
        YYCOLOR                         GetDefaultLinkFontColor() const;
        void                            SetDefaultLinkHoverFontColor(YYCOLOR color);
        YYCOLOR                         GetDefaultLinkHoverFontColor() const;
        void                            SetDefaultSelectedBkColor(YYCOLOR color);
        YYCOLOR                         GetDefaultSelectedBkColor() const;
    private:
        HWND                            m_hWnd;
        YYPOINT                         m_ptLastMousePos;
        YYSIZE                          m_szMinWindow;      //窗口最小值
        YYSIZE                          m_szMaxWindow;
        YYSIZE                          m_szInitWindowSize;
        YYRECT                          m_rcSizeBox;  //SizeBox就是边框指示区，鼠标放上面用来调整窗口大小
        YYSIZE                          m_szRoundCorner;
        YYRECT                          m_rcCaption;  //标题栏大小
        YYCOLOR                         m_DefaultFontColor;
        YYCOLOR                         m_DisableFontColor;
        YYCOLOR                         m_LinkFontColor;
        YYCOLOR                         m_LinkHoverFontColor;
        YYCOLOR                         m_SelectBKColor;
        float                           m_fAlpha;
        bool                            m_bAlphaBackGround;
    };


}