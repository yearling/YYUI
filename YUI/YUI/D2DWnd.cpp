#include "YUI.h"
#include "D2DWnd.h"
#include "Canvas2D.h"
#include "Button.h"
#include "resource.h"
using std::cout;
using std::wcout;
using std::endl;
namespace YUI
{


	D2DWnd::D2DWnd()
	{
    }

	D2DWnd::~D2DWnd()
	{

	}

	UINT D2DWnd::GetClassStyle() const
	{
		return CS_DBLCLKS;
	}

	LPCTSTR D2DWnd::GetWindowClassName() const
	{
		return _T("D2DWnd");
	}

	void D2DWnd::OnFinalMessage(HWND hWnd)
	{
       // PostQuitMessage(0);
	}

  
	void D2DWnd::Init()
	{
		YString msc_fontName = _T("Verdana");
		static const FLOAT msc_fontSize = 50;
		Button *pButton(dynamic_cast<Button*>(m_pControlManger->FindControl(_T("closebtn"))));
		assert(pButton && "pButton not find");
		pButton->SetClickEvent([&](){
			Close();
		});
		pButton = dynamic_cast<Button*>(m_pControlManger->FindControl(_T("minbtn")));
		assert(pButton && "pButton not find");
		pButton->SetClickEvent([&](){
			MinWindow();
		});
		pButton = dynamic_cast<Button*>(m_pControlManger->FindControl(_T("maxbtn")));
		assert(pButton && "pButton not find");
		pButton->SetClickEvent([&](){
			MaxWindow();
			Button* pButton = dynamic_cast<Button*>(m_pControlManger->FindControl(_T("maxbtn")));
			pButton->SetVisible(false);
			CountRefPtr<Button> RestoreButton = dynamic_cast<Button*>(m_pControlManger->FindControl(_T("restorebtn")));	
			assert(RestoreButton && "restore button can't find");
			RestoreButton->SetVisible(true);
		});
		pButton = dynamic_cast<Button*>(m_pControlManger->FindControl(_T("restorebtn")));
		assert(pButton && "pButton not find");
		pButton->SetClickEvent([&](){
			RestoreWindow();
			Button *pButton = dynamic_cast<Button*>(m_pControlManger->FindControl(_T("restorebtn")));
			pButton->SetVisible(false);
			CountRefPtr<Button> MaxButton = dynamic_cast<Button*>(m_pControlManger->FindControl(_T("maxbtn")));	
			assert(MaxButton && "restore button can't find");
			MaxButton->SetVisible(true);
		});
        SetIcon(IDI_BLACKCAT);
		HRESULT hr = S_OK;
	}

	void D2DWnd::OnResize(float width,float height)
	{
		Canvas2D canvas(m_hWnd);
        canvas.OnResize((float)width,(float)height);
        InvalidateRect(m_hWnd,NULL,FALSE);
	}

	void D2DWnd::OnRender()
	{
        Ycout<<"D2DWnd OnRender"<<endl;
        Canvas2D cavas(m_hWnd);
        /*if(!cavas.BeginDraw())
            return;*/
      /*  cavas.Clear(YYRGB(155,155,244));
        YString strOut= _T("HELLO D2D");
        FontD2D defaultFont(_T("Verdana"),50);
        YYCOLOR color(YYRGB(0,0,0));
        YYRECT rect(0.0f,0.0f,cavas.GetSize().width,cavas.GetSize().height);
        cavas.DrawBitmap(_T("sampleImage.jpg"),rect);
        cavas.DrawSolidText(strOut,defaultFont,rect,color);
        YYPOINT start(50,50);
        YYPOINT end(100,100);
        cavas.DrawLine(start,end,YYRGB(255,0,0),1.0f);
        YYRECT rcCenter(200,200,600,400);
        cavas.DrawRect(rcCenter,YYRGB(0,255,0),1.0f);
        YYRECT rcFill(650,100,750,300);
        cavas.FillRect(rcFill,YYRGBA(155,155,244,128));*/
        //cavas.EndDraw();
	}

	void D2DWnd::CreateDeviceDependResource()
	{
		
	}

	void D2DWnd::DiscardDeviceResources()
	{
		m_pBlackBrush = nullptr;
	}

    //void D2DWnd::SetInternVisible(bool bVisible /*= true*/)
    //{
    //    __super::SetInternVisible(bVisible);
    //    ::ShowWindow(m_hWnd, bVisible);
    //}

    void D2DWnd::SetPos(RECT &rc)
    {
        //__super::SetPos(rc);
        ::SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
    }

