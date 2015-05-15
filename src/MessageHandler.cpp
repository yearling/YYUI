#include "YUI.h"
#include "MessageHandler.h"
namespace YUI
{
    


    void MsgHandleChainBase::HandleMsg(const NotifyMsg & msg)
    {
        auto sp= m_wpSuccessor.lock();
        if(sp)
            sp->HandleMsg(msg);
    }

    void MsgHandleChainBase::HandleMsg(const MsgWrap & msg)
    {
        auto sp= m_wpSuccessor.lock();
        if(sp)
            sp->HandleMsg(msg);
    }

    void MsgHandleChainBase::SetSuccessor(std::shared_ptr<MsgHandleChainBase> & sp)
    {
        m_wpSuccessor = sp;
    }

    MsgHandleChainBase::MsgHandleChainBase()
    {

    }

    MsgHandleChainBase::~MsgHandleChainBase()
    {

    }


    IMsgHandler::IMsgHandler()
    {

    }

    IMsgHandler::~IMsgHandler()
    {

    }

    void IMsgHandler::HandleMsg(const MsgWrap & msg) throw()
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

    void IMsgHandler::AddEntry(const YString &strType,FucHandleMsg fuc)
    {
        m_MessageMap.insert(MsgMap::value_type(strType,fuc));
    }

    void IMsgHandler::DeleteEntry(const YString &strType,FucHandleMsg)
    {
        assert(0 && "unimplement");
    }

}