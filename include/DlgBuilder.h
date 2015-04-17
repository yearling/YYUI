#pragma once

#include "YUI.h"
#include <memory>

namespace YUI
{
    class IDialogBuilderCallBack
    {
    public:
        virtual std::shared_ptr<ControlUI> 
                                        CreateControl(YString pstrClass) =0 ;
    };

    class DialogBuilder
    {
    public:
        DialogBuilder();
    };
}