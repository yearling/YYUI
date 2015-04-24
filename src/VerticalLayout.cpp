#include "YUI.h"
#include "VerticalLayout.h"
#include "RenderDGI.h"

namespace YUI
{


    VerticalLayout::VerticalLayout()
        :m_iSepHeight(0)
        ,m_uButtonState(0)
        ,m_bImmMode(false)
    {
        ptLastMouse.x =ptLastMouse.y = 0;
        ::ZeroMemory(&m_rcNewPos,sizeof(m_rcNewPos));
    }

    LPCTSTR VerticalLayout::GetClass() const
    {
        return _T("VerticalLayoutUI");
    }

    std::shared_ptr<ControlUI> VerticalLayout::GetInterface(const YString & strName)
    {
        if( strName == CTR_VERTICALLAYOUT)
            return shared_from_this();
        return Container::GetInterface(strName);
    }

    void VerticalLayout::SetAttribute(const YString &pstrName, const YString& pstrValue)
    {
        if( pstrName == _T("sepheight") ) 
            SetSepHeight(_ttoi(pstrValue.c_str()));
        else if( pstrName == _T("sepimm") )
            SetSepImmMode(pstrValue == _T("true"));
        else Container::SetAttribute(pstrName, pstrValue);
    }

    UINT VerticalLayout::GetControlFlags() const
    {
        if( IsEnabled() && m_iSepHeight != 0 ) return UIFLAG_SETCURSOR;
        else return 0;
    }

