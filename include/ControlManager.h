#pragma once
#include "YUI.h"
#include <commctrl.h>
namespace YUI
{

    class ControlManager:public std::enable_shared_from_this<ControlManager>
    {
    public:
        ControlManager(void);
        ~ControlManager(void);
        void                            Init(HWND hWnd) { m_hWnd = hWnd;}
        void                            Invalidate(const YYRECT &rc);
        void                            SendMsg(SPControlUI pControl,const MsgWrap &msg);
        void                            NeedUpdate() { m_bNeedUpdate = true;}
        void                            InitControls(std::shared_ptr<ControlUI> &pControl,std::weak_ptr<ControlUI> parent);
        void                            AttachDialog(std::shared_ptr<ControlUI> pControl);
    public:
        SPControlUI                     GetFocus() const;
        SPControlUI                     GetRoot() const ;
        void                            SetFocus(SPControlUI &pControl);
        bool                            MessageHandler(UINT uMesg, WPARAM wParam, LPARAM lParam, LRESULT &lRes);
        SPControlUI                     FindControl(const YYPOINT& pt);
        void                            SetCapture();
        void                            ReleaseCapture();
        HWND                            GetHWND() const { return m_hWnd;}
        const WindowProperty*           GetWindowProperty() const;
    private:    
        HWND                            m_hWnd;
        SPControlUI                     m_pFocus;
        SPControlUI                     m_pRoot;
        SPControlUI                     m_pHover;
        SPControlUI                     m_pClick;
        YYPOINT                         m_ptLastMousePos;
        bool                            m_bNeedUpdate;
        bool                            m_bMouseTracking;
        TOOLINFO                        m_ToolTip;
        bool                            m_bMouseCapture;
    };


}