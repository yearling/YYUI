#pragma once

#include "YUI.h"
#include "WindowWnd.h"
#include "UIDef.h"
#include "DlgBuilder.h"
#include "PaintManagerUI.h"

namespace YUI
{
    enum UILIB_RESOURCETYPE
    {
        UILIB_FILE=1,				    // 来自磁盘文件
        UILIB_ZIP,						// 来自磁盘zip压缩包
        UILIB_RESOURCE,			        // 来自资源
        UILIB_ZIPRESOURCE,	            // 来自资源的zip压缩包
    };

    class WindowImpl 
        : public WindowWnd
        , public INotifyPump
        , public INotifyUI
        , public IMessageFilterUI
        , public IDialogBuilderCallback
        , public std::enable_shared_from_this<WindowImpl>
    {
    public:
                                        WindowImpl();
        virtual                         ~WindowImpl();
        virtual void                    InitWindow();
        virtual void                    OnFinalMessage(HWND hWnd);
        virtual void                    Notify(NotifyMsg & msg);
        virtual void                    OnClick(NotifyMsg & msg);
        virtual std::shared_ptr<ControlUI> 
                                        CreateControl(YString pstrClass);
    protected:
        virtual YString                 GetSkinFolder() = 0 ;
        virtual YString                 GetSkinFile() =0 ;
        virtual LPCTSTR                 GetWindowClassName(void) const= 0;
        virtual LRESULT                 ResponseDefaultKeyEvent(WPARAM wParam);

        std::shared_ptr<PaintManagerUI> m_spPaintManager;

    public:
        virtual UINT                    GetClassStyle() const;
        virtual UILIB_RESOURCETYPE      GetResourceType() const;
        virtual YString                 GetZIPFileName() const;
        virtual YString                 GetResourceID() const;
        virtual std::shared_ptr<ControlUI> CreateControl(const YString & strClassName);
        virtual LRESULT                 MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/);
        virtual LRESULT                 OnClose(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
        virtual LRESULT                 OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
        virtual LRESULT                 OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
        virtual LRESULT                 OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT                 OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
        virtual LRESULT                 OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT                 OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT                 OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
        virtual LRESULT                 OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT                 OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT                 OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT                 OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT                 OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LRESULT                 OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
        virtual LRESULT                 OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
        virtual LRESULT                 OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
        virtual LRESULT                 OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
        virtual LRESULT                 OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
        virtual LRESULT                 OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
        virtual LRESULT                 HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        virtual LRESULT                 HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        virtual LONG                    GetStyle();              

    };

}