    void VerticalLayout::DoEvent(ControlEvent& eve)
    {
        if( m_iSepHeight != 0 ) {
            if( eve.m_Type == UIEVENT_BUTTONDOWN && IsEnabled() )
            {
                RECT rcSeparator = GetThumbRect(false);
                if( ::PtInRect(&rcSeparator, eve.m_ptMouse) ) {
                    m_uButtonState |= UISTATE_CAPTURED;
                    ptLastMouse = eve.m_ptMouse;
                    m_rcNewPos = m_rcItem;
                    if( !m_bImmMode && m_pManager ) m_pManager->AddPostPaint(shared_from_this());
                    return;
                }
            }
            if( eve.m_Type == UIEVENT_BUTTONUP )
            {
                if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
                    m_uButtonState &= ~UISTATE_CAPTURED;
                    m_rcItem = m_rcNewPos;
                    if( !m_bImmMode && m_pManager ) m_pManager->RemovePostPaint(shared_from_this());
                    NeedParentUpdate();
                    return;
                }
            }
            if( eve.m_Type == UIEVENT_MOUSEMOVE )
            {
                if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
                    LONG cy = eve.m_ptMouse.y - ptLastMouse.y;
                    ptLastMouse = eve.m_ptMouse;
                    RECT rc = m_rcNewPos;
                    if( m_iSepHeight >= 0 ) {
                        if( cy > 0 && eve.m_ptMouse.y < m_rcNewPos.bottom + m_iSepHeight ) return;
                        if( cy < 0 && eve.m_ptMouse.y > m_rcNewPos.bottom ) return;
                        rc.bottom += cy;
                        if( rc.bottom - rc.top <= GetMinHeight() ) {
                            if( m_rcNewPos.bottom - m_rcNewPos.top <= GetMinHeight() ) return;
                            rc.bottom = rc.top + GetMinHeight();
                        }
                        if( rc.bottom - rc.top >= GetMaxHeight() ) {
                            if( m_rcNewPos.bottom - m_rcNewPos.top >= GetMaxHeight() ) return;
                            rc.bottom = rc.top + GetMaxHeight();
                        }
                    }
                    else {
                        if( cy > 0 && eve.m_ptMouse.y < m_rcNewPos.top ) return;
                        if( cy < 0 && eve.m_ptMouse.y > m_rcNewPos.top + m_iSepHeight ) return;
                        rc.top += cy;
                        if( rc.bottom - rc.top <= GetMinHeight() ) {
                            if( m_rcNewPos.bottom - m_rcNewPos.top <= GetMinHeight() ) return;
                            rc.top = rc.bottom - GetMinHeight();
                        }
                        if( rc.bottom - rc.top >= GetMaxHeight() ) {
                            if( m_rcNewPos.bottom - m_rcNewPos.top >= GetMaxHeight() ) return;
                            rc.top = rc.bottom - GetMaxHeight();
                        }
                    }

                    YRect rcInvalidate = GetThumbRect(true);
                    m_rcNewPos = rc;
                    m_cXYFixed.cy = m_rcNewPos.bottom - m_rcNewPos.top;

                    if( m_bImmMode ) {
                        m_rcItem = m_rcNewPos;
                        NeedParentUpdate();
                    }
                    else {
                        rcInvalidate.Join(GetThumbRect(true));
                        rcInvalidate.Join(GetThumbRect(false));
                        if( m_pManager ) m_pManager->Invalidate(rcInvalidate);
                    }
                    return;
                }
            }
            if( eve.m_Type == UIEVENT_SETCURSOR )
            {
                RECT rcSeparator = GetThumbRect(false);
                if( IsEnabled() && ::PtInRect(&rcSeparator, eve.m_ptMouse) ) {
                    ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
                    return;
                }
            }
        }
        Container::DoEvent(eve);
    }

    void VerticalLayout::SetPos(RECT &rc)
    {
        ControlUI::SetPos(rc);
        rc = m_rcItem;

        // Adjust for inset
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;
      /*  if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();*/

        if( m_SetItems.empty()) {
            ProcessScrollBar(rc, 0, 0);
            return;
        }

        // Determine the minimum size
        SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
       /* if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
            szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();*/

        int nAdjustables = 0;
        int cyFixed = 0;
        int nEstimateNum = 0;
        for( auto pControl : m_SetItems ) {
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) continue;
            SIZE sz = pControl->EstimateSize(szAvailable);
            if( sz.cy == 0 ) {
                nAdjustables++;
            }
            else {
                if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
                if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
            }
            cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;
            nEstimateNum++;
        }
        cyFixed += (nEstimateNum - 1) * m_iChildPadding;

        // Place elements
        int cyNeeded = 0;
        int cyExpand = 0;
        if( nAdjustables > 0 ) cyExpand = max(0, (szAvailable.cy - cyFixed) / nAdjustables);
        // Position the elements
        SIZE szRemaining = szAvailable;
        int iPosY = rc.top;
      /*  if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
            iPosY -= m_pVerticalScrollBar->GetScrollPos();
        }*/
        int iPosX = rc.left;
        /*
        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
            iPosX -= m_pHorizontalScrollBar->GetScrollPos();
        }*/
        int iAdjustable = 0;
        int cyFixedRemaining = cyFixed;
        for( auto pControl: m_SetItems ) {
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) {
                SetFloatPos(pControl);
                continue;
            }

            RECT rcPadding = pControl->GetPadding();
            szRemaining.cy -= rcPadding.top;
            SIZE sz = pControl->EstimateSize(szRemaining);
            if( sz.cy == 0 ) {
                iAdjustable++;
                sz.cy = cyExpand;
                // Distribute remaining to last element (usually round-off left-overs)
                if( iAdjustable == nAdjustables ) {
                    sz.cy = max(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
                } 
                if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
                if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
            }
            else {
                if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
                if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
                cyFixedRemaining -= sz.cy;
            }

            sz.cx = pControl->GetFixedWidth();
            if( sz.cx == 0 ) sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
            if( sz.cx < 0 ) sz.cx = 0;
            if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
            if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

            RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top + rcPadding.bottom };
            pControl->SetPos(rcCtrl);

            iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
            cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
            szRemaining.cy -= sz.cy + m_iChildPadding + rcPadding.bottom;
        }
        cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

        // Process the scrollbar
        ProcessScrollBar(rc, 0, cyNeeded);
    }

    void VerticalLayout::DoPostPaint(HDC hDC, const RECT& rcPaint)
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode ) {
            RECT rcSeparator = GetThumbRect(true);
            RenderGDI::DrawColor(hDC, rcSeparator, 0xAA000000);
        }
    }

    RECT VerticalLayout::GetThumbRect(bool bUseNew /*= false*/) const
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew) {
            if( m_iSepHeight >= 0 ) 
                return YRect(m_rcNewPos.left, max(m_rcNewPos.bottom - m_iSepHeight, m_rcNewPos.top), 
                m_rcNewPos.right, m_rcNewPos.bottom);
            else 
                return YRect(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.right, 
                min(m_rcNewPos.top - m_iSepHeight, m_rcNewPos.bottom));
        }
        else {
            if( m_iSepHeight >= 0 ) 
                return YRect(m_rcItem.left, max(m_rcItem.bottom - m_iSepHeight, m_rcItem.top), m_rcItem.right, 
                m_rcItem.bottom);
            else 
                return YRect(m_rcItem.left, m_rcItem.top, m_rcItem.right, 
                min(m_rcItem.top - m_iSepHeight, m_rcItem.bottom));
        }
    }

    void VerticalLayout::SetSepHeight(int iHeight)
    {
        m_iSepHeight = iHeight;
    }

    int VerticalLayout::GetSepHeight() const
    {
        return m_iSepHeight;
    }

    void VerticalLayout::SetSepImmMode(bool bImmediately)
    {
        if( m_bImmMode == bImmediately ) return;
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL ) {
            m_pManager->RemovePostPaint(shared_from_this());
        }

        m_bImmMode = bImmediately;
    }

    bool VerticalLayout::IsSepImmMode() const
    {
        return m_bImmMode;
    }

}