    LRESULT D2DWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        OnResize((float)GET_X_LPARAM(lParam),(float)GET_Y_LPARAM(lParam));
        bHandled = FALSE;
        return 0;
    }

	LRESULT D2DWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL bZoomed = ::IsZoomed(*this);
		LRESULT lRes = FrameLessWindow::OnSysCommand(uMsg,wParam,lParam,bHandled);
		if( ::IsZoomed(*this) != bZoomed )
		{
			//add by yyCom to fix the 当最大化时，没有最大化的icon
			Button* pbtnMax = dynamic_cast<Button*>(m_pControlManger->FindControl(_T("maxbtn")));
			Button* pbtnRestore = dynamic_cast<Button*>(m_pControlManger->FindControl(_T("restorebtn")));
			// 切换最大化按钮和还原按钮的状态
			if (pbtnMax && pbtnRestore)
			{
				pbtnMax->SetVisible(TRUE == bZoomed);       // 此处用表达式是为了避免编译器BOOL转换的警告
				pbtnRestore->SetVisible(FALSE == bZoomed);
			}
		}
		return lRes;
	}

    LRESULT D2DWnd::OnWindowMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lRes = FrameLessWindow::OnWindowMoving(uMsg,wParam,lParam,bHandled);
        //add by yyCom to fix the 当最大化时，没有最大化的icon
        Button* pbtnMax = dynamic_cast<Button*>(m_pControlManger->FindControl(_T("maxbtn")));
        Button* pbtnRestore = dynamic_cast<Button*>(m_pControlManger->FindControl(_T("restorebtn")));
        if( ::IsZoomed(*this)  )
        {
            // 切换最大化按钮和还原按钮的状态
            if (pbtnMax && pbtnRestore)
            {
                pbtnMax->SetVisible(false);       // 此处用表达式是为了避免编译器BOOL转换的警告
                pbtnRestore->SetVisible(true);
            }
        }
        else
        {
            if (pbtnMax && pbtnRestore)
            {
                pbtnMax->SetVisible(true);       // 此处用表达式是为了避免编译器BOOL转换的警告
                pbtnRestore->SetVisible(false);
            }
        }
        return lRes;
    }


    
	void CWndUI::SetInternVisible(bool bVisible /*= true*/)
	{
			__super::SetInternVisible(bVisible);
			::ShowWindow(m_hWnd, bVisible);
	}

	void CWndUI::SetPos(YYRECT &rc)
	{
			__super::SetPos(rc);
			::SetWindowPos(m_hWnd, NULL,(int) rc.left, (int)rc.top, (int)(rc.right - rc.left), (int) (rc.bottom - rc.top), SWP_NOZORDER | SWP_NOACTIVATE);

	}

	BOOL CWndUI::Attach(HWND hWndNew)
	{
		
			if (! ::IsWindow(hWndNew))
			{
				return FALSE;
			}

			m_hWnd = hWndNew;
			return TRUE;
	}

	HWND CWndUI::Detach()
	{
			HWND hWnd = m_hWnd;
			m_hWnd = NULL;
			return hWnd;
	}


    D3DWnd::D3DWnd()
    {

    }

    D3DWnd::~D3DWnd()
    {

    }

    void D3DWnd::SetInternVisible(bool bVisible /*= true*/)
    {
        __super::SetInternVisible(bVisible);
        ::ShowWindow(m_hWnd, bVisible);
    }

    void D3DWnd::SetPos(YYRECT &rc)
    {
        __super::SetPos(rc);
        ::SetWindowPos(m_hWnd, NULL, (int)rc.left, (int)rc.top,int( rc.right - rc.left), int(rc.bottom - rc.top), SWP_NOZORDER | SWP_NOACTIVATE);
    }

    UINT D3DWnd::GetClassStyle() const
    {
        return CS_DBLCLKS;
    }

    LPCTSTR D3DWnd::GetWindowClassName() const
    {
        return _T("D2DWnd");
    }

    void D3DWnd::OnFinalMessage(HWND hWnd)
    {

    }

    LRESULT D3DWnd::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_PAINT:
            {
                OnRender();
            }
            break;
        case WM_SIZE:
            {
                OnResize(LOWORD(lParam),HIWORD(lParam));
            }
            break;
        default:
            break;
        }
        return WindowWnd::OnSysMessage(uMsg,wParam,lParam);
    }

    void D3DWnd::Init()
    {

    }

    void D3DWnd::OnResize(unsigned int width,unsigned int height)
    {

    }

    void D3DWnd::OnRender()
    {

    }

}