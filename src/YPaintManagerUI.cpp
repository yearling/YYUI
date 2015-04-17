//////////////////////////////////////////////////////////////////////////
// Create by yyCom, 2015/4/10
//////////////////////////////////////////////////////////////////////////

#include "YYUI.h"
#include "YPaintManagerUI.h"
#include "YYUtility.h"
#include <commctrl.h>
#include <WindowsX.h>
#pragma comment(lib,"comctl32.lib")
namespace YYCOM
{

    void YYCOM::YPaintManagerUI::SetInstance(HINSTANCE hInst)
    {
        g_hInstance = hInst;
    }
    HPEN g_hUpdateRectPen = NULL;

    YYCOM::YPaintManagerUI::YPaintManagerUI():
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
        if( !YPaintManagerUI::g_strDefaultFontName.empty() )
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

    YYCOM::YPaintManagerUI::~YPaintManagerUI()
    {
        //!! todo
    }


    bool YYCOM::YPaintManagerUI::TranslateMessage(const LPMSG pMsg)
    {
        return false;
    }
    HINSTANCE YYCOM::YPaintManagerUI::g_hInstance = NULL;

    std::vector<void*> YYCOM::YPaintManagerUI::g_vecTranslateAccelerator;

    YString YYCOM::YPaintManagerUI::g_strDefaultFontName;





    short YYCOM::YPaintManagerUI::g_L       = 180;

    short YYCOM::YPaintManagerUI::g_S       = 180;

    short YYCOM::YPaintManagerUI::g_H       = 180;

    HANDLE YYCOM::YPaintManagerUI::g_hResourceZip = NULL;

    bool YYCOM::YPaintManagerUI::g_bCachedResourceZip = false;

    YString YYCOM::YPaintManagerUI::g_strResourceZip;

    YString YYCOM::YPaintManagerUI::g_strResourcePath;

    HINSTANCE YYCOM::YPaintManagerUI::g_hResourceInstance = NULL;

   // std::set<funCreateControl> YPaintManagerUI::g_setPlugins;

    std::vector< std::weak_ptr<YPaintManagerUI> > YPaintManagerUI::g_vecPreMessages;






    void YPaintManagerUI::Init(HWND hWnd)
    {
        assert( ::IsWindow(hWnd ) && "hwnd 不是个有效值");

        m_hWndPaint = hWnd;
        m_hDCPaint = ::GetDC(hWnd);
        g_vecPreMessages.push_back(this->shared_from_this());
    }

    YString YPaintManagerUI::GetInstancePath()
    {
        if( g_hInstance == NULL )
            return _T('\0');
        TCHAR szModule[MAX_PATH + 1] = { 0 };
        ::GetModuleFileName(g_hInstance,szModule,MAX_PATH);
        return GetFileDir(YString(szModule));
    }

    YYCOM::YString YPaintManagerUI::GetCurrentPath()
    {
        TCHAR szPath[MAX_PATH +1 ]= { 0 };
        ::GetCurrentDirectory(MAX_PATH,szPath);
        return szPath;
    }

    HINSTANCE YPaintManagerUI::GetResourceDll()
    {
        if( g_hResourceInstance == NULL )
            return g_hInstance;
        return g_hResourceInstance;
    }

    const YString & YPaintManagerUI::GetSourcePath()
    {
        return g_strResourcePath;
    }

    const YString & YPaintManagerUI::GetResourceZip()
    {
        return g_strResourceZip;
    }

    bool YPaintManagerUI::IsCachedResourceZip()
    {
        return g_bCachedResourceZip;
    }

    HANDLE YPaintManagerUI::GetResourceZipHandle()
    {
        return g_hResourceZip;
    }

   
    void YPaintManagerUI::SetCurrentPath(const YString & strPath)
    {
        ::SetCurrentDirectory(strPath.c_str());
    }

    void YPaintManagerUI::SetResourceDll(HINSTANCE hInstance)
    {
        g_hResourceInstance = hInstance;
    }

    void YPaintManagerUI::SetResourcePath(const YString &strPath)
    {
        g_strResourcePath = strPath;
        if(g_strResourcePath.empty())
            return;
        if(g_strResourcePath[g_strResourcePath.size()-1] == _T('\\') ||
            g_strResourcePath[g_strResourcePath.size()-1] == _T('/') )
            g_strResourcePath+=_T('\\');
    }

    void YPaintManagerUI::SetResourceZip(void)
    {
        //!todo 
        return;
    }

    void YPaintManagerUI::ReloadSkin()
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

