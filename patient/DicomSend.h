#ifndef DICOMSEND_H_
#define DICOMSEND_H_
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string>
#include <vector>
class DicomSend;
typedef void (*HandleMenuItemCDActivate_t)(GtkMenuItem *menuitem, gpointer *data) ;
using namespace std;
class DicomSend
{
public:
	DicomSend();
	~DicomSend();
void destroyItem();
	vector<GtkWidget*> m_vecItem;
    static DicomSend *GetInstance();
static int HandleDicomBtnExportEvent(GtkWidget *widget, GdkEvent *event, DicomSend *data) ;

    void createLabelButton(GtkWidget *fixedWin);
GtkWidget* create_button_icon(GtkWidget *label, GtkWidget *icon_image);
void createPopup(char *label,HandleMenuItemCDActivate_t HandleMenuItemFlashActivate);
int BtnExportEvent(GtkWidget *widget, GdkEvent *event);
    static DicomSend *m_pInstance;
GtkWidget *m_itemFlash;
GtkWidget *m_menu;
};
 
#endif
