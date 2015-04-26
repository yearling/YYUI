#include "YUI.h"
#include "UIDef.h"
#include "ControlUI.h"
#include "PaintManagerUI.h"
#include "RenderDGI.h"
namespace YUI
{
    ControlUI::ControlUI():
         m_bUpdateNeeded(true)
        ,m_bMenuUsed(false)
        ,m_bVisible(true)
        ,m_bInternVisible(true)
        ,m_bFocused(false)
        ,m_bEnable(true)
        ,m_bMouseEnabled(true)
        ,m_bKeyboardEnabled(true)
        ,m_bFloat(false)
        ,m_bSetPos(false)
        ,m_chShortcut(_T('\0'))
        ,m_pTag(NULL)
        ,m_dwBackColor(0)
        ,m_dwBackColor2(0)
        ,m_dwBackColor3(0)
        ,m_dwBorderColor(0)
        ,m_dwFocusBorderColor(0)
        ,m_bColorHSL(false)
        ,m_nBorderSize(0)
        ,m_nBorderStyle(PS_SOLID)
        ,m_nTooltipWidth(300)
    {
        m_cXY.cx = m_cXY.cy = 0;
        m_cXYFixed.cx = m_cXYFixed.cy = 0;
        m_cxyMin.cx = m_cxyMin.cy = 0;
        m_cxyMax.cx = m_cxyMax.cy = 9999;
        m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;
        ZeroMemory(&m_rcPaint,sizeof(RECT));
        ZeroMemory(&m_rcItem, sizeof(RECT));
        ZeroMemory(&m_rcPadding, sizeof(RECT));
        ZeroMemory(&m_rcBorderSize ,sizeof(RECT));
        ZeroMemory(&m_RelativePos,sizeof(RelativePosUI));
    }

    LPCTSTR ControlUI::GetClass() const
    {
        return _T("ControlUI");
    }


    YUI::YString ControlUI::GetName() const
    {
        return m_strName;
    }

    void ControlUI::SetName(const YString & strName)
    {
        m_strName = strName;
    }

    std::shared_ptr<ControlUI> ControlUI::QueryInterface(const YString & strName)
    {
        if( strName == CTR_CONTROL )
            return this->shared_from_this();
        return std::shared_ptr<ControlUI>();
    }

    UINT ControlUI::GetControlFlags() const
    {
        return 0;
    }

    bool ControlUI::Activate()
    {
        if(!IsVisible())
            return false;
        if(!IsVisible())
            return false;
        return true;
    }

    std::shared_ptr<PaintManagerUI> ControlUI::GetManager() const
    {
        return m_pManager;
    }

    void ControlUI::SetManager(std::shared_ptr<PaintManagerUI> &pManager, std::weak_ptr<ControlUI> pParent, bool bInit/*=true*/)
    {
        m_pManager=pManager;
        m_pParent=pParent;
        if(bInit&& (!m_pParent.expired()))
            Init();
    }

    std::weak_ptr<ControlUI> ControlUI::GetParent() const
    {
        return m_pParent;
    }

    YUI::YString ControlUI::GetText() const
    {
       return m_strText; 
    }

    void ControlUI::SetText(YString const & strName)
    {
        if( m_strText == strName )
            return ;
        m_strText = strName;
        Invalidate();
    }

    DWORD ControlUI::GetBkColor() const
    {
        return m_dwBackColor;
    }

    void ControlUI::SetBkColor(DWORD dwBackColor)
    {
        if( m_dwBackColor == dwBackColor )
            return;
        m_dwBackColor = dwBackColor;
        Invalidate();
    }

    DWORD ControlUI::GetBkColor2() const
    {
        return m_dwBackColor2;
    }

    void ControlUI::SetBkColor2(DWORD dwBackColor)
    {
        if( m_dwBackColor2 == dwBackColor )
            return;
        m_dwBackColor2 = dwBackColor;
        Invalidate();
    }

    DWORD ControlUI::GetBkColor3() const
    {
        return m_dwBackColor3;
    }

    void ControlUI::SetBkColor3(DWORD dwBackColor)
    {
        if( m_dwBackColor3 == dwBackColor ) return;

        m_dwBackColor3 = dwBackColor;
        Invalidate();
    }

   

