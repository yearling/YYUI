#pragma once
#include "progressbar.h"
namespace YUI
{

class SliderBar :public ProgressBar
{
public:
    SliderBar(void);
    virtual ~SliderBar(void);
    virtual LPCTSTR                 GetClass() const;
    void                            AddHander();
    virtual void					HandleMsg(const MsgWrap & msg)throw();
    virtual void                    SetAttribute(const std::string &strName, const std::string& strValue);                       
    virtual void                    PaintStatusImage();
	virtual void                    PaintBkImage();
    virtual void                    SetEnabled(bool bEnable = true);
public:
    float                           GetChangeStep();
    void                            SetChangeStep(float step);
    void                            SetThumbSize(const YYSIZE & szXY);
    YYRECT                          GetThumbRect() const;
    YString                         GetThumbImage() const;
    void                            SetThumbImage(const YString & strImage);
    YString                         GetThumbHotImage() const;
    void                            SetThumbHotImage(const YString & strImage);
    YString                         GetThumbPushedImage() const;
    void                            SetThumbPushedImage(const YString & strImage);


protected:
    YYSIZE                          m_szThumb;
    UINT                            m_uButtonState;
    float                           m_fStep;
    YString                         m_strThumbImage;
    YString                         m_strThumbHotImage;
    YString                         m_strThumbPushedImage;
    MsgHandler                      m_SliderBarMsgHandler;
};

}
