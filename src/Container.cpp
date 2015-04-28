#include "YUI.h"
#include "Container.h"
#include "ControlUI.h"
#include "PaintManagerUI.h"
#include "RenderDGI.h"

namespace YUI
{


    Container::Container():
                         m_iChildPadding(0)
                        ,m_bAutoDestroy(true)
                        ,m_bDelayedDestroy(true)
                        ,m_bMouseChildEnabled(true)
                        ,m_bScrollProcess(false)

    {
        ::ZeroMemory(&m_rcInset,sizeof(m_rcInset));
    }

    Container::~Container()
    {

    }

    LPCTSTR Container::GetClass() const
    {
        return _T("Container");
    }

    std::shared_ptr<ControlUI> Container::QueryInterface(const YString & strName)
    {
        if( strName == CTR_CONTAINER ) 
			return this->shared_from_this();
        return ControlUI::QueryInterface(strName);
    }

    

    int Container::GetCount() const
    {
        return m_SetItems.size();
    }

    bool Container::Add(std::shared_ptr<ControlUI>& pControl)
    {
        if(pControl == NULL )
            return false;
        if( m_pManager )
            m_pManager->InitControls(pControl,this->shared_from_this());
        if( IsVisible() )
            NeedUpdate();
        else
            pControl->SetInternVisible(false);
        m_SetItems.insert(pControl);
        return true;
    }



    bool Container::Remove(std::shared_ptr<ControlUI> &pControl)
    {
        auto iter = m_SetItems.find(pControl);
        if( iter!= m_SetItems.end() )
        {
            NeedUpdate();
            m_SetItems.erase(iter);
        }
        return false;
    }

  

    void Container::RemoveAll()
    {
        m_SetItems.clear();
        NeedUpdate();
    }

    void Container::DoEvent(ControlEvent& eve)
    {
        if( !IsMouseEnabled() && eve.m_Type > UIEVENT__MOUSEBEGIN && eve.m_Type < UIEVENT__MOUSEEND ) {
            auto sp = m_pParent.lock();
            if(sp)
                sp->DoEvent(eve);
            else
                ControlUI::DoEvent(eve);
            return;
        }

        if( eve.m_Type == UIEVENT_SETFOCUS ) 
        {
            m_bFocused = true;
            return;
        }
        if( eve.m_Type == UIEVENT_KILLFOCUS ) 
        {
            m_bFocused = false;
            return;
        }
        if( m_pVerticalScrollBar != NULL /*&& m_pVerticalScrollBar->IsVisible() && m_pVerticalScrollBar->IsEnabled()*/ )
        {
            if( eve.m_Type == UIEVENT_KEYDOWN ) 
            {
                switch( eve.m_chKey ) {
                case VK_DOWN:
                    LineDown();
                    return;
                case VK_UP:
                    LineUp();
                    return;
                case VK_NEXT:
                    PageDown();
                    return;
                case VK_PRIOR:
                    PageUp();
                    return;
                case VK_HOME:
                    HomeUp();
                    return;
                case VK_END:
                    EndDown();
                    return;
                }
            }
            else if( eve.m_Type == UIEVENT_SCROLLWHEEL )
            {
                switch( LOWORD(eve.m_wParam) ) {
                case SB_LINEUP:
                    LineUp();
                    return;
                case SB_LINEDOWN:
                    LineDown();
                    return;
                }
            }
        }
        else if( m_pHorizontalScrollBar != NULL /*&& m_pHorizontalScrollBar->IsVisible() && m_pHorizontalScrollBar->IsEnabled()*/ ) {
            if( eve.m_Type == UIEVENT_KEYDOWN ) 
            {
                switch( eve.m_chKey ) {
                case VK_DOWN:
                    LineRight();
                    return;
                case VK_UP:
                    LineLeft();
                    return;
                case VK_NEXT:
                    PageRight();
                    return;
                case VK_PRIOR:
                    PageLeft();
                    return;
                case VK_HOME:
                    HomeLeft();
                    return;
                case VK_END:
                    EndRight();
                    return;
                }
            }
            else if( eve.m_Type == UIEVENT_SCROLLWHEEL )
            {
                switch( LOWORD(eve.m_wParam) ) {
                case SB_LINEUP:
                    LineLeft();
                    return;
                case SB_LINEDOWN:
                    LineRight();
                    return;
                }
            }
        }
        ControlUI::DoEvent(eve);
    }

