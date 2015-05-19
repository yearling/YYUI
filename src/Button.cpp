#include "Button.h"
#include "RenderDGI.h"
#include "ControlUI.h"

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
        if( !ControlUI::Activate() ) return false;
        //if( m_pManager != NULL ) m_pManager->SendNotify(shared_from_this(), MSG_Click);
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
        if( strcmp(pstrName, ("normalimage")) == 0 ) SetNormalImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("hotimage")) == 0 ) SetHotImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("pushedimage")) == 0 ) SetPushedImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("focusedimage")) == 0 ) SetFocusedImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("disabledimage")) == 0 ) SetDisabledImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("foreimage")) == 0 ) SetForeImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("hotforeimage")) == 0 ) SetHotForeImage(UTF8ToGBK(pstrValue));
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

    void Button::PaintText(HDC hDC)
    {
        if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
        else m_uButtonState &= ~ UISTATE_FOCUSED;
        if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
        else m_uButtonState &= ~ UISTATE_DISABLED;

        /*if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
        if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();*/

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

        /*if( m_bShowHtml )
            RenderGDI::DrawHtmlText(hDC, m_pManager, rc, m_strText, clrColor, \
            NULL, NULL, nLinks, m_uTextStyle);
        else
            RenderGDI::DrawText(hDC, m_pManager, rc, m_strText, clrColor, \
            m_Fontid, m_uTextStyle);*/
    }

    void Button::PaintStatusImage(HDC hDC)
    {
        if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
        else m_uButtonState &= ~ UISTATE_FOCUSED;
        if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
        else m_uButtonState &= ~ UISTATE_DISABLED;

        if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
            if( !m_sDisabledImage.empty() )
            {
                if( !DrawImage(hDC, m_sDisabledImage ) )
                    m_sDisabledImage.clear();
                else goto Label_ForeImage;
            }
        }
        else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
            if( !m_sPushedImage.empty() ) {
                if( !DrawImage(hDC, m_sPushedImage) ){
                    m_sPushedImage.clear();
                }
                if( !m_sPushedForeImage.empty() )
                {
                    if( !DrawImage(hDC, m_sPushedForeImage) )
                        m_sPushedForeImage.clear();
                    return;
                }
                else goto Label_ForeImage;
            }
        }
        else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
            if( !m_sHotImage.empty() ) {
                if( !DrawImage(hDC, m_sHotImage) ){
                    m_sHotImage.clear();
                }
                if( !m_sHotForeImage.empty() ) {
                    if( !DrawImage(hDC, m_sHotForeImage) )
                        m_sHotForeImage.clear();
                    return;
                }
                else goto Label_ForeImage;
            }
            else if(m_dwHotBkColor != 0) {
                //RenderGDI::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwHotBkColor));
                return;
            }
        }
        else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
            if( !m_sFocusedImage.empty() ) {
                if( !DrawImage(hDC, m_sFocusedImage) ) 
                    m_sFocusedImage.clear();
                else goto Label_ForeImage;
            }
        }

        if( !m_sNormalImage.empty() ) {
            if( !DrawImage(hDC, m_sNormalImage) ) 
                m_sNormalImage.empty();
            else goto Label_ForeImage;
        }

        if(!m_strForeImage.empty() )
            goto Label_ForeImage;

        return;

Label_ForeImage:
        if(!m_strForeImage.empty() ) {
            if( !DrawImage(hDC, m_strForeImage) ) 
                m_strForeImage.clear();
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

        AddEntry(UIMSG_SETFOCUS,[&](const MsgWrap &msg)
        {
            Invalidate();
            Label::HandleMsg(msg);
        });
        AddEntry(UIMSG_KILLFOCUS,[&](const MsgWrap &msg)
        {
            Invalidate();
            Label::HandleMsg(msg);
        });
        
        AddEntry(UIMSG_KEYDOWN,[&](const MsgWrap &msg)
        {
            if (IsKeyboardEnabled()) {
                if( msg.wParam == VK_SPACE || msg.wParam == VK_RETURN ) {
                    Activate();
                }
            }
        });

        AddEntry(UIMSG_LBUTTONDOWN,[&](const MsgWrap &msg)
        {
            POINT pt= { GET_X_LPARAM(msg.lParam),GET_Y_LPARAM(msg.lParam)};
            if( ::PtInRect(&m_rcItem, pt) && IsEnabled() ) 
            {
                m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
                Invalidate();
            }
        });

        AddEntry(UIMSG_DBLCLICK,[&](const MsgWrap &msg)
        {
            POINT pt= { GET_X_LPARAM(msg.lParam),GET_Y_LPARAM(msg.lParam)};
            if( ::PtInRect(&m_rcItem, pt) && IsEnabled() ) 
            {
                m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
                Invalidate();
            }
        });

        AddEntry(UIMSG_MOUSEMOVE,[&](const MsgWrap &msg)
        {
            POINT pt= { GET_X_LPARAM(msg.lParam),GET_Y_LPARAM(msg.lParam)};
            if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) 
            {
                if( ::PtInRect(&m_rcItem, pt) ) m_uButtonState |= UISTATE_PUSHED;
                else m_uButtonState &= ~UISTATE_PUSHED;
                Invalidate();
            }
        });

        AddEntry(UIMSG_LBUTTONUP,[&](const MsgWrap &msg)
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
       
        AddEntry(UIMSG_MOUSEENTER,[&](const MsgWrap &msg)
        {
            if( IsEnabled() ) 
            {
                m_uButtonState |= UISTATE_HOT;
                Invalidate();
            }
            MsgHandleChainBase::HandleMsg(msg);
        }); 
        
        AddEntry(UIMSG_MOUSELEAVE,[&](const MsgWrap &msg)
        {
            if( IsEnabled() ) 
            {
                m_uButtonState &= ~UISTATE_HOT;
                Invalidate();
            }
            MsgHandleChainBase::HandleMsg(msg);
        }); 

        AddEntry(UIMSG_SETCURSOR,[&](const MsgWrap &msg)
        {
           ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
        });
    }

}