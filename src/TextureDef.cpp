#include "YUI.h"
#include "TextureDef.h"

namespace YUI
{


    TextureD2D::TextureD2D(const YString & strPath)
    {
        m_strPath = strPath;
    }

    TextureD2D::TextureD2D()
    {

    }

    TextureD2D::~TextureD2D()
    {

    }

    YUI::YString TextureD2D::GetPath() const
    {
        return m_strPath;
    }

    bool operator<(const TextureD2D& lhs, const TextureD2D & rhs)
    {
        return lhs.GetPath()<rhs.GetPath();
    }

};