#pragma  once
#include "YUI.h"
#include "Container.h"

namespace YUI
{
    class TileLayout : public Container
    {
    public:
        TileLayout();
        virtual void                    SetAttribute(const YString &pstrName, const YString& pstrValue);
        virtual LPCTSTR                 GetClass() const;
        virtual std::shared_ptr<ControlUI>                  
            GetInterface(const YString & strName);
        virtual void                    SetPos(RECT rc);
        SIZE                            GetItemSize() const;
        void                            SetItemSize(SIZE szItem);
        int                             GetColumns() const;
        void                            SetColumns(int nCols);
    protected:
        SIZE                            m_szItem;
        int                             m_nColumns;
    };
}