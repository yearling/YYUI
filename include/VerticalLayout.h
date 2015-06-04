#include "YUI.h"
#include "Container.h"

namespace YUI
{
    class VerticalLayout: public Container
    {
    public:    
        VerticalLayout();
        virtual LPCTSTR                 GetClass() const;
        virtual void                    SetAttribute(const std::string &strName, const std::string& strValue);
        virtual UINT                    GetControlFlags() const;
        virtual void SetPos(YYRECT &rc);
        virtual void DoPostPaint(HDC hDC, const YYRECT& rcPaint);

        YYRECT							GetThumbRect(bool bUseNew = false) const;
        void                            SetSepHeight(int iHeight);
        int                             GetSepHeight() const;
        void                            SetSepImmMode(bool bImmediately);
        bool                            IsSepImmMode() const;
    protected:
        int                             m_iSepHeight;
        UINT                            m_uButtonState;
        POINT                           ptLastMouse;
        RECT                            m_rcNewPos;
        bool                            m_bImmMode;
    };
}