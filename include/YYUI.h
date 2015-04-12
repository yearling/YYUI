//////////////////////////////////////////////////////////////////////////
// create by yyCom 2015/4/10
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <tchar.h>
#include <Windows.h>
#include <exception>
#include <string>
#include <memory>
#include "YYExceptionAll.h"
#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
namespace YYCOM
{
#if defined UNICODE || defined _UNICODE
    typedef std::wstring                YString;
#else
    typedef std::string                 YString;
#endif
	struct YYUIException : virtual YYCOM::Exception, virtual std::exception{};
	typedef YYCOM::ErrorInfo<struct tag_err_str,YString> UIErrorStr;

    class YControlUI;
    class YNotifyPump;      
}
