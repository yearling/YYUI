#pragma once
#include "YUI.h"

namespace YUI
{

    class WindowProperty
    {
    public:
        WindowProperty(void);
        ~WindowProperty(void);
        void                            Init(HWND hWnd) { m_hWnd = hWnd;}
    public:

        YYPOINT                         GetMousePos() const;
        YYSIZE                          GetClientSize() const;
        YYRECT                          GetClientRect() const;
        YYSIZE                          GetInitSize() const;
        void                            SetInitSize(int cx, int cy);
        YYRECT                          GetSizeBox() const ;
        void                            SetSizeBox(const RECT &rcSizeBox);
        YYRECT                          GetCaptionRect() const ;
        void                            SetCaptionRect( const RECT & rcCaption);
        YYSIZE                          GetMinInfo() const;
        void                            SetMinInfo(int cx, int cy);
        YYSIZE                          GetMaxInfo() const;
        void                            SetMaxInfo(int cx, int cy);
        int                             GetTransparent() const;
        void                            SetTransparent(float nOpacity);
        void                            SetBackgroundTransparent(bool bTrans);
    private:
        HWND                            m_hWnd;
        YYPOINT                         m_ptLastMousePos;
        YYSIZE                          m_szMinWindow;      //������Сֵ
        YYSIZE                          m_szMaxWindow;
        YYSIZE                          m_szInitWindowSize;
        YYRECT                          m_rcSizeBox;  //SizeBox���Ǳ߿�ָʾ�����������������������ڴ�С
        YYSIZE                          m_szRoundCorner;
        YYRECT                          m_rcCaption;  //��������С
        float                           m_fAlpha;
        bool                            m_bAlphaBackGround;
    };


}