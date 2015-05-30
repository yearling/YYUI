// YUI.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "YUI.h"
#include "WindowWnd.h"
#include "MutiScreen.h"
#include "resource.h"
#include <iostream>
#include "D2DWnd.h"
#include "Canvas2D.h"
#include "MessageSystem.h"
#include "SystemInfo.h"
using namespace YUI;
class MyWindow : public WindowWnd
{
public:
    MyWindow(){};
protected:
    virtual LPCTSTR                     GetWindowClassName() const { return _T("MyWindow") ;}

};
//
//class TestWindow:public WindowImpl
//{
//public:
//    virtual LPCTSTR						GetWindowClassName() const { return _T("DUIMainFrame");}
//    virtual YString						GetSkinFile()  { return _T("duilib.xml");}
//    virtual YString						GetSkinFolder() { return _T("click");}
//    virtual void                        OnFinalMessage(HWND hWnd)
//    {
//        ::PostQuitMessage(0);
//    }
//    virtual void						InitWindow()
//    {
//        AddEntry(MSG_Click,_T("closebtn"),[&](const NotifyMsg& msg){Close();});
//        AddEntry(MSG_Click,_T("btnHello"),[&](const NotifyMsg& msg){::MessageBox(NULL, _T("button"),_T("push button"),NULL);});
//    }
//	virtual std::shared_ptr<ControlUI> CreateControl(std::string pstrClass)
//	{
//		/*if (strcmp(pstrClass.c_str(), ("Wnd")) == 0)
//		{
//			auto pUI  = std::make_shared<D2DWnd>();           
//            pUI->Create(m_hWnd,_T("D2D"),UI_WNDSTYLE_CHILD,WS_EX_WINDOWEDGE);
//			return pUI;
//		}*/
//
//		return NULL;
//	}
//};


int APIENTRY _tWinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPTSTR lpCmdLine, __in int nShowCmd )
{
    try{
		 if (FAILED(CoInitialize(NULL)))
			 return 0;
    YYCOM::YYSetConsoleA();
#if 0
	{
		PaintManagerUI::SetInstance(hInstance);
		std::shared_ptr<TestWindow> spWindow= std::make_shared<TestWindow>();
		spWindow->Create(NULL,_T("DUIWND"),WS_CAPTION,WS_EX_WINDOWEDGE);
		spWindow->CenterWindow();
		spWindow->SetIcon(IDI_ICON1);
		//spWindow->ShowModal();
    //PaintManagerUI::MessageLoop();
	}
#endif
	    Canvas2D::Init(); 
        SystemInfo::GetInstance()->SetProcessInstance(hInstance);
		D2DWnd *spD2D =  new D2DWnd;
		spD2D->Init();
		spD2D->Create(NULL,_T("D2DWnd"),UI_WNDSTYLE_FRAME,WS_EX_WINDOWEDGE);
		spD2D->CenterWindow();
        D2DWnd *spD3D = new D2DWnd;
        spD3D->Init();
        spD3D->Create(NULL,_T("D2DWnd"),UI_WNDSTYLE_FRAME,WS_EX_WINDOWEDGE);
        spD3D->CenterWindow();

       /* std::shared_ptr<D2DWnd> spD4D = std::make_shared<D2DWnd>();
        spD4D->Init();
        spD4D->Create(NULL,_T("D2DWnd"),UI_WNDSTYLE_FRAME,WS_EX_WINDOWEDGE);
        spD4D->CenterWindow();
*/
		//spD2D->ShowModal();
        MessageSystem::GetInstance()->MeesageLoop();
		CoUninitialize();
    } 
    catch(YYUIException &e)
    {
        UNREFERENCED_PARAMETER(e);
        Ycout<<"catched in main"<<std::endl;
        std::cout<<YYCOM::CurrentExceptionDiagnosticInformation();
        CoUninitialize();
    }
	return 0; 
}

