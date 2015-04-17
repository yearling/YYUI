
#pragma once

#include "YUI.h"
#include "UIDef.h"
#include <functional>
namespace YUI
{

    class ControlUI:public std::enable_shared_from_this<ControlUI>
    {
    public:
        typedef std::function<bool(NotifyMsg &)> FuncEvent;
        typedef std::function<std::weak_ptr<ControlUI>(std::weak_ptr<ControlUI>) > funcFindControl;
    public:
        YString                         GetName();
        virtual void                    Event( ControlEvent& eve );
        virtual void                    DoPaint(HDC hDC, const RECT& rcPaint);
        virtual void                    DoPostPaint(HDC hDC, const RECT& rcPaint);
        bool                            IsUpdateNeeded() const;
        void                            SetPos(const RECT &rc);
        std::shared_ptr<ControlUI>&     FindControl(funcFindControl func,UINT uFlags);
        YString                         GetVirtualWnd() const;
        YString                         GetToolTip() const;
        void                            NeedUpdate();
        const RECT &                    GetPos() const;
        int                             GetToolTipWidth(void);
        virtual std::shared_ptr<PaintManagerUI>
                                        GetManager() const;
        void                            SetFocus();
        bool                            IsVisible() const;
        bool                            IsEnable() const;
    public:
        FuncEvent                       OnNotify;

    protected:
        YString                         m_sToolTip;
        RECT                            m_rcItem;
        int                             m_nTooltipWidth;
        std::shared_ptr<PaintManagerUI> m_pManager;
        bool                            m_bVisible;
        bool                            m_bInternVisible;
        bool                            m_bEnable;
    };
}