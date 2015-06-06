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
    virtual void                    SetAttribute(const std::string &strName, const std::string& strValue);                       
    virtual void                    PaintStatusImage();
protected:
    bool                            m_bHorizontal;
    bool                            m_bStrtchForeImage;
    float                           m_fMaxValue;
    float                           m_fMinValue;
    float                           m_fValue;

    YString                         m_strForeImage;
    YString                         m_sForeImageModify;
};

}