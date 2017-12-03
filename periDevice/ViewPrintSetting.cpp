#include "ViewPrintSetting.h"
#include "../display/gui_func.h"
#include "../display/gui_global.h"
#include "../keyboard/KeyValueOpr.h"
#include "../sysMan/SysGeneralSetting.h"
#include "../keyboard/KeyDef.h"
#include "../sysMan/SysPrinterSetting.h"
#include "Printer.h"
#include "ViewPrintPreview.h"

const int MediaSizeMax = 5;
const int LandscapeMax = 2;
const int PageSizeMax = 5;
const int SonyGammaMax = 4;
const char* PrinterMediaSize[MediaSizeMax] = {"A4", "Letter", "Legal", "COM10", "DL"};
const char* PrinterDirection[LandscapeMax] = {"landscape", "landscape=90"};
const char* PrinterPageSize[PageSizeMax] = {"960x1280", "1280x1280", "1920x1280", "3840x1280", "4096x1280"};
const char* PrinterSonyGamma[SonyGammaMax] = {"TONE1", "TONE2", "TONE3", "TONE4"};

ViewPrintSetting* ViewPrintSetting::m_ptrInstance = NULL;

ViewPrintSetting* ViewPrintSetting::GetInstance()
{
    if (m_ptrInstance == NULL)
	m_ptrInstance = new ViewPrintSetting;
    return m_ptrInstance;
}

ViewPrintSetting::ViewPrintSetting(void)
{
}

ViewPrintSetting::~ViewPrintSetting(void)
{
    if (m_ptrInstance != NULL) 
	delete m_ptrInstance; 
}

bool ViewPrintSetting::SonyURI(void)
{
    Printer prt;
    string uriName;
    bool sonyoront;

    const string uri = "SONY/UP-D897";
    prt.GetUsbPrinterUri(uriName);
    return sonyoront = (uriName.find(uri) != string::npos);
}

void ViewPrintSetting::KeyEvent(unsigned char keyValue)
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

