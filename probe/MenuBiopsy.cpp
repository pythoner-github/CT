#include <gtk/gtk.h>
#include "probe/MenuBiopsy.h"
#include "display/gui_func.h"
#include "display/gui_global.h"
#include "display/ImageAreaDraw.h"
#include "display/MenuArea.h"
#include "probe/BiopsyMan.h"
#include "display/HintArea.h"

MenuBiopsy g_menuBiopsy;
bool MenuBiopsy::m_isDraw=false;
bool MenuBiopsy::m_isDoubleLine=false;

MenuBiopsy::MenuBiopsy(void)
{
    m_ptrImgDraw = ImageAreaDraw::GetInstance();
    m_ptrBiopsy = BiopsyLine::GetInstance();
	m_vecMenuItem.clear();
	m_subMenuAngle=NULL;//2016.08.30--hy
	m_vecAngleType.clear();
	m_vecWidgetIndex.clear();//2016.10.13
}

void MenuBiopsy::Hide(void)
{
	 //-----------------------------------------------//
      gtk_widget_hide_all(m_vboxBiopsy);
	  m_ptrBiopsy->Clear();//2016.11.01
      SetDrawStatus(false);//2016.11.01
}

void MenuBiopsy::Show(void)
{
	int bioBracketTypeNum=BiopsyMan::GetInstance()->GetBioBracketTypeNumOfCurProbe();
    if(bioBracketTypeNum<=0)
    {
        HintArea::GetInstance()->UpdateHint(_("No biopsy bracket found!"), 2);
        MenuShowUndo();
    }
    else
    {
	    SetDrawStatus(true);//2016.11.01

	    UpdateBioBracketTypeLabel();//2016.08.09

	    UpdateSubMenuAngle();//2016.08.06

        gtk_widget_show_all(m_vboxBiopsy);

        m_ptrBiopsy->Create();
    	//--------------------------------------------------------//
    	SetSystemCursor(90,130);//	SetSystemCursor(90,100);
    	doBtnEvent(1, 1);//fake btn press
	    doBtnEvent(1, 0);//fake btn unpres
	    SetMenuBiopsyCursorYRange(128,250);//SetMenuBiopsyCursorYRange(128,188);//2016.08.26
	//----------------------------------------------------------------//
    }
}

GtkWidget* MenuBiopsy::Create(void)
{
	GtkWidget* menuBar0;

	m_vboxBiopsy=gtk_vbox_new(false,0);

	m_labelBioBracketType= create_label("", 0, 0, g_lightGray, NULL);
    gtk_box_pack_start(GTK_BOX(m_vboxBiopsy),m_labelBioBracketType,false,false,0);

  	menuBar0=gtk_menu_bar_new();

	gtk_widget_modify_bg(menuBar0,GTK_STATE_NORMAL, g_deep);//2016.10.10

	m_menuItemAngle=gtk_menu_item_new_with_label("");
	gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(m_menuItemAngle)), GTK_STATE_NORMAL, g_white);

	gtk_widget_set_usize(m_menuItemAngle, WIDTH_BIOPSY_MENU, HEIGHT_BIOPSY_MENUITEM);

	gtk_menu_bar_append(GTK_MENU_BAR(menuBar0),m_menuItemAngle);
//	g_signal_connect(G_OBJECT(m_menuItemAngle),"activate",G_CALLBACK(),this);
//	g_signal_connect(G_OBJECT(m_menuItemAngle),"select",G_CALLBACK(HandleMenuItemAngleSelect),this);
	g_signal_connect(G_OBJECT(m_menuItemAngle),"activate",G_CALLBACK(HandleMenuItemAngleActivate),this);
//	g_signal_connect(G_OBJECT(m_menuItemAngle),"deselect",G_CALLBACK(HandleMenuItemAngleDeSelect),this);
//	g_signal_connect(G_OBJECT(m_menuItemAngle),"button-release-event",G_CALLBACK(HandleMenuItemAngleButtonRelease),this);
//	g_signal_connect(G_OBJECT(m_menuItemAngle),"button-press-event",G_CALLBACK(HandleMenuItemAngleButtonPress),this);
	gtk_widget_show(m_menuItemAngle);
//------------2016.11.02------------------------------------------------------------//
	m_menuItemLineType=gtk_menu_item_new_with_label("");

	gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(m_menuItemLineType)), GTK_STATE_NORMAL, g_white);

	gtk_widget_set_usize(m_menuItemLineType, WIDTH_BIOPSY_MENU, HEIGHT_BIOPSY_MENUITEM);

	gtk_menu_bar_append(GTK_MENU_BAR(menuBar0),m_menuItemLineType);
