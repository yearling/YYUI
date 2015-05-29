#include "YUI.h"
#include "SystemInfo.h"

namespace YUI
{
    SystemInfo::SystemInfo(void)
    {
    }


    SystemInfo::~SystemInfo(void)
    {
    }

    SystemInfo* SystemInfo::GetInstance()
    {
        return &m_Instant;
    }

    HINSTANCE SystemInfo::GetProcessInstance() const
    {
        return m_hIns;
    }

    void SystemInfo::SetProcessInstance(HINSTANCE hIns)
    {
        m_hIns =hIns;
    }

    YUI::SystemInfo SystemInfo::m_Instant;

}