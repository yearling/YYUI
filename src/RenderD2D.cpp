#include "RenderD2D.h"

namespace YUI
{

	RenderD2D::RenderD2D(void)
	{
	}

	RenderD2D::~RenderD2D(void)
	{

	}

	void RenderD2D::Init()
	{
		static const WCHAR msc_fontName[] = L"Verdana";
		static const FLOAT msc_fontSize = 50;
		HRESULT hr = S_OK;
#pragma region 
		// Create a Direct2D factory.
		if(FAILED(hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory)))
			THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Create D2D Factory FAILED!"))<<UIErrorHr(hr));
		// Create a DirectWrite factory.
		if(FAILED( hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory.p)
			)))
		{
			THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Create DWriteCreateFactory FAILED!"))<<UIErrorHr(hr));
		}
		// Create a DirectWrite text format object
		if(FAILED( hr = m_pDWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"", //locale
			&m_pTextFormat
			)))
		{
			THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Create Font Error"))<<UIErrorHr(hr));
		}
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
#pragma endregion CreateDeviceIndependentResource 
	}

	void RenderD2D::Resize(HWND hWnd,unsigned int width, unsigned int height)
	{
		assert(IsWindow(hWnd));
		auto RenderTarget = GetRenderTarget(hWnd);
		assert(RenderTarget && "Render Target is NULL");
		if(RenderTarget)
		{
			D2D1_SIZE_U size;
			size.width = width;
			size.height = height;
			auto LastSize = RenderTarget->GetSize();
			if( width != LastSize.width || height != LastSize.height)
				RenderTarget->Resize(size);
		}
	}

	CComPtr<ID2D1HwndRenderTarget> RenderD2D::CreateHwndRenderTarget(HWND hwnd)
	{
		assert(IsWindow(hwnd) );
		HRESULT hr= S_OK;
		// Create a Direct2D render target.
		D2D1_RENDER_TARGET_PROPERTIES rtp;
		rtp=D2D1::RenderTargetProperties();
		rtp.minLevel=D2D1_FEATURE_LEVEL_10;
		rtp.usage=D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
		RECT rc;
		GetClientRect(hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
			);
		D2D1_HWND_RENDER_TARGET_PROPERTIES hrtp=D2D1::HwndRenderTargetProperties(hwnd,size);
		hrtp.presentOptions=D2D1_PRESENT_OPTIONS_IMMEDIATELY;
		assert(m_pD2DFactory &&"d2d factory create failed");
		CComPtr<ID2D1HwndRenderTarget>  hWndRT;
		if(FAILED( hr = m_pD2DFactory->CreateHwndRenderTarget(
			rtp,
			hrtp,
			&hWndRT
			)))
		{
			THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Create Render Target Failed"))<<UIErrorHr(hr));
		}
		assert(hWndRT && "render target create filed!");
		//插入到map中保存
		m_mapHwndToTarget[hwnd]	= hWndRT;
		return hWndRT;
	}

	CComPtr<ID2D1HwndRenderTarget> RenderD2D::CreateHwndRenderTarget(HWND hwnd,unsigned int width, unsigned int height)
	{
		assert(IsWindow(hwnd) );
		HRESULT hr= S_OK;
		// Create a Direct2D render target.
		D2D1_RENDER_TARGET_PROPERTIES rtp;
		rtp=D2D1::RenderTargetProperties();
		rtp.minLevel=D2D1_FEATURE_LEVEL_10;
		rtp.usage=D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
		D2D1_SIZE_U size = D2D1::SizeU(width,height);
		D2D1_HWND_RENDER_TARGET_PROPERTIES hrtp=D2D1::HwndRenderTargetProperties(hwnd,size);
		hrtp.presentOptions=D2D1_PRESENT_OPTIONS_IMMEDIATELY;
		assert(m_pD2DFactory &&"d2d factory create failed");
		CComPtr<ID2D1HwndRenderTarget>  hWndRT;
		if(FAILED( hr = m_pD2DFactory->CreateHwndRenderTarget(
			rtp,
			hrtp,
			&hWndRT
			)))
		{
			THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Create Render Target Failed"))<<UIErrorHr(hr));
		}
		assert(hWndRT && "render target create filed!");
		//插入到map中保存
		m_mapHwndToTarget[hwnd]	= hWndRT;
		return hWndRT;
	}

	CComPtr<ID2D1HwndRenderTarget> RenderD2D::GetRenderTarget(HWND hWnd) 
	{
		auto iter = m_mapHwndToTarget.find(hWnd);
		if( iter == m_mapHwndToTarget.end() )
			return nullptr;
		else 
			return m_mapHwndToTarget[hWnd];
	}

	YUI::RenderD2D RenderD2D::m_Instance;

}
