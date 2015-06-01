#include "YUI.h"
#include "WindowManager.h"

namespace YUI
{


    WindowManger::~WindowManger()
    {

    }

    WindowManger::WindowManger()
    {

    }

      LRESULT CALLBACK WindowManger::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        WindowWnd *pWnd = NULL;
        if(uMsg == WM_NCCREATE )
        {
            //窗口刚开始创建时CreateWindowEx，最后一个参数LPVOID lpParam，保存了当前的this指针，这里取出来；
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>( lParam );
            pWnd = static_cast<WindowWnd*>( lpcs->lpCreateParams );
            pWnd->m_hWnd = hWnd;
            //这时把YWindowWnd的指针绑到当前生成的HWND上
            //相当于每个HWND都通过这个指针映射到一个YWinodwWnd的类
            ::SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pWnd) );
            m_Instance.RegisterWindow(hWnd,pWnd);
        }
        else
        {
            pWnd = reinterpret_cast< WindowWnd* >( ::GetWindowLongPtr( hWnd,GWLP_USERDATA ) );
           
            //！！这里用的是WM_NCDESTROY，就是窗口即将消失的时候。这个消息在WM_CLOSE消息被正确处理后触发
            if(uMsg == WM_NCDESTROY && pWnd != NULL )
            {
                //一般调用的是DefWindowProc
                LRESULT lRes = ::CallWindowProc( pWnd->m_OldWndProc, hWnd, uMsg, wParam, lParam );
                //清空hWnd绑到的YWindowWnd
                ::SetWindowLongPtr( hWnd, GWLP_USERDATA , NULL );
                //???
                if( pWnd->m_bSubClassed )
                    pWnd->UnsubClass();
                pWnd->m_hWnd = NULL;
                pWnd->OnFinalMessage(hWnd);
                m_Instance.UnRegisterWindow(hWnd);
                return lRes;
            }
        }
        if( pWnd != NULL )
            return pWnd->OnSysMessage(uMsg,wParam,lParam);
        else
            return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
    }

      void WindowManger::RegisterWindow(HWND hWnd,WindowWnd *pWindow)
      {
          assert((m_mapHWNDToWindow.find(hWnd)== m_mapHWNDToWindow.end())&&"register agian");
          auto iter = m_mapHWNDToWindow.find(hWnd);
          if(iter!=m_mapHWNDToWindow.end())
              THROW_EXCEPTION(WindowMangerExcption()<<UIErrorStr(_T("register window again")));
          m_mapHWNDToWindow[hWnd]=pWindow;
      }

      void WindowManger::UnRegisterWindow(HWND hWnd)
      {
          assert((m_mapHWNDToWindow.find(hWnd)!= m_mapHWNDToWindow.end())&&"can't find window to unregister it ");
          auto iter = m_mapHWNDToWindow.find(hWnd);
          if(iter==m_mapHWNDToWindow.end())
              THROW_EXCEPTION(WindowMangerExcption()<<UIErrorStr(_T("can't find window to unregister it")));
          m_mapHWNDToWindow.erase(iter);
      }

      UINT WindowManger::GetWindowsNumber() const
      {
          return m_mapHWNDToWindow.size();
      }

      const WindowProperty* WindowManger::GetWindowProperty(HWND hWnd) 
      {
          auto iter = m_mapHWNDToWindow.find(hWnd);
          if(iter == m_mapHWNDToWindow.end() )
              return nullptr;
          m_mapHWNDToWindow[hWnd]->GetWindowProperty();
      }

    YUI::WindowManger WindowManger::m_Instance;

}