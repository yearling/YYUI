#pragma  once
#include "YUI.h"
#include "Container.h"

namespace YUI
{
    class TabLayout: public Container
    {
    public:
        TabLayout();
        virtual void SetAttribute(const std::string &strName, const std::string& strValue);
        virtual LPCTSTR                 GetClass() const;
        virtual std::shared_ptr<ControlUI> QueryInterface(const std::string & strName);
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