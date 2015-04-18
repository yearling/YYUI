#pragma once
#include "YUI.h"
#include "PaintManagerUI.h"

namespace YUI
{
    class RenderClip
    {
    };

    class RenderGDI
    {
    public:
      static bool                       DrawImageString(HDC hDC, 
                                        std::shared_ptr<PaintManagerUI>& pManager, 
                                        const RECT& rcItem, 
                                        const RECT& rcPaint, 
                                        const YString & StrImage, 
                                        const YString & pStrModify = _T(""));
    };
}