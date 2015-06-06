#pragma once

#include "YUI.h"
#include "ControlUI.h"

//#include <GdiPlus.h>
//#pragma comment( lib, "GdiPlus.lib" )
//using namespace Gdiplus;
//class __declspec(dllexport) Gdiplus::RectF;
//struct __declspec(dllexport) Gdiplus::GdiplusStartupInput;

namespace YUI
{
    class Label: public ControlUI
    {
    public:
        Label();
        virtual ~Label();
        virtual LPCTSTR                 GetClass() const;
        void                            AddHander();
        virtual void					HandleMsg(const MsgWrap & msg)throw();
        void                            SetTextStyle(UINT uStyle);
        UINT                            GetTextStyle() const;
        void                            SetTextColor(DWORD dwTextColor);
        DWORD                           GetTextColor() const;
        void                            SetDisabledTextColor(DWORD dwTextColor);
        DWORD                           GetDisabledTextColor() const;
        void                            SetFont(const YString &fontid);
        YString                         GetFont() const;
        YYRECT                          GetTextPadding() const;
        void                            SetTextPadding(YYRECT rc);
        bool                            IsShowHtml();
        void                            SetShowHtml(bool bShowHtml = true);
        YYSIZE                          EstimateSize(YYSIZE szAvailable);
        virtual void                    SetAttribute(const std::string &strName, const std::string& strValue);

        virtual void                    PaintText();

        void		                    SetEnabledEffect(bool _EnabledEffect);
        bool		                    GetEnabledEffect();
        void		                    SetText(LPCTSTR pstrText);
        YString	                        GetText() const;
        void                            SetTransShadow(float _TransShadow);
        float                           GetTransShadow();
        void                            SetTransShadow1(float _TransShadow);
        float                           GetTransShadow1();
        void                            SetTransText(float _TransText);
        float                           GetTransText();
        void                            SetTransText1(float _TransText);
        float                           GetTransText1();
        void                            SetTransStroke(float _TransStroke);
        float                           GetTransStroke();
        void                            SetGradientLength(float _GradientLength);
        float                           GetGradientLength();
        void                            SetTextRenderingHintAntiAlias(float _TextRenderingHintAntiAlias);
        float                           GetTextRenderingHintAntiAlias();
        void                            SetShadowOffset(float _offset,float _angle);
        YYRECT	                        GetShadowOffset();
        void		                    SetTextColor1(DWORD _TextColor1);
        DWORD		                    GetTextColor1();
        void		                    SetTextShadowColorA(DWORD _TextShadowColorA);
        DWORD		                    GetTextShadowColorA();
        void		                    SetTextShadowColorB(DWORD _TextShadowColorB);
        DWORD		                    GetTextShadowColorB();
        void		                    SetStrokeColor(DWORD _StrokeColor);
        DWORD		                    GetStrokeColor();
        void                            SetGradientAngle(float _SetGradientAngle);
        float                           GetGradientAngle();
        void		                    SetEnabledStroke(bool _EnabledStroke);
        bool		                    GetEnabledStroke();
        void		                    SetEnabledShadow(bool _EnabledShadowe);
        bool		                    GetEnabledShadow();

    protected:
        DWORD	                        m_dwTextColor;
        DWORD	                        m_dwDisabledTextColor;
        //int		                        m_iFont;
        YString                         m_Fontid;
        UINT	                        m_uTextStyle;
        YYRECT	                        m_rcTextPadding;
        bool	                        m_bShowHtml;

        float					        m_hAlign;
        float						    m_vAlign;
        float						    m_TransShadow;
		float						    m_TransShadow1;
        float						    m_TransText;
        float						    m_TransText1;
        float						    m_TransStroke;
        float						    m_GradientLength;
        float						    m_GradientAngle;
        bool					        m_EnableEffect;
        bool					        m_EnabledStroke;
        bool					        m_EnabledShadow;
        DWORD					        m_dwTextColor1;
        DWORD					        m_dwTextShadowColorA;
        DWORD					        m_dwTextShadowColorB;
        DWORD					        m_dwStrokeColor;
        YYRECT					        m_ShadowOffset;
        YString				            m_TextValue;
        MsgHandler						m_LabelMsgHandler;
    };
}