//	g_signal_connect(G_OBJECT(m_menuItemLineType),"activate",G_CALLBACK(HandleMenuItemLineTypeActivate),this);
	g_signal_connect(G_OBJECT(m_menuItemLineType),"button-release-event",G_CALLBACK(HandleMenuItemLineTypeButtonRelease),this);

	gtk_widget_show(m_menuItemLineType);
//----------------------------------------------------------------------//
	m_menuItemSetup=gtk_menu_item_new_with_label("");
	gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(m_menuItemSetup)), GTK_STATE_NORMAL, g_white);
	gtk_widget_set_usize(m_menuItemSetup, WIDTH_BIOPSY_MENU, HEIGHT_BIOPSY_MENUITEM);

	gtk_menu_bar_append(GTK_MENU_BAR(menuBar0),m_menuItemSetup);
	g_signal_connect(G_OBJECT(m_menuItemSetup),"activate",G_CALLBACK(HandleMenuItemSetupActivate),this);
	gtk_widget_show(m_menuItemSetup);

    gtk_box_pack_start(GTK_BOX(m_vboxBiopsy),menuBar0,false,false,0);

	gtk_menu_bar_set_pack_direction(GTK_MENU_BAR(menuBar0),GTK_PACK_DIRECTION_TTB);

	UpdateLabel();

    return m_vboxBiopsy;
}
void MenuBiopsy::UpdateLabel(void)
{
/*	string bracketType=BiopsyMan::GetInstance()->GetDefaultBioBracketTypeOfCurProbe();
    gtk_label_set_text(GTK_LABEL(m_labelBioBracketType), _(bracketType.c_str()));*/

	//gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle), _("Angle None"));
	if(m_isDoubleLine)
	{
		gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemLineType), _("Line Type     Double"));
	}
	else
	{
		gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemLineType), _("Line Type     Single"));
	}
	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemSetup), _("Setting"));
//	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemExit), _("Exit"));

}
//---------------------------------------------------------//
void MenuBiopsy::ClearBiopsyLine(void)
{
	if(m_isDraw)
	{
		BiopsyLine::GetInstance()->Clear();//
	}
}
void MenuBiopsy::UpdateBiopsyLine(void)
{
	if(m_isDraw)
	{
		BiopsyLine::GetInstance()->Clear();
		bool bisInitOk=BiopsyLine::GetInstance()->InitPara();
		if(bisInitOk)
		{
			BiopsyLine::GetInstance()->Redraw();
		}
		else
		{
			CloseBiopsyLine();//
		}
	}
}
void MenuBiopsy::CloseBiopsyLine(void) //--2016.09.18----
{
	m_isDraw=false;
	SetDrawStatus(false);
}
//------------------------------------------------------------//
void  MenuBiopsy::SubMenuItemAngleActivate(GtkMenuItem *menuitem)
{
	//---2016.09.06--------------------------------------------//
	int size0=m_vecMenuItem.size();
	for(int i=0;i<size0;i++)
	{
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(m_vecMenuItem[i]),false);
	}

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuitem),true);
	//-----------------------------------------------------//

	string text=gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));

//	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle),gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem)));
	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle),text.c_str());
	string noAngle="Angle None";
	if(text.compare(noAngle)!=0)
	{
		m_isDraw=true;
		BiopsyMan::GetInstance()->SetCurBioAngleType(text);//2016.08.09
		m_ptrBiopsy->AngleSwitch();
	}
	else
	{
		m_isDraw=false;
		m_ptrBiopsy->Clear();
	}
//------2016.08.30---------------------------------------//
	SetSystemCursor(90,170);//SetSystemCursor(90,170);
	doBtnEvent(1, 1);//fake btn press
	doBtnEvent(1, 0);//fake btn unpress
