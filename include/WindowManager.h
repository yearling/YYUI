#pragma once
#include "YUI.h"
#include "WindowWnd.h"
namespace YUI
{
    struct WindowMangerExcption:YYUIException{};
    class WindowManger
    {
    public:
        ~WindowManger();      
        void                            RegisterWindow(HWND hWnd,WindowWnd *pWindow);
        void                            UnRegisterWindow(HWND hWnd);
        static LRESULT CALLBACK         WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static WindowManger*            GetInstance() { return &m_Instance;}
    public:
        UINT                            GetWindowsNumber() const;
        const WindowProperty*           GetWindowProperty(HWND hWnd);
    private:
        std::map<HWND,CountRefPtr<WindowWnd > >      m_mapHWNDToWindow;
    private:
        static WindowManger             m_Instance;
        WindowManger();
        WindowManger(const WindowManger&);
        WindowManger operator=(const WindowManger &);
    };
}