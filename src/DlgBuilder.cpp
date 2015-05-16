
#include "YUI.h"
#include "DlgBuilder.h"
#include "UIUtility.h"
#include "ControlUI.h"
#include "PaintManagerUI.h"
#include "Label.h"
#include "Button.h"

#include <iostream>
#include "Container.h"
#include "VerticalLayout.h"
#include "HorizontalLayout.h"
#include "WindowProperty.h"
using std::cout;
using std::endl;
using std::wcout;
using std::wcin;

using tinyxml2::XMLElement;
using tinyxml2::XMLNode;
using tinyxml2::XMLAttribute;
using tinyxml2::XMLText;


namespace YUI
{


    DialogBuilder::DialogBuilder():
        m_pCallback(NULL)
        ,m_pstrtype(NULL)
    {
        m_pDoc = std::make_shared<tinyxml2::XMLDocument>();
    }


    std::shared_ptr<ControlUI> DialogBuilder::Create(YString xml, WindowProperty & perperty, IDialogBuilderCallback* pCallback, std::weak_ptr<ControlUI> pParent)
    {
        tinyxml2::XMLError err= tinyxml2::XML_SUCCESS;
#if defined UNICODE || defined _UNICODE
        err= m_pDoc->LoadFile(W2S(xml).c_str());
#else
        err= m_pDoc->LoadFile(xml.c_str());
#endif // UNICODE || defined _UNICODE
        if(err!= tinyxml2::XML_SUCCESS)
            THROW_EXCEPTION(YYUIException()<<UIErrorStr(_T("parse xml failed!")));

        m_pCallback  = pCallback;
        return Create(pCallback,perperty,pParent);
    }

    std::shared_ptr<ControlUI> DialogBuilder::Create(IDialogBuilderCallback* pCallback , 
        WindowProperty &Winproperty,std::weak_ptr<ControlUI> pParent)
    {
        m_pCallback = pCallback;
        XMLElement *pRoot = m_pDoc->FirstChildElement();
        const char * pstrClass = nullptr;
        int nAttributes = 0;
        const char * pstrName = nullptr;
        const char * pstrValue = nullptr;
        char * pstr = nullptr;

        //todo 处理一些如default的东西，如font,image
        //处理window头
#pragma region 处理window标签
        pstrClass = pRoot->Name();
        cout<<pstrClass<<endl;
        if(strcmp(pstrClass, "Window") ==0 )
        {
            if(Winproperty.GetHWND())
            {
                char * pstr = nullptr;
                //size属性
                {
                    pstrValue = pRoot->Attribute("size");
                    if(pstrValue)
                    {
                        int cx = strtol(pstrValue,&pstr,10);
                        int cy = strtol(pstr+1,&pstr,10);
                        Winproperty.SetInitSize(cx,cy);
                        cout<<"size: cx:"<<cx<<endl;
                        cout<<"size: cy:"<<cy<<endl;
                    }
                }
                //sizeBox
                {
                    pstrValue = pRoot->Attribute("sizebox");

                    if(pstrValue) 
                    {
                        RECT rcSizeBox = { 0 };
                        pstr = nullptr;
                        rcSizeBox.left = strtol(pstrValue, &pstr, 10);  assert(pstr);    
                        rcSizeBox.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
                        rcSizeBox.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
                        rcSizeBox.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);   
                        Winproperty.SetSizeBox(rcSizeBox);
                    } 
                }
                //caption
                {
                    pstrValue = pRoot->Attribute("caption");
                    if(pstrValue)
                    {
                        RECT rcCaption = { 0 };
                        pstr = NULL;
                        rcCaption.left = strtol(pstrValue, &pstr, 10);  assert(pstr);    
                        rcCaption.top = strtol(pstr + 1, &pstr, 10);    assert(pstr);    
                        rcCaption.right = strtol(pstr + 1, &pstr, 10);  assert(pstr);    
                        rcCaption.bottom = strtol(pstr + 1, &pstr, 10); assert(pstr);    
                        Winproperty.SetCaptionRect(rcCaption);
                    }
                }
                //roundcorner
                {
                    if( pstrValue = pRoot->Attribute("roundcorner") )
                    {
                        int cx = strtol(pstrValue, &pstr, 10);  assert(pstr);    
                        int cy = strtol(pstr + 1, &pstr, 10);    assert(pstr); 
                        //pManager->SetRoundCorner(cx, cy); 
                    }
                }
                //mininfo
                {
                    if( pstrValue = pRoot->Attribute("mininfo") )
                    {
                        int cx = strtol(pstrValue, &pstr, 10);  assert(pstr);    
                        int cy = strtol(pstr + 1, &pstr, 10);    assert(pstr); 
                        Winproperty.SetMinInfo(cx, cy);
                    }
                }
                //maxinfo
                {
                    if( pstrValue = pRoot->Attribute("maxinfo") )
                    {
                        int cx = strtol(pstrValue, &pstr, 10);  assert(pstr);    
                        int cy = strtol(pstr + 1, &pstr, 10);    assert(pstr); 
                        Winproperty.SetMinInfo(cx, cy);
                    }
                }
                //showdirty
                {
                    if( pstrValue = pRoot->Attribute("showdirty") )
                    {
                       // pManager->SetShowUpdateRect(strcmp(pstrValue,"true")== 0);
                    }
                }
                //alpha
                {
                    if( pstrValue = pRoot->Attribute("alpha"))
                    {
                        Winproperty.SetTransparent(((float)atoi(pstrValue))/255.0f);
                    }
                }
                //bktrans
                {
                    if( pstrValue = pRoot->Attribute("bktrans") )
                    {
                        Winproperty.SetBackgroundTransparent(strcmp(pstrValue, "true") == 0);
                    }
                }
                //disablefontcolor
                {
                    if(pstrValue = pRoot->Attribute("disabledfontcolor") )
                    {
                        if( *pstrValue == '#') pstrValue = ::CharNextA(pstrValue);
                        LPSTR pstr = NULL;
                        DWORD clrColor = strtoul(pstrValue, &pstr, 16);
                        Winproperty.SetDefaultDisabledColor(clrColor);
                    }
                }
                //defaultFontColor
                {
                    if(pstrValue = pRoot->Attribute("defaultfontcolor"))
                    {
                        if( *pstrValue == '#') pstrValue = ::CharNextA(pstrValue);
                        DWORD clrColor = strtoul(pstrValue, &pstr, 16);
                        Winproperty.SetDefaultFontColor(clrColor);
                    }
                }
                //linkFontColor
                {
                    if(pstrValue = pRoot->Attribute("linkfontcolor"))
                    {
                        if( *pstrValue == '#') pstrValue = ::CharNextA(pstrValue);
                        DWORD clrColor = strtoul(pstrValue, &pstr, 16);
                        Winproperty.SetDefaultLinkFontColor(clrColor);
                    }
                }
                //linkhoverfontcolor
                {
                    if(pstrValue = pRoot->Attribute("linkhoverfontcolor"))
                    {
                        if( *pstrValue == '#') pstrValue = ::CharNextA(pstrValue);
                        DWORD clrColor = strtoul(pstrValue, &pstr, 16);
                        Winproperty.SetDefaultLinkHoverFontColor(clrColor);
                    }
                }
                //selectedcolor
                {
                    if(pstrValue = pRoot->Attribute("linkhoverfontcolor"))
                    {
                        if( *pstrValue == '#') pstrValue = ::CharNextA(pstrValue);
                        DWORD clrColor = strtoul(pstrValue, &pstr, 16);
                        Winproperty.SetDefaultSelectedBkColor(clrColor);
                    }
                }
            }
        }
#pragma endregion

