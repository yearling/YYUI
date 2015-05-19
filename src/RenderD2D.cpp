#include "RenderD2D.h"
#include <dxgi1_2.h>
#include "UIUtility.h"

namespace YUI
{

	RenderD2D::RenderD2D(void)
	{
        if (FAILED(CoInitialize(NULL)))
        {
            assert(0&&"CoInitialize failed!");
            ::terminate();
        }
        HRESULT hr = S_OK;
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

       if(FAILED( hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IWICImagingFactory,
            reinterpret_cast<void **>(&m_pWICFactory)
            )))
       {
            THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Create WICImageFactory FAILED!"))<<UIErrorHr(hr));
       }
	}

	RenderD2D::~RenderD2D(void)
	{

	}


	void RenderD2D::Resize(HWND hWnd,unsigned int width, unsigned int height)
	{
		assert(IsWindow(hWnd));
        GetHwndRenderTarget(hWnd,width,height);
	}

	std::shared_ptr<RenderTargetHWND> RenderD2D::GetHwndRenderTarget(HWND hwnd)
	{

		assert(IsWindow(hwnd) );
        auto iter = m_mapHwndToTarget.find(hwnd);
        if(iter!= m_mapHwndToTarget.end())
            return iter->second;

        
		HRESULT hr= S_OK;
		// Create a Direct2D render target.
		D2D1_RENDER_TARGET_PROPERTIES rtp;
		rtp=D2D1::RenderTargetProperties();
		rtp.minLevel=D2D1_FEATURE_LEVEL_10;
		rtp.usage=D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
		RECT rc;
		GetClientRect(hwnd, &rc);
	    return GetHwndRenderTarget(hwnd,rc.right-rc.left,rc.bottom-rc.top);	
	}

	std::shared_ptr<RenderTargetHWND> RenderD2D::GetHwndRenderTarget(HWND hwnd,unsigned int width, unsigned int height)
	{
		assert(IsWindow(hwnd) );
        auto iter = m_mapHwndToTarget.find(hwnd);
        if(iter!=m_mapHwndToTarget.end())
        {
            auto & rt=iter->second->GetRenderTarget();
            auto LastSize = rt->GetSize();
            D2D1_SIZE_U size;
            size.width = width;
            size.height = height;
            if( width != LastSize.width || height != LastSize.height)
                rt->Resize(size);
            return iter->second;
        }
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
        std::shared_ptr<RenderTargetHWND> spRT(new RenderTargetHWND(hWndRT));
        m_mapHwndToTarget[hwnd]	= spRT;
        return spRT;
	}

    CComPtr<IDWriteTextFormat> RenderD2D::GetTextureFormat(const YString &strFontname,float fFontsize)
    {
        FontD2D tmp(strFontname,fFontsize);
        return GetTextureFormat(tmp);
    }

    CComPtr<IDWriteTextFormat> RenderD2D::GetTextureFormat( const FontD2D &font)
    {
        
        auto iter = m_mapFontToTextFormat.find(font);
        if(iter == m_mapFontToTextFormat.end())
        {
            HRESULT hr = S_OK;
            CComPtr<IDWriteTextFormat> TextFormat; 
            if(FAILED( hr = m_pDWriteFactory->CreateTextFormat(
                ToCWSTR(font.GetFontFamilyName()),
                NULL,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                font.GetFontSize(),
                L"", //locale
                &TextFormat
                )))
            {
                THROW_EXCEPTION(FontD2DException()<<UIErrorStr(_T("Create Font Error"))<<UIErrorHr(hr));
            }
            TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            m_mapFontToTextFormat[font]=TextFormat;
            return TextFormat;
        }
        else
            return iter->second;
    }

    void RenderD2D::DrawSolidText(HWND hWnd,const YString& strText,const FontD2D& font,const YYRECT &rc,const YYCOLOR color)
    {
        auto & rt=GetHwndRenderTarget(hWnd)->GetRenderTarget();
        auto & textFormat = GetTextureFormat(font);
        auto & brush = GetHwndRenderTarget(hWnd)->GetSolidColorBrush(color);
        rt->DrawText(ToCWSTR(strText),CWSTRLength(strText),textFormat,rc,brush);
    }

    bool RenderD2D::BeginDraw(HWND hWnd)
    {
        return GetHwndRenderTarget(hWnd)->BeginDraw();
    }

    void RenderD2D::EndDraw(HWND hWnd)
    {
        GetHwndRenderTarget(hWnd)->EndDraw();
    }

    void RenderD2D::Clear(HWND hWnd,const YYCOLOR &color)
    {
        //auto &rt = GetHwndRenderTarget(hWnd)->GetRenderTarget();
        //rt->Clear(color);
        (*GetHwndRenderTarget(hWnd))->Clear(color);
    }

    YUI::YYSIZE RenderD2D::GetDrawSize(HWND hWnd)
    {
        return (*GetHwndRenderTarget(hWnd))->GetSize();
    }

    CComPtr<ID2D1Bitmap> RenderD2D::GetBitmap(HWND hwnd,const YString &uri,UINT destinationWidth/*= 0*/,UINT destinationHeight /*= 0*/)
    {
        return GetHwndRenderTarget(hwnd)->GetBitmap(uri,destinationWidth,destinationHeight);
    }

    void RenderD2D::DrawBitmap(HWND hWnd,const YString &strBmp,const YYRECT &rc)
    {
        return GetHwndRenderTarget(hWnd)->DrawBitmap(strBmp,rc);
    }

    void RenderD2D::DrawLine(HWND hWnd,YYPOINT start,YYPOINT end,const YYCOLOR &brush,float strokewidth/*=1.0f*/)
    {
        return GetHwndRenderTarget(hWnd)->DrawLine(start,end,brush,strokewidth);
    }

    void RenderD2D::DrawRect(HWND hWnd,const YYRECT rc,const YYCOLOR &brush,float strokewidth/*=1.0f*/)
    {
        return GetHwndRenderTarget(hWnd)->DrawRect(rc,brush,strokewidth);
    }

    void RenderD2D::FillRect(HWND hWnd,const YYRECT rc,const YYCOLOR &brush)
    {
         return GetHwndRenderTarget(hWnd)->FillRect(rc,brush);
    }

    void RenderD2D::ClipRect(HWND hwnd,ClipRegionDef &clip)
    {
        return GetHwndRenderTarget(hwnd)->ClipRect(clip);
    }

    CComPtr<IDWriteFactory> RenderD2D::m_pDWriteFactory;

    CComPtr<IWICImagingFactory> RenderD2D::m_pWICFactory;

    CComPtr<ID2D1Factory> RenderD2D::m_pD2DFactory;

