#pragma once
#include "YUI.h"

namespace YUI
{
    //��Ϣ�����ģʽ
    class MsgHandleChainBase
    {
    public:
        MsgHandleChainBase();
        virtual                         ~MsgHandleChainBase();
        virtual void					HandleMsg(const NotifyMsg & msg)throw();
        virtual void					HandleMsg(const MsgWrap & msg)throw();
        //��Ƽ̳еĶ��󣬾�����һ��Ҫ��������Ķ���
        void							SetSuccessor(MsgHandleChainBase* pBase);

    private:
        MsgHandleChainBase*			   m_pSuccessor;//��һ��Ҫ�����Ĵ���Ķ���
    };

   

    class MsgHandler : public MsgHandleChainBase
    {
    public:
        typedef std::multimap<const YString,FucHandleMsg> MsgMap;
    public:
        MsgHandler();
        virtual ~MsgHandler();
        virtual void					HandleMsg(const MsgWrap & msg)throw();	
        void							AddEntry(const YString &strType,FucHandleMsg  );
        void							DeleteEntry(const YString &strType,FucHandleMsg );	
    private:   
        MsgMap							m_MessageMap;  //ÿ���ؼ������Լ�����Ϣ 
    };
}
