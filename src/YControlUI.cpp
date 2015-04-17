#include "YYUI.h"
#include "YYUIDef.h"
#include "YControlUI.h"

namespace YYCOM
{


    YString YControlUI::GetName()
    {
        return _T("");
    }

    void YControlUI::Event(tagYEventUI& eve)
    {

    }

    bool YControlUI::IsUpdateNeeded() const 
    {
        return false;
    }

    void YControlUI::SetPos(const RECT &rc)
    {

    }

    std::weak_ptr<YControlUI> YControlUI::FindControl(funcFindControl func,UINT uFlags)
    {
         return std::weak_ptr<YControlUI>();
    }

    YYCOM::YString YControlUI::GetVirtualWnd() const
    {
        return _T("");
    }

}