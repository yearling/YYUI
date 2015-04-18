// YUI.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "YUI.h"
#include "WindowWnd.h"
#include "PaintManagerUI.h"
#include "MutiScreen.h"
using namespace YUI;
class MyWindow : public WindowWnd
{
public:
    MyWindow(){};
protected:
    virtual LPCTSTR                     GetWindowClassName() const { return _T("MyWindow") ;}

};

int APIENTRY _tWinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPTSTR lpCmdLine, __in int nShowCmd )
{
    YYCOM::YYSetConsoleA();
	PaintManagerUI::SetInstance(hInstance);
	MyWindow duiFrame;
	duiFrame.Create(NULL,_T("DUIWND"),WS_OVERLAPPEDWINDOW,WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();
    duiFrame.SetIcon(IDI_ICON1);
	duiFrame.ShowModal();
    
	return 0; 
}
