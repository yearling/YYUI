#pragma once
#include "YUI.h"

namespace YUI
{
    //��Ϣ�����ģʽ
    template<class MSG>
    class MsgHandleChainBase
    {
    public:
        MsgHandleChainBase():m_pSuccessor(nullptr){}
        virtual ~MsgHandleChainBase(){}
    public:
        virtual void					HandleMsg(const MSG & msg)throw()
        {
            if( m_pSuccessor )
                m_pSuccessor->HandleMsg(msg);
        }
        //��Ƽ̳еĶ��󣬾�����һ��Ҫ��������Ķ���
        void							SetSuccessor(MsgHandleChainBase* pBase)
        {
            m_pSuccessor = pBase;
        }

    private:
        MsgHandleChainBase*			    m_pSuccessor;//��һ��Ҫ�����Ĵ���Ķ���
    };

   
    template<class MSG> 
    class MsgHandlerTemp : public MsgHandleChainBase<MSG>
    {
    private:
        typedef std::function<void (const MSG & )>  FUNMSG;
        typedef std::multimap<const YString,FUNMSG> MsgMap;

    public:
        virtual void					HandleMsg(const MSG & msg)throw()
        {
            auto iterLow = m_MessageMap.lower_bound(msg.strType);
            auto iterUp = m_MessageMap.upper_bound(msg.strType);
            if(iterLow == iterUp)
                MsgHandleChainBase<MSG>::HandleMsg(msg);
            while( iterLow != iterUp )
            {
                iterLow->second(msg);
                iterLow++;
            }
        }

        void							AddEntry(const YString &strType,FUNMSG func)
        {
             m_MessageMap.insert(MsgMap::value_type(strType,func));
        }
        void							DeleteEntry(const YString &strType,FUNMSG )
        {
             assert(0 && "unimplement");
        }
    private:   
        MsgMap							m_MessageMap;  //ÿ���ؼ������Լ�����Ϣ 
    };
    typedef MsgHandlerTemp<MsgWrap>     MsgHandler;
}
