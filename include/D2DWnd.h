#pragma once
#include "YUI.h"
#include "WindowWnd.h"
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <atlcomcli.h>
#include "ControlUI.h"
#include "RenderD2D.h"
#include "FrameLessWindow.h"
namespace YUI
{
	struct D2DException: public YYUIException {};
	
	class CWndUI: public ControlUI
	{
	public:
		CWndUI(): m_hWnd(NULL){}
		virtual void SetInternVisible(bool bVisible = true);
		virtual void SetPos(RECT &rc);
		BOOL Attach(HWND hWndNew);
		HWND Detach();
	
	protected:
		HWND m_hWnd;
	};

	class D2DWnd: public FrameLessWindow/*, public ControlUI*/
	{
	public:
		D2DWnd();
		virtual ~D2DWnd();
        //virtual void                    SetInternVisible(bool bVisible = true);
        virtual void                    SetPos(RECT &rc);
		virtual UINT                    GetClassStyle() const;
		virtual LPCTSTR                 GetWindowClassName() const ;
		virtual void                    OnFinalMessage(HWND hWnd);
		virtual LRESULT                 OnSysMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		virtual void					Init();
		virtual void					OnResize(unsigned int width,unsigned int height);
		virtual void					OnRender();
		void							CreateDeviceDependResource();
		void                            DiscardDeviceResources();
	protected:
		CComPtr<ID2D1Factory>			m_pD2DFactory;
		CComPtr<IWICImagingFactory>		m_pWICFactory;
		CComPtr<IDWriteFactory>			m_pDWriteFactory;
		CComPtr<ID2D1HwndRenderTarget>  m_pRenderTarget;
		CComPtr<IDWriteTextFormat>		m_pTextFormat;
		CComPtr<ID2D1SolidColorBrush>	m_pBlackBrush;
	};

    class D3DWnd: public WindowWnd, public ControlUI
    {
    public:
        D3DWnd();
        virtual ~D3DWnd();
        virtual void                    SetInternVisible(bool bVisible = true);
        virtual void                    SetPos(RECT &rc);
        virtual UINT                    GetClassStyle() const;
        virtual LPCTSTR                 GetWindowClassName() const ;
        virtual void                    OnFinalMessage(HWND hWnd);
        virtual LRESULT                 OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    public:
        virtual void					Init();
        virtual void					OnResize(unsigned int width,unsigned int height);
        virtual void					OnRender();
    };
}