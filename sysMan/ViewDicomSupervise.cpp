#include <glib/gstdio.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "ViewDicomSupervise.h"
#include "gui_func.h"
#include "gui_global.h"
#include "../keyboard/KeyDef.h"
#include "../keyboard/MultiFuncFactory.h"
#include "KeyValueOpr.h"
#include "ViewMain.h"
#include "SysDicomSetting.h"
#include "ViewDialog.h"
#include "DCMRegister.h"
#include "PeripheralMan.h"

//std::string ViewDicomSupervise::noticeInfo = _("Note: According to the license file,the manufacturer will generate\na unique register key for every machine. You can register DICOM\nfunction by inputting this register key. If register is successful,\nyou can use the DICOM function.");
//std::string ViewDicomSupervise::noticeInfo1=_("Please insert Udisk,and click button 'Export License File'. You will\nfind this file named 'license' in Udisk. Please send this file to\nmanufacturer.");
ViewDicomSupervise* ViewDicomSupervise::m_ptrInstance = NULL;
ViewDicomSupervise::ViewDicomSupervise()
{

}

ViewDicomSupervise::~ViewDicomSupervise()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

ViewDicomSupervise* ViewDicomSupervise::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ViewDicomSupervise;

    return m_ptrInstance;
}

void ViewDicomSupervise::DestroyWindow(void)
{
    if(GTK_IS_WIDGET(m_window))	{
        Exit();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1)
        {
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
        }
    }
}

void ViewDicomSupervise::Exit(void)
{
    g_keyInterface.Pop(); 
}

gboolean ViewDicomSupervise::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWindow();
    return FALSE;
}

//GtkWidget* ViewDicomSupervise::CreateWindow(GtkWidget *parent)
void ViewDicomSupervise::CreateWindow(void)
{
    GtkWidget *fixed;
    GtkWidget *labelNote;
    GtkWidget *btnExport;
    GtkWidget *btnRegister;
    GtkWidget *hseparator1;
    GtkWidget *labelKey;

    GtkWidget *hseparator2;
    GtkWidget *btnExit;

    GtkWidget *window_register;
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (m_window, 480, 400);
	gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
	gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
	gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_title (GTK_WINDOW (m_window), _("DICOM Register"));
	gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
	//gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(parent));
	g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(HandleWindowDeleteEvent), this);

    fixed = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    labelNote = gtk_label_new(_("Note: According to the license file,the manufacturer will generate\na unique register key for every machine,you can register DICOM\nfunction by inputting thist register key.If register is successful,\nyou can use the DICOM function.\n\nPlease insert Udisk,and click button 'Export License File'. You will\nfind this file named 'license' in Udisk. Please send this file to\nmanufacturer."));
    gtk_widget_show(labelNote);
    gtk_fixed_put (GTK_FIXED (fixed), labelNote, 20, 15);
    gtk_label_set_use_markup (GTK_LABEL (labelNote), TRUE);
    gtk_widget_set_size_request (labelNote, 450, 112);

    m_labelExport = gtk_label_new ("");
    gtk_widget_show (m_labelExport);
    gtk_fixed_put (GTK_FIXED (fixed), m_labelExport, 10, 155);
    gtk_label_set_use_markup (GTK_LABEL (m_labelExport), TRUE);
    gtk_widget_set_size_request (m_labelExport, 240, 33);
    
    btnExport = gtk_button_new_with_mnemonic(_("Export License File"));
    gtk_fixed_put (GTK_FIXED (fixed), btnExport, 250, 155);
    gtk_widget_set_size_request (btnExport, 200, 35);
	g_signal_connect (G_OBJECT(btnExport), "clicked", G_CALLBACK(HandleBtnExportClicked), this);
    hseparator1 = gtk_hseparator_new ();
    gtk_widget_show (hseparator1);
    gtk_fixed_put (GTK_FIXED (fixed), hseparator1, 8, 200);
    gtk_widget_set_size_request (hseparator1, 450, 16);

    labelKey = gtk_label_new (_("Register Key:"));
    gtk_widget_show (labelKey);
    gtk_fixed_put (GTK_FIXED (fixed), labelKey, 30, 245);
    gtk_label_set_use_markup (GTK_LABEL (labelKey), TRUE);

    m_entry_key = gtk_entry_new ();
    gtk_widget_show (m_entry_key);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_key, 130+8, 240);
    gtk_widget_set_size_request (m_entry_key, 200+115, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_key), 9679);

    m_labelRegister = gtk_label_new ("");
    gtk_widget_show (m_labelRegister);
    gtk_fixed_put (GTK_FIXED (fixed), m_labelRegister, 10, 300);
    gtk_label_set_use_markup (GTK_LABEL (m_labelRegister), TRUE);
    gtk_widget_set_size_request (m_labelRegister, 200, 33);

    btnRegister = gtk_button_new_with_mnemonic(_("Register"));
    gtk_fixed_put (GTK_FIXED (fixed), btnRegister, 300+45, 300);
    gtk_widget_set_size_request (btnRegister, 100, 35);
	g_signal_connect (G_OBJECT(btnRegister), "clicked", G_CALLBACK(HandleBtnRegisterClicked), this);
    
    hseparator2 = gtk_hseparator_new ();
    gtk_widget_show (hseparator2);
    gtk_fixed_put (GTK_FIXED (fixed), hseparator2, 8, 340);
    gtk_widget_set_size_request (hseparator2, 450, 16);

    btnExit = gtk_button_new_with_mnemonic(_("Exit"));
    gtk_fixed_put (GTK_FIXED (fixed), btnExit, 300+45, 360);
    gtk_widget_set_size_request (btnExit, 100, 35);
	g_signal_connect (G_OBJECT(btnExit), "clicked", G_CALLBACK(HandleBtnExitClicked), this);
    
    gtk_widget_show_all(m_window);
    window_register = m_window;

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();

    //return window_register;
}

