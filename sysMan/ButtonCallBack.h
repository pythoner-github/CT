#ifndef BUTTONCALLBACK_H_
#define BUTTONCALLBACK_H_
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string>
using namespace std;
class ButtonCallBack
{
public:

enum{
            COL_STORAGE_DEVICE,
            COL_SERVICE_NAME,
            COL_AE_TITLE,
            COL_PORT,
            COL_DEFAULT,
            NUM_COLS
        };

    ButtonCallBack();
    ~ButtonCallBack();

static ButtonCallBack * GetInstance();

char *  GtkTreeDeviceGet(GtkWidget *m_treeview_worklist);
char * GtkTreeServiceGet(GtkWidget *m_treeview_worklist);
char * GtkTreeAEGet(GtkWidget *m_treeview_worklist);
char * GtkTreePortGet(GtkWidget *m_treeview_worklist);

void  ButtonPrintAddClicked(GtkButton *button,GtkWidget *m_combobox_worklist_device,GtkWidget *m_entry_worklist_AE,GtkWidget *m_entry_worklist_port,GtkWidget *m_entry_worklist_name,GtkWidget *m_treeview_worklist);

void ButtonPrintClearClicked(GtkButton *button,GtkWidget *m_combobox_worklist_device,GtkWidget *m_entry_worklist_name,GtkWidget *m_entry_worklist_AE,GtkWidget *m_entry_worklist_port);
bool ButtonPrintDeleteClicked(GtkButton *button,GtkWidget *m_treeview_worklist);
	static ButtonCallBack *m_pInstance;
void ButtonPrintDefaultClicked(GtkButton *button,GtkWidget *m_treeview_worklist);
void ButtonPrintConnectClicked(GtkButton *button,GtkWidget *m_treeview_worklist);
void PrintDeviceChanged(GtkComboBox *combobox,GtkWidget *m_combobox_worklist_device,GtkWidget *m_entry_worklist_name);
string m_selectedDevice;
};
static gboolean WorklistConnectTimeout(gpointer data);
#endif
