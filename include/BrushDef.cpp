#include "YUI.h"
#include "BrushDef.h"

namespace YUI
{


    ColorBrushD2D::ColorBrushD2D()
    {

    }

    ColorBrushD2D::~ColorBrushD2D()
    {

    }

    bool operator<(const ColorBrushD2D &lhs,const ColorBrushD2D &rhs)
    {
        return (unsigned int )lhs.GetColor() < (unsigned int)rhs.GetColor();
    }

}

