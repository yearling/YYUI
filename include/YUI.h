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
#include <iostream>
using YYCOM::Exception;
using YYCOM::ThrowException;
using YYCOM::ErrorInfo;
namespace YUI
{
    //YString 用的就是标准string
#if defined UNICODE || defined _UNICODE
    typedef std::wstring                YString;
#define Ycout std::wcout
#else
    typedef std::string                 YString;
#define  Ycout std::cout
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
#define  MSG_TABSELECT                  (_T("tabselect"))
#define  MSG_Click                      (_T("click"))
    //定义控件名
#define  CTR_EDIT                       (("Edit"))
#define  CTR_LIST                       (("List"))
#define  CTREXT                         (("Text"))

#define  CTR_COMBO                      (("Combo"))
#define  CTR_LABEL                      (("Label"))
#define  CTR_FLASH						(("Flash"))

#define  CTR_BUTTON                     (("Button"))
#define  CTR_OPTION                     (("Option"))
#define  CTR_SLIDER                     (("Slider"))

#define  CTR_CONTROL                    (("Control"))
#define  CTR_ACTIVEX                    (("ActiveX"))

#define  CTR_LISTITEM                   (("ListItem"))
#define  CTR_PROGRESS                   (("Progress"))
#define  CTR_RICHEDIT                   (("RichEdit"))
#define  CTR_CHECKBOX                   (("CheckBox"))
#define  CTR_COMBOBOX                   (("ComboBox"))
#define  CTR_DATETIME                   (("DateTime"))
#define  CTR_TREEVIEW                     (("TreeView"))
#define  CTR_TREENODE                     (("TreeNode"))

#define  CTR_CONTAINER                  (("Container"))
#define  CTR_TABLAYOUT                    (("TabLayout"))
#define  CTR_SCROLLBAR                  (("ScrollBar"))

#define  CTR_LISTHEADER                 (("ListHeader"))
#define  CTR_TILELAYOUT                   (("TileLayout"))
#define  CTR_WEBBROWSER                 (("WebBrowser"))

#define  CTR_CHILDLAYOUT                (("ChildLayout"))
#define  CTR_LISTELEMENT                (("ListElement"))

#define  CTR_DIALOGLAYOUT               (("DialogLayout"))

#define  CTR_VERTICALLAYOUT             (("VerticalLayout"))
#define  CTR_LISTHEADERITEM             (("ListHeaderItem"))

#define  CTR_LISTTEXTELEMENT            (("ListTextElement"))

#define  CTR_HORIZONTALLAYOUT           (("HorizontalLayout"))
#define  CTR_LISTLABELELEMENT           (("ListLabelElement"))

#define  CTR_LISTCONTAINERELEMENT       (("ListContainerElement"))

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_ME_FIRST      0x80000000
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
        YString                         m_strFontName;
        int                             m_nSize;
        bool                            m_bBold;                     
        bool                            m_bUnderline;
        bool                            m_bItalic;
        TEXTMETRIC                      m_tm;
        static void                     FontInfoDeleter(FontInfo *pFont)
        {
            if(pFont->m_hFont)
                ::DeleteObject(pFont->m_hFont);
            delete pFont;
        }
    };

    struct ImageInfo
    {
        HBITMAP                         m_hBitmap;
        int                             m_nX;
        int                             m_nY;
        bool                            m_alphaChannel;
        YString                         m_strResType;
        DWORD                           m_dwMask;

        static void                     ImageInfoDeleter(ImageInfo * pImag )
        {
            if(pImag->m_hBitmap) 
                ::DeleteObject(pImag->m_hBitmap);
            delete pImag;
        } 
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
