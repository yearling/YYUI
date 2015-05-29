#pragma  once
#include "YUI.h"
#include <dwrite.h>
#include <atlcomcli.h>
namespace YUI
{
    struct FontD2DException : YYUIException{};
    class FontD2D
    {
    friend class RenderD2D;
    public:
        FontD2D():m_fFontSize(10.0f){};
        FontD2D(const YString &strFontName, float fFontSize):m_strFontFamilyName(strFontName),m_fFontSize(fFontSize){};
        YUI::YString                    GetFontFamilyName() const { return m_strFontFamilyName; }
        float                           GetFontSize() const { return m_fFontSize; }
    private:
        YString                         m_strFontFamilyName;
        float                           m_fFontSize;
    }; 
    bool operator<(const FontD2D &lhs,const FontD2D &rhs);
    class FontD2DToHash
    {
    public:
        size_t operator()( const FontD2D & s)
        {
            std::hash<YString> s2h;
            std::hash<float> f2h;
            return s2h(s.GetFontFamilyName()) + f2h(s.GetFontSize());
        }
    };
    class FontD2DToEql
    {
    public:
        bool operator()(const FontD2D &lhs, const FontD2D &rhs)
        {
            return (lhs.GetFontFamilyName()==rhs.GetFontFamilyName()) && (lhs.GetFontSize() == rhs.GetFontSize());
        }

    };
    typedef std::unordered_map<FontD2D,CComPtr<IDWriteTextFormat>,FontD2DToHash,FontD2DToEql> FontHash;
}