    void ControlUI::SetBkImage(const YString &strImage)
    {
        if( m_strBKImage == strImage )
            return;
        m_strBKImage = strImage;
        Invalidate();
    }

    DWORD ControlUI::GetFocusBorderColor() const
    {
        return m_dwFocusBorderColor;
    }

    void ControlUI::SetFocusBorderColor(DWORD dwBorderColor)
    {
        if( m_dwFocusBorderColor == dwBorderColor )
            return;
        m_dwFocusBorderColor = dwBorderColor;
        Invalidate();
    }

    bool ControlUI::IsColorHSL() const
    {
        return m_bColorHSL;
    }

    void ControlUI::SetColorHSL(bool bColorHSL)
    {
        if( m_bColorHSL == bColorHSL )
            return;
        m_bColorHSL = bColorHSL;
        Invalidate();
    }

    YUI::YString ControlUI::GetBkImage()
    {
        return m_strBKImage;
    }

    SIZE ControlUI::GetBorderRound() const
    {
        return m_cxyBorderRound;
    }

    void ControlUI::SetBorderRound(SIZE cxyRound)
    {
        m_cxyBorderRound = cxyRound;
        Invalidate();
    }

    bool ControlUI::DrawImage(HDC hDc, const YString strImg, const YString strModify /*=_T("")*/)
    {
        return RenderGDI::DrawImageString(hDc,m_pManager,m_rcItem,m_rcPaint,strImg,strModify);
    }

    int ControlUI::GetBorderSize() const
    {
        return m_nBorderSize;
    }

    void ControlUI::SetBorderSize(int nSize)
    {
        if( m_nBorderSize == nSize )
            return;
        m_nBorderSize = nSize;
        Invalidate();
    }

    void ControlUI::SetBorderSize(RECT rc)
    {
        m_rcBorderSize = rc;
        Invalidate();
    }

    DWORD ControlUI::GetBorderColor() const
    {
       return  m_dwBorderColor;
    }

    void ControlUI::SetBorderColor(DWORD dwBorderColor)
    {
        if( m_dwBorderColor == dwBorderColor )
            return;
        m_dwBorderColor = dwBorderColor;
        Invalidate();
    }

    int ControlUI::GetLeftBorderSize() const
    {
        return m_rcBorderSize.left;
    }

    void ControlUI::SetLeftBorderSize(int nSize)
    {
        m_rcBorderSize.left = nSize;
        Invalidate();
    }

    int ControlUI::GetTopBorderSize() const
    {
       return m_rcBorderSize.top; 
    }

    void ControlUI::SetTopBorderSize(int nSize)
    {
        m_rcBorderSize.top = nSize;
        Invalidate();
    }

    int ControlUI::GetRightBorderSize() const
    {
       return m_rcBorderSize.right; 
    }

    void ControlUI::SetRightBorderSize(int nSize)
    {
        m_rcBorderSize.right = nSize;
        Invalidate();
    }

    int ControlUI::GetBottomBorderSize() const
    {
        return m_rcBorderSize.bottom;
    }

    void ControlUI::SetBottomBorderSize(int nSize)
    {
        m_rcBorderSize.bottom = nSize;
        Invalidate();

    }

    int ControlUI::GetBorderStyle() const
    {
        return m_nBorderStyle;
    }

    void ControlUI::SetBorderStyle(int nStyle)
    {
        m_nBorderStyle = nStyle;
        Invalidate();
    }

    RECT ControlUI::GetPos() const
    {
        return m_rcItem;
    }

