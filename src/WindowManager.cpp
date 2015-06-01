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
            //���ڸտ�ʼ����ʱCreateWindowEx�����һ������LPVOID lpParam�������˵�ǰ��thisָ�룬����ȡ������
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>( lParam );
            pWnd = static_cast<WindowWnd*>( lpcs->lpCreateParams );
            pWnd->m_hWnd = hWnd;
            //��ʱ��YWindowWnd��ָ��󵽵�ǰ���ɵ�HWND��
            //�൱��ÿ��HWND��ͨ�����ָ��ӳ�䵽һ��YWinodwWnd����
            ::SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pWnd) );
            m_Instance.RegisterWindow(hWnd,pWnd);
        }
        else
        {
            pWnd = reinterpret_cast< WindowWnd* >( ::GetWindowLongPtr( hWnd,GWLP_USERDATA ) );
           
            //���������õ���WM_NCDESTROY�����Ǵ��ڼ�����ʧ��ʱ�������Ϣ��WM_CLOSE��Ϣ����ȷ����󴥷�
            if(uMsg == WM_NCDESTROY && pWnd != NULL )
            {
                //һ����õ���DefWindowProc
                LRESULT lRes = ::CallWindowProc( pWnd->m_OldWndProc, hWnd, uMsg, wParam, lParam );
                //���hWnd�󵽵�YWindowWnd
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