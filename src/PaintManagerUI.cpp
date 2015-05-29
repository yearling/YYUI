//////////////////////////////////////////////////////////////////////////
// Create by yyCom, 2015/4/10
//////////////////////////////////////////////////////////////////////////

#include "YUI.h"
#include "PaintManagerUI.h"
#include "UIUtility.h"
#include "RenderDGI.h"
#include <commctrl.h>
#include <WindowsX.h>
#include <olectl.h>
#include <algorithm>
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
        ,m_hDcOffscreen( NULL )
        ,m_hDcBackground( NULL )
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
        }
        m_DefaultFontInfo = std::make_shared<FontInfo>();
        HFONT hDefaultFont = ::CreateFontIndirect( &lf );
        m_DefaultFontInfo->m_hFont = hDefaultFont;
        m_DefaultFontInfo->m_strFontName = (lf.lfFaceName);
        m_DefaultFontInfo->m_nSize = -lf.lfHeight;
        m_DefaultFontInfo->m_bBold = (lf.lfWeight >= FW_BOLD );
        m_DefaultFontInfo->m_bUnderline = ( lf.lfUnderline == TRUE );
        m_DefaultFontInfo->m_bItalic = (lf.lfItalic == TRUE );
        memset( &m_DefaultFontInfo->m_tm,0,sizeof(m_DefaultFontInfo->m_tm) );

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

    YUI::PaintManagerUI::~PaintManagerUI()
    {
        //!! todo
    }


    bool YUI::PaintManagerUI::TranslateMessage(const LPMSG pMsg)
    {
        // Pretranslate Message takes care of system-wide messages, such as
        // tabbing and shortcut key-combos. We'll look for all messages for
        // each window and any child control attached.
        UINT uStyle = GetWindowStyle(pMsg->hwnd);
        UINT uChildRes = uStyle & WS_CHILD;	
        LRESULT lRes = 0;
        if (uChildRes != 0)
        {
            HWND hWndParent = ::GetParent(pMsg->hwnd);

            for( auto iter : g_vecPreMessages)
            {
                auto pT = iter.lock();
                if(! pT)
                    return false;
                HWND hTempParent = hWndParent;
                while(hTempParent)
                {
                    if(pMsg->hwnd == pT->GetPaintWindow() || hTempParent == pT->GetPaintWindow())
                    {
                        if (pT->TranslateAccelerator(pMsg))
                            return true;

                        if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
                            return true;

                        return false;
                    }
                    hTempParent = GetParent(hTempParent);
                }
            }
        }
        else
        {
            for( auto iter : g_vecPreMessages) 
            {
                auto  pT = iter.lock();
                if( ! pT )
                    return false;
                if(pMsg->hwnd == pT->GetPaintWindow())
                {
                    if (pT->TranslateAccelerator(pMsg))
                        return true;

                    if( pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes) ) 
                        return true;

                    return false;
                }
            }
        }
        return false;
    }
    HINSTANCE YUI::PaintManagerUI::g_hInstance = NULL;


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


    std::vector<std::weak_ptr<ITranslateAccelerator> > YUI::PaintManagerUI::g_vecTranslateAccelerator;




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

    void PaintManagerUI::SetResourceZip(const YString &strPath, bool bCachedResourceZip /*= false*/)
    {
        
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
                ::SetFocus(NULL);
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
                    HDC dc=::BeginPaint(m_hWndPaint, &ps);
                    HPEN  hPen = CreatePen( PS_SOLID , 3 , RGB( 255 , 0 , 0 ));
                    HPEN  hPenOld = ( HPEN )SelectObject ( dc , hPen );
                    ::MoveToEx(dc,0,0,NULL);
                    ::LineTo(dc,100,0);
                    ::EndPaint(m_hWndPaint,&ps);
                    return true;
                }

				if( m_bUpdateNeeded ) 
				{
					m_bUpdateNeeded = false;
					RECT rcClient = { 0 };
					::GetClientRect(m_hWndPaint, &rcClient);
					if( !::IsRectEmpty(&rcClient) ) 
					{
						if( m_pRoot->IsUpdateNeeded() ) 
						{
							m_pRoot->SetPos(rcClient);
							if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
							if( m_hDcBackground != NULL ) ::DeleteDC(m_hDcBackground);
							if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
							if( m_hbmpBackground != NULL ) ::DeleteObject(m_hbmpBackground);
							m_hDcOffscreen = NULL;
							m_hDcBackground = NULL;
							m_hbmpOffscreen = NULL;
							m_hbmpBackground = NULL;
						}
						else 
						{
							/*	CControlUI* pControl = NULL;
							while( pControl = m_pRoot->FindControl(__FindControlFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST) ) {
							pControl->SetPos( pControl->GetPos() );
							}*/
						}
						// We'll want to notify the window when it is first initialized
						// with the correct layout. The window form would take the time
						// to submit swipes/animations.
						if( m_bFirstLayout ) 
						{
							m_bFirstLayout = false;
							SendNotify(m_pRoot,MSG_WindowInit,  0, 0, false);
						}
					}
				}
				// Set focus to first control?
				if( m_bFocusNeeded ) 
				{
					SetNextTabControl();
				}
				//
				// Render screen
				//
				// Prepare offscreen bitmap?
				if( m_bOffscreenPaint && m_hbmpOffscreen == NULL )
				{
					RECT rcClient = { 0 };
					::GetClientRect(m_hWndPaint, &rcClient);
					m_hDcOffscreen = ::CreateCompatibleDC(m_hDCPaint);
					m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDCPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
					assert(m_hDcOffscreen);
					assert(m_hbmpOffscreen);
				}
				// Begin Windows paint
				PAINTSTRUCT ps = { 0 };
				::BeginPaint(m_hWndPaint, &ps);
				if( m_bOffscreenPaint )
				{
					HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
					int iSaveDC = ::SaveDC(m_hDcOffscreen);
					//if( m_bAlphaBackGround )
					//{
					//	if( m_hbmpBackground == NULL ) 
					//	{
					//		RECT rcClient = { 0 };
					//		::GetClientRect(m_hWndPaint, &rcClient);
					//		m_hDcBackground = ::CreateCompatibleDC(m_hDCPaint);;
					//		m_hbmpBackground = ::CreateCompatibleBitmap(m_hDCPaint, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top); 
					//		assert(m_hDcBackground);
					//		assert(m_hbmpBackground);
					//		::SelectObject(m_hDcBackground, m_hbmpBackground);
					//		//::BitBlt(m_hDcBackground, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
					//			//ps.rcPaint.bottom - ps.rcPaint.top, ps.hdc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
					//	}
					//	else
					//		::SelectObject(m_hDcBackground, m_hbmpBackground);
					//	::BitBlt(m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
					//		ps.rcPaint.bottom - ps.rcPaint.top, m_hDcBackground, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
					//}
					m_pRoot->DoPaint(m_hDcOffscreen, ps.rcPaint);
					/*for( int i = 0; i < m_aPostPaintControls.GetSize(); i++ ) {
						CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
						pPostPaintControl->DoPostPaint(m_hDcOffscreen, ps.rcPaint);
					}*/
					::RestoreDC(m_hDcOffscreen, iSaveDC);
					::BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left,
						ps.rcPaint.bottom - ps.rcPaint.top, m_hDcOffscreen, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
					::SelectObject(m_hDcOffscreen, hOldBitmap);

					if( m_bShowUpdateRect ) {
					/*	HPEN hOldPen = (HPEN)::SelectObject(ps.hdc, m_bShowUpdateRect);
						::SelectObject(ps.hdc, ::GetStockObject(HOLLOW_BRUSH));
						::Rectangle(ps.hdc, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
						::SelectObject(ps.hdc, hOldPen);*/
					}
				}
				else
				{
					// A standard paint job
					int iSaveDC = ::SaveDC(ps.hdc);
					m_pRoot->DoPaint(ps.hdc, ps.rcPaint);
					::RestoreDC(ps.hdc, iSaveDC);
				}
				// All Done!
				::EndPaint(m_hWndPaint, &ps);
			}
			// If any of the painting requested a resize again, we'll need
			// to invalidate the entire window once more.
			if( m_bUpdateNeeded ) 
			{
				::InvalidateRect(m_hWndPaint, NULL, FALSE);
			}
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
                auto spControlFocus = m_pFocus;
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
                    break;
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
                    break;
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
        case WM_LBUTTONUP:
            {
                POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                m_ptLastMousePos = pt;
                if( m_pEventClick == NULL ) break;
                ReleaseCapture();
                ControlEvent eve ;
                eve.m_Type = UIEVENT_BUTTONUP;
                eve.m_pSender = m_pEventClick;
                eve.m_wParam = wParam;
                eve.m_lParam = lParam;
                eve.m_ptMouse = pt;
                eve.m_wKeyState = (WORD)wParam;
                eve.m_dwTimestamp = ::GetTickCount();
                m_pEventClick->Event(eve);
                m_pEventClick = NULL;
            }
            break;
        case WM_RBUTTONDOWN:
            {
                ::SetFocus(m_hWndPaint);
                POINT pt= { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                auto spControl = FindControl(pt);
                if( spControl == NULL )
                    break;
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
            break;
        case WM_MOUSEWHEEL:
            {
                ::SetFocus(m_hWndPaint);
                POINT pt= { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                auto spControl = FindControl(pt);
                if( spControl == NULL )
                    break;
                if( spControl->GetManager() != shared_from_this())
                    break;
                int ZDelta = (int)(short)HIWORD(wParam);
                ControlEvent eve;
                eve.m_Type = UIEVENT_SCROLLWHEEL;
                eve.m_pSender = spControl;
                eve.m_wParam = MAKELPARAM( ZDelta <0 ? SB_LINEDOWN :SB_LINEUP  ,0 );
                eve.m_lParam = lParam;
                eve.m_wKeyState = MapKeyState();
                eve.m_dwTimestamp = ::GetTickCount();
                spControl->Event(eve);
                // Let's make sure that the scroll item below the cursor is the same as before...
                ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0,(LPARAM)MAKELPARAM( m_ptLastMousePos.x, m_ptLastMousePos.y));
            }
            break;
        case WM_CHAR:
            {
                if( ! m_pFocus )
                    break;
                ControlEvent eve;
                eve.m_Type = UIEVENT_CHAR;
                eve.m_chKey = (TCHAR)wParam;
                eve.m_ptMouse = m_ptLastMousePos;
                eve.m_wKeyState = MapKeyState();
                eve.m_dwTimestamp = ::GetTickCount();
                m_pFocus->Event(eve);
            }
            break;
        case WM_KEYDOWN:
            {
                if( ! m_pFocus )
                    break;
                ControlEvent eve;
                eve.m_Type = UIEVENT_KEYDOWN;
                eve.m_chKey = (TCHAR)wParam;
                eve.m_ptMouse = m_ptLastMousePos;
                eve.m_wKeyState = MapKeyState();
                eve.m_dwTimestamp = ::GetTickCount();
                m_pFocus->Event(eve);

            }
            break;
        case WM_KEYUP:
            {
                if( ! m_pFocus )
                    break;
                ControlEvent eve;
                eve.m_Type = UIEVENT_KEYUP;
                eve.m_chKey = (TCHAR)wParam;
                eve.m_ptMouse = m_ptLastMousePos;
                eve.m_wKeyState = MapKeyState();
                eve.m_dwTimestamp = ::GetTickCount();
                m_pFocus->Event(eve);
            }
            break;
        case WM_SETCURSOR:
            {
                if( LOWORD(lParam) != HTCLIENT ) break;
                if( m_bMouseCapture ) return true;

                POINT pt = { 0 };
                ::GetCursorPos(&pt);
                ::ScreenToClient(m_hWndPaint, &pt);
                auto spControl = FindControl(pt);
                if( spControl == NULL )
                    break;
                if( (spControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 ) break;
                ControlEvent eve;
                eve.m_Type = UIEVENT_SETCURSOR;
                eve.m_wParam = wParam;
                eve.m_lParam = lParam;
                eve.m_ptMouse = pt;
                eve.m_wKeyState = MapKeyState();
                eve.m_dwTimestamp = ::GetTickCount();
                spControl->Event(eve);
            }
            break;
        case WM_NOTIFY:
            {
                LPNMHDR lpNMHDR = (LPNMHDR) lParam;
                if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMesg, wParam, lParam);
                return true;
            }
            break;
        case WM_COMMAND:
            {
                if( lParam == 0 ) break;
                HWND hWndChild = (HWND) lParam;
                lRes = ::SendMessage(hWndChild, OCM__BASE + uMesg, wParam, lParam);
                return true;
            }
            break;
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORSTATIC:
            {
                // Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
                // Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
                if( lParam == 0 ) break;
                HWND hWndChild = (HWND) lParam;
                lRes = ::SendMessage(hWndChild, OCM__BASE + uMesg, wParam, lParam);
                return true;
            }
            break;
        default:
            break;
        }
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
        return false;
    }

    void PaintManagerUI::SendNotify(NotifyMsg &msg, bool bAsync /*= false */)
    {
        //return;
		msg.ptMouse = m_ptLastMousePos;
		msg.lTimeStamp = ::GetTickCount();
		auto spControl = msg.pSender.lock();
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

    void PaintManagerUI::SendNotify(std::shared_ptr<ControlUI>& spControl,YString strMessage,WPARAM wParam /*=0*/, LPARAM lParam /*=0*/,bool bAsync /*= false */)
    {
        NotifyMsg msg;
        msg.pSender = spControl;
        msg.strType = strMessage;
        msg.wParam = wParam;
        msg.lParam = lParam;
        SendNotify(msg,bAsync);
       
    }

    bool PaintManagerUI::SetNextTabControl(bool bForward/*= true*/)
    {
        return false;
    }

    std::shared_ptr<ControlUI> PaintManagerUI::FindControl(POINT pt) const
    {
        assert(m_pRoot);
        return m_pRoot->FindControlFromPoint(pt,UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
    }

    std::shared_ptr<ControlUI> PaintManagerUI::FindControl(const YString & strControlName)
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
                       && pControl->IsEnabled())
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

    void PaintManagerUI::SetFocus(std::shared_ptr<ControlUI> pControl)
    {
        HWND hFocusWnd = ::GetFocus();
        if( hFocusWnd != m_hWndPaint && pControl != m_pFocus ) ::SetFocus(m_hWndPaint);
        // Already has focus?
        if( pControl == m_pFocus ) return;
        // Remove focus from old control
        if( m_pFocus != NULL ) 
        {
            ControlEvent event ;
            event.m_Type = UIEVENT_KILLFOCUS;
            event.m_pSender = pControl;
            event.m_dwTimestamp = ::GetTickCount();
            m_pFocus->Event(event);
            SendNotify(m_pFocus, MSG_KillFocus);
            m_pFocus = NULL;
        }
        if( pControl == NULL ) return;
        // Set focus to new control
        if( pControl != NULL 
            && pControl->GetManager() == shared_from_this() 
            && pControl->IsVisible() 
            && pControl->IsEnabled() ) 
        {
            m_pFocus = pControl;
            ControlEvent event;
            event.m_Type = UIEVENT_SETFOCUS;
            event.m_pSender = pControl;
            event.m_dwTimestamp = ::GetTickCount();
            m_pFocus->Event(event);
            SendNotify(m_pFocus, MSG_SetFocus);
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

    UINT PaintManagerUI::MapKeyState()
    {
        UINT uState = 0;
        if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
        if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_LBUTTON;
        if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_RBUTTON;
        if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
        if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
        return uState;
    }

    void PaintManagerUI::NeedUpdate()
    {
        m_bUpdateNeeded = true;
    }

    void PaintManagerUI::Invalidate(const RECT & rcItem)
    {
        ::InvalidateRect(m_hWndPaint,&rcItem,FALSE);
    }



    void PaintManagerUI::MessageLoop()
    {
        MSG msg = { 0 };
        while( ::GetMessage(&msg, NULL, 0, 0) ) 
        {
            if( !PaintManagerUI::TranslateMessage(&msg) )
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if( msg.message == WM_QUIT )
                    break;
            }
        }
    }

    void PaintManagerUI::AddPreMessageFilter(std::shared_ptr<IMessageFilterUI> spFilter)
    {
       m_vecPreMessageFilers.push_back(spFilter); 
    }

    YUI::YString PaintManagerUI::GetResourcePath()
    {
        return g_strResourcePath;
    }

    std::shared_ptr<ControlUI> PaintManagerUI::GetFocus() const
    {
        return m_pFocus;
    }

    void PaintManagerUI::SetFocusNeeded(std::shared_ptr<ControlUI> pControl)
    {

    }

    std::shared_ptr<const ImageInfo> PaintManagerUI::GetImage(const YString &bitmap)
    {
         if(m_mapImageHash.find(bitmap) != m_mapImageHash.end())
             return m_mapImageHash[bitmap];
        auto spParent = m_wpParentResourcePM.lock();
        if(spParent)
         {
           return spParent->GetImage(bitmap); 
         }
         return std::shared_ptr<const ImageInfo>();
    }

    std::shared_ptr<const ImageInfo> PaintManagerUI::GetImageEx(const YString &bitmap, LPCTSTR type /*= NULL*/, DWORD mask /*= 0*/)
    {
        if( m_mapImageHash.find(bitmap) == m_mapImageHash.end())
        {
            return AddImage(bitmap,type,mask);
        }
        else
        {
            return m_mapImageHash[bitmap];
        }
    }

    std::shared_ptr<ImageInfo> PaintManagerUI::AddImage(const YString &bitmap, LPCTSTR type /*= NULL*/, DWORD mask /*= 0*/)
    {
        auto data= RenderGDI::LoadImage(bitmap,type,mask);
        if(data)
        {
            if(type != NULL)
                data->m_strResType = type;
            data->m_dwMask = mask;
            m_mapImageHash[bitmap] = data;
        }
        return data;
    }

    std::shared_ptr<ImageInfo> PaintManagerUI::AddImage(const YString& bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha)
    {
        if( hBitmap == NULL || iWidth <= 0 || iHeight <=0 )
            return NULL;
        auto data = std::shared_ptr<ImageInfo>(new ImageInfo,&ImageInfo::ImageInfoDeleter);
        data->m_hBitmap = hBitmap;
        data->m_nX = iWidth;
        data->m_nY = iHeight;
        data->m_alphaChannel = bAlpha;
        data->m_strResType =_T("");
        data->m_dwMask = 0;
        m_mapImageHash[bitmap]=data;
        return data;
    }

    bool PaintManagerUI::RemoveImage(const YString& bitmap)
    {
        auto pos = m_mapImageHash.find(bitmap);
        if(pos != m_mapImageHash.end())
        {
            m_mapImageHash.erase(pos);
            return true;
        }
        else
            return false;
    }

    void PaintManagerUI::RemoveAllImages()
    {
        m_mapImageHash.clear();
    }

    void PaintManagerUI::ReloadAllImages()
    {
        bool bRedraw= false;
        for(auto iter : m_mapImageHash )
        {
            auto spNewImage = RenderGDI::LoadImage(iter.first,NULL,iter.second->m_dwMask);
            if( !spNewImage )
                continue;
            iter.second = spNewImage;
            bRedraw = true;
        }
        if(bRedraw)
            m_pRoot->Invalidate();
    }

    HFONT PaintManagerUI::AddFont(const YString &StrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
    {
        LOGFONT lf = { 0 };
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        _tcsncpy_s(lf.lfFaceName, StrFontName.c_str(), LF_FACESIZE);
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfHeight = -nSize;
        if( bBold ) lf.lfWeight += FW_BOLD;
        if( bUnderline ) lf.lfUnderline = TRUE;
        if( bItalic ) lf.lfItalic = TRUE;
        HFONT hFont = ::CreateFontIndirect(&lf);
        if( hFont == NULL ) return NULL;

        auto pFontInfo = std::shared_ptr<FontInfo>(new FontInfo, &FontInfo::FontInfoDeleter);
        if( !pFontInfo ) 
            return NULL;
        pFontInfo->m_hFont = hFont;
        pFontInfo->m_strFontName = StrFontName;
        pFontInfo->m_nSize = nSize;
        pFontInfo->m_bBold = bBold;
        pFontInfo->m_bUnderline = bUnderline;
        pFontInfo->m_bItalic = bItalic;
        if( m_hDCPaint ) {
            HFONT hOldFont = (HFONT) ::SelectObject(m_hDCPaint, hFont);
            ::GetTextMetrics(m_hDCPaint, &pFontInfo->m_tm);
            ::SelectObject(m_hDCPaint, hOldFont);
        }
        m_mapCustomFonts[StrFontName] = pFontInfo;

        return hFont;
    }

    HFONT PaintManagerUI::GetFont(const YString &StrFontName)
    {
        if(StrFontName.empty() || (m_mapCustomFonts.find(StrFontName)==m_mapCustomFonts.end()))
            return GetDefaultFontInfo()->m_hFont;
        return m_mapCustomFonts[StrFontName]->m_hFont;
    }

    std::shared_ptr<FontInfo> PaintManagerUI::GetDefaultFontInfo()
    {
        auto spParent = m_wpParentResourcePM.lock();
        if(spParent)
            spParent->GetDefaultFontInfo();

        if(m_DefaultFontInfo->m_tm.tmHeight == 0)
        {
            HFONT hOldFont = (HFONT) ::SelectObject(m_hDCPaint, m_DefaultFontInfo->m_hFont);
            ::GetTextMetrics(m_hDCPaint, &m_DefaultFontInfo->m_tm);
            ::SelectObject(m_hDCPaint, hOldFont);
        }
        return m_DefaultFontInfo;
    }

    std::shared_ptr<FontInfo> PaintManagerUI::GetFontInfo(HFONT hFont)
    {
        auto iter = std::find_if(m_mapCustomFonts.begin(),
                    m_mapCustomFonts.end(),
                    [&]( std::map<YString,std::shared_ptr<FontInfo>>::value_type &p)
                    {
                        if(p.second->m_hFont== hFont )
                            return true;
                        else
                            return false;
                    });
        if(iter!= m_mapCustomFonts.end())
         {
			 auto sp = iter->second;
			 if(sp->m_tm.tmHeight == 0)
			 {
				 HFONT hOldFont = (HFONT) ::SelectObject(m_hDCPaint, sp->m_hFont);
				 ::GetTextMetrics(m_hDCPaint, &sp->m_tm);
				 ::SelectObject(m_hDCPaint, hOldFont);
			 }
			 return sp;
		}
        else
        {
            auto spParent = m_wpParentResourcePM.lock();
            if(spParent)
                return spParent->GetFontInfo(hFont);
        }
        return GetDefaultFontInfo();
    }

	std::shared_ptr<FontInfo> PaintManagerUI::GetFontInfo(const YString &StrFontName)
	{
		auto pos = m_mapCustomFonts.find(StrFontName);
		if(pos == m_mapCustomFonts.end())
			return GetDefaultFontInfo();
		else
		{
			auto sp = m_mapCustomFonts[StrFontName];
			if(sp->m_tm.tmHeight == 0)
			{
				HFONT hOldFont = (HFONT) ::SelectObject(m_hDCPaint, sp->m_hFont);
				::GetTextMetrics(m_hDCPaint, &sp->m_tm);
				::SelectObject(m_hDCPaint, hOldFont);
			}
		}
	}

    bool PaintManagerUI::RemoveFont(HFONT hFont)
    {
        auto iter = std::find_if(m_mapCustomFonts.begin(),
            m_mapCustomFonts.end(),
            [&]( std::map<YString,std::shared_ptr<FontInfo>>::value_type &p)
        {
            if(p.second->m_hFont== hFont )
                return true;
            else
                return false;
        });
        m_mapCustomFonts.erase(iter);
        return true;
    }

    void PaintManagerUI::RemoveAllFonts()
    {
        m_mapCustomFonts.clear();
    }

    void PaintManagerUI::SetDefaultFont(const YString &StrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
    {
        LOGFONT lf = { 0 };
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        _tcsncpy_s(lf.lfFaceName, StrFontName.c_str(), LF_FACESIZE);
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfHeight = -nSize;
        if( bBold ) lf.lfWeight += FW_BOLD;
        if( bUnderline ) lf.lfUnderline = TRUE;
        if( bItalic ) lf.lfItalic = TRUE;
        HFONT hFont = ::CreateFontIndirect(&lf);
        if( hFont == NULL ) return;

        ::DeleteObject(m_DefaultFontInfo->m_hFont);
        m_DefaultFontInfo->m_hFont = hFont;
        m_DefaultFontInfo->m_strFontName = StrFontName;
        m_DefaultFontInfo->m_nSize = nSize;
        m_DefaultFontInfo->m_bBold = bBold;
        m_DefaultFontInfo->m_bUnderline = bUnderline;
        m_DefaultFontInfo->m_bItalic = bItalic;
        ::ZeroMemory(&m_DefaultFontInfo->m_tm, sizeof(m_DefaultFontInfo->m_tm));
        if( m_hDCPaint ) {
            HFONT hOldFont = (HFONT) ::SelectObject(m_hDCPaint, hFont);
            ::GetTextMetrics(m_hDCPaint, &m_DefaultFontInfo->m_tm);
            ::SelectObject(m_hDCPaint, hOldFont);
        }
    }

    DWORD PaintManagerUI::GetDefaultDisabledColor() const
    {
        auto sp = m_wpParentResourcePM.lock();
        if(sp)
            return sp->GetDefaultDisabledColor();
        return m_dwDefaultDisabledColor;
    }

    void PaintManagerUI::SetDefaultDisabledColor(DWORD dwColor)
    {
        m_dwDefaultDisabledColor = dwColor;
    }

    DWORD PaintManagerUI::GetDefaultFontColor() const
    {
        auto sp = m_wpParentResourcePM.lock();
        if(sp)
            return sp->GetDefaultFontColor();
        return m_dwDefaultFontColor;
    }

    void PaintManagerUI::SetDefaultFontColor(DWORD dwColor)
    {
        m_dwDefaultFontColor = dwColor;
    }

    DWORD PaintManagerUI::GetDefaultLinkFontColor() const
    {
        auto sp = m_wpParentResourcePM.lock();
        if(sp)
            return sp->GetDefaultLinkFontColor();
        return m_dwDefaultLinkFontColor;
    }

    void PaintManagerUI::SetDefaultLinkFontColor(DWORD dwColor)
    {
        m_dwDefaultLinkFontColor = dwColor;
    }

    DWORD PaintManagerUI::GetDefaultLinkHoverFontColor() const
    {
        auto sp = m_wpParentResourcePM.lock();
        if(sp)
            return sp->GetDefaultLinkHoverFontColor();
        return m_dwDefaultLinkHoverFontColor;
    }

    void PaintManagerUI::SetDefaultLinkHoverFontColor(DWORD dwColor)
    {
        m_dwDefaultLinkHoverFontColor = dwColor;
    }

    DWORD PaintManagerUI::GetDefaultSelectedBkColor() const
    {
        auto sp = m_wpParentResourcePM.lock();
        if(sp)
            return sp->GetDefaultSelectedBkColor();
        return m_dwDefaultSelectedBKColor;
    }

    void PaintManagerUI::SetDefaultSelectedBkColor(DWORD dwColor)
    {
        m_dwDefaultSelectedBKColor = dwColor;
    }

    bool PaintManagerUI::InitControls(std::shared_ptr<ControlUI> &pControl,std::weak_ptr<ControlUI> parent)
    {
        assert(pControl);
        if( !pControl )
            return false;
        if(parent.lock())
            pControl->SetManager(this->shared_from_this(),parent,true);
        else
            pControl->SetManager(this->shared_from_this(),pControl->GetParent(),true);
        return true;
    }

    std::shared_ptr<ControlUI> PaintManagerUI::GetRoot() const
    {
        return m_pRoot;
    }

    int PaintManagerUI::GetPostPaintCount() const
    {
        return m_vecPostPaintControls.size();
    }

    bool PaintManagerUI::AddPostPaint(std::shared_ptr<ControlUI> pControl)
    {
        m_vecPostPaintControls.push_back(pControl);
        return true;
    }

    bool PaintManagerUI::RemovePostPaint(std::shared_ptr<ControlUI> pControl)
    {
        m_vecPostPaintControls.erase(std::remove(m_vecPostPaintControls.begin(),m_vecPostPaintControls.end(),pControl),m_vecPostPaintControls.end());
        return true;
    }

    bool PaintManagerUI::SetPostPaintIndex(std::shared_ptr<ControlUI> pContro)
    {
       RemovePostPaint(pContro);
       m_vecPostPaintControls.push_back(pContro);
       return true;
    }

	bool PaintManagerUI::AttachDialog(std::shared_ptr<ControlUI> pControl)
	{
		assert( ::IsWindow(m_hWndPaint) );
		::SetFocus(NULL);
		m_pEventKey = NULL;
		m_pEventHover = NULL;
		m_pEventClick = NULL;
		if( m_pRoot != NULL )
			m_vecPostPaintControls.clear();
		m_pRoot = pControl;
		m_bUpdateNeeded = true;
		m_bFirstLayout = true;
		m_bFocusNeeded = true;
		return InitControls(pControl,std::shared_ptr<ControlUI>());
	}

	bool PaintManagerUI::AddNotifier(std::shared_ptr<INotifyUI> pControl)
	{
		m_vecNotifiers.push_back(pControl);
		return true;
	}


    HINSTANCE YUI::PaintManagerUI::GetInstance()
    {
        return g_hInstance;
    }

    bool YUI::PaintManagerUI::TranslateAccelerator(LPMSG pMsg)
    {
        for(auto iter : g_vecTranslateAccelerator)
        {
            auto spTA= iter.lock();
            if( spTA )
            {
                LRESULT hr=spTA->TranslateAccelerator(pMsg);
                if(hr == S_OK )
                    return true;
            }
        }
         
        return false;
    }


}