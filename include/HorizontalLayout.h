#pragma once
#include "YUI.h"
#include "Container.h"

namespace YUI
{
    class HorizontalLayout : public Container
    {
    public:     
        HorizontalLayout();
        LPCTSTR                         GetClass() const;
        virtual std::shared_ptr<ControlUI>                  
                                        GetInterface(const YString & strName);
        UINT                            GetControlFlags() const;

        void                            SetSepWidth(int iWidth);
        int                             GetSepWidth() const;
        void                            SetSepImmMode(bool bImmediately);
        bool                            IsSepImmMode() const;
        virtual void                    SetAttribute(const YString &pstrName, const YString& pstrValue);
        virtual void                    DoEvent(ControlEvent& eve);
        virtual void                    SetPos(RECT &rc);
        virtual void                    DoPostPaint(HDC hDC, const RECT& rcPaint);

        RECT                            GetThumbRect(bool bUseNew = false) const;
    protected:
        int                             m_iSepWidth;
        UINT                            m_uButtonState;
        POINT                           ptLastMouse;
        RECT                            m_rcNewPos;
        bool                            m_bImmMode;
    };
}