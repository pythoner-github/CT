// -*- c++ -*-
#ifndef _MENU_BIOPSY_H_
#define _MENU_BIOPSY_H_

#include "Def.h"
#include "CusSpin.h"
#include "BiopsyLine.h"
#include "ImageAreaDraw.h"

#define WIDTH_BIOPSY_MENU  175 //220
#define HEIGHT_BIOPSY_MENU 635

#define HEIGHT_BIOPSY_MENUITEM 25 //

class MenuBiopsy
{
public:
	struct StructWidgetIndex
	{
		MenuBiopsy* pData;
		int index;
	};
    MenuBiopsy();
    ~MenuBiopsy() {}
    void Hide(void);
    void Show(void);
    GtkWidget * Create(void);
    
    void UpdateLabel(void);
	//---------------------------------------------------//
	void SetBiopsyBracketTypeLabel(string bioBracketType);
	void SetAngleMenuItem(string strText);
	//------------------------------------------------------//
	void UpdateSubMenuAngle(void);
	void UpdateBioBracketTypeLabel(void);
	void UpdateAngleMenuItem(void);
	void SetDrawStatus(bool status);
	//------------Reserved-2016.07.25-------------------//
	void ClearBiopsyLine(void);
	void UpdateBiopsyLine(void);
	static bool GetDrawStatus(void){return m_isDraw;}
	static bool GetDoubleLineStatus(void){return m_isDoubleLine;}
	//--------------------------------------------------//
	void CloseBiopsyLine(void);//2016.09.18
	//--------------------------------------------------//
public:
   static  bool m_isDraw; 
   static bool m_isDoubleLine;
private:
    BiopsyLine * m_ptrBiopsy;
    ImageAreaDraw *m_ptrImgDraw;

    GtkWidget *m_vboxBiopsy;
	GtkWidget *m_labelBioBracketType;      
   // GtkWidget *m_menuBiopsy;
//	GtkWidget *m_menuBarBiopsy;

	GtkWidget *m_menuItemAngle;
	GtkWidget *m_menuItemLineType;
	GtkWidget *m_menuItemSetup;
	GtkWidget *m_menuItemExit;
   //------------------------------//
   	GtkWidget*m_subMenuAngle;
	vector<GtkWidget*> m_vecMenuItem;
	//--------------------------------------//
	vector<string> m_vecAngleType;
	vector<StructWidgetIndex*> m_vecWidgetIndex;//2016.10.13
   ////----------------------------------//
  //signal handle
    void SubMenuItemAngleActivate(GtkMenuItem *menuitem);
    void MenuItemLineTypeActivate(GtkMenuItem *menuitem);
    void MenuItemSetupActivate(GtkMenuItem *menuitem);
   // void MenuItemExitActivate(GtkMenuItem *menuitem);
	void MenuItemAngleSelect(GtkMenuItem *menuitem);
	void MenuItemAngleDeSelect(GtkMenuItem *menuitem);

	void MenuItemAngleActivate(GtkMenuItem *menuitem);
	void MenuItemAngleButtonRelease(GtkMenuItem *menuitem);
	//void SubMenuItemAngleButtonRelease(GtkMenuItem *menuitem);//2016.09.06
	void SubMenuItemAngleButtonRelease(GtkMenuItem *menuitem,int index);//2016.09.06

//signal connect
    static void HandleSubMenuItemAngleActivate(GtkMenuItem *menuitem, MenuBiopsy *data) { data->SubMenuItemAngleActivate(menuitem);}
    static void HandleMenuItemLineTypeActivate(GtkMenuItem *menuitem, MenuBiopsy *data) { data->MenuItemLineTypeActivate(menuitem);}
    static void HandleMenuItemSetupActivate(GtkMenuItem *menuitem, MenuBiopsy *data) { data->MenuItemSetupActivate(menuitem);}
  	static gboolean HandleMenuItemLineTypeButtonRelease(GtkWidget *widget,GdkEvent *event,gpointer user_data)
	{
		((MenuBiopsy*)user_data)->MenuItemLineTypeActivate(GTK_MENU_ITEM(widget));
		return TRUE;
	}
	static void HandleMenuItemAngleSelect(GtkMenuItem *menuitem, MenuBiopsy *data)
	{
		data->MenuItemAngleSelect(menuitem);
	}
	static void HandleMenuItemAngleDeSelect(GtkMenuItem *menuitem, MenuBiopsy *data)
	{
		data->MenuItemAngleDeSelect(menuitem);
	}
	static void HandleMenuItemAngleActivate(GtkMenuItem *menuitem, MenuBiopsy *data)
	{
		data->MenuItemAngleActivate(menuitem);
	}
	static gboolean HandleMenuItemAngleButtonRelease(GtkWidget *widget,GdkEvent *event,gpointer user_data)
	{
		((MenuBiopsy*)user_data)->MenuItemAngleButtonRelease(GTK_MENU_ITEM(widget));
		return TRUE;
	}
	static gboolean HandleMenuItemAngleButtonPress(GtkWidget *widget,GdkEvent *event,gpointer user_data)
	{
		return TRUE;
	}

	static gboolean HandleSubMenuItemAngleButtonRelease(GtkWidget *widget, GdkEvent *event, gpointer user_data)
	{
		(((StructWidgetIndex*)user_data)->pData)->SubMenuItemAngleButtonRelease(GTK_MENU_ITEM(widget), ((StructWidgetIndex*)user_data)->index);
		return TRUE;
	}

};

extern MenuBiopsy g_menuBiopsy;

#endif
