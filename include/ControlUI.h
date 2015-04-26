
#pragma once

#include "YUI.h"
#include "UIDef.h"
#include <functional>
namespace YUI
{

    class ControlUI:public std::enable_shared_from_this<ControlUI>
    {
    public:
        ControlUI();
        virtual ~ControlUI();
    public:
        typedef std::function<bool(NotifyMsg &)> FuncNotify;
        typedef std::function<bool(std::weak_ptr<ControlUI>)> FuncInit;
        typedef std::function<bool(std::weak_ptr<ControlUI>)> FuncSize;
        typedef std::function<bool(ControlEvent&)> FuncEvent;
        typedef std::function<std::weak_ptr<ControlUI>(std::weak_ptr<ControlUI>) > funcFindControl;
    public:
        virtual YString                 GetName() const;
        virtual void                    SetName(const YString & strName);
        virtual LPCTSTR                 GetClass() const;
        virtual std::shared_ptr<ControlUI> QueryInterface(const YString & strName);
        virtual bool                    Activate();
        virtual std::shared_ptr<PaintManagerUI>
                                        GetManager() const;
        virtual void SetManager(std::shared_ptr<PaintManagerUI> &pManager,
                                                    std::weak_ptr<ControlUI> pParent,
                                                    bool bInit=true);
        virtual std::weak_ptr<ControlUI>
                                        GetParent() const;
        
        //字体类
        virtual YString                 GetText() const;
        virtual void                    SetText(YString const & strName);

        //图像类
        DWORD                           GetBkColor() const;
        void                            SetBkColor(DWORD dwBackColor);
        DWORD                           GetBkColor2() const;
        void                            SetBkColor2(DWORD dwBackColor);
        DWORD                           GetBkColor3() const;
        void                            SetBkColor3(DWORD dwBackColor);
        YString                         GetBkImage();
        void                            SetBkImage(const YString &strImage);
        DWORD                           GetFocusBorderColor() const;
        void                            SetFocusBorderColor(DWORD dwBorderColor);
        bool                            IsColorHSL() const;
        void                            SetColorHSL(bool bColorHSL);
        SIZE                            GetBorderRound() const;
        void                            SetBorderRound(SIZE cxyRound);
        bool                            DrawImage(HDC hDc, 
                                                  const YString strImg,
                                                  const YString strModify =_T(""));

        //边框
        int                             GetBorderSize() const;
        void                            SetBorderSize(int nSize);
        DWORD                           GetBorderColor() const;
        void                            SetBorderColor(DWORD dwBorderColor);
        void                            SetBorderSize(RECT rc);
        int                             GetLeftBorderSize() const;
        void                            SetLeftBorderSize(int nSize);
        int                             GetTopBorderSize() const;
        void                            SetTopBorderSize(int nSize);
        int                             GetRightBorderSize() const;
        void                            SetRightBorderSize(int nSize);
        int                             GetBottomBorderSize() const;
        void                            SetBottomBorderSize(int nSize);
        int                             GetBorderStyle() const;
        void                            SetBorderStyle(int nStyle);


        // 位置相关
        virtual RECT                    GetPos() const;
        virtual int                     GetWidth() const;
        virtual int                     GetHeight() const;
        virtual int                     GetX() const;
        virtual int                     GetY() const;
        virtual RECT                    GetPadding() const;
        virtual void                    SetPadding(RECT rcPadding); // 设置外边距，由上层窗口绘制
        virtual SIZE                    GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
        virtual void                    SetFixedXY(SIZE szXY);      // 仅float为true时有效
        virtual int                     GetFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
        virtual void                    SetFixedWidth(int cx);      // 预设的参考值
        virtual int                     GetFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
        virtual void                    SetFixedHeight(int cy);     // 预设的参考值
        virtual int                     GetMinWidth() const;
        virtual void                    SetMinWidth(int cx);
        virtual int                     GetMaxWidth() const;
        virtual void                    SetMaxWidth(int cx);
        virtual int                     GetMinHeight() const;
        virtual void                    SetMinHeight(int cy);
        virtual int                     GetMaxHeight() const;
        virtual void                    SetMaxHeight(int cy);
        virtual void                    SetRelativePos(SIZE szMove,SIZE szZoom);
        virtual void                    SetRelativeParentSize(SIZE sz);
        virtual RelativePosUI           GetRelativePos() const;
        virtual bool                    IsRelativePos() const;



        virtual void                    SetPos(RECT &rc);

        virtual UINT                    GetControlFlags() const;

        // 鼠标提示
        virtual YString                 GetToolTip() const;
        virtual void                    SetToolTip(const YString& pstrText);
        virtual void                    SetToolTipWidth(int nWidth);
        virtual int	                    GetToolTipWidth(void);	// 多行ToolTip单行最长宽度