    void Container::SetVisible(bool bVisible /*= true*/)
    {
        if( m_bVisible == bVisible )
            return;
        ControlUI::SetVisible(true);
        for(auto iter: m_SetItems)
            iter->SetInternVisible(IsVisible());
    }

    // 逻辑上，对于Container控件不公开此方法
    // 调用此方法的结果是，内部子控件隐藏，控件本身依然显示，背景等效果存在
    void Container::SetInternVisible(bool bVisible /*= true*/)
    {
        ControlUI::SetInternVisible(bVisible);
        // 控制子控件显示状态
        // InternVisible状态应由子控件自己控制
        for(auto iter: m_SetItems)
            iter->SetInternVisible(IsVisible()); 
    }

    void Container::SetMouseEnabled(bool bEnable /*= true*/)
    {
       //todo 
      /*  if( m_pVerticalScrollBar )
            m_pVerticalScrollBar-*/
        ControlUI::SetMouseEnabled(bEnable);
    }

    RECT Container::GetInset() const
    {
        return m_rcInset;
    }

    void Container::SetInset(RECT rcInset)
    {
        m_rcInset = rcInset;
        NeedUpdate();
    }

    int Container::GetChildPadding() const
    {
        return m_iChildPadding;
    }

    void Container::SetChildPadding(int iPadding)
    {
        m_iChildPadding = iPadding;
        NeedUpdate();
    }

    bool Container::IsAutoDestroy() const
    {
        return m_bAutoDestroy;
    }

    void Container::SetAutoDestroy(bool bAuto)
    {
        m_bAutoDestroy = bAuto;
    }

    bool Container::IsDelayedDestroy() const
    {
        return m_bDelayedDestroy;
    }

    void Container::SetDelayedDestroy(bool bDelayed)
    {
        m_bDelayedDestroy = bDelayed;
    }

    bool Container::IsMouseChildEnabled() const
    {
        return m_bMouseChildEnabled;
    }

    void Container::SetMouseChildEnabled(bool bEnable /*= true*/)
    {
        m_bMouseChildEnabled = bEnable;
    }

    int Container::FindSelectable(int iIndex, bool bForward /*= true*/) const
    {
        //todo 
        return -1;
    }

    void Container::SetPos(RECT &rc)
    {
        ControlUI::SetPos(rc);
        if( m_SetItems.empty() ) return;
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;

        for(auto pControl : m_SetItems)
        {
            if( !pControl->IsVisible() ) continue;
            if( pControl->IsFloat() ) {
                SetFloatPos(pControl);
            }
            else {
                pControl->SetPos(rc); // 所有非float子控件放大到整个客户区
            }
        }
    }

    void Container::DoPaint(HDC hDC, const RECT& rcPaint)
    {
        RECT rcTemp = { 0 };
        if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;

        RenderClip clip;
        RenderClip::GenerateClip(hDC, rcTemp, clip);
        ControlUI::DoPaint(hDC, rcPaint);

        if( !m_SetItems.empty() ) 
        {
            RECT rc = m_rcItem;
            rc.left += m_rcInset.left;
            rc.top += m_rcInset.top;
            rc.right -= m_rcInset.right;
            rc.bottom -= m_rcInset.bottom;
            /*  if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
            if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();*/

            if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) 
            {
                for(auto& pControl : m_SetItems)
                {
                    if( !pControl->IsVisible() ) continue;
                    if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                    if( pControl ->IsFloat() )
                    {
                        if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                        pControl->DoPaint(hDC, rcPaint);
                    }
                }
            }
            else {
                RenderClip childClip;
                RenderClip::GenerateClip(hDC, rcTemp, childClip);
                for(auto& pControl : m_SetItems)
                {
                    if( !pControl->IsVisible() ) continue;
                    if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
                    if( pControl ->IsFloat() ) {
                        if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
                        RenderClip::UseOldClipBegin(hDC, childClip);
                        pControl->DoPaint(hDC, rcPaint);
                        RenderClip::UseOldClipEnd(hDC, childClip);
                    }
                    else {
                        if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
                        pControl->DoPaint(hDC, rcPaint);
                    }
                }
            }
        }

