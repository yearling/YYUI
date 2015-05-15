#pragma once
#include "YUI.h"
#include "WindowWnd.h"
#include "WindowProperty.h"
namespace YUI
{
    class FrameLessWindow:public WindowWnd,
                          public IMsgHandler,
                          public std::enable_shared_from_this<FrameLessWindow>
    {

#pragma region Constructor&Destructor
    public:
        FrameLessWindow();
        virtual    ~FrameLessWindow();
    private:
        FrameLessWindow(const FrameLessWindow &);
       // FrameLessWindow operator=(const FrameLessWindow &);
#pragma endregion 

#pragma region inherite the interfaces
    public:
        virtual LRESULT                 OnSysMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        virtual void					HandleMsg(const MsgWrap & msg)throw();	
        virtual UINT                    GetClassStyle() const;
#pragma endregion �̳����Ľӿ�

#pragma region interfaces
    public:
        virtual void                    InitWindow();
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
    protected:
        virtual LPCTSTR                 GetWindowClassName(void) const= 0;//���Ǹ������
        virtual LONG                    GetStyle();
#pragma endregion ����Ҫ�̳еĽӿ�

    protected:
        WindowProperty                  m_Property;
    };
}

