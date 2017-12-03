#include <gtk/gtk.h>
#include "probe/MenuBiopsyBracket.h"
#include "display/gui_func.h"
#include "display/gui_global.h"
#include "display/MenuArea.h"
#include "probe/BiopsyMan.h"
#include "probe/MenuBiopsy.h"
#include "display/HintArea.h"

MenuBiopsyBracket g_menuBiopsyBracket;

MenuBiopsyBracket::MenuBiopsyBracket()
{
	m_vecMenuItem.clear();
//	m_vecMenuItem.clear();
}
GtkWidget * MenuBiopsyBracket::Create(void)
{
	GtkWidget* menuItemExit;

	m_vboxBioBracketType=gtk_vbox_new(false,0);

	m_menuBar=gtk_menu_bar_new();
	gtk_widget_modify_bg(m_menuBar,GTK_STATE_NORMAL, g_deep);//2016.10.10

	menuItemExit=gtk_menu_item_new_with_label("");

	gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(menuItemExit)), GTK_STATE_NORMAL, g_white);

//	gtk_widget_set_usize(menuItemExit, WIDTH_BIOPSYBRACKET_MENU, -1);
	gtk_widget_set_usize(menuItemExit, WIDTH_BIOPSYBRACKET_MENU, HEIGHT_BIOPSYBRACKET_MENUITEM);

	gtk_menu_item_set_label(GTK_MENU_ITEM(menuItemExit), _("Exit"));

	gtk_menu_bar_append(GTK_MENU_BAR(m_menuBar),menuItemExit);
	g_signal_connect(G_OBJECT(menuItemExit),"activate",G_CALLBACK(HandleMenuItemExitActivate),this);

	gtk_widget_show(menuItemExit);

    gtk_box_pack_start(GTK_BOX(m_vboxBioBracketType),m_menuBar,false,false,0);
	gtk_menu_bar_set_pack_direction(GTK_MENU_BAR(m_menuBar),GTK_PACK_DIRECTION_TTB);
   // UpdateLabel();
    return m_vboxBioBracketType;
}
void MenuBiopsyBracket::UpdateLabel(void)
{
}
void MenuBiopsyBracket::Show(void)
{
	 g_menuBiopsy.SetDrawStatus(true);//2016.11.01

	UpdateMenuBiopsyBracket();//2016.08.07
    gtk_widget_show_all(m_vboxBioBracketType);
	//--------------------------------------------------------//
	SetSystemCursor(90,110);//	SetSystemCursor(90,100);
	doBtnEvent(1, 1);//fake btn press
	doBtnEvent(1, 0);//fake btn unpress
	SetMenuBiopsyCursorYRange(110,250);//SetMenuBiopsyCursorYRange(110,150);//2016.08.26

	ModeStatus ms;
    if (ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B) && ms.IsUnFreezeMode())
    {
        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::BIOPSY);
    }
	else
    {
		HintArea::GetInstance()->UpdateHint(_("[Biopsy]: Only valid in B mode and UnFreeze status."), 1);
    }

	//-------------------------------------------------------------//
	BiopsyLine::GetInstance()->Draw();//2016.11.01
}
void MenuBiopsyBracket::Hide(void)
{
    gtk_widget_hide_all(m_vboxBioBracketType);
	BiopsyLine::GetInstance()->Clear();//2016.11.01
	g_menuBiopsy.SetDrawStatus(false);//2016.11.01
}

void  MenuBiopsyBracket::MenuItemTypeActivate(GtkMenuItem *menuitem)
{
	string bracketType=gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));
	BiopsyMan::GetInstance()->SetCurBioBracketType(bracketType);
	g_menuBiopsy.UpdateBioBracketTypeLabel();//2016.08.09
	MenuArea::GetInstance()->ShowBioVerifyMenu();
}
void  MenuBiopsyBracket::MenuItemExitActivate(GtkMenuItem *menuitem)
{
	MenuArea::GetInstance()->ShowBiopsyMenu();
}
void MenuBiopsyBracket::UpdateMenuBiopsyBracket(void)
{
	int size1=m_vecMenuItem.size();
	for(int i=0;i<size1;i++)
	{
		gtk_widget_destroy(m_vecMenuItem[i]);
	}
	m_vecMenuItem.clear();

	vector<string> vecBracket=BiopsyMan::GetInstance()->GetBioBracketTypesOfCurProbe();

	int size2=vecBracket.size();
	for(int j=0;j<size2;j++)
	{
		GtkWidget*	menuItemType=gtk_menu_item_new_with_label(vecBracket[j].c_str());
		gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(menuItemType)), GTK_STATE_NORMAL, g_white);

		gtk_widget_set_usize(menuItemType, WIDTH_BIOPSYBRACKET_MENU, HEIGHT_BIOPSYBRACKET_MENUITEM);
		gtk_menu_bar_append(GTK_MENU_BAR(m_menuBar),menuItemType);
		g_signal_connect(G_OBJECT(menuItemType),"activate",G_CALLBACK(HandleMenuItemTypeActivate),this);

		m_vecMenuItem.push_back(menuItemType);

	}
}