#if 0 
    CComPtr<ID2D1RenderTarget> RenderD2D::CreateDirect2DDC()
    {
        assert(0 && "not support for win8 app");
        // This flag adds support for surfaces with a different color channel ordering than the API default.
        // You need it for compatibility with Direct2D.
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        // This array defines the set of DirectX hardware feature levels this app  supports.
        // The ordering is important and you should  preserve it.
        // Don't forget to declare your app's minimum required feature level in its
        // description.  All apps are assumed to support 9.1 unless otherwise stated.
        D3D_FEATURE_LEVEL featureLevels[] = 
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };
        // Create the DX11 API device object, and get a corresponding context.
        CComPtr<ID3D11Device> device;
        CComPtr<ID3D11DeviceContext> context;
        HRESULT hr= S_OK;
        if(FAILED( hr= 
            D3D11CreateDevice(
            nullptr,                    // specify null to use the default adapter
            D3D_DRIVER_TYPE_HARDWARE,
            0,                          
            creationFlags,              // optionally set debug and Direct2D compatibility flags
            featureLevels,              // list of feature levels this app can support
            ARRAYSIZE(featureLevels),   // number of possible feature levels
            D3D11_SDK_VERSION,          
            &device,                    // returns the Direct3D device created
            &m_featureLevel,            // returns feature level of device created
            &context                    // returns the device immediate context
            )))
        {
            THROW_EXCEPTION(Render2DException()<<UIErrorStr("Create D3D11 device failed!")<<UIErrorHr(hr));
        }

        CComPtr<IDXGIDevice> dxgiDevice;
        // Obtain the underlying DXGI device of the Direct3D11 device.
        if(FAILED(hr=device->QueryInterface(__uuidof(IDXGIDevice),(void**)&dxgiDevice)))
        {
            THROW_EXCEPTION(Render2DException()<<UIErrorStr("Query IDXGIDevice failed")<<UIErrorHr(hr));
        }
         

        // Obtain the Direct2D device for 2-D rendering.
        D2D1_CREATION_PROPERTIES props;
        props.debugLevel = D2D1_DEBUG_LEVEL_NONE;
        props.options = D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;
        props.threadingMode = D2D1_THREADING_MODE_MULTI_THREADED;
        m_d2dDevice = nullptr;
        if(FAILED(hr = D2D1CreateDevice(dxgiDevice.p, props,&m_d2dDevice)))
        {
           THROW_EXCEPTION(Render2DException()<<UIErrorStr("Create D2D1 device failed")<<UIErrorHr(hr)); 
        }

        // Get Direct2D device's corresponding device context object.
        if(FAILED(hr = m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,&m_d2dDC)) )
        {
           THROW_EXCEPTION(Render2DException()<<UIErrorStr("Create D2D1 device context failed")<<UIErrorHr(hr)); 
        }
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
        swapChainDesc.Width = 0;                           // use automatic sizing
        swapChainDesc.Height = 0;
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // this is the most common swapchain format
        swapChainDesc.Stereo = false; 
        swapChainDesc.SampleDesc.Count = 1;                // don't use multi-sampling
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;                     // use double buffering to enable flip
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // all apps must use this SwapEffect
        swapChainDesc.Flags = 0;
        
        CComPtr<IDXGIAdapter> dxgiAdpater;
        if(FAILED(hr=dxgiDevice->GetParent(__uuidof(IDXGIAdapter),(void**)&dxgiAdpater)))
        {
            THROW_EXCEPTION(Render2DException()<<UIErrorStr("Query IDXGAdapter failed")<<UIErrorHr(hr));
        }
        CComPtr<IDXGIFactory2> dxgiFactory;
        if(FAILED(hr=dxgiAdpater->GetParent(__uuidof(IDXGIFactory),(void**)&dxgiFactory)))
        {
            THROW_EXCEPTION(Render2DException()<<UIErrorStr("Query IDXGFactory failed")<<UIErrorHr(hr));
        }
        //if(FAILED(hr= dxgiFactory->CreateSwapChainForCoreWindow(device,)))
    }
