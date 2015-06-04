#pragma once
#include "YUI.h"
#include "ControlUI.h"
#include <set>

namespace YUI
{
    class IContainer
    {
    public:
        virtual int                     GetCount() const = 0;
        virtual bool                    Add(ControlUI* pControl) = 0;

        virtual bool                    Remove(ControlUI* pControl) = 0;
        virtual void                    RemoveAll() = 0;
    };
    class Scrollbar;
    class Container: public ControlUI,public IContainer
    {
    public:
        Container();
        virtual ~Container();
    public:
        virtual LPCTSTR                 GetClass() const;

        virtual int                     GetCount() const;
        virtual bool                    Add(ControlUI* pControl);

        virtual bool                    Remove(ControlUI *pControl) ;
        virtual void                    RemoveAll();
        void                            AddHandler();
        virtual void					HandleMsg(const MsgWrap & msg)throw();
        virtual void                    SetVisible(bool bVisible = true);
        virtual void                    SetInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
        virtual void                    SetMouseEnabled(bool bEnable = true);


        virtual RECT                    GetInset() const;
        virtual void                    SetInset(RECT rcInset); // 设置内边距，相当于设置客户区
        virtual int                     GetChildPadding() const;
        virtual void                    SetChildPadding(int iPadding);
        virtual bool                    IsAutoDestroy() const;
        virtual void                    SetAutoDestroy(bool bAuto);
        virtual bool                    IsDelayedDestroy() const;
        virtual void                    SetDelayedDestroy(bool bDelayed);
        virtual bool                    IsMouseChildEnabled() const;
        virtual void                    SetMouseChildEnabled(bool bEnable = true);

        virtual int                     FindSelectable(int iIndex, bool bForward = true) const;
        virtual void SetPos(YYRECT &rc);
        virtual void                    DoPaint(const YYRECT &rc);
        virtual void                    SetAttribute(const std::string &strName, const std::string& strValue);
        virtual void                    SetManager(ControlManager* pManager,ControlUI* pParent,bool bInit=true);

        bool                            SetSubControlText(const YString & pstrSubControlName,const YString& pstrText);
        bool                            SetSubControlFixedHeight(const YString & pstrSubControlName,int cy);
        bool                            SetSubControlFixedWdith(const YString& pstrSubControlName,int cx);
        bool                            SetSubControlUserData(const YString& pstrSubControlName,const YString& pstrText);
        YString                         GetSubControlText(const YString& pstrSubControlName);
        int                             GetSubControlFixedHeight(const YString& pstrSubControlName);
        int                             GetSubControlFixedWdith(const YString& pstrSubControlName);
        YString                         GetSubControlUserData(LPCTSTR pstrSubControlName);
        ControlUI*                      FindSubControl(const YString& pstrSubControlName);
        virtual ControlUI*
                                        FindControlFromPoint(POINT pt,UINT flag);
        virtual YYSIZE GetScrollPos() const;
        virtual YYSIZE GetScrollRange() const;
        virtual void                    SetScrollPos(YYSIZE szPos);
        virtual void                    LineUp();
        virtual void                    LineDown();
        virtual void                    PageUp();
        virtual void                    PageDown();
        virtual void                    HomeUp();
        virtual void                    EndDown();
        virtual void                    LineLeft();
        virtual void                    LineRight();
        virtual void                    PageLeft();
        virtual void                    PageRight();
        virtual void                    HomeLeft();
        virtual void                    EndRight();
        virtual void                    EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
        virtual Scrollbar*              GetVerticalScrollBar() const;
        virtual Scrollbar*              GetHorizontalScrollBar() const;
    protected:
        virtual void                    SetFloatPos(ControlUI* pControl);
        virtual void                    ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);
    protected:
        std::list<CountRefPtr<ControlUI> > m_ListItems;
        RECT                            m_rcInset;    //tag 'inset',基本上相当于收缩当前区域
        int                             m_iChildPadding;
        bool                            m_bAutoDestroy;
        bool                            m_bDelayedDestroy;
        bool                            m_bMouseChildEnabled;
        bool                            m_bScrollProcess; // 防止SetPos循环调用

    /*    CountRefPtr<Scrollbar>          m_pVerticalScrollBar;
        CountRefPtr<Scrollbar>          m_pHorizontalScrollBar;*/
        MsgHandler						m_ContainerMsgHandler;
    };
}