//----------------------------------------------------------//
}
void  MenuBiopsy::MenuItemLineTypeActivate(GtkMenuItem *menuitem)
{
	if(m_isDoubleLine)
	{
        m_ptrBiopsy->ClearDoubleLine();
		m_isDoubleLine=false;
		gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemLineType), _("Line Type     Single"));
	}
	else
	{
        m_ptrBiopsy->ClearSingleLine();
		m_isDoubleLine=true;
		gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemLineType), _("Line Type     Double"));
	}
	if(m_isDraw)
	{
		m_ptrBiopsy->Draw();
	}

}
void  MenuBiopsy::MenuItemSetupActivate(GtkMenuItem *menuitem)
{
	int bioBracketTypeNum=BiopsyMan::GetInstance()->GetBioBracketTypeNumOfCurProbe();
	if(bioBracketTypeNum<=1)
	{
		MenuArea::GetInstance()->ShowBioVerifyMenu();
	}
	else
	{
		MenuArea::GetInstance()->ShowBioBracketMenu();
	}
}
/*void  MenuBiopsy::MenuItemExitActivate(GtkMenuItem *menuitem)
{
	   MenuShowUndo();
	   MultiFuncUndo();//2016.09.13
}*/
void MenuBiopsy::SetBiopsyBracketTypeLabel(string bioBracketType)
{
	gtk_label_set_text(GTK_LABEL(m_labelBioBracketType), _(bioBracketType.c_str()));
}
void MenuBiopsy::SetAngleMenuItem(string strText)
{
	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle), _(strText.c_str()));
}

void MenuBiopsy::UpdateSubMenuAngle(void)
{
	if(m_subMenuAngle!=NULL)
	{
		gtk_widget_destroy(m_subMenuAngle);
	}

	m_vecMenuItem.clear();//2016.09.06

	m_subMenuAngle=gtk_menu_new();
	gtk_widget_set_usize(m_subMenuAngle, WIDTH_BIOPSY_MENU, -1);
	gtk_widget_modify_bg(m_subMenuAngle,GTK_STATE_NORMAL, g_deep);//2016.10.10
	//----------------------------------------------------------------------------------//
	if(m_vecWidgetIndex.size()>0)
	{
		int size0=m_vecWidgetIndex.size();
		for(int i=0;i<size0;i++)
		{
			delete m_vecWidgetIndex[i];
		}
	}
	m_vecWidgetIndex.clear();//2016.10.13
	m_vecAngleType.clear();

	vector<string> vecAngleType=BiopsyMan::GetInstance()->GetBioAngleTypesOfcurBioBracket();

	m_vecAngleType=vecAngleType;//2016.10.12

	int size1=vecAngleType.size();
	if(size1==1)
	{
		string angleOnOff;
		/*if(m_isDraw)
		{
			angleOnOff=vecAngleType[0]+"     On";
		}
		else
		{
			angleOnOff=vecAngleType[0]+"     Off";
		}*/
		angleOnOff=vecAngleType[0];

		SetAngleMenuItem(angleOnOff);
	}
	else
	{
		if(size1>1)
		{

			for(int j=0;j<size1;j++)
			{
				GtkWidget* menuItem=gtk_check_menu_item_new_with_label(_(vecAngleType[j].c_str()));//2016.09.05
				gtk_widget_modify_base(menuItem,GTK_STATE_NORMAL, g_deepGray);
				gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(menuItem)), GTK_STATE_NORMAL, g_white);
			//	GtkWidget* menuItem=gtk_menu_item_new_with_label(vecAngleType[j].c_str());//2016.09.05
				gtk_widget_set_usize(menuItem, WIDTH_BIOPSY_MENU, HEIGHT_BIOPSY_MENUITEM);//2016.08.25

			//	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(HandleSubMenuItemAngleActivate),this);
			//	g_signal_connect(G_OBJECT(menuItem),"button-release-event",G_CALLBACK(HandleSubMenuItemAngleButtonRelease),this);

				StructWidgetIndex* pWidgetIndex0=new StructWidgetIndex;
				pWidgetIndex0->pData=this;
				pWidgetIndex0->index=j;//pWidgetIndex0->index=j+1;
				m_vecWidgetIndex.push_back(pWidgetIndex0);

			/*	g_signal_connect(G_OBJECT(menuItem),"button-release-event",G_CALLBACK(HandleSubMenuItemAngleButtonRelease),m_vecWidgetIndex[j+1]);*/
				g_signal_connect(G_OBJECT(menuItem),"button-release-event",G_CALLBACK(HandleSubMenuItemAngleButtonRelease),m_vecWidgetIndex[j]);

				gtk_menu_shell_append(GTK_MENU_SHELL(m_subMenuAngle),menuItem);
				m_vecMenuItem.push_back(menuItem);//
			}

			gtk_menu_item_set_submenu(GTK_MENU_ITEM(m_menuItemAngle),m_subMenuAngle);

			//-------------------------------------------------------------------------//
			if(m_isDraw)
			{
				string curAngleType=BiopsyMan::GetInstance()->GetCurBioAngleType();//2016.09.14
				for(int i=0;i<size1;i++)
				{
					if(vecAngleType[i].compare(curAngleType)==0)
					{
						gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(m_vecMenuItem[i]),true);
						break;
					}
				}

				SetAngleMenuItem(curAngleType);//2016.09.22
			}
		/*	else
			{
				gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(m_vecMenuItem[0]),true);

				SetAngleMenuItem(gtk_menu_item_get_label(GTK_MENU_ITEM(m_vecMenuItem[0])));//2016.09.22
			}*/
			//-----------------------------------------------------------------------------//
		}
	}
}

