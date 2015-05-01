#include "YUI.h"
#include "D2DRender.h"

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

	}

	LRESULT D2DWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		default:
			break;
		}
		return WindowWnd::HandleMessage(uMsg,wParam,lParam);
	}

	void D2DWnd::Init()
	{
		static const WCHAR msc_fontName[] = L"Verdana";
		static const FLOAT msc_fontSize = 50;
		HRESULT hr = S_OK;
#pragma region 
		 // Create a Direct2D factory.
		if(FAILED(hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory)))
			THROW_EXCEPTION(D2DException()<<UIErrorStr(_T("Create D2D Factory FAILED!"))<<UIErrorHr(hr));
		// Create a DirectWrite factory.
		if(FAILED( hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory.p)
			)))
		{
			THROW_EXCEPTION(D2DException()<<UIErrorStr(_T("Create DWriteCreateFactory FAILED!"))<<UIErrorHr(hr));
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
			THROW_EXCEPTION(D2DException()<<UIErrorStr(_T("Create Font Error"))<<UIErrorHr(hr));
		}
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
#pragma endregion CreateDeviceIndependentResource 
	}

	void D2DWnd::OnResize(unsigned int width,unsigned int height)
	{
		if(m_pRenderTarget)
		{
			D2D1_SIZE_U size;
			size.width = width;
			size.height = height;
			m_pRenderTarget->Resize(size);
		}
	}

	void D2DWnd::OnRender()
	{
		CreateDeviceDependResource();
		HRESULT hr= S_OK;
		if ( !(m_pRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
		{
			static const WCHAR sc_helloWorld[] = L"Hello, World Direct2D!";

			// Retrieve the size of the render target.
			D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

			m_pRenderTarget->BeginDraw();

			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

			m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

			m_pRenderTarget->DrawText(
				sc_helloWorld,
				ARRAYSIZE(sc_helloWorld) - 1,
				m_pTextFormat,
				D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
				m_pBlackBrush
				);

			hr = m_pRenderTarget->EndDraw();

			if (hr == D2DERR_RECREATE_TARGET)
			{
				hr = S_OK;
				DiscardDeviceResources();
			}
		}
	}

	void D2DWnd::CreateDeviceDependResource()
	{
		HRESULT hr = S_OK;
		if(!m_pRenderTarget)
		{
			RECT rc;
			GetClientRect(m_hWnd, &rc);

			D2D1_SIZE_U size = D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top
				);

			// Create a Direct2D render target.
			D2D1_RENDER_TARGET_PROPERTIES rtp;
			rtp=D2D1::RenderTargetProperties();
			rtp.minLevel=D2D1_FEATURE_LEVEL_10;
			rtp.usage=D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;

			D2D1_HWND_RENDER_TARGET_PROPERTIES hrtp=D2D1::HwndRenderTargetProperties(m_hWnd,size);
			hrtp.presentOptions=D2D1_PRESENT_OPTIONS_IMMEDIATELY;
			assert(m_pD2DFactory &&"d2d factory create failed");
			if(FAILED( hr = m_pD2DFactory->CreateHwndRenderTarget(
				rtp,
				hrtp,
				&m_pRenderTarget.p
				)))
			{
				THROW_EXCEPTION(D2DException()<<UIErrorStr(_T("Create Render Target Failed"))<<UIErrorHr(hr));
			}
			assert(m_pRenderTarget && "render target is null");
			if(FAILED(hr= m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&m_pBlackBrush
				)))
			{
				THROW_EXCEPTION(D2DException()<<UIErrorStr(_T("Create Brush Failed"))<<UIErrorHr(hr));
			}
		}
	}

	void D2DWnd::DiscardDeviceResources()
	{
		m_pRenderTarget = nullptr;
		m_pBlackBrush = nullptr;
	}

}