        return Parse(pRoot,pParent);
    }


    std::shared_ptr<ControlUI> DialogBuilder::Parse(tinyxml2::XMLNode* pRoot, std::weak_ptr<ControlUI> pParent)
    {
        //ContainerUI* pContainer = NULL;
        std::shared_ptr<ControlUI> pReturn;
        for( XMLElement *pChild = pRoot->FirstChildElement();pChild;pChild= pChild->NextSiblingElement())
        {
            const char *pstrClass = pChild->Name();
            if(strcmp(pstrClass,"Image") == 0 ||
                strcmp(pstrClass,"Font") == 0 ||
                strcmp(pstrClass,"Default") == 0 )
            {
                continue;
            }
            std::shared_ptr<ControlUI> pControl;
            if( strcmp(pstrClass,"Include")==0)
            {
                assert(0 && "no implement");
            }
            else if( strcmp(pstrClass, "TreeNode") == 0)
            {
                assert(0 && "no implement");
            }
            else
            {
                if(strcmp(pstrClass,CTR_LABEL)== 0)
                    pControl = std::make_shared<Label>();
                if(strcmp(pstrClass,CTR_BUTTON)== 0)
                    pControl = std::make_shared<Button>();
                else if(strcmp(pstrClass,CTR_CONTAINER) == 0)
                    pControl = std::make_shared<Container>();
                else if(strcmp(pstrClass,CTR_VERTICALLAYOUT) == 0)
                    pControl = std::make_shared<VerticalLayout>();
                else if(strcmp(pstrClass,CTR_HORIZONTALLAYOUT) == 0)
                    pControl = std::make_shared<HorizontalLayout>();
                else if(strcmp(pstrClass,CTR_CONTROL) == 0)
                    pControl = std::make_shared<ControlUI>();
                else if(strcmp(pstrClass,CTR_CONTAINER) == 0)
                    pControl = std::make_shared<Container>();
                //if( !pControl )
                //{
                //	//load from plugin;
                //	//not implement;
                //                cout<<pstrClass<<endl;
                //	assert( 0 && "not implement");
                //}
                if( pControl == NULL && m_pCallback != NULL ) 
                {
                    pControl = m_pCallback->CreateControl(pstrClass);
                }
                assert(pControl);
                if( !pChild->NoChildren() )
                {
                    Parse(pChild,pControl);
                }
                //attach to parent
                auto spParent = pParent.lock();
                if(spParent)
                {
                    //std::shared_ptr<IContainer> pContianer = static_cast<std::shared_ptr<IContainer>>(spParent->QueryInterface(_T("IContainer")));
                    std::shared_ptr<IContainer> pContianer = std::dynamic_pointer_cast<IContainer >(spParent);
                    if(!pContianer)
                        return nullptr;
                    pContianer->Add(pControl);
                }
                for(const XMLAttribute *pAttribue = pChild->FirstAttribute();pAttribue;pAttribue= pAttribue->Next())
                {
                    pControl->SetAttribute(pAttribue->Name(),pAttribue->Value());
                }
            }
            if( pReturn == nullptr)
            {
                pReturn = pControl;
            }
        }

        return pReturn;
    }

}