    bool YPaintManagerUI::LoadPlugin(const YString & strModuleName)
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

    HDC YPaintManagerUI::GetPaintDC() const
    {
        return m_hDCPaint;
    }

    HWND YPaintManagerUI::GetPaintWindow() const
    {
        return m_hWndPaint;
    }

    HWND YPaintManagerUI::GetTooltipWindow() const
    {
        return m_hwndTooltip;
    }

    POINT YPaintManagerUI::GetMousePos() const
    {
        return m_ptLastMousePos;
    }

    SIZE YPaintManagerUI::GetClientSize() const
    {
        RECT rcClient = { 0 };
        ::GetClientRect( m_hWndPaint, &rcClient);
        return YSize(rcClient.right - rcClient.left , rcClient.bottom - rcClient.top);
    }

    SIZE YPaintManagerUI::GetInitSize() const
    {
        return m_szInitWindowSize;
    }

    void YPaintManagerUI::SetInitSize(int cx, int cy)
    {
        m_szInitWindowSize.cx = cx;
        m_szInitWindowSize.cy = cy;
        if( m_pRoot == NULL && m_hWndPaint != NULL )
            ::SetWindowPos(m_hWndPaint, NULL, 0, 0, cx, cy,SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    }

    const RECT& YPaintManagerUI::GetSizeBox() const 
    {
        return m_rcSizeBox;
    }

    void YPaintManagerUI::SetSizeBox(const RECT &rcSizeBox)
    {
        m_rcSizeBox = rcSizeBox;
    }

    const RECT& YPaintManagerUI::GetCaptionRect() const
    {
        return m_rcCaption;
    }

    void YPaintManagerUI::SetCaptionRect(const RECT & rcCaption)
    {
        m_rcCaption =rcCaption;
    }

    SIZE YPaintManagerUI::GetRoundCorner() const
    {
        return m_szRoundCorner;
    }

    void YPaintManagerUI::SetRoundCorner(int cx, int cy)
    {
        m_szRoundCorner.cx = cx;
        m_szRoundCorner.cy = cy;
    }

    SIZE YPaintManagerUI::GetMinInfo() const
    {
        return m_szMinWindow;
    }

    void YPaintManagerUI::SetMinInfo(int cx, int cy)
    {
        assert(cx >= 0 && cy >= 0);
        m_szMinWindow.cx = cx;
        m_szMinWindow.cy = cy;
    }

    SIZE YPaintManagerUI::GetMaxInfo() const
    {
        return m_szMaxWindow;
    }

    void YPaintManagerUI::SetMaxInfo(int cx, int cy)
    {
        assert(cx >= 0 && cy >= 0);
        m_szMaxWindow.cx = cx;
        m_szMaxWindow.cy = cy;
    }

    int YPaintManagerUI::GetTransparent() const
    {
        return m_nOpacity;
    }

    void YPaintManagerUI::SetTransparent(int nOpacity)
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

    void YPaintManagerUI::SetBackgroundTransparent(bool bTrans)
    {
        m_bAlphaBackGround = bTrans;
    }

    bool YPaintManagerUI::IsShowUpdateRect() const
    {
        return m_bShowUpdateRect;
    }

    void YPaintManagerUI::SetShowUpdateRect(bool bShow)
    {
        m_bShowUpdateRect = bShow;
    }

    struct tagFINDSHORTCUT
    {
        TCHAR ch;
        bool bPickNext;
    };

    bool YPaintManagerUI::PreMessageHandler(UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT &lRes)
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

    bool YPaintManagerUI::MessageHandler(UINT uMesg, WPARAM wParam, LPARAM lParam, LRESULT &lRes)
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
                ControlEvent eve = { 0 };
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
                           SendNotify(m_pRoot,MSGTYPE_windows_init,0,0,false);
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
                    ControlEvent eve =  { 0 };
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
                
            }
            break;
        default:
            break;
        }
    }

    void YPaintManagerUI::SendNotify(NotifyMsg &Msg, bool bAsync /*= false */)
    {
        assert(0 && "废弃的接口");
    }

    void YPaintManagerUI::SendNotify(std::shared_ptr<YControlUI>& spControl,YString strMessage,WPARAM wParam /*=0*/, LPARAM lParam /*=0*/,bool bAsync /*= false */)
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

    bool YPaintManagerUI::SetNextTabControl(bool bForward/*= true*/)
    {
        return false;
    }



    HINSTANCE YYCOM::YPaintManagerUI::GetInstance()
    {
        return g_hInstance;
    }

}
