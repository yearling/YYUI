#pragma once

#include "YUI.h"
#include <functional>
#include "ControlManager.h"
#include "MessageHandler.h"
namespace YUI
{

    class ControlUI:public CountRefObj
    {
    public:
        ControlUI();
        virtual ~ControlUI();
    public:
        virtual YString                 GetName() const;
        virtual void                    SetName(const YString & strName);
        virtual void					HandleMsg(const MsgWrap & msg)throw();
        virtual LPCTSTR                 GetClass() const;
        virtual bool                    Activate();
        virtual ControlManager*         GetManager() const;
        virtual void                    SetManager(ControlManager* pManager, ControlUI* pParent,bool bInit=true);
        virtual ControlUI*              GetParent() const;
        
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
        void                            DrawImage(const YString &strImg);

        //边框
        float                           GetBorderSize() const;
        void                            SetBorderSize(float nSize);
        DWORD                           GetBorderColor() const;
        void                            SetBorderColor(DWORD dwBorderColor);
        void                            SetBorderSize(YYRECT rc);
        float                           GetLeftBorderSize() const;
        void                            SetLeftBorderSize(float nSize);
        float                           GetTopBorderSize() const;
        void                            SetTopBorderSize(float nSize);
        float                           GetRightBorderSize() const;
        void                            SetRightBorderSize(float nSize);
        float                           GetBottomBorderSize() const;
        void                            SetBottomBorderSize(float nSize);
        int                             GetBorderStyle() const;
        void                            SetBorderStyle(int nStyle);


        // 位置相关
        virtual YYRECT                  GetPos() const;
        virtual float                   GetWidth() const;
        virtual float                   GetHeight() const;
        virtual float                   GetX() const;
        virtual float                   GetY() const;
        virtual YYRECT                  GetPadding() const;
        virtual void                    SetPadding(YYRECT rcPadding); // 设置外边距，由上层窗口绘制
        virtual YYSIZE                  GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
        virtual void                    SetFixedXY(YYSIZE szXY);      // 仅float为true时有效
        virtual float                   GetFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
        virtual void                    SetFixedWidth(float cx);      // 预设的参考值
        virtual float                   GetFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
        virtual void                    SetFixedHeight(float cy);     // 预设的参考值
        virtual float                   GetMinWidth() const;
        virtual void                    SetMinWidth(float cx);
        virtual float                   GetMaxWidth() const;
        virtual void                    SetMaxWidth(float cx);
        virtual float                   GetMinHeight() const;
        virtual void                    SetMinHeight(float cy);
        virtual float                   GetMaxHeight() const;
        virtual void                    SetMaxHeight(float cy);
        virtual void                    SetRelativePos(YYSIZE szMove,YYSIZE szZoom);
        virtual void                    SetRelativeParentSize(YYSIZE sz);
        virtual RelativePosUI           GetRelativePos() const;
        virtual bool                    IsRelativePos() const;



        virtual void                    SetPos(YYRECT &rc);

        virtual UINT                    GetControlFlags() const;

        // 鼠标提示
        virtual YString                 GetToolTip() const;
        virtual void                    SetToolTip(const YString& pstrText);
        virtual void                    SetToolTipWidth(float nWidth);
        virtual float                   GetToolTipWidth(void);	// 多行ToolTip单行最长宽度

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
        //坐标转换
        YYPOINT                         ClipToLocal(const YYPOINT &ptInClient);
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
        virtual ControlUI*              FindControlFromPoint(POINT pt,UINT flag);
        virtual ControlUI*              FindControlFromName(const YString & strName);
        void                            Invalidate();
        bool                            IsUpdateNeeded() const;
        void                            NeedUpdate();
        void                            NeedParentUpdate();

        virtual void                    Init();

        void                            AddHandler();
        virtual void                    SetAttribute(const std::string &strName, const std::string& strValue);
        ControlUI*                      ApplyAttributeList(const std::string & strList);

        virtual YYSIZE                  EstimateSize(YYSIZE szAvailable);

        virtual void                    DoPaint(const YYRECT &rc);
        virtual void                    PaintBkColor();
        virtual void                    PaintBkImage();
        virtual void                    PaintStatusImage();
        virtual void                    PaintText();
        virtual void                    PaintBorder();

        virtual void                    DoPostPaint(HDC hDC, const YYRECT& rcPaint);

        //虚拟窗口参数
        void                            SetVirtualWnd(LPCTSTR pstrValue);
        YString                         GetVirtualWnd() const;
    protected:
        CountRefPtr<ControlManager>     m_pManager;
        ControlUI*                      m_pParent;
        YString                         m_strVirtualWnd;
        YString                         m_strName; // tag 'name'
        bool                            m_bUpdateNeeded;
        bool                            m_bMenuUsed;
        YYRECT                          m_rcPadding; //tag 'padding'
        YYSIZE                          m_cXY; // maybe control 左上角的点  tag 'pos'
        YYSIZE                          m_cXYFixed; //fix宽高 tag 'pos' 'width' 'height'
        YYSIZE                          m_cxyMin;// tag 'minwidth' 'minheight'
        YYSIZE                          m_cxyMax;//tag 'maxwidth' 'maxheight'
        YYRECT                          m_rcItem;

        
        float                           m_nTooltipWidth;
        bool                            m_bVisible;// tag 'visible'
        bool                            m_bInternVisible;
        bool                            m_bEnable; // tag 'endable'
        bool                            m_bMouseEnabled; // tag 'mouse'
        bool                            m_bKeyboardEnabled; // tag 'keyboard'
        bool                            m_bFocused; //
        bool                            m_bFloat; // tag 'float'
        bool                            m_bSetPos; //防止SetPos循环调用
        RelativePosUI                   m_RelativePos; //相对于父节点的坐标 tag 'relativepos'

        YString                         m_strText;  //文本，对应'text'标签
        YString                         m_strToolTip; // tag 'tooltip'
        TCHAR                           m_chShortcut; // tag 'shortcut'
        YString                         m_strUserData;// tag 'userdata'
        UINT_PTR                        m_pTag; 

        DWORD                           m_dwBackColor; // tag 'bkcolor' 'bkcolor'
        DWORD                           m_dwBackColor2;//tag 'bkcolor2'
        DWORD                           m_dwBackColor3; //tag 'bkcolor3'
        YString                         m_strBKImage; //tag 'bkimage'
        YString                         m_strForeImage;
        DWORD                           m_dwBorderColor; //tag 'bordercolor'
        DWORD                           m_dwFocusBorderColor;//tag 'focusbordercolor'
        float                           m_nBorderSize;//tag 'boradersize'  
        int                             m_nBorderStyle; // tag 'borderstyple'
        YYRECT                          m_rcPaint; 
        YYRECT                          m_rcBorderSize;//tag 'boradersize' 'leftbordersize' 'topboradersize' 'bottombordersize' 
    protected:
        MsgHandler					    m_ControlMsgHandler;
    };
}