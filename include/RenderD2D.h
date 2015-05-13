#pragma once
#include "YUI.h"
#include <d2d1.h>
#include <d2d1helper.h>
#include <d2derr.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>
#include <d2d1effects.h>
#include <d2d1effecthelpers.h>
#include <dwrite.h>
#include <wincodec.h>
#include <atlcomcli.h>
#include <d3d11.h>
#include "FontDef.h"
#include "BrushDef.h"
#include <set>
namespace YUI
{
	struct Render2DException: YYUIException{};
    class RenderTargetHWND
    {
    public:
        friend class RenderD2D;
        operator CComPtr<ID2D1HwndRenderTarget> () { return m_rt;}
        operator ID2D1HwndRenderTarget* () { return m_rt.p;}
        CComPtr<ID2D1SolidColorBrush>   GetSolidColorBrush(const ColorBrushD2D& brush);
        CComPtr<ID2D1SolidColorBrush>   GetSolidColorBrush(const YYCOLOR& color);
        CComPtr<ID2D1HwndRenderTarget>  GetRenderTarget() const{ return m_rt;}
        //ID2D1HwndRenderTarget*          operator->() { return m_rt.p;}
    private:
        RenderTargetHWND(CComPtr<ID2D1HwndRenderTarget>& rt);
        CComPtr<ID2D1HwndRenderTarget>  m_rt;
        std::map<ColorBrushD2D, CComPtr<ID2D1SolidColorBrush> >
                                        m_mapColorBrush;
    };
	class RenderD2D
	{
	private:
        friend class Canvas2D;
		RenderD2D(void);
		RenderD2D(const RenderD2D &);
		RenderD2D& operator=(const RenderD2D &);
	public:
		virtual							~RenderD2D(void);

	public:
		void							Resize(HWND hWnd,unsigned int width, unsigned int height);
        void                            BeginDraw(HWND hWnd);
        void                            EndDraw(HWND hWnd);
        void                            Clear(HWND hWnd,const YYCOLOR &color);
        YYSIZE                          GetDrawSize(HWND hWnd);
    public:
        void                            DrawSolidText(HWND hWnd,const YString& strText,const FontD2D& font,const YYRECT &rc,const YYCOLOR color);
	public: /*Get & Set*/
		std::shared_ptr<RenderTargetHWND>	GetHwndRenderTarget(HWND hwnd);
		std::shared_ptr<RenderTargetHWND>	GetHwndRenderTarget(HWND hwnd,unsigned int width, unsigned int height);
	    CComPtr<ID2D1Factory>			GetD2DFactory() const { return m_pD2DFactory; }
		void							SetD2DFactory(CComPtr<ID2D1Factory> val) { m_pD2DFactory = val; }
		CComPtr<IWICImagingFactory>		GetWICFactory() const { return m_pWICFactory; }
		void							SetWICFactory(CComPtr<IWICImagingFactory> val) { m_pWICFactory = val; }
		CComPtr<IDWriteFactory>			GetDWriteFactory() const { return m_pDWriteFactory; }
		void							SetDWriteFactory(CComPtr<IDWriteFactory> val) { m_pDWriteFactory = val; }
        CComPtr<IDWriteTextFormat>      GetTextureFormat(const YString &strFontname,float fFontsize);
        inline CComPtr<IDWriteTextFormat>      GetTextureFormat(const FontD2D &font);
        //CComPtr<ID2D1SolidColorBrush>   GetColorBrush(HWND hWnd,const YYCOLOR &color); 
	private:
		 CComPtr<ID2D1Factory>			m_pD2DFactory;
		 CComPtr<IWICImagingFactory>		m_pWICFactory;
		 CComPtr<IDWriteFactory>			m_pDWriteFactory;
		
		std::map<HWND,std::shared_ptr<RenderTargetHWND>>
										m_mapHwndToTarget;
        std::map<FontD2D,CComPtr<IDWriteTextFormat> > m_mapFontToTextFormat;
        std::map<ColorBrushD2D,CComPtr<ID2D1SolidColorBrush> > m_mapSolidColorBrush;
        D3D_FEATURE_LEVEL               m_featureLevel; // !for win8 app
        CComPtr<ID2D1Device>            m_d2dDevice;    //!for win8 app
        CComPtr<ID2D1DeviceContext>     m_d2dDC;         //!for win8 app
	};

}