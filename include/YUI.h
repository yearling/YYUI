//////////////////////////////////////////////////////////////////////////
// create by yyCom 2015/4/10
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <tchar.h>
#include <Windows.h>
#include <Windowsx.h>
#include <exception>
#include <string>
#include <memory>
#include "YYExceptionAll.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <d2derr.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>
#include <d2d1effects.h>
#include <d2d1effecthelpers.h>
#include <dwrite.h>
#include <wincodec.h>
#include <atlcomcli.h>
#include <d3d11.h>
#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include "CountRef.h"
#include "UIUtility.h"
using YYCOM::Exception;
using YYCOM::ThrowException;
using YYCOM::ErrorInfo;

//
//#ifdef  UTF8ToGBK
//#undef UTF8ToGBK
//#endif 
//
//#ifdef GBKToUTF8
//#undef GBKToUTF8
//#endif

#if defined _UNICODE | defined UNICODE
#define UTF8ToGBK UTF8ToGBKW 
#define GBKToUTF8 GBKToUTF8W
#else 
#define UTF8ToGBK UTF8ToGBKA 
#define GBKToUTF8 GBKToUTF8A
#endif

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
	typedef YYCOM::ErrorInfo<struct tag_err_hr,HRESULT>	 UIErrorHr;

    

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
#define  CTR_TREEVIEW                   (("TreeView"))
#define  CTR_TREENODE                   (("TreeNode"))

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
    class IMessageFilterUI;
    class ITranslateAccelerator;
    class RenderD2D;
    class WindowWnd;
    class WindowProperty;
    class ControlManager;
    //定义常用的SP

    //新的消息体
	struct MsgWrap
	{
		YString							strType;
		ControlUI*						pSender;
		unsigned long					lTimeStamp;
		POINT							ptMouse;
		WPARAM							wParam;
		LPARAM							lParam;
	};
    typedef std::function<void(const MsgWrap&)> FucHandleMsg;

#define  UIMSG_SETFOCUS     _T("SETFOCUS")
#define  UIMSG_KILLFOCUS    _T("KILLFOCUS")
#define  UIMSG_SETCURSOR    _T("SETCURSOR")
#define  UIMSG_TIMER        _T("TIMER")
#define  UIMSG_KEYDOWN      _T("KEYDOWN")
#define  UIMSG_KEYUP        _T("KEYUP")
#define  UIMSG_LBUTTONDOWN  _T("LBUTTONDOWN")
#define  UIMSG_LBUTTONBLICK _T("LBUTTONBLICK")
#define  UIMSG_DBLCLICK     _T("DLBUTTONBLICK")
#define  UIMSG_LBUTTONUP    _T("LBUTTONUP")
#define  UIMSG_RBUTTONDOWN  _T("RBUTTONDOWN")
#define  UIMSG_MOUSEMOVE    _T("MOUSEMOVE")
#define  UIMSG_MOUSEENTER   _T("MOUSEENTER")
#define  UIMSG_MOUSELEAVE   _T("MOUSELEAVE")
#define  UIMSG_WINDOWSIZE   _T("WINDOWSIZE")
#define  UIMSG_MOUSEHOVER   _T("MOUSEHOVER")
#define  UIMSG_MOUSEWHEEL   _T("MOUSEWHEEL")
#define  UIMSG_CHAR         _T("CHAR")
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

   
    


    /*class YRect : public RECT
    {
    public:
        YRect();
        YRect(const RECT & src);
        YRect(int nleft, int nTop, int nRight, int nBottom);

        int                             GetWidth() const;
        int                             GetHeight() const;
        void                            Empty();
        bool                            IsEmpty() const;
        void                            Join(const RECT & rc);
        void                            ResetOffset();
        void                            Normalize();
        void                            OffSet(int cx, int cy);
        void                            Inflate(int cx,int cy);
        void                            Defalte(int cx,int cy);
        void                            Union(YRect & rc);
    };
    class YSize: public tagSIZE
    {
    public:
        YSize();
        YSize(const SIZE & src);
        YSize(const RECT rc);
        YSize(int cx, int cy);
    };*/



}
