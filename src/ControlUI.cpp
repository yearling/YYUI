#include "YUI.h"
#include "UIDef.h"
#include "ControlUI.h"
#include "PaintManagerUI.h"

namespace YUI
{


    YString ControlUI::GetName()
    {
        return _T("");
    }

    void ControlUI::Event(ControlEvent& eve)
    {

    }

    bool ControlUI::IsUpdateNeeded() const 
    {
        return false;
    }

    void ControlUI::SetPos(const RECT &rc)
    {

    }

    std::shared_ptr<ControlUI>& ControlUI::FindControl(funcFindControl func,UINT uFlags)
    {
         return std::shared_ptr<ControlUI>();
    }

    YUI::YString ControlUI::GetVirtualWnd() const
    {
        return _T("");
    }

    void ControlUI::DoPaint(HDC hDC, const RECT& rcPaint)
    {

    }

    void ControlUI::DoPostPaint(HDC hDC, const RECT& rcPaint)
    {

    }

    void ControlUI::NeedUpdate()
    {

    }

    YUI::YString ControlUI::GetToolTip() const
    {
        return m_sToolTip;
    }

    const RECT & ControlUI::GetPos() const
    {
        return m_rcItem;
    }

    int ControlUI::GetToolTipWidth(void)
    {
        return m_nTooltipWidth;
    }

    std::shared_ptr<PaintManagerUI> ControlUI::GetManager() const
    {
        return m_pManager;
    }

    void ControlUI::SetFocus()
    {
        m_pManager->SetFocus(this->shared_from_this());
    }

    bool ControlUI::IsVisible() const
    {
        return m_bVisible && m_bInternVisible;
    }

    bool ControlUI::IsEnable() const
    {
        return m_bEnable;
    }

}