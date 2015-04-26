//////////////////////////////////////////////////////////////////////////
// Create by yyCom, 2015/4/10
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "YUI.h"
#include "UIUtility.h"
#include <commctrl.h>
#include "UIDef.h"

namespace YUI
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

    typedef std::function<ControlUI *(const YString &) > funCreateControl;
    typedef ControlUI* (*pfnCreateControl)(const YString &);


    class PaintManagerUI:public std::enable_shared_from_this<PaintManagerUI>
    {
    public:    
        PaintManagerUI();
        ~PaintManagerUI();

    public:
        void                            Init(HWND hWnd);
        void                            NeedUpdate();
        void                            Invalidate(const RECT& rcItem);
        
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

        DWORD                           GetDefaultDisabledColor() const;
        void                            SetDefaultDisabledColor(DWORD dwColor);
        DWORD                           GetDefaultFontColor() const;
        void                            SetDefaultFontColor(DWORD dwColor);
        DWORD                           GetDefaultLinkFontColor() const;
        void                            SetDefaultLinkFontColor(DWORD dwColor);
        DWORD                           GetDefaultLinkHoverFontColor() const;
        void                            SetDefaultLinkHoverFontColor(DWORD dwColor);
        DWORD                           GetDefaultSelectedBkColor() const;
        void                            SetDefaultSelectedBkColor(DWORD dwColor);

        void                            ReloadAllImages();
        bool                            SetNextTabControl(bool bForward= true);
        std::shared_ptr<ControlUI>      FindControl(POINT pt) const;
        std::shared_ptr<ControlUI>      FindControl(const YString & strControlName);
        bool                            MessageHandler(UINT uMesg, WPARAM wParam, LPARAM lParam, LRESULT &lRes);
        void                            SendNotify(NotifyMsg &Msg, bool bAsync = false );
        void                            SendNotify(std::shared_ptr<ControlUI>& spControl,YString strMessage,WPARAM wParam =0, LPARAM lParam =0,bool bAsync = false );
        void                            SetFocus(std::shared_ptr<ControlUI> &pControl);
        void                            SetCapture();
        void                            ReleaseCapture();
        bool                            TranslateAccelerator(LPMSG pMsg);
        bool                            InitControls(std::shared_ptr<ControlUI> &pControl,std::weak_ptr<ControlUI> parent);
    public:
        bool                            PreMessageHandler(UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT &lRes);
        void                            AddPreMessageFilter(std::shared_ptr<IMessageFilterUI> spFilter );
        std::shared_ptr<ControlUI>      GetFocus() const;
        void                            SetFocus(std::shared_ptr<ControlUI> pControl);
        void                            SetFocusNeeded(std::shared_ptr<ControlUI> pControl);

        std::shared_ptr<const ImageInfo>GetImage(const YString &bitmap);
        std::shared_ptr<const ImageInfo>GetImageEx(const YString &bitmap, LPCTSTR type = NULL, DWORD mask = 0);
        std::shared_ptr<ImageInfo>      AddImage(const YString &bitmap, LPCTSTR type = NULL, DWORD mask = 0);
        std::shared_ptr<ImageInfo>      AddImage(const YString& bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha);
        bool                            RemoveImage(const YString& bitmap);
        void                            RemoveAllImages();

        std::shared_ptr<FontInfo>       GetDefaultFontInfo();
        void                            SetDefaultFont(const YString &StrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
        DWORD                           GetCustomFontCount() const;
        HFONT                           AddFont(const YString &StrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
        HFONT                           GetFont(const YString &StrFontName);
        std::shared_ptr<FontInfo>       GetFontInfo(HFONT hFont);
        bool                            RemoveFont(HFONT hFont);
        void                            RemoveAllFonts();

        std::shared_ptr<ControlUI>      GetRoot() const;
		bool							AttachDialog(std::shared_ptr<ControlUI> pControl);
		bool							AddNotifier(std::shared_ptr<INotifyUI> pControl);
        int                             GetPostPaintCount() const;
        bool                            AddPostPaint(std::shared_ptr<ControlUI> pControl);
        bool                            RemovePostPaint(std::shared_ptr<ControlUI> pControl);
        bool                            SetPostPaintIndex(std::shared_ptr<ControlUI> pContro);
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
        static void                     SetResourceZip(const YString &, bool bCachedResourceZip = false);
        static void                     ReloadSkin();
        static bool                     LoadPlugin(const YString & strModuleName);
        static UINT                     MapKeyState();
        static void                     MessageLoop();
        static YString                  GetResourcePath() ;


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
        
        std::shared_ptr<ControlUI>      m_pRoot;
        std::shared_ptr<ControlUI>      m_pFocus;
        std::shared_ptr<ControlUI>      m_pEventHover;
        std::shared_ptr<ControlUI>      m_pEventClick;
        std::shared_ptr<ControlUI>      m_pEventKey;

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
        std::vector<TimerInfo>          m_vecTimers;
        std::vector<std::weak_ptr<IMessageFilterUI>>
                                        m_vecPreMessageFilers;
        std::vector<std::weak_ptr<IMessageFilterUI>>             
                                        m_vecMessageFilters;
        //have bugs!!! 
        std::vector<std::shared_ptr<ControlUI> >              
                                        m_vecPostPaintControls;
        std::vector<void*>              m_vecDelayedCleanup;
        std::list<NotifyMsg>            m_ListAsyncNotify;
        std::vector<void*>              m_vecFoundCountrols;

        std::map<YString,void*>         m_mapNameHash;
        std::map<YString,void*>         m_mapOptionGroup;

        std::weak_ptr<PaintManagerUI>   m_wpParentResourcePM;
        DWORD                           m_dwDefaultDisabledColor;
        DWORD                           m_dwDefaultFontColor;
        DWORD                           m_dwDefaultLinkFontColor;
        DWORD                           m_dwDefaultLinkHoverFontColor;
        DWORD                           m_dwDefaultSelectedBKColor;
        std::shared_ptr<FontInfo>       m_DefaultFontInfo;
        std::map<YString,std::shared_ptr<FontInfo> >              
                                        m_mapCustomFonts;

        std::map<YString,std::shared_ptr<ImageInfo> > 
                                        m_mapImageHash;

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
        static std::vector< std::weak_ptr<PaintManagerUI> >       
                                        g_vecPreMessages;
#if 0
        static std::unordered_set<funCreateControl>        g_setPlugins;  
        static std::unordered_set<funCreateControl>  & GetPlugins();
#endif
    public:
        static YString                  g_strDefaultFontName;
        static  std::vector<std::weak_ptr<ITranslateAccelerator> >      
                                        g_vecTranslateAccelerator;
    };
}