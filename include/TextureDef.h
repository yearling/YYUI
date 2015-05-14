#include "YUI.h"

namespace YUI
{
    struct TextureD2DException : YYUIException{};
    class  TextureD2D
    {
        friend class RenderTargetHWND;
    public:
        explicit TextureD2D(const YString & strPath);
        virtual ~TextureD2D();
        YString                         GetPath() const;
    private:
        TextureD2D();
        YString                         m_strPath;
    };
    bool operator<(const TextureD2D& lhs, const TextureD2D & rhs);
    class TextureD2DToHash
    {
    public:
        size_t operator()(const TextureD2D & s)
        {
            std::hash<std::string> stringHash;
            return stringHash(s.GetPath());
        }
    };

    class TextureD2DEqul
    {
    public:
        bool operator() (const TextureD2D& lhs, const TextureD2D & rhs )
        {
            return lhs.GetPath() == rhs.GetPath();
        }
    };
    typedef std::unordered_map<TextureD2D,CComPtr<ID2D1Bitmap>,TextureD2DToHash,TextureD2DEqul> TextureHash;
}