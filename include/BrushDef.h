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

namespace YUI
{
    struct ColorBrushD2DException : YYUIException{};
    class ColorBrushD2D
    {
        friend class RenderTargetHWND;
    public:
      
        explicit ColorBrushD2D(YYCOLOR col):m_color(col){};
        virtual ~ColorBrushD2D();
        YYCOLOR                         GetColor() const { return m_color;}
    private: 
        ColorBrushD2D();
        YYCOLOR                         m_color;
    };
    bool operator<(const ColorBrushD2D &lhs,const ColorBrushD2D &rhs);
    class ColorBrushD2DToHash
    {
    public:
        size_t operator()( const ColorBrushD2D & s)
        {
            return (unsigned int)s.GetColor();
        }
    };
    class ColorBrushD2DToEql
    {
    public:
        bool operator()(const ColorBrushD2D &lhs, const ColorBrushD2D &rhs)
        {
            return  ((unsigned int)lhs.GetColor()) == ((unsigned int)rhs.GetColor());
        }

    };
    typedef std::unordered_map<ColorBrushD2D,CComPtr<ID2D1SolidColorBrush>,ColorBrushD2DToHash,ColorBrushD2DToEql> ColorBrushD2DHash;
}