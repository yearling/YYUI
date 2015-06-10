#include "SliderBar.h"
namespace YUI
{

SliderBar::SliderBar(void)
    :m_uButtonState(0)
    ,m_fStep(10.0f)
{
    m_uTextStyle = DT_SINGLELINE | DT_CENTER;
    m_szThumb.width = m_szThumb.height = 10.0f;
	AddHander();
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
	m_SliderBarMsgHandler.AddEntry(UIMSG_LBUTTONDOWN,[&](const MsgWrap &msg)
	{
		if( !IsEnabled() )
			return;
		YYRECT rcThumb = GetThumbRect();
		YYPOINT ptInLocal=ClientToLocal(msg.ptMouse);
		if(PtInRect(rcThumb,ptInLocal))
		{
			m_uButtonState |=UISTATE_CAPTURED;
		}
	});
	m_SliderBarMsgHandler.AddEntry(UIMSG_LBUTTONBLICK,[&](const MsgWrap &msg)
	{
		if( !IsEnabled() )
			return;
		YYRECT rcThumb = GetThumbRect();
		YYPOINT ptInLocal=ClientToLocal(msg.ptMouse);
		if(PtInRect(rcThumb,ptInLocal))
		{
			m_uButtonState |=UISTATE_CAPTURED;
		}
	});
	m_SliderBarMsgHandler.AddEntry(UIMSG_LBUTTONUP,[&](const MsgWrap &msg)
	{
		float fValue;
		if( (m_uButtonState & UISTATE_CAPTURED) != 0)
		{
			m_uButtonState &= ~UISTATE_CAPTURED;
		}
		YYPOINT ptInLocal=ClientToLocal(msg.ptMouse);
		if(m_bHorizontal)
		{
			if(ptInLocal.x >= m_rcItem.GetWidth() - m_szThumb.width /2 )
					fValue = m_fMaxValue;
			else if( ptInLocal.x <= m_szThumb.width/2)
					fValue = m_fMinValue;
			else  
				fValue = Lerp(m_fMinValue,m_fMaxValue,(ptInLocal.x-m_szThumb.width/2)/(m_rcItem.GetWidth()-m_szThumb.width));
		}
		else
		{
			if( ptInLocal.y >= m_rcItem.GetHeight() - m_szThumb.height / 2 )
				fValue = m_fMaxValue;
			else if( ptInLocal.y <= m_szThumb.height / 2  ) 
				fValue = m_fMinValue;
			else 
				fValue = Lerp(m_fMinValue,m_fMinValue,(ptInLocal.y-m_szThumb.height/2)/(m_rcItem.GetHeight()-m_szThumb.height));
		}
		if(m_fValue != fValue && m_fMinValue <= fValue && fValue <= m_fMaxValue )
		{
			m_fValue = fValue;
			Invalidate();
		}
		return;
	});
	m_SliderBarMsgHandler.AddEntry(UIMSG_MOUSEWHEEL,[&](const MsgWrap &msg)
	{
		switch( LOWORD(msg.wParam) ) 
		{
		case SB_LINEUP:
			SetValue(GetValue() + GetChangeStep());
			return;
		case SB_LINEDOWN:
			SetValue(GetValue() - GetChangeStep());
			return;
		}
	});
	m_SliderBarMsgHandler.AddEntry(UIMSG_MOUSEMOVE,[&](const MsgWrap &msg)
	{
		if( (m_uButtonState & UISTATE_CAPTURED) == 0 )
			return;
		YYPOINT ptInLocal=ClientToLocal(msg.ptMouse);
		if(m_bHorizontal)
		{
			if(ptInLocal.x >= m_rcItem.GetWidth() - m_szThumb.width /2 )
				m_fValue = m_fMaxValue;
			else if( ptInLocal.x <= m_szThumb.width/2)
				m_fValue = m_fMinValue;
			else  
				m_fValue = Lerp(m_fMinValue,m_fMaxValue,(ptInLocal.x-m_szThumb.width/2)/(m_rcItem.GetWidth()-m_szThumb.width));
		}
		else 
		{
			if( ptInLocal.y >= m_rcItem.GetHeight() - m_szThumb.height / 2 )
				m_fValue = m_fMaxValue;
			else if( ptInLocal.y <= m_szThumb.height / 2  ) 
				m_fValue = m_fMinValue;
			else 
				m_fValue = Lerp(m_fMinValue,m_fMinValue,(ptInLocal.y-m_szThumb.height/2)/(m_rcItem.GetHeight()-m_szThumb.height));
		}
		Invalidate();
	});
	m_SliderBarMsgHandler.AddEntry(UIMSG_SETCURSOR,[&](const MsgWrap &msg)
	{
		YYRECT rcThumb = GetThumbRect();
		if( IsEnabled() && PtInRect(rcThumb,msg.ptMouse))
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
	});
	m_SliderBarMsgHandler.AddEntry(UIMSG_MOUSEENTER,[&](const MsgWrap &msg)
	{
		if(!IsEnabled())
			return;
		m_uButtonState |= UISTATE_HOT;
		Invalidate();
	});
	m_SliderBarMsgHandler.AddEntry(UIMSG_MOUSELEAVE,[&](const MsgWrap &msg)
	{
		if(!IsEnabled())
			return;
		m_uButtonState &= ~UISTATE_HOT;
		Invalidate();
	});

}

void SliderBar::HandleMsg(const MsgWrap & msg) throw()
{
	m_SliderBarMsgHandler.HandleMsg(msg);
}

void SliderBar::SetAttribute(const std::string &strName, const std::string& strValue)
{
	LPCSTR pstrName = strName.c_str();
	LPCSTR pstrValue = strValue.c_str();
	if( strcmp(pstrName, ("thumbimage")) == 0 ) SetThumbImage(pstrValue);
	else if( strcmp(pstrName, ("thumbhotimage")) == 0 ) SetThumbHotImage(pstrValue);
	else if( strcmp(pstrName, ("thumbpushedimage")) == 0 ) SetThumbPushedImage(pstrValue);
	else if( strcmp(pstrName, ("thumbsize")) == 0 ) {
		YYSIZE szXY;
		LPSTR pstr = NULL;
		szXY.width= (float)strtol(pstrValue, &pstr, 10);  assert(pstr);    
		szXY.height= (float)strtol(pstr + 1, &pstr, 10);   assert(pstr); 
		SetThumbSize(szXY);
	}
	else if( strcmp(pstrName, "step") == 0 ) {
		SetChangeStep((float)atof(pstrValue));
	}
	else ProgressBar::SetAttribute(pstrName, pstrValue);
}

void SliderBar::PaintStatusImage()
{
	ProgressBar::PaintStatusImage();
	YYRECT rcThumb = GetThumbRect();
	std::stringstream ss;
	if( (m_uButtonState & UISTATE_CAPTURED) != 0)
	{
		if(!m_strThumbPushedImage.empty())
		{
			ss.str("");
			ss<<" dest= \'";
			ss<<(int)rcThumb.left<<','<<(int)rcThumb.top<<','<<(int)rcThumb.right<<','<<(int)rcThumb.bottom<<'\'';
			DrawImage("file = \'"+m_strThumbPushedImage+'\'' + ss.str());
			return;
		}
	}
	else if( (m_uButtonState & UISTATE_HOT)!= 0)
	{
		if(!m_strThumbPushedImage.empty())
		{
			ss.str("");
			ss<<" dest= \'";
			ss<<(int)rcThumb.left<<','<<(int)rcThumb.top<<','<<(int)rcThumb.right<<','<<(int)rcThumb.bottom<<'\'';
			DrawImage("file = \'"+m_strThumbHotImage+'\'' + ss.str());
			return;
		}
	}
	if(!m_strThumbImage.empty())
	{
		ss.str("");
		ss<<" dest= \'";
		ss<<(int)rcThumb.left<<','<<(int)rcThumb.top<<','<<(int)rcThumb.right<<','<<(int)rcThumb.bottom<<'\'';
		DrawImage("file = \'"+m_strThumbImage+'\'' + ss.str());
		return;
	}
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
	m_szThumb = szXY;
}

YUI::YYRECT SliderBar::GetThumbRect() const
{
	if(m_bHorizontal)
	{
		float left = Lerp(0.0f,m_rcItem.GetWidth()-m_szThumb.width,(m_fValue-m_fMinValue)/(m_fMaxValue-m_fMinValue));
		float top = (m_rcItem.GetHeight()-m_szThumb.height)/2;
		return YYRECT(left,top,left+m_szThumb.width,top+m_szThumb.height);
	}
	else
	{
		/*float left = (m_rcItem.right+ m_rcItem.left - m_szThumb.width ) /2;
		float top = m_rcItem.bottom - m_szThumb.height-(m_rcItem.bottom - m_rcItem.top - m_szThumb.height) * (m_fValue - m_fMinValue) / (m_fMaxValue - m_fMinValue);*/
		float left = (m_rcItem.GetWidth() - m_szThumb.width)/2;
		float top = Lerp(0.0f,m_rcItem.GetHeight()-m_szThumb.height,(m_fValue-m_fMinValue)/(m_fMaxValue-m_fMinValue));
		return YYRECT(left,top,left+m_szThumb.width,top+m_szThumb.height);
	}
}

YUI::YString SliderBar::GetThumbImage() const
{
	return m_strThumbImage;
}

void SliderBar::SetThumbImage(const YString & strImage)
{
	m_strThumbImage = strImage;
	Invalidate();
}

YUI::YString SliderBar::GetThumbHotImage() const
{
	return m_strThumbImage;
}

void SliderBar::SetThumbHotImage(const YString & strImage)
{
	m_strThumbHotImage = strImage;
	Invalidate();
}

YUI::YString SliderBar::GetThumbPushedImage() const
{
	return m_strThumbPushedImage;
}

void SliderBar::SetThumbPushedImage(const YString & strImage)
{
	m_strThumbPushedImage = strImage;
	Invalidate();
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

void SliderBar::PaintBkImage()
{
	if(m_strBKImage.empty())
		return;
	std::string strModify;
	std::stringstream ss;
	ss.str("");
	YYRECT rc;
	if(m_bHorizontal)
	{
		rc.left=m_szThumb.width/2;
		rc.right= m_rcItem.GetWidth()-m_szThumb.width/2;
		rc.top= 0.0f;
		rc.bottom= m_rcItem.GetHeight();
		ss<<" dest= \'";
		ss<<(int)rc.left<<','<<(int)rc.top<<','<<(int)rc.right<<','<<(int)rc.bottom<<'\'';
	}
	else
	{
		rc.top=m_szThumb.height/2;
		rc.bottom= m_rcItem.GetWidth()-m_szThumb.height/2;
		rc.left =0.0f;
		rc.right=m_rcItem.GetWidth();
		ss<<" dest= \'";
		ss<<(int)rc.left<<','<<(int)rc.top<<','<<(int)rc.right<<','<<(int)rc.bottom<<'\'';
	}
	strModify= ss.str();
	DrawImage("file = \'"+m_strBKImage+'\'' + strModify);
}

}
