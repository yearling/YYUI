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
        virtual void                    SetAttribute(const YString &pstrName, const YString& pstrValue);
        void                            SetChildLayoutXML(const YString& pXML);
        YString                         GetChildLayoutXML();
        virtual LPCTSTR                 GetClass() const;
        virtual std::shared_ptr<ControlUI> QueryInterface(const YString & strName);

    private:
        YString                         m_pstrXMLFile;
    };
}