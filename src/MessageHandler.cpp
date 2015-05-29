#include "YUI.h"
#include "MessageHandler.h"
namespace YUI
{
    


    void MsgHandleChainBase::HandleMsg(const NotifyMsg & msg)
    {
            m_pSuccessor->HandleMsg(msg);
    }

    void MsgHandleChainBase::HandleMsg(const MsgWrap & msg)
    {
		if( m_pSuccessor )
            m_pSuccessor->HandleMsg(msg);
    }

    void MsgHandleChainBase::SetSuccessor(MsgHandleChainBase* pBase)
    {
        m_pSuccessor = pBase;
    }

    MsgHandleChainBase::MsgHandleChainBase()
		:m_pSuccessor( nullptr)
    {

    }

    MsgHandleChainBase::~MsgHandleChainBase()
    {

    }


    MsgHandler::MsgHandler()
    {

    }

    MsgHandler::~MsgHandler()
    {

    }

    void MsgHandler::HandleMsg(const MsgWrap & msg) throw()
    {
        auto iterLow = m_MessageMap.lower_bound(msg.strType);
        auto iterUp = m_MessageMap.upper_bound(msg.strType);
        if(iterLow == iterUp)
            MsgHandleChainBase::HandleMsg(msg);
        while( iterLow != iterUp )
        {
            iterLow->second(msg);
            iterLow++;
        }
    }

    void MsgHandler::AddEntry(const YString &strType,FucHandleMsg fuc)
    {
        m_MessageMap.insert(MsgMap::value_type(strType,fuc));
    }

    void MsgHandler::DeleteEntry(const YString &strType,FucHandleMsg)
    {
        assert(0 && "unimplement");
    }

}