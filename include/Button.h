#pragma once

#include "YUI.h"
#include "ControlUI.h"
#include "Label.h"
#include "UIUtility.h"
namespace YUI
{

    class Button:public Label
    {
    public:
        typedef std::function<void() throw()> EventClick;
        Button();
        virtual ~Button();
        void                            AddHander();
        virtual void					HandleMsg(const MsgWrap & msg)throw();
        LPCTSTR                         GetClass() const;
        virtual UINT                    GetControlFlags() const;
        virtual bool                    Activate();
        virtual void                    SetEnabled(bool bEnable = true);
        virtual SIZE                    EstimateSize(SIZE szAvailable);
        virtual void                    SetAttribute(const std::string &strName, const std::string& strValue);
        virtual void                    PaintText();
        virtual void                    PaintStatusImage();
        void                            SetClickEvent(EventClick& eve) { m_eventClick = eve;}
        YString                         GetNormalImage();
        void                            SetNormalImage(const YString& pStrImage);
        YString                         GetHotImage();
        void                            SetHotImage(const YString& pStrImage);
        YString                         GetPushedImage();
        void                            SetPushedImage(const YString& pStrImage);
        YString                         GetFocusedImage();
        void                            SetFocusedImage(const YString& pStrImage);
        YString                         GetDisabledImage();
        void                            SetDisabledImage(const YString& pStrImage);
        YString                         GetForeImage();
        void                            SetForeImage(const YString& pStrImage);
        YString                         GetHotForeImage();
        void                            SetHotForeImage(const YString& pStrImage);

        void                            SetHotBkColor(DWORD dwColor);
        DWORD                           GetHotBkColor() const;
        void                            SetHotTextColor(DWORD dwColor);
        DWORD                           GetHotTextColor() const;
        void                            SetPushedTextColor(DWORD dwColor);
        DWORD                           GetPushedTextColor() const;
        void                            SetFocusedTextColor(DWORD dwColor);
        DWORD                           GetFocusedTextColor() const;

    protected:
        UINT                            m_uButtonState;

        DWORD                           m_dwHotBkColor;
        DWORD                           m_dwHotTextColor;
        DWORD                           m_dwPushedTextColor;
        DWORD                           m_dwFocusedTextColor;
        EventClick                      m_eventClick;
        YString                         m_sNormalImage;
        YString                         m_sHotImage;
        YString                         m_sHotForeImage;
        YString                         m_sPushedImage;
        YString                         m_sPushedForeImage;
        YString                         m_sFocusedImage;
        YString                         m_sDisabledImage;
        MsgHandler						m_ButtonMsgHandler;
    };

}