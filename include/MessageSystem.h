#pragma once
#include "YUI.h"

namespace YUI
{
    class MessageSystem
    {
    public:
        static  MessageSystem*          GetInstance() { return &m_Instance;}
        void                            MeesageLoop() throw();
    private:
        MessageSystem();
        MessageSystem(const MessageSystem &);
        MessageSystem operator=(const MessageSystem &);
        static MessageSystem            m_Instance;
    };
}