    void ControlUI::SetPos(RECT &rc)
    {
        if( rc.right < rc.left ) 
            std::swap(rc.right,rc.left);
        if( rc.bottom < rc.top ) 
            std::swap(rc.top,rc.bottom);
            

        YRect invalidateRc = m_rcItem;
        if( ::IsRectEmpty(&invalidateRc) ) invalidateRc = rc;

        m_rcItem = rc;
        if( m_pManager == NULL ) return;

        if( !m_bSetPos ) {
            m_bSetPos = true;
            if( OnSize ) OnSize(shared_from_this());
            m_bSetPos = false;
        }

        if( m_bFloat ) {
            auto pParent = GetParent().lock();
            if( pParent ) {
                RECT rcParentPos = pParent->GetPos();
                if( m_cXY.cx >= 0 ) m_cXY.cx = m_rcItem.left - rcParentPos.left;
                else m_cXY.cx = m_rcItem.right - rcParentPos.right;
                if( m_cXY.cy >= 0 ) m_cXY.cy = m_rcItem.top - rcParentPos.top;
                else m_cXY.cy = m_rcItem.bottom - rcParentPos.bottom;
                m_cXYFixed.cx = m_rcItem.right - m_rcItem.left;
                m_cXYFixed.cy = m_rcItem.bottom - m_rcItem.top;
            }
        }

        m_bUpdateNeeded = false;
        invalidateRc.Join(m_rcItem);

        auto pParent = shared_from_this();
        RECT rcTemp;
        RECT rcParent;
        while( pParent = pParent->GetParent().lock() )
        {
            rcTemp = invalidateRc;
            rcParent = pParent->GetPos();
            if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
            {
                return;
            }
        }
        m_pManager->Invalidate(invalidateRc);
    }

    int ControlUI::GetWidth() const
    {
        return m_rcItem.right - m_rcItem.left;
    }

    int ControlUI::GetHeight() const
    {
        return m_rcItem.bottom - m_rcItem.top;
    }

    int ControlUI::GetX() const
    {
        return m_rcItem.left;
    }

    int ControlUI::GetY() const
    {
        return m_rcItem.top;
    }

    RECT ControlUI::GetPadding() const
    {
        return m_rcPadding;
    }

    void ControlUI::SetPadding(RECT rcPadding)
    {
        m_rcPadding = rcPadding;
        Invalidate();
    }

    SIZE ControlUI::GetFixedXY() const
    {
        return m_cXY;
    }

    void ControlUI::SetFixedXY(SIZE szXY)
    {
        m_cXY.cx = szXY.cx;
        m_cXY.cy = szXY.cy;
        if( !m_bFloat )
            NeedParentUpdate();
        else
            NeedUpdate();
    }

    int ControlUI::GetFixedWidth() const
    {
        return m_cXYFixed.cx;
    }

    void ControlUI::SetFixedWidth(int cx)
    {
        if(cx < 0 )
            return;
        m_cXYFixed.cx = cx;
        if( !m_bFloat )
            NeedParentUpdate();
        else 
            NeedUpdate();
    }

    int ControlUI::GetFixedHeight() const
    {
        return m_cXYFixed.cy;
    }

    void ControlUI::SetFixedHeight(int cy)
    {
        if( cy < 0 )
            return ;
        m_cXYFixed.cy = cy;
        if( !m_bFloat )
            NeedParentUpdate();
        else
            NeedUpdate();
    }

    int ControlUI::GetMinWidth() const
    {
        return m_cxyMin.cx ;
    }

    void ControlUI::SetMinWidth(int cx)
    {
        if( m_cxyMin.cx == cx)
            return;
        if(cx < 0 )
            return ;
        m_cxyMin.cx = cx;
        if( !m_bFloat )
            NeedParentUpdate();
        else
            NeedUpdate();
    }

    int ControlUI::GetMaxWidth() const
    {
        return m_cxyMax.cx;
    }

    void ControlUI::SetMaxWidth(int cx)
    {
        if( m_cxyMax.cx == cx ) return;

        if( cx < 0 ) return; 
        m_cxyMax.cx = cx;
        if( !m_bFloat ) NeedParentUpdate();
        else NeedUpdate();
    }

    int ControlUI::GetMinHeight() const
    {
        return m_cxyMin.cy;
    }

    void ControlUI::SetMinHeight(int cy)
    {
        if( m_cxyMin.cy == cy ) return;

        if( cy < 0 ) return; 
        m_cxyMin.cy = cy;
        if( !m_bFloat ) NeedParentUpdate();
        else NeedUpdate();
    }

    int ControlUI::GetMaxHeight() const
    {
         return m_cxyMax.cy;
    }

