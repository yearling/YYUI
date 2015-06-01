#pragma once

#include "YUI.h"
#include <memory>
#include "thirdparty\tinyxml2\tinyxml2.h"

namespace YUI
{
    class IDialogBuilderCallback
    {
    public:
        virtual ControlUI*              CreateControl(std::string pstrClass) =0 ;
    };

    class DialogBuilder
    {
    public:
        DialogBuilder();

        CountRefPtr<ControlUI>          Create(YString xml, 
                                        WindowProperty & Winproperty,
                                        IDialogBuilderCallback* pCallback,
                                        ControlUI* pParent);

        CountRefPtr<ControlUI>          Create(IDialogBuilderCallback* pCallback , 
                                        WindowProperty &Winproperty,
                                        ControlUI* pParent);
    private:
       CountRefPtr<ControlUI>           Parse(tinyxml2::XMLNode* pRoot, 
                                        ControlUI* pParent);
       IDialogBuilderCallback*          m_pCallback;
       LPCTSTR                          m_pstrtype;
       std::shared_ptr<tinyxml2::XMLDocument>
                                        m_pDoc;
    };
}