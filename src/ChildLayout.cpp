#include "YUI.h"
#include "ChildLayout.h"
#include "DlgBuilder.h"

namespace YUI
{


    ChildLayout::ChildLayout()
    {

    }

    void ChildLayout::Init()
    {
        if (!m_pstrXMLFile.empty())
        {
            DialogBuilder builder;
            auto &pChildWindow = builder.Create(m_pstrXMLFile, (UINT)0, NULL, m_pManager,std::weak_ptr<ControlUI>());

            if (pChildWindow)
            {
                this->Add(pChildWindow);
            }
            else
            {
                this->RemoveAll();
            }
        }
    }

    void ChildLayout::SetAttribute(const YString &pstrName, const YString& pstrValue)
    {
        if(pstrName==_T("xmlfile"))
            SetChildLayoutXML(pstrValue);
        else
            Container::SetAttribute(pstrName,pstrValue);
    }

    void ChildLayout::SetChildLayoutXML(const YString& pXML)
    {
        m_pstrXMLFile=pXML;
    }

    YUI::YString ChildLayout::GetChildLayoutXML()
    {
        return m_pstrXMLFile;
    }

    LPCTSTR ChildLayout::GetClass() const
    {
        return _T("ChildLayoutUI");

    }

    std::shared_ptr<ControlUI> ChildLayout::GetInterface(const YString & strName)
    {
        if( strName == CTR_CHILDLAYOUT ) 
            return this->shared_from_this();
        return Container::GetInterface(strName);
    }

}