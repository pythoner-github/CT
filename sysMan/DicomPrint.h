#ifndef DICOMPRINT_H_
#define DICOMPRINT_H_
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string>
#include "base/IniFile.h"
#include "periDevice/DCMMan.h"
#include "sysMan/ButtonCallBack.h"
#include <string.h>
using namespace std;
#include "Def.h"

typedef struct
{
    char labelTitle[50];
    int label_width;
    int label_height;
    int label_x;
    int label_y;
    int entry_width;
    int entry_height;
    int entry_x;
    int entry_y;
    GtkWidget *parent;

}widgetInfo_t;

typedef struct
{
    int x;
    int y;
    int width;
    int height;
    GtkWidget *parent;
    char label[50];

}buttonInfo_t;

enum PropertyState{E_INIT,E_ONLYREAD,E_ONLYWRITE};

class DicomPrint
{
    typedef struct
    {
        int active;
        GtkWidget *comboText;
    }property_t;

    public:
    DicomPrint();
    ~DicomPrint();
    enum{
        COL_STORAGE_DEVICE,
        COL_SERVICE_NAME,
        COL_AE_TITLE,
        COL_PORT,
        COL_DEFAULT,
        NUM_COLS
    };

    typedef struct
    {
        char buf[30];
        int index;
    }device_index_t;

    vector<device_index_t> device_index;

    GtkWidget * createNotePrint();
    bool AddPrintConfiguration();
    bool SetPrintConfiguration();
    void UpdatePrintConfiguration(DCMPRINTCONFIGELEMENT &element);
    void StateMachine(PropertyState currentState);
    static DicomPrint *GetInstance();
    void createComboLabel(widgetInfo_t widgetInfo);
    void createEntryLabel(widgetInfo_t widgetInfo);
    void createButton(buttonInfo_t buttonInfo);
    GtkTreeModel* create_service_model();
    static void GetSingleServiceAttribute(string device, string serviceName,string aeTitle, int port, bool isDefault,void *data);
    void init_print_setting();
    void DigitsLimit(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void CharacterLimit(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void setListFreeze(gboolean enable);
   void HandleStringInsertChar(char *src,char c,int position);
    gchar *combo_box_get_text(GtkComboBox *comboBox);
    string entry_get_text(GtkEntry *entry);

    void SetPrintPropertyDefault();
    void SetDevicePropertyDefault();
    void LockPrintProperty();
    void UnlockPrintProperty();
    void LockDeviceProperty();
    void UnlockDeviceProperty();
    static DicomPrint *m_pInstance;

    GtkWidget *m_fixed_print_property;

    GtkWidget *m_button_print_add;
    GtkWidget *m_delete_button;
    GtkWidget *m_default_button;
    GtkWidget *m_connect_button;

    GtkWidget *m_button_add;
    GtkWidget *m_button_clear;

    GtkWidget *m_combobox_device;
    GtkWidget *m_entry_print_name;
    GtkWidget *m_treeview_print;
    GtkWidget* create_service_treeview();
    GtkWidget *m_btnBox;
    GtkWidget *m_combobox_print_device;

    PropertyState m_currentState;

    string EntryGetTextMinDensity();
    string EntryGetTextMaxDensity();

    private:
    GtkWidget *m_comboBoxColor;
    GtkWidget *m_comboBoxFormat;
    GtkWidget *m_comboBoxOrientation;
    GtkWidget *m_comboBoxMagnification;
    GtkWidget *m_comboBoxMediumType;
    GtkWidget *m_comboBoxFilmSize;
    GtkWidget *m_comboBoxDestination;
    GtkWidget *m_comboBoxPrioritys;
    GtkWidget *m_comboBoxCopies;
    GtkWidget *m_EntryConfInfo;

    public:
    GtkWidget  *m_entry_print_AE;
    GtkWidget *m_entry_print_port;
    GtkWidget *m_entry_print_service_name;

    GtkWidget *m_entryMinDensity;
    GtkWidget *m_entryMaxDensity;

    IniFile *ptrIni;
    gboolean m_enableList;
};

static char *device;
static 	char *service;
static 	char *ae;
static 	char *port;
static DCMPRINTCONFIGELEMENT element;

static void HandleListClicked(GtkTreeSelection *treeselection,gpointer  user_data)
{
    int row = 0;

    bool ret = 0;
    int i = 0;
    row = gtk_tree_selection_count_selected_rows(treeselection);
    PRINTF("row = %d\n",row);
    if(row != 0)
    {
        device = ButtonCallBack::GetInstance()->GtkTreeDeviceGet(DicomPrint::GetInstance()->m_treeview_print);
        service = ButtonCallBack::GetInstance()->GtkTreeServiceGet(DicomPrint::GetInstance()->m_treeview_print);
        ae = ButtonCallBack::GetInstance()->GtkTreeAEGet(DicomPrint::GetInstance()->m_treeview_print);
        port = ButtonCallBack::GetInstance()->GtkTreePortGet(DicomPrint::GetInstance()->m_treeview_print);
        for(i = 0;i < DicomPrint::GetInstance()->device_index.size();i++)
        {
            PRINTF("device = %s buf = %s\n",device,DicomPrint::GetInstance()->device_index[i].buf);
            if(strcmp(DicomPrint::GetInstance()->device_index[i].buf,device) == 0)
            {
                gtk_combo_box_set_active(GTK_COMBO_BOX(DicomPrint::GetInstance()->m_combobox_print_device),DicomPrint::GetInstance()->device_index[i].index);
            }
        }

        gtk_entry_set_text (GTK_ENTRY(DicomPrint::GetInstance()->m_entry_print_service_name),service);
        gtk_entry_set_text (GTK_ENTRY(DicomPrint::GetInstance()->m_entry_print_AE),ae);
        gtk_entry_set_text (GTK_ENTRY(DicomPrint::GetInstance()->m_entry_print_port),port);

        PRINTF("device = %s\n",device);
        ret = CDCMMan::GetMe()->GetPrintServiceConfiguration(device,element);
        if(ret == FALSE)
        {
            PRINTF("Fail to get Print Configuration!\n");
            return ;
        }
        DicomPrint::GetInstance()->UpdatePrintConfiguration(element);

        DicomPrint::GetInstance()->StateMachine(E_ONLYREAD);
    }
}

static void HandleButtonPrintAddClicked(GtkButton *button, DicomPrint *data);
static void HandleButtonPrintClearClicked(GtkButton *button, DicomPrint *data);
static void HandleButtonPrintDeleteClicked(GtkButton *button, DicomPrint *data);
static void HandleButtonPrintDefaultClicked(GtkButton *button, DicomPrint *data);
static void HandleButtonPrintConnectClicked(GtkButton *button, DicomPrint *data);
static void HandlePrintDeviceChanged(GtkComboBox *combobox,DicomPrint *data);
static void HandleDigitsLimit(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, DicomPrint *data)
{ data->DigitsLimit(editable, new_text, new_text_length, position); }

static void HandleCharacterLimit(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, DicomPrint *data)
{

    data->CharacterLimit(editable, new_text, new_text_length,  position);
}

static gboolean HandleIsEnableList(GtkWidget *widget,GdkEvent  *event,gpointer   user_data)
{
    return DicomPrint::GetInstance()->m_enableList;
}
#endif