        // 快捷键
        virtual TCHAR                   GetShortcut() const;
        virtual void                    SetShortcut(TCHAR ch);

        // 菜单
        virtual bool                    IsContextMenuUsed() const;
        virtual void                    SetContextMenuUsed(bool bMenuUsed);

        // 用户属性
        virtual YString                 GetUserData(); // 辅助函数，供用户使用
        virtual void                    SetUserData(const YString &strText); // 辅助函数，供用户使用
        virtual UINT_PTR                GetTag() const; // 辅助函数，供用户使用
        virtual void                    SetTag(UINT_PTR pTag); // 辅助函数，供用户使用
    public:
       
        // 一些重要的属性
        virtual bool                    IsVisible() const;
        virtual void                    SetVisible(bool bVisible = true);
        virtual void                    SetInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
        virtual bool                    IsEnabled() const;
        virtual void                    SetEnabled(bool bEnable = true);
        virtual bool                    IsMouseEnabled() const;
        virtual void                    SetMouseEnabled(bool bEnable = true);
        virtual bool                    IsKeyboardEnabled() const;
        virtual void                    SetKeyboardEnabled(bool bEnable = true);
        virtual bool                    IsFocused() const;
        virtual void                    SetFocus();
        virtual bool                    IsFloat() const;
        virtual void                    SetFloat(bool bFloat = true);

        //find
        virtual std::shared_ptr<ControlUI>
                                        FindControlFromPoint(POINT pt,UINT flag);
        virtual std::shared_ptr<ControlUI>&
                                        FindControlFromName(const YString & strName);
        void                            Invalidate();
        bool                            IsUpdateNeeded() const;
        void                            NeedUpdate();
        void                            NeedParentUpdate();
        DWORD                           GetAdjustColor(DWORD dwColor);

        virtual void                    Init();
        virtual void                    DoInit();

        virtual void                    Event(ControlEvent& eve);
        virtual void                    DoEvent(ControlEvent& eve);

        virtual void                    SetAttribute(const YString &pstrName, const YString& pstrValue);
        std::shared_ptr<ControlUI>      ApplyAttributeList(const YString& strList);

        virtual SIZE                    EstimateSize(SIZE szAvailable);

        virtual void                    DoPaint(HDC hDC, const RECT& rcPaint);
        virtual void                    PaintBkColor(HDC hDC);
        virtual void                    PaintBkImage(HDC hDC);
        virtual void                    PaintStatusImage(HDC hDC);
        virtual void                    PaintText(HDC hDC);
        virtual void                    PaintBorder(HDC hDC);

        virtual void                    DoPostPaint(HDC hDC, const RECT& rcPaint);

        //虚拟窗口参数
        void                            SetVirtualWnd(LPCTSTR pstrValue);
        YString                         GetVirtualWnd() const;
    public:
        FuncNotify                      OnNotify;
        FuncInit                        OnInit;
        FuncEvent                       OnEvent;
        FuncSize                        OnSize;
    protected:
        std::shared_ptr<PaintManagerUI> m_pManager;
        std::weak_ptr<ControlUI>        m_pParent;
        YString                         m_strVirtualWnd;
        YString                         m_strName;
        bool                            m_bUpdateNeeded;
        bool                            m_bMenuUsed;
        RECT                            m_rcPadding;
        SIZE                            m_cXY;
        SIZE                            m_cXYFixed;
        SIZE                            m_cxyMin;
        SIZE                            m_cxyMax;
        RECT                            m_rcItem;

        
        int                             m_nTooltipWidth;
        bool                            m_bVisible;
        bool                            m_bInternVisible;
        bool                            m_bEnable;
        bool                            m_bMouseEnabled;
        bool                            m_bKeyboardEnabled;
        bool                            m_bFocused;
        bool                            m_bFloat;
        bool                            m_bSetPos; //防止SetPos循环调用
        RelativePosUI                   m_RelativePos;

        YString                         m_strText;
        YString                         m_strToolTip;
        TCHAR                           m_chShortcut;
        YString                         m_strUserData;
        UINT_PTR                        m_pTag;

        DWORD                           m_dwBackColor;
        DWORD                           m_dwBackColor2;
        DWORD                           m_dwBackColor3;
        YString                         m_strBKImage;
        YString                         m_strForeImage;
        DWORD                           m_dwBorderColor;
        DWORD                           m_dwFocusBorderColor;
        bool                            m_bColorHSL;
        int                             m_nBorderSize;
        int                             m_nBorderStyle;
        SIZE                            m_cxyBorderRound;
        RECT                            m_rcPaint;
        RECT                            m_rcBorderSize;

    };
}