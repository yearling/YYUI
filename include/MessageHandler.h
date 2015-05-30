#pragma once
#include "YUI.h"

namespace YUI
{
    //消息链设计模式
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
        //设计继承的对象，就是下一步要交给处理的对像
        void							SetSuccessor(MsgHandleChainBase* pBase)
        {
            m_pSuccessor = pBase;
        }

    private:
        MsgHandleChainBase*			    m_pSuccessor;//下一个要交给的处理的对像
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
        MsgMap							m_MessageMap;  //每个控件处理自己的消息 
    };
    typedef MsgHandlerTemp<MsgWrap>     MsgHandler;
}
