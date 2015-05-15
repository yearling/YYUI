#pragma once
#include "YUI.h"

namespace YUI
{
    //消息链设计模式
    class MsgHandleChainBase
    {
    public:
        MsgHandleChainBase();
        virtual                      ~MsgHandleChainBase();
        virtual void                 HandleMsg(const NotifyMsg & msg)throw();
        virtual void					HandleMsg(const MsgWrap & msg)throw();
        //设计继承的对象，就是下一步要交给处理的对像
        void                         SetSuccessor(std::shared_ptr<MsgHandleChainBase> sp);

    private:
        std::weak_ptr<MsgHandleChainBase>  m_wpSuccessor;//下一个要交给的处理的对像
    };

   

    class IMsgHandler : public MsgHandleChainBase
    {
    public:
        typedef std::multimap<const YString,FucHandleMsg> MsgMap;
    public:
        IMsgHandler();
        virtual ~IMsgHandler();
        virtual void					HandleMsg(const MsgWrap & msg)throw();	
        void							AddEntry(const YString &strType,FucHandleMsg  );
        void							DeleteEntry(const YString &strType,FucHandleMsg );	
    private:   
        MsgMap							m_MessageMap;  //每个控件处理自己的消息 
    };
}
