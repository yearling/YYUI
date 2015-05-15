#include "YUI.h"
#include "WindowProperty.h"

namespace YUI
{

    WindowProperty::WindowProperty(void)
        :m_hWnd(NULL)
        ,m_fAlpha(1.0f)
        ,m_bAlphaBackGround(false)
    {
    }


    WindowProperty::~WindowProperty(void)
    {
    }

    YUI::YYPOINT WindowProperty::GetMousePos() const
    {
        return m_ptLastMousePos;
    }

    YUI::YYSIZE WindowProperty::GetClientSize() const
    {
        RECT rcClient = { 0 };
        ::GetClientRect( m_hWnd, &rcClient);
        return YYSIZE(rcClient.right - rcClient.left , rcClient.bottom - rcClient.top);
    }

    YUI::YYSIZE WindowProperty::GetInitSize() const
    {
        return m_szInitWindowSize;
    }

    void WindowProperty::SetInitSize(int cx, int cy)
    {
        m_szInitWindowSize.width = (float)cx;
        m_szInitWindowSize.height = (float)cy;
        ::SetWindowPos(m_hWnd, NULL, 0, 0, cx, cy,SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    }

    YUI::YYRECT WindowProperty::GetSizeBox() const
    {
        return m_rcSizeBox;
    }

    void WindowProperty::SetSizeBox(const RECT &rcSizeBox)
    {
         m_rcSizeBox = rcSizeBox;
    }

    YUI::YYRECT WindowProperty::GetCaptionRect() const
    {
        return m_rcCaption;
    }

    void WindowProperty::SetCaptionRect(const RECT & rcCaption)
    {
        m_rcCaption =rcCaption;
    }

    YUI::YYSIZE WindowProperty::GetMinInfo() const
    {
        return m_szMinWindow;
    }

    void WindowProperty::SetMinInfo(int cx, int cy)
    {
        assert(cx >= 0 && cy >= 0);
        m_szMinWindow.width =(float) cx;
        m_szMinWindow.height =(float) cy;
    }

    YUI::YYSIZE WindowProperty::GetMaxInfo() const
    {
        return m_szMaxWindow;
    }

    void WindowProperty::SetMaxInfo(int cx, int cy)
    {
        assert(cx >= 0 && cy >= 0);
        m_szMaxWindow.width = (float)cx;
        m_szMaxWindow.height = (float)cy;
    }

    int WindowProperty::GetTransparent() const
    {
        return m_fAlpha;
    }

    void WindowProperty::SetTransparent(float nOpacity)
    {
        if( m_fAlpha < 0.0f)
            m_fAlpha = 0.0f;
        else if( m_fAlpha >1.0f)
            m_fAlpha =1.0f;
        int nAlpha = (int)m_fAlpha*255;
        if(m_hWnd == NULL )
            return;
        typedef BOOL (__stdcall *PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
        PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes = NULL;

        HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
        if (hUser32)
        {
            fSetLayeredWindowAttributes = 
                (PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
            if( fSetLayeredWindowAttributes == NULL ) return;
        }

        DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
        DWORD dwNewStyle = dwStyle;
        if( nOpacity >= 0 && nOpacity < 256 ) dwNewStyle |= WS_EX_LAYERED;
        else dwNewStyle &= ~WS_EX_LAYERED;
        if(dwStyle != dwNewStyle) ::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewStyle);
        fSetLayeredWindowAttributes(m_hWnd, 0, nOpacity, LWA_ALPHA);
    }

    void WindowProperty::SetBackgroundTransparent(bool bTrans)
    {
        m_bAlphaBackGround = bTrans;
    }

    YUI::YYRECT WindowProperty::GetClientRect() const
    {
        RECT rcClient = { 0 };
        ::GetClientRect( m_hWnd, &rcClient);
        return rcClient;
    }


    

}