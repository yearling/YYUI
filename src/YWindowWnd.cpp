//////////////////////////////////////////////////////////////////////////
// Create by yyCom ,2015/4/10
//////////////////////////////////////////////////////////////////////////

#include "YYUI.h"
#include "YWindowWnd.h"
#include "YControlUI.h"
#include "YYUIDef.h"
#include "YYUtility.h"
#include "YPaintManagerUI.h"


namespace YYCOM
{

    YWindowWnd::YWindowWnd() : m_hWnd( NULL ),
        m_OldWndProc( ::DefWindowProc ),
        m_bSubClassed( false )
    {

    }

    void YWindowWnd::RegisterWindowClass()
    {
        WNDCLASSEX wc;
        ZeroMemory(&wc,sizeof(wc));
        wc.cbSize = sizeof(wc);
        wc.style = GetClassStyle();
        wc.lpfnWndProc = YWindowWnd::WndProc; //用自己的WndProc
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = YPaintManagerUI::GetInstance();
        wc.hIcon = NULL;
        wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = GetWindowClassName();
        wc.hIconSm = NULL;

        ATOM ret = ::RegisterClassEx( &wc );

        DWORD err = ::GetLastError();
        assert( ret !=0  || err == ERROR_CLASS_ALREADY_EXISTS );

        if( ret == 0  && err != ERROR_CLASS_ALREADY_EXISTS)
        {
            THROW_EXCEPTION(YYUIException()<<UIErrorStr(_T("Call RegisterClassEx Failed! :")+FormatGetLastError(err)));
        }

    }


    void YWindowWnd::RegisterSuperClass()
    {
        //！！目前还不知道有什么用
        //功能是如下
        // Get the class information from an existing
        // window so we can subclass it later on...
        //为什么是ControlProc不明白
        WNDCLASSEX wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.cbSize = sizeof(wc);
        if( !::GetClassInfoEx( NULL, GetSuperClassName() , &wc) )
        {
            if( !::GetClassInfoEx(YPaintManagerUI::GetInstance(),GetSuperClassName(),&wc))
            {
                THROW_EXCEPTION(YYUIException()<< UIErrorStr(_T("找不到Register Class Name"))); 
            }
        }
        m_OldWndProc = wc.lpfnWndProc;
        wc.lpfnWndProc = YWindowWnd::ControlProc;
        wc.hInstance = YPaintManagerUI::GetInstance();
        wc.lpszClassName = GetWindowClassName();

        ATOM hr = ::RegisterClassEx(&wc);
        assert( hr !=0  || hr == ERROR_CLASS_ALREADY_EXISTS );

        if( hr == 0  && hr != ERROR_CLASS_ALREADY_EXISTS)
        {
            THROW_EXCEPTION(YYUIException()<<UIErrorStr(_T("Call RegisterClassEx Failed! :")+FormatGetLastError(hr)));
        }
    }

