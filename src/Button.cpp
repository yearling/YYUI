#include "Button.h"
#include "RenderDGI.h"
#include "ControlUI.h"
#include "WindowProperty.h"
#include "Canvas2D.h"
namespace YUI
{


    Button::Button()
        :m_uButtonState(0)
        ,m_dwHotTextColor(0)
        ,m_dwPushedTextColor(0)
        ,m_dwFocusedTextColor(0)
        ,m_dwHotBkColor(0)
    {
        m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
        AddHander();
    }

    Button::~Button()
    {
    
    }

    LPCTSTR Button::GetClass() const
    {
        return _T("ButtonUI");
    }

    std::shared_ptr<ControlUI> Button::QueryInterface(const std::string & strName)
    {
        if( strName ==  CTR_BUTTON  ) 
            return shared_from_this();
        return Label::QueryInterface(strName);
    }

    UINT Button::GetControlFlags() const
    {
        return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
    }

    bool Button::Activate()
    {
        if(m_eventClick)
            m_eventClick();
        return true;
    }

    void Button::SetEnabled(bool bEnable /*= true*/)
    {
        ControlUI::SetEnabled(bEnable);
        if( !IsEnabled() ) {
            m_uButtonState = 0;
        }
    }

    SIZE Button::EstimateSize(SIZE szAvailable)
    {
        //if( m_cXYFixed.cy == 0 ) return YSize(m_cXYFixed.cx, m_pManager->GetFontInfo(GetFont())->m_tm.tmHeight + 8);
		return ControlUI::EstimateSize(szAvailable);
    }

