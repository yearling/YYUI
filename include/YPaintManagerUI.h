//////////////////////////////////////////////////////////////////////////
// Create by yyCom, 2015/4/10
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "YYUI.h"
#include <commctrl.h>
#include "YYUIDef.h"

namespace YYCOM
{

    class YPaintManagerUI:public std::enable_shared_from_this<YPaintManagerUI>
    {
    public:    
        YPaintManagerUI();
        ~YPaintManagerUI();

    public:
        void                            Init(HWND hWnd);
        void                            NeedUpdate();
        void                            Invalidate(RECT& rcItem);
        
        HDC                             GetPaintDC() const;

    public:
        static void                     SetInstance(HINSTANCE hInst);
        static std::string              GetInstancePath();
        static std::string              GetCurrentPath();
        static HINSTANCE                GetInstance();
        static bool                     TranslateMessage(const LPMSG pMsg);
         
    private:
        HWND                            m_hWndPaint;
        int                             m_nOpacity;
        HDC                             m_hDCPaint;
        HDC                             m_hDCOffscreen;
        HDC                             m_hDCBackground;
        HBITMAP                         m_hbmpOffscreen;
        HBITMAP                         m_hbmpBackground;
        HWND                            m_hwndTooltip;
        TOOLINFO                        m_ToolTip;
        bool                            m_bShowUpdateRect;
        
        std::shared_ptr<YControlUI>     m_pRoot;
        std::shared_ptr<YControlUI>     m_pFocus;
        std::shared_ptr<YControlUI>     m_pEventHover;
        std::shared_ptr<YControlUI>     m_pEventClick;
        std::shared_ptr<YControlUI>     m_pEventKey;

        POINT                           m_ptLastMousePos;
        SIZE                            m_szMinWindow;
        SIZE                            m_szMaxWindow;
        SIZE                            m_szInitWindowSize;
        RECT                            m_rcSizeBox;
        SIZE                            m_szRoundCorner;
        RECT                            m_rcCaption;
        UINT                            m_uTimerID;

        bool                            m_bFirstLayout;
        bool                            m_bUpdateNeeded;
        bool                            m_bFocusNeeded;
        bool                            m_bOffscreenPaint;
        bool                            m_bAlphaBackGround;
        bool                            m_bMouseTracking;
        bool                            m_bMouseCapture;
        bool                            m_bUsedVirtualWnd;

        std::vector<void*>              m_vecNotifiers;
        std::vector<void*>              m_vecTimers;
        std::vector<void*>              m_vecPreMessageFilers;
        std::vector<void*>              m_vecMessageFilters;
        std::vector<void*>              m_vecPostPaintControls;
        std::vector<void*>              m_vecDelayedCleanup;
        std::vector<void*>              m_vecAsyncNotify;
        std::vector<void*>              m_vecFoundCountrols;

        std::map<std::string,void*>     m_mapNameHash;
        std::map<std::string,void*>     m_mapOptionGroup;

        std::weak_ptr<YPaintManagerUI>  m_wpParentResourcePM;
        DWORD                           m_dwDefaultDisabledColor;
        DWORD                           m_dwDefaultFontColor;
        DWORD                           m_dwDefaultLinkFontColor;
        DWORD                           m_dwDefaultLinkHoverFontColor;
        DWORD                           m_dwDefaultSelectedBKColor;
        tagTFontInfo                    m_DefaultFontInfo;
        std::vector<void*>              m_vecCustomFonts;

        std::map<std::string,void*>     m_mapImageHash;
        std::map<std::string,void*>     m_DefaultAttrHash;

        static HINSTANCE                m_hInstance;
        static HINSTANCE                m_hResourceInstance;
        static std::string              m_strResourcePath;
        static std::string              m_strResourceZip;
        static bool                     m_bCachedResourceZip;
        static HANDLE                   m_hResourceZip;
        static short                    m_H;
        static short                    m_S;
        static short                    m_L;
        static std::vector< std::weak_ptr<YPaintManagerUI> >       
                                        g_vecPreMessages;
        static std::vector<void*>       m_vecPlugins;  
    public:
        static std::string              m_strDefaultFontName;
        static std::vector<void*>       m_vecTranslateAccelerator;
    };
}