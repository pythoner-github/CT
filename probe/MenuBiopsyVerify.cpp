#include <gtk/gtk.h>
#include "MenuBiopsyVerify.h"
#include "gui_func.h"
#include "gui_global.h"
#include "MenuArea.h"
#include "BiopsyMan.h"
#include "BiopsyLine.h"
#include "MenuBiopsy.h"
#include "HintArea.h"

MenuBiopsyVerify g_menuBiopsyVerify;

MenuBiopsyVerify::MenuBiopsyVerify()
{
	m_vecMenuItem.clear();
	m_subMenuAngle=NULL;
}
GtkWidget * MenuBiopsyVerify::Create(void)
{
	GtkWidget* menuBar0;

	m_vboxBioVerify=gtk_vbox_new(false,0);

	m_labelBioBracketType= create_label("", 0, 0, g_lightGray, NULL);
    gtk_box_pack_start(GTK_BOX(m_vboxBioVerify),m_labelBioBracketType,false,false,0);

  	menuBar0=gtk_menu_bar_new();
	gtk_widget_modify_bg(menuBar0,GTK_STATE_NORMAL, g_deep);//2016.10.10

	m_menuItemExit=gtk_menu_item_new_with_label("");
	gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(m_menuItemExit)), GTK_STATE_NORMAL, g_white);

	gtk_widget_set_usize(m_menuItemExit, WIDTH_BIOPSYVERIFY_MENU, HEIGHT_BIOPSYVERIFY_MENUITEM);

	gtk_menu_bar_append(GTK_MENU_BAR(menuBar0),m_menuItemExit);
	g_signal_connect(G_OBJECT(m_menuItemExit),"activate",G_CALLBACK(HandleMenuItemExitActivate),this);
	gtk_widget_show(m_menuItemExit);

	m_menuItemAngle=gtk_menu_item_new_with_label("");
	gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(m_menuItemAngle)), GTK_STATE_NORMAL, g_white);

	gtk_widget_set_usize(m_menuItemAngle, WIDTH_BIOPSYVERIFY_MENU, HEIGHT_BIOPSYVERIFY_MENUITEM);

	gtk_menu_bar_append(GTK_MENU_BAR(menuBar0),m_menuItemAngle);
//	g_signal_connect(G_OBJECT(m_menuItemAngle),"actvie",G_CALLBACK(),this);
	g_signal_connect(G_OBJECT(m_menuItemAngle),"button-release-event",G_CALLBACK(HandleMenuItemAngleButtonRelease),this);
	gtk_widget_show(m_menuItemAngle);

	m_menuItemSave=gtk_menu_item_new_with_label("");
	gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(m_menuItemSave)), GTK_STATE_NORMAL, g_white);
	gtk_widget_set_usize(m_menuItemSave, WIDTH_BIOPSYVERIFY_MENU, HEIGHT_BIOPSYVERIFY_MENUITEM);

	gtk_menu_bar_append(GTK_MENU_BAR(menuBar0),m_menuItemSave);
