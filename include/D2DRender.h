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
namespace YUI
{
	struct D2DException: public YYUIException {};
	class D2DWnd: public WindowWnd, public std::enable_shared_from_this<D2DWnd>
	{
	public:
		D2DWnd();
		virtual ~D2DWnd();
		virtual UINT                    GetClassStyle() const;
		virtual LPCTSTR                 GetWindowClassName() const ;
		virtual void                    OnFinalMessage(HWND hWnd);
		virtual LRESULT                 HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		virtual void					Init();
		virtual void					OnResize(unsigned int width,unsigned int height);
		virtual void					OnRender();
		void							CreateDeviceDependResource();
		void DiscardDeviceResources();
	protected:
		CComPtr<ID2D1Factory>			m_pD2DFactory;
		CComPtr<IWICImagingFactory>		m_pWICFactory;
		CComPtr<IDWriteFactory>			m_pDWriteFactory;
		CComPtr<ID2D1HwndRenderTarget>  m_pRenderTarget;
		CComPtr<IDWriteTextFormat>		m_pTextFormat;
		CComPtr<ID2D1SolidColorBrush>	m_pBlackBrush;
	};
}