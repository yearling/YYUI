// YUI.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "YUI.h"
#include "WindowWnd.h"
#include "PaintManagerUI.h"
#include "MutiScreen.h"
#include "WindowImpl.h"
#include <iostream>
using namespace YUI;
class MyWindow : public WindowWnd
{
public:
    MyWindow(){};
protected:
    virtual LPCTSTR                     GetWindowClassName() const { return _T("MyWindow") ;}

};

class TestWindow:public WindowImpl
{
public:
    virtual LPCTSTR GetWindowClassName() const { return _T("DUIMainFrame");}
    virtual YString GetSkinFile()  { return _T("duilib.xml");}
    virtual YString GetSkinFolder() { return _T("click");}
    virtual void                       InitWindow()
    {
        AddEntry(MSG_Click,_T("closebtn"),[&](const NotifyMsg& msg){Close();});
        AddEntry(MSG_Click,_T("btnHello"),[&](const NotifyMsg& msg){::MessageBox(NULL,"button","push button",NULL);});
    }
};


int APIENTRY _tWinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPTSTR lpCmdLine, __in int nShowCmd )
{
    try{
    YYCOM::YYSetConsoleA();
	PaintManagerUI::SetInstance(hInstance);
	//MyWindow duiFrame;
	//duiFrame.Create(NULL,_T("DUIWND"),WS_OVERLAPPEDWINDOW,WS_EX_WINDOWEDGE);
	//duiFrame.CenterWindow();
 //   duiFrame.SetIcon(IDI_ICON1);
	//duiFrame.ShowModal();
    std::shared_ptr<TestWindow> spWindow= std::make_shared<TestWindow>();
    spWindow->Create(NULL,_T("DUIWND"),UI_WNDSTYLE_FRAME,WS_EX_WINDOWEDGE);
    spWindow->CenterWindow();
    spWindow->SetIcon(IDI_ICON1);
    spWindow->ShowModal();
    } 
    catch(YYUIException &e)
    {
        UNREFERENCED_PARAMETER(e);
        Ycout<<"catched in main"<<std::endl;
        std::cout<<YYCOM::CurrentExceptionDiagnosticInformation();
    }
	return 0; 
}