    void ControlUI::SetMaxHeight(int cy)
    {
        if( m_cxyMax.cy == cy ) return;

        if( cy < 0 ) return; 
        m_cxyMax.cy = cy;
        if( !m_bFloat ) NeedParentUpdate();
        else NeedUpdate();
    }

    void ControlUI::SetRelativePos(SIZE szMove,SIZE szZoom)
    {
        m_RelativePos.m_bRelative = true;
        m_RelativePos.m_nMoveXPercent = szMove.cx;
        m_RelativePos.m_nMoveYPercent = szMove.cy;
        m_RelativePos.m_nZoomXPercent = szZoom.cx;
        m_RelativePos.m_nZoomYPercent = szZoom.cy;
    }

    void ControlUI::SetRelativeParentSize(SIZE sz)
    {
         m_RelativePos.m_szParent = sz;
    }

    YUI::RelativePosUI ControlUI::GetRelativePos() const
    {
        return m_RelativePos;
    }

    bool ControlUI::IsRelativePos() const
    {
        return m_RelativePos.m_bRelative;
    }

    YUI::YString ControlUI::GetToolTip() const
    {
        return m_strToolTip;
    }

    void ControlUI::SetToolTip(const YString&  pstrText)
    {
        m_strToolTip = pstrText;
       // m_strToolTip.replace(0,std::string::npos,_T("<n>"),_T("\r\n"));
    }

    void ControlUI::SetToolTipWidth(int nWidth)
    {
        m_nTooltipWidth = nWidth;
    }

    int ControlUI::GetToolTipWidth(void)
    {
        return m_nTooltipWidth;
    }

    TCHAR ControlUI::GetShortcut() const
    {
        return m_chShortcut;
    }

    void ControlUI::SetShortcut(TCHAR ch)
    {
        m_chShortcut = ch;
    }

    bool ControlUI::IsContextMenuUsed() const
    {
        return m_bMenuUsed;
    }

    void ControlUI::SetContextMenuUsed(bool bMenuUsed)
    {
        m_bMenuUsed = bMenuUsed;
    }

    YUI::YString ControlUI::GetUserData()
    {
        return m_strUserData;
    }

    void ControlUI::SetUserData(const YString& strText)
    {
        m_strUserData = strText;
    }

    UINT_PTR ControlUI::GetTag() const
    {
        return m_pTag;
    }

    void ControlUI::SetTag(UINT_PTR pTag)
    {
        m_pTag = pTag;
    }

    bool ControlUI::IsVisible() const
    {
        return m_bVisible && m_bInternVisible;
    }

    void ControlUI::SetVisible(bool bVisible /*= true*/)
    {
        if( m_bVisible == bVisible )
            return;
        bool v = IsVisible();
        m_bVisible = bVisible;
        if( m_bFocused )
            m_bFocused = false;
        if( !bVisible && m_pManager && m_pManager->GetFocus()==shared_from_this())
        {
            m_pManager->SetFocus(NULL);
        }
        if( IsVisible() != v)
            NeedParentUpdate();
    }

    void ControlUI::SetInternVisible(bool bVisible /*= true*/)
    {
        m_bInternVisible = bVisible;
        if( !bVisible && m_pManager && m_pManager->GetFocus() == shared_from_this())
            m_pManager->SetFocus(nullptr);
    }

    bool ControlUI::IsEnabled() const
    {
        return m_bEnable;
    }

    void ControlUI::SetEnabled(bool bEnable /*= true*/)
    {
        if( m_bEnable == bEnable )
            return;
        m_bEnable = bEnable;
        Invalidate();
    }

    bool ControlUI::IsMouseEnabled() const
    {
        return m_bMouseEnabled;
    }

    void ControlUI::SetMouseEnabled(bool bEnable /*= true*/)
    {
        m_bMouseEnabled = bEnable;
    }

    bool ControlUI::IsKeyboardEnabled() const
    {
        return m_bKeyboardEnabled;
    }

    void ControlUI::SetKeyboardEnabled(bool bEnable /*= true*/)
    {
        m_bKeyboardEnabled = bEnable; 
    }

    bool ControlUI::IsFocused() const
    {
       return m_bFocused;
    }

