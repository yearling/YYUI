#include "YUI.h"
#include "UIDef.h"
#include "ControlUI.h"
#include "ControlManager.h"
#include "Canvas2D.h"
#include "UIUtility.h"

using  std::cout;
using  std::wcout;
using  std::endl;
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
        ,m_nBorderSize(0)
        ,m_nBorderStyle(PS_SOLID)
        ,m_nTooltipWidth(300)
    {
        m_cXY.cx = m_cXY.cy = 0;
        m_cXYFixed.cx = m_cXYFixed.cy = 0;
        m_cxyMin.cx = m_cxyMin.cy = 0;
        m_cxyMax.cx = m_cxyMax.cy = 9999;
        ZeroMemory(&m_rcPaint,sizeof(RECT));
        ZeroMemory(&m_rcItem, sizeof(RECT));
        ZeroMemory(&m_rcPadding, sizeof(RECT));
        ZeroMemory(&m_rcBorderSize ,sizeof(RECT));
        ZeroMemory(&m_RelativePos,sizeof(RelativePosUI));
        AddHandler();
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

    UINT ControlUI::GetControlFlags() const
    {
        return 0;
    }

    bool ControlUI::Activate()
    {
        if(!IsVisible())
            return false;
        if(!IsEnabled())
            return false;
        return true;
    }

    ControlManager* ControlUI::GetManager() const
    {
        return m_pManager;
    }

    void ControlUI::SetManager(ControlManager *pManager,ControlUI* pParent, bool bInit/*=true*/)
    {
        m_pManager=pManager;
        m_pParent=pParent;
        if(bInit && m_pParent)
            Init();
    }

    ControlUI *ControlUI::GetParent() const
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

   

  

    YUI::YString ControlUI::GetBkImage()
    {
        return m_strBKImage;
    }

  

    void ControlUI::DrawImage(const YString &strImg)
    {
        Canvas2D canvas(m_pManager->GetHWND());
        //canvas.DrawBitmap(strImg,m_rcItem);
        // 1、aaa.jpg
        // 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
        // mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'

        YString sImageName = strImg;
        LPCTSTR pStrImage = strImg.c_str();
        YString sImageResType;
        RECT rcItem = m_rcItem;
        RECT rc = m_rcItem;
        RECT rcBmpPart = {0};
        RECT rcCorner = {0};
        DWORD dwMask = 0;
        BYTE bFade = 0xFF;
        bool bHole = false;
        bool bTiledX = false;
        bool bTiledY = false;

        int image_count = 0;
        YString sItem;
        YString sValue;
        LPTSTR pstr = NULL;
        for( int i = 0; i < 2; ++i,image_count = 0 )
        {
            if(strImg.empty())
                continue;
            while( *pStrImage != _T('\0') ) {
                sItem.clear();
                sValue.clear();
                while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
                while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
                    LPTSTR pstrTemp = ::CharNext(pStrImage);
                    while( pStrImage < pstrTemp) {
                        sItem += *pStrImage++;
                    }
                }
                while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
                if( *pStrImage++ != _T('=') ) break;
                while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
                if( *pStrImage++ != _T('\'') ) break;
                while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
                    LPTSTR pstrTemp = ::CharNext(pStrImage);
                    while( pStrImage < pstrTemp) {
                        sValue += *pStrImage++;
                    }
                }
                if( *pStrImage++ != _T('\'') ) break;
                if( !sValue.empty() ) {
                    if( sItem == _T("file") || sItem == _T("res") ) {
                        if( image_count > 0 )
                        {
                            assert(0&&"should not be here");
                        }
                        sImageName = sValue;
                        if( sItem == _T("file") )
                            ++image_count;
                    }
                    else if( sItem == _T("restype") ) {
                        if( image_count > 0 )
                        {
                            assert(0&&"should not be here");
                        }
                        sImageResType = sValue;
                        ++image_count;
                    }
                    else if( sItem == _T("dest") ) {
                        rcItem.left = rc.left + _tcstol(sValue.c_str(), &pstr, 10);  assert(pstr);    
                        rcItem.top = rc.top + _tcstol(pstr + 1, &pstr, 10);    assert(pstr);
                        rcItem.right = rc.left + _tcstol(pstr + 1, &pstr, 10);  assert(pstr);
                        if (rcItem.right > rc.right) rcItem.right = rc.right;
                        rcItem.bottom = rc.top + _tcstol(pstr + 1, &pstr, 10); assert(pstr);
                        if (rcItem.bottom > rc.bottom) rcItem.bottom = rc.bottom;
                    }
                    else if( sItem == _T("source") ) {
                        rcBmpPart.left = _tcstol(sValue.c_str(), &pstr, 10);  assert(pstr);    
                        rcBmpPart.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
                        rcBmpPart.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
                        rcBmpPart.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);  
                    }
                    else if( sItem == _T("corner") ) {
                        rcCorner.left = _tcstol(sValue.c_str(), &pstr, 10);  assert(pstr);    
                        rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    assert(pstr);    
                        rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  assert(pstr);    
                        rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); assert(pstr);
                    }
                    else if( sItem == _T("mask") ) {
                        if( sValue[0] == _T('#')) dwMask = _tcstoul(sValue.c_str() + 1, &pstr, 16);
                        else dwMask = _tcstoul(sValue.c_str(), &pstr, 16);
                    }
                    else if( sItem == _T("fade") ) {
                        bFade = (BYTE)_tcstoul(sValue.c_str(), &pstr, 10);
                    }
                    else if( sItem == _T("hole") ) {
                        bHole = (_tcscmp(sValue.c_str(), _T("true")) == 0);
                    }
                    else if( sItem == _T("xtiled") ) {
                        bTiledX = (_tcscmp(sValue.c_str(), _T("true")) == 0);
                    }
                    else if( sItem == _T("ytiled") ) {
                        bTiledY = (_tcscmp(sValue.c_str(), _T("true")) == 0);
                    }
                }
                if( *pStrImage++ != _T(' ') ) break;
            }
        }
        canvas.DrawBitmap(sImageName,rcItem,rcBmpPart,((float)dwMask)/255.0f);
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
            

        YYRECT invalidateRc = m_rcItem;
        //!!FIX ME
        //if( ::IsRectEmpty(&invalidateRc) ) 
            invalidateRc = rc;

        m_rcItem = rc;
        if( m_pManager == NULL ) return;
        
        // FIX ME!! 在这里处理子窗口改变的消息
       /* if( !m_bSetPos ) {
            m_bSetPos = true;
            if( OnSize ) OnSize(shared_from_this());
            m_bSetPos = false;
        }*/

        if( m_bFloat ) {
            auto pParent = GetParent();
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

        auto pParent = this;
        RECT rcTemp;
        RECT rcParent;
        while( pParent = pParent->GetParent() )
        {
            rcTemp = invalidateRc;
            rcParent = pParent->GetPos();
            //!!FIX ME
           // if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
           // {
           //     return;
           // }
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
        if( !bVisible && m_pManager && m_pManager->GetFocus()== this)
        {
            m_pManager->SetFocus(nullptr);
        }
        if( IsVisible() != v)
            NeedParentUpdate();
    }

    void ControlUI::SetInternVisible(bool bVisible /*= true*/)
    {
        m_bInternVisible = bVisible;
        if( !bVisible && m_pManager && m_pManager->GetFocus() ==this)
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
            m_pManager->SetFocus(this);
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

    ControlUI* ControlUI::FindControlFromPoint(POINT pt,UINT flag)
    {
        if( !IsVisible() ) 
            return NULL;
        if( !IsEnabled() ) 
            return NULL;
        //!!fix me 
        return ::PtInRect(&m_rcItem,pt)? this : NULL;
    }

    void ControlUI::Invalidate()
    {
        if( !IsVisible() ) return;

        RECT invalidateRc = m_rcItem;

        auto pParent = this;
        RECT rcTemp;
        RECT rcParent;
        while( pParent = pParent->GetParent() )
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
        auto spParent = GetParent();
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
    void ControlUI::Init()
    {
    }

    void ControlUI::SetAttribute(const std::string &strName, const std::string& strValue)
    {
        auto pstrName = strName.c_str();
        auto pstrValue = strValue.c_str();
        if( strcmp(pstrName, ("pos")) == 0 ) {
            RECT rcPos = { 0 };
            LPSTR pstr = NULL;
            rcPos.left = strtol(pstrValue, &pstr, 10);  assert(pstr);    
            rcPos.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
            rcPos.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
            rcPos.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);    
            SIZE szXY = {rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom};
            SetFixedXY(szXY);
            SetFixedWidth(rcPos.right - rcPos.left);
            SetFixedHeight(rcPos.bottom - rcPos.top);
        }
        else if( strcmp(pstrName, ("relativepos")) == 0 ) {
            SIZE szMove,szZoom;
            LPSTR pstr = NULL;
            szMove.cx = strtol(pstrValue, &pstr, 10);  assert(pstr);    
            szMove.cy = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
            szZoom.cx = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
            szZoom.cy = strtol(pstr + 1, &pstr, 10); assert(pstr); 
            SetRelativePos(szMove,szZoom);
        }
        else if( strcmp(pstrName, ("padding")) == 0 ) {
            RECT rcPadding = { 0 };
            LPSTR pstr = NULL;
            rcPadding.left = strtol(pstrValue, &pstr, 10);  assert(pstr);    
            rcPadding.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
            rcPadding.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
            rcPadding.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);    
            SetPadding(rcPadding);
        }
        else if( strcmp(pstrName, ("bkcolor")) == 0 || strcmp(pstrName, ("bkcolor1")) == 0 ) {
            while( *pstrValue > ('\0') && *pstrValue <= (' ') ) pstrValue = ::CharNextA(pstrValue);
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetBkColor(clrColor);
        }
        else if( strcmp(pstrName, ("bkcolor2")) == 0 ) {
            while( *pstrValue > ('\0') && *pstrValue <= (' ') ) pstrValue = ::CharNextA(pstrValue);
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetBkColor2(clrColor);
        }
        else if( strcmp(pstrName, ("bkcolor3")) == 0 ) {
            while( *pstrValue > ('\0') && *pstrValue <= (' ') ) pstrValue = ::CharNextA(pstrValue);
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetBkColor3(clrColor);
        }
        else if( strcmp(pstrName, ("bordercolor")) == 0 ) {
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetBorderColor(clrColor);
        }
        else if( strcmp(pstrName, ("focusbordercolor")) == 0 ) {
            if( *pstrValue == ('#')) pstrValue = ::CharNextA(pstrValue);
            LPSTR pstr = NULL;
            DWORD clrColor = strtoul(pstrValue, &pstr, 16);
            SetFocusBorderColor(clrColor);
        }
        else if( strcmp(pstrName, ("colorhsl")) == 0 ) {}
        else if( strcmp(pstrName, ("bordersize")) == 0 ) {
            std::string nValue = pstrValue;
            if(nValue.find((',')) < 0)
            {
                SetBorderSize(atoi(pstrValue));
                RECT rcPadding = {0};
                SetBorderSize(rcPadding);
            }
            else
            {
                RECT rcPadding = { 0 };
                LPSTR pstr = NULL;
                rcPadding.left = strtol(pstrValue, &pstr, 10);  assert(pstr);
                rcPadding.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);
                rcPadding.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);
                rcPadding.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);
                SetBorderSize(rcPadding);
            }
        }
        else if( strcmp(pstrName, ("leftbordersize")) == 0 ) SetLeftBorderSize(atoi(pstrValue));
        else if( strcmp(pstrName, ("topbordersize")) == 0 ) SetTopBorderSize(atoi(pstrValue));
        else if( strcmp(pstrName, ("rightbordersize")) == 0 ) SetRightBorderSize(atoi(pstrValue));
        else if( strcmp(pstrName, ("bottombordersize")) == 0 ) SetBottomBorderSize(atoi(pstrValue));
        else if( strcmp(pstrName, ("borderstyle")) == 0 ) SetBorderStyle(atoi(pstrValue));
        else if( strcmp(pstrName, ("borderround")) == 0 ) {
            SIZE cxyRound = { 0 };
            LPSTR pstr = NULL;
            cxyRound.cx = strtol(pstrValue, &pstr, 10);  assert(pstr);    
            cxyRound.cy = strtol(pstr + 1, &pstr, 10);    assert(pstr);     
        }
        else if( strcmp(pstrName, ("bkimage")) == 0 )   SetBkImage(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("width")) == 0 ) SetFixedWidth(atoi(pstrValue));
        else if( strcmp(pstrName, ("height")) == 0 ) SetFixedHeight(atoi(pstrValue));
        else if( strcmp(pstrName, ("minwidth")) == 0 ) SetMinWidth(atoi(pstrValue));
        else if( strcmp(pstrName, ("minheight")) == 0 ) SetMinHeight(atoi(pstrValue));
        else if( strcmp(pstrName, ("maxwidth")) == 0 ) SetMaxWidth(atoi(pstrValue));
        else if( strcmp(pstrName, ("maxheight")) == 0 ) SetMaxHeight(atoi(pstrValue));
        else if( strcmp(pstrName, ("name")) == 0 ) SetName(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("text")) == 0 ) SetText(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("tooltip")) == 0 )SetToolTip(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("userdata")) == 0 ) SetUserData(UTF8ToGBK(pstrValue));
        else if( strcmp(pstrName, ("enabled")) == 0 ) SetEnabled(strcmp(pstrValue, ("true")) == 0);
        else if( strcmp(pstrName, ("mouse")) == 0 ) SetMouseEnabled(strcmp(pstrValue, ("true")) == 0);
        else if( strcmp(pstrName, ("keyboard")) == 0 ) SetKeyboardEnabled(strcmp(pstrValue, ("true")) == 0);
        else if( strcmp(pstrName, ("visible")) == 0 ) SetVisible(strcmp(pstrValue, ("true")) == 0);
        else if( strcmp(pstrName, ("float")) == 0 ) SetFloat(strcmp(pstrValue, ("true")) == 0);
        else if( strcmp(pstrName, ("shortcut")) == 0 ) SetShortcut(pstrValue[0]);
        else if( strcmp(pstrName, ("menu")) == 0 ) SetContextMenuUsed(strcmp(pstrValue, ("true")) == 0);
        else if( strcmp(pstrName, ("virtualwnd")) == 0 )  SetVirtualWnd(UTF8ToGBK(pstrValue).c_str());

    }

    ControlUI* ControlUI::ApplyAttributeList(const std::string & strList)
    {
        std::string sItem;
        std::string sValue;
        auto pstrList = strList.c_str();
        while( *pstrList != _T('\0') ) {
            sItem.clear();
            sValue.clear();
            while( *pstrList != _T('\0') && *pstrList != _T('=') ) {
                LPSTR pstrTemp = ::CharNextA(pstrList);
                while( pstrList < pstrTemp) {
                    sItem += *pstrList++;
                }
            }
            assert( *pstrList == _T('=') );
            if( *pstrList++ != _T('=') ) 
                return this;
            assert( *pstrList == _T('\"') );
            if( *pstrList++ != _T('\"') )
                return this;
            while( *pstrList != _T('\0') && *pstrList != _T('\"') ) {
                LPSTR pstrTemp = ::CharNextA(pstrList);
                while( pstrList < pstrTemp) {
                    sValue += *pstrList++;
                }
            }
            assert( *pstrList == _T('\"') );
            if( *pstrList++ != _T('\"') ) 
                return this;
            SetAttribute(sItem, sValue);
            if( *pstrList++ != _T(' ') )
                return this;
        }
        return this;
    }

    SIZE ControlUI::EstimateSize(SIZE szAvailable)
    {
        return m_cXYFixed;
    }

   

    void ControlUI::DoPaint(const YYRECT &rc)
    {
        RECT rcPaint = rc;
         if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) 
             return;

        // 绘制循序：背景颜色->背景图->状态图->文本->边框
            PaintBkColor();
            PaintBkImage();
            PaintStatusImage();
            PaintText();
            PaintBorder();
    }

    void ControlUI::PaintBkColor()
    {
        Canvas2D canvas(m_pManager->GetHWND());
        canvas.FillRect(m_rcItem,m_dwBackColor);
    }

    void ControlUI::PaintBkImage()
    {
        /*Canvas2D canvas(m_pManager->GetHWND());
        if(m_strBKImage.empty())
            return;
        canvas.DrawBitmap(m_strBKImage,m_rcItem);*/
        if(m_strBKImage.empty())
            return;
        DrawImage(m_strBKImage);
    }

    void ControlUI::PaintStatusImage()
    {
        return ;
    }

    void ControlUI::PaintText()
    {
        return;
    }

    void ControlUI::PaintBorder()
    {
        Canvas2D canvas(m_pManager->GetHWND());
        if(IsFocused() && m_dwFocusBorderColor != 0 && m_nBorderSize >0)
            canvas.DrawRect(m_rcItem,m_dwFocusBorderColor,m_nBorderSize);
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

    ControlUI* ControlUI::FindControlFromName(const YString & strName)
    {
        if(strName == GetName() )
            return this;
        else
            return nullptr;
    }

     void ControlUI::AddHandler()
    {
        m_ControlMsgHandler.AddEntry(UIMSG_SETCURSOR,[&](const MsgWrap &msg)
        {
             ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
        });

        m_ControlMsgHandler.AddEntry(UIMSG_SETFOCUS,[&](const MsgWrap &msg)
        {
            m_bFocused = true;
            Invalidate();
        });

        m_ControlMsgHandler.AddEntry(UIMSG_KILLFOCUS,[&](const MsgWrap &msg)
        {
            m_bFocused = false;
            Invalidate();
        });
       /* m_pControlMsgHandler->AddEntry(UIMSG_MOUSEENTER,[&](const MsgWrap &msg)
        {
            m_pManager->SetCapture();
            Invalidate();
        });*/
        /*
        m_pControlMsgHandler->AddEntry(UIMSG_MOUSELEAVE,[&](const MsgWrap &msg)
        {
            m_pManager->ReleaseCapture();
            Ycout<<GetName()<<_T(" release capture!!!")<<endl;
            Invalidate();
        });*/
    }

     void ControlUI::HandleMsg(const MsgWrap & msg) throw()
     {
         m_ControlMsgHandler.HandleMsg(msg);
     }




}