//////////////////////////////////////////////////////////////////////////
// Create by yyCom, 2015/4/10
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "YYUI.h"
#include "YYUtility.h"
#include <commctrl.h>
#include "YYUIDef.h"

namespace YYCOM
{
    

    class INotifyUI
    {
    public:
        virtual void                    Notify(NotifyMsg &msg) = 0;
    };

    class IMessageFilterUI
    {
    public:
        virtual LRESULT                 MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) = 0 ;
    };

    class ITranslateAccelerator
    {
    public:
        virtual LRESULT                 TranslateAccelerator(MSG *pMsg) = 0;
    };

    typedef std::function<YControlUI *(const YString &) > funCreateControl;
    typedef YControlUI* (*pfnCreateControl)(const YString &);


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
        HWND                            GetPaintWindow() const;
        HWND                            GetTooltipWindow() const;
        POINT                           GetMousePos() const;
        SIZE                            GetClientSize() const;
        SIZE                            GetInitSize() const;
        void                            SetInitSize(int cx, int cy);
        const RECT&                     GetSizeBox() const ;
        void                            SetSizeBox(const RECT &rcSizeBox);
        const RECT&                     GetCaptionRect() const ;
        void                            SetCaptionRect( const RECT & rcCaption);
        SIZE                            GetRoundCorner() const;
        void                            SetRoundCorner(int cx, int cy);
        SIZE                            GetMinInfo() const;
        void                            SetMinInfo(int cx, int cy);
        SIZE                            GetMaxInfo() const;
        void                            SetMaxInfo(int cx, int cy);
        int                             GetTransparent() const;
        void                            SetTransparent(int nOpacity);
        void                            SetBackgroundTransparent(bool bTrans);
        bool                            IsShowUpdateRect() const;
        void                            SetShowUpdateRect(bool bShow);
        void                            ReloadAllImages();
        bool                            SetNextTabControl(bool bForward= true);

        bool                            MessageHandler(UINT uMesg, WPARAM wParam, LPARAM lParam, LRESULT &lRes);
        void                            SendNotify(NotifyMsg &Msg, bool bAsync = false );
        void                            SendNotify(std::shared_ptr<YControlUI>& spControl,YString strMessage,WPARAM wParam =0, LPARAM lParam =0,bool bAsync = false );
    public:
        bool                            PreMessageHandler(UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT &lRes);
    public:
        static void                     SetInstance(HINSTANCE hInst);
        static YString                  GetInstancePath();
        static YString                  GetCurrentPath();
        static HINSTANCE                GetInstance();
        static HINSTANCE                GetResourceDll();
        static const YString &          GetSourcePath();
        static const YString &          GetResourceZip();
        static bool                     IsCachedResourceZip();
        static HANDLE                   GetResourceZipHandle();
        static void                     SetCurrentPath(const YString &strPath);
        static void                     SetResourceDll(HINSTANCE hInstance);
        static void                     SetResourcePath(const YString &strPath);
        static void                     SetResoucePath(const YString &strPath);
        static void                     SetResourceZip(void);
        static void                     ReloadSkin();
        static bool                     LoadPlugin(const YString & strModuleName);






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
        std::weak_ptr<YControlUI>       m_pFocus;
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

        std::vector<std::weak_ptr<INotifyUI>>            
                                        m_vecNotifiers;
        std::vector<YTimer>              m_vecTimers;
        std::vector<void*>              m_vecPreMessageFilers;
        std::vector<std::weak_ptr<IMessageFilterUI>>             
                                        m_vecMessageFilters;
        std::vector<std::weak_ptr<YControlUI> >              
                                        m_vecPostPaintControls;
        std::vector<void*>              m_vecDelayedCleanup;
        std::list<NotifyMsg>            m_ListAsyncNotify;
        std::vector<void*>              m_vecFoundCountrols;

        std::map<YString,void*>         m_mapNameHash;
        std::map<YString,void*>         m_mapOptionGroup;

        std::weak_ptr<YPaintManagerUI>  m_wpParentResourcePM;
        DWORD                           m_dwDefaultDisabledColor;
        DWORD                           m_dwDefaultFontColor;
        DWORD                           m_dwDefaultLinkFontColor;
        DWORD                           m_dwDefaultLinkHoverFontColor;
        DWORD                           m_dwDefaultSelectedBKColor;
        tagTFontInfo                    m_DefaultFontInfo;
        std::vector<void*>              m_vecCustomFonts;

        std::map<YString,void*>         m_mapImageHash;
        std::map<YString,void*>         m_DefaultAttrHash;

        static HINSTANCE                g_hInstance;
        static HINSTANCE                g_hResourceInstance;
        static YString                  g_strResourcePath;
        static YString                  g_strResourceZip;
        static bool                     g_bCachedResourceZip;
        static HANDLE                   g_hResourceZip;
        static short                    g_H;
        static short                    g_S;
        static short                    g_L;
        static std::vector< std::weak_ptr<YPaintManagerUI> >       
                                        g_vecPreMessages;
#if 0
        static std::unordered_set<funCreateControl>        g_setPlugins;  
        static std::unordered_set<funCreateControl>  & GetPlugins();
#endif
    public:
        static YString                  g_strDefaultFontName;
        static  std::vector<void*>      g_vecTranslateAccelerator;
    };
}