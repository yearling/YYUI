#include "YUI.h"
#include "Canvas2D.h"

namespace YUI
{


    Canvas2D::Canvas2D(HWND hWnd):m_hWnd(hWnd)
    {
      
    }

    void Canvas2D::DrawSolidText(const YString& strText,const FontD2D& font,const YYRECT &rc,const YYCOLOR color)
    {
        m_RenderEngine->DrawSolidText(m_hWnd,strText,font,rc,color);
    }

    bool Canvas2D::BeginDraw()
    {
       return m_RenderEngine->BeginDraw(m_hWnd);
    }

    void Canvas2D::EndDraw()
    {
        m_RenderEngine->EndDraw(m_hWnd);
    }

    void Canvas2D::OnResize(float width,float height)
    {
        m_RenderEngine->Resize(m_hWnd,(unsigned int)width,(unsigned int)height );
    }

    void Canvas2D::Clear(const YYCOLOR & color)
    {
        m_RenderEngine->Clear(m_hWnd,color);
    }

    YYSIZE Canvas2D::GetSize()
    {
        return m_RenderEngine->GetDrawSize(m_hWnd);
    }

    void Canvas2D::Init()
    {
       m_RenderEngine.reset(new RenderD2D);
    }

    CComPtr<ID2D1HwndRenderTarget> Canvas2D::GetHWNDRT(HWND hWnd)
    {
        return m_RenderEngine->m_mapHwndToTarget[hWnd]->GetRenderTarget();
    }

    void Canvas2D::DrawBitmap(const YString &strBmp,const YYRECT &rc)
    {
        return m_RenderEngine->DrawBitmap(m_hWnd,strBmp,rc);
    }

    void Canvas2D::DrawLine(YYPOINT start,YYPOINT end,const YYCOLOR &brush,float strokewidth/*=1.0f*/)
    {
        return m_RenderEngine->DrawLine(m_hWnd,start,end,brush,strokewidth);
    }

    void Canvas2D::DrawRect(const YYRECT rc,const YYCOLOR &brush,float strokewidth/*=1.0f*/)
    {
        return m_RenderEngine->DrawRect(m_hWnd,rc,brush,strokewidth);
    }

   

    std::shared_ptr<RenderD2D> Canvas2D::m_RenderEngine;
}