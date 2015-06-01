
#pragma once

#include "YUI.h"
#include "UIDef.h"
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
        // FIX ME !!���Բ����麯��
        virtual void                    SetManager(ControlManager* pManager, ControlUI* pParent,bool bInit=true);
        virtual ControlUI*              GetParent() const;
        
        //������
        virtual YString                 GetText() const;
        virtual void                    SetText(YString const & strName);

        //ͼ����
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

        //�߿�
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


        // λ�����
        virtual RECT                    GetPos() const;
        virtual int                     GetWidth() const;
        virtual int                     GetHeight() const;
        virtual int                     GetX() const;
        virtual int                     GetY() const;
        virtual RECT                    GetPadding() const;
        virtual void                    SetPadding(RECT rcPadding); // ������߾࣬���ϲ㴰�ڻ���
        virtual SIZE                    GetFixedXY() const;         // ʵ�ʴ�Сλ��ʹ��GetPos��ȡ������õ�����Ԥ��Ĳο�ֵ
        virtual void                    SetFixedXY(SIZE szXY);      // ��floatΪtrueʱ��Ч
        virtual int                     GetFixedWidth() const;       // ʵ�ʴ�Сλ��ʹ��GetPos��ȡ������õ�����Ԥ��Ĳο�ֵ
        virtual void                    SetFixedWidth(int cx);      // Ԥ��Ĳο�ֵ
        virtual int                     GetFixedHeight() const;      // ʵ�ʴ�Сλ��ʹ��GetPos��ȡ������õ�����Ԥ��Ĳο�ֵ
        virtual void                    SetFixedHeight(int cy);     // Ԥ��Ĳο�ֵ
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

        // �����ʾ
        virtual YString                 GetToolTip() const;
        virtual void                    SetToolTip(const YString& pstrText);
        virtual void                    SetToolTipWidth(int nWidth);
        virtual int	                    GetToolTipWidth(void);	// ����ToolTip��������

        // ��ݼ�
        virtual TCHAR                   GetShortcut() const;
        virtual void                    SetShortcut(TCHAR ch);

        // �˵�
        virtual bool                    IsContextMenuUsed() const;
        virtual void                    SetContextMenuUsed(bool bMenuUsed);

        // �û�����
        virtual YString                 GetUserData(); // �������������û�ʹ��
        virtual void                    SetUserData(const YString &strText); // �������������û�ʹ��
        virtual UINT_PTR                GetTag() const; // �������������û�ʹ��
        virtual void                    SetTag(UINT_PTR pTag); // �������������û�ʹ��
    public:
       
        // һЩ��Ҫ������
        virtual bool                    IsVisible() const;
        virtual void                    SetVisible(bool bVisible = true);
        virtual void                    SetInternVisible(bool bVisible = true); // �����ڲ����ã���ЩUIӵ�д��ھ������Ҫ��д�˺���
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

        virtual SIZE                    EstimateSize(SIZE szAvailable);

        virtual void                    DoPaint(const YYRECT &rc);
        virtual void                    PaintBkColor();
        virtual void                    PaintBkImage();
        virtual void                    PaintStatusImage();
        virtual void                    PaintText();
        virtual void                    PaintBorder();

        virtual void                    DoPostPaint(HDC hDC, const RECT& rcPaint);

        //���ⴰ�ڲ���
        void                            SetVirtualWnd(LPCTSTR pstrValue);
        YString                         GetVirtualWnd() const;
    protected:
        CountRefPtr<ControlManager>     m_pManager;
        ControlUI*                      m_pParent;
        YString                         m_strVirtualWnd;
        YString                         m_strName; // tag 'name'
        bool                            m_bUpdateNeeded;
        bool                            m_bMenuUsed;
        RECT                            m_rcPadding; //tag 'padding'
        SIZE                            m_cXY; // maybe control ���Ͻǵĵ�  tag 'pos'
        SIZE                            m_cXYFixed; //fix��� tag 'pos' 'width' 'height'
        SIZE                            m_cxyMin;// tag 'minwidth' 'minheight'
        SIZE                            m_cxyMax;//tag 'maxwidth' 'maxheight'
        RECT                            m_rcItem;

        
        int                             m_nTooltipWidth;
        bool                            m_bVisible;// tag 'visible'
        bool                            m_bInternVisible;
        bool                            m_bEnable; // tag 'endable'
        bool                            m_bMouseEnabled; // tag 'mouse'
        bool                            m_bKeyboardEnabled; // tag 'keyboard'
        bool                            m_bFocused; //
        bool                            m_bFloat; // tag 'float'
        bool                            m_bSetPos; //��ֹSetPosѭ������
        RelativePosUI                   m_RelativePos; //����ڸ��ڵ������ tag 'relativepos'

        YString                         m_strText;  //�ı�����Ӧ'text'��ǩ
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
        int                             m_nBorderSize;//tag 'boradersize'  
        int                             m_nBorderStyle; // tag 'borderstyple'
        RECT                            m_rcPaint; 
        RECT                            m_rcBorderSize;//tag 'boradersize' 'leftbordersize' 'topboradersize' 'bottombordersize' 
    protected:
        MsgHandler					    m_ControlMsgHandler;
    };
}