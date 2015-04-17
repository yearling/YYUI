
#include "YYUI.h"
#include "YWindowImpl.h"
#include "YYUtility.h"
#include <Windowsx.h>
namespace YYCOM
{

    WindowImpl::WindowImpl()
    {

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

    }

    void WindowImpl::OnClick(NotifyMsg & msg)
    {

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

    YYCOM::UILIB_RESOURCETYPE WindowImpl::GetResourceType() const
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

    std::shared_ptr<YControlUI> WindowImpl::CreateControl(const YString & strClassName)
    {
        return nullptr;
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
        LPRECT  pRect = NULL;
        if( wParam == TRUE )
        {
            LPNCCALCSIZE_PARAMS pParam = reinterpret_cast<LPNCCALCSIZE_PARAMS>(lParam);
            pRect = &pParam->rgrc[0];
        }
        else
        {
            pRect = reinterpret_cast<LPRECT>(lParam);
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
#if 0
        POINT pt; 
        pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
        ::ScreenToClient(*this, &pt);

        RECT rcClient;
        ::GetClientRect(*this, &rcClient);

        if( !::IsZoomed(*this) )
        {
            RECT rcSizeBox = m_PaintManager.GetSizeBox();
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

        RECT rcCaption = m_PaintManager.GetCaptionRect();
        if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
            && pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
                CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(pt));
                if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
                    _tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
                    _tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 )
                    return HTCAPTION;
        }

#endif
        return HTCLIENT;
    }

   

}