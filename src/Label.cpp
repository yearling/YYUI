#include "YUI.h"
#include "Label.h"
#include "UIUtility.h"
#include <atlconv.h>
#include "ControlUI.h"
#include "ControlManager.h"
#include "WindowProperty.h"
#include "Canvas2D.h"

namespace YUI
{

   
    Label::Label()
        :m_uTextStyle(DT_VCENTER)
        ,m_dwTextColor(0)
        ,m_dwDisabledTextColor(0)
        ,m_bShowHtml(false)
        ,m_EnableEffect(false)
        ,m_TransShadow(60)
        ,m_TransText(168)
        ,m_TransShadow1(60)
        ,m_TransText1(168)
        ,m_hAlign(DT_LEFT)
        ,m_vAlign(DT_CENTER)
        ,m_dwTextColor1(-1)
        ,m_dwTextShadowColorA(0xff000000)
        ,m_dwTextShadowColorB(-1)
        ,m_GradientAngle(0)
        ,m_EnabledStroke(false)
        ,m_TransStroke(255)
        ,m_dwStrokeColor(0)
        ,m_EnabledShadow(false)
        ,m_GradientLength(0)
    {
        ::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
        AddHander();
    }

    Label::~Label()
    {
    }

    LPCTSTR Label::GetClass() const
    {
        return _T("LabelUI");
    }

   

    void Label::SetTextStyle(UINT uStyle)
    {
        m_uTextStyle = uStyle;
        Invalidate();
    }

    UINT Label::GetTextStyle() const
    {
        return m_uTextStyle;
    }

    void Label::SetTextColor(DWORD dwTextColor)
    {
        m_dwTextColor = dwTextColor;
        Invalidate();
    }

    DWORD Label::GetTextColor() const
    {
        return m_dwTextColor;
    }

    void Label::SetDisabledTextColor(DWORD dwTextColor)
    {
        m_dwDisabledTextColor = dwTextColor;
        Invalidate();
    }

    DWORD Label::GetDisabledTextColor() const
    {
        return m_dwDisabledTextColor;
    }

    void Label::SetFont(const YString &fontid)
    {
        m_Fontid = fontid;
        Invalidate();
    }

    YString Label::GetFont() const
    {
        return m_Fontid;
    }

    YUI::YYRECT Label::GetTextPadding() const
{
        return m_rcTextPadding;
    }

    void Label::SetTextPadding(YYRECT rc)
    {
        m_rcTextPadding = rc;
        Invalidate();
    }

    bool Label::IsShowHtml()
    {
        return m_bShowHtml;
    }

    void Label::SetShowHtml(bool bShowHtml /*= true*/)
    {
        if( m_bShowHtml == bShowHtml ) return;

        m_bShowHtml = bShowHtml;
        Invalidate();
    }

    YUI::YYSIZE Label::EstimateSize(YYSIZE szAvailable)
    {
        //if( m_cXYFixed.cy == 0 ) return YSize(m_cXYFixed.cx, m_pManager->GetFontInfo((HFONT)_ttoi(GetFont().c_str()))->m_tm.tmHeight + 4);
        return ControlUI::EstimateSize(szAvailable);
    }

