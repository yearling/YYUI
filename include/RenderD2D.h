#pragma once
#include "YUI.h"
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <atlcomcli.h>

namespace YUI
{
	struct Render2DException: YYUIException{};

	class RenderD2D
	{
	private:
		RenderD2D(void);
		RenderD2D(const RenderD2D &);
		RenderD2D& operator=(const RenderD2D &);
	public:
		virtual							~RenderD2D(void);

		void							Init();
		RenderD2D*						GetInstance() { return &m_Instance;}
	public:
		void							Resize(HWND hWnd,unsigned int width, unsigned int height);
		CComPtr<ID2D1HwndRenderTarget>	CreateHwndRenderTarget(HWND hwnd);
		CComPtr<ID2D1HwndRenderTarget>	CreateHwndRenderTarget(HWND hwnd,unsigned int width, unsigned int height);
		CComPtr<ID2D1HwndRenderTarget>  GetRenderTarget(HWND hWnd) ;
	public: /*Get & Set*/
		CComPtr<ID2D1Factory>			GetD2DFactory() const { return m_pD2DFactory; }
		void							SetD2DFactory(CComPtr<ID2D1Factory> val) { m_pD2DFactory = val; }
		CComPtr<IWICImagingFactory>		GetWICFactory() const { return m_pWICFactory; }
		void							SetWICFactory(CComPtr<IWICImagingFactory> val) { m_pWICFactory = val; }
		CComPtr<IDWriteFactory>			GetDWriteFactory() const { return m_pDWriteFactory; }
		void							SetDWriteFactory(CComPtr<IDWriteFactory> val) { m_pDWriteFactory = val; }
	private:
		static RenderD2D				m_Instance;
		CComPtr<ID2D1Factory>			m_pD2DFactory;
		CComPtr<IWICImagingFactory>		m_pWICFactory;
		CComPtr<IDWriteFactory>			m_pDWriteFactory;
		
		CComPtr<IDWriteTextFormat>		m_pTextFormat;
		CComPtr<ID2D1SolidColorBrush>	m_pBlackBrush;
		std::map<HWND,CComPtr<ID2D1HwndRenderTarget>>
										m_mapHwndToTarget;
	};

}