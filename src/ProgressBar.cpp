#include "YUI.h"
#include "ProgressBar.h"
#include <iosfwd>

namespace YUI
{
ProgressBar::ProgressBar(void)
    :m_bHorizontal(true)
    ,m_bStrtchForeImage(true)
    ,m_fMinValue(0.0f)
    ,m_fMaxValue(100.0f)
    ,m_fValue(50.0f)
{
    m_uTextStyle = DT_SINGLELINE | DT_CENTER;
    SetFixedHeight(12);
}


ProgressBar::~ProgressBar(void)
{
}

LPCTSTR ProgressBar::GetClass() const
{
    return ("ProgressBar");
}

void ProgressBar::AddHander()
{

}

bool ProgressBar::IsHorizontal() const
{
    return m_bHorizontal;
}

void ProgressBar::SetHorizontal(bool bHorizontal /*= true*/)
{
    if(m_bHorizontal == bHorizontal )
        return;
    m_bHorizontal = bHorizontal;
    Invalidate();
}

void ProgressBar::SetStretchForeImage(bool bStretchForeImage /*= true*/)
{
    if(m_bStrtchForeImage == bStretchForeImage)
        return;
    m_bStrtchForeImage = bStretchForeImage;
    Invalidate();
}

float ProgressBar::GetMinValue() const
{
    return m_fMinValue;
}

void ProgressBar::SetMinValue(float fMinValue)
{
    m_fMinValue = fMinValue;
    Invalidate();
}

float ProgressBar::GetMaxValue() const
{
    return m_fMaxValue;
}

void ProgressBar::SetMaxValue(float fMaxValue)
{
    m_fMaxValue = fMaxValue;
    Invalidate();
}

float ProgressBar::GetValue() const
{
    return m_fValue ;
}

void ProgressBar::SetValue(float fValue)
{
    if(fValue == m_fValue  || fValue < m_fMinValue || fValue > m_fMaxValue)
        return;
    m_fValue = fValue;
    Invalidate();
}

YUI::YString ProgressBar::GetForeImage() const
{
    return m_strForeImage;
}

void ProgressBar::SetForeImage(const YString &strForeImage)
{
    if( m_strForeImage == strForeImage )
        return;
    m_strForeImage = strForeImage;
    Invalidate();
}

void ProgressBar::SetAttribute(const std::string &strName, const std::string& strValue)
{
    LPCSTR pstrName = strName.c_str();
    LPCSTR pstrValue = strValue.c_str();
    if( strcmp(pstrName, ("foreimage")) == 0 ) SetForeImage(pstrValue);
    else if( strcmp(pstrName, ("hor")) == 0 ) SetHorizontal(strcmp(pstrValue, ("true")) == 0);
    else if( strcmp(pstrName, ("min")) == 0 ) SetMinValue((float)atoi(pstrValue));
    else if( strcmp(pstrName, ("max")) == 0 ) SetMaxValue((float)atoi(pstrValue));
    else if( strcmp(pstrName, ("value")) == 0 ) SetValue((float)atoi(pstrValue));
    else if( strcmp(pstrName, ("isstretchfore"))==0) SetStretchForeImage(strcmp(pstrValue, ("true")) == 0? true : false);
    else Label::SetAttribute(pstrName, pstrValue);
}

void ProgressBar::PaintStatusImage()
{
    assert( m_fMaxValue > m_fMinValue );
    if( m_fValue > m_fMaxValue )
        m_fValue = m_fMaxValue;
    if( m_fValue < m_fMinValue )
        m_fValue = m_fMinValue;

    YYRECT rc;
    if( m_bHorizontal )
    {
        rc.right = (m_fValue - m_fMinValue )/(m_fMaxValue - m_fMinValue )*(m_rcItem.right - m_rcItem.left);
        rc.bottom = m_rcItem.bottom - m_rcItem.top;
    }
    else
    {
        rc.top  = (m_rcItem.bottom - m_rcItem.top) * (m_fMaxValue - m_fValue) /( m_fMaxValue - m_fMinValue);
        rc.right = m_rcItem.right - m_rcItem.left;
        rc.bottom = m_rcItem.bottom - m_rcItem.top;
    }
    if(m_strForeImage.empty())
        return;
    std::string strModify;
    std::stringstream ss;
    ss.str("");
    //FIX ME!!尼玛，DrawImage的字符解析只支持int
    if(m_bStrtchForeImage)
    {
        ss<<" dest= \'";
        ss<<(int)rc.left<<','<<(int)rc.top<<','<<(int)rc.right<<','<<(int)rc.bottom<<'\'';
    }
    else
    {
        ss<<" dest= \'";
        ss<<(int)rc.left<<','<<(int)rc.top<<','<<(int)rc.right<<','<<(int)rc.bottom<<"\' ";
        ss<<"source=\'";
        ss<<(int)rc.left<<','<<(int)rc.top<<','<<(int)rc.right<<','<<(int)rc.bottom<<"\' ";
    }
    strModify= ss.str();
    DrawImage("file = \'"+m_strForeImage+'\'' + strModify);
}

}