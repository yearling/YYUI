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
        UINT                            GetControlFlags() const;
        void                            SetSepWidth(int iWidth);
        int                             GetSepWidth() const;
        void                            SetSepImmMode(bool bImmediately);
        bool                            IsSepImmMode() const;
        virtual void                    SetAttribute(const std::string &strName, const std::string& strValue);
        virtual void SetPos(YYRECT &rc);
        virtual void DoPostPaint(HDC hDC, const YYRECT& rcPaint);

        YYRECT GetThumbRect(bool bUseNew = false) const;
    protected:
        int                             m_iSepWidth;
        UINT                            m_uButtonState;
        POINT                           ptLastMouse;
        RECT                            m_rcNewPos;
        bool                            m_bImmMode;
    };
}