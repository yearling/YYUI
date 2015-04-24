#include "YUI.h"
#include "TileLayout.h"



namespace YUI
{


    TileLayout::TileLayout()
        :m_nColumns(1)
    {
        m_szItem.cx = m_szItem.cy =0;
    }

    void TileLayout::SetAttribute(const YString &pstrName, const YString& pstrValue)
    {
        if( pstrName == _T("itemsize") ) {
            SIZE szItem = { 0 };
            LPTSTR pstr = NULL;
            szItem.cx = _tcstol(pstrValue.c_str(), &pstr, 10);  assert(pstr);    
            szItem.cy = _tcstol(pstr + 1, &pstr, 10);   assert(pstr);     
            SetItemSize(szItem);
        }
        else if( pstrName ==  _T("columns") ) 
            SetColumns(_ttoi(pstrValue.c_str()));
        else 
            Container::SetAttribute(pstrName, pstrValue);
    }

    LPCTSTR TileLayout::GetClass() const
    {
        return _T("TileLayoutUI");
    }

    std::shared_ptr<ControlUI> TileLayout::GetInterface(const YString & strName)
    {
        if( strName == CTR_TILELAYOUT ) 
            return this->shared_from_this();
        return Container::GetInterface(strName);
    }

    void TileLayout::SetPos(RECT rc)
    {
        ControlUI::SetPos(rc);
        rc = m_rcItem;

        // Adjust for inset
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;

        if( m_SetItems.empty()) {
            ProcessScrollBar(rc, 0, 0);
            return;
        }

     /*   if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();*/

        // Position the elements
        if( m_szItem.cx > 0 ) m_nColumns = (rc.right - rc.left) / m_szItem.cx;
        if( m_nColumns == 0 ) m_nColumns = 1;

        int cyNeeded = 0;
        int cxWidth = (rc.right - rc.left) / m_nColumns;
       /* if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
            cxWidth = (rc.right - rc.left + m_pHorizontalScrollBar->GetScrollRange() ) / m_nColumns; ;*/

        int cyHeight = 0;
        int iCount = 0;
        POINT ptTile = { rc.left, rc.top };
      /*  if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
            ptTile.y -= m_pVerticalScrollBar->GetScrollPos();
        }*/
        int iPosX = rc.left;
       /* if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
            iPosX -= m_pHorizontalScrollBar->GetScrollPos();
            ptTile.x = iPosX;
        }*/
        for( auto pControl : m_SetItems) {
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) {
                SetFloatPos(pControl);
                continue;
            }

            // Determine size
            RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y };
            if( (iCount % m_nColumns) == 0 )
            {
                int iIndex = iCount;
                for( auto pLineControl : m_SetItems ) {
                    if( !pLineControl->IsVisible() ) continue;
                    if( pLineControl->IsFloat() ) continue;

                    RECT rcPadding = pLineControl->GetPadding();
                    SIZE szAvailable = { rcTile.right - rcTile.left - rcPadding.left - rcPadding.right, 9999 };
                    if( iIndex == iCount || (iIndex + 1) % m_nColumns == 0 ) {
                        szAvailable.cx -= m_iChildPadding / 2;
                    }
                    else {
                        szAvailable.cx -= m_iChildPadding;
                    }

                    if( szAvailable.cx < pControl->GetMinWidth() ) szAvailable.cx = pControl->GetMinWidth();
                    if( szAvailable.cx > pControl->GetMaxWidth() ) szAvailable.cx = pControl->GetMaxWidth();

                    SIZE szTile = pLineControl->EstimateSize(szAvailable);
                    if( szTile.cx < pControl->GetMinWidth() ) szTile.cx = pControl->GetMinWidth();
                    if( szTile.cx > pControl->GetMaxWidth() ) szTile.cx = pControl->GetMaxWidth();
                    if( szTile.cy < pControl->GetMinHeight() ) szTile.cy = pControl->GetMinHeight();
                    if( szTile.cy > pControl->GetMaxHeight() ) szTile.cy = pControl->GetMaxHeight();

                    cyHeight = max(cyHeight, szTile.cy + rcPadding.top + rcPadding.bottom);
                    if( (++iIndex % m_nColumns) == 0) break;
                }
            }

            RECT rcPadding = pControl->GetPadding();

            rcTile.left += rcPadding.left + m_iChildPadding / 2;
            rcTile.right -= rcPadding.right + m_iChildPadding / 2;
            if( (iCount % m_nColumns) == 0 ) {
                rcTile.left -= m_iChildPadding / 2;
            }

            if( ( (iCount + 1) % m_nColumns) == 0 ) {
                rcTile.right += m_iChildPadding / 2;
            }

            // Set position
            rcTile.top = ptTile.y + rcPadding.top;
            rcTile.bottom = ptTile.y + cyHeight;

            SIZE szAvailable = { rcTile.right - rcTile.left, rcTile.bottom - rcTile.top };
            SIZE szTile = pControl->EstimateSize(szAvailable);
            if( szTile.cx == 0 ) szTile.cx = szAvailable.cx;
            if( szTile.cy == 0 ) szTile.cy = szAvailable.cy;
            if( szTile.cx < pControl->GetMinWidth() ) szTile.cx = pControl->GetMinWidth();
            if( szTile.cx > pControl->GetMaxWidth() ) szTile.cx = pControl->GetMaxWidth();
            if( szTile.cy < pControl->GetMinHeight() ) szTile.cy = pControl->GetMinHeight();
            if( szTile.cy > pControl->GetMaxHeight() ) szTile.cy = pControl->GetMaxHeight();
            RECT rcPos = {(rcTile.left + rcTile.right - szTile.cx) / 2, (rcTile.top + rcTile.bottom - szTile.cy) / 2,
                (rcTile.left + rcTile.right - szTile.cx) / 2 + szTile.cx, (rcTile.top + rcTile.bottom - szTile.cy) / 2 + szTile.cy};
            pControl->SetPos(rcPos);

            if( (++iCount % m_nColumns) == 0 ) {
                ptTile.x = iPosX;
                ptTile.y += cyHeight + m_iChildPadding;
                cyHeight = 0;
            }
            else {
                ptTile.x += cxWidth;
            }
            cyNeeded = rcTile.bottom - rc.top;
           /* if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) cyNeeded += m_pVerticalScrollBar->GetScrollPos();*/
        }

        // Process the scrollbar
        ProcessScrollBar(rc, 0, cyNeeded);
    }

    SIZE TileLayout::GetItemSize() const
    {
      return m_szItem; 
    }

    void TileLayout::SetItemSize(SIZE szItem)
    {
        if( m_szItem.cx != szItem.cx || m_szItem.cy != szItem.cy ) {
            m_szItem = szItem;
            NeedUpdate();
        }
    }

    int TileLayout::GetColumns() const
    {
        return m_nColumns;
    }

    void TileLayout::SetColumns(int nCols)
    {
        if( nCols <= 0 ) return;
        m_nColumns = nCols;
        NeedUpdate();
    }

}