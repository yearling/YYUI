// YUI.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "YUI.h"
#include "WindowWnd.h"
#include "PaintManagerUI.h"
#include "MutiScreen.h"
#include "WindowImpl.h"
#include "resource.h"
#include <iostream>
#include "D2DRender.h"
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
    virtual LPCTSTR						GetWindowClassName() const { return _T("DUIMainFrame");}
    virtual YString						GetSkinFile()  { return _T("duilib.xml");}
    virtual YString						GetSkinFolder() { return _T("click");}
    virtual void						InitWindow()
    {
        AddEntry(MSG_Click,_T("closebtn"),[&](const NotifyMsg& msg){Close();});
        AddEntry(MSG_Click,_T("btnHello"),[&](const NotifyMsg& msg){::MessageBox(NULL, _T("button"),_T("push button"),NULL);});
    }
	virtual std::shared_ptr<ControlUI> 
										CreateControl(YString pstrClass)
	{
		if (_tcsicmp(pstrClass.c_str(), _T("Wnd")) == 0)
		{
			auto pUI  = std::make_shared<D2DWnd>();           
            pUI->Create(m_hWnd,_T("D2D"),UI_WNDSTYLE_CHILD,WS_EX_WINDOWEDGE);
			return pUI;
		}

		return NULL;
	}
};


int APIENTRY _tWinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPTSTR lpCmdLine, __in int nShowCmd )
{
    try{
		 if (FAILED(CoInitialize(NULL)))
			 return 0;
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
	 
		/*std::shared_ptr<D2DWnd> spD2D = std::make_shared<D2DWnd>();
		spD2D->Init();
		spD2D->Create(NULL,_T("D2DWnd"),UI_WNDSTYLE_FRAME,WS_EX_WINDOWEDGE);
		spD2D->CenterWindow();
		spD2D->ShowModal();*/
		CoUninitialize();
    } 
    catch(YYUIException &e)
    {
        UNREFERENCED_PARAMETER(e);
        Ycout<<"catched in main"<<std::endl;
        std::cout<<YYCOM::CurrentExceptionDiagnosticInformation();
    }
	return 0; 
}

