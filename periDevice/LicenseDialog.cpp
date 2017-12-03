#include "LicenseDialog.h"
#include "gui_global.h"
#include "Authorize.h"
#include "gui_func.h"
#include "KeyValueOpr.h"
#include "ViewMain.h"
#include "KeyDef.h"
#include "./sysMan/SysGeneralSetting.h"
CLicenseDialog *CLicenseDialog::m_ptrInstance = NULL;
const char *g_warnInfo[3] = 
{
	N_("Not inserted Ukey, please insert Ukey and click button 'OK' to try again.\nClick button 'Close' to power off."), 
	N_("The Ukey has expired, please power off and update license file for Ukey."), 
	N_("Not authorize, please contact vendor.")
};

CLicenseDialog *CLicenseDialog::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new CLicenseDialog;

	return m_ptrInstance;
}

CLicenseDialog::CLicenseDialog() : m_window(NULL), m_isInitial(true), m_preCursor(true)
{
}

void CLicenseDialog::Create(WarnType type, bool isInitial)
{
	m_isInitial = isInitial;

	GtkWidget *fixed_window;
    GtkWidget *image_icon;

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_widget_set_size_request (m_window, 420, 150);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_decorated (GTK_WINDOW(m_window), FALSE);
	if(!isInitial)
	{
		gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
	}
	//gtk_window_set_keep_above(GTK_WINDOW(m_window), TRUE);

    fixed_window = gtk_fixed_new ();
    gtk_widget_show (fixed_window);
    gtk_container_add (GTK_CONTAINER (m_window), fixed_window);

    label_text = gtk_label_new (_(g_warnInfo[type]));
    gtk_widget_show(label_text);
    gtk_fixed_put (GTK_FIXED (fixed_window), label_text, 85, 15);
    gtk_widget_set_size_request (label_text, 320, 60);
    gtk_misc_set_alignment (GTK_MISC (label_text), 0, 0.5);
    gtk_label_set_line_wrap (GTK_LABEL(label_text), TRUE);
    gtk_label_set_line_wrap_mode (GTK_LABEL(label_text), PANGO_WRAP_WORD_CHAR);

    image_icon = gtk_image_new();
    gtk_widget_show(image_icon);
    gtk_fixed_put (GTK_FIXED (fixed_window), image_icon, 20, 20);

    gtk_image_set_from_stock(GTK_IMAGE(image_icon), "gtk-dialog-warning", GTK_ICON_SIZE_DIALOG);

	button_ok = gtk_button_new_from_stock ("gtk-ok");
	if (type == NODEVICE)
		gtk_widget_show(button_ok);
	else
		gtk_widget_hide(button_ok);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_ok, 200, 100);
    gtk_widget_set_size_request (button_ok, 100, 35);
    g_signal_connect ((gpointer) button_ok, "clicked", G_CALLBACK (ButtonOkClicked), this);

    button_close = gtk_button_new_from_stock ("gtk-close");
    gtk_fixed_put (GTK_FIXED (fixed_window), button_close, 310, 100);
    gtk_widget_set_size_request (button_close, 100, 35);
    g_signal_connect ((gpointer) button_close, "clicked", G_CALLBACK (ButtonCloseClicked), this);
	gtk_widget_show(button_close);

    gtk_widget_show(m_window);

	if (!isInitial)
	{
		m_preCursor = ViewMain::GetInstance()->GetCursorVisible();
		InvisibleCursor(false, false);
	}
    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
}

void CLicenseDialog::ButtonOkClicked(GtkButton *button, gpointer data)
{
    CLicenseDialog *pClass = (CLicenseDialog *)data;

    SysGeneralSetting sysGS;
    int index_fid = sysGS.GetFID();
    int status = CAuthorize::GetInstance()->CheckAuthorize(index_fid);
    // int status = CAuthorize::GetInstance()->CheckAuthorize(FEATURE);
    if (status == -1)
    {
		return;
    }
	else if (status == -2)
	{
		gtk_widget_hide(pClass->button_ok);
		gtk_label_set_text(GTK_LABEL(pClass->label_text), _(g_warnInfo[EXPIRE]));
	}
	else if (status == -3)
	{
		gtk_widget_hide(pClass->button_ok);
		gtk_label_set_text(GTK_LABEL(pClass->label_text), _(g_warnInfo[NOAUTHORIZE]));
	}
	else
	{
		CAuthorize::GetInstance()->AddTimeout();
		gtk_widget_destroy(pClass->m_window);

		g_keyInterface.Pop();
		pClass->m_window = NULL;
		if (!pClass->m_isInitial)
		{
			InvisibleCursor(!pClass->m_preCursor);
			if (g_keyInterface.Size() == 1)
				SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
		}
		else
			gtk_main_quit();
	}
}

void CLicenseDialog::ButtonCloseClicked(GtkButton *button, gpointer data)
{
	execlp("poweroff", "poweroff", NULL);
}

void CLicenseDialog::KeyEvent(unsigned char keyValue)
{
	switch(keyValue) 
	{
		case KEY_SET:
			FakeMouseButton(keyValue);
			break;
		//case KEY_ESC:
		//	ButtonCloseClicked(GTK_BUTTON(button_close), (gpointer)this);
			break;
		default:
			break;
	}
}