    HWND YWindowWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu/*= NULL*/)
    {
        return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu );
    }

    HWND YWindowWnd::Create(HWND hWndParent, LPCTSTR pstrName, DWORD dwStyple, DWORD dwExStyle, int x /*= CW_USEDEFAULT*/, int y /*= CW_USEDEFAULT*/, int cx /*= CW_USEDEFAULT*/, int cy /*= CW_USEDEFAULT*/, HMENU hMenu /*= NULL*/)
    {
        try
        {

            if( GetSuperClassName() != nullptr )
                RegisterSuperClass();   //如果SuperClassName存在的话就创建SuperClass
            else
                RegisterWindowClass();
            m_hWnd = CreateWindowEx( dwExStyle, GetWindowClassName(), pstrName, dwStyple, x, y, cx, cy, hWndParent, hMenu, YPaintManagerUI::GetInstance(), this);

            assert( m_hWnd != NULL );
            return m_hWnd;
        }
        catch( YYUIException &e )
        {
            throw;
        }
    }

    HWND YWindowWnd::CreateUIWindow(HWND hWndParent, LPCTSTR pstrWindowName, DWORD dwStyle /*=0*/, DWORD dwExStyle/*=0*/)
    {
        return Create(hWndParent,pstrWindowName,dwStyle,dwExStyle,0,0,0,0,NULL);
    }

    UINT YWindowWnd::GetClassStyle() const
    {
        return 0;
    }

    LRESULT CALLBACK YWindowWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        YWindowWnd *pWnd = NULL;

        if(uMsg == WM_NCCREATE )
        {
            //窗口刚开始创建时CreateWindowEx，最后一个参数LPVOID lpParam，保存了当前的this指针，这里取出来；
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>( lParam );
            pWnd = static_cast<YWindowWnd*>( lpcs->lpCreateParams );
            pWnd->m_hWnd = hWnd;
            //这时把YWindowWnd的指针绑到当前生成的HWND上
            //相当于每个HWND都通过这个指针映射到一个YWinodwWnd的类
            ::SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pWnd) );
        }
        else
        {
            pWnd = reinterpret_cast< YWindowWnd* >( ::GetWindowLongPtr( hWnd,GWLP_USERDATA ) );
            if(uMsg == WM_NCDESTROY && pWnd != NULL )
            {
                //一般调用的是DefWindowProc
                LRESULT lRes = ::CallWindowProc( pWnd->m_OldWndProc, hWnd, uMsg, wParam, lParam );
                //清空hWnd绑到的YWindowWnd
                ::SetWindowLongPtr( hWnd, GWLP_USERDATA , NULL );
                //???
                if( pWnd->m_bSubClassed )
                    pWnd->UnsubClass();
                pWnd->m_hWnd = NULL;
                pWnd->OnFinalMessage(hWnd);
                return lRes;
            }
        }
        if( pWnd != NULL )
            return pWnd->HandleMessage(uMsg,wParam,lParam);
        else
            return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
    }

    void YWindowWnd::UnsubClass()
    {
        assert( ::IsWindow(m_hWnd) );
        if( ::IsWindow(m_hWnd) )
            return ;
        if( !m_bSubClassed )
            return ;
        SetWindowLongPtr((m_hWnd), GWLP_WNDPROC, (LPARAM)(WNDPROC)(m_OldWndProc)); 
        m_OldWndProc = ::DefWindowProc;
        m_bSubClassed = false;
    }

    HWND YWindowWnd::SubClass(HWND hWnd)
    {
        assert(::IsWindow(hWnd));
        assert(m_hWnd == NULL);
        //把当前YWindowWnd的WndProc设为hWnd的WndProc,之前的保存;
        m_OldWndProc = (WNDPROC)SetWindowLongPtr((hWnd), GWLP_WNDPROC, (LPARAM)(WNDPROC)(YWindowWnd::WndProc));
        if( m_OldWndProc == NULL )
            return NULL;
        m_bSubClassed = true;
        m_hWnd = hWnd;
        ::SetWindowLongPtr( hWnd, GWLP_USERDATA , reinterpret_cast<LPARAM>(this));
        return m_hWnd;
    }

    void YWindowWnd::OnFinalMessage(HWND /*hWnd*/)
    {

    }

    LRESULT YWindowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return ::CallWindowProc( m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
    }

    

    LPCTSTR YWindowWnd::GetSuperClassName() const
    {
        return NULL;
    }

    LRESULT CALLBACK YWindowWnd::ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        YWindowWnd *pWnd = nullptr;
        if( uMsg == WM_NCCREATE )
        {
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>( lParam );
            pWnd = static_cast<YWindowWnd*>( lpcs->lpCreateParams );
            //之前的窗口的附加指针已经被占用，只能用附加Prop
            ::SetProp( hWnd, _T("YYProperty"), (HANDLE)pWnd );
            pWnd->m_hWnd = hWnd;
        }
        else
        {
            pWnd = reinterpret_cast<YWindowWnd *>(::GetProp(hWnd, _T("YYProperty")));
            if( uMsg == WM_NCDESTROY && pWnd != nullptr )
            {
                LRESULT lRes = ::CallWindowProc( pWnd->m_OldWndProc, hWnd, uMsg, wParam, lParam );
                if( pWnd->m_bSubClassed )
                    pWnd->UnsubClass();
                ::SetProp( hWnd, _T("YYProperty"), NULL );
                pWnd->m_hWnd = NULL;
                pWnd->OnFinalMessage( hWnd );
                return lRes;
            }
        }
        if( pWnd != nullptr )
            return pWnd->HandleMessage(uMsg, wParam, lParam);
        else
            return ::DefWindowProc( hWnd, uMsg, wParam, lParam);
    }

    

    void YWindowWnd::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= true*/)
    {
        assert( ::IsWindow( m_hWnd ) );
        if( !::IsWindow( m_hWnd ) )
            return;
        ::ShowWindow(m_hWnd, bShow? ( bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE ) : SW_HIDE);
    }

    UINT YWindowWnd::ShowModal()
    {
        assert(::IsWindow(m_hWnd));
        UINT nRet = 0;
        HWND hWndParent = ::GetWindow(m_hWnd,GW_OWNER);
        ::ShowWindow(m_hWnd , SW_SHOWNORMAL );
        ::EnableWindow(hWndParent, FALSE);
        MSG msg = { 0 };

        while( ::IsWindow( m_hWnd ) && ::GetMessage( &msg, NULL, 0, 0) )
        {
            if( msg.message == WM_CLOSE  && msg.hwnd == m_hWnd )
            {
                nRet =(UINT) msg.wParam; 
                ::EnableWindow( hWndParent, TRUE ) ;
                ::SetFocus( hWndParent );
            }
            if( !YPaintManagerUI::TranslateMessage(& msg ) )
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if( msg.message == WM_QUIT )
                    break;
            }
        }
        ::EnableWindow(hWndParent,TRUE);
        ::SetFocus( hWndParent );
        if( msg.message == WM_QUIT)
            ::PostQuitMessage(msg.wParam);
        return nRet;
    }

    void YWindowWnd::Close(UINT nRet)
    {
        assert(::IsWindow(m_hWnd));
        if( !::IsWindow( m_hWnd ) )
            return ;
        PostMessage( WM_CLOSE, (WPARAM)nRet , 0L );
    }

    LRESULT YWindowWnd::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0L*/)
    {
        assert( ::IsWindow( m_hWnd ) );
        return ::SendMessage( m_hWnd, uMsg, wParam, lParam);
    }

    LRESULT YWindowWnd::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0L*/)
    {
        assert( ::IsWindow( m_hWnd ) );
        return ::PostMessage( m_hWnd, uMsg, wParam, lParam);
    }

    void YWindowWnd::CenterWindow()
    {
        assert( ::IsWindow( m_hWnd ));
        assert( (GetWindowLong( m_hWnd, GWL_STYLE) & WS_CHILD) == 0 );
        RECT rcFrame = { 0 };
        ::GetWindowRect(m_hWnd, &rcFrame);

        RECT rcArea = { 0 };
        RECT rcCenter = { 0 };

        HWND hWnd = *this;
        HWND hWndCenter = ::GetWindow(m_hWnd ,GW_OWNER);
        if( hWndCenter != NULL )
            hWnd = hWndCenter;

        // 处理多显示器模式下屏幕居中;
        MONITORINFO monInfo;
        ZeroMemory( &monInfo, sizeof(monInfo) );
        monInfo.cbSize = sizeof(monInfo);
        ::GetMonitorInfo(::MonitorFromWindow( hWnd, MONITOR_DEFAULTTONEAREST), &monInfo);
        rcArea = monInfo.rcWork;

        if( hWndCenter == NULL )
            rcCenter = rcArea;
        else
            ::GetWindowRect( hWndCenter, &rcCenter );

        int width = rcFrame.right - rcFrame.left;
        int height = rcFrame.bottom - rcFrame.top;

        int xLeft = (rcCenter.left + rcCenter.right - width )/2;
        int yTop = (rcCenter.top + rcCenter.bottom - height )/2;

        if( xLeft < rcArea.left ) 
            xLeft = rcArea.left;
        else if( xLeft + width > rcArea.right )
            xLeft = rcArea.right- width;

        if( yTop < rcArea.top )
            yTop = rcArea.top;
        else if( yTop + height > rcArea.bottom )
            yTop = rcArea.bottom - height;

        ::SetWindowPos( m_hWnd , NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );

    }

    void YWindowWnd::SetIcon(UINT nRes)
    {
        HICON hIcon = (HICON)::LoadImage(YPaintManagerUI::GetInstance(), 
                                        MAKEINTRESOURCE(nRes), 
                                        IMAGE_ICON,
                                        (::GetSystemMetrics(SM_CXICON) + 15) & ~15,
                                        (::GetSystemMetrics(SM_CYICON) + 15) & ~15,	// 防止高DPI下图标模糊
                                        LR_DEFAULTCOLOR);
        assert(hIcon);
        ::SendMessage( m_hWnd, WM_SETICON, (WPARAM)FALSE, (LPARAM)hIcon);
    }

    YWindowWnd::~YWindowWnd()
    {

    }


    void YNotifyPump::NotifyPump(NotifyMsg & msg)
    {
        //to do ??没看明白加了个virtual wnd做什么用
        HandleMsg(msg);
    }

   

    

    void YNotifyPump::HandleMsg(const NotifyMsg & msg)
    {
        auto strMsgType = msg.strType;
        auto spControler = msg.pSender.lock();
        if( !spControler)
            return ;
        auto strControlerName = spControler->GetName();
        YStrStr ss(strMsgType,strControlerName);
        auto pos = m_MessageMap.find(ss);
        if(pos == m_MessageMap.end())
        {
            YMsgHandleChain::HandleMsg(msg);
        }
        else
        {
            auto &func = pos->second;
            func(msg);
        }
    }


    void YMsgHandleChain::HandleMsg(const NotifyMsg & msg)
    {
        auto sp= m_wpSuccessor.lock();
        if(sp)
            sp->HandleMsg(msg);
    }

    void YMsgHandleChain::SetSuccessor(std::shared_ptr<YMsgHandleChain> & sp)
    {
        m_wpSuccessor = sp;
    }

    YMsgHandleChain::YMsgHandleChain()
    {

    }

}