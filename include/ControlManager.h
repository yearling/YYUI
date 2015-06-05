#pragma once
#include "YUI.h"
#include <commctrl.h>
namespace YUI
{

    class ControlManager:public CountRefObj
    {
    public:
        ControlManager(void);
        ~ControlManager(void);
        void                            Init(HWND hWnd) { m_hWnd = hWnd;}
        void                            Invalidate(const YYRECT &rc);
        void                            SendMsg(ControlUI* pControl,const MsgWrap &msg);
        void                            NeedUpdate() { m_bNeedUpdate = true;}
        void                            InitControls(ControlUI *pControl,ControlUI* parent);
        void                            AttachDialog(ControlUI* pControl);
    public:
        ControlUI*                      GetFocus() const;
        ControlUI*                      GetRoot() const ;
        void                            SetFocus(ControlUI *pControl);
        bool                            MessageHandler(UINT uMesg, WPARAM wParam, LPARAM lParam, LRESULT &lRes);
        ControlUI*                      FindControl(const YYPOINT& pt);
		ControlUI*						FindControl(const YString& strControlName);
        void                            SetCapture();
        void                            ReleaseCapture();
        HWND                            GetHWND() const { return m_hWnd;}
        const WindowProperty*           GetWindowProperty() const;
    private:    
        HWND                            m_hWnd;
        ControlUI*                      m_pFocus;
        CountRefPtr<ControlUI>          m_pRoot;
        ControlUI*                      m_pHover;
        ControlUI*                      m_pClick;
        YYPOINT                         m_ptLastMousePos;
        bool                            m_bNeedUpdate;
        bool                            m_bMouseTracking;
        TOOLINFO                        m_ToolTip;
        bool                            m_bMouseCapture;
        HWND                            m_hwndTooltip;
    };


}