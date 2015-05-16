#pragma once

#include "YUI.h"
#include <memory>
#include "thirdparty\tinyxml2\tinyxml2.h"

namespace YUI
{
    class IDialogBuilderCallback
    {
    public:
        virtual std::shared_ptr<ControlUI> CreateControl(std::string pstrClass) =0 ;
    };

    class DialogBuilder
    {
    public:
        DialogBuilder();

         std::shared_ptr<ControlUI>     Create(YString xml, 
                                        WindowProperty & Winproperty,
                                        IDialogBuilderCallback* pCallback,
                                        std::weak_ptr<ControlUI> pParent);

         std::shared_ptr<ControlUI>     Create(IDialogBuilderCallback* pCallback , 
                                        WindowProperty &Winproperty,
                                        std::weak_ptr<ControlUI> pParent);
    private:
       std::shared_ptr<ControlUI>       Parse(tinyxml2::XMLNode* pRoot, 
                                        std::weak_ptr<ControlUI> pParent);
       IDialogBuilderCallback*          m_pCallback;
       LPCTSTR                          m_pstrtype;
       std::shared_ptr<tinyxml2::XMLDocument>
                                        m_pDoc;
    };
}