       /* if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
            if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
                m_pVerticalScrollBar->DoPaint(hDC, rcPaint);
            }
        }

        if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
            if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
                m_pHorizontalScrollBar->DoPaint(hDC, rcPaint);
            }
        }*/
    }

    void Container::SetAttribute(const YString &strName, const YString& strValue)
    {
        LPCTSTR pstrName = strName.c_str();
        LPCTSTR pstrValue = strValue.c_str();
        if( _tcscmp(pstrName, _T("inset")) == 0 ) {
            RECT rcInset = { 0 };
            LPTSTR pstr = NULL;
            rcInset.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
            rcInset.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
            rcInset.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
            rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
            SetInset(rcInset);
        }
        else if( _tcscmp(pstrName, _T("mousechild")) == 0 ) SetMouseChildEnabled(_tcscmp(pstrValue, _T("true")) == 0);
        else if( _tcscmp(pstrName, _T("vscrollbar")) == 0 ) {
            EnableScrollBar(_tcscmp(pstrValue, _T("true")) == 0, GetHorizontalScrollBar() != NULL);
        }
      /*  else if( _tcscmp(pstrName, _T("vscrollbarstyle")) == 0 ) {
            EnableScrollBar(true, GetHorizontalScrollBar() != NULL);
            if( GetVerticalScrollBar() ) GetVerticalScrollBar()->ApplyAttributeList(pstrValue);
        }
        else if( _tcscmp(pstrName, _T("hscrollbar")) == 0 ) {
            EnableScrollBar(GetVerticalScrollBar() != NULL, _tcscmp(pstrValue, _T("true")) == 0);
        }
        else if( _tcscmp(pstrName, _T("hscrollbarstyle")) == 0 ) {
            EnableScrollBar(GetVerticalScrollBar() != NULL, true);
            if( GetHorizontalScrollBar() ) GetHorizontalScrollBar()->ApplyAttributeList(pstrValue);
        }*/
        else if( _tcscmp(pstrName, _T("childpadding")) == 0 ) SetChildPadding(_ttoi(pstrValue));
        else ControlUI::SetAttribute(pstrName, pstrValue);
    }

    void Container::SetManager(std::shared_ptr<PaintManagerUI> &pManager, std::weak_ptr<ControlUI> pParent, bool bInit/*=true*/)
    {
        for( auto &pControl : m_SetItems ) {
            pControl->SetManager(pManager, shared_from_this(), bInit);
        }

       /* if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->SetManager(pManager, this, bInit);
        if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->SetManager(pManager, this, bInit);*/
        ControlUI::SetManager(pManager, pParent, bInit);
    }

    bool Container::SetSubControlText(const YString & pstrSubControlName,const YString& pstrText)
    {
        auto &pSubControl=this->FindSubControl(pstrSubControlName);
        if (pSubControl!=NULL)
        {
            pSubControl->SetText(pstrText);
            return TRUE;
        }
        else
            return FALSE;
    }

    bool Container::SetSubControlFixedHeight(const YString & pstrSubControlName,int cy)
    {
           auto &pSubControl=this->FindSubControl(pstrSubControlName);
           if (pSubControl!=NULL)
           {
               pSubControl->SetFixedHeight(cy);
               return TRUE;
           }
           else
               return FALSE;
    }

    bool Container::SetSubControlFixedWdith(const YString& pstrSubControlName,int cx)
    {
           auto &pSubControl=this->FindSubControl(pstrSubControlName);
           if (pSubControl!=NULL)
           {
               pSubControl->SetFixedWidth(cx);
               return TRUE;
           }
           else
               return FALSE;
    }

    bool Container::SetSubControlUserData(const YString& pstrSubControlName,const YString& pstrText)
    {

           auto &pSubControl=this->FindSubControl(pstrSubControlName);
           pSubControl=this->FindSubControl(pstrSubControlName);
           if (pSubControl!=NULL)
           {
               pSubControl->SetUserData(pstrText);
               return TRUE;
           }
           else
               return FALSE;
    }

    YUI::YString Container::GetSubControlText(const YString& pstrSubControlName)
    {
         auto &pSubControl=this->FindSubControl(pstrSubControlName);
         if (pSubControl==NULL)
             return _T("");
         else
             return pSubControl->GetText();
    }

    int Container::GetSubControlFixedHeight(const YString& pstrSubControlName)
    {
         auto &pSubControl=this->FindSubControl(pstrSubControlName);
         if (pSubControl==NULL)
             return -1;
         else
             return pSubControl->GetFixedHeight();
    }

    int Container::GetSubControlFixedWdith(const YString& pstrSubControlName)
    {
        auto &pSubControl=this->FindSubControl(pstrSubControlName);
        if (pSubControl==NULL)
            return -1;
        else
            return pSubControl->GetFixedWidth();
    }

    YUI::YString Container::GetSubControlUserData(LPCTSTR pstrSubControlName)
    {
        auto &pSubControl=this->FindSubControl(pstrSubControlName);
        if (pSubControl==NULL)
            return _T("");
        else
            return pSubControl->GetUserData();
    }

    std::shared_ptr<ControlUI> Container::FindSubControl(const YString& pstrSubControlName)
    {
        auto spRoot = m_pManager->GetRoot();
        return spRoot->FindControlFromName(pstrSubControlName);
    }

    SIZE Container::GetScrollPos() const
    {
        SIZE sz = {0, 0};
        return sz;
    }

    SIZE Container::GetScrollRange() const
    {
        SIZE sz = {0, 0};
        return sz;
    }

    void Container::SetScrollPos(SIZE szPos)
    {
       
    }

    void Container::LineUp()
    {
        int cyLine = 8;
        if( m_pManager ) cyLine = m_pManager->GetDefaultFontInfo()->m_tm.tmHeight + 8;

        SIZE sz = GetScrollPos();
        sz.cy -= cyLine;
        SetScrollPos(sz);
    }

    void Container::LineDown()
    {
        int cyLine = 8;
        if( m_pManager ) cyLine = m_pManager->GetDefaultFontInfo()->m_tm.tmHeight + 8;

        SIZE sz = GetScrollPos();
        sz.cy += cyLine;
        SetScrollPos(sz);
    }

    void Container::PageUp()
    {
        SIZE sz = GetScrollPos();
        int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
        /*if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();*/
        sz.cy -= iOffset;
        SetScrollPos(sz);
    }

    void Container::PageDown()
    {
        SIZE sz = GetScrollPos();
        int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
      /*  if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();*/
        sz.cy += iOffset;
        SetScrollPos(sz);
    }

    void Container::HomeUp()
    {
        SIZE sz = GetScrollPos();
        sz.cy = 0;
        SetScrollPos(sz);
    }

    void Container::EndDown()
    {
        SIZE sz = GetScrollPos();
        sz.cy = GetScrollRange().cy;
        SetScrollPos(sz);
    }

    void Container::LineLeft()
    {
        SIZE sz = GetScrollPos();
        sz.cx -= 8;
        SetScrollPos(sz);
    }

    void Container::LineRight()
    {
        SIZE sz = GetScrollPos();
        sz.cx += 8;
        SetScrollPos(sz);
    }

    void Container::PageLeft()
    {
        SIZE sz = GetScrollPos();
        int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
        /*if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) iOffset -= m_pVerticalScrollBar->GetFixedWidth();*/
        sz.cx -= iOffset;
        SetScrollPos(sz);
    }

    void Container::PageRight()
    {
        SIZE sz = GetScrollPos();
        int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
        /*if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) iOffset -= m_pVerticalScrollBar->GetFixedWidth();*/
        sz.cx += iOffset;
        SetScrollPos(sz);
    }

    void Container::HomeLeft()
    {
        SIZE sz = GetScrollPos();
        sz.cx = 0;
        SetScrollPos(sz);
    }

    void Container::EndRight()
    {
        SIZE sz = GetScrollPos();
        sz.cx = GetScrollRange().cx;
        SetScrollPos(sz);
    }

    void Container::EnableScrollBar(bool bEnableVertical /*= true*/, bool bEnableHorizontal /*= false*/)
    {

    }

    std::shared_ptr<Scrollbar> Container::GetVerticalScrollBar() const
    {
        return m_pVerticalScrollBar;
    }

    std::shared_ptr<Scrollbar> Container::GetHorizontalScrollBar() const
    {
        return m_pHorizontalScrollBar;
    }

    void Container::SetFloatPos(std::shared_ptr<ControlUI> &pControl)
    {
        // 因为CControlUI::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
        if( !pControl || m_SetItems.empty() ) return;

        if( !pControl->IsVisible() ) return;
        if( !pControl->IsFloat() ) return;

        SIZE szXY = pControl->GetFixedXY();
        SIZE sz = {pControl->GetFixedWidth(), pControl->GetFixedHeight()};
        RECT rcCtrl = { 0 };
        if( szXY.cx >= 0 ) {
            rcCtrl.left = m_rcItem.left + szXY.cx;
            rcCtrl.right = m_rcItem.left + szXY.cx + sz.cx;
        }
        else {
            rcCtrl.left = m_rcItem.right + szXY.cx - sz.cx;
            rcCtrl.right = m_rcItem.right + szXY.cx;
        }
        if( szXY.cy >= 0 ) {
            rcCtrl.top = m_rcItem.top + szXY.cy;
            rcCtrl.bottom = m_rcItem.top + szXY.cy + sz.cy;
        }
        else {
            rcCtrl.top = m_rcItem.bottom + szXY.cy - sz.cy;
            rcCtrl.bottom = m_rcItem.bottom + szXY.cy;
        }
        if( pControl->IsRelativePos() )
        {
            RelativePosUI tRelativePos = pControl->GetRelativePos();
            SIZE szParent = {m_rcItem.right-m_rcItem.left,m_rcItem.bottom-m_rcItem.top};
            if(tRelativePos.m_szParent.cx != 0)
            {
                int nIncrementX = szParent.cx-tRelativePos.m_szParent.cx;
                int nIncrementY = szParent.cy-tRelativePos.m_szParent.cy;
                rcCtrl.left += (nIncrementX*tRelativePos.m_nMoveXPercent/100);
                rcCtrl.top += (nIncrementY*tRelativePos.m_nMoveYPercent/100);
                rcCtrl.right = rcCtrl.left+sz.cx+(nIncrementX*tRelativePos.m_nZoomXPercent/100);
                rcCtrl.bottom = rcCtrl.top+sz.cy+(nIncrementY*tRelativePos.m_nZoomYPercent/100);
            }
            pControl->SetRelativeParentSize(szParent);
        }
        pControl->SetPos(rcCtrl);
    }

    void Container::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
    {

    }

    std::shared_ptr<ControlUI> Container::FindControlFromPoint(POINT pt,UINT flag)
    {  
        std::shared_ptr<ControlUI> pResult ;
        if( (flag & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
        if( (flag & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
        if( (flag & UIFIND_HITTEST) != 0 ) {
            if( !::PtInRect(&m_rcItem, pt) ) return NULL;
            if( !m_bMouseChildEnabled ) {
             /*   if( m_pVerticalScrollBar != NULL ) pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
                if( pResult == NULL && m_pHorizontalScrollBar != NULL ) pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags);*/
                if( pResult == NULL ) pResult = ControlUI::FindControlFromPoint(pt,flag);
                return pResult;
            }
        }
      
        /*if( m_pVerticalScrollBar != NULL ) pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
        if( pResult == NULL && m_pHorizontalScrollBar != NULL ) pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags);*/
        if( pResult != NULL ) return pResult;

        if( (flag & UIFIND_ME_FIRST) != 0 ) 
        {
            pResult = ControlUI::FindControlFromPoint(pt,flag);
            if( pResult != NULL ) return pResult;
        }
        RECT rc = m_rcItem;
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;
       /* if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
        if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();*/
        //本身的m_items自带排序功能，按插入位置
        if( (flag & UIFIND_TOP_FIRST) != 0 ) 
        {
           /* for( int it = m_items.GetSize() - 1; it >= 0; it-- ) {
                CControlUI* pControl = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
                if( pControl != NULL ) {
                    if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
                        continue;
                    else 
                        return pControl;
                }            
            }*/
            for(auto iter = m_SetItems.begin();iter!=m_SetItems.end();++iter)
            {
                pResult = (*iter)->FindControlFromPoint(pt,flag);
                if( pResult != nullptr)
                {
                    if( (flag & UIFIND_HITTEST)!=0 && !pResult->IsFloat() &&!::PtInRect(&rc,pt) )
                        continue;
                    else
                        return pResult;
                }
            }
        }
     /*   else {
            for( int it = 0; it < m_items.GetSize(); it++ ) {
                CControlUI* pControl = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
                if( pControl != NULL ) {
                    if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
                        continue;
                    else 
                        return pControl;
                } 
            }
        }*/

        if( pResult == NULL && (flag & UIFIND_ME_FIRST) == 0 ) pResult = ControlUI::FindControlFromPoint(pt,flag);
        return pResult;
    }

}