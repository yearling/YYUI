
#include "YUI.h"
#include "WindowImpl.h"
#include "UIUtility.h"
#include <Windowsx.h>
namespace YUI
{

    WindowImpl::WindowImpl()
    {
        m_spPaintManager = std::make_shared<PaintManagerUI>();
    }

    WindowImpl::~WindowImpl()
    {

    }

    void WindowImpl::OnFinalMessage(HWND hWnd)
    {
        
    }

    void WindowImpl::InitWindow()
    {

    }

    void WindowImpl::Notify(NotifyMsg & msg)
    {
        return INotifyPump::NotifyPump(msg);
    }

    void WindowImpl::OnClick(NotifyMsg & msg)
    {
        auto sp = msg.pSender.lock();
        if(!sp)
            return;
        YString sCtrlName = sp->GetName();
        if( sCtrlName == _T("closebtn") )
        {
            Close();
            return; 
        }
        else if( sCtrlName == _T("minbtn"))
        { 
            SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
            return; 
        }
        else if( sCtrlName == _T("maxbtn"))
        { 
            SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
            return; 
        }
        else if( sCtrlName == _T("restorebtn"))
        { 
            SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
            return; 
        }
        return;
    }

    LRESULT WindowImpl::ResponseDefaultKeyEvent(WPARAM wParam)
    {
        if( wParam == VK_RETURN )
        {
            return FALSE;
        }
        else if(wParam == VK_ESCAPE)
        {
            Close();
            return TRUE;
        }
        return FALSE;
    }

    UINT WindowImpl::GetClassStyle() const
    {
        return CS_DBLCLKS;
    }

    YUI::UILIB_RESOURCETYPE WindowImpl::GetResourceType() const
    {
        return UILIB_FILE;
    }

    YString WindowImpl::GetZIPFileName() const
    {
        return _T("");
    }

    YString WindowImpl::GetResourceID() const
    {
        return _T("");
    }

    std::shared_ptr<ControlUI> WindowImpl::CreateControl(const YString & strClassName)
    {
        return nullptr;
    }

    std::shared_ptr<ControlUI> WindowImpl::CreateControl(YString pstrClass)
    {
        return NULL;
    }