#endif


    RenderTargetHWND::RenderTargetHWND(CComPtr<ID2D1HwndRenderTarget>& rt):m_rt(rt)
    {
    }

    CComPtr<ID2D1SolidColorBrush> RenderTargetHWND::GetSolidColorBrush(const ColorBrushD2D& brush)
    {
        auto iter = m_mapColorBrush.find(brush);
        if(iter!= m_mapColorBrush.end())
            return iter->second;
        HRESULT hr = S_OK;
        CComPtr<ID2D1SolidColorBrush> brushTmp;
        assert(m_rt && "render target is null");
        if(FAILED(hr= m_rt->CreateSolidColorBrush(
            D2D1::ColorF(brush.GetColor()),
            &brushTmp
            )))
        {
            THROW_EXCEPTION(ColorBrushD2DException()<<UIErrorStr(_T("Create Brush Failed"))<<UIErrorHr(hr));
        }
        m_mapColorBrush[brush] = brushTmp;
        return brushTmp;
    }

    CComPtr<ID2D1SolidColorBrush> RenderTargetHWND::GetSolidColorBrush(const YYCOLOR& color)
    {
        return GetSolidColorBrush(ColorBrushD2D(color));
    }

    CComPtr<ID2D1Bitmap> RenderTargetHWND::GetBitmap(const TextureD2D & texture)
    {
        auto iter = m_mapBitmap.find(texture);
        if(iter!= m_mapBitmap.end())
            return iter->second;
        return GetBitmap(texture.GetPath());

    }

    CComPtr<ID2D1Bitmap> RenderTargetHWND::GetBitmap(const YString &uri,UINT desWidth/*= 0*/,UINT desHeight /*= 0*/)
    {
        TextureD2D tex(uri);
        auto iter = m_mapBitmap.find(tex);
        if(iter!= m_mapBitmap.end())
            return iter->second;
        
        HRESULT hr = S_OK;
        CComPtr<ID2D1Bitmap>    pBitmap;
        CComPtr<IWICBitmapDecoder> pDecoder;
        CComPtr<IWICBitmapFrameDecode> pSource;
        CComPtr<IWICStream> pStream ;
        CComPtr<IWICFormatConverter> pConverter ;
        CComPtr<IWICBitmapScaler> pScaler;
        if(FAILED(hr = RenderD2D::GetWICFactory()->CreateDecoderFromFilename(
            ToCWSTR(uri),
            NULL,
            GENERIC_READ,
            WICDecodeMetadataCacheOnLoad,
            &pDecoder
            )))
        {
            THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Create Picture Decoder[")+uri+_T("]  failed!"))<<UIErrorHr(hr));
        }

        if(FAILED(hr = pDecoder->GetFrame(0, &pSource)))
        {
            THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Create Picture Frame0[")+uri+_T("]  failed!"))<<UIErrorHr(hr));
        }
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        if(FAILED(hr = RenderD2D::GetWICFactory()->CreateFormatConverter(&pConverter)))
        {
            THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Convert Picture [")+uri+_T("]  failed!"))<<UIErrorHr(hr)); 
        }

        // If a new width or height was specified, create an
        // IWICBitmapScaler and use it to resize the image.
        if (desWidth != 0 || desHeight != 0)
        {
            UINT originalWidth, originalHeight;
            if(FAILED(hr = pSource->GetSize(&originalWidth, &originalHeight)))
            {
                THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Get Picture [")+uri+_T("] size failed!"))<<UIErrorHr(hr)); 
            }
            if (desWidth == 0)
            {
                FLOAT scalar = static_cast<FLOAT>(desHeight) / static_cast<FLOAT>(originalHeight);
                desWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
            }
            else if (desHeight == 0)
            {
                FLOAT scalar = static_cast<FLOAT>(desWidth) / static_cast<FLOAT>(originalWidth);
                desHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
            }

            if(FAILED(hr = RenderD2D::GetWICFactory()->CreateBitmapScaler(&pScaler)))
            {
                THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Create bitmapScalar [")+uri+_T("] failed!"))<<UIErrorHr(hr)); 
            }
            if(FAILED  (hr = pScaler->Initialize(
                pSource,
                desWidth,
                desHeight,
                WICBitmapInterpolationModeCubic
                )))
            {
                THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Init bitmap scalar [")+uri+_T("] failed!"))<<UIErrorHr(hr)); 
            }
            if(FAILED(  hr = pConverter->Initialize(
                pScaler,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
                )))
            {
                THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Init bitmap convert [")+uri+_T("] failed!"))<<UIErrorHr(hr)); 
            }
        }
        else // Don't scale the image.
        {
            if(FAILED(hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppPBGRA,
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
                )))
            {
                THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("Init bitmap convert [")+uri+_T("] failed!"))<<UIErrorHr(hr)); 
            }
        }
        // Create a Direct2D bitmap from the WIC bitmap.
        if(FAILED(hr =m_rt->CreateBitmapFromWicBitmap(
            pConverter,
            NULL,
            &pBitmap
            )))
        {
            THROW_EXCEPTION(Render2DException()<<UIErrorStr(_T("D2D CreateBitmap form wicbitmap  [")+uri+_T("] failed!"))<<UIErrorHr(hr)); 
        }
        m_mapBitmap[tex] = pBitmap;
        return pBitmap;
    }

    bool RenderTargetHWND::BeginDraw()
    {
        if(m_rt->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED)
            return false;

        m_rt->BeginDraw();

        m_rt->SetTransform(D2D1::Matrix3x2F::Identity());
        return true;
    }

    void RenderTargetHWND::EndDraw()
    {
        if(D2DERR_RECREATE_TARGET == m_rt->EndDraw())
            DiscardResource();
    }

    void RenderTargetHWND::DiscardResource()
    {
        m_mapColorBrush.clear();
        m_mapBitmap.clear();
    }

    void RenderTargetHWND::DrawBitmap(const YString &strBmp,const YYRECT &rc)
    {
        auto m_pTexture= GetBitmap(strBmp);
        m_rt->DrawBitmap(m_pTexture,rc);
    }

    void RenderTargetHWND::DrawLine(YYPOINT start,YYPOINT end,const YYCOLOR &brush,float strokewidth)
    {
        m_rt->DrawLine(start,end,GetSolidColorBrush(brush),strokewidth);
    }

    void RenderTargetHWND::DrawRect(const YYRECT rc,const YYCOLOR &brush,float strokewidth/*=1.0f*/)
    {
        m_rt->DrawRectangle(rc,GetSolidColorBrush(brush),strokewidth);
    }

    void RenderTargetHWND::FillRect(const YYRECT rc,const YYCOLOR &brush)
    {
        m_rt->FillRectangle(rc,GetSolidColorBrush(brush));
    }

    void RenderTargetHWND::ClipRect(ClipRegionDef &clip)
    {
        auto rc = clip.GetClipRect();
        m_rt->PushAxisAlignedClip(rc,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        clip.SetDestroy([&](){
            m_rt->PopAxisAlignedClip();
        });
    }

    

}
