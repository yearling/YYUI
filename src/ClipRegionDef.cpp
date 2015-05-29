#include "YUI.h"
#include "ClipRegionDef.h"

namespace YUI
{


    ClipRegionDef::ClipRegionDef(const YYRECT &rc)
        :m_rc(rc)
    {

    }

    ClipRegionDef::~ClipRegionDef()
    {
        if(m_FunDestroy)
            m_FunDestroy();
    }

    void ClipRegionDef::SetDestroy(m_FunDestroyClipRegion funDestroy)
    {
        m_FunDestroy = funDestroy;
    }

    YUI::YYRECT ClipRegionDef::GetClipRect() const
    {
        return m_rc;
    }

}