void ViewPrintSetting::CreateWindow(GtkWindow *parent)
{
    GtkWidget *fixed_window;
    GtkWidget *frame_window;
    GtkWidget *fixed;
    GtkObject *spinbutton_copies_adj;
    GtkObject *spinbutton_gamma_adj;
    GtkObject *spinbutton_brightness_adj;
    GtkWidget *label_media;
    GtkWidget *label_copies;
    GtkWidget *label_gamma;
    GtkWidget *label_brightness;
    GtkWidget *label_title;
    GtkWidget *label_landscade;
    GtkWidget *label_pagesize;

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, 600, 400);
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(m_window), parent);
    gtk_window_set_decorated (GTK_WINDOW (m_window), FALSE);

    fixed_window = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (m_window), fixed_window);

    frame_window = gtk_frame_new (NULL);
    gtk_fixed_put (GTK_FIXED (fixed_window), frame_window, 20, 20);
    gtk_widget_set_size_request (frame_window, 560, 280);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_window), GTK_SHADOW_OUT);

    fixed = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (frame_window), fixed);

    spinbutton_copies_adj = gtk_adjustment_new (1, 1, 10, 1, 1, 0);
    m_spinbuttonCopies = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_copies_adj), 1, 0);
    gtk_fixed_put (GTK_FIXED (fixed), m_spinbuttonCopies, 130, 30);
    gtk_widget_set_size_request (m_spinbuttonCopies, 80, 27);
    label_copies = gtk_label_new (_("Print Copies:"));
    gtk_fixed_put (GTK_FIXED (fixed), label_copies, 30, 30);
    gtk_widget_set_size_request (label_copies, 100, 30);

    if(SonyURI()){
		m_comboboxLandscade = gtk_combo_box_new_text ();
		gtk_fixed_put (GTK_FIXED (fixed), m_comboboxLandscade, 130, 80);
		gtk_widget_set_size_request (m_comboboxLandscade, 100, 30);
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxLandscade), PrinterDirection[0]);
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxLandscade), PrinterDirection[1]);
		label_landscade = gtk_label_new (_("Direction:"));
		gtk_fixed_put (GTK_FIXED (fixed), label_landscade, 30, 80);
		gtk_widget_set_size_request (label_landscade, 100, 30);

		m_comboboxPageSize = gtk_combo_box_new_text ();
		gtk_fixed_put (GTK_FIXED (fixed), m_comboboxPageSize, 130, 130);
		gtk_widget_set_size_request (m_comboboxPageSize, 100, 30);
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxPageSize), PrinterPageSize[0]);
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxPageSize), PrinterPageSize[1]);
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxPageSize), PrinterPageSize[2]);
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxPageSize), PrinterPageSize[3]);
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxPageSize), PrinterPageSize[4]);
		label_pagesize = gtk_label_new (_("PageSize:"));
		gtk_fixed_put (GTK_FIXED (fixed), label_pagesize, 30, 130);
		gtk_widget_set_size_request (label_pagesize, 100, 30);

		m_comboboxSonyGamma = gtk_combo_box_new_text ();
		gtk_fixed_put (GTK_FIXED (fixed), m_comboboxSonyGamma, 360, 30);
		gtk_widget_set_size_request (m_comboboxSonyGamma, 100, 30);
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSonyGamma), PrinterSonyGamma[0]);
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSonyGamma), PrinterSonyGamma[1]);
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSonyGamma), PrinterSonyGamma[2]);
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSonyGamma), PrinterSonyGamma[3]);
		label_pagesize = gtk_label_new (_("SonyGamma:"));
		gtk_fixed_put (GTK_FIXED (fixed), label_pagesize, 260, 30);
		gtk_widget_set_size_request (label_pagesize, 100, 30);
	}
    else{
			spinbutton_gamma_adj = gtk_adjustment_new (1000, 0, 10000, 500, 500, 0);
    	    m_spinbuttonGamma = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_gamma_adj), 1, 0);
    	    gtk_fixed_put (GTK_FIXED (fixed), m_spinbuttonGamma, 130, 80);
    	    gtk_widget_set_size_request (m_spinbuttonGamma, 80, 27);
    	    label_gamma = gtk_label_new (_("Gamma:"));
    	    gtk_fixed_put (GTK_FIXED (fixed), label_gamma, 30, 80);
    	    gtk_widget_set_size_request (label_gamma, 100, 30);

    	    spinbutton_brightness_adj = gtk_adjustment_new (100, 0, 200, 10, 10, 0);
    	    m_spinbuttonBrightness = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_brightness_adj), 1, 0);
    	    gtk_fixed_put (GTK_FIXED (fixed), m_spinbuttonBrightness, 130, 130);
    	    gtk_widget_set_size_request (m_spinbuttonBrightness, 80, 27);
    	    // gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonBrightness), 1);
    	    // g_signal_connect(G_OBJECT(m_spinbuttonBrightness), "insert_text", G_CALLBACK(on_spinbutton_insert_brightness), this);
    	    label_brightness = gtk_label_new (_("Brightness:"));
    	    gtk_fixed_put (GTK_FIXED (fixed), label_brightness, 30, 130);
    	    gtk_widget_set_size_request (label_brightness, 100, 30);

    	    m_comboboxMedia = gtk_combo_box_new_text ();
    	    gtk_fixed_put (GTK_FIXED (fixed), m_comboboxMedia, 360+12, 30);
    	    gtk_widget_set_size_request (m_comboboxMedia, 100, 30);
    	    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxMedia), PrinterMediaSize[0]);
    	    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxMedia), PrinterMediaSize[1]);
    	    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxMedia), PrinterMediaSize[2]);
    	    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxMedia), PrinterMediaSize[3]);
    	    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxMedia), PrinterMediaSize[4]);
    	    label_media = gtk_label_new (_("Media Size:"));
    	    gtk_fixed_put (GTK_FIXED (fixed), label_media, 260, 30);
    	    gtk_widget_set_size_request (label_media, 100+12, 30);
    }

    label_title = gtk_label_new (_("<b>Printer Setting</b>"));
    gtk_frame_set_label_widget (GTK_FRAME (frame_window), label_title);
    gtk_label_set_use_markup (GTK_LABEL (label_title), TRUE);

    GtkWidget *image_default = gtk_image_new_from_stock ("gtk-redo", GTK_ICON_SIZE_BUTTON);
    GtkWidget *label_default = gtk_label_new_with_mnemonic (_("Default\nFactory"));
    GtkWidget *button_default = create_button_icon(label_default, image_default);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_default, 180, 330);

    GtkWidget *image_ok = gtk_image_new_from_stock ("gtk-ok", GTK_ICON_SIZE_BUTTON);
    GtkWidget *label_ok = gtk_label_new_with_mnemonic (_("OK"));
    GtkWidget *button_ok = create_button_icon(label_ok, image_ok);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_ok, 320, 330);

    GtkWidget *image_cancel = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    GtkWidget *label_cancel = gtk_label_new_with_mnemonic (_("Cancel"));
    GtkWidget *button_cancel = create_button_icon(label_cancel, image_cancel);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_cancel, 460, 330);

    g_signal_connect ((gpointer) button_ok, "clicked", G_CALLBACK (on_button_ok_clicked), this);
    g_signal_connect ((gpointer) button_cancel, "clicked", G_CALLBACK (on_button_cancel_clicked), this);
    g_signal_connect ((gpointer) button_default, "clicked", G_CALLBACK (on_button_default_clicked), this);

    UpdateSetting();

    g_keyInterface.Push(this);
    gtk_widget_show_all(m_window);
}

