#include "YUI.h"
#include "ControlManager.h"
#include "ControlUI.h"
namespace YUI
{

    ControlManager::ControlManager(void)
        :m_bNeedUpdate(false)
    {
    }


    ControlManager::~ControlManager(void)
    {
    }

    void ControlManager::Invalidate(const YYRECT &rc)
    {
        ::InvalidateRect(m_hWnd,&((RECT)rc),FALSE);
    }

    YUI::SPControlUI ControlManager::GetFocus() const
    {
        return m_pFocus;
    }

    void ControlManager::SetFocus(SPControlUI &pControl)
    {
        HWND hFocusWnd = ::GetFocus();
        //�õ���ǰFocus�Ľ���
        //������㲻Ϊ��ǰ��hWnd
        if( hFocusWnd != m_hWnd && pControl != m_pFocus) 
            ::SetFocus(m_hWnd);
        if(pControl == m_pFocus )
            return;

        if( m_pFocus != NULL)
        {
            MsgWrap msg;
            msg.strType = UIMSG_KILLFOCUS;
            msg.pSender = pControl;
            msg.lTimeStamp = ::GetTickCount();
            SendMsg(m_pFocus,msg);
            m_pFocus = nullptr;
        }
        if(pControl == nullptr )
            return;

        if(pControl != nullptr && pControl->IsVisible() && pControl->IsEnabled())
        {
            assert(pControl->GetManager() == shared_from_this() && "impossible");
            m_pFocus = pControl;
            MsgWrap msg;
            msg.strType = UIMSG_SETFOCUS;
            msg.pSender = pControl;
            msg.lTimeStamp = ::GetTickCount();
            SendMsg(pControl,msg);
        }
    }

    void ControlManager::SendMsg(SPControlUI pControl,const MsgWrap &msg)
    {
        pControl->HandleMsg(msg);
    }

    YUI::SPControlUI ControlManager::GetRoot() const
    {
       return  m_pRoot;
    }

    void ControlManager::InitControls(std::shared_ptr<ControlUI> &pControl,std::weak_ptr<ControlUI> parent)
    {
        assert(pControl);
        if( !pControl )
            return;
        if(parent.lock())
            pControl->SetManager(this->shared_from_this(),parent,true);
        else
            pControl->SetManager(this->shared_from_this(),pControl->GetParent(),true);
    }

    void ControlManager::AttachDialog(std::shared_ptr<ControlUI> pControl)
    {
        assert( ::IsWindow(m_hWnd) );
        ::SetFocus(NULL);
        m_pRoot = pControl;
        m_bNeedUpdate = true;
        return InitControls(pControl,std::shared_ptr<ControlUI>());
    }

