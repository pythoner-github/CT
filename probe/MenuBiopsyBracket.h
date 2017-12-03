#ifndef _MENU_BIOPSYBRACKET_H_
#define _MENU_BIOPSYBRACKET_H_


#define WIDTH_BIOPSYBRACKET_MENU  175//220
#define HEIGHT_BIOPSYBRACKET_MENU  635

#define HEIGHT_BIOPSYBRACKET_MENUITEM 25 //

#include <vector>

using namespace std;

class MenuBiopsyBracket
{
public:
	 MenuBiopsyBracket();
    ~MenuBiopsyBracket() {}
    GtkWidget * Create(void);
    void UpdateLabel(void);
    void Show(void);
    void Hide(void);
	void UpdateMenuBiopsyBracket(void);
private:
   
    GtkWidget *m_vboxBioBracketType;
	GtkWidget *m_menuBar;
	//---------------------------------------//
	vector<GtkWidget*> m_vecMenuItem;
//	vector<GtkWidget*> m_vecMenuItem;
	//-------------------------------------------//
 //signal handle
    void MenuItemTypeActivate(GtkMenuItem *menuitem);
    void MenuItemExitActivate(GtkMenuItem *menuitem);
//signal connect
    static void HandleMenuItemTypeActivate(GtkMenuItem *menuitem, MenuBiopsyBracket *data) { data->MenuItemTypeActivate(menuitem);}
    static void HandleMenuItemExitActivate(GtkMenuItem *menuitem, MenuBiopsyBracket *data) { data-> MenuItemExitActivate(menuitem); }

};

extern MenuBiopsyBracket g_menuBiopsyBracket;
#endif