void ViewPrintSetting::BtnCancelClicked(GtkButton *button)
{
    g_keyInterface.Pop();
    gtk_widget_destroy(m_window);
}

void ViewPrintSetting::BtnOkClicked(GtkButton *button)
{
    SysPrinterSetting sps;

    int copies = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbuttonCopies));
	sps.SetCopies(copies);
    if(SonyURI()){
    	int landscape_ang = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxLandscade));
    	int pagesize = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxPageSize));
    	int sonygamma = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxSonyGamma));
        sps.SetLandscape(landscape_ang);
        sps.SetPageSize(pagesize);
        sps.SetSonyGamma(sonygamma);
    }
    else{
        int gamma = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbuttonGamma));
        int brightness = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbuttonBrightness));
        int index_media_size = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxMedia));
		sps.SetGamma(gamma);
		sps.SetBrightness(brightness);
		sps.SetMediaSize(index_media_size);
    }

    sps.SyncFile();

    g_keyInterface.Pop();
    gtk_widget_destroy(m_window);
}

void ViewPrintSetting::BtnDefaultClicked(GtkButton *button)
{
    SysPrinterSetting sps;

    sps.DefaultFactory();
    gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonCopies), sps.GetCopies());
    if(SonyURI()){
        gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboboxLandscade), sps.GetLandscape());
        gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboboxPageSize), sps.GetPageSize());
        gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboboxSonyGamma), sps.GetSonyGamma());
    }
    else {
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonGamma), sps.GetGamma());
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonBrightness), sps.GetBrightness());
		gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboboxMedia), sps.GetMediaSize());
    }

}

void ViewPrintSetting::UpdateSetting(void)
{
    SysPrinterSetting sps;

    gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonCopies), sps.GetCopies());
    if(SonyURI()){
        gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboboxLandscade), sps.GetLandscape());
        gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboboxPageSize), sps.GetPageSize());
        gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboboxSonyGamma), sps.GetSonyGamma());
    }
    else{
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonGamma), sps.GetGamma());
		gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbuttonBrightness), sps.GetBrightness());
		gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboboxMedia), sps.GetMediaSize());
    }
}
