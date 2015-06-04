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
        virtual void                    SetInternVisible(bool bVisible = true); // �����ڲ����ã���ЩUIӵ�д��ھ������Ҫ��д�˺���
        virtual void                    SetMouseEnabled(bool bEnable = true);


        virtual RECT                    GetInset() const;
        virtual void                    SetInset(RECT rcInset); // �����ڱ߾࣬�൱�����ÿͻ���
        virtual float GetChildPadding() const;
        virtual void SetChildPadding(float iPadding);
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
        bool SetSubControlFixedHeight(const YString & pstrSubControlName,float cy);
        bool SetSubControlFixedWdith(const YString& pstrSubControlName,float cx);
        bool                            SetSubControlUserData(const YString& pstrSubControlName,const YString& pstrText);
        YString                         GetSubControlText(const YString& pstrSubControlName);
        float GetSubControlFixedHeight(const YString& pstrSubControlName);
        float GetSubControlFixedWdith(const YString& pstrSubControlName);
        YString                         GetSubControlUserData(LPCTSTR pstrSubControlName);
        ControlUI*                      FindSubControl(const YString& pstrSubControlName);
        virtual ControlUI*
                                        FindControlFromPoint(POINT pt,UINT flag);
        virtual ControlUI*              FindControlFromName(const YString & strName);
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
        virtual void ProcessScrollBar(YYRECT rc, float cxRequired, float cyRequired);
    protected:
        std::list<CountRefPtr<ControlUI> > m_ListItems;
        YYRECT                            m_rcInset;    //tag 'inset',�������൱��������ǰ����
        float                            m_iChildPadding;
        bool                            m_bAutoDestroy;
        bool                            m_bDelayedDestroy;
        bool                            m_bMouseChildEnabled;
        bool                            m_bScrollProcess; // ��ֹSetPosѭ������

    /*    CountRefPtr<Scrollbar>          m_pVerticalScrollBar;
        CountRefPtr<Scrollbar>          m_pHorizontalScrollBar;*/
        MsgHandler						m_ContainerMsgHandler;
    };
}