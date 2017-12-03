#include <gtk/gtk.h>
#include "Def.h"
#include "ViewDialog.h"
#include "KeyValueOpr.h"
#include "gui_global.h"
#include "gui_func.h"
#include "ViewMain.h"
#include "KeyDef.h"

ViewDialog* ViewDialog::m_ptrInstance = NULL;

ViewDialog* ViewDialog::GetInstance()
{
    if (m_ptrInstance == NULL)
	m_ptrInstance = new ViewDialog;
    return m_ptrInstance;
}

ViewDialog::ViewDialog(void)
{
    m_window = NULL;
    m_ptrFunc = NULL;
    m_ptrFuncCancel = NULL;
	m_preCursor = true;
}

ViewDialog::~ViewDialog(void)
{
    if (m_ptrInstance != NULL) 
	delete m_ptrInstance; 
}

void ViewDialog::Create(GtkWindow *parent, EDialogType type, const char *info, pFuncDialog ptrFunc, pFuncDialog ptrFuncCancel)
{
    GtkWidget *fixed_window;
    GtkWidget *image_icon;
//    GtkWidget *label_text;

    GtkWidget *button_ok;
    GtkWidget *button_cancel;
    GtkWidget *button_prg_cancel;
    GtkWidget *image_prg_cancel;
    GtkWidget *button_close;

    m_ptrFunc = ptrFunc;
    m_ptrFuncCancel = ptrFuncCancel;
    m_type = type;
	m_preCursor = ViewMain::GetInstance()->GetCursorVisible();
	InvisibleCursor(false, false);

    if (ViewHintDialog::GetInstance()->Exist())
        ViewHintDialog::GetInstance()->Destroy();

    GtkWidget *old_window = m_window;
    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_widget_set_size_request (m_window, 420, 150);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(m_window), parent);
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this);

    fixed_window = gtk_fixed_new ();
    gtk_widget_show (fixed_window);
    gtk_container_add (GTK_CONTAINER (m_window), fixed_window);

    image_icon = gtk_image_new();
    gtk_widget_show(image_icon);
    gtk_fixed_put (GTK_FIXED (fixed_window), image_icon, 20, 20);

    m_label_text = gtk_label_new (info);
    gtk_widget_show(m_label_text);
    gtk_fixed_put (GTK_FIXED (fixed_window), m_label_text, 85, 15);
    gtk_widget_set_size_request (m_label_text, 320, 60);
    gtk_misc_set_alignment (GTK_MISC (m_label_text), 0, 0.5);
    gtk_label_set_line_wrap (GTK_LABEL(m_label_text), TRUE);
    gtk_label_set_line_wrap_mode (GTK_LABEL(m_label_text), PANGO_WRAP_WORD_CHAR);

    button_ok = gtk_button_new_from_stock ("gtk-ok");
    gtk_widget_hide(button_ok);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_ok, 200, 100);
    gtk_widget_set_size_request (button_ok, 100, 35);
    g_signal_connect ((gpointer) button_ok, "clicked", G_CALLBACK (on_button_ok_clicked), this);

    button_cancel = gtk_button_new_from_stock ("gtk-cancel");
    gtk_widget_hide(button_cancel);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_cancel, 310, 100);
    gtk_widget_set_size_request (button_cancel, 100, 35);
    g_signal_connect ((gpointer) button_cancel, "clicked", G_CALLBACK (on_button_cancel_clicked), this);

    button_close = gtk_button_new_from_stock ("gtk-close");
    gtk_widget_hide(button_close);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_close, 310, 100);
    gtk_widget_set_size_request (button_close, 100, 35);
    g_signal_connect ((gpointer) button_close, "clicked", G_CALLBACK (on_button_close_clicked), this);

    m_entry = gtk_entry_new_with_max_length(64);
    gtk_widget_hide(m_entry);
    gtk_fixed_put(GTK_FIXED(fixed_window), m_entry, 85, 70);
    gtk_widget_set_size_request(m_entry, 250, 25);
    g_signal_connect ((gpointer) m_entry, "insert_text", G_CALLBACK (on_entry_insert_text), this);

    m_progress_bar = gtk_progress_bar_new();
    gtk_widget_hide(m_progress_bar);
    gtk_fixed_put(GTK_FIXED(fixed_window), m_progress_bar, 85, 90);
    gtk_widget_set_size_request(m_progress_bar, 250, 30);

    button_prg_cancel = gtk_button_new();
    gtk_widget_hide(button_prg_cancel);
    gtk_fixed_put(GTK_FIXED(fixed_window), button_prg_cancel, 350, 80);
    gtk_widget_set_size_request(button_prg_cancel, 45, 45);
    gtk_button_set_relief(GTK_BUTTON(button_prg_cancel), GTK_RELIEF_NONE);
    g_signal_connect ((gpointer) button_prg_cancel, "clicked", G_CALLBACK (on_button_prg_cancel_clicked), this);
    image_prg_cancel = gtk_image_new_from_stock("gtk-stop", GTK_ICON_SIZE_DND);
    gtk_widget_hide(image_prg_cancel);
    gtk_container_add(GTK_CONTAINER(button_prg_cancel), image_prg_cancel);

    switch (m_type) {
    case INFO:
        gtk_window_set_title (GTK_WINDOW (m_window), _("Information"));
        gtk_image_set_from_stock(GTK_IMAGE(image_icon), "gtk-dialog-info", GTK_ICON_SIZE_DIALOG);
        gtk_widget_show(button_close);
        break;
    case WARNING:
        gtk_window_set_title (GTK_WINDOW (m_window), _("Warning"));
        gtk_image_set_from_stock(GTK_IMAGE(image_icon), "gtk-dialog-warning", GTK_ICON_SIZE_DIALOG);
        gtk_widget_show(button_close);
        break;
    case QUESTION:
	//	gtk_window_set_title (GTK_WINDOW (m_window), _("Question"));
	//	gtk_window_set_title (GTK_WINDOW (m_window), info);
		gtk_window_set_title (GTK_WINDOW (m_window), "");
        gtk_image_set_from_stock(GTK_IMAGE(image_icon), "gtk-dialog-question", GTK_ICON_SIZE_DIALOG);
        gtk_widget_show(button_ok);
        gtk_widget_show(button_cancel);
        break;
    case ERROR:
        gtk_window_set_title (GTK_WINDOW (m_window), _("Error"));
        gtk_image_set_from_stock(GTK_IMAGE(image_icon), "gtk-dialog-error", GTK_ICON_SIZE_DIALOG);
        gtk_widget_show(button_close);
        break;
    // case HINT:
    //     gtk_window_set_decorated (GTK_WINDOW(m_window), FALSE);
    //     gtk_image_set_from_stock(GTK_IMAGE(image_icon), "gtk-dialog-info", GTK_ICON_SIZE_DIALOG);
    //     break;
    case PROGRESS:
        gtk_window_set_decorated (GTK_WINDOW(m_window), FALSE);
        gtk_image_set_from_stock(GTK_IMAGE(image_icon), "gtk-dialog-info", GTK_ICON_SIZE_DIALOG);
        gtk_widget_show(m_progress_bar);
        break;
    case PRG_CANCEL:
        gtk_window_set_decorated (GTK_WINDOW(m_window), FALSE);
        gtk_image_set_from_stock(GTK_IMAGE(image_icon), "gtk-dialog-info", GTK_ICON_SIZE_DIALOG);
        gtk_widget_show(m_progress_bar);
        gtk_widget_show(button_prg_cancel);
        gtk_widget_show(image_prg_cancel);
        break;
    case FILENAME:
        gtk_window_set_decorated (GTK_WINDOW(m_window), FALSE);
        gtk_image_set_from_stock(GTK_IMAGE(image_icon), "gtk-dialog-info", GTK_ICON_SIZE_DIALOG);
        gtk_widget_show(m_entry);
        gtk_widget_show(button_ok);
        gtk_widget_show(button_cancel);
        break;
    }
    gtk_widget_show(m_window);

	if (GTK_IS_WIDGET(old_window)) {
		g_keyInterface.Pop();
		gtk_widget_destroy (old_window);
	}

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
}