    void ControlUI::SetFocus()
    {
        if( m_pManager )
            m_pManager->SetFocus(shared_from_this());
    }

    bool ControlUI::IsFloat() const
    {
        return m_bFloat;
    }

    void ControlUI::SetFloat(bool bFloat /*= true*/)
    {
        if( m_bFloat == bFloat )
            return;
        m_bFloat = bFloat;
        NeedParentUpdate();
    }

    std::shared_ptr<ControlUI> ControlUI::FindControlFromPoint(POINT pt,UINT flag)
    {
        if( (flag & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
        if( (flag & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
        if( (flag & UIFIND_HITTEST) != 0 && (!m_bMouseEnabled || !::PtInRect(&m_rcItem, pt ) ))
            return NULL;
        return ::PtInRect(&m_rcItem,pt)? shared_from_this() : NULL;
    }

    void ControlUI::Invalidate()
    {
        if( !IsVisible() ) return;

        RECT invalidateRc = m_rcItem;

        auto pParent = shared_from_this();
        RECT rcTemp;
        RECT rcParent;
        while( pParent = pParent->GetParent().lock() )
        {
            rcTemp = invalidateRc;
            rcParent = pParent->GetPos();
            if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
            {
                return;
            }
        }

        if( m_pManager != NULL ) 
            m_pManager->Invalidate(invalidateRc);
    }

    bool ControlUI::IsUpdateNeeded() const
    {
        return m_bUpdateNeeded;
    }

    void ControlUI::NeedUpdate()
    {
        if( !IsVisible() ) return;
        m_bUpdateNeeded = true;
        Invalidate();

        if( m_pManager != NULL ) m_pManager->NeedUpdate();
    }

    void ControlUI::NeedParentUpdate()
    {
        auto spParent = GetParent().lock();
        if( spParent ) 
        {
            spParent->NeedUpdate();
            spParent->Invalidate();
        }
        else 
        {
            NeedUpdate();
        }

        if( m_pManager != NULL ) 
            m_pManager->NeedUpdate();
    }

    DWORD ControlUI::GetAdjustColor(DWORD dwColor)
    {
       /* if( !m_bColorHSL ) return dwColor;
        short H, S, L;*/
        
    /*    CPaintManagerUI::GetHSL(&H, &S, &L);
        return CRenderEngine::AdjustColor(dwColor, H, S, L);*/
        return dwColor;
    }

    void ControlUI::Init()
    {
        DoInit();
        if( OnInit )
            OnInit(shared_from_this());
    }

    void ControlUI::DoInit()
    {

    }

    void ControlUI::Event(ControlEvent& eve)
    {
        if(OnEvent && OnEvent(eve) )
            DoEvent(eve);
    }

    void ControlUI::DoEvent(ControlEvent& eve)
    {
        if( eve.m_Type == UIEVENT_SETCURSOR )
        {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
            return;
        }
        if( eve.m_Type == UIEVENT_SETFOCUS ) 
        {
            m_bFocused = true;
            Invalidate();
            return;
        }
        if( eve.m_Type == UIEVENT_KILLFOCUS ) 
        {
            m_bFocused = false;
            Invalidate();
            return;
        }
        if( eve.m_Type == UIEVENT_TIMER )
        {
            m_pManager->SendNotify(shared_from_this(), MSG_TIMER, eve.m_wParam, eve.m_lParam);
            return;
        }
        if( eve.m_Type == UIEVENT_CONTEXTMENU )
        {
            if( IsContextMenuUsed() ) {
                m_pManager->SendNotify(shared_from_this(), MSG_MENU, eve.m_wParam, eve.m_lParam);
                return;
            }
        }
        auto spParent = m_pParent.lock();
        if( spParent ) 
            spParent->DoEvent(eve);
    }

    void ControlUI::SetAttribute(const YString &strName, const YString& strValue)
    {
        auto pstrName = strName.c_str();
        auto pstrValue = strValue.c_str();
        if( _tcscmp(pstrName, _T("pos")) == 0 ) {
            RECT rcPos = { 0 };
            LPTSTR pstr = NULL;
            rcPos.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
            rcPos.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
            rcPos.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
            rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
            SIZE szXY = {rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom};
            SetFixedXY(szXY);
            SetFixedWidth(rcPos.right - rcPos.left);
            SetFixedHeight(rcPos.bottom - rcPos.top);
        }
        else if( _tcscmp(pstrName, _T("relativepos")) == 0 ) {
            SIZE szMove,szZoom;
            LPTSTR pstr = NULL;
            szMove.cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
            szMove.cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
            szZoom.cx = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
            szZoom.cy = _tcstol(pstr + 1, &pstr, 10); assert(pstr); 
            SetRelativePos(szMove,szZoom);
        }
        else if( _tcscmp(pstrName, _T("padding")) == 0 ) {
            RECT rcPadding = { 0 };
            LPTSTR pstr = NULL;
            rcPadding.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
            rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
            rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
            rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);    
            SetPadding(rcPadding);
        }
        else if( _tcscmp(pstrName, _T("bkcolor")) == 0 || _tcscmp(pstrName, _T("bkcolor1")) == 0 ) {
            while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
            if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetBkColor(clrColor);
        }
        else if( _tcscmp(pstrName, _T("bkcolor2")) == 0 ) {
            while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
            if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetBkColor2(clrColor);
        }
        else if( _tcscmp(pstrName, _T("bkcolor3")) == 0 ) {
            while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
            if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetBkColor3(clrColor);
        }
        else if( _tcscmp(pstrName, _T("bordercolor")) == 0 ) {
            if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetBorderColor(clrColor);
        }
        else if( _tcscmp(pstrName, _T("focusbordercolor")) == 0 ) {
            if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetFocusBorderColor(clrColor);
        }
        else if( _tcscmp(pstrName, _T("colorhsl")) == 0 ) SetColorHSL(_tcscmp(pstrValue, _T("true")) == 0);
        else if( _tcscmp(pstrName, _T("bordersize")) == 0 ) {
            YString nValue = pstrValue;
            if(nValue.find(_T(',')) < 0)
            {
                SetBorderSize(_ttoi(pstrValue));
                RECT rcPadding = {0};
                SetBorderSize(rcPadding);
            }
            else
            {
                RECT rcPadding = { 0 };
                LPTSTR pstr = NULL;
                rcPadding.left = _tcstol(pstrValue, &pstr, 10);  assert(pstr);
                rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);
                rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);
                rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
                SetBorderSize(rcPadding);
            }
        }
        else if( _tcscmp(pstrName, _T("leftbordersize")) == 0 ) SetLeftBorderSize(_ttoi(pstrValue));
        else if( _tcscmp(pstrName, _T("topbordersize")) == 0 ) SetTopBorderSize(_ttoi(pstrValue));
        else if( _tcscmp(pstrName, _T("rightbordersize")) == 0 ) SetRightBorderSize(_ttoi(pstrValue));
        else if( _tcscmp(pstrName, _T("bottombordersize")) == 0 ) SetBottomBorderSize(_ttoi(pstrValue));
        else if( _tcscmp(pstrName, _T("borderstyle")) == 0 ) SetBorderStyle(_ttoi(pstrValue));
        else if( _tcscmp(pstrName, _T("borderround")) == 0 ) {
            SIZE cxyRound = { 0 };
            LPTSTR pstr = NULL;
            cxyRound.cx = _tcstol(pstrValue, &pstr, 10);  assert(pstr);    
            cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);     
            SetBorderRound(cxyRound);
        }
        else if( _tcscmp(pstrName, _T("bkimage")) == 0 ) SetBkImage(pstrValue);
        else if( _tcscmp(pstrName, _T("width")) == 0 ) SetFixedWidth(_ttoi(pstrValue));
        else if( _tcscmp(pstrName, _T("height")) == 0 ) SetFixedHeight(_ttoi(pstrValue));
        else if( _tcscmp(pstrName, _T("minwidth")) == 0 ) SetMinWidth(_ttoi(pstrValue));
        else if( _tcscmp(pstrName, _T("minheight")) == 0 ) SetMinHeight(_ttoi(pstrValue));
        else if( _tcscmp(pstrName, _T("maxwidth")) == 0 ) SetMaxWidth(_ttoi(pstrValue));
        else if( _tcscmp(pstrName, _T("maxheight")) == 0 ) SetMaxHeight(_ttoi(pstrValue));
        else if( _tcscmp(pstrName, _T("name")) == 0 ) SetName(pstrValue);
        else if( _tcscmp(pstrName, _T("text")) == 0 ) SetText(pstrValue);
        else if( _tcscmp(pstrName, _T("tooltip")) == 0 ) SetToolTip(pstrValue);
        else if( _tcscmp(pstrName, _T("userdata")) == 0 ) SetUserData(pstrValue);
        else if( _tcscmp(pstrName, _T("enabled")) == 0 ) SetEnabled(_tcscmp(pstrValue, _T("true")) == 0);
        else if( _tcscmp(pstrName, _T("mouse")) == 0 ) SetMouseEnabled(_tcscmp(pstrValue, _T("true")) == 0);
        else if( _tcscmp(pstrName, _T("keyboard")) == 0 ) SetKeyboardEnabled(_tcscmp(pstrValue, _T("true")) == 0);
        else if( _tcscmp(pstrName, _T("visible")) == 0 ) SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
        else if( _tcscmp(pstrName, _T("float")) == 0 ) SetFloat(_tcscmp(pstrValue, _T("true")) == 0);
        else if( _tcscmp(pstrName, _T("shortcut")) == 0 ) SetShortcut(pstrValue[0]);
        else if( _tcscmp(pstrName, _T("menu")) == 0 ) SetContextMenuUsed(_tcscmp(pstrValue, _T("true")) == 0);
        else if( _tcscmp(pstrName, _T("virtualwnd")) == 0 ) SetVirtualWnd(pstrValue);
    }

