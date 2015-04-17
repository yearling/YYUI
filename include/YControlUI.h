
#pragma once

#include "YYUI.h"
#include "YYUIDef.h"
#include <functional>
namespace YYCOM
{

    class YControlUI
    {
    public:
        typedef std::function<bool(NotifyMsg &)> FuncEvent;
        typedef std::function<std::weak_ptr<YControlUI>(std::weak_ptr<YControlUI>) > funcFindControl;
    public:
        YString                         GetName();
        virtual void                    Event( tagYEventUI& eve );
        bool                            IsUpdateNeeded() const;
        void                            SetPos(const RECT &rc);
        std::weak_ptr<YControlUI>       FindControl(funcFindControl func,UINT uFlags);
        YString                         GetVirtualWnd() const;
    public:
        FuncEvent                       OnNotify;
    };
}