//	g_signal_connect(G_OBJECT(m_menuItemSave),"activate",G_CALLBACK(HandleMenuItemSaveActivate),this);
	g_signal_connect(G_OBJECT(m_menuItemSave),"button-release-event",G_CALLBACK(HandleMenuItemSaveButtonRelease),this);

	gtk_widget_show(m_menuItemSave);

	m_menuItemLoadFactory=gtk_menu_item_new_with_label("");
	gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(m_menuItemLoadFactory)), GTK_STATE_NORMAL, g_white);

	gtk_widget_set_usize(m_menuItemLoadFactory, WIDTH_BIOPSYVERIFY_MENU, HEIGHT_BIOPSYVERIFY_MENUITEM);
	gtk_menu_bar_append(GTK_MENU_BAR(menuBar0),m_menuItemLoadFactory);
    g_signal_connect(G_OBJECT(m_menuItemLoadFactory),"button-release-event",G_CALLBACK(HandleMenuItemLoadFactoryButtonRelease),this);
    gtk_widget_show(m_menuItemLoadFactory);

    m_menuItemEnhance=gtk_menu_item_new_with_label("");
    gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(m_menuItemEnhance)), GTK_STATE_NORMAL, g_white);

    gtk_widget_set_usize(m_menuItemEnhance, WIDTH_BIOPSYVERIFY_MENU, HEIGHT_BIOPSYVERIFY_MENUITEM);
    gtk_menu_bar_append(GTK_MENU_BAR(menuBar0),m_menuItemEnhance);
    g_signal_connect(G_OBJECT(m_menuItemEnhance),"button-release-event",G_CALLBACK(HandleMenuItemEnhanceButtonRelease),this);
    gtk_widget_show(m_menuItemEnhance);


    gtk_box_pack_start(GTK_BOX(m_vboxBioVerify),menuBar0,false,false,0);
	gtk_menu_bar_set_pack_direction(GTK_MENU_BAR(menuBar0),GTK_PACK_DIRECTION_TTB);

	UpdateLabel();
    
	return m_vboxBioVerify;

}
void MenuBiopsyVerify::UpdateLabel(void)
{
	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemSave), _("Save"));
	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemLoadFactory), _("Default Factory"));
	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemEnhance), _("Biopsy Enhance"));
	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemExit), _("Exit"));
}
void MenuBiopsyVerify:: Show(void)
{
	 g_menuBiopsy.SetDrawStatus(true);//2016.11.01
	 UpdateBioBracketType();//2016.08.07
	 UpdateAngleMenuItem();//2016.08.07

	 UpdateSubMenuAngle();//2016.08.07

	 gtk_widget_show_all(m_vboxBioVerify);

	//----------------------------------------------------// 
	 g_menuBiopsy.SetDrawStatus(true);//2016.09.22
	//-----------------------------------------------------//
	
	 BiopsyLine::GetInstance()->Create();
	 //-----------------------------------------------------------//
	/* SetSystemCursor(90,130);//	SetSystemCursor(90,100);
	 doBtnEvent(1, 1);//fake btn press
 	 doBtnEvent(1, 0);//fake btn unpress
	 SetMenuBiopsyCursorYRange(128,188);*///2016.08.26
	 //-----------------------------------------------------------------//
	//--------2016.08.10----------------------------//
	ModeStatus ms;
	if (((ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B))||(ms.IsCFMMode()&&(ms.GetFormatCfm()==FormatCfm::B))||(ms.IsPDIMode()&&(ms.GetFormatCfm()==FormatCfm::B)))&& ms.IsUnFreezeMode())
    {
        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::BIOPSY_VERIFY);
    }
	else
    {
		HintArea::GetInstance()->UpdateHint(_("[Verify]: Only valid in B mode,CFM,PDI and UnFreeze status."), 1);
    }
}
void MenuBiopsyVerify:: Hide(void)
{
	gtk_widget_hide_all(m_vboxBioVerify);
	BiopsyLine::GetInstance()->Clear();//2016.11.01
    g_menuBiopsy.SetDrawStatus(false);//2016.11.01
}

void MenuBiopsyVerify::MenuItemAngleActivate(GtkMenuItem *menuitem)
{
	string angleType=gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));

	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle),gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem)));

	BiopsyMan::GetInstance()->SetCurBioAngleType(angleType);//2016.08.09
	g_menuBiopsy.UpdateAngleMenuItem();
	BiopsyLine::GetInstance()->AngleSwitch(); 
}
void MenuBiopsyVerify::MenuItemExitActivate(GtkMenuItem *menuitem)
{
	MenuArea::GetInstance()->ShowBiopsyMenu();
}
void  MenuBiopsyVerify::MenuItemSaveActivate(GtkMenuItem *menuitem)
{
	BiopsyLine::GetInstance()->WritePara();
}
void  MenuBiopsyVerify::MenuItemLoadFactoryActivate(GtkMenuItem *menuitem)
{
	BiopsyLine::GetInstance()->SetDefault();
}

void  MenuBiopsyVerify::MenuItemEnhanceActivate(GtkMenuItem *menuitem)
{
    BiopsyLine::GetInstance()->BiopsyEnhance();
}