    void Label::SetAttribute(const std::string &strName, const std::string& strValue)
    {
        LPCSTR pstrName = strName.c_str();
        LPCSTR pstrValue = strValue.c_str();
        if( strcmp(pstrName, ("align")) == 0 ) {
            if( strstr(pstrValue, ("left")) != NULL ) {
                m_uTextStyle &= ~(DT_CENTER | DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
                m_uTextStyle |= DT_LEFT;
            }
            if( strstr(pstrValue, ("center")) != NULL ) {
                m_uTextStyle &= ~(DT_LEFT | DT_RIGHT );
                m_uTextStyle |= DT_CENTER;
            }
            if( strstr(pstrValue, ("right")) != NULL ) {
                m_uTextStyle &= ~(DT_LEFT | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                m_uTextStyle |= DT_RIGHT;
            }
            if( strstr(pstrValue, ("top")) != NULL ) {
                m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER | DT_VCENTER);
                m_uTextStyle |= (DT_TOP | DT_SINGLELINE);
            }
            if( strstr(pstrValue, ("vcenter")) != NULL ) {
                m_uTextStyle &= ~(DT_TOP | DT_BOTTOM );			
                m_uTextStyle |= (DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
            if( strstr(pstrValue, ("bottom")) != NULL ) {
                m_uTextStyle &= ~(DT_TOP | DT_VCENTER | DT_VCENTER);
                m_uTextStyle |= (DT_BOTTOM | DT_SINGLELINE);
            }
        }
        else if( strcmp(pstrName, ("endellipsis")) == 0 ) {
            if( strcmp(pstrValue, ("true")) == 0 ) m_uTextStyle |= DT_END_ELLIPSIS;
            else m_uTextStyle &= ~DT_END_ELLIPSIS;
        }  
        else if( strcmp(pstrName, ("font")) == 0 )   SetFont(UTF8ToGBK(pstrValue)); 
        else if( strcmp(pstrName, ("textcolor")) == 0 ) {
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetTextColor(clrColor);
        }
        else if( strcmp(pstrName, ("disabledtextcolor")) == 0 ) {
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetDisabledTextColor(clrColor);
        }
        else if( strcmp(pstrName, ("textpadding")) == 0 ) {
            YYRECT rcTextPadding ;
            LPSTR pstr = NULL;
            rcTextPadding.left = (float)strtol(pstrValue, &pstr, 10);  assert(pstr);    
            rcTextPadding.top = (float)strtol(pstr + 1, &pstr, 10);    assert(pstr);    
            rcTextPadding.right = (float)strtol(pstr + 1, &pstr, 10);  assert(pstr);    
            rcTextPadding.bottom = (float)strtol(pstr + 1, &pstr, 10); assert(pstr);    
            SetTextPadding(rcTextPadding);
        }
        else if( strcmp(pstrName, ("showhtml")) == 0 ) SetShowHtml(strcmp(pstrValue, ("true")) == 0);

        else if( strcmp(pstrName, ("enabledeffect")) == 0 ) SetEnabledEffect(strcmp(pstrValue, ("true")) == 0);
        else if( strcmp(pstrName, ("rhaa")) == 0 ) SetTextRenderingHintAntiAlias((float)atoi(pstrValue));
        else if( strcmp(pstrName, ("transshadow")) == 0 ) SetTransShadow((float)atoi(pstrValue));
        else if( strcmp(pstrName, ("transtext")) == 0 ) SetTransText((float)atoi(pstrValue));
        else if( strcmp(pstrName, ("transshadow1")) == 0 ) SetTransShadow1((float)atoi(pstrValue));
        else if( strcmp(pstrName, ("transtext1")) == 0 ) SetTransText1((float)atoi(pstrValue));
        else if( strcmp(pstrName, ("gradientangle")) == 0 ) SetGradientAngle((float)atoi(pstrValue));
        else if( strcmp(pstrName, ("enabledstroke")) == 0 ) SetEnabledStroke(strcmp(pstrValue, ("true")) == 0);
        else if( strcmp(pstrName, ("enabledshadow")) == 0 ) SetEnabledShadow(strcmp(pstrValue, ("true")) == 0);
        else if( strcmp(pstrName, ("transstroke")) == 0 ) SetTransStroke((float)atoi(pstrValue));
        else if( strcmp(pstrName, ("gradientlength")) == 0 ) SetGradientLength((float)atoi(pstrValue));
        else if( strcmp(pstrName, ("shadowoffset")) == 0 ){
            LPSTR pstr = NULL;
            float offsetx = (float)strtol(pstrValue, &pstr, 10);	assert(pstr);    
            float offsety = (float)strtol(pstr + 1, &pstr, 10);		assert(pstr);
            SetShadowOffset(offsetx,offsety);
        }
        else if( strcmp(pstrName, ("textcolor1")) == 0 ) {
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetTextColor1(clrColor);
        }
        else if( strcmp(pstrName, ("textshadowcolora")) == 0 ) {
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetTextShadowColorA(clrColor);
        }
        else if( strcmp(pstrName, ("textshadowcolorb")) == 0 ) {
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetTextShadowColorB(clrColor);
        }
        else if( strcmp(pstrName, ("strokecolor")) == 0 ) {
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetStrokeColor(clrColor);
        }
        else ControlUI::SetAttribute(pstrName, pstrValue);
    }

    void Label::PaintText()
    {
        if( m_dwTextColor == 0 ) 
            m_dwTextColor= m_pManager->GetWindowProperty()->GetDefaultFontColor();
        if( m_dwDisabledTextColor == 0 ) 
            m_dwDisabledTextColor = m_pManager->GetWindowProperty()->GetDefaultDisabledColor();

        YYRECT rc = m_rcItem;
        rc.left += m_rcTextPadding.left;
        rc.right -= m_rcTextPadding.right;
        rc.top += m_rcTextPadding.top;
        rc.bottom -= m_rcTextPadding.bottom;

        Canvas2D canvas(m_pManager->GetHWND());
        FontD2D font(_T("Verdana"),20);
        canvas.DrawSolidText(m_strText,font,rc,m_dwTextColor);
    }

    void Label::SetEnabledEffect(bool _EnabledEffect)
    {
            m_EnableEffect = _EnabledEffect;
    }

    bool Label::GetEnabledEffect()
    {
            return m_EnableEffect;
    }

    void Label::SetText(LPCTSTR pstrText)
    {
        if(!GetEnabledEffect())
            return ControlUI::SetText(pstrText);

        m_TextValue = pstrText;
    }

    YUI::YString Label::GetText() const
    {
        if(!m_EnableEffect)
            return ControlUI::GetText();
        return m_TextValue;
    }

    void Label::SetTransShadow(float _TransShadow)
    {
        m_TransShadow = _TransShadow;
    }

    float Label::GetTransShadow()
{
        return m_TransShadow;
    }

    void Label::SetTransShadow1(float _TransShadow)
    {
        m_TransShadow1	= _TransShadow;
    }

    float Label::GetTransShadow1()
{
        return m_TransShadow1;
    }

    void Label::SetTransText(float _TransText)
    {
        	m_TransText = _TransText;
    }

    float Label::GetTransText()
{
        return m_TransText;
    }

    void Label::SetTransText1(float _TransText)
    {
        m_TransText1	= _TransText;
    }

    float Label::GetTransText1()
{
        return m_TransText1;
    }

    void Label::SetTransStroke(float _TransStroke)
    {
        m_TransStroke = _TransStroke;
    }

    float Label::GetTransStroke()
{
        return m_TransStroke;
    }

    void Label::SetGradientLength(float _GradientLength)
    {
       m_GradientLength	= _GradientLength;
    }

    float Label::GetGradientLength()
{
       return m_GradientLength;
    }

    void Label::SetTextRenderingHintAntiAlias(float _TextRenderingHintAntiAlias)
    {
        if(_TextRenderingHintAntiAlias < 0 || _TextRenderingHintAntiAlias > 5)
            _TextRenderingHintAntiAlias = 0;
        //m_TextRenderingHintAntiAlias = (TextRenderingHint)extRenderingHintAntiAlias;
    }

    float Label::GetTextRenderingHintAntiAlias()
{
        //return m_TextRenderingHintAntiAlias;
        return 1;
    }

    void Label::SetShadowOffset(float _offset,float _angle)
    {
        if(_angle > 180 || _angle < -180)
            return;

        YYRECT rc = m_rcItem;

        if(_angle >= 0 && _angle <= 180)
            rc.top -= _offset;
        else if(_angle > -180 && _angle < 0)
            rc.top += _offset;

        if(_angle > -90 && _angle <= 90)
            rc.left -= _offset;
        else if( _angle > 90 || _angle < -90)
            rc.left += _offset;

        m_ShadowOffset.top = (float)rc.top;
        m_ShadowOffset.left = (float)rc.left;
    }

    YYRECT Label::GetShadowOffset()
    {
        return m_ShadowOffset;
    }

    void Label::SetTextColor1(DWORD extColor1)
    {
        m_dwTextColor1	= extColor1;
    }

    DWORD Label::GetTextColor1()
    {
        return m_dwTextColor1;
    }

    void Label::SetTextShadowColorA(DWORD extShadowColorA)
    {
        m_dwTextShadowColorA	= extShadowColorA;
    }

    DWORD Label::GetTextShadowColorA()
    {
        return m_dwTextShadowColorA;
    }

    void Label::SetTextShadowColorB(DWORD extShadowColorB)
    {
        m_dwTextShadowColorB	= extShadowColorB;
    }

    DWORD Label::GetTextShadowColorB()
    {
        return m_dwTextShadowColorB;
    }

    void Label::SetStrokeColor(DWORD _StrokeColor)
    {
        m_dwStrokeColor = _StrokeColor;
    }

    DWORD Label::GetStrokeColor()
    {
        return m_dwStrokeColor;
    }

    void Label::SetGradientAngle(float _SetGradientAngle)
    {
        m_GradientAngle	= _SetGradientAngle;
    }

    float Label::GetGradientAngle()
{
        return m_GradientAngle;
    }

    void Label::SetEnabledStroke(bool _EnabledStroke)
    {
        m_EnabledStroke = _EnabledStroke;
    }

    bool Label::GetEnabledStroke()
    {
        return m_EnabledStroke;
    }

    void Label::SetEnabledShadow(bool _EnabledShadowe)
    {
        m_EnabledShadow = _EnabledShadowe;
    }

    bool Label::GetEnabledShadow()
    {
        return m_EnabledShadow;
    }

   

    void Label::AddHander()
    {
        m_LabelMsgHandler.SetSuccessor(&m_ControlMsgHandler);
        m_LabelMsgHandler.AddEntry(UIMSG_SETFOCUS,[&](const MsgWrap &msg)
        {
            m_bFocused = true;
            Invalidate();
        });

        m_LabelMsgHandler.AddEntry(UIMSG_KILLFOCUS,[&](const MsgWrap &msg)
        {
            m_bFocused = false;
            Invalidate();
        });
    }

    void Label::HandleMsg(const MsgWrap & msg) throw()
    {
        m_LabelMsgHandler.HandleMsg(msg);
    }

}