bool ViewDialog::Exist(void)
{
    if (m_window != NULL && GTK_IS_WINDOW(m_window))
        return true;
    else 
        return false;
}

static gboolean ExitWindow(gpointer data)
{
    ViewDialog *tmp;
    tmp = (ViewDialog *)data;
    tmp->Destroy();
    return FALSE;
}

void ViewDialog::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);

    switch(keyValue) {
    case KEY_ESC:
        if(m_type!=PRG_CANCEL && m_type!=PROGRESS) {
            // g_timeout_add(100, ExitWindow, this);
            // FakeEscKey();
            ExitWindow(this);
            if (m_ptrFuncCancel)
                (*m_ptrFuncCancel)(NULL);
        }
        break;
    default:
        break;
    }
}

gboolean ViewDialog::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    Destroy();
    if (m_ptrFuncCancel)
        (*m_ptrFuncCancel)(NULL);
    return FALSE;
}

void ViewDialog::SetText(const char *info)
{
	if(m_label_text)
		gtk_label_set_text(GTK_LABEL(m_label_text), info);
}

gboolean HandleFun(gpointer data)
{
    ViewDialog *tmp;
    tmp = (ViewDialog *)data;
    if (tmp->m_ptrFunc)
        (*tmp->m_ptrFunc)(NULL);
    return false;
 }

