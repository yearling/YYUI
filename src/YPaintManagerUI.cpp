//////////////////////////////////////////////////////////////////////////
// Create by yyCom, 2015/4/10
//////////////////////////////////////////////////////////////////////////

#include "YYUI.h"
#include "YPaintManagerUI.h"
#include "YYUtility.h"
#include <commctrl.h>
#pragma comment(lib,"comctl32.lib")
namespace YYCOM
{

void YYCOM::YPaintManagerUI::SetInstance(HINSTANCE hInst)
{
    m_hInstance = hInst;
}
HPEN g_hUpdateRectPen = NULL;

YYCOM::YPaintManagerUI::YPaintManagerUI():
    m_hWndPaint( NULL )
   ,m_hDCPaint( NULL)
   ,m_hDCOffscreen( NULL )
   ,m_hDCBackground( NULL )
   ,m_hbmpOffscreen( NULL )
   ,m_hbmpBackground( NULL )
   ,m_hwndTooltip( NULL )
   ,m_bShowUpdateRect( false )
   ,m_uTimerID( 0x1000 )
   ,m_bFirstLayout( true )
   ,m_bFocusNeeded( false )
   ,m_bUpdateNeeded( false )
   ,m_bMouseTracking( false )
   ,m_bMouseCapture( false )
   ,m_bOffscreenPaint( true )
   ,m_bAlphaBackGround( false )
   ,m_bUsedVirtualWnd( true )
   ,m_nOpacity( 255 )
   ,m_dwDefaultDisabledColor( 0xFFA7A6AA)
   ,m_dwDefaultFontColor( 0xFF000000 )
   ,m_dwDefaultLinkFontColor( 0xFF0000FF )
   ,m_dwDefaultLinkHoverFontColor( 0xFFD3215F )
   ,m_dwDefaultSelectedBKColor( 0xFFBAE4FF )
{
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT),sizeof(lf),&lf);
    lf.lfCharSet = DEFAULT_CHARSET;
    if( !YPaintManagerUI::m_strDefaultFontName.empty() )
    {
#if defined UNICODE || defined _UNICODE
        _tcscpy_s(lf.lfFaceName, LF_FACESIZE, Ansi2Wchar(m_strDefaultFontName.c_str()).c_str());
#else 
        _tcscpy_s(lf.lfFaceName, LF_FACESIZE, m_strDefaultFontName.c_str());
#endif
        HFONT hDefaultFont = ::CreateFontIndirect( &lf );
        m_DefaultFontInfo.m_hFont = hDefaultFont;
#if defined UNICODE || defined _UNICODE
        m_DefaultFontInfo.m_strFontName = WChar2Ansi(lf.lfFaceName);
#else 
        m_DefaultFontInfo.m_strFontName = lf.lfFaceName;
#endif
        m_DefaultFontInfo.m_nSize = -lf.lfHeight;
        m_DefaultFontInfo.m_bBold = (lf.lfWeight >= FW_BOLD );
        m_DefaultFontInfo.m_bUnderline = ( lf.lfUnderline == TRUE );
        m_DefaultFontInfo.m_bItalic = (lf.lfItalic == TRUE );
        memset( &m_DefaultFontInfo.m_tm,0,sizeof(m_DefaultFontInfo.m_tm) );

        if(  g_hUpdateRectPen == NULL )
        {
            //指示哪些地方在被更新，调试用
            g_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
            // Boot Windows Common Controls (for the ToolTip control)
            ::InitCommonControls();
            ::LoadLibrary(_T("msimg32.dll"));
        }

        m_szMinWindow.cx = 0;
        m_szMinWindow.cy = 0;
        m_szMaxWindow.cx = 0;
        m_szMaxWindow.cy = 0;
        m_szInitWindowSize.cx = 0;
        m_szInitWindowSize.cy = 0;
        m_szRoundCorner.cx = m_szRoundCorner.cy = 0;
        ZeroMemory(&m_rcSizeBox, sizeof(m_rcSizeBox));
        ZeroMemory(&m_rcCaption, sizeof(m_rcCaption));
        m_ptLastMousePos.x = m_ptLastMousePos.y = -1;
    }
}

YYCOM::YPaintManagerUI::~YPaintManagerUI()
{
    //!! todo
}


bool YYCOM::YPaintManagerUI::TranslateMessage(const LPMSG pMsg)
{
    return false;
}
HINSTANCE YYCOM::YPaintManagerUI::m_hInstance = NULL;

std::vector<void*> YYCOM::YPaintManagerUI::m_vecTranslateAccelerator;

std::string YYCOM::YPaintManagerUI::m_strDefaultFontName;

std::vector<void*> YYCOM::YPaintManagerUI::m_vecPlugins;


short YYCOM::YPaintManagerUI::m_L       = 180;

short YYCOM::YPaintManagerUI::m_S       = 180;

short YYCOM::YPaintManagerUI::m_H       = 180;

HANDLE YYCOM::YPaintManagerUI::m_hResourceZip = NULL;

bool YYCOM::YPaintManagerUI::m_bCachedResourceZip = false;

std::string YYCOM::YPaintManagerUI::m_strResourceZip;

std::string YYCOM::YPaintManagerUI::m_strResourcePath;

HINSTANCE YYCOM::YPaintManagerUI::m_hResourceInstance = NULL;


std::vector< std::weak_ptr<YPaintManagerUI> > YPaintManagerUI::g_vecPreMessages;



void YPaintManagerUI::Init(HWND hWnd)
{
    assert( ::IsWindow(hWnd ) && "hwnd 不是个有效值");

    m_hWndPaint = hWnd;
    m_hDCPaint = ::GetDC(hWnd);
    g_vecPreMessages.push_back(this->shared_from_this());
}

std::string YPaintManagerUI::GetInstancePath()
{
    if( m_hInstance == NULL )
        return '\0';
    TCHAR szModule[MAX_PATH + 1] = { 0 };
    ::GetModuleFileName(m_hInstance,szModule,MAX_PATH);
}

HINSTANCE YYCOM::YPaintManagerUI::GetInstance()
{
    return m_hInstance;
}

}
