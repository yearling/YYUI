#include "YUI.h"
#include "HorizontalLayout.h"
#include "RenderDGI.h"

namespace YUI
{


    HorizontalLayout::HorizontalLayout()
                                        :m_iSepWidth(0)
                                        ,m_uButtonState(0)
                                        ,m_bImmMode(false)
    {
        ptLastMouse.x= ptLastMouse.y=0;
        ::ZeroMemory(&m_rcNewPos,sizeof(m_rcNewPos));
    }

    LPCTSTR HorizontalLayout::GetClass() const
    {
        return _T("HorizontalLayoutUI");
    }

    std::shared_ptr<ControlUI> HorizontalLayout::GetInterface(const YString & strName)
    {
        if( strName == CTR_HORIZONTALLAYOUT) 
            return shared_from_this();
        return Container::GetInterface(strName);
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
            m_pManager->RemovePostPaint(shared_from_this());
        }

        m_bImmMode = bImmediately;
    }

    bool HorizontalLayout::IsSepImmMode() const
    {
        return m_bImmMode;
    }

    void HorizontalLayout::SetAttribute(const YString &pstrName, const YString& pstrValue)
    {
        if( pstrName == _T("sepwidth")) 
            SetSepWidth(_ttoi(pstrValue.c_str()));
        else if( pstrName == _T("sepimm")) 
            SetSepImmMode(pstrValue == _T("true"));
        else 
            Container::SetAttribute(pstrName, pstrValue);
    }

    void HorizontalLayout::DoEvent(ControlEvent& eve)
    {
        if( m_iSepWidth != 0 ) {
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
                    LONG cx = eve.m_ptMouse.x - ptLastMouse.x;
                    ptLastMouse = eve.m_ptMouse;
                    RECT rc = m_rcNewPos;
                    if( m_iSepWidth >= 0 ) {
                        if( cx > 0 && eve.m_ptMouse.x < m_rcNewPos.right - m_iSepWidth ) return;
                        if( cx < 0 && eve.m_ptMouse.x > m_rcNewPos.right ) return;
                        rc.right += cx;
                        if( rc.right - rc.left <= GetMinWidth() ) {
                            if( m_rcNewPos.right - m_rcNewPos.left <= GetMinWidth() ) return;
                            rc.right = rc.left + GetMinWidth();
                        }
                        if( rc.right - rc.left >= GetMaxWidth() ) {
                            if( m_rcNewPos.right - m_rcNewPos.left >= GetMaxWidth() ) return;
                            rc.right = rc.left + GetMaxWidth();
                        }
                    }
                    else {
                        if( cx > 0 && eve.m_ptMouse.x < m_rcNewPos.left ) return;
                        if( cx < 0 && eve.m_ptMouse.x > m_rcNewPos.left - m_iSepWidth ) return;
                        rc.left += cx;
                        if( rc.right - rc.left <= GetMinWidth() ) {
                            if( m_rcNewPos.right - m_rcNewPos.left <= GetMinWidth() ) return;
                            rc.left = rc.right - GetMinWidth();
                        }
                        if( rc.right - rc.left >= GetMaxWidth() ) {
                            if( m_rcNewPos.right - m_rcNewPos.left >= GetMaxWidth() ) return;
                            rc.left = rc.right - GetMaxWidth();
                        }
                    }

                    YRect rcInvalidate = GetThumbRect(true);
                    m_rcNewPos = rc;
                    m_cXYFixed.cx = m_rcNewPos.right - m_rcNewPos.left;

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
                    ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
                    return;
                }
            }
        }
        Container::DoEvent(eve);
    }

    void HorizontalLayout::SetPos(RECT &rc)
    {
        ControlUI::SetPos(rc);
        rc = m_rcItem;

        // Adjust for inset
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;

        if( m_SetItems.empty()) 
        {
            ProcessScrollBar(rc, 0, 0);
            return;
        }

     /*   if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();*/

        // Determine the width of elements that are sizeable
        SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
     /*   if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
            szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();*/

        int nAdjustables = 0;
        int cxFixed = 0;
        int nEstimateNum = 0;
        for( auto &pControl : m_SetItems)
        {
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) continue;
            SIZE sz = pControl->EstimateSize(szAvailable);
            if( sz.cx == 0 ) {
                nAdjustables++;
            }
            else {
                if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
                if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
            }
            cxFixed += sz.cx +  pControl->GetPadding().left + pControl->GetPadding().right;
            nEstimateNum++;
        }
        cxFixed += (nEstimateNum - 1) * m_iChildPadding;

        int cxExpand = 0;
        int cxNeeded = 0;
        if( nAdjustables > 0 ) cxExpand = max(0, (szAvailable.cx - cxFixed) / nAdjustables);
        // Position the elements
        SIZE szRemaining = szAvailable;
        int iPosX = rc.left;
       /* if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
            iPosX -= m_pHorizontalScrollBar->GetScrollPos();
        }*/
        int iAdjustable = 0;
        int cxFixedRemaining = cxFixed;
        for( auto pControl : m_SetItems)
        {
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) {
                SetFloatPos(pControl);
                continue;
            }
            RECT rcPadding = pControl->GetPadding();
            szRemaining.cx -= rcPadding.left;
            SIZE sz = pControl->EstimateSize(szRemaining);
            if( sz.cx == 0 ) {
                iAdjustable++;
                sz.cx = cxExpand;
                // Distribute remaining to last element (usually round-off left-overs)
                if( iAdjustable == nAdjustables ) {
                    sz.cx = max(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
                }
                if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
                if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
            }
            else {
                if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
                if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

                cxFixedRemaining -= sz.cx;
            }

            sz.cy = pControl->GetFixedHeight();
            if( sz.cy == 0 ) sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
            if( sz.cy < 0 ) sz.cy = 0;
            if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
            if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

            RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left + rcPadding.right, rc.top + rcPadding.top + sz.cy};
            pControl->SetPos(rcCtrl);
            iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
            cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
            szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
        }
        cxNeeded += (nEstimateNum - 1) * m_iChildPadding;
        ProcessScrollBar(rc, cxNeeded, 0);
    }

    void HorizontalLayout::DoPostPaint(HDC hDC, const RECT& rcPaint)
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode ) {
            RECT rcSeparator = GetThumbRect(true);
            RenderGDI::DrawColor(hDC, rcSeparator, 0xAA000000);
        }
    }

    RECT HorizontalLayout::GetThumbRect(bool bUseNew /*= false*/) const
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew) {
            if( m_iSepWidth >= 0 ) return YRect(m_rcNewPos.right - m_iSepWidth, m_rcNewPos.top, m_rcNewPos.right, m_rcNewPos.bottom);
            else return YRect(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.left - m_iSepWidth, m_rcNewPos.bottom);
        }
        else {
            if( m_iSepWidth >= 0 ) return YRect(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
            else return YRect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom);
        }
    }

}