    bool ControlManager::MessageHandler(UINT uMesg, WPARAM wParam, LPARAM lParam, LRESULT &lRes)
    {
        if(m_hWnd == NULL )
            return false;
        switch (uMesg)
        {
        case WM_CLOSE:
            {
                MsgWrap msg;
                msg.ptMouse = m_ptLastMousePos;
                msg.lTimeStamp = ::GetTickCount();
                if( m_pHover )
                {
                    //��ǰ��ͣ�Ŀؼ�����mouse_leave����Ϣ
                    msg.strType = UIMSG_MOUSELEAVE;
                    msg.pSender = m_pHover;
                    SendMsg(m_pHover,msg);
                }
                if( m_pClick)
                {
                    //��ǰClick�Ŀؼ�����Buttonup����Ϣ
                    msg.strType = UIMSG_LBUTTONUP;
                    msg.pSender = m_pClick;
                    SendMsg(m_pClick,msg);
                }
                ::SetFocus(NULL);
                //��������Ϊ����
                HWND hWndParent = GetWindowOwner(m_hWnd);
                if( hWndParent != NULL )
                {
                    ::SetFocus(hWndParent);
                }
            }
            break;
        case WM_ERASEBKGND:
            {
                lRes = 1;
            }
            break;
        case WM_PAINT:
            {
                RECT rcPaint = { 0 };
                //����Ҫ����
                if( ! ::GetUpdateRect(m_hWnd,&rcPaint,FALSE) )
                     return true;
                if( m_pRoot == NULL)
                {
                    return true;
                }

				if( m_bNeedUpdate ) 
				{
					m_bNeedUpdate = false;
					RECT rcClient = { 0 };
					::GetClientRect(m_hWnd, &rcClient);
					if( !::IsRectEmpty(&rcClient) ) 
					{
						if( m_pRoot->IsUpdateNeeded() ) 
						{
							m_pRoot->SetPos(rcClient);
						}
					}
				}
					m_pRoot->DoPaint();
			}
			// If any of the painting requested a resize again, we'll need
			// to invalidate the entire window once more.
			if( m_bNeedUpdate ) 
			{
				::InvalidateRect(m_hWnd, NULL, FALSE);
			}
			return true;
        case WM_SIZE:
            {
                if(m_pFocus)
                {
                    MsgWrap msg;
                    msg.strType= UIMSG_WINDOWSIZE;
                    msg.pSender = m_pFocus;
                    msg.lTimeStamp = ::GetTickCount();
                    SendMsg(m_pFocus,msg);
                }
                if( m_pRoot )
                    m_pRoot->NeedUpdate();
            }
            return true;
        case WM_TIMER:
            {

            }
            break;

        case WM_MOUSEHOVER:
            {
                POINT pt= { GET_X_LPARAM( lParam ), GET_Y_LPARAM(lParam) };
                auto spControl = FindControl(pt);
                if(! spControl)
                    break;
                if( m_pHover )
                {
                    MsgWrap msg;
                    msg.strType = UIMSG_MOUSEHOVER;
                    msg.ptMouse = pt;
                    msg.pSender = m_pHover;
                    msg.lTimeStamp = ::GetTickCount();
                    SendMsg(m_pHover,msg);
                }
            }
            return true;
            break;
        case WM_MOUSELEAVE:
            {
                if( m_bMouseTracking ) ::SendMessage(m_hWnd, WM_MOUSEMOVE, 0, (LPARAM) -1);
            }
            break;
        case WM_MOUSEMOVE:
            {
               
                POINT pt= { GET_X_LPARAM( lParam) ,GET_Y_LPARAM(lParam) };
                m_ptLastMousePos = pt;
                std::shared_ptr<ControlUI> spNewHover = FindControl(pt);
                if( spNewHover != NULL && spNewHover->GetManager() != this->shared_from_this() )
                    break;
                MsgWrap msg;
                msg.strType = UIMSG_MOUSELEAVE;
                msg.lTimeStamp = ::GetTickCount();
                msg.ptMouse = pt;
                //�¾�hover�Ĳ�һ�������ɵķ���mouse leave,���µķ���mousehover
                if(spNewHover != m_pHover && m_pHover)
                {
                    msg.pSender = m_pHover;
                    SendMsg(m_pHover,msg);
                    m_pHover = NULL;
                }
                //�¾ɵ�hover��һ�����µĴ���
                if(spNewHover != m_pHover && spNewHover)
                {
                    msg.strType = UIMSG_MOUSEENTER;
                    msg.pSender = spNewHover;
                    SendMsg(spNewHover ,msg);
                    m_pHover = spNewHover;
                }
                //����Ŀؼ���Ϊ��
                if(m_pClick != NULL )
                {
                    msg.strType = UIMSG_MOUSEMOVE;
                    msg.pSender = m_pClick;
                    SendMsg(m_pClick,msg);
                }
                else if( spNewHover )
                {
                    msg.strType = UIEVENT_MOUSEMOVE;
                    msg.pSender = spNewHover;
                    SendMsg(spNewHover,msg);
                }

            }
            break;
        case WM_LBUTTONDOWN:
            {
                ::SetFocus(m_hWnd);
                POINT pt= { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                auto spControl = FindControl(pt);
                if( spControl == NULL )
                    break;
                if( spControl->GetManager() != shared_from_this())
                    break;
                m_pClick = spControl;
                spControl->SetFocus();
                SetCapture();
                MsgWrap msg;
                msg.strType = UIMSG_LBUTTONDOWN;
                msg.pSender = spControl;
                msg.wParam = wParam;
                msg.lParam = lParam;
                msg.ptMouse = pt;
                msg.lTimeStamp = ::GetTickCount();
                SendMsg(spControl,msg);
            }
            break;
        case WM_LBUTTONDBLCLK:
            {
                ::SetFocus(m_hWnd);
                POINT pt= { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                auto spControl = FindControl(pt);
                if( spControl == NULL )
                    break;
                if( spControl->GetManager() != shared_from_this())
                    break;
                SetCapture();
                MsgWrap msg;
                msg.strType = UIMSG_DBLCLICK;
                msg.pSender = spControl;
                msg.wParam = wParam;
                msg.lParam = lParam;
                msg.ptMouse = pt;
                msg.lTimeStamp = ::GetTickCount();
                SendMsg(spControl,msg);
                m_pClick = spControl;
            }
            break;
        case WM_LBUTTONUP:
            {
                POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                m_ptLastMousePos = pt;
                if( m_pClick == NULL ) break;
                ReleaseCapture();
                MsgWrap msg;
                msg.strType = UIMSG_LBUTTONUP;
                msg.pSender = m_pClick;
                msg.wParam = wParam;
                msg.lParam = lParam;
                msg.ptMouse = pt;
                msg.lTimeStamp = ::GetTickCount();
                SendMsg(m_pClick,msg);
                m_pClick = NULL;
            }
            break;
        case WM_RBUTTONDOWN:
            {
                ::SetFocus(m_hWnd);
                POINT pt= { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                auto spControl = FindControl(pt);
                if( spControl == NULL )
                    break;
                if( spControl->GetManager() != shared_from_this())
                    break;
                spControl->SetFocus();
                SetCapture();
                MsgWrap msg;
                msg.strType = UIMSG_RBUTTONDOWN;
                msg.pSender = spControl;
                msg.wParam = wParam;
                msg.lParam = lParam;
                msg.ptMouse = pt;
                msg.lTimeStamp = ::GetTickCount();
                SendMsg(spControl,msg);
                m_pClick = spControl;
            }
            break;
        case WM_CONTEXTMENU:
            {
              /*  POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
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
                m_pEventClick = NULL;*/
            }
            break;
        case WM_MOUSEWHEEL:
            {
                ::SetFocus(m_hWnd);
                POINT pt= { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                auto spControl = FindControl(pt);
                if( spControl == NULL )
                    break;
                if( spControl->GetManager() != shared_from_this())
                    break;
                int ZDelta = (int)(short)HIWORD(wParam);
                MsgWrap msg;
                msg.strType = UIMSG_MOUSEWHEEL;
                msg.pSender = spControl;
                msg.wParam =MAKELPARAM( ZDelta <0 ? SB_LINEDOWN :SB_LINEUP  ,0 ); 
                msg.lParam = lParam;
                msg.lTimeStamp = ::GetTickCount();
                SendMsg(spControl,msg);
                // Let's make sure that the scroll item below the cursor is the same as before...
                ::SendMessage(m_hWnd, WM_MOUSEMOVE, 0,(LPARAM)MAKELPARAM( m_ptLastMousePos.x, m_ptLastMousePos.y));
            }
            break;
        case WM_CHAR:
            {
                if( ! m_pFocus )
                    break;
                MsgWrap msg;
                msg.strType = UIMSG_CHAR;
                msg.wParam = wParam;
                msg.lParam = lParam;
                msg.ptMouse = m_ptLastMousePos;
                msg.lTimeStamp = ::GetTickCount();
                SendMsg(m_pFocus,msg);
            }
            break;
        case WM_KEYDOWN:
            {
                if( ! m_pFocus )
                    break;
                MsgWrap msg;
                msg.strType = UIMSG_KEYDOWN;
                msg.wParam = wParam;
                msg.lParam = lParam;
                msg.ptMouse = m_ptLastMousePos;
                msg.lTimeStamp = ::GetTickCount();
                SendMsg(m_pFocus,msg);
            }
            break;
        case WM_KEYUP:
            {
                if( ! m_pFocus )
                    break;
                MsgWrap msg;
                msg.strType = UIMSG_KEYUP;
                msg.wParam = wParam;
                msg.lParam = lParam;
                msg.ptMouse = m_ptLastMousePos;
                msg.lTimeStamp = ::GetTickCount();
                SendMsg(m_pFocus,msg);
            }
            break;
        case WM_SETCURSOR:
            {
                if( LOWORD(lParam) != HTCLIENT ) break;

                POINT pt = { 0 };
                ::GetCursorPos(&pt);
                ::ScreenToClient(m_hWnd, &pt);
                auto spControl = FindControl(pt);
                if( spControl == NULL )
                    break;
                if( (spControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0 ) break;
                MsgWrap msg;
                msg.strType = UIMSG_SETCURSOR;
                msg.wParam = wParam;
                msg.lParam =lParam;
                msg.ptMouse = pt;
                msg.lTimeStamp = ::GetTickCount();
                SendMsg(spControl,msg);
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
    }

    YUI::SPControlUI ControlManager::FindControl(const YYPOINT &pt)
    {
        return m_pRoot->FindControlFromPoint(pt,UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
    }

    void ControlManager::SetCapture()
    {
        ::SetCapture(m_hWnd);
        m_bMouseCapture = true;
    }

    void ControlManager::ReleaseCapture()
    {
        ::ReleaseCapture();
        m_bMouseCapture = false;
    }

}