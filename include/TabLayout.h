#pragma  once
#include "YUI.h"
#include "Container.h"

namespace YUI
{
    class TabLayout: public Container
    {
    public:
        TabLayout();
        virtual void                    SetAttribute(const YString &pstrName, const YString& pstrValue);
        virtual LPCTSTR                 GetClass() const;
        virtual std::shared_ptr<ControlUI>                  
                                        GetInterface(const YString & strName);
        virtual bool                    Add(std::shared_ptr<ControlUI>& pControl);

        virtual bool                    Remove(std::shared_ptr<ControlUI> &pControl) ;
        virtual void                    RemoveAll();
        std::shared_ptr<ControlUI>      GetCurSel() const;
        bool                            SelectItem(std::shared_ptr<ControlUI> pControl);

       virtual void                     SetPos(RECT rc);
    protected:
        std::shared_ptr<ControlUI>      m_iCurSel;
    };
}