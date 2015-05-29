#pragma once
#include "YUI.h"
namespace YUI
{
    class ClipRegionDef
    {
    public:
        typedef std::function<void()>   m_FunDestroyClipRegion;
       ClipRegionDef(const YYRECT &rc);
       ~ClipRegionDef(); 
       void                             SetDestroy(m_FunDestroyClipRegion funDestroy);
       YYRECT                           GetClipRect() const ;
    private:
        void* operator new(size_t size);
        void operator delete(void *);
        YYRECT                          m_rc;
        m_FunDestroyClipRegion          m_FunDestroy;
    };
}