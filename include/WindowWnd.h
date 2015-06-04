//////////////////////////////////////////////////////////////////////////
//create by yyCom 2015/4/10
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "YUI.h"
#include "WindowProperty.h"
#include <map>
namespace YUI
{

#define UI_WNDSTYLE_CONTAINER  (0)
#define UI_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define UI_WNDSTYLE_CHILD      (WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define UI_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define UI_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
    
   
	class WindowWnd:public CountRefObj
	{
    public:
        friend  class                   WindowManger;
		                                WindowWnd();
		virtual                         ~WindowWnd();
    public:
       
        //注册当前窗口的类名，如果注册失败（没有重复注册）,会抛异常; 
        void                            RegisterWindowClass();
        
        void                            RegisterSuperClass();
        //创建窗口，功能和CreateWindow很像; 
        HWND                            Create(HWND hwndParent, 
                                                LPCTSTR pstrName,
                                                DWORD dwStyle,
                                                DWORD dwExStyle,
                                                const RECT rc,
                                                HMENU hMenu= NULL);

        HWND                            Create(HWND hWndParent,
                                                LPCTSTR pstrName,
                                                DWORD dwStyple,
                                                DWORD dwExStyle,
                                                int x = CW_USEDEFAULT,
                                                int y = CW_USEDEFAULT,
                                                int cx = CW_USEDEFAULT,
                                                int cy = CW_USEDEFAULT,
                                                HMENU hMenu = NULL);

        HWND                            CreateUIWindow( HWND hWndParent,
                                                LPCTSTR pstrWindowName,
                                                DWORD dwStyle =0,
                                                DWORD dwExStyle=0);
        //替换hWnd窗口，把自己的WndProc，YWindowWnd的指针替换成自己的
        HWND                            SubClass(HWND hWnd);
        void                            UnsubClass();

        void                            ShowWindow(bool bShow = true, bool bTakeFocus = true);
        UINT                            ShowModal();
        void                            Close(UINT nRet = IDOK);
		void							MaxWindow();
		void							MinWindow();
		void							RestoreWindow();
        void                            CenterWindow();
        LRESULT                         SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
        LRESULT                         PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L); 
#pragma region GET/SET
    public: 
        void                            SetIcon(UINT nRes);
        const WindowProperty*           GetWindowProperty() const { return &m_Property;}
#pragma endregion GET/SET
    public: 
        HWND                            GetHWND() const  { return m_hWnd; }
        operator                        HWND() const { return m_hWnd; }
#pragma region interface
    protected:
        virtual UINT                    GetClassStyle() const;
        virtual LPCTSTR                 GetWindowClassName() const = 0;

        virtual void                    OnFinalMessage(HWND hWnd);
        virtual LRESULT                 OnSysMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        virtual LPCTSTR                 GetSuperClassName() const;
#pragma endregion 
    protected:
        static  LRESULT CALLBACK        ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		HWND							m_hWnd;
		WNDPROC							m_OldWndProc;
		bool							m_bSubClassed; //用自己的WndProc代替了原有的Proc
    protected:
        WindowProperty                  m_Property;
    private:
        WindowWnd(const WindowWnd &);
        WindowWnd & operator=(const WindowWnd &);
	};
    
}