    LRESULT WindowImpl::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/)
    {
        if (uMsg == WM_KEYDOWN)
        {
            switch (wParam)
            {
            case VK_RETURN:
            case VK_ESCAPE:
                return ResponseDefaultKeyEvent(wParam);
            default:
                break;
            }
        }
        return FALSE;
    }

    LRESULT WindowImpl::OnClose(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if( ::IsIconic(*this) ) bHandled = FALSE;
        return (wParam == 0) ? TRUE : FALSE;
    }

    LRESULT WindowImpl::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPRECT pRect=NULL;
        if ( wParam == TRUE)
        {
            LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;
            pRect=&pParam->rgrc[0];
        }
        else
        {
            pRect=(LPRECT)lParam;
        }

        if( ::IsZoomed( m_hWnd ))
        {
            MONITORINFO oMonitor = {};
            oMonitor.cbSize = sizeof(oMonitor);
            ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
            YRect rcWork = oMonitor.rcWork;
            YRect rcMonitor = oMonitor.rcMonitor;
            rcWork.OffSet(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

            pRect->right = pRect->left + rcWork.GetWidth();
            pRect->bottom = pRect->top + rcWork.GetHeight();
            return WVR_REDRAW;
        }
        return 0;
    }

    LRESULT WindowImpl::OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 0;
    }

    LRESULT WindowImpl::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        POINT pt; 
        pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
        ::ScreenToClient(*this, &pt);

        RECT rcClient;
        ::GetClientRect(*this, &rcClient);

        if( !::IsZoomed(*this) )
        {
            RECT rcSizeBox = m_spPaintManager->GetSizeBox();
            if( pt.y < rcClient.top + rcSizeBox.top )
            {
                if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
                if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
                return HTTOP;
            }
            else if( pt.y > rcClient.bottom - rcSizeBox.bottom )
            {
                if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
                if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
                return HTBOTTOM;
            }

            if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
            if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
        }

        RECT rcCaption = m_spPaintManager->GetCaptionRect();
        if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right && pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
                auto pControl = m_spPaintManager->FindControl(pt);
                if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
                    _tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
                    _tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 )
                    return HTCAPTION;
        }

        return HTCLIENT;
    }

    LRESULT WindowImpl::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;

        MONITORINFO oMonitor = {};
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
        YRect rcWork = oMonitor.rcWork;
        YRect rcMonitor = oMonitor.rcMonitor;
        rcWork.OffSet(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

        // 计算最大化时，正确的原点坐标
        lpMMI->ptMaxPosition.x	= rcWork.left;
        lpMMI->ptMaxPosition.y	= rcWork.top;

        lpMMI->ptMaxTrackSize.x =rcWork.GetWidth();
        lpMMI->ptMaxTrackSize.y =rcWork.GetHeight();

        lpMMI->ptMinTrackSize.x =m_spPaintManager->GetMinInfo().cx;
        lpMMI->ptMinTrackSize.y =m_spPaintManager->GetMinInfo().cy;

        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        SIZE szRoundCorner = m_spPaintManager->GetRoundCorner();
        if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
            YRect rcWnd;
            ::GetWindowRect(*this, &rcWnd);
            rcWnd.OffSet(-rcWnd.left, -rcWnd.top);
            rcWnd.right++; rcWnd.bottom++;
            HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
            ::SetWindowRgn(*this, hRgn, TRUE);
            ::DeleteObject(hRgn);
        }
        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (wParam == SC_CLOSE)
        {
            bHandled = TRUE;
            SendMessage(WM_CLOSE);
            return 0;
        }

        BOOL bZoomed = ::IsZoomed(*this);
        LRESULT lRes = WindowWnd::HandleMessage(uMsg, wParam, lParam);
        if( ::IsZoomed(*this) != bZoomed )
        {
            //add by yyCom to fix the 当最大化时，没有最大化的icon
            auto pbtnMax     = m_spPaintManager->FindControl(_T("maxbtn"));       // 最大化按钮
            auto pbtnRestore = m_spPaintManager->FindControl(_T("restorebtn"));   // 还原按钮

            // 切换最大化按钮和还原按钮的状态
            if (pbtnMax && pbtnRestore)
            {
                pbtnMax->SetVisible(TRUE == bZoomed);       // 此处用表达式是为了避免编译器BOOL转换的警告
                pbtnRestore->SetVisible(FALSE == bZoomed);
            }
        }
        return lRes;
    }

    LRESULT WindowImpl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
        //去掉标题栏
        styleValue &= ~WS_CAPTION;
        //WS_CLIPSIBLIGS是指两个子窗口重叠的时候，不主动绘制重叠的部分
        //WS_CLIPCHLDREN是指父窗口不主动更新被子窗口重叠的问题，交由子窗口绘制
        ::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        RECT rcClient;
        ::GetClientRect(*this, &rcClient);
        ::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

        m_spPaintManager->Init(m_hWnd);
        m_spPaintManager->AddPreMessageFilter(shared_from_this());
        
        DialogBuilder builder;
        if (m_spPaintManager->GetResourcePath().empty())
        {	// 允许更灵活的资源路径定义
            YString strResourcePath=m_spPaintManager->GetInstancePath();
            strResourcePath+=GetSkinFolder();
            m_spPaintManager->SetResourcePath(strResourcePath);
        }

        switch(GetResourceType())
        {
        case UILIB_ZIP:
            m_spPaintManager->SetResourceZip(GetZIPFileName(), true);
            break;
        case UILIB_ZIPRESOURCE:
            {
                HRSRC hResource = ::FindResource(m_spPaintManager->GetResourceDll(), GetResourceID().c_str(), _T("ZIPRES"));
                if( hResource == NULL )
                    return 0L;
                DWORD dwSize = 0;
                HGLOBAL hGlobal = ::LoadResource(m_spPaintManager->GetResourceDll(), hResource);
                if( hGlobal == NULL ) 
                {
                    ::FreeResource(hResource);
                    return 0L;
                }
                dwSize = ::SizeofResource(m_spPaintManager->GetResourceDll(), hResource);
                if( dwSize == 0 )
                    return 0L;
#if 0
                m_lpResourceZIPBuffer = new BYTE[ dwSize ];
                if (m_lpResourceZIPBuffer != NULL)
                {
                    ::CopyMemory(m_lpResourceZIPBuffer, (LPBYTE)::LockResource(hGlobal), dwSize);
                }
                ::FreeResource(hResource);
                m_PaintManager.SetResourceZip(m_lpResourceZIPBuffer, dwSize);
#endif
            }
            break;
        }
        try
        {

            std::shared_ptr<ControlUI> pRoot;
            std::shared_ptr<ControlUI> spNull;
            if (GetResourceType()==UILIB_RESOURCE)
            {

                pRoot = builder.Create(GetSkinFile(), _T("xml"),NULL, m_spPaintManager,spNull);
            }
            else
                pRoot = builder.Create(GetSkinFile(), (UINT)0, NULL, m_spPaintManager,spNull);
        }        
        catch(YYUIException &e)
        {
            UNREFERENCED_PARAMETER(e);
            Ycout<<_T("catched in main")<<std::endl;
            std::cout<<YYCOM::CurrentExceptionDiagnosticInformation();
        }
#if 0
        ASSERT(pRoot);
        if (pRoot==NULL)
        {
            MessageBox(NULL,_T("加载资源文件失败"),_T("Duilib"),MB_OK|MB_ICONERROR);
            ExitProcess(1);
            return 0;
        }
        m_PaintManager.AttachDialog(pRoot);
        m_PaintManager.AddNotifier(this);
        m_PaintManager.SetBackgroundTransparent(TRUE);
#endif
        InitWindow();
        return 0;
    }

    LRESULT WindowImpl::OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT WindowImpl::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRes = 0;
        BOOL bHandled = TRUE;
        switch (uMsg)
        {
        case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
        case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
        case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
        case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
        //case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
        case WM_NCPAINT:		lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
        case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
        case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
        case WM_MOUSEWHEEL:		lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); break;
        case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
        case WM_CHAR:		    lRes = OnChar(uMsg, wParam, lParam, bHandled); break;
        case WM_SYSCOMMAND:		lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
        case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
        case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
        case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
        case WM_LBUTTONUP:		lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled); break;
        case WM_LBUTTONDOWN:	lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled); break;
        case WM_MOUSEMOVE:		lRes = OnMouseMove(uMsg, wParam, lParam, bHandled); break;
        case WM_MOUSEHOVER:	    lRes = OnMouseHover(uMsg, wParam, lParam, bHandled); break;
        default:				bHandled = FALSE; break;
        }
        
        if (bHandled) return lRes;
        
        lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
        if (bHandled) return lRes;
        
        if (m_spPaintManager->MessageHandler(uMsg, wParam, lParam, lRes))
            return lRes;
        return WindowWnd::HandleMessage(uMsg, wParam, lParam);
    }
    
    LRESULT WindowImpl::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LONG WindowImpl::GetStyle()
    {
        LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
        styleValue &= ~WS_CAPTION;

        return styleValue;
    }

   

}