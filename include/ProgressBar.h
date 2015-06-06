#pragma once
#include "YUI.h"
#include "ControlUI.h"
#include "Label.h"
namespace YUI
{

class ProgressBar: public Label
{
public:
    ProgressBar(void);
    virtual ~ProgressBar(void);
    virtual LPCTSTR                 GetClass() const;
    void                            AddHander();
    virtual void					HandleMsg(const MsgWrap & msg)throw();
    virtual void                    PaintStatusImage();
public:
    bool                            IsHorizontal() const;
    void                            SetHorizontal( bool bHorizontal = true);
    void                            SetStretchForeImage( bool bStretchForeImage = true);
    float                           GetMinValue() const;
    void                            SetMinValue(float fMinValue);
    float                           GetMaxValue() const;
    void                            SetMaxValue(float fMaxValue);
    float                           GetValue() const;
    void                            SetValue(float fValue);
    YString                         GetForeImage() const;
    void                            SetForeImage(const YString &strForeImage);
protected:
    bool                            m_bHorizontal;
    bool                            m_bStrtchForeImage;
    float                           m_fMaxValue;
    float                           m_fMinValue;
    float                           m_fValue;
    YString                         m_strForeImage;
    MsgHandler                      m_ProgressMsgHandler;
};

}