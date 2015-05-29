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
#include "TextureDef.h"
#include <set>
#include "ClipRegionDef.h"
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
        CComPtr<ID2D1Bitmap>            GetBitmap(const TextureD2D & texture);
        CComPtr<ID2D1Bitmap>            GetBitmap(const YString &uri,UINT desWidth= 0,UINT desHeight = 0);
        ID2D1HwndRenderTarget*          operator->() { return m_rt.p;}
        void                            DiscardResource();
        bool                            BeginDraw();
        void                            EndDraw();
        void                            DrawBitmap(const YString &strBmp,const YYRECT &rc);
        void                            DrawBitmap(const YString &strBmp,const YYRECT &desRC,const YYRECT &srcRC,float alpha=1.0f);
        void                            DrawLine(YYPOINT start,YYPOINT end,const YYCOLOR &brush,float strokewidth=1.0f);
        void                            DrawRect(const YYRECT rc,const YYCOLOR &brush,float strokewidth=1.0f);
        void                            FillRect(const YYRECT rc,const YYCOLOR &brush);
        void                            ClipRect(ClipRegionDef &clip);
    private:
        RenderTargetHWND(CComPtr<ID2D1HwndRenderTarget>& rt);
        CComPtr<ID2D1HwndRenderTarget>  m_rt;
        std::map<ColorBrushD2D, CComPtr<ID2D1SolidColorBrush> >
                                        m_mapColorBrush;
        std::map<TextureD2D,CComPtr<ID2D1Bitmap> >
                                        m_mapBitmap;
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
        bool                            BeginDraw(HWND hWnd);
        void                            EndDraw(HWND hWnd);
        void                            Clear(HWND hWnd,const YYCOLOR &color);
        YYSIZE                          GetDrawSize(HWND hWnd);
    public:
        void                            DrawSolidText(HWND hWnd,const YString& strText,const FontD2D& font,const YYRECT &rc,const YYCOLOR color);
        void                            DrawBitmap(HWND hWnd,const YString &strBmp,const YYRECT &rc);
        void                            DrawBitmap(HWND hWnd,const YString &strBmp,const YYRECT &desRC,const YYRECT &srcRC,float alpha=1.0f);
        void                            DrawLine(HWND hWnd,YYPOINT start,YYPOINT end,const YYCOLOR &brush,float strokewidth=1.0f);
        void                            DrawRect(HWND hWnd,const YYRECT rc,const YYCOLOR &brush,float strokewidth=1.0f);
        void                            FillRect(HWND hWnd,const YYRECT rc,const YYCOLOR &brush);
        void                            ClipRect(HWND hwnd,ClipRegionDef &clip);
	public: /*Get & Set*/
		std::shared_ptr<RenderTargetHWND>	GetHwndRenderTarget(HWND hwnd);
		std::shared_ptr<RenderTargetHWND>	GetHwndRenderTarget(HWND hwnd,unsigned int width, unsigned int height);
	    static CComPtr<ID2D1Factory>			GetD2DFactory()  { return m_pD2DFactory; }
		static CComPtr<IWICImagingFactory>		GetWICFactory()  { return m_pWICFactory; }
		static CComPtr<IDWriteFactory>			GetDWriteFactory()  { return m_pDWriteFactory; }
        CComPtr<IDWriteTextFormat>              GetTextureFormat(const YString &strFontname,float fFontsize);
        inline CComPtr<IDWriteTextFormat>       GetTextureFormat(const FontD2D &font);

    //生成纹理资源
    public: 
        CComPtr<ID2D1Bitmap>            GetBitmap(HWND hwnd,const YString &uri,UINT desWidth= 0,UINT desHeight = 0);
	private:
	static	 CComPtr<ID2D1Factory>			m_pD2DFactory;
	static	 CComPtr<IWICImagingFactory>    m_pWICFactory;
	static	 CComPtr<IDWriteFactory>	    m_pDWriteFactory;
		
		std::map<HWND,std::shared_ptr<RenderTargetHWND>>
										m_mapHwndToTarget;
        std::map<FontD2D,CComPtr<IDWriteTextFormat> > m_mapFontToTextFormat;
        std::map<ColorBrushD2D,CComPtr<ID2D1SolidColorBrush> > m_mapSolidColorBrush;
        D3D_FEATURE_LEVEL               m_featureLevel; // !for win8 app
        CComPtr<ID2D1Device>            m_d2dDevice;    //!for win8 app
        CComPtr<ID2D1DeviceContext>     m_d2dDC;         //!for win8 app
	};

}