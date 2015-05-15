#pragma once
#include "YUI.h"

namespace YUI
{
    //��Ϣ�����ģʽ
    class MsgHandleChainBase
    {
    public:
        MsgHandleChainBase();
        virtual                      ~MsgHandleChainBase();
        virtual void                 HandleMsg(const NotifyMsg & msg)throw();
        virtual void					HandleMsg(const MsgWrap & msg)throw();
        //��Ƽ̳еĶ��󣬾�����һ��Ҫ��������Ķ���
        void                         SetSuccessor(std::shared_ptr<MsgHandleChainBase> sp);

    private:
        std::weak_ptr<MsgHandleChainBase>  m_wpSuccessor;//��һ��Ҫ�����Ĵ���Ķ���
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
        MsgMap							m_MessageMap;  //ÿ���ؼ������Լ�����Ϣ 
    };
}
