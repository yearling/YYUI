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
using YYCOM::Exception;
using YYCOM::ThrowException;
using YYCOM::ErrorInfo;
namespace YUI
{
    //YString 用的就是标准string
#if defined UNICODE || defined _UNICODE
    typedef std::wstring                YString;
#else
    typedef std::string                 YString;
#endif

    //定义异常基类
	struct YYUIException : virtual YYCOM::Exception, virtual std::exception{};
	typedef YYCOM::ErrorInfo<struct tag_err_str,YString> UIErrorStr;

    //定义消息名
#define  MSG_WindowInit                 (_T("windowinit"))
#define  MSG_KillFocus                  (_T("killfocus"))
#define  MSG_SetFocus                   (_T("setfocus"))
   
    //前置定义
    class ControlUI;
    class INotifyUI;
    class IMessageFilterUI;
    class ITranslateAccelerator;
    class PaintManagerUI;

    //定义消息体
    struct NotifyMsg
    {
        YString                         strType;
        YString                         strVirtualWnd;
        std::weak_ptr<ControlUI>       pSender;
        unsigned long                   lTimeStamp;
        POINT                           ptMouse;
        WPARAM                          wParam;
        LPARAM                          lParam;
    };
    typedef std::function<void(const NotifyMsg&)> FunNofity;

    typedef enum EVENTTYPE_UI
    {
        UIEVENT__FIRST = 1,
        UIEVENT__KEYBEGIN,
        UIEVENT_KEYDOWN,
        UIEVENT_KEYUP,
        UIEVENT_CHAR,
        UIEVENT_SYSKEY,
        UIEVENT__KEYEND,
        UIEVENT__MOUSEBEGIN,
        UIEVENT_MOUSEMOVE,
        UIEVENT_MOUSELEAVE,
        UIEVENT_MOUSEENTER,
        UIEVENT_MOUSEHOVER,
        UIEVENT_BUTTONDOWN,
        UIEVENT_BUTTONUP,
        UIEVENT_RBUTTONDOWN,
        UIEVENT_DBLCLICK,
        UIEVENT_CONTEXTMENU,
        UIEVENT_SCROLLWHEEL,
        UIEVENT__MOUSEEND,
        UIEVENT_KILLFOCUS,
        UIEVENT_SETFOCUS,
        UIEVENT_WINDOWSIZE,
        UIEVENT_SETCURSOR,
        UIEVENT_TIMER,
        UIEVENT_NOTIFY,
        UIEVENT_COMMAND,
        UIEVENT__LAST,
    };
    struct FontInfo
    {
        HFONT                           m_hFont;
        YString                     m_strFontName;
        int                             m_nSize;
        bool                            m_bBold;                     
        bool                            m_bUnderline;
        bool                            m_bItalic;
        TEXTMETRIC                      m_tm;
    };

    struct ImageInfo
    {
        HBITMAP                         m_hBitmap;
        int                             m_nX;
        int                             m_nY;
        bool                            m_alphaChannel;
        YString                     m_strResType;
        DWORD                           m_dwMask;
    };
    struct TimerInfo
    {
        std::weak_ptr<ControlUI>       pSender;
        UINT                            nLocalID;
        HWND                            hWnd;
        UINT                            uWinTimer;
        bool                            bKilled;
    };
    struct ControlEvent
    {
        int                             m_Type;
        std::weak_ptr<ControlUI>        m_pSender;
        DWORD                           m_dwTimestamp;
        POINT                           m_ptMouse;
        TCHAR                           m_chKey;
        WORD                            m_wKeyState;
        WPARAM                          m_wParam;
        LPARAM                          m_lParam;
        ControlEvent():m_Type( UIEVENT__FIRST )
                                        ,m_dwTimestamp(0)
                                        ,m_wKeyState(0)
                                        ,m_wParam(0)
                                        ,m_lParam(0)
        {
            m_ptMouse.x = -1;
            m_ptMouse.y = -1;
            m_chKey = _T('\0');
        }

    };

    struct RelativePosUI
    {
        bool                            m_bRelative;
        SIZE                            m_szParent;
        int                             m_nMoveXPercent;
        int                             m_nMoveYPercent;
        int                             m_nZoomXPercent;
        int                             m_nZoomYPercent;
    };

}