    void Button::SetAttribute(const std::string &strName, const std::string& strValue)
    {
        auto pstrName = strName.c_str();
        auto pstrValue = strValue.c_str();
        if( strcmp(pstrName, ("normalimage")) == 0 ) 
            SetNormalImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("hotimage")) == 0 ) 
            SetHotImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("pushedimage")) == 0 )  
            SetPushedImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("focusedimage")) == 0 ) 
            SetFocusedImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("disabledimage")) == 0 )
            SetDisabledImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("foreimage")) == 0 ) 
            SetForeImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("hotforeimage")) == 0 ) 
            SetHotForeImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("hotbkcolor")) == 0 )
        {
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetHotBkColor(clrColor);
        }
        else if( strcmp(pstrName, ("hottextcolor")) == 0 )
        {
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetHotTextColor(clrColor);
        }
        else if( strcmp(pstrName, ("pushedtextcolor")) == 0 )
        {
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetPushedTextColor(clrColor);
        }
        else if( strcmp(pstrName, ("focusedtextcolor")) == 0 )
        {
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetFocusedTextColor(clrColor);
        }
        else Label::SetAttribute(pstrName, pstrValue);
    }

    void Button::PaintText()
    {
        if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
        else m_uButtonState &= ~ UISTATE_FOCUSED;
        if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
        else m_uButtonState &= ~ UISTATE_DISABLED;

        if( m_dwTextColor == 0 ) 
            m_dwTextColor= m_pManager->GetWindowProperty()->GetDefaultFontColor();
        if( m_dwDisabledTextColor == 0 ) 
            m_dwDisabledTextColor = m_pManager->GetWindowProperty()->GetDefaultDisabledColor();

        if( m_strText.empty() ) return;
        int nLinks = 0;
        RECT rc = m_rcItem;
        rc.left += m_rcTextPadding.left;
        rc.right -= m_rcTextPadding.right;
        rc.top += m_rcTextPadding.top;
        rc.bottom -= m_rcTextPadding.bottom;

        DWORD clrColor = IsEnabled()?m_dwTextColor:m_dwDisabledTextColor;

        if( ((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedTextColor() != 0) )
            clrColor = GetPushedTextColor();
        else if( ((m_uButtonState & UISTATE_HOT) != 0) && (GetHotTextColor() != 0) )
            clrColor = GetHotTextColor();
        else if( ((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedTextColor() != 0) )
            clrColor = GetFocusedTextColor();

        Canvas2D canvas(m_pManager->GetHWND());
        FontD2D font(_T("Verdana"),10);
        canvas.DrawSolidText(m_strText,font,rc,m_dwTextColor);
    }

    void Button::PaintStatusImage()
    {
        Canvas2D canvas(m_pManager->GetHWND());

        if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
        else m_uButtonState &= ~ UISTATE_FOCUSED;
        if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
        else m_uButtonState &= ~ UISTATE_DISABLED;

        if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
            if( !m_sDisabledImage.empty() )
            {
                DrawImage(m_sDisabledImage);
            }
        }
        else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
            if( !m_sPushedImage.empty() ) {
                DrawImage(m_sPushedImage);
                //DrawImage(m_sPushedForeImage);
            }
        }
        else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
            if( !m_sHotImage.empty() ) {
                DrawImage(m_sHotImage);
                //DrawImage(m_sHotForeImage);
            }
            else if(m_dwHotBkColor != 0) {
                canvas.FillRect(m_rcPaint,m_dwHotBkColor);
                return;
            }
        }
        else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
            if( !m_sFocusedImage.empty() ) {
                DrawImage(m_sFocusedImage);
            }
        }

        if( !m_sNormalImage.empty() ) {
            DrawImage(m_sNormalImage);
        }

    }

    YUI::YString Button::GetNormalImage()
    {
        return m_sNormalImage;
    }

    void Button::SetNormalImage(const YString& pStrImage)
    {
        m_sNormalImage = pStrImage;
        Invalidate();
    }

    YUI::YString Button::GetHotImage()
    {
        return m_sHotImage;
    }

    void Button::SetHotImage(const YString& pStrImage)
    {
        m_sHotImage = pStrImage;
        Invalidate();
    }

    YUI::YString Button::GetPushedImage()
    {
        return m_sPushedImage;
    }

    void Button::SetPushedImage(const YString& pStrImage)
    {
        m_sPushedImage = pStrImage;
        Invalidate();
    }

    YUI::YString Button::GetFocusedImage()
    {
        return m_sFocusedImage;
    }

    void Button::SetFocusedImage(const YString& pStrImage)
    {
        m_sFocusedImage = pStrImage;
        Invalidate();
    }

    YUI::YString Button::GetDisabledImage()
    {
        return m_sDisabledImage;
    }

    void Button::SetDisabledImage(const YString& pStrImage)
    {
        m_sDisabledImage = pStrImage;
        Invalidate();
    }

    YUI::YString Button::GetForeImage()
    {
        return m_strForeImage;
    }

    void Button::SetForeImage(const YString& pStrImage)
    {
        m_strForeImage = pStrImage;
        Invalidate();
    }

    YUI::YString Button::GetHotForeImage()
    {
        return m_sHotForeImage;
    }

    void Button::SetHotForeImage(const YString& pStrImage)
    {
        m_sHotForeImage = pStrImage;
        Invalidate();
    }

    void Button::SetHotBkColor(DWORD dwColor)
    {
        m_dwHotBkColor = dwColor;
    }

    DWORD Button::GetHotBkColor() const
    {
        return m_dwHotBkColor;
    }

    void Button::SetHotTextColor(DWORD dwColor)
    {
        m_dwHotTextColor = dwColor;
    }

    DWORD Button::GetHotTextColor() const
    {
        return m_dwHotTextColor;
    }

    void Button::SetPushedTextColor(DWORD dwColor)
    {
        m_dwPushedTextColor = dwColor;
    }

    DWORD Button::GetPushedTextColor() const
    {
        return m_dwPushedTextColor;
    }

    void Button::SetFocusedTextColor(DWORD dwColor)
    {
        	m_dwFocusedTextColor = dwColor;
    }

    DWORD Button::GetFocusedTextColor() const
    {
        return m_dwFocusedTextColor;
    }

    void Button::AddHander()
    {
        m_pButtonMsgHandler = std::make_shared<IMsgHandler>();
        m_pButtonMsgHandler->SetSuccessor(m_pLabelMsgHandler);
        
       m_pButtonMsgHandler->AddEntry(UIMSG_SETFOCUS,[&](const MsgWrap &msg)
        {
            Invalidate();
           m_pButtonMsgHandler->MsgHandleChainBase::HandleMsg(msg);
        });
        m_pButtonMsgHandler->AddEntry(UIMSG_KILLFOCUS,[&](const MsgWrap &msg)
        {
            Invalidate();
           m_pButtonMsgHandler->MsgHandleChainBase::HandleMsg(msg);
        });
        
        m_pButtonMsgHandler->AddEntry(UIMSG_KEYDOWN,[&](const MsgWrap &msg)
        {
            if (IsKeyboardEnabled()) {
                if( msg.wParam == VK_SPACE || msg.wParam == VK_RETURN ) {
                    Activate();
                }
            }
        });

        m_pButtonMsgHandler->AddEntry(UIMSG_LBUTTONDOWN,[&](const MsgWrap &msg)
        {
            POINT pt= { GET_X_LPARAM(msg.lParam),GET_Y_LPARAM(msg.lParam)};
            if( ::PtInRect(&m_rcItem, pt) && IsEnabled() ) 
            {
                m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
                Invalidate();
            }
        });

        m_pButtonMsgHandler->AddEntry(UIMSG_DBLCLICK,[&](const MsgWrap &msg)
        {
            POINT pt= { GET_X_LPARAM(msg.lParam),GET_Y_LPARAM(msg.lParam)};
            if( ::PtInRect(&m_rcItem, pt) && IsEnabled() ) 
            {
                m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
                Invalidate();
            }
        });

        m_pButtonMsgHandler->AddEntry(UIMSG_MOUSEMOVE,[&](const MsgWrap &msg)
        {
            POINT pt= { GET_X_LPARAM(msg.lParam),GET_Y_LPARAM(msg.lParam)};
            if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) 
            {
                if( ::PtInRect(&m_rcItem, pt) ) m_uButtonState |= UISTATE_PUSHED;
                else m_uButtonState &= ~UISTATE_PUSHED;
                Invalidate();
            }
        });

        m_pButtonMsgHandler->AddEntry(UIMSG_LBUTTONUP,[&](const MsgWrap &msg)
        {
            POINT pt= { GET_X_LPARAM(msg.lParam),GET_Y_LPARAM(msg.lParam)};
            if( (m_uButtonState & UISTATE_CAPTURED) != 0 )
            {
                if( ::PtInRect(&m_rcItem, pt ) )
                    Activate();
                m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
                Invalidate();
            }
        }); 
       
        m_pButtonMsgHandler->AddEntry(UIMSG_MOUSEENTER,[&](const MsgWrap &msg)
        {
            if( IsEnabled() ) 
            {
                m_uButtonState |= UISTATE_HOT;
                Invalidate();
            }
           m_pButtonMsgHandler->MsgHandleChainBase::HandleMsg(msg);
        }); 
        
        m_pButtonMsgHandler->AddEntry(UIMSG_MOUSELEAVE,[&](const MsgWrap &msg)
        {
            if( IsEnabled() ) 
            {
                m_uButtonState &= ~UISTATE_HOT;
                Invalidate();
            }
            m_pButtonMsgHandler->MsgHandleChainBase::HandleMsg(msg);
        }); 

        m_pButtonMsgHandler->AddEntry(UIMSG_SETCURSOR,[&](const MsgWrap &msg)
        {
           ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
        });

        m_eventClick = [](){ ::MessageBeep(MB_OK);};
    }

    void Button::HandleMsg(const MsgWrap & msg) throw()
    {
        assert(m_pButtonMsgHandler);
        m_pButtonMsgHandler->HandleMsg(msg);
    }

  }