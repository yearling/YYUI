#include "YUI.h"
#include "FontDef.h"
#include "RenderD2D.h"
#include "UIUtility.h"

namespace YUI
{

    bool operator<(const FontD2D &lhs,const FontD2D &rhs)
    {
        if(lhs.GetFontFamilyName()  < rhs.GetFontFamilyName())
            return true;
        if(lhs.GetFontFamilyName() > rhs.GetFontFamilyName() )
            return false;
        if(lhs.GetFontSize() < rhs.GetFontSize() )
            return true;
        if(lhs.GetFontSize() > rhs.GetFontSize() )
            return false;
        return false;
    }

}