void ViewDialog::BtnOkClicked(GtkButton *button)
{
	// gtk_widget_destroy (m_window);
	// g_keyInterface.Pop();
	if(m_type == FILENAME)
	{
		char* name = (char*)malloc(256);
		sprintf(name, "%s", gtk_entry_get_text(GTK_ENTRY(m_entry)));

		Destroy();
		
		if(strlen(name)>0)
		{
			if (m_ptrFunc)
				(*m_ptrFunc)(name);
		}
	}
	else
	{
        Destroy();
        g_timeout_add(50, HandleFun, this); // 为了解决结束检查时出现白框的问题
#if 0 
		if (m_ptrFunc)
			(*m_ptrFunc)(NULL);
#endif
	}
}

void ViewDialog::BtnCancelClicked(GtkButton *button)
{
	// gtk_widget_destroy (m_window);
	// g_keyInterface.Pop();

    Destroy();
    if (m_ptrFuncCancel)
        (*m_ptrFuncCancel)(NULL);
}

void ViewDialog::BtnCloseClicked(GtkButton *button)
{
    Destroy();
    if (m_ptrFunc)
        (*m_ptrFunc)(NULL);
}

void ViewDialog::BtnPrgCancelClicked(GtkButton *button)
{
	SetText(_("Cancelling..."));
//	SetProgressBar(0);
	if (m_ptrFunc)
		(*m_ptrFunc)(NULL);
}

void ViewDialog::EntryInsertText(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));
	
    if (g_ascii_ispunct(*new_text) || (old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable)))) {
        gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
    }
}

void ViewDialog::Destroy(void)
{
    if (GTK_IS_WIDGET(m_window)) {
        g_keyInterface.Pop();
        gtk_widget_hide_all(m_window);
        gtk_widget_destroy (m_window);
        m_window = NULL;
        InvisibleCursor(!m_preCursor);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
}

void ViewDialog::SetProgressBar(double fraction)
{
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(m_progress_bar), fraction);
    char text_buf[10];
    sprintf(text_buf, "%d%%", (int)(fraction*100));
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(m_progress_bar), text_buf);
}


ViewHintDialog* ViewHintDialog::m_ptrInstance = NULL;

ViewHintDialog* ViewHintDialog::GetInstance()
{
    if (m_ptrInstance == NULL)
	m_ptrInstance = new ViewHintDialog;
    return m_ptrInstance;
}

ViewHintDialog::ViewHintDialog(void)
{
    m_window = NULL;
}

ViewHintDialog::~ViewHintDialog(void)
{
    if (m_ptrInstance != NULL) 
	delete m_ptrInstance; 
}

void ViewHintDialog::Create(GtkWindow *parent, const char *info)
{
    GtkWidget *fixed_window;
    GtkWidget *image_icon;
    GtkWidget *label_text;

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_widget_set_size_request (m_window, 420, 150);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(m_window), parent);
    gtk_window_set_decorated (GTK_WINDOW(m_window), FALSE);

    fixed_window = gtk_fixed_new ();
    gtk_widget_show (fixed_window);
    gtk_container_add (GTK_CONTAINER (m_window), fixed_window);

    label_text = gtk_label_new (info);
    gtk_widget_show(label_text);
    gtk_fixed_put (GTK_FIXED (fixed_window), label_text, 85, 15);
    gtk_widget_set_size_request (label_text, 320, 60);
    gtk_misc_set_alignment (GTK_MISC (label_text), 0, 0.5);
    gtk_label_set_line_wrap (GTK_LABEL(label_text), TRUE);
    gtk_label_set_line_wrap_mode (GTK_LABEL(label_text), PANGO_WRAP_WORD_CHAR);

    image_icon = gtk_image_new();
    gtk_widget_show(image_icon);
    gtk_fixed_put (GTK_FIXED (fixed_window), image_icon, 20, 20);

    gtk_image_set_from_stock(GTK_IMAGE(image_icon), "gtk-dialog-info", GTK_ICON_SIZE_DIALOG);
    gtk_widget_show(m_window);

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
}

bool ViewHintDialog::Exist(void)
{
    if (m_window != NULL && GTK_IS_WINDOW(m_window))
        return true;
    else 
        return false;
}

void ViewHintDialog::Destroy(void)
{
    if (GTK_IS_WIDGET(m_window)) {
        g_keyInterface.Pop();
        gtk_widget_destroy (m_window);
        m_window = NULL;
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
}
