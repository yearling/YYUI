#include "SliderBar.h"

namespace YUI
{

SliderBar::SliderBar(void)
    :m_uButtonState(0)
    ,m_fStep(10.0f)
{
    m_uTextStyle = DT_SINGLELINE | DT_CENTER;
    m_szThumb.width = m_szThumb.height = 10.0f;
}


SliderBar::~SliderBar(void)
{
}

LPCTSTR SliderBar::GetClass() const
{
    return  _T("SliderBar");
}

void SliderBar::AddHander()
{
    m_SliderBarMsgHandler.SetSuccessor(&m_ProgressMsgHandler);
}

void SliderBar::HandleMsg(const MsgWrap & msg) throw()
{

}

void SliderBar::SetAttribute(const std::string &strName, const std::string& strValue)
{

}

void SliderBar::PaintStatusImage()
{

}

float SliderBar::GetChangeStep()
{
    return m_fStep;
}

void SliderBar::SetChangeStep(float step)
{
    m_fStep = step;
}

void SliderBar::SetThumbSize(const YYSIZE & szXY)
{

}

YUI::YYRECT SliderBar::GetThumbRect() const
{

}

YUI::YString SliderBar::GetThumbImage() const
{

}

void SliderBar::SetThumbImage(const YString & strImage)
{

}

YUI::YString SliderBar::GetThumbHotImage() const
{

}

void SliderBar::SetThumbHotImage(const YString & strImage)
{

}

YUI::YString SliderBar::GetThumbPushedImage() const
{

}

void SliderBar::SetThumbPushedImage(const YString & strImage)
{

}

void SliderBar::SetEnabled(bool bEnable )
{
    //throw std::logic_error("The method or operation is not implemented.");
    ControlUI::SetEnabled(true);
    if( !IsEnabled() )
    {
        m_uButtonState = 0;
    }
}

}
