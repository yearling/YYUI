#pragma once
#include "YYUI.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <wingdi.h>

namespace YYCOM
{


    struct NotifyMsg
    {
        std::string                     strType;
        std::string                     strVirtualWnd;
        std::weak_ptr<YControlUI>       pSender;
        unsigned long                   lTimeStamp;
        POINT                           ptMouse;
        WPARAM                          wParam;
        LPARAM                          lParam;
    };



    typedef std::function<void(const NotifyMsg&)> FunNofity;

    struct YStrStr
    {
    public:
        YStrStr() {};
        YStrStr(const std::string &strType
            ,const std::string &strContent):
        m_strType(strType),
            m_strControlName(strContent)
        {}
        std::string  m_strType;
        std::string  m_strControlName;
    };
    class ssToHash
    {
    public:
        size_t operator()( const YStrStr & s)
        {
            std::hash<std::string> s2h;
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
    struct tagTFontInfo
    {
        HFONT                           m_hFont;
        std::string                     m_strFontName;
        int                             m_nSize;
        bool                            m_bBold;                     
        bool                            m_bUnderline;
        bool                            m_bItalic;
        TEXTMETRIC                      m_tm;
    };

    struct tagTImageInfo
    {
        HBITMAP                         m_hBitmap;
        int                             m_nX;
        int                             m_nY;
        bool                            m_alphaChannel;
        std::string                     m_strResType;
        DWORD                           m_dwMask;
    };

    struct tagTEventUI
    {
        int                             m_Type;
        YControlUI*                     m_pSender;
        DWORD                           m_dwTimestamp;
        POINT                           m_ptMouse;
        TCHAR                           m_chKey;
        WORD                            m_wKeyState;
        WPARAM                          m_wParam;
        LPARAM                          m_lParam;
    };

    struct tagTRelativePosUI
    {
        bool                            m_bRelative;
        SIZE                            m_szParent;
        int                             m_nMoveXPercent;
        int                             m_nMoveYPercent;
        int                             m_nZoomXPercent;
        int                             m_nZoomYPercent;
    };

    class INotifyUI
    {
    public:
        virtual void                    Notify(NotifyMsg &msg) = 0;
    };

    class IMessageFilterUI
    {
    public:
        virtual LRESULT                 MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) = 0 ;
    };

    class ITranslateAccelerator
    {
    public:
        virtual LRESULT                 TranslateAccelerator(MSG *pMsg) = 0;
    };

}