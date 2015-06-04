#include "YUI.h"
#include "Container.h"
#include "ControlUI.h"
#include "Canvas2D.h"
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
        AddHandler();
    }

    Container::~Container()
    {

    }

    LPCTSTR Container::GetClass() const
    {
        return _T("Container");
    }

  

    

    int Container::GetCount() const
    {
        return m_ListItems.size();
    }

    bool Container::Add(ControlUI* pControl)
    {
        if(pControl == nullptr )
            return false;
        if( m_pManager )
            m_pManager->InitControls(pControl,this);
        if( IsVisible() )
            NeedUpdate();
        else
            pControl->SetInternVisible(false);
        m_ListItems.push_back(pControl);
        return true;
    }



    bool Container::Remove(ControlUI* pControl)
    {
         m_ListItems.remove(pControl);
         NeedUpdate();
        return true;
    }

  

    void Container::RemoveAll()
    {
        m_ListItems.clear();
        NeedUpdate();
    }


    void Container::SetVisible(bool bVisible /*= true*/)
    {
        if( m_bVisible == bVisible )
            return;
        ControlUI::SetVisible(true);
        for(auto iter: m_ListItems)
            iter->SetInternVisible(IsVisible());
    }

    // 逻辑上，对于Container控件不公开此方法
    // 调用此方法的结果是，内部子控件隐藏，控件本身依然显示，背景等效果存在
    void Container::SetInternVisible(bool bVisible /*= true*/)
    {
        ControlUI::SetInternVisible(bVisible);
        // 控制子控件显示状态
        // InternVisible状态应由子控件自己控制
        for(auto iter: m_ListItems)
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

    void Container::SetPos(YYRECT &rc)
    {
        ControlUI::SetPos(rc);
        if( m_ListItems.empty() ) return;
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;

        for(auto pControl : m_ListItems)
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

   

    void Container::DoPaint(const YYRECT &rc)
    {
        YYRECT rcTemp;
        YYRECT rcPaint = rc;
        if( !IntersectRect(&rcTemp, rcPaint, m_rcItem) ) 
            return;
        Canvas2D canvas(m_pManager->GetHWND());
        ClipRegionDef clipregion(rcTemp);
        canvas.ClipRect(clipregion);

        ControlUI::DoPaint(rcPaint);

        if( !m_ListItems.empty())
        {
            YYRECT subRC = m_rcItem;
            subRC.left += m_rcInset.left;
            subRC.top += m_rcInset.top;
            subRC.right -= m_rcInset.right;
            subRC.bottom -= m_rcInset.bottom;
            if(IntersectRect(&rcTemp,rcPaint,subRC))
            { 
                ClipRegionDef subRegion(rcTemp);
                canvas.ClipRect(subRegion);
               
                 for(auto &item : m_ListItems )
                 {
                     if(!item->IsVisible())
                         continue;
                     if(!IntersectRect(&rcTemp,rcPaint,item->GetPos()))
                         continue;
                     if(item->IsFloat())
                     {
                         if(!IntersectRect(&rcTemp,m_rcItem,item->GetPos()))
                             continue;
                         item->DoPaint(rcPaint);
                     }
                     else
                     {
                         if(!IntersectRect(&rcTemp,subRC,item->GetPos()))
                             continue; 
                         item->DoPaint(rcPaint);
                     }
                 }
            }
            else
            {
             /*for( auto &item : m_SetItems)
                 {
                if(!item->IsVisible())
                    continue;
               if(!::IntersectRect(&rcTemp,&rcPaint,&item->GetPos()))
                    continue;

            }*/
            }
         
        }

    }

    void Container::SetAttribute(const std::string &strName, const std::string& strValue)
    {
        LPCSTR pstrName = strName.c_str();
        LPCSTR pstrValue = strValue.c_str();
        if( strcmp(pstrName, "inset") == 0 ) {
            RECT rcInset = { 0 };
            LPSTR pstr = NULL;
            rcInset.left = strtol(pstrValue, &pstr, 10);  assert(pstr);    
            rcInset.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
            rcInset.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
            rcInset.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);    
            SetInset(rcInset);
        }
        else if( strcmp(pstrName,"mousechild") == 0 ) SetMouseChildEnabled(strcmp(pstrValue, "true") == 0);
        else if( strcmp(pstrName, "vscrollbar") == 0 ) {
            EnableScrollBar(strcmp(pstrValue, "true") == 0, GetHorizontalScrollBar() != NULL);
        }
        else if( strcmp(pstrName,"childpadding") == 0 ) SetChildPadding(atoi(pstrValue));
        else ControlUI::SetAttribute(strName, strValue);
    }

    void Container::SetManager(ControlManager* pManager, ControlUI* pParent, bool bInit/*=true*/)
    {
        for( auto &pControl : m_ListItems ) {
            pControl->SetManager(pManager, this, bInit);
        }

       /* if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->SetManager(pManager, this, bInit);
        if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->SetManager(pManager, this, bInit);*/
        ControlUI::SetManager(pManager, pParent, bInit);
    }

    bool Container::SetSubControlText(const YString & pstrSubControlName,const YString& pstrText)
    {
        auto pSubControl=FindSubControl(pstrSubControlName);
        if (pSubControl!=NULL)
        {
            pSubControl->SetText(pstrText);
            return TRUE;
        }
        else
            return FALSE;
    }

    bool Container::SetSubControlFixedHeight(const YString & pstrSubControlName,int height)
    {
           auto pSubControl=this->FindSubControl(pstrSubControlName);
           if (pSubControl!=NULL)
           {
               pSubControl->SetFixedHeight(height);
               return TRUE;
           }
           else
               return FALSE;
    }

    bool Container::SetSubControlFixedWdith(const YString& pstrSubControlName,int width)
    {
           auto pSubControl=this->FindSubControl(pstrSubControlName);
           if (pSubControl!=NULL)
           {
               pSubControl->SetFixedWidth(width);
               return TRUE;
           }
           else
               return FALSE;
    }

    bool Container::SetSubControlUserData(const YString& pstrSubControlName,const YString& pstrText)
    {

           auto pSubControl=this->FindSubControl(pstrSubControlName);
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
         auto pSubControl=this->FindSubControl(pstrSubControlName);
         if (pSubControl==NULL)
             return _T("");
         else
             return pSubControl->GetText();
    }

    int Container::GetSubControlFixedHeight(const YString& pstrSubControlName)
    {
         auto pSubControl=this->FindSubControl(pstrSubControlName);
         if (pSubControl==NULL)
             return -1;
         else
             return pSubControl->GetFixedHeight();
    }

    int Container::GetSubControlFixedWdith(const YString& pstrSubControlName)
    {
        auto pSubControl=this->FindSubControl(pstrSubControlName);
        if (pSubControl==NULL)
            return -1;
        else
            return pSubControl->GetFixedWidth();
    }

    YUI::YString Container::GetSubControlUserData(LPCTSTR pstrSubControlName)
    {
        auto pSubControl=this->FindSubControl(pstrSubControlName);
        if (pSubControl==NULL)
            return _T("");
        else
            return pSubControl->GetUserData();
    }

    ControlUI* Container::FindSubControl(const YString& pstrSubControlName)
    {
        auto spRoot = m_pManager->GetRoot();
        return spRoot->FindControlFromName(pstrSubControlName);
    }

    YUI::YYSIZE Container::GetScrollPos() const
{
        YYSIZE sz ;
        return sz;
    }

    YUI::YYSIZE Container::GetScrollRange() const
{
        YYSIZE sz ;
        return sz;
    }

    void Container::SetScrollPos(YYSIZE szPos)
    {
       
    }

    void Container::LineUp()
    {
        int cyLine = 8;
        //if( m_pManager ) cyLine = m_pManager->GetDefaultFontInfo()->m_tm.tmHeight + 8;

        YYSIZE sz = GetScrollPos();
        sz.height -= cyLine;
        SetScrollPos(sz);
    }

    void Container::LineDown()
    {
        int cyLine = 8;
        //if( m_pManager ) cyLine = m_pManager->GetDefaultFontInfo()->m_tm.tmHeight + 8;

        YYSIZE sz = GetScrollPos();
        sz.height += cyLine;
        SetScrollPos(sz);
    }

    void Container::PageUp()
    {
        YYSIZE sz = GetScrollPos();
        int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
        /*if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();*/
        sz.height -= iOffset;
        SetScrollPos(sz);
    }

    void Container::PageDown()
    {
        YYSIZE sz = GetScrollPos();
        int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
      /*  if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();*/
        sz.height += iOffset;
        SetScrollPos(sz);
    }

    void Container::HomeUp()
    {
        YYSIZE sz = GetScrollPos();
        sz.height = 0;
        SetScrollPos(sz);
    }

    void Container::EndDown()
    {
        YYSIZE sz = GetScrollPos();
        sz.height = GetScrollRange().height;
        SetScrollPos(sz);
    }

    void Container::LineLeft()
    {
        YYSIZE sz = GetScrollPos();
        sz.width -= 8;
        SetScrollPos(sz);
    }

    void Container::LineRight()
    {
        YYSIZE sz = GetScrollPos();
        sz.width += 8;
        SetScrollPos(sz);
    }

    void Container::PageLeft()
    {
        YYSIZE sz = GetScrollPos();
        int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
        /*if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) iOffset -= m_pVerticalScrollBar->GetFixedWidth();*/
        sz.width -= iOffset;
        SetScrollPos(sz);
    }

    void Container::PageRight()
    {
        YYSIZE sz = GetScrollPos();
        int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
        /*if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) iOffset -= m_pVerticalScrollBar->GetFixedWidth();*/
        sz.width += iOffset;
        SetScrollPos(sz);
    }

    void Container::HomeLeft()
    {
        YYSIZE sz = GetScrollPos();
        sz.width = 0;
        SetScrollPos(sz);
    }

    void Container::EndRight()
    {
        YYSIZE sz = GetScrollPos();
        sz.width = GetScrollRange().width;
        SetScrollPos(sz);
    }

    void Container::EnableScrollBar(bool bEnableVertical /*= true*/, bool bEnableHorizontal /*= false*/)
    {

    }

    Scrollbar* Container::GetVerticalScrollBar() const
    {
        //return m_pVerticalScrollBar;
        return nullptr;
    }

    Scrollbar* Container::GetHorizontalScrollBar() const
    {
        //return m_pHorizontalScrollBar;
        return nullptr;
    }

    void Container::SetFloatPos(ControlUI* pControl)
    {
        // 因为CControlUI::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
        if( !pControl || m_ListItems.empty() ) return;

        if( !pControl->IsVisible() ) return;
        if( !pControl->IsFloat() ) return;

        YYSIZE szXY = pControl->GetFixedXY();
        YYSIZE sz(pControl->GetFixedWidth(), pControl->GetFixedHeight());
        YYRECT rcCtrl;
        if( szXY.width >= 0 ) {
            rcCtrl.left = m_rcItem.left + szXY.width;
            rcCtrl.right = m_rcItem.left + szXY.width + sz.width;
        }
        else {
            rcCtrl.left = m_rcItem.right + szXY.width - sz.width;
            rcCtrl.right = m_rcItem.right + szXY.width;
        }
        if( szXY.height >= 0 ) {
            rcCtrl.top = m_rcItem.top + szXY.height;
            rcCtrl.bottom = m_rcItem.top + szXY.height + sz.height;
        }
        else {
            rcCtrl.top = m_rcItem.bottom + szXY.height - sz.height;
            rcCtrl.bottom = m_rcItem.bottom + szXY.height;
        }
        if( pControl->IsRelativePos() )
        {
            RelativePosUI tRelativePos = pControl->GetRelativePos();
            YYSIZE szParent(m_rcItem.right-m_rcItem.left,m_rcItem.bottom-m_rcItem.top);
            if(tRelativePos.m_szParent.width != 0)
            {
                int nIncrementX = szParent.width-tRelativePos.m_szParent.width;
                int nIncrementY = szParent.height-tRelativePos.m_szParent.height;
                rcCtrl.left += (nIncrementX*tRelativePos.m_nMoveXPercent/100);
                rcCtrl.top += (nIncrementY*tRelativePos.m_nMoveYPercent/100);
                rcCtrl.right = rcCtrl.left+sz.width+(nIncrementX*tRelativePos.m_nZoomXPercent/100);
                rcCtrl.bottom = rcCtrl.top+sz.height+(nIncrementY*tRelativePos.m_nZoomYPercent/100);
            }
            pControl->SetRelativeParentSize(szParent);
        }
        pControl->SetPos(rcCtrl);
    }

    void Container::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
    {

    }

    ControlUI* Container::FindControlFromPoint(POINT pt,UINT flag)
    {  
        ControlUI *pResult = nullptr ;
        if(  !IsVisible() ) 
            return NULL;
        if( !IsEnabled() ) 
            return NULL;
        RECT rc = m_rcItem;
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;
        //本身的m_items自带排序功能，按插入位置
    
            for(auto iter = m_ListItems.begin();iter!=m_ListItems.end();++iter)
            {
                pResult = (*iter)->FindControlFromPoint(pt,flag);
                if( pResult != nullptr)
                {
                    return pResult;
                }
            }

        if( pResult == NULL ) 
            pResult = ControlUI::FindControlFromPoint(pt,flag);
        return pResult;
    }

    void Container::AddHandler()
    {
        m_ContainerMsgHandler.SetSuccessor(&m_ControlMsgHandler);
        m_ContainerMsgHandler.AddEntry(UIMSG_SETFOCUS,[&](const MsgWrap &msg)
        {
            m_bFocused = true;
            Invalidate();
        });

        m_ContainerMsgHandler.AddEntry(UIMSG_KILLFOCUS,[&](const MsgWrap &msg)
        {
            m_bFocused = false;
            Invalidate();
        });
    }

    void Container::HandleMsg(const MsgWrap & msg) throw()
    {
        m_ContainerMsgHandler.HandleMsg(msg);
    }

}