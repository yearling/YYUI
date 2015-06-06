
#include "YUI.h"
#include "DlgBuilder.h"
#include "UIUtility.h"
#include "ControlUI.h"
#include "Label.h"
#include "Button.h"
#include "ProgressBar.h"

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


    CountRefPtr<ControlUI> DialogBuilder::Create(YString xml, WindowProperty & perperty, IDialogBuilderCallback* pCallback, ControlUI* pParent)
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

    CountRefPtr<ControlUI> DialogBuilder::Create(IDialogBuilderCallback* pCallback , 
        WindowProperty &Winproperty,ControlUI* pParent)
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
                        float cx = (float)strtol(pstrValue,&pstr,10);
                        float cy = (float)strtol(pstr+1,&pstr,10);
                        Winproperty.SetInitSize(cx,cy);
                    }
                }
                //sizeBox
                {
                    pstrValue = pRoot->Attribute("sizebox");

                    if(pstrValue) 
                    {
                        YYRECT rcSizeBox;
                        pstr = nullptr;
                        rcSizeBox.left = (float)strtol(pstrValue, &pstr, 10);  assert(pstr);    
                        rcSizeBox.top = (float)strtol(pstr + 1, &pstr, 10);    assert(pstr);    
                        rcSizeBox.right = (float)strtol(pstr + 1, &pstr, 10);  assert(pstr);    
                        rcSizeBox.bottom = (float)strtol(pstr + 1, &pstr, 10); assert(pstr);   
                        Winproperty.SetSizeBox(rcSizeBox);
                    } 
                }
                //caption
                {
                    pstrValue = pRoot->Attribute("caption");
                    if(pstrValue)
                    {
                        YYRECT rcCaption;
                        pstr = NULL;
                        rcCaption.left = (float)strtol(pstrValue, &pstr, 10);  assert(pstr);    
                        rcCaption.top = (float)strtol(pstr + 1, &pstr, 10);    assert(pstr);    
                        rcCaption.right = (float)strtol(pstr + 1, &pstr, 10);  assert(pstr);    
                        rcCaption.bottom = (float)strtol(pstr + 1, &pstr, 10); assert(pstr);    
                        Winproperty.SetCaptionRect(rcCaption);
                    }
                }
                //roundcorner
                {
                    if( pstrValue = pRoot->Attribute("roundcorner") )
                    {
                        float cx = (float)strtol(pstrValue, &pstr, 10);  assert(pstr);    
                        float cy = (float)strtol(pstr + 1, &pstr, 10);    assert(pstr); 
                        //pManager->SetRoundCorner(cx, cy); 
                    }
                }
                //mininfo
                {
                    if( pstrValue = pRoot->Attribute("mininfo") )
                    {
                        float cx = (float)strtol(pstrValue, &pstr, 10);  assert(pstr);    
                        float cy = (float)strtol(pstr + 1, &pstr, 10);    assert(pstr); 
                        Winproperty.SetMinInfo(cx, cy);
                    }
                }
                //maxinfo
                {
                    if( pstrValue = pRoot->Attribute("maxinfo") )
                    {
                        float cx = (float)strtol(pstrValue, &pstr, 10);  assert(pstr);    
                        float cy = (float)strtol(pstr + 1, &pstr, 10);    assert(pstr); 
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


    CountRefPtr<ControlUI> DialogBuilder::Parse(tinyxml2::XMLNode* pRoot, ControlUI* pParent)
    {
        //ContainerUI* pContainer = NULL;
        CountRefPtr<ControlUI> pReturn;
        for( XMLElement *pChild = pRoot->FirstChildElement();pChild;pChild= pChild->NextSiblingElement())
        {
            const char *pstrClass = pChild->Name();
            if(strcmp(pstrClass,"Image") == 0 ||
                strcmp(pstrClass,"Font") == 0 ||
                strcmp(pstrClass,"Default") == 0 )
            {
                continue;
            }
            CountRefPtr<ControlUI> pControl;
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
                    pControl = new Label;
                if(strcmp(pstrClass,CTR_BUTTON)== 0)
                    pControl = new Button;
                else if(strcmp(pstrClass,CTR_CONTAINER) == 0)
                    pControl = new Container;
                else if(strcmp(pstrClass,CTR_VERTICALLAYOUT) == 0)
                    pControl = new VerticalLayout;
                else if(strcmp(pstrClass,CTR_HORIZONTALLAYOUT) == 0)
                    pControl = new HorizontalLayout;
                else if(strcmp(pstrClass,CTR_CONTROL) == 0)
                    pControl = new ControlUI;
                else if(strcmp(pstrClass,CTR_CONTAINER) == 0)
                    pControl = new Container;
                else if(strcmp(pstrClass,CTR_PROGRESS) == 0)
                    pControl = new ProgressBar;
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
                    //如果有子节点，递归解析
                    Parse(pChild,pControl);
                }
                //attach to parent
                if(pParent)
                {
                    //如果父控件是个container,把当前control加到container中去
                    IContainer* pContianer = dynamic_cast<IContainer* >(pParent);
                    if(!pContianer)
                        return nullptr;
                    pContianer->Add(pControl);
                }
                for(const XMLAttribute *pAttribue = pChild->FirstAttribute();pAttribue;pAttribue= pAttribue->Next())
                {
                    //设置当前节点的属性
                    pControl->SetAttribute(pAttribue->Name(),pAttribue->Value());
                }
            }
            //用来返回的是第一个节点~~
            if( pReturn == nullptr)
            {
                pReturn = pControl;
            }
        }

        return pReturn;
    }

}