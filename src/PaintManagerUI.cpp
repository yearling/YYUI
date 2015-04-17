//////////////////////////////////////////////////////////////////////////
// Create by yyCom, 2015/4/10
//////////////////////////////////////////////////////////////////////////

#include "YUI.h"
#include "PaintManagerUI.h"
#include "UIUtility.h"
#include <commctrl.h>
#include <WindowsX.h>
#pragma comment(lib,"comctl32.lib")
namespace YUI
{

    void YUI::PaintManagerUI::SetInstance(HINSTANCE hInst)
    {
        g_hInstance = hInst;
    }
    HPEN g_hUpdateRectPen = NULL;

    YUI::PaintManagerUI::PaintManagerUI():
        m_hWndPaint( NULL )
        ,m_hDCPaint( NULL)
        ,m_hDCOffscreen( NULL )
        ,m_hDCBackground( NULL )
        ,m_hbmpOffscreen( NULL )
        ,m_hbmpBackground( NULL )
        ,m_hwndTooltip( NULL )
        ,m_bShowUpdateRect( false )
        ,m_uTimerID( 0x1000 )
        ,m_bFirstLayout( true )
        ,m_bFocusNeeded( false )
        ,m_bUpdateNeeded( false )
        ,m_bMouseTracking( false )
        ,m_bMouseCapture( false )
        ,m_bOffscreenPaint( true )
        ,m_bAlphaBackGround( false )
        ,m_bUsedVirtualWnd( true )
        ,m_nOpacity( 255 )
        ,m_dwDefaultDisabledColor( 0xFFA7A6AA)
        ,m_dwDefaultFontColor( 0xFF000000 )
        ,m_dwDefaultLinkFontColor( 0xFF0000FF )
        ,m_dwDefaultLinkHoverFontColor( 0xFFD3215F )
        ,m_dwDefaultSelectedBKColor( 0xFFBAE4FF )
    {
        LOGFONT lf = { 0 };
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT),sizeof(lf),&lf);
        lf.lfCharSet = DEFAULT_CHARSET;
        if( !PaintManagerUI::g_strDefaultFontName.empty() )
        {
            _tcscpy_s(lf.lfFaceName, LF_FACESIZE, g_strDefaultFontName.c_str());
            HFONT hDefaultFont = ::CreateFontIndirect( &lf );
            m_DefaultFontInfo.m_hFont = hDefaultFont;
            m_DefaultFontInfo.m_strFontName = (lf.lfFaceName);
            m_DefaultFontInfo.m_nSize = -lf.lfHeight;
            m_DefaultFontInfo.m_bBold = (lf.lfWeight >= FW_BOLD );
            m_DefaultFontInfo.m_bUnderline = ( lf.lfUnderline == TRUE );
            m_DefaultFontInfo.m_bItalic = (lf.lfItalic == TRUE );
            memset( &m_DefaultFontInfo.m_tm,0,sizeof(m_DefaultFontInfo.m_tm) );

            if(  g_hUpdateRectPen == NULL )
            {
                //指示哪些地方在被更新，调试用
                g_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
                // Boot Windows Common Controls (for the ToolTip control)
                ::InitCommonControls();
                ::LoadLibrary(_T("msimg32.dll"));
            }

            m_szMinWindow.cx = 0;
            m_szMinWindow.cy = 0;
            m_szMaxWindow.cx = 0;
            m_szMaxWindow.cy = 0;
            m_szInitWindowSize.cx = 0;
            m_szInitWindowSize.cy = 0;
            m_szRoundCorner.cx = m_szRoundCorner.cy = 0;
            ZeroMemory(&m_rcSizeBox, sizeof(m_rcSizeBox));
            ZeroMemory(&m_rcCaption, sizeof(m_rcCaption));
            m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
        }
    }

    YUI::PaintManagerUI::~PaintManagerUI()
    {
        //!! todo
    }


    bool YUI::PaintManagerUI::TranslateMessage(const LPMSG pMsg)
    {
        return false;
    }
    HINSTANCE YUI::PaintManagerUI::g_hInstance = NULL;

    std::vector<void*> YUI::PaintManagerUI::g_vecTranslateAccelerator;

    YString YUI::PaintManagerUI::g_strDefaultFontName;





    short YUI::PaintManagerUI::g_L       = 180;

    short YUI::PaintManagerUI::g_S       = 180;

    short YUI::PaintManagerUI::g_H       = 180;

    HANDLE YUI::PaintManagerUI::g_hResourceZip = NULL;

    bool YUI::PaintManagerUI::g_bCachedResourceZip = false;

    YString YUI::PaintManagerUI::g_strResourceZip;

    YString YUI::PaintManagerUI::g_strResourcePath;

    HINSTANCE YUI::PaintManagerUI::g_hResourceInstance = NULL;

   // std::set<funCreateControl> YPaintManagerUI::g_setPlugins;

    std::vector< std::weak_ptr<PaintManagerUI> > PaintManagerUI::g_vecPreMessages;






    void PaintManagerUI::Init(HWND hWnd)
    {
        assert( ::IsWindow(hWnd ) && "hwnd 不是个有效值");

        m_hWndPaint = hWnd;
        m_hDCPaint = ::GetDC(hWnd);
        g_vecPreMessages.push_back(this->shared_from_this());
    }

    YString PaintManagerUI::GetInstancePath()
    {
        if( g_hInstance == NULL )
            return _T('\0');
        TCHAR szModule[MAX_PATH + 1] = { 0 };
        ::GetModuleFileName(g_hInstance,szModule,MAX_PATH);
        return GetFileDir(YString(szModule));
    }

    YUI::YString PaintManagerUI::GetCurrentPath()
    {
        TCHAR szPath[MAX_PATH +1 ]= { 0 };
        ::GetCurrentDirectory(MAX_PATH,szPath);
        return szPath;
    }

    HINSTANCE PaintManagerUI::GetResourceDll()
    {
        if( g_hResourceInstance == NULL )
            return g_hInstance;
        return g_hResourceInstance;
    }

    const YString & PaintManagerUI::GetSourcePath()
    {
        return g_strResourcePath;
    }

    const YString & PaintManagerUI::GetResourceZip()
    {
        return g_strResourceZip;
    }

    bool PaintManagerUI::IsCachedResourceZip()
    {
        return g_bCachedResourceZip;
    }

    HANDLE PaintManagerUI::GetResourceZipHandle()
    {
        return g_hResourceZip;
    }

   
    void PaintManagerUI::SetCurrentPath(const YString & strPath)
    {
        ::SetCurrentDirectory(strPath.c_str());
    }

    void PaintManagerUI::SetResourceDll(HINSTANCE hInstance)
    {
        g_hResourceInstance = hInstance;
    }

    void PaintManagerUI::SetResourcePath(const YString &strPath)
    {
        g_strResourcePath = strPath;
        if(g_strResourcePath.empty())
            return;
        if(g_strResourcePath[g_strResourcePath.size()-1] == _T('\\') ||
            g_strResourcePath[g_strResourcePath.size()-1] == _T('/') )
            g_strResourcePath+=_T('\\');
    }

    void PaintManagerUI::SetResourceZip(void)
    {
        //!todo 
        return;
    }

    void PaintManagerUI::ReloadSkin()
    {
        for(auto iter = g_vecPreMessages.begin();iter!=g_vecPreMessages.end();++iter)
        {
            auto sp = iter->lock();
#if 0
            if( sp )
                sp->ReloadAllImages();
#endif
        }
    }

    bool PaintManagerUI::LoadPlugin(const YString & strModuleName)
    {
        assert(!strModuleName.empty());
        if(strModuleName.empty())
            return false;
        HMODULE hModule = ::LoadLibrary(strModuleName.c_str());
        if( hModule )
        {
            funCreateControl func = reinterpret_cast<pfnCreateControl>(::GetProcAddress(hModule,"CreateControl"));
            if( func )
            {
               // g_setPlugins.insert(func);
                return true;
            }

        }
        return false;
    }
