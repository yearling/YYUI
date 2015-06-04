#include "YUI.h"
#include "HorizontalLayout.h"

namespace YUI
{


    HorizontalLayout::HorizontalLayout()
                                        :m_iSepWidth(0)
                                        ,m_uButtonState(0)
                                        ,m_bImmMode(false)
    {
        ptLastMouse.x= ptLastMouse.y=0;
        ::ZeroMemory(&m_rcNewPos,sizeof(m_rcNewPos));
        AddHandler();
    }

    LPCTSTR HorizontalLayout::GetClass() const
    {
        return _T("HorizontalLayoutUI");
    }

   

    UINT HorizontalLayout::GetControlFlags() const
    {
        if( IsEnabled() && m_iSepWidth != 0 ) return UIFLAG_SETCURSOR;
        else return 0;
    }

    void HorizontalLayout::SetSepWidth(int iWidth)
    {
        m_iSepWidth = iWidth;
    }

    int HorizontalLayout::GetSepWidth() const
    {
        return m_iSepWidth;
    }

    void HorizontalLayout::SetSepImmMode(bool bImmediately)
    {
        if( m_bImmMode == bImmediately ) return;
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL ) {
            //m_pManager->RemovePostPaint(shared_from_this());
        }

        m_bImmMode = bImmediately;
    }

    bool HorizontalLayout::IsSepImmMode() const
    {
        return m_bImmMode;
    }

    void HorizontalLayout::SetAttribute(const std::string &strName, const std::string& strValue)
    {
        if( strName == ("sepwidth")) 
            SetSepWidth(atoi(strValue.c_str()));
        else if( strName == ("sepimm")) 
            SetSepImmMode(strValue == ("true"));
        else 
            Container::SetAttribute(strName, strValue);
    }

    void HorizontalLayout::SetPos(YYRECT &rc)
    {
        ControlUI::SetPos(rc);
        rc = m_rcItem;

        // Adjust for inset
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;

        if( m_ListItems.empty()) 
        {
            ProcessScrollBar(rc, 0, 0);
            return;
        }

     /*   if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();*/

        // Determine the width of elements that are sizeable
        YYSIZE szAvailable( rc.right - rc.left, rc.bottom - rc.top );
     /*   if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
            szAvailable.width += m_pHorizontalScrollBar->GetScrollRange();*/

        int nAdjustables = 0;
        float cxFixed = 0;
        int nEstimateNum = 0;
        for( auto &pControl : m_ListItems)
        {
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) continue;
            YYSIZE sz = pControl->EstimateSize(szAvailable);
            if( sz.width == 0 ) {
                nAdjustables++;
            }
            else {
                if( sz.width < pControl->GetMinWidth() ) sz.width = pControl->GetMinWidth();
                if( sz.width > pControl->GetMaxWidth() ) sz.width = pControl->GetMaxWidth();
            }
            cxFixed += sz.width +  pControl->GetPadding().left + pControl->GetPadding().right;
            nEstimateNum++;
        }
        cxFixed += (nEstimateNum - 1) * m_iChildPadding;

        int cxExpand = 0;
        int cxNeeded = 0;
        if( nAdjustables > 0 ) cxExpand = max(0, (szAvailable.width - cxFixed) / nAdjustables);
        // Position the elements
        YYSIZE szRemaining = szAvailable;
        int iPosX = rc.left;
       /* if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
            iPosX -= m_pHorizontalScrollBar->GetScrollPos();
        }*/
        int iAdjustable = 0;
        int cxFixedRemaining = cxFixed;
        for( auto pControl : m_ListItems)
        {
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) {
                SetFloatPos(pControl);
                continue;
            }
            YYRECT rcPadding = pControl->GetPadding();
            szRemaining.width -= rcPadding.left;
            YYSIZE sz = pControl->EstimateSize(szRemaining);
            if( sz.width == 0 ) {
                iAdjustable++;
                sz.width = cxExpand;
                // Distribute remaining to last element (usually round-off left-overs)
                if( iAdjustable == nAdjustables ) {
                    sz.width = max(0, szRemaining.width - rcPadding.right - cxFixedRemaining);
                }
                if( sz.width < pControl->GetMinWidth() ) sz.width = pControl->GetMinWidth();
                if( sz.width > pControl->GetMaxWidth() ) sz.width = pControl->GetMaxWidth();
            }
            else {
                if( sz.width < pControl->GetMinWidth() ) sz.width = pControl->GetMinWidth();
                if( sz.width > pControl->GetMaxWidth() ) sz.width = pControl->GetMaxWidth();

                cxFixedRemaining -= sz.width;
            }

            sz.height = pControl->GetFixedHeight();
            if( sz.height == 0 ) sz.height = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
            if( sz.height < 0 ) sz.height = 0;
            if( sz.height < pControl->GetMinHeight() ) sz.height = pControl->GetMinHeight();
            if( sz.height > pControl->GetMaxHeight() ) sz.height = pControl->GetMaxHeight();

            YYRECT rcCtrl( iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.width + rcPadding.left + rcPadding.right, rc.top + rcPadding.top + sz.height);
            pControl->SetPos(rcCtrl);
            iPosX += sz.width + m_iChildPadding + rcPadding.left + rcPadding.right;
            cxNeeded += sz.width + rcPadding.left + rcPadding.right;
            szRemaining.width -= sz.width + m_iChildPadding + rcPadding.right;
        }
        cxNeeded += (nEstimateNum - 1) * m_iChildPadding;
        ProcessScrollBar(rc, cxNeeded, 0);
    }

    void HorizontalLayout::DoPostPaint(HDC hDC, const YYRECT& rcPaint)
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode ) {
            RECT rcSeparator = GetThumbRect(true);
            //RenderGDI::DrawColor(hDC, rcSeparator, 0xAA000000);
        }
    }

    YUI::YYRECT HorizontalLayout::GetThumbRect(bool bUseNew /*= false*/) const
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew) {
            if( m_iSepWidth >= 0 ) return YYRECT(m_rcNewPos.right - m_iSepWidth, m_rcNewPos.top, m_rcNewPos.right, m_rcNewPos.bottom);
            else return YYRECT(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.left - m_iSepWidth, m_rcNewPos.bottom);
        }
        else {
            if( m_iSepWidth >= 0 ) return YYRECT(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
            else return YYRECT(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom);
        }
    }

    

}