#include "YUI.h"
#include "TabLayout.h"
#include "ControlUI.h"
#include "PaintManagerUI.h"

namespace YUI
{


    TabLayout::TabLayout()
    {

    }

    void TabLayout::SetAttribute(const std::string &strName, const std::string& strValue)
    {
        //bugs to do 
       /* if( pstrName== _T("selectedid")) 
            SelectItem(_ttoi(pstrValue.c_str()));*/
        return Container::SetAttribute(strName, strValue);
    }

    LPCTSTR TabLayout::GetClass() const
    {
        return _T("TabLayoutUI");
    }

    std::shared_ptr<ControlUI> TabLayout::QueryInterface(const std::string & strName)
    {
        if( strName == CTR_TABLAYOUT ) 
            return this->shared_from_this();
        return Container::QueryInterface(strName);
    }

    bool TabLayout::Add(std::shared_ptr<ControlUI>& pControl)
    {
        Container::Add(pControl);
        if(!m_iCurSel && pControl->IsVisible())
        {
            m_iCurSel= pControl;
        }
        else
        {
            pControl->SetVisible(false);
        }
        return true;
    }

    bool TabLayout::Remove(std::shared_ptr<ControlUI> &pControl)
    {
        if(!pControl)
            return true;
        Container::Remove(pControl);
        if(m_iCurSel == pControl )
        {
            m_iCurSel = nullptr;
            if(!m_SetItems.empty())
                (*m_SetItems.begin())->SetVisible(true);
            NeedParentUpdate();
        }
        else
            m_iCurSel = nullptr;
    }

    void TabLayout::RemoveAll()
    {
        m_iCurSel = nullptr;
        Container::RemoveAll();
        NeedParentUpdate();
    }

    std::shared_ptr<ControlUI> TabLayout::GetCurSel() const
    {
        return m_iCurSel;
    }

    bool TabLayout::SelectItem(std::shared_ptr<ControlUI> pControl)
    {
        if( !pControl)
            return false;
        if( pControl == m_iCurSel )
            return true;

        auto soOld = m_iCurSel;
        m_iCurSel= pControl;
        for( auto iter : m_SetItems )
        {
            if(iter == m_iCurSel)
            {
                iter->SetVisible(true);
                iter->SetFocus();
                SetPos(m_rcItem);
            }
            else
                iter->SetVisible(false);
        }
        NeedParentUpdate();
        if( m_pManager )
        {
            m_pManager->SetNextTabControl();
            //通知消息有问题~~
            //m_pManager->SendNotify(this, DUI_MSGTYPE_TABSELECT, m_iCurSel, iOldSel);
            m_pManager->SendNotify(shared_from_this(),MSG_TABSELECT,0,1);
        }
        return true;
    }

    void TabLayout::SetPos(RECT rc)
    {
        ControlUI::SetPos(rc);
        rc = m_rcItem;

        // Adjust for inset
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;

        for(auto pControl : m_SetItems ) {
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) {
                SetFloatPos(pControl);
                continue;
            }

            if( pControl != m_iCurSel ) continue;

            RECT rcPadding = pControl->GetPadding();
            rc.left += rcPadding.left;
            rc.top += rcPadding.top;
            rc.right -= rcPadding.right;
            rc.bottom -= rcPadding.bottom;

            SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

            SIZE sz = pControl->EstimateSize(szAvailable);
            if( sz.cx == 0 ) {
                sz.cx = max(0, szAvailable.cx);
            }
            if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
            if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

            if(sz.cy == 0) {
                sz.cy = max(0, szAvailable.cy);
            }
            if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
            if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

            RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy};
            pControl->SetPos(rcCtrl);
        }
    }

}