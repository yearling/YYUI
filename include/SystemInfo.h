#pragma once
#include "YUI.h"
namespace YUI
{

    class SystemInfo
    {
    private:
        SystemInfo(void);
        SystemInfo(const SystemInfo     &);
        SystemInfo operator =(const SystemInfo &);
        static SystemInfo               m_Instant;
    public:
        ~SystemInfo(void);
        static SystemInfo*              GetInstance();
        HINSTANCE                       GetProcessInstance() const;
        void                            SetProcessInstance(HINSTANCE hIns); 
    private:
        HINSTANCE                       m_hIns;
    };


}