#include "YUI.h"
#include "Container.h"

namespace YUI
{
    class VerticalLayout: public Container
    {
    public:    
        VerticalLayout();
        void                            AddHandler();
        virtual LPCTSTR                 GetClass() const;
        virtual std::shared_ptr<ControlUI> QueryInterface(const std::string & strName);
        virtual void SetAttribute(const std::string &strName, const std::string& strValue);
        virtual UINT                    GetControlFlags() const;
        virtual void                    SetPos(RECT &rc);
        virtual void                    DoPostPaint(HDC hDC, const RECT& rcPaint);

        RECT                            GetThumbRect(bool bUseNew = false) const;
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