void ViewDicomSupervise::Authenticate(void)
{
    if (m_timer > 0)
    {
        g_source_remove(m_timer);
        m_timer = 0;
        g_keyInterface.Pop();
    }

    g_keyInterface.Push(this);
    m_vecAuthenInfo.clear();
    m_statusAuthen = TRUE;
    m_timer = g_timeout_add(10000, HandleAuthen, NULL);
}

gboolean ViewDicomSupervise::IsAuthenValid(void)
{
    m_timer = 0;
    if (m_vecAuthenInfo.empty() || ((!m_vecAuthenInfo.empty()) && m_vecAuthenInfo[0] != 'I'))
    {
        PRINTF("not authen\n");
        Exit();
    }
    else
    {
        PRINTF("authen\n");
    }

    return FALSE;
}

void ViewDicomSupervise::ConfirmAuthen(void)
{
    unsigned char info[AUTHEN_NUM+1]; 
    int size = m_vecAuthenInfo.size();
    int i;

    ASSERT(size <= (int)AUTHEN_NUM);
    info[size] = '\0';
    for (i = 0; i < size; i ++)
    {
        info[i] = m_vecAuthenInfo[i]; 
    }

    // clear
    m_vecAuthenInfo.clear();

    // compare
    if (strcmp((const char*)info, "icome") == 0)
        CreateWindow();    
}

void ViewDicomSupervise::BtnExportClicked(GtkButton *button)
{
   PeripheralMan *ptr = PeripheralMan::GetInstance();
   char info1[256];
    if(!ptr->CheckUsbStorageState())
    {
        sprintf(info1,"%s",_("No USB storage found!"));
        gtk_label_set_text(GTK_LABEL(m_labelExport),info1);
        gtk_widget_show (m_labelExport);

        return;
    }
    else
    {
        if(!ptr->MountUsbStorage())
        {
            sprintf(info1,"%s",_("Failed to mount USB storage!"));
            gtk_label_set_text(GTK_LABEL(m_labelExport),info1);
            gtk_widget_show (m_labelExport);
            return;
        }
    }
    string destFileDir = UDISK_PATH;
    if(CDCMRegister::GetMe()==NULL)
        return;
    bool value = CDCMRegister::GetMe()->GenerateLicenseFile(destFileDir);
    char info[256];
    if(value)
    {
        sprintf(info,"%s",_("Succeed to export file!"));
    }
    else
    {
        sprintf(info,"%s",_("Fail to export file!"));
    }
    gtk_label_set_text(GTK_LABEL(m_labelExport),info);
    gtk_widget_show (m_labelExport);

    ptr->UmountUsbStorage();
}

void ViewDicomSupervise::BtnRegisterClicked(GtkButton *button)
{
   const gchar *registerKey = gtk_entry_get_text(GTK_ENTRY(m_entry_key));
    if(CDCMRegister::GetMe()==NULL)
        return;

    bool value =CDCMRegister::GetMe()->CheckAuthorize(registerKey); 
    char info[256];
    if(value)
    {
        sprintf(info,"%s",_("Succeed to register!"));
    }
    else
    {
        sprintf(info,"%s",_("Fail to register!"));
    }
    gtk_label_set_text(GTK_LABEL(m_labelRegister),info);
    gtk_widget_show (m_labelRegister);
}

void ViewDicomSupervise::BtnExitClicked(GtkButton *button)
{
    DestroyWindow();
}

static gboolean ExitWindow(gpointer data)
{
    ViewDicomSupervise *tmp;
    tmp = (ViewDicomSupervise*)data;
    tmp->DestroyWindow();
    return FALSE;
}

void ViewDicomSupervise::KeyEvent(unsigned char keyValue)
{
    switch(keyValue)
    {
        case KEY_ESC:
            g_timeout_add(100, ExitWindow, this);
			FakeEscKey();
			break;

		default:
            FakeXEvent::KeyEvent(keyValue);
			break;

#if 0
            if (m_statusAuthen)
            {
                Exit();
            }
            else
            {
                //DestroyWindow();
                g_timeout_add(100, ExitWindow, this);
                FakeEscKey();
            }
            break;
        default:
            {
                if (m_statusAuthen)
                {
                    m_vecAuthenInfo.push_back(keyValue);
                    if (m_vecAuthenInfo.size() == AUTHEN_NUM)
                    {
                        g_keyInterface.Pop();
                        m_statusAuthen = FALSE;
                        ConfirmAuthen();
                    }
                }
                else
                {
                    FakeXEvent::KeyEvent(keyValue);
                }
            }
            break;
#endif
    }
}