void MenuBiopsyVerify::UpdateSubMenuAngle(void)
{
	if(m_subMenuAngle!=NULL)
	{
		gtk_widget_destroy(m_subMenuAngle);
	}

	m_vecMenuItem.clear();

	m_subMenuAngle=gtk_menu_new();
	gtk_widget_modify_bg(m_subMenuAngle,GTK_STATE_NORMAL, g_deep);//2016.10.10
	gtk_widget_set_usize(m_subMenuAngle, WIDTH_BIOPSYVERIFY_MENU, -1);
    //------------------------------------------------//
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
    //----------------------------------------------------//
	vector<string> vecAngleType=BiopsyMan::GetInstance()->GetBioAngleTypesOfcurBioBracket();
    m_vecAngleType=vecAngleType;//2016.11.05
	int size1=vecAngleType.size();
    if(size1==1)
    {
        gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle), _(vecAngleType[0].c_str()));
    }
    else
    {
	    if(size1>1)
    	{
		    for(int j=0;j<size1;j++)
		    {
			    GtkWidget* menuItem=gtk_check_menu_item_new_with_label(_(vecAngleType[j].c_str()));

			    gtk_widget_modify_base(menuItem,GTK_STATE_NORMAL, g_deepGray);
			    gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(menuItem)), GTK_STATE_NORMAL, g_white);

			    gtk_widget_set_usize(menuItem, WIDTH_BIOPSYVERIFY_MENU, HEIGHT_BIOPSYVERIFY_MENUITEM);

        	    StructWidgetIndex* pWidgetIndex0=new StructWidgetIndex;
			    pWidgetIndex0->pData=this;
			    pWidgetIndex0->index=j;//pWidgetIndex0->index=j+1;
			    m_vecWidgetIndex.push_back(pWidgetIndex0);

		    //	g_signal_connect(G_OBJECT(menuItem),"activate",G_CALLBACK(HandleMenuItemAngleActivate),this);
                g_signal_connect(G_OBJECT(menuItem),"button-release-event",G_CALLBACK(HandleSubMenuItemAngleButtonRelease),m_vecWidgetIndex[j]);

		        /*	g_signal_connect(G_OBJECT(menuItem),"button-release-event",G_CALLBACK(HandleSubMenuItemAngleButtonRelease),this);*/

			    gtk_menu_shell_append(GTK_MENU_SHELL(m_subMenuAngle),menuItem);
			    m_vecMenuItem.push_back(menuItem);
		    }
		    gtk_menu_item_set_submenu(GTK_MENU_ITEM(m_menuItemAngle),m_subMenuAngle);
		    //-----------------------------------------------------------------------------//
		    string curAngleType=BiopsyMan::GetInstance()->GetCurBioAngleType();//2016.09.14
		    for(int i=0;i<size1;i++)
		    {
			    if(vecAngleType[i].compare(curAngleType)==0)
			    {
				    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(m_vecMenuItem[i]),true);
				    break;
			    }
		    }

		//-----------------------------------------------------------------------------------//
	    }
    }
	//--------------------------------------------------------------------//
}
void MenuBiopsyVerify::UpdateBioBracketType(void)
{
	string bracketType=BiopsyMan::GetInstance()->GetCurBioBracketType();
    gtk_label_set_text(GTK_LABEL(m_labelBioBracketType), _(bracketType.c_str()));

}
void MenuBiopsyVerify::UpdateAngleMenuItem(void)
{
	string angleType;
	if(BiopsyMan::GetInstance()->IsBioBracketChanged())
	{
		angleType=BiopsyMan::GetInstance()->GetDefaultAngleTypeOfCurBioBracket();
	}
	else
	{
		angleType=BiopsyMan::GetInstance()->GetCurBioAngleType();
	}
	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle), _(angleType.c_str()));

	BiopsyMan::GetInstance()->SetCurBioAngleType(angleType);//2016.08.09
}

void MenuBiopsyVerify::SubMenuItemAngleButtonRelease(GtkMenuItem *menuitem,int index)
{
	int size0=m_vecMenuItem.size();
	for(int i=0;i<size0;i++)
	{
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(m_vecMenuItem[i]),false);
	}

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuitem),true);
	
	//----------------------------------------------------------------//

	string angleType=gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));

	gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle),gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem)));

    BiopsyMan::GetInstance()->SetCurBioAngleType(m_vecAngleType[index]);

	//g_menuBiopsy.UpdateAngleMenuItem();//2016.11.05
	BiopsyLine::GetInstance()->AngleSwitch();
	//--------------------------------------------------------------//
	/*SetSystemCursor(90,180);//SetSystemCursor(90,170);
	doBtnEvent(1, 1);//fake btn press
	doBtnEvent(1, 0);*///fake btn unpress
	//-------------------------------------------------------------//
}