#if 0
    std::set<funCreateControl> & YPaintManagerUI::GetPlugins()
    {
        return g_setPlugins;
    }
#endif

    HDC PaintManagerUI::GetPaintDC() const
    {
        return m_hDCPaint;
    }

    HWND PaintManagerUI::GetPaintWindow() const
    {
        return m_hWndPaint;
    }

    HWND PaintManagerUI::GetTooltipWindow() const
    {
        return m_hwndTooltip;
    }

    POINT PaintManagerUI::GetMousePos() const
    {
        return m_ptLastMousePos;
    }

    SIZE PaintManagerUI::GetClientSize() const
    {
        RECT rcClient = { 0 };
        ::GetClientRect( m_hWndPaint, &rcClient);
        return YSize(rcClient.right - rcClient.left , rcClient.bottom - rcClient.top);
    }

    SIZE PaintManagerUI::GetInitSize() const
    {
        return m_szInitWindowSize;
    }

    void PaintManagerUI::SetInitSize(int cx, int cy)
    {
        m_szInitWindowSize.cx = cx;
        m_szInitWindowSize.cy = cy;
        if( m_pRoot == NULL && m_hWndPaint != NULL )
            ::SetWindowPos(m_hWndPaint, NULL, 0, 0, cx, cy,SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    }

    const RECT& PaintManagerUI::GetSizeBox() const 
    {
        return m_rcSizeBox;
    }

    void PaintManagerUI::SetSizeBox(const RECT &rcSizeBox)
    {
        m_rcSizeBox = rcSizeBox;
    }

    const RECT& PaintManagerUI::GetCaptionRect() const
    {
        return m_rcCaption;
    }

    void PaintManagerUI::SetCaptionRect(const RECT & rcCaption)
    {
        m_rcCaption =rcCaption;
    }

    SIZE PaintManagerUI::GetRoundCorner() const
    {
        return m_szRoundCorner;
    }

    void PaintManagerUI::SetRoundCorner(int cx, int cy)
    {
        m_szRoundCorner.cx = cx;
        m_szRoundCorner.cy = cy;
    }

    SIZE PaintManagerUI::GetMinInfo() const
    {
        return m_szMinWindow;
    }

    void PaintManagerUI::SetMinInfo(int cx, int cy)
    {
        assert(cx >= 0 && cy >= 0);
        m_szMinWindow.cx = cx;
        m_szMinWindow.cy = cy;
    }

    SIZE PaintManagerUI::GetMaxInfo() const
    {
        return m_szMaxWindow;
    }

    void PaintManagerUI::SetMaxInfo(int cx, int cy)
    {
        assert(cx >= 0 && cy >= 0);
        m_szMaxWindow.cx = cx;
        m_szMaxWindow.cy = cy;
    }

    int PaintManagerUI::GetTransparent() const
    {
        return m_nOpacity;
    }

    void PaintManagerUI::SetTransparent(int nOpacity)
    {
        if( nOpacity < 0)
            nOpacity = 0 ;
        else if(nOpacity > 255)
            nOpacity = 255;
        else
            m_nOpacity = nOpacity;
        if( m_hWndPaint == NULL)
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

        DWORD dwStyle = ::GetWindowLong(m_hWndPaint, GWL_EXSTYLE);
        DWORD dwNewStyle = dwStyle;
        if( nOpacity >= 0 && nOpacity < 256 ) dwNewStyle |= WS_EX_LAYERED;
        else dwNewStyle &= ~WS_EX_LAYERED;
        if(dwStyle != dwNewStyle) ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewStyle);
        fSetLayeredWindowAttributes(m_hWndPaint, 0, nOpacity, LWA_ALPHA);
    }

    void PaintManagerUI::SetBackgroundTransparent(bool bTrans)
    {
        m_bAlphaBackGround = bTrans;
    }

    bool PaintManagerUI::IsShowUpdateRect() const
    {
        return m_bShowUpdateRect;
    }

    void PaintManagerUI::SetShowUpdateRect(bool bShow)
    {
        m_bShowUpdateRect = bShow;
    }

    struct tagFINDSHORTCUT
    {
        TCHAR ch;
        bool bPickNext;
    };

    bool PaintManagerUI::PreMessageHandler(UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT &lRes)
    {
        for( auto filter : m_vecMessageFilters ) 
        {
            bool bHandled = false;
            auto spFilter = std::shared_ptr<IMessageFilterUI>(filter.lock());
            if( ! spFilter )
                return false;
            LRESULT lResult = spFilter->MessageHandler(uMsg,wParam,lParam,bHandled);
            if( bHandled )
                return true;
        }
        switch (uMsg)
        {
        case WM_KEYDOWN:
            {
                if( wParam == VK_TAB )
                {
#if 0
                    auto sp_focus = m_pFocus.lock();
                    if(sp_focus && sp_focus->IsVisible() && sp_focus->IsEnabled() && us && m_pFocus->IsVisible() && m_pFocus->IsEnabled() && _tcsstr(m_pFocus->GetClass(), _T("RichEditUI")) != NULL )
                    {
                        if(sp_focus->IsWantTab() )
                            return false;
                    }
                    SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
#endif
                    return true;
                }

            }
            break;
        case WM_SYSCHAR:
            {
#if 0
                // Handle ALT-shortcut key-combinations
                tagFINDSHORTCUT fs = { 0 };
                fs.ch = toupper((int)wParam);
                auto wp = 
#endif
            }
            break;
        case WM_SYSKEYDOWN:
                {
#if 0
                    auto sp_focus = m_pFocus.lock();
#endif
                }
                break;
        }
            
        return false;
    }

    bool PaintManagerUI::MessageHandler(UINT uMesg, WPARAM wParam, LPARAM lParam, LRESULT &lRes)
    {
        if( m_hWndPaint == NULL )
            return false;
        for(auto iter = m_ListAsyncNotify.begin();
            iter!= m_ListAsyncNotify.end();)
        {
            auto spSend= iter->pSender.lock();
            if(spSend)
            {
                if(spSend->OnNotify)
                    spSend->OnNotify(*iter);
            }
            for( auto notifyer : m_vecNotifiers)
            {
                auto spNotifyer = notifyer.lock();
                if( spNotifyer )
                    spNotifyer->Notify(*iter);
            }
            iter= m_ListAsyncNotify.erase(iter);
        }

        for(auto MessageFilter : m_vecMessageFilters)
        {
            bool bHandle = false;
            auto spFilter= MessageFilter.lock();
            if(spFilter)
            {
                auto lResult = spFilter->MessageHandler(uMesg,wParam,lParam,bHandle);
                if( bHandle )
                    lRes = lResult;
                return true;
            }
        }

        switch ( uMesg )
        {
        case WM_APP + 1:
            {
                //已经使用sp,没必要delay delete
                break;
            }
        case WM_CLOSE:
            {
                ControlEvent eve;
                eve.m_ptMouse = m_ptLastMousePos;
                eve.m_dwTimestamp = ::GetTickCount();
                if( m_pEventHover )
                {
                    eve.m_Type = UIEVENT_MOUSELEAVE;
                    eve.m_pSender = m_pEventHover;
                    m_pEventHover->Event(eve);
                }
                if( m_pEventClick )
                {
                    eve.m_Type = UIEVENT_BUTTONUP;
                    eve.m_pSender = m_pEventClick;
                    m_pEventClick->Event(eve);
                }
                SetFocus(NULL);
                HWND hwndParent = GetWindowOwner( m_hWndPaint );
                if( hwndParent != NULL )
                {
                    ::SetFocus(hwndParent);
                }
            }
            break;
        case WM_ERASEBKGND:
            {
                lRes = 1;
            }
            return true;
        case WM_PAINT:
            {
                RECT rcPaint = { 0 };
                //不需要更新
                if( ! ::GetUpdateRect(m_hWndPaint,&rcPaint,FALSE) )
                     return true;
                if( m_pRoot == NULL)
                {
                    PAINTSTRUCT ps = { 0 };
                    ::BeginPaint(m_hWndPaint, &ps);
                    ::EndPaint(m_hWndPaint,&ps);
                    return true;
                }

                if( m_bUpdateNeeded )
                {
                    m_bUpdateNeeded = false;
                    RECT rcClient = { 0 };
                    ::GetClientRect(m_hWndPaint, &rcClient );
                    if( !::IsRectEmpty(&rcClient) )
                    {
                        if( m_pRoot->IsUpdateNeeded())
                        {
                            m_pRoot->SetPos(rcClient);
                            if( m_hDCOffscreen != NULL )
                                ::DeleteDC(m_hDCOffscreen);
                            if( m_hDCBackground != NULL )
                                ::DeleteDC(m_hDCBackground);
                            if( m_hbmpOffscreen != NULL )
                                ::DeleteObject(m_hbmpOffscreen);
                            if( m_hbmpBackground != NULL )
                                ::DeleteObject(m_hbmpBackground);
                            m_hDCOffscreen = NULL;
                            m_hDCBackground = NULL;
                            m_hbmpBackground = NULL;
                            m_hbmpBackground = NULL;
                        }
                        else
                        {
#if 0 
                            std::weak_ptr<YControlUI> wp;
                            typedef std::function<std::weak_ptr<YControlUI>(std::weak_ptr<YControlUI>) > funcFindControl;
                            funcFindControl func= []()
                            while(wp = m_pRoot->FindControl([]()))
#endif 
                        }
                        if( m_bFirstLayout )
                        {
                            m_bFirstLayout = false;
                           SendNotify(m_pRoot,MSG_WindowInit,0,0,false);
                        }
                    }
                }

                if( m_bFocusNeeded )
                    SetNextTabControl();

                //渲染屏幕
                //准备离线bitmap
                if( m_bOffscreenPaint && m_hbmpOffscreen == NULL )
                {
                    RECT rcClient = { 0 };
                    ::GetClientRect(m_hWndPaint, &rcClient );
                    m_hDCOffscreen = ::CreateCompatibleDC( m_hDCPaint );
                    m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDCPaint, rcClient.right-rcClient.left,rcClient.bottom - rcClient.top );
                    assert( m_hDCOffscreen);
                    assert( m_hbmpOffscreen);
                }
                //begin render
                PAINTSTRUCT ps = { 0 };
                ::BeginPaint(m_hWndPaint,&ps);
                if( m_bOffscreenPaint )
                {
                    HBITMAP hOldBitmap = (HBITMAP )::SelectObject(m_hDCOffscreen,m_hbmpOffscreen);
                    int iSaveDC = ::SaveDC( m_hDCOffscreen );
                    if( m_bAlphaBackGround )
                    {
                        if( m_hbmpBackground == NULL )
                        {
                            RECT rcClient = { 0 };
                            ::GetClientRect( m_hWndPaint, &rcClient );
                            m_hDCBackground = ::CreateCompatibleDC( m_hDCPaint );
                            m_hbmpBackground = ::CreateCompatibleBitmap( m_hDCPaint,rcClient.right-rcClient.left, rcClient.bottom - rcClient.top );
                            ::SelectObject(m_hDCBackground, m_hbmpBackground);
                            ::BitBlt(m_hDCBackground,ps.rcPaint.left,ps.rcPaint.top ,ps.rcPaint.right-ps.rcPaint.left,ps.rcPaint.bottom-ps.rcPaint.top ,ps.hdc,ps.rcPaint.left,ps.rcPaint.top,SRCCOPY);
                        }
                        else
                            ::SelectObject(m_hDCBackground, m_hbmpBackground);
                     ::BitBlt(m_hDCOffscreen,ps.rcPaint.left,ps.rcPaint.top ,ps.rcPaint.right-ps.rcPaint.left,ps.rcPaint.bottom-ps.rcPaint.top ,m_hDCBackground,ps.rcPaint.left,ps.rcPaint.top,SRCCOPY);
                    }
                    m_pRoot->DoPaint(m_hDCOffscreen,ps.rcPaint);
                    for(auto  iter : m_vecPostPaintControls)
                    {
                        auto spControl  = iter.lock();
                        if( spControl )
                        {
                            spControl->DoPostPaint(m_hDCOffscreen,ps.rcPaint);
                        }
                    }
                    ::RestoreDC(m_hDCOffscreen,iSaveDC);
                    ::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
                        ps.rcPaint.bottom - ps.rcPaint.top, m_hDCOffscreen, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
                    if( m_bShowUpdateRect )
                    {
                        HPEN hOldPen = (HPEN)::SelectObject(ps.hdc, g_hUpdateRectPen);
                        ::SelectObject(ps.hdc, ::GetStockObject(HOLLOW_BRUSH));
                        ::Rectangle(ps.hdc, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
                        ::SelectObject(ps.hdc, hOldPen);
                    }
                }
                else
                {
                    int iSaveDc = ::SaveDC(ps.hdc);
                    m_pRoot->DoPaint(ps.hdc,ps.rcPaint);
                    ::RestoreDC(ps.hdc, iSaveDc);
                }
                ::EndPaint( m_hWndPaint, &ps );
            }
            if( m_bUpdateNeeded )
                ::InvalidateRect(m_hWndPaint, NULL ,FALSE );
            return true;

        case WM_PRINTCLIENT:
                break;
        case WM_GETMINMAXINFO:
            {
                LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
                if( m_szMinWindow.cx > 0 ) lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
                if( m_szMinWindow.cy > 0 ) lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
                if( m_szMaxWindow.cx > 0 ) lpMMI->ptMaxTrackSize.x = m_szMaxWindow.cx;
                if( m_szMaxWindow.cy > 0 ) lpMMI->ptMaxTrackSize.y = m_szMaxWindow.cy;
            }
            break;
        case WM_SIZE:
            {
                auto spControlFocus = m_pFocus.lock();
                if(spControlFocus)
                {
                    ControlEvent eve;
                    eve.m_Type = UIEVENT_WINDOWSIZE;
                    eve.m_pSender = m_pFocus;
                    eve.m_dwTimestamp = ::GetTickCount();
                    spControlFocus->Event(eve);
                }
                if( m_pRoot )
                    m_pRoot->NeedUpdate();
            }
            return true;

        case WM_TIMER:
            {
                for( auto iter : m_vecTimers)
                {
                    if(iter.hWnd == m_hWndPaint && iter.uWinTimer == LOWORD(wParam) && iter.bKilled == false)
                    {
                        ControlEvent eve;
                        eve.m_Type = UIEVENT_TIMER;
                        eve.m_pSender = iter.pSender;
                        eve.m_wParam = iter.nLocalID;
                        eve.m_dwTimestamp = ::GetTickCount();
                        auto sp = iter.pSender.lock();
                        if(sp)
                            sp->Event(eve);
                        break;
                    }
                }
            }
            break;
        case WM_MOUSEHOVER:
            {
                m_bMouseTracking = false;
                POINT pt= { GET_X_LPARAM( lParam ), GET_Y_LPARAM(lParam) };
                auto spControl = FindControl(pt);
                if(! spControl)
                    break;
                if( m_pEventHover )
                {
                    ControlEvent eve;
                    eve.m_ptMouse = pt;
                    eve.m_Type = UIEVENT_MOUSEHOVER;
                    eve.m_pSender = m_pEventHover;
                    eve.m_dwTimestamp = ::GetTickCount();
                    m_pEventHover->Event(eve);
                }
                YString sToolTip = spControl->GetToolTip();
                if( sToolTip.empty() ) return true;
                ::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
                m_ToolTip.cbSize = sizeof(TOOLINFO);
                m_ToolTip.uFlags = TTF_IDISHWND;
                m_ToolTip.hwnd = m_hWndPaint;
                m_ToolTip.uId = (UINT_PTR) m_hWndPaint;
                m_ToolTip.hinst = g_hInstance;
                m_ToolTip.lpszText = const_cast<LPTSTR>( (LPCTSTR) sToolTip.c_str() );
                m_ToolTip.rect = spControl->GetPos();
                if( m_hwndTooltip == NULL ) {
                    m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, g_hInstance, NULL);
                    ::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
                }
                ::SendMessage( m_hwndTooltip,TTM_SETMAXTIPWIDTH,0, spControl->GetToolTipWidth());
                ::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
                ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);

            }
            return true;
            break;
        case WM_MOUSELEAVE:
            {
                if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
                if( m_bMouseTracking ) ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) -1);
                m_bMouseTracking = false;
            }
            break;
        case WM_MOUSEMOVE:
            {
                if(! m_bMouseTracking )
                {
                    TRACKMOUSEEVENT tme = { 0 };
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_HOVER | TME_LEAVE;
                    tme.hwndTrack = m_hWndPaint;
                    tme.dwHoverTime = m_hwndTooltip == NULL ? 400UL : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
                    _TrackMouseEvent(&tme);
                    m_bMouseTracking = true;
                }
                POINT pt= { GET_X_LPARAM( lParam) ,GET_Y_LPARAM(lParam) };
                m_ptLastMousePos = pt;
                std::shared_ptr<ControlUI> spNewHover = FindControl(pt);
                if( spNewHover != NULL && spNewHover->GetManager() != this->shared_from_this() )
                    break;
                ControlEvent eve;
                eve.m_ptMouse = pt;
                eve.m_dwTimestamp = ::GetTickCount();
                if(spNewHover != m_pEventHover && m_pEventHover)
                {
                    eve.m_Type = UIEVENT_MOUSELEAVE;
                    eve.m_pSender = m_pEventHover;
                    m_pEventHover->Event(eve);
                    m_pEventHover = NULL;
                    if( m_hwndTooltip != NULL )
                        ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
                }
                if(spNewHover != m_pEventHover && spNewHover)
                {
                    eve.m_Type= UIEVENT_MOUSEENTER;
                    eve.m_pSender = spNewHover;
                    spNewHover->Event(eve);
                    m_pEventHover = spNewHover;
                }
                if(m_pEventClick != NULL )
                {
                    eve.m_Type = UIEVENT_MOUSEMOVE;
                    eve.m_pSender = m_pEventClick;
                    m_pEventClick->Event(eve);
                }
                else if( spNewHover )
                {
                    eve.m_Type = UIEVENT_MOUSEMOVE;
                    eve.m_pSender  = spNewHover;
                    spNewHover->Event(eve);
                }
                
            }
            break;
        case WM_LBUTTONDOWN:
            {
                ::SetFocus(m_hWndPaint);
                POINT pt= { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                auto spControl = FindControl(pt);
                if( spControl == NULL )
                    return;
                if( spControl->GetManager() != shared_from_this())
                    break;
                m_pEventClick = spControl;
                spControl->SetFocus();
                SetCapture();
                ControlEvent eve;
                eve.m_Type = UIEVENT_BUTTONDOWN;
                eve.m_pSender = spControl;
                eve.m_wParam = wParam;
                eve.m_lParam = lParam;
                eve.m_ptMouse = pt;
                eve.m_wKeyState = (WORD )wParam;
                eve.m_dwTimestamp = ::GetTickCount();
                spControl->Event(eve);
            }
            break;
        case WM_LBUTTONDBLCLK:
            {
                ::SetFocus(m_hWndPaint);
                POINT pt= { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                auto spControl = FindControl(pt);
                if( spControl == NULL )
                    return;
                if( spControl->GetManager() != shared_from_this())
                    break;
                SetCapture();
                ControlEvent eve;
                eve.m_Type = UIEVENT_DBLCLICK;
                eve.m_pSender = spControl;
                eve.m_ptMouse = pt;
                eve.m_wKeyState = (WORD )wParam;
                eve.m_dwTimestamp = ::GetTickCount();
                spControl->Event(eve);
                m_pEventClick = spControl;
            }
            break;
        case WM_RBUTTONDOWN:
            {
                ::SetFocus(m_hWndPaint);
                POINT pt= { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                auto spControl = FindControl(pt);
                if( spControl == NULL )
                    return;
                if( spControl->GetManager() != shared_from_this())
                    break;
                spControl->SetFocus();
                SetCapture();
                ControlEvent eve;
                eve.m_Type = UIEVENT_RBUTTONDOWN;
                eve.m_pSender = spControl;
                eve.m_wParam = wParam;
                eve.m_lParam = lParam;
                eve.m_ptMouse = pt;
                eve.m_wKeyState = (WORD )wParam;
                eve.m_dwTimestamp = ::GetTickCount();
                spControl->Event(eve);
                eve.m_lParam = lParam;
                m_pEventClick = spControl;
            }
            break;
        case WM_CONTEXTMENU:
            {
                POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                ::ScreenToClient(m_hWndPaint,&pt);
                m_ptLastMousePos = pt;
                if( m_pEventClick == NULL )
                    break;
                ReleaseCapture();
                ControlEvent eve;
                eve.m_Type = UIEVENT_CONTEXTMENU;
                eve.m_pSender = m_pEventClick;
                eve.m_ptMouse  = pt;
                eve.m_wKeyState = (WORD)wParam;
                eve.m_lParam = (LPARAM) m_pEventClick.get();
                eve.m_dwTimestamp = ::GetTickCount();
                m_pEventClick->Event( eve );
                m_pEventClick = NULL;
            }
        default:
            break;
        }
    }

    void PaintManagerUI::SendNotify(NotifyMsg &Msg, bool bAsync /*= false */)
    {
        assert(0 && "废弃的接口");
    }

    void PaintManagerUI::SendNotify(std::shared_ptr<ControlUI>& spControl,YString strMessage,WPARAM wParam /*=0*/, LPARAM lParam /*=0*/,bool bAsync /*= false */)
    {
        NotifyMsg msg;
        msg.pSender = spControl;
        msg.strType = strMessage;
        msg.wParam = wParam;
        msg.lParam = lParam;
        SendNotify(msg,bAsync);
        msg.ptMouse = m_ptLastMousePos;
        msg.lTimeStamp = ::GetTickCount();
        if( m_bUsedVirtualWnd )
        {
            msg.strVirtualWnd = spControl->GetVirtualWnd();
        }
        if( !bAsync )
        {
            if( spControl->OnNotify )
            {
                 spControl->OnNotify(msg);
            }
            for( auto iter : m_vecNotifiers )
            {
                auto spNotifer= iter.lock();
                spNotifer->Notify(msg);
            }
        }
        else
        {
            m_ListAsyncNotify.push_back(msg);
        }
    }

    bool PaintManagerUI::SetNextTabControl(bool bForward/*= true*/)
    {
        return false;
    }

    std::shared_ptr<ControlUI> PaintManagerUI::FindControl(POINT pt) const
    {
        return std::shared_ptr<ControlUI>();
    }

    void PaintManagerUI::SetFocus(std::shared_ptr<ControlUI> &pControl)
    {
        HWND hFocusWnd = ::GetFocus();
        if( hFocusWnd != m_hWndPaint && pControl != m_pFocus)
        {
            ::SetFocus(m_hWndPaint);
        }
        if( pControl == m_pFocus )
            return;
        if( m_pFocus )
        {
            ControlEvent eve;
            eve.m_Type = UIEVENT_KILLFOCUS;
            eve.m_pSender = pControl;
            eve.m_dwTimestamp = ::GetTickCount();
            m_pFocus->Event(eve);
            SendNotify(m_pFocus,MSG_KillFocus);
            m_pFocus = nullptr;
        }
        if( !pControl)
            return;
        if( pControl  && pControl->GetManager() == shared_from_this()
                       && pControl->IsVisible()
                       && pControl->IsEnable())
        {
            m_pFocus = pControl;
            ControlEvent eve;
            eve.m_Type = UIEVENT_SETFOCUS;
            eve.m_pSender = pControl;
            eve.m_dwTimestamp = ::GetTickCount();
            m_pFocus->Event(eve);
            SendNotify(m_pFocus,MSG_SetFocus);
        }
    }

    void PaintManagerUI::SetCapture()
    {
        ::SetCapture( m_hWndPaint );
        m_bMouseCapture = true;
    }

    void PaintManagerUI::ReleaseCapture()
    {
        ::ReleaseCapture();
        m_bMouseCapture = false;
    }



    HINSTANCE YUI::PaintManagerUI::GetInstance()
    {
        return g_hInstance;
    }

}
