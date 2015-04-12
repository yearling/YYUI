#pragma once

#include "YYUI.h"
#include <memory>
namespace YYCOM
{
    class IDialogBuilderCallBack
    {
    public:
        virtual std::shared_ptr<YControlUI> 
                                        CreateControl(std::string pstrClass) =0 ;
    };

    class YDialogBuilder
    {
    public:
        YDialogBuilder();
    };
}