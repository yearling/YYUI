#pragma once
#include "YUI.h"
#include "ControlUI.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <wingdi.h>

namespace YUI
{
    struct YStrStr
    {
    public:
        YStrStr() {};
        YStrStr(const YString &strType
            ,const YString &strContent):
        m_strType(strType),
            m_strControlName(strContent)
        {}
        YString  m_strType;
        YString  m_strControlName;
    };
    class ssToHash
    {
    public:
        size_t operator()( const YStrStr & s)
        {
            std::hash<YString> s2h;
            return s2h(s.m_strType)+ s2h(s.m_strControlName);
        }
    };
    class ssEql
    {
    public:
        bool operator()(const YStrStr &lhs, const YStrStr &rhs)
        {
            return (lhs.m_strType==rhs.m_strType) && (lhs.m_strControlName == rhs.m_strControlName);
        }

    };
    typedef std::unordered_map<YStrStr,FunNofity,ssToHash,ssEql> YMessageHash;

    
    
}