#include "YUI.h"
#include "MessageSystem.h"
#include "WindowManager.h"
namespace YUI
{


    void MessageSystem::MeesageLoop() throw()
    {
        MSG msg = { 0 };
        while( ::GetMessage(&msg, NULL, 0, 0) ) 
        {
            if( !::TranslateMessage(&msg) )
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            } 

            if( msg.message == WM_QUIT && WindowManger::GetInstance()->GetWindowsNumber()== 0 )
                break;
            /*if( msg.message == WM_CLOSE && WindowManger::GetInstance()->GetWindowsNumber()== 0 )
                break;*/
        }
    }

    MessageSystem::MessageSystem()
    {

    }

    YUI::MessageSystem MessageSystem::m_Instance;

}