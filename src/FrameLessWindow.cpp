#include "YUI.h"
#include "FrameLessWindow.h"
#include <windows.h>
#include "DlgBuilder.h"
#include "ControlUI.h"
using std::cout;
using std::wcout;
using std::endl;
namespace YUI
{


    FrameLessWindow::FrameLessWindow():m_spControlManger(new ControlManager)
    {
    }

    FrameLessWindow::~FrameLessWindow()
    {

    }

    LRESULT FrameLessWindow::OnSysMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRes = 0;
        BOOL bHandled = TRUE;
        switch (uMsg)
        {
        case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
        case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
        case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
        case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
        case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
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

        if (m_spControlManger->MessageHandler(uMsg, wParam, lParam, lRes))
            return lRes;
        return WindowWnd::OnSysMessage(uMsg, wParam, lParam);
    }

    void FrameLessWindow::HandleMsg(const MsgWrap & msg) throw()
    {

    }

    void FrameLessWindow::InitWindow()
    {

    }

    LRESULT FrameLessWindow::OnClose(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT FrameLessWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT FrameLessWindow::OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if( ::IsIconic(*this) ) bHandled = FALSE;
        return (wParam == 0) ? TRUE : FALSE;
    }

    LRESULT FrameLessWindow::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
            YYRECT rcWork = oMonitor.rcWork;
            YYRECT rcMonitor = oMonitor.rcMonitor;
            rcWork.OffSet(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

            pRect->right = pRect->left + rcWork.GetWidth();
            pRect->bottom = pRect->top + rcWork.GetHeight();
            return WVR_REDRAW;
        }
        return 0;
    }

    LRESULT FrameLessWindow::OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 0;
    }

    LRESULT FrameLessWindow::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        POINT pt; 
        pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
        ::ScreenToClient(*this, &pt);

        RECT rcClient;
        ::GetClientRect(*this, &rcClient);

        if( !::IsZoomed(*this) )
        {
            YYRECT rcSizeBox = m_Property.GetSizeBox();
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

        YYRECT rcCaption = m_Property.GetCaptionRect();
        //cout<<"hello"<<endl;
        if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right && pt.y >= rcCaption.top && pt.y < rcCaption.bottom )
        {
            //auto pControl = FindControl(pt);
            ////用来防止把点击右上角的maxSize，close等button当成点击标题栏
            //if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
            //    _tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
            //    _tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 )
                return HTCAPTION;
        }

        return HTCLIENT;
    }

    LRESULT FrameLessWindow::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;

        MONITORINFO oMonitor = {};
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
        YYRECT rcWork = oMonitor.rcWork;
        YYRECT rcMonitor = oMonitor.rcMonitor;
        rcWork.OffSet(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

        // 计算最大化时，正确的原点坐标
        lpMMI->ptMaxPosition.x	= rcWork.left;
        lpMMI->ptMaxPosition.y	= rcWork.top;

        lpMMI->ptMaxTrackSize.x =rcWork.GetWidth();
        lpMMI->ptMaxTrackSize.y =rcWork.GetHeight();

        lpMMI->ptMinTrackSize.x =(int)m_Property.GetMinInfo().width;
        lpMMI->ptMinTrackSize.y =(int)m_Property.GetMinInfo().height;

        bHandled = FALSE;
        return 0;
    }

    LRESULT FrameLessWindow::OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT FrameLessWindow::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT FrameLessWindow::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT FrameLessWindow::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT FrameLessWindow::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (wParam == SC_CLOSE)
        {
            bHandled = TRUE;
            SendMessage(WM_CLOSE);
            return 0;
        }
        BOOL bZoomed = ::IsZoomed(*this);
        LRESULT lRes = WindowWnd::OnSysMessage(uMsg, wParam, lParam);
        return lRes;
    }
   
#if 1
    LRESULT FrameLessWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
        DialogBuilder builder;
        m_Property.Init(m_hWnd);
        std::shared_ptr<ControlUI> pRoot;
        std::shared_ptr<ControlUI> spNull;
        pRoot=builder.Create(_T("duilib.xml"),m_Property,nullptr,spNull);
        m_spControlManger->Init(m_hWnd);
        m_spControlManger->AttachDialog(pRoot);
        InitWindow();
        Ycout<<"FrameLess: OnCreate"<<endl;
        bHandled = FALSE;
        return 0;
    }
#endif
    LRESULT FrameLessWindow::OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        Ycout<<"FrameLess: OnKeyDown"<<endl;
        return 0;
    }

    LRESULT FrameLessWindow::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT FrameLessWindow::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT FrameLessWindow::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT FrameLessWindow::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT FrameLessWindow::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    UINT FrameLessWindow::GetClassStyle() const
    {
         //return CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
         return CS_DBLCLKS;
    }

    LONG FrameLessWindow::GetStyle()
    {
        LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
        styleValue &= ~WS_CAPTION;

        return styleValue;
    }

    LRESULT FrameLessWindow::OnSysKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        switch (wParam)
        {
        case VK_F4:
               Close(0);
               bHandled =TRUE;
               break;
        default:
            break;
        }
        bHandled = FALSE;
        return 0;
    }

}