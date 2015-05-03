#pragma once

#include "YUI.h"
#include "Container.h"

namespace YUI
{
    class ChildLayout : public Container
    {
    public:
        ChildLayout();
        void                            Init();
        virtual void SetAttribute(const std::string &strName, const std::string& strValue);
        void SetChildLayoutXML(const YString& pXML);
        YString GetChildLayoutXML();
        virtual LPCTSTR                 GetClass() const;
        virtual std::shared_ptr<ControlUI> QueryInterface(const std::string & strName);

    private:
        YString                     m_pstrXMLFile;
    };
}