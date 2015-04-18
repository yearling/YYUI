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
#define  MSG_TIMER                      (_T("timer"))
#define  MSG_MENU                       (_T("menu"))
    //定义控件名
#define  CTR_EDIT                       (_T("Edit"))
#define  CTR_LIST                       (_T("List"))
#define  CTR_TEXT                       (_T("Text"))

#define  CTR_COMBO                      (_T("Combo"))
#define  CTR_LABEL                      (_T("Label"))
#define  CTR_FLASH						(_T("Flash"))

#define  CTR_BUTTON                     (_T("Button"))
#define  CTR_OPTION                     (_T("Option"))
#define  CTR_SLIDER                     (_T("Slider"))

#define  CTR_CONTROL                    (_T("Control"))
#define  CTR_ACTIVEX                    (_T("ActiveX"))

#define  CTR_LISTITEM                   (_T("ListItem"))
#define  CTR_PROGRESS                   (_T("Progress"))
#define  CTR_RICHEDIT                   (_T("RichEdit"))
#define  CTR_CHECKBOX                   (_T("CheckBox"))
#define  CTR_COMBOBOX                   (_T("ComboBox"))
#define  CTR_DATETIME                   (_T("DateTime"))
#define  CTR_TREEVIEW                   (_T("TreeView"))
#define  CTR_TREENODE                   (_T("TreeNode"))

#define  CTR_CONTAINER                  (_T("Container"))
#define  CTR_TABLAYOUT                  (_T("TabLayout"))
#define  CTR_SCROLLBAR                  (_T("ScrollBar"))

#define  CTR_LISTHEADER                 (_T("ListHeader"))
#define  CTR_TILELAYOUT                 (_T("TileLayout"))
#define  CTR_WEBBROWSER                 (_T("WebBrowser"))

#define  CTR_CHILDLAYOUT                (_T("ChildLayout"))
#define  CTR_LISTELEMENT                (_T("ListElement"))

#define  CTR_DIALOGLAYOUT               (_T("DialogLayout"))

#define  CTR_VERTICALLAYOUT             (_T("VerticalLayout"))
#define  CTR_LISTHEADERITEM             (_T("ListHeaderItem"))

#define  CTR_LISTTEXTELEMENT            (_T("ListTextElement"))

#define  CTR_HORIZONTALLAYOUT           (_T("HorizontalLayout"))
#define  CTR_LISTLABELELEMENT           (_T("ListLabelElement"))

#define  CTR_LISTCONTAINERELEMENT       (_T("ListContainerElement"))

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

    // Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

    // Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_ME_FIRST      0x80000000

    // Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020

    // Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040

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
