#include "gui_func.h"
#include "gui_global.h"
#include "ViewPrinterAdd.h"
#include "KeyValueOpr.h"
#include "KeyDef.h"
#include "Printer.h"
#include "../sysMan/ViewSystem.h"
#include "ViewPPDSelect.h"
#include "../display/ViewDialog.h"
#include "../periDevice/PeripheralMan.h"

using std::string;

ViewPrinterAdd* ViewPrinterAdd::m_ptrInstance = NULL;

ViewPrinterAdd::ViewPrinterAdd()
{
}

ViewPrinterAdd::~ViewPrinterAdd()
{
    if (m_ptrInstance != NULL)
	delete m_ptrInstance;
}

ViewPrinterAdd* ViewPrinterAdd::GetInstance()
{
    if (m_ptrInstance == NULL)
	m_ptrInstance = new ViewPrinterAdd;

    return m_ptrInstance;
}

void ViewPrinterAdd::CreateWindow(GtkWindow *parent)
{
    GtkWidget *fixed_window;
    GtkWidget *label_printer_uri;
    GtkWidget *label_uri;
    GtkWidget *label_printer_name;
    GtkWidget *label_ppd_file;
    GtkWidget *label_ppd_note;
    // GtkWidget *filechooserbutton_ppd_file;
    GtkWidget *button_ppd_file;
    GtkWidget *hbox;
    GtkWidget *vseparator;
    GtkWidget *ppd_image;

    GtkWidget *hseparator;
    GtkWidget *button_add;
    GtkWidget *button_cancel;

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, 530, 220);
    gtk_window_set_title (GTK_WINDOW (m_window), _("Add Printer"));
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(m_window), parent);
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this);

    fixed_window = gtk_fixed_new ();
    gtk_widget_show (fixed_window);
    gtk_container_add (GTK_CONTAINER (m_window), fixed_window);

    label_printer_uri = gtk_label_new (_("Printer URI:"));
    gtk_widget_show (label_printer_uri);
    gtk_fixed_put (GTK_FIXED (fixed_window), label_printer_uri, 20, 10);
    gtk_widget_set_size_request (label_printer_uri, 140, 30);
    gtk_misc_set_alignment (GTK_MISC (label_printer_uri), 0, 0.5);

    label_uri = gtk_label_new (_("URI not found!"));
    gtk_widget_show (label_uri);
    gtk_fixed_put (GTK_FIXED (fixed_window), label_uri, 160, 10);
    gtk_widget_set_size_request (label_uri, 300, 30);
    gtk_misc_set_alignment (GTK_MISC (label_uri), 0, 0.5);

    Printer prt;
    prt.GetUsbPrinterUri(m_uriName);
    if (!m_uriName.empty())
        gtk_label_set_text(GTK_LABEL(label_uri), m_uriName.c_str());

    label_printer_name = gtk_label_new (_("Printer Name:"));
    gtk_widget_show (label_printer_name);
    gtk_fixed_put (GTK_FIXED (fixed_window), label_printer_name, 20, 50);
    gtk_widget_set_size_request (label_printer_name, 140, 30);
    gtk_misc_set_alignment (GTK_MISC (label_printer_name), 0, 0.5);

    m_entry_printer_name = gtk_entry_new ();
    gtk_widget_show (m_entry_printer_name);
    gtk_fixed_put (GTK_FIXED (fixed_window), m_entry_printer_name, 160, 50);
    gtk_widget_set_size_request (m_entry_printer_name, 200, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_printer_name), 9679);

    label_ppd_file = gtk_label_new (_("Specifies a PPD file:"));
    gtk_widget_show (label_ppd_file);
    gtk_fixed_put (GTK_FIXED (fixed_window), label_ppd_file, 20, 90);
    gtk_widget_set_size_request (label_ppd_file, 140, 30);
    gtk_misc_set_alignment (GTK_MISC (label_ppd_file), 0, 0.5);

    button_ppd_file = gtk_button_new ();
    gtk_widget_show (button_ppd_file);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_ppd_file, 160, 90);
    gtk_widget_set_size_request (button_ppd_file, 100, 30);

    hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox);
    gtk_container_add (GTK_CONTAINER (button_ppd_file), hbox);

    m_labelPPDName = gtk_label_new ("");
    gtk_widget_show (m_labelPPDName);
    gtk_box_pack_start (GTK_BOX (hbox), m_labelPPDName, TRUE, TRUE, 0);

    vseparator = gtk_vseparator_new ();
    gtk_widget_show (vseparator);
    gtk_box_pack_start (GTK_BOX (hbox), vseparator, FALSE, TRUE, 0);
    gtk_widget_set_size_request (vseparator, 12, 1);

    ppd_image = gtk_image_new_from_icon_name ("gtk-open", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show (ppd_image);
    gtk_box_pack_start (GTK_BOX (hbox), ppd_image, FALSE, FALSE, 0);
    gtk_widget_set_size_request (ppd_image, 22, 22);

    g_signal_connect (G_OBJECT(button_ppd_file), "clicked", G_CALLBACK(on_button_ppd_file_clicked), this);

    label_ppd_note = gtk_label_new (_("(PostScript Printer Description file)"));
    gtk_widget_show (label_ppd_note);
    gtk_fixed_put (GTK_FIXED (fixed_window), label_ppd_note, 270, 90);
    gtk_widget_set_size_request (label_ppd_note, 240+10, 30);
    gtk_misc_set_alignment (GTK_MISC (label_ppd_note), 0, 0.5);

    hseparator = gtk_hseparator_new ();
    gtk_widget_show (hseparator);
    gtk_fixed_put (GTK_FIXED (fixed_window), hseparator, 20, 130);
    gtk_widget_set_size_request (hseparator, 490, 15);

    GtkWidget *image_add = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_BUTTON);
    GtkWidget *label_add = gtk_label_new_with_mnemonic (_("Add"));
    button_add = create_button_icon(label_add, image_add);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_add, 250, 150);
    g_signal_connect ((gpointer) button_add, "clicked",
                      G_CALLBACK (on_button_add_clicked),
                      this);
 
    GtkWidget *image_cancel = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    GtkWidget *label_cancel = gtk_label_new_with_mnemonic (_("Cancel"));
    button_cancel = create_button_icon(label_cancel, image_cancel);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_cancel, 390, 150);
    g_signal_connect ((gpointer) button_cancel, "clicked",
                      G_CALLBACK (on_button_cancel_clicked),
                      this);

    gtk_widget_show_all(m_window);

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
   
    return ;
}

