#pragma  once
#include "YUI.h"
#include "Container.h"

namespace YUI
{
    class TileLayout : public Container
    {
    public:
        TileLayout();
        virtual void SetAttribute(const std::string &strName, const std::string& strValue);
        virtual LPCTSTR                 GetClass() const;
        virtual std::shared_ptr<ControlUI> QueryInterface(const std::string & strName);
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