    std::shared_ptr<ControlUI> ControlUI::ApplyAttributeList(const YString& strList)
    {
        YString sItem;
        YString sValue;
        auto pstrList = strList.c_str();
        while( *pstrList != _T('\0') ) {
            sItem.clear();
            sValue.clear();
            while( *pstrList != _T('\0') && *pstrList != _T('=') ) {
                LPTSTR pstrTemp = ::CharNext(pstrList);
                while( pstrList < pstrTemp) {
                    sItem += *pstrList++;
                }
            }
            assert( *pstrList == _T('=') );
            if( *pstrList++ != _T('=') ) 
                return shared_from_this();
            assert( *pstrList == _T('\"') );
            if( *pstrList++ != _T('\"') )
                return shared_from_this();
            while( *pstrList != _T('\0') && *pstrList != _T('\"') ) {
                LPTSTR pstrTemp = ::CharNext(pstrList);
                while( pstrList < pstrTemp) {
                    sValue += *pstrList++;
                }
            }
            assert( *pstrList == _T('\"') );
            if( *pstrList++ != _T('\"') ) 
                return shared_from_this();
            SetAttribute(sItem, sValue);
            if( *pstrList++ != _T(' ') )
                return shared_from_this();
        }
        return shared_from_this();
    }

    SIZE ControlUI::EstimateSize(SIZE szAvailable)
    {
        return m_cXYFixed;
    }

    void ControlUI::DoPaint(HDC hDC, const RECT& rcPaint)
    {
        
    }

    void ControlUI::PaintBkColor(HDC hDC)
    {

    }

    void ControlUI::PaintBkImage(HDC hDC)
    {

    }

    void ControlUI::PaintStatusImage(HDC hDC)
    {

    }

    void ControlUI::PaintText(HDC hDC)
    {

    }

    void ControlUI::PaintBorder(HDC hDC)
    {

    }

    void ControlUI::DoPostPaint(HDC hDC, const RECT& rcPaint)
    {

    }

    void ControlUI::SetVirtualWnd(LPCTSTR pstrValue)
    {

    }

    YUI::YString ControlUI::GetVirtualWnd() const
    {
        return _T("");
    }

    ControlUI::~ControlUI()
    {

    }

    std::shared_ptr<ControlUI>& ControlUI::FindControlFromName(const YString & strName)
    {
        if(strName == GetName() )
            return shared_from_this();
        else
            return std::shared_ptr<ControlUI>();
    }




}