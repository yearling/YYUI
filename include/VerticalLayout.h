#include "YUI.h"
#include "Container.h"

namespace YUI
{
    class VerticalLayout: public Container
    {
    public:    
        VerticalLayout();
        virtual LPCTSTR                 GetClass() const;
        virtual std::shared_ptr<ControlUI>                  
                                        GetInterface(const YString & strName);
        virtual void                    SetAttribute(const YString &pstrName, const YString& pstrValue);
        virtual UINT                    GetControlFlags() const;
        virtual void                    DoEvent(ControlEvent& eve);
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