void MenuBiopsy::UpdateBioBracketTypeLabel(void)
{
	string bracketType=BiopsyMan::GetInstance()->GetCurBioBracketType();
	SetBiopsyBracketTypeLabel(bracketType);
}
void MenuBiopsy::UpdateAngleMenuItem(void)
{

	string angleType=BiopsyMan::GetInstance()->GetCurBioAngleType();
	SetAngleMenuItem(angleType);
	SetDrawStatus(true);

}
void MenuBiopsy::SetDrawStatus(bool status)
{
	m_isDraw=status;
}
void MenuBiopsy::MenuItemAngleSelect(GtkMenuItem *menuitem)
{
	//	SetMenuBiopsyCursorYRange(128,208);
	//SetMenuBiopsyCursorYRange(128,208);
	SetMenuBiopsyCursorYRange(128,250);
}
void MenuBiopsy::MenuItemAngleDeSelect(GtkMenuItem *menuitem)
{
	/*SetSystemCursor(90,130);//SetSystemCursor(90,170);
	doBtnEvent(1, 1);//fake btn press
	doBtnEvent(1, 0);*///fake btn unpress

//	SetMenuBiopsyCursorYRange(128,188);
	SetMenuBiopsyCursorYRange(128,250);

}
void MenuBiopsy::MenuItemAngleActivate(GtkMenuItem *menuitem)
{
//	SetMenuBiopsyCursorYRange(128,208);//2016.08.26
	SetMenuBiopsyCursorYRange(128,250);//2016.08.26

}
void MenuBiopsy::MenuItemAngleButtonRelease(GtkMenuItem *menuitem)
{
	vector<string> vecAngleType=BiopsyMan::GetInstance()->GetBioAngleTypesOfcurBioBracket();
	int size1=vecAngleType.size();
	if(size1==1)
	{
		string angleOnOff;
		if(m_isDraw)
		{
			m_isDraw=false;

			angleOnOff=vecAngleType[0]+"     Off";

			m_ptrBiopsy->Clear();
		}
		else
		{
			m_isDraw=true;

			angleOnOff=vecAngleType[0]+"     On";

			BiopsyMan::GetInstance()->SetCurBioAngleType(vecAngleType[0]);//2016.08.09

			m_ptrBiopsy->AngleSwitch();
		}

		SetAngleMenuItem(angleOnOff);
	}
//----------------------------------------------------//

}

void MenuBiopsy::SubMenuItemAngleButtonRelease(GtkMenuItem *menuitem,int index)
{

    int size0=m_vecMenuItem.size();//2016.10.12

	for(int i=0;i<size0;i++)
	{
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(m_vecMenuItem[i]),false);
	}

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuitem),true);

	string text=gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));

	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle),text.c_str());

	/*if(index!=0)
	{
		m_isDraw=true;
		BiopsyMan::GetInstance()->SetCurBioAngleType(m_vecAngleType[index-1]);
		m_ptrBiopsy->AngleSwitch();
	}
	else
	{
		m_isDraw=false;
		m_ptrBiopsy->Clear();
	}*/
	BiopsyMan::GetInstance()->SetCurBioAngleType(m_vecAngleType[index]);
	m_ptrBiopsy->AngleSwitch();

	/*string noAngle="Angle None";
	if(text.compare(noAngle)!=0)
	{
		m_isDraw=true;
		BiopsyMan::GetInstance()->SetCurBioAngleType(text);//2016.08.09
		m_ptrBiopsy->AngleSwitch();
	}
	else
	{
		m_isDraw=false;
		m_ptrBiopsy->Clear();
	}*/
//------2016.08.30---------------------------------------//
/*	SetSystemCursor(90,180);//SetSystemCursor(90,170);
	doBtnEvent(1, 1);//fake btn press
	doBtnEvent(1, 0);*///fake btn unpress
//----------------------------------------------------------//

}