void ViewPrinterAdd::DestroyWindow(void)
{
    if(GTK_IS_WIDGET(m_window)) {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        m_window = NULL;
    }
}

gboolean ViewPrinterAdd::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWindow();
    return FALSE;
}

void ViewPrinterAdd::BtnPPDNameClicked(GtkButton *button)
{
    ViewPPDSelect::GetInstance()->CreateWindow(GTK_WINDOW(m_window));
}

void ViewPrinterAdd::BtnAddClicked(GtkButton *button)
{
    if (m_uriName.empty()) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                                          ViewDialog::ERROR,
                                          _("Add Printer Error: URI not found!"), 
                                          NULL);
        return;
    }
    if (!gtk_entry_get_text_length(GTK_ENTRY(m_entry_printer_name))) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                                          ViewDialog::ERROR,
                                          _("Add Printer Error: Please enter printer name!"), 
                                          NULL);
        gtk_widget_grab_focus(m_entry_printer_name);
        return;
    }

    if (m_ppdFileName.empty()) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                                          ViewDialog::ERROR,
                                          _("Add Printer Error: Please select a PPD file!"), 
                                          NULL);
        return ;
    }

    Printer prt;
    const char* prt_name = gtk_entry_get_text(GTK_ENTRY(m_entry_printer_name));
    PRINTF("print name : %s\nuri : %s\nppd file : %s\n", prt_name, m_uriName.c_str(), m_ppdFileName.c_str());
    prt.AddPrinter(prt_name, m_uriName.c_str(), m_ppdFileName.c_str());
	PeripheralMan::GetInstance()->SwitchPrinterDriver();
    ViewSystem::GetInstance()->update_specific_printer_model();
    DestroyWindow();
}

void ViewPrinterAdd::BtnCancelClicked(GtkButton *button)
{
    DestroyWindow();
}

void ViewPrinterAdd::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);
    
    switch(keyValue) {
    case KEY_ESC:
	BtnCancelClicked(NULL);
	break;
    default:
	break;
    }
}

void ViewPrinterAdd::SetPPDName(const char* name, const char *path)
{ 
    m_ppdFileName = path;
    gtk_label_set_text(GTK_LABEL(m_labelPPDName), name);
}
