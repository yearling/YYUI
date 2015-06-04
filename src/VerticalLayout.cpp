#include "YUI.h"
#include "VerticalLayout.h"
using std::endl;
namespace YUI
{


    VerticalLayout::VerticalLayout()
        :m_iSepHeight(0)
        ,m_uButtonState(0)
        ,m_bImmMode(false)
    {
        ptLastMouse.x =ptLastMouse.y = 0;
        ::ZeroMemory(&m_rcNewPos,sizeof(m_rcNewPos));
        AddHandler();
    }

    LPCTSTR VerticalLayout::GetClass() const
    {
        return _T("VerticalLayoutUI");
    }

   

    void VerticalLayout::SetAttribute(const std::string &strName, const std::string& strValue)
    {
        if( strName =="sepheight" ) 
            SetSepHeight(atoi(strValue.c_str()));
        else if( strName == "sepimm" )
            SetSepImmMode(strValue == "true");
        else Container::SetAttribute(strName, strValue);
    }

    UINT VerticalLayout::GetControlFlags() const
    {
        if( IsEnabled() && m_iSepHeight != 0 ) return UIFLAG_SETCURSOR;
        else return 0;
    }

 

    void VerticalLayout::SetPos(YYRECT &rc)
    {
        ControlUI::SetPos(rc);
        //////////////////////////////////////////////////////////////////////////
        static int traceid= 0;
        //if(traceid % 3 == 0)
        Ycout<<"[Verti "<<traceid++<<"] :"<<rc.left<<"  "<<rc.top<<"  "<<rc.right<<"  "<<rc.bottom<<endl;
       
        //////////////////////////////////////////////////////////////////////////
        rc = m_rcItem;
        if(traceid%3==1)
        Ycout<<"m_rcInset "<<m_rcInset.left<<"  "<<m_rcInset.top<<"  "<<m_rcInset.right<<"  "<<m_rcInset.bottom<<endl;
        // Adjust for inset
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;
      /*  if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();*/

        if( m_ListItems.empty()) {
            ProcessScrollBar(rc, 0, 0);
            return;
        }

        // Determine the minimum size
        YYSIZE szAvailable ( rc.right - rc.left, rc.bottom - rc.top );
       /* if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
            szAvailable.width += m_pHorizontalScrollBar->GetScrollRange();*/
  
        int nAdjustables = 0;
        int cyFixed = 0;
        int nEstimateNum = 0;
        for( auto pControl : m_ListItems ) {
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) continue;
            YYSIZE sz = pControl->EstimateSize(szAvailable);
            if( sz.height == 0 ) {
                nAdjustables++;
            }
            else {
                if( sz.height < pControl->GetMinHeight() ) sz.height = pControl->GetMinHeight();
                if( sz.height > pControl->GetMaxHeight() ) sz.height = pControl->GetMaxHeight();
            }
            cyFixed += sz.height + pControl->GetPadding().top + pControl->GetPadding().bottom;
            nEstimateNum++;
        }
        cyFixed += (nEstimateNum - 1) * m_iChildPadding;
        if(traceid%3==1)
        Ycout<<"NAdjustables "<<nAdjustables<<"   cyFixed "<<cyFixed<<"    nEstimateNum "<<nEstimateNum<<endl;
        // Place elements
        int cyNeeded = 0;
        int cyExpand = 0;
        if( nAdjustables > 0 ) cyExpand = max(0, (szAvailable.height - cyFixed) / nAdjustables);
        // Position the elements
        YYSIZE szRemaining = szAvailable;
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
        for( auto pControl: m_ListItems ) 
        {
            if( !pControl->IsVisible() ) 
                continue;
            if( pControl->IsFloat() ) 
            {
                SetFloatPos(pControl);
                continue;
            }

            YYRECT rcPadding = pControl->GetPadding();
            szRemaining.height -= rcPadding.top;
                if(traceid%3==1)
            Ycout<<"szRemaining.height "<<szRemaining.height<<endl;
            YYSIZE sz = pControl->EstimateSize(szRemaining);
            if( sz.height == 0 ) {
                iAdjustable++;
                sz.height = cyExpand;
                // Distribute remaining to last element (usually round-off left-overs)
                if( iAdjustable == nAdjustables ) {
                    sz.height = max(0, szRemaining.height - rcPadding.bottom - cyFixedRemaining);
                } 
                if( sz.height < pControl->GetMinHeight() ) sz.height = pControl->GetMinHeight();
                if( sz.height > pControl->GetMaxHeight() ) sz.height = pControl->GetMaxHeight();
            }
            else {
                if( sz.height < pControl->GetMinHeight() ) sz.height = pControl->GetMinHeight();
                if( sz.height > pControl->GetMaxHeight() ) sz.height = pControl->GetMaxHeight();
                cyFixedRemaining -= sz.height;
            }

            sz.width = pControl->GetFixedWidth();
            if( sz.width == 0 ) sz.width = szAvailable.width - rcPadding.left - rcPadding.right;
            if( sz.width < 0 ) sz.width = 0;
            if( sz.width < pControl->GetMinWidth() ) sz.width = pControl->GetMinWidth();
            if( sz.width > pControl->GetMaxWidth() ) sz.width = pControl->GetMaxWidth();
                        if(traceid%3==1)
            Ycout<<"final sz.width :"<<sz.width<<"  sz.height: "<<sz.height<<endl;
            YYRECT rcCtrl(iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.width, iPosY + sz.height + rcPadding.top + rcPadding.bottom );
            pControl->SetPos(rcCtrl);

            iPosY += sz.height + m_iChildPadding + rcPadding.top + rcPadding.bottom;
            cyNeeded += sz.height + rcPadding.top + rcPadding.bottom;
            szRemaining.height -= sz.height + m_iChildPadding + rcPadding.bottom;
        }
        cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

        // Process the scrollbar
        ProcessScrollBar(rc, 0, cyNeeded);
    }

    void VerticalLayout::DoPostPaint(HDC hDC, const YYRECT& rcPaint)
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode ) {
            RECT rcSeparator = GetThumbRect(true);
            //RenderGDI::/*DrawColor*/(hDC, rcSeparator, 0xAA000000);
        }
    }

    YUI::YYRECT VerticalLayout::GetThumbRect(bool bUseNew /*= false*/) const
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew) {
            if( m_iSepHeight >= 0 ) 
                return YYRECT(m_rcNewPos.left, max(m_rcNewPos.bottom - m_iSepHeight, m_rcNewPos.top), 
                m_rcNewPos.right, m_rcNewPos.bottom);
            else 
                return YYRECT(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.right, 
                min(m_rcNewPos.top - m_iSepHeight, m_rcNewPos.bottom));
        }
        else {
            if( m_iSepHeight >= 0 ) 
                return YYRECT(m_rcItem.left, max(m_rcItem.bottom - m_iSepHeight, m_rcItem.top), m_rcItem.right, 
                m_rcItem.bottom);
            else 
                return YYRECT(m_rcItem.left, m_rcItem.top, m_rcItem.right, 
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
            //m_pManager->RemovePostPaint(shared_from_this());
        }

        m_bImmMode = bImmediately;
    }

    bool VerticalLayout::IsSepImmMode() const
    {
        return m_bImmMode;
    }

    

}