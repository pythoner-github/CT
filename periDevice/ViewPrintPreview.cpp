#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// #include <errno.h>
#include "display/gui_func.h"
#include "display/gui_global.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/KeyDef.h"
#include "periDevice/ViewPrintPreview.h"
#include "ViewMain.h"
#include "display/ViewDialog.h"
#include "sysMan/SysGeneralSetting.h"
#include "periDevice/Usblp.h"
#include "periDevice/PeripheralMan.h"

#include "periDevice/Printer.h"

#include "periDevice/ViewPrintSetting.h"
#include "sysMan/SysPrinterSetting.h"
#include "calcPeople/ViewReport.h"
#include "patient/PatientInfo.h"
#include "display/TopArea.h"
#include "periDevice/Usblp.h"
#include "patient/ImgMan.h"
#include "calcPeople/ViewReport.h"
#include "sysMan/ViewSystem.h"
#include "patient/ViewDicomDataSelect.h"
#include "patient/ViewQueryRetrieve.h"

#include "periDevice/Printmain.h"
#include "periDevice/Printfunctions.h"

#if (1)  //use NEW Print MODE
#define MODE_PRINT_NEW (1)
#else
#define MODE_PRINT_NEW (0)
#endif

//#define PRINTDIR "./res/printer/"
//#define PRINTIMAGETEMP "./report"

extern const int MediaSizeMax = 5;
extern const int LandscapeMax = 2;
extern const int PageSizeMax = 5;
extern const int SonyGammaMax = 4;
extern const char* PrinterMediaSize[MediaSizeMax];
extern const char* PrinterDirection[LandscapeMax];
extern const char* PrinterPageSize[PageSizeMax];
extern const char* PrinterSonyGamma[SonyGammaMax];

ViewPrintPreview* ViewPrintPreview::m_ptrInstance = NULL;

//test
void GetMeaResult(gchar *title, gchar *value)
{
#if 0
    char *path[10];
    string strTemp;

    for (int i = 0; i < 10; i++)
    {
        path[i] = "l width value is 1.5";
    }
   for (int j = 0; j < 10; j++)
   {
       string strT1(path[j]);
       strTemp += strT1;
   }
      return strTemp.c_str();
#endif
      strcat(title,"l width");
      strcat(value, "1.5ms");
}

ViewPrintPreview::ViewPrintPreview(void)
{
    m_scrolledwindow_auto= NULL;
    m_scrolledwindow_orignial= NULL;
    m_pixmapPrintImage = 0;
    pagelayout = NULL;
    pagepixmap = NULL;
    pagegc = NULL;
    drawingarea = NULL;
    report_page_count = 0;
    page_num = false;
    m_path = NULL;
    for(int max = 30; max > 0; max--)
    {
        report_image[max-1] = NULL;
      }
    m_headFont = pango_font_description_from_string("DejaVu Sans Mono, Book, 28");
    m_baseFont = pango_font_description_from_string("DejaVu Sans Mono, Book, 14");
    m_commentTmpFont = pango_font_description_from_string("DejaVu Sans Mono, Book, 14");
}

ViewPrintPreview::~ViewPrintPreview(void)
{
    m_ptrInstance = NULL;
    pango_font_description_free(m_baseFont);
    pango_font_description_free(m_headFont);
    pango_font_description_free(m_commentTmpFont);
    if (m_path != NULL)
    {
        free(m_path);
        m_path = NULL;
    }
    if (pagegc != NULL)
    {
        g_object_unref(pagegc);
        pagegc = NULL;
    }
    if (pagelayout != NULL)
    {
        g_object_unref(pagelayout);
        pagelayout = NULL;
    }
    if (pagepixmap != NULL)
    {
        g_object_unref(pagepixmap);
        pagepixmap = NULL;
    }
}

ViewPrintPreview* ViewPrintPreview::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ViewPrintPreview;
    return m_ptrInstance;
}

void ViewPrintPreview::Printing(void)
{
    SysGeneralSetting sys_gen;
    if (sys_gen.GetPrinterMethod() == 0) {
        int ppd_sign;
        char print_command[256];
        char ppd_filename[20];

        ppd_sign = sys_gen.GetPrinter();

        switch (ppd_sign) {
        case 0:
            strcpy(ppd_filename, "laserjet.ppd");
            break;
        case 1:
            strcpy(ppd_filename, "deskjet.ppd");
            break;
        case 2:
            strcpy(ppd_filename, "deskjet2.ppd");
            break;
        case 3:
            strcpy(ppd_filename, "epson9.ppd");
            break;
        case 4:
            strcpy(ppd_filename, "epson24.ppd");
            break;
        case 5:
            strcpy(ppd_filename, "stcolor.ppd");
            break;
        case 6:
            strcpy(ppd_filename, "stcolor2.ppd");
            break;
        case 7:
            strcpy(ppd_filename, "stphoto.ppd");
            break;
        case 8:
            strcpy(ppd_filename, "stphoto2.ppd");
            break;
        default:
            strcpy(ppd_filename, "laserjet.ppd");
            break;
        }

        SysPrinterSetting sps;
        char options[100];
        char *grp_options;
        const int media_size = sps.GetMediaSize();
        const int copies = sps.GetCopies();
        if (media_size < MediaSizeMax && media_size >= 0 && copies > 0) {
            sprintf(options, "gamma=%d brightness=%d media=%s", sps.GetGamma(), sps.GetBrightness(), PrinterMediaSize[media_size]);
            grp_options = options;
        } else {
            grp_options = NULL;
        }

        char device[128];
        if(PeripheralMan::GetInstance()->GetPrinterDev(device)) {
            if (ppd_sign < 3)
                sprintf(print_command, "%s/hp_print.sh %s %s %s %s %d %s", PRINTDIR, PRINTDIR, device, ppd_filename, m_imgPath.c_str(), copies, grp_options);
            else
                sprintf(print_command, "%s/epson_print.sh %s %s %s %s %d %s", PRINTDIR, PRINTDIR, device, ppd_filename, m_imgPath.c_str(), copies, grp_options);
            _system_(print_command);
        } else {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
                                              ViewDialog::INFO,
                                              _("No printer found."),
                                              NULL);
        }
    } else {
        Printer prt;
        if (prt.CancelAllJobs()) {
            SysPrinterSetting sps;
            char options[100];
            char *grp_options;
            const int media_size = sps.GetMediaSize();
            const int copies = sps.GetCopies();
            const int landscape = sps.GetLandscape();
            const int pagesize = sps.GetPageSize();
            if (landscape < LandscapeMax && media_size < MediaSizeMax && media_size >= 0 && copies > 0) {
            	if(ViewPrintSetting::GetInstance()->SonyURI())
                	 sprintf(options, "gamma=%d brightness=%d media=%s scaling=100 %s PageSize=%s", sps.GetGamma(),//mirror
                			 sps.GetBrightness(), PrinterMediaSize[media_size], PrinterDirection[landscape], PrinterPageSize[pagesize]);
                else
                    sprintf(options, "gamma=%d brightness=%d media=%s", sps.GetGamma(), sps.GetBrightness(),
                    		PrinterMediaSize[media_size]);
                grp_options = options;
            } else {
                grp_options = NULL;
            }

            int vet = prt.print(NULL, m_imgPath.c_str(), grp_options, copies);
            if (vet < 0) {
                if(vet == -2) {
                    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
							ViewDialog::INFO,
							_("No printer found or specified."),
							NULL);
				}
				else {
					ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
							ViewDialog::INFO,
							_("Failed to print!"),
							NULL);
				}
			}
        }
    }
}

void ViewPrintPreview::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);

    switch(keyValue) {
    case KEY_ESC:
	BtnExitClicked(NULL);
	break;
    default:
	break;
    }
}

void ViewPrintPreview::CreateWindow()
{
    GtkWidget *fixed_window;
    GtkWidget *scrolledwin;
    GtkWidget *viewport;

    GtkWidget *image_button_print;
    GtkWidget *label_button_print;
    GtkWidget *button_print;
    GtkWidget *image_button_exit;
    GtkWidget *label_button_exit;
    GtkWidget *button_exit;
    GtkWidget *image_button_setting;
    GtkWidget *label_button_setting;
    GtkWidget *button_setting;
 	GtkWidget * fixed_image;

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, SCREEN_WIDTH, SCREEN_HEIGHT);
    gtk_window_set_title (GTK_WINDOW (m_window), _("Print Preview"));
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewReport::GetInstance()->GetWindow()));
    g_signal_connect (G_OBJECT (m_window), "delete-event", G_CALLBACK (HandleDeleteWinEvent), this);

    fixed_window = gtk_fixed_new ();
    gtk_widget_show (fixed_window);
    gtk_container_add (GTK_CONTAINER (m_window), fixed_window);

    scrolledwin = gtk_scrolled_window_new (NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixed_window), scrolledwin, 12, 0);
    gtk_widget_set_size_request (scrolledwin, SCREEN_WIDTH - 24, 668);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwin), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwin), GTK_SHADOW_IN);
    gtk_container_set_border_width (GTK_CONTAINER (scrolledwin), 5);

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);
    gtk_container_add (GTK_CONTAINER (scrolledwin), viewport);

    fixed_image = gtk_fixed_new ();
    gtk_widget_show (fixed_image);
    gtk_container_add (GTK_CONTAINER (viewport), fixed_image);

    // image area
    m_image = gtk_drawing_area_new();
    gtk_fixed_put(GTK_FIXED(fixed_image), m_image, 38, 0);
    //gtk_container_add (GTK_CONTAINER (viewport), m_image);
    gtk_drawing_area_size(GTK_DRAWING_AREA(m_image), REPORT_WIDTH, REPORT_HEIGHT);
    g_signal_connect(G_OBJECT(m_image), "configure_event", G_CALLBACK(HandlePrintPreviewAreaConfigure), this);
    g_signal_connect(G_OBJECT(m_image), "expose_event", G_CALLBACK(HandlePrintPreviewAreaExpose), this);

    image_button_setting = gtk_image_new_from_stock ("gtk-properties", GTK_ICON_SIZE_BUTTON);
    label_button_setting = gtk_label_new_with_mnemonic (_("Setting"));
    button_setting = create_button_icon (label_button_setting, image_button_setting);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_setting, 624, 671);

    image_button_print = gtk_image_new_from_stock ("gtk-print", GTK_ICON_SIZE_BUTTON);
    label_button_print = gtk_label_new_with_mnemonic (_("Print"));
    button_print = create_button_icon (label_button_print, image_button_print);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_print, 754, 671);

    image_button_exit = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    label_button_exit = gtk_label_new_with_mnemonic (_("Exit"));
    button_exit = create_button_icon (label_button_exit, image_button_exit);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_exit, 884, 671);

    g_signal_connect ((gpointer) button_print, "clicked",
             G_CALLBACK (button_print_clicked),
             this);
    g_signal_connect ((gpointer) button_exit, "clicked",
            G_CALLBACK (button_exit_clicked),
            this);
    g_signal_connect ((gpointer) button_setting, "clicked",
            G_CALLBACK (button_setting_clicked),
            this);

    g_keyInterface.Push(this);
    gtk_widget_show_all(m_window);

    return;
}

void ViewPrintPreview::AutoRadioToggled(GtkToggleButton *togglebutton)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_auto)))
    {
        SysGeneralSetting sysGeneralSetting;
        sysGeneralSetting.SetPrintViewMode(0);
        sysGeneralSetting.SyncFile();

        gtk_widget_hide (m_scrolledwindow_orignial);
        gtk_widget_show (m_scrolledwindow_auto);
    }

}

void ViewPrintPreview::OrignialRadioToggled(GtkToggleButton *togglebutton)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_orignial)))
    {
        SysGeneralSetting sysGeneralSetting;
        sysGeneralSetting.SetPrintViewMode(1);
        sysGeneralSetting.SyncFile();

        gtk_widget_hide (m_scrolledwindow_auto);
        gtk_widget_show (m_scrolledwindow_orignial);
    }
}

GtkWidget* ViewPrintPreview::CreateImageWindow(GtkWidget *fix, int viewmode)
{
    GtkWidget *scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_fixed_put (GTK_FIXED (fix), scrolledwindow, 20, 20);
    gtk_widget_set_size_request (scrolledwindow, SCREEN_WIDTH-40, 650);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
   gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);
    gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 5);

    GtkWidget *viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    GtkWidget *fixed_image = gtk_fixed_new ();
    gtk_widget_show (fixed_image);
    gtk_container_add (GTK_CONTAINER (viewport), fixed_image);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(m_imgPath.c_str(), NULL);

    if(viewmode)
    {
        GtkWidget * print_image = gtk_image_new_from_pixbuf(pixbuf);
        gtk_fixed_put(GTK_FIXED(fixed_image), print_image, (970-gdk_pixbuf_get_width(pixbuf))/2, 2);
    }
    else
    {
        GdkPixbuf *rotate_pixbuf = gdk_pixbuf_rotate_simple(pixbuf, GDK_PIXBUF_ROTATE_CLOCKWISE);
        int width = gdk_pixbuf_get_width(rotate_pixbuf);
        int height = gdk_pixbuf_get_height(rotate_pixbuf);
        int w = 630*width/height ;
        GdkPixbuf *scale_pixbuf = gdk_pixbuf_scale_simple(rotate_pixbuf, w, 630, GDK_INTERP_BILINEAR);
        GtkWidget *print_image = gtk_image_new_from_pixbuf(scale_pixbuf);
        gtk_fixed_put(GTK_FIXED(fixed_image), print_image, (970-w)/2, 2);
        g_object_unref(rotate_pixbuf);
        g_object_unref(scale_pixbuf);
    }
    g_object_unref(pixbuf);

    gtk_widget_hide(scrolledwindow);
    return scrolledwindow;
}

void ViewPrintPreview::CreateWindow(const char* path, GtkWidget *parent)
{
    GtkWidget *fixed_window;
    GtkWidget *image_button_print;
    GtkWidget *label_button_print;
    GtkWidget *button_print;
    GtkWidget *image_button_exit;
    GtkWidget *label_button_exit;
    GtkWidget *button_exit;
    GtkWidget *image_button_setting;
    GtkWidget *label_button_setting;
    GtkWidget *button_setting;

	m_imgPath = path;

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, SCREEN_WIDTH, SCREEN_HEIGHT);
    gtk_window_set_title (GTK_WINDOW (m_window), _("Print Preview"));
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    //gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewReport::GetInstance()->GetWindow()));
    gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(parent));
    gtk_window_set_decorated (GTK_WINDOW (m_window), FALSE);
    g_signal_connect (G_OBJECT (m_window), "delete-event", G_CALLBACK (HandleDeleteEvent), this);

    fixed_window = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (m_window), fixed_window);

    //View Mode
    GtkWidget* frame_print = gtk_frame_new (NULL);
    gtk_widget_show (frame_print);
    gtk_fixed_put (GTK_FIXED (fixed_window), frame_print, 304-60, 690);
    gtk_widget_set_size_request (frame_print, 290+60, 50);
    gtk_frame_set_label_align (GTK_FRAME (frame_print), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_print), GTK_SHADOW_IN);

    GtkWidget* label_print = gtk_label_new (_("View Mode"));
    gtk_widget_show (label_print);
    gtk_frame_set_label(GTK_FRAME (frame_print), _("View Mode"));
    //gtk_frame_set_label(GTK_FRAME (frame_print), label_print);
    gtk_label_set_use_markup (GTK_LABEL (label_print), TRUE);

    GtkWidget* hbox_print = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_print);
    gtk_container_add (GTK_CONTAINER (frame_print), hbox_print);

    GSList *radiobutton_printview_group = NULL;

    m_radiobutton_auto = gtk_radio_button_new_with_mnemonic (NULL, _("Auto Size"));
    gtk_widget_show (m_radiobutton_auto);
    gtk_box_pack_start (GTK_BOX (hbox_print), m_radiobutton_auto, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_auto), radiobutton_printview_group);
    radiobutton_printview_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_auto));
    g_signal_connect((gpointer)m_radiobutton_auto, "toggled", G_CALLBACK (on_auto_radio_button_toggled), this);

    m_radiobutton_orignial= gtk_radio_button_new_with_mnemonic (NULL, _("Orignial Size"));
    gtk_widget_show (m_radiobutton_orignial);
    gtk_box_pack_start (GTK_BOX (hbox_print), m_radiobutton_orignial, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_orignial), radiobutton_printview_group);
    radiobutton_printview_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_orignial));
    g_signal_connect((gpointer)m_radiobutton_orignial, "toggled", G_CALLBACK (on_orignial_radio_button_toggled), this);

    SysGeneralSetting sysGeneralSetting;
    if(sysGeneralSetting.GetPrintViewMode())
    {
        m_scrolledwindow_auto = CreateImageWindow(fixed_window, 0);
        m_scrolledwindow_orignial = CreateImageWindow(fixed_window, 1);

        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_orignial), TRUE);
        gtk_widget_show (m_scrolledwindow_orignial);
    }
    else
    {
        m_scrolledwindow_orignial = CreateImageWindow(fixed_window, 1);
        m_scrolledwindow_auto = CreateImageWindow(fixed_window, 0);

        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_auto), TRUE);
        gtk_widget_show (m_scrolledwindow_auto);
    }

    image_button_setting = gtk_image_new_from_stock ("gtk-properties", GTK_ICON_SIZE_BUTTON);
    label_button_setting = gtk_label_new_with_mnemonic (_("Setting"));
    button_setting = create_button_icon (label_button_setting, image_button_setting);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_setting, 624, 688);

    image_button_print = gtk_image_new_from_stock ("gtk-print", GTK_ICON_SIZE_BUTTON);
    label_button_print = gtk_label_new_with_mnemonic (_("Print"));
    button_print = create_button_icon (label_button_print, image_button_print);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_print, 754, 688);

    image_button_exit = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    label_button_exit = gtk_label_new_with_mnemonic (_("Exit"));
    button_exit = create_button_icon (label_button_exit, image_button_exit);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_exit, 884, 688);

    g_signal_connect ((gpointer) button_print, "clicked",
		      G_CALLBACK (on_button_print_clicked),
		      this);
    g_signal_connect ((gpointer) button_exit, "clicked",
		      G_CALLBACK (on_button_exit_clicked),
		      this);
    g_signal_connect ((gpointer) button_setting, "clicked",
		      G_CALLBACK (on_button_setting_clicked),
		      this);

    g_keyInterface.Push(this);
    gtk_widget_show_all(m_window);
}

GtkWidget* ViewPrintPreview::CreateReportWindow(GtkWidget *fix, int viewmode)
{
    GtkWidget* scrolledwin = gtk_scrolled_window_new (NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fix), scrolledwin, 12, 0);
    gtk_widget_set_size_request (scrolledwin, SCREEN_WIDTH - 24, 668);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwin), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwin), GTK_SHADOW_IN);
    gtk_container_set_border_width (GTK_CONTAINER (scrolledwin), 5);

    GtkWidget*viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);
    gtk_container_add (GTK_CONTAINER (scrolledwin), viewport);

    GtkWidget*fixed_image = gtk_fixed_new ();
    gtk_widget_show (fixed_image);
    gtk_container_add (GTK_CONTAINER (viewport), fixed_image);

    if(viewmode)
    {
        int grap_y = 20;
        for(int i = 0; i < report_page_count; i++)
        {
	        char *pathname = GetReportPageName(i);
            GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(pathname, NULL);
            GtkWidget *print_image = gtk_image_new_from_pixbuf(pixbuf);
            gtk_fixed_put(GTK_FIXED(fixed_image), print_image, 38, (REPORT_HEIGHT + grap_y)*i);
            g_object_unref(pixbuf);
            free(pathname);
        }
    }
    else
    {
        int grap_y = 20;
        for(int i = 0; i < report_page_count; i++)
        {
        	char *pathname = GetReportPageName(i);
            int width = 650*REPORT_WIDTH/REPORT_HEIGHT;
            int height = 650;
            GError *error = NULL;
            GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(pathname, &error);
            if(pixbuf && !error)
            {
                GdkPixbuf *scale_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
                GtkWidget *print_image = gtk_image_new_from_pixbuf(scale_pixbuf);
                gtk_fixed_put(GTK_FIXED(fixed_image), print_image, (990-width)/2, (height + grap_y)*i);
                g_object_unref(scale_pixbuf);
                g_object_unref(pixbuf);
            }
            free(pathname);
        }
    }

    gtk_widget_hide(scrolledwin);
    return scrolledwin;

}

void ViewPrintPreview::CreateWindow2(void)
{
    GtkWidget *fixed_window;
    GtkWidget *image_button_print;
    GtkWidget *label_button_print;
    GtkWidget *button_print;
    GtkWidget *image_button_exit;
    GtkWidget *label_button_exit;
    GtkWidget *button_exit;
    GtkWidget *image_button_setting;
    GtkWidget *label_button_setting;
    GtkWidget *button_setting;

    CreatePages((char *)"./res/reportpage");

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, SCREEN_WIDTH, SCREEN_HEIGHT);
    gtk_window_set_title (GTK_WINDOW (m_window), _("Print Preview"));
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewReport::GetInstance()->GetWindow()));
    g_signal_connect (G_OBJECT (m_window), "delete-event", G_CALLBACK (HandleDeleteWinEvent), this);

    fixed_window = gtk_fixed_new ();
    gtk_widget_show (fixed_window);
    gtk_container_add (GTK_CONTAINER (m_window), fixed_window);

    //View Mode
    GtkWidget* frame_print = gtk_frame_new (NULL);
    gtk_widget_show (frame_print);
    gtk_fixed_put (GTK_FIXED (fixed_window), frame_print, 304-50, 675);
    gtk_widget_set_size_request (frame_print, 270+50, 50);
    gtk_frame_set_label_align (GTK_FRAME (frame_print), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_print), GTK_SHADOW_IN);

    GtkWidget* label_print = gtk_label_new (_("View Mode"));
    gtk_widget_show (label_print);
    gtk_frame_set_label(GTK_FRAME (frame_print), _("View Mode"));
    gtk_label_set_use_markup (GTK_LABEL (label_print), TRUE);

    GtkWidget* hbox_print = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_print);
    gtk_container_add (GTK_CONTAINER (frame_print), hbox_print);
    GSList *radiobutton_printview_group = NULL;
    m_radiobutton_auto = gtk_radio_button_new_with_mnemonic (NULL, _("Auto Size"));
    gtk_widget_show (m_radiobutton_auto);
    gtk_box_pack_start (GTK_BOX (hbox_print), m_radiobutton_auto, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_auto), radiobutton_printview_group);
    radiobutton_printview_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_auto));
    g_signal_connect((gpointer)m_radiobutton_auto, "toggled", G_CALLBACK (on_auto_radio_button_toggled), this);

    m_radiobutton_orignial= gtk_radio_button_new_with_mnemonic (NULL, _("Orignial Size"));
    gtk_widget_show (m_radiobutton_orignial);
    gtk_box_pack_start (GTK_BOX (hbox_print), m_radiobutton_orignial, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_orignial), radiobutton_printview_group);
    radiobutton_printview_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_orignial));
    g_signal_connect((gpointer)m_radiobutton_orignial, "toggled", G_CALLBACK (on_orignial_radio_button_toggled), this);

    SysGeneralSetting sysGeneralSetting;
    if(sysGeneralSetting.GetPrintViewMode())
    {
        m_scrolledwindow_auto = CreateReportWindow(fixed_window, 0);
        m_scrolledwindow_orignial = CreateReportWindow(fixed_window, 1);

        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_orignial), TRUE);
        gtk_widget_show (m_scrolledwindow_orignial);
    }
    else
    {
        m_scrolledwindow_orignial = CreateReportWindow(fixed_window, 1);
        m_scrolledwindow_auto = CreateReportWindow(fixed_window, 0);

        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_auto), TRUE);
        gtk_widget_show (m_scrolledwindow_auto);
    }

    image_button_setting = gtk_image_new_from_stock ("gtk-properties", GTK_ICON_SIZE_BUTTON);
    label_button_setting = gtk_label_new_with_mnemonic (_("Setting"));
    button_setting = create_button_icon (label_button_setting, image_button_setting);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_setting, 624, 671);

    image_button_print = gtk_image_new_from_stock ("gtk-print", GTK_ICON_SIZE_BUTTON);
    label_button_print = gtk_label_new_with_mnemonic (_("Print"));
    button_print = create_button_icon (label_button_print, image_button_print);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_print, 754, 671);

    image_button_exit = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    label_button_exit = gtk_label_new_with_mnemonic (_("Exit"));
    button_exit = create_button_icon (label_button_exit, image_button_exit);
    gtk_fixed_put (GTK_FIXED (fixed_window), button_exit, 884, 671);

    g_signal_connect ((gpointer) button_print, "clicked",
             G_CALLBACK (HandleClickPrintButton),
             this);
    g_signal_connect ((gpointer) button_exit, "clicked",
            G_CALLBACK (button_exit_clicked),
            this);
    g_signal_connect ((gpointer) button_setting, "clicked",
            G_CALLBACK (button_setting_clicked),
            this);

    g_keyInterface.Push(this);
    gtk_widget_show_all(m_window);
    return;
}

void ViewPrintPreview::BtnExitClicked(GtkButton *button)
{
    g_keyInterface.Pop();
    gtk_widget_destroy(m_window);
}

#include "periDevice/Printfunctions.h"
void ViewPrintPreview::BtnPrintClicked(GtkButton *button)
{
	if(MODE_PRINT_NEW) //new print some error 20130424, jhuang
    {
       // char device[128];
      //  if(PeripheralMan::GetInstance()->GetPrinterDev(device))
      //  {
            const char *path = m_imgPath.c_str();
            Printmain::GetInstance()->GetPrintOptions();
            Printfunctions::GetInstance()->PrintFunc(path);
       // }
      //  else
      //  {
       //     ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("No printer found."), NULL);
      //  }
	}
	else
		Printing();
}

void ViewPrintPreview::QuickPrintScreen(const char *path)
{
	if(MODE_PRINT_NEW) //new print some error 20130424, jhuang
    {
        Printmain::GetInstance()->GetPrintOptions();
        Printfunctions::GetInstance()->PrintFunc(path);
	}
	else
		Printing();
}

void ViewPrintPreview::BtnSettingClicked(GtkButton *button)
{
    if(MODE_PRINT_NEW) //new setting some error 20130424, jhuang
		Printmain::GetInstance()->ViewPrintWindowCreat(GTK_WINDOW(m_window));
	else
		ViewPrintSetting::GetInstance()->CreateWindow(GTK_WINDOW(m_window));
}

GtkWidget *ViewPrintPreview::GetMainWindow(void)
{
	return m_window;
}

GtkWidget *GetMainWindow(void)
{
	return ViewPrintPreview::GetInstance()->GetMainWindow();
}

//lhm
void ViewPrintPreview::BtnPrintWinClicked(GtkButton *button)
{
    if (SaveReport_To_Print(PRINTIMAGE) == 0)
    {
        char device[128];
        if(PeripheralMan::GetInstance()->GetPrinterDev(device))
        {
            m_imgPath = PRINTIMAGE;
            if(MODE_PRINT_NEW)
            {
                const char *path = m_imgPath.c_str();
                Printmain::GetInstance()->GetPrintOptions();
                Printfunctions::GetInstance()->PrintFunc(path);
            }
            else
                Printing();
        }
        else
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("No printer found."), NULL);
        }
    }
    else
    {
        PRINTF("save file is error\n");
    }
}

void ViewPrintPreview::BtnExitWinClicked(GtkButton *button)
{
    g_keyInterface.Pop();
    gtk_widget_destroy(m_window);
}

void ViewPrintPreview::BtnSettingWinClicked(GtkButton *button)
{
    if(MODE_PRINT_NEW) //new setting some error 20130424, jhuang
        Printmain::GetInstance()->ViewPrintWindowCreat(GTK_WINDOW(m_window));
    else
         ViewPrintSetting::GetInstance()->CreateWindow(GTK_WINDOW(m_window));
}

void ViewPrintPreview::PrintAreaConfigure(GtkWidget *widget, GdkEventConfigure *event)
{
   if (m_pixmapPrintImage)
       g_object_unref(m_pixmapPrintImage);
   m_pixmapPrintImage = gdk_pixmap_new(widget->window, REPORT_WIDTH, REPORT_HEIGHT, -1);

   GdkGC *gc_report = gdk_gc_new(m_pixmapPrintImage);
   gdk_gc_set_foreground(gc_report, g_white);
   gdk_gc_set_function(gc_report, GDK_COPY);

   gdk_draw_rectangle(m_pixmapPrintImage, gc_report, TRUE, 0, 0, REPORT_WIDTH, REPORT_HEIGHT);
   g_object_unref(gc_report);
#ifdef VET
#else
   Item_To_Get();
#endif
}

void ViewPrintPreview::PrintAreaExpose(GtkWidget *widget, GdkEventExpose *event)
{
      gdk_draw_drawable(widget->window,
                     widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
            m_pixmapPrintImage,
            0, 0,
            0, 0,
            REPORT_WIDTH, REPORT_HEIGHT);
}
#ifdef VET
#else
void ViewPrintPreview::Item_To_Get()
{
   struct print_re_item item;
   memset(&item, 0x0, sizeof(item));

	char id[50], name[60], check_num[50], sex[50], age[50], section[50], check_date[50], check_doctor[50], report_doctor[50], ul_idea[2000], comment[2000];
	char tmp[1000];

	PatientInfo::Info report_info;
	g_patientInfo.GetInfo(report_info);
	strcpy(id, report_info.p.id.c_str());
	sprintf(name, "%s %s %s", report_info.p.name.last.c_str(), report_info.p.name.first.c_str(), report_info.p.name.mid.c_str());
	strcpy(check_num, report_info.e.examNum.c_str());
#ifdef VET
	if (report_info.p.sex == 0)
	    strcpy(sex, _("Female "));
	else if (report_info.p.sex == 1)
	    strcpy(sex, _("Male "));
	else
	    strcpy(sex, _("Other"));
#else
	if (report_info.p.sex == 0)
	    strcpy(sex, _("Female"));
	else if (report_info.p.sex == 1)
	    strcpy(sex, _("Male"));
	else
	    strcpy(sex, _("Other"));
#endif

    sprintf(age, "%d", report_info.p.age);

    string part;
	TopArea::GetInstance()->GetCheckPart(part);
	strcpy(section, part.c_str());
	char dayTmp[4], monthTmp[4], yearTmp[6];
	itoa(report_info.e.examDate.day, dayTmp, 10);
	if (strlen(dayTmp)==1){dayTmp[2]='\0', dayTmp[1] = dayTmp[0]; dayTmp[0] = '0';}
	itoa(report_info.e.examDate.month, monthTmp, 10);
	if (strlen(monthTmp)==1){monthTmp[2]='\0', monthTmp[1] = monthTmp[0]; monthTmp[0] = '0';}
	itoa(report_info.e.examDate.year, yearTmp, 10);

	SysGeneralSetting sys;
	int date_format = sys.GetDateFormat();
	if (date_format == 1)
		sprintf(check_date, "%s-%s-%s", monthTmp, dayTmp, yearTmp);
	else if (date_format == 2)
		sprintf(check_date, "%s-%s-%s", dayTmp, monthTmp, yearTmp);
	else
		sprintf(check_date, "%s-%s-%s", yearTmp, monthTmp, dayTmp);

	string hospital;
	SysGeneralSetting get_hospital;
	get_hospital.GetHospital(hospital);
    sprintf(tmp, "%s", hospital.c_str());

	strcpy(check_doctor, report_info.e.examDoctor.c_str());
	strcpy(report_doctor, report_info.e.reportDoctor.c_str());

	//snprintf(ul_idea, sizeof(ul_idea), ViewReport::GetInstance()->GetIndication()); //save warning: format not a string literal and no format arguments
	sprintf(ul_idea, "%s", ViewReport::GetInstance()->GetIndication());
	sprintf(comment, "%s", ViewReport::GetInstance()->GetComments());
	//snprintf(comment, sizeof(comment), ViewReport::GetInstance()->GetComments());

	item.id = id;
	item.name = name;
	item.check_num = check_num;
	item.sex = sex;
	item.age = age;
	item.section = section;
	item.diag_date = check_date;
	//item.hospital = (char*)(hospital.c_str());
    item.hospital = tmp;
    item.diag_doctor = check_doctor;
    item.report_doctor = report_doctor;
    item.ul_idea = ul_idea;
    item.comment = comment;
	for (int i=0; i<2; i++) item.image_data[i] = NULL;

	vector<string> imageName;
	imageName = ViewReport::GetInstance()->GetSelectImage();
	ImgMan::ImgItem printImg[2];
	float imgScale = 0.7;
	int lenOfLine = 0;
	int imageNums = imageName.size();
	if (imageNums > 2)
		imageNums = 2;	//现在最多能传入2幅图片
	GdkPixbuf *buf[2];
	int i, j;
	for (i=0; i<2; i++) item.image_data[i] = NULL;

	for (i=0; i<imageNums; i++)//现在最多能传入2幅图片
	{
		if(ImgMan::GetInstance()->ReadSnap(imageName[i].c_str(), &printImg[i]) == 0)
		{
			buf[i] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, IMG_W*imgScale, IMG_H*imgScale);
			//buf[i] = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width_w, height_w);
			//gdk_pixbuf_scale(printImg[i].pixbuf, buf[i], 0, 0, width_w, height_w, (-80)*imgScale, (-120)*imgScale, imgScale, imgScale, GDK_INTERP_HYPER);
			gdk_pixbuf_scale(printImg[i].pixbuf, buf[i], 0, 0, IMG_W*imgScale, IMG_H*imgScale, (-80)*imgScale, (-120)*imgScale, imgScale, imgScale, GDK_INTERP_HYPER);
			lenOfLine = gdk_pixbuf_get_rowstride(buf[i]);
            item.image_data[i] = (char*)gdk_pixbuf_get_pixels(buf[i]);
            g_object_unref(printImg[i].pixbuf);
        }
		else
		{
			if(i>0)
			{
				for(j=0; j<i; j++)
					g_object_unref(buf[j]);
			}
			PRINTF("%s: ReadSnap %d Error!\n", __FUNCTION__, i);
			return ;
		}
	}

    Draw_To_Report(item, lenOfLine, imageNums);
}
#endif
void ViewPrintPreview::Draw_To_Report(struct print_re_item Item, int lenOfline, int numOfimage)
{
    char ulTemp[2000];
    memset(ulTemp, 0x0, 2000);
    char *report_head = _("REPORT OF ULTRASOUND");
 	char *hospital = _("Hospital: ");
    char *check_date = _("Exam Date:  ");
    char *id = _("ID: ");
    char *check_num = _("Exam ID:  ");
    char *name = _("Name: ");
    char *sex = _("Gender: ");
    char *age = _("Age: ");
    char *section = _("Exam Type: ");
    char *ul_idea = _("Descriptor: ");
    char *comment = _("Comments: ");
    char *check_doctor = _("Diagnostician: ");
    char *report_doctor = _("Physician: ");

    int width;
    int height;
    int offsetY = 15;
    float scale = 0.7;
    int width_w = IMG_W * scale;
    int height_w = IMG_H * scale;
    GdkPixbuf *selectImg[2];
    int w(0), h(0);

#if 0
    ///test
    int size = (int) IMG_W*IMG_H*3*scale*scale;
    char temp[size];
    memset(temp, 126, sizeof(temp));
    Item.image_data[0] = temp;
#endif
    for ( int i = 0; i < numOfimage; i++)
    {
        if (Item.image_data[i] != NULL)
            selectImg[i] = gdk_pixbuf_new_from_data((unsigned char*)Item.image_data[i],
                    GDK_COLORSPACE_RGB,
                    FALSE, 8,
                    width_w, height_w,
                    lenOfline,
                    NULL, NULL);
    }

    PangoLayout *layout;
    GdkGC *gc_report = gdk_gc_new(m_pixmapPrintImage);
    gdk_gc_set_foreground(gc_report, g_black);
    gdk_gc_set_function(gc_report, GDK_COPY);
    layout = gtk_widget_create_pango_layout(m_image, report_head);
    pango_layout_set_font_description(layout, m_headFont);
    gdk_draw_layout(m_pixmapPrintImage, gc_report, 250, 30, layout);

    //hospital name
    sprintf (ulTemp, "%s%s",hospital, Item.hospital);
    pango_layout_set_text(layout, ulTemp ,-1);
    pango_layout_set_font_description(layout, m_baseFont);
    pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
    gdk_draw_layout(m_pixmapPrintImage, gc_report, 0, 120, layout);

    //check date
    sprintf (ulTemp, "%s%s",check_date, Item.diag_date);
    pango_layout_set_text(layout, ulTemp ,-1);
    //gdk_draw_layout(m_pixmapPrintImage, gc_report, 600, 120, layout);
    gdk_draw_layout(m_pixmapPrintImage, gc_report, 560, 120, layout);

    //draw line
    pango_layout_get_pixel_size(layout, &width, &height);

    gdk_draw_line(m_pixmapPrintImage, gc_report,0,120+height+offsetY-5, REPORT_WIDTH, 120+height+offsetY-5);

    //id
    sprintf (ulTemp, "%s%s",id, Item.id);
    pango_layout_set_text(layout, ulTemp, -1);
    gdk_draw_layout(m_pixmapPrintImage, gc_report, 0, 160, layout);

    //name
    //
    sprintf (ulTemp, "%s%s",name, Item.name);
    pango_layout_set_text(layout, ulTemp, -1);
    gdk_draw_layout(m_pixmapPrintImage, gc_report, 280, 160, layout);

    //sex
    //
    sprintf (ulTemp, "%s%s", sex, Item.sex);
    pango_layout_set_text(layout, ulTemp, -1);
    gdk_draw_layout(m_pixmapPrintImage, gc_report, 0, 195, layout);

    //age
    sprintf (ulTemp, "%s%s%s", age, Item.age,Item.ageUnit);
    pango_layout_set_text(layout, ulTemp, -1);
    gdk_draw_layout(m_pixmapPrintImage, gc_report, 180-40, 195, layout);
    //gdk_draw_layout(m_pixmapPrintImage, gc_report, 200, 195, layout);

    //section
    sprintf (ulTemp, "%s%s",section, Item.section);
    pango_layout_set_text(layout, ulTemp, -1);
    gdk_draw_layout(m_pixmapPrintImage, gc_report, 340-40, 195, layout);
    //gdk_draw_layout(m_pixmapPrintImage, gc_report, 400, 195, layout);

    //check num
    sprintf (ulTemp, "%s%s",check_num, Item.check_num);
    pango_layout_set_text(layout, ulTemp, -1);
    gdk_draw_layout(m_pixmapPrintImage, gc_report, 660+20, 195, layout);

    //draw line
    gdk_draw_line(m_pixmapPrintImage, gc_report,0,195+height+offsetY, REPORT_WIDTH, 195+height+offsetY);

    //insert picture
    switch (numOfimage)
    {
        case 2:
            gdk_draw_pixbuf(m_pixmapPrintImage,
                    gc_report,
                    selectImg[0],
                    0, 0, 0, 195+height+2*offsetY,
                    -1,-1,
                    GDK_RGB_DITHER_NORMAL, 0, 0);

            gdk_draw_pixbuf(m_pixmapPrintImage,
                    gc_report,
                    selectImg[1],
                    0, 0, 450, 195+height+2*offsetY,
                    -1,-1,
                    GDK_RGB_DITHER_NORMAL, 0, 0);
            break;

        case 1:
            gdk_draw_pixbuf(m_pixmapPrintImage,
                    gc_report,
                    selectImg[0],
                    0, 0, 230,195+height+2*offsetY,
                    -1,-1,
                    GDK_RGB_DITHER_NORMAL, 0, 0);
            break;

        default:
            break;
    }

   //cal result
    int num = ViewReport::GetInstance()->GetCountFromCalcArr();
    int j = 0;
    int positionX = 40;
    char font_desc[100];
    char gw[256];
    char edcb[256];
    memset (gw, '\0',256);
    memset (edcb, '\0', 256);
    CalcNode_t calcnode;
    sprintf(font_desc, "%s, 13", FONT_STRING);
    PangoFontDescription *font = pango_font_description_from_string(font_desc);
    pango_layout_set_font_description(layout,font);
    for (int k = 0; k < num; k++)
    {
        ViewReport::GetInstance()->GetItemFromCalcArr(k, &calcnode);
        strcpy(gw, calcnode.gw);
        strcpy(edcb, calcnode.edcb);
        if ((strlen(gw)) && (strlen(edcb)))
            sprintf(ulTemp, "%s  %s  GW  %s  EDD  %s;", calcnode.title, calcnode.value, calcnode.gw, calcnode.edcb);
        else if ((strlen(gw)) && (!strlen(edcb)))
            sprintf(ulTemp, "%s  %s  GW  %s;", calcnode.title, calcnode.value, calcnode.gw);
        else if ((!strlen(gw)) && (strlen(edcb)))
            sprintf(ulTemp, "%s  %s  EDD  %s;", calcnode.title, calcnode.value, calcnode.edcb);
        else sprintf(ulTemp, "%s  %s;",calcnode.title, calcnode.value);

        pango_layout_set_text(layout, (char *)ulTemp, -1);
        pango_layout_get_pixel_size(layout, &w, &h);
        if ((positionX+w) < REPORT_WIDTH-5)
        {
            gdk_draw_layout(m_pixmapPrintImage, gc_report, positionX, 574+j*10+j*h, layout);
        }
        else
        {
            j++;
            positionX = 40;
            gdk_draw_layout(m_pixmapPrintImage, gc_report, positionX, 574+j*10+j*h, layout);
        }
        positionX += w+25;
    }
    j++;
    pango_font_description_free(font);
    int autochagePosY = 574 + j*10 + j*h + offsetY;
    //description
    if (ViewSystem::GetInstance()->CheckFlagFromReportTemplet(OTHERS_INDICTION_SHOW_ID))
    {
        sprintf (ulTemp, "%s", ul_idea);
        pango_layout_set_font_description(layout, m_baseFont);
        pango_layout_set_text(layout, (char *)ulTemp, -1);
        gdk_draw_layout(m_pixmapPrintImage, gc_report, 0, autochagePosY, layout);

        sprintf (ulTemp, "%s", Item.ul_idea);
        pango_layout_set_width(layout, (REPORT_WIDTH-40*2)*PANGO_SCALE);
        pango_layout_set_justify(layout, TRUE);
        pango_layout_set_text(layout, (char *)ulTemp, -1);
        pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
        pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
        pango_layout_get_pixel_size(layout, &width, &height);
        gdk_draw_layout(m_pixmapPrintImage, gc_report, 40, autochagePosY + offsetY + 10 , layout);
    }

    //comment
    if (ViewSystem::GetInstance()->CheckFlagFromReportTemplet(OTHERS_COMMENT_SHOW_ID))
    {
        sprintf (ulTemp, "%s",comment);
        pango_layout_set_text(layout, ulTemp, -1);
        gdk_draw_layout(m_pixmapPrintImage, gc_report, 0, autochagePosY + height + offsetY + 10, layout);
        //gdk_draw_layout(m_pixmapPrintImage, gc_report, 0, 1025, layout);

        sprintf (ulTemp, "%s",Item.comment);
        pango_layout_set_text(layout, ulTemp, -1);
        pango_layout_get_pixel_size(layout, &w, &h);
        int tempY = autochagePosY + height + h + 2*offsetY ;
        PRINTF("-----tempY %d\n", tempY);
        if (tempY > 1185)
        {
            pango_layout_set_font_description(layout, m_commentTmpFont);
            gdk_draw_layout(m_pixmapPrintImage, gc_report, 40, autochagePosY + height + 2*offsetY + 20, layout);
        }
        else
        {
            gdk_draw_layout(m_pixmapPrintImage, gc_report, 40, autochagePosY + height + 2*offsetY + 20, layout);
            //pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_NONE);
        }
    }
    pango_layout_set_font_description(layout, m_baseFont);
    gdk_draw_line(m_pixmapPrintImage, gc_report,0,1200-offsetY, REPORT_WIDTH, 1200-offsetY);

    //check doctor
    sprintf (ulTemp, "%s%s",check_doctor, Item.diag_doctor);
    pango_layout_set_text(layout,ulTemp ,-1);
    gdk_draw_layout(m_pixmapPrintImage, gc_report, 0, 1200, layout);

    //report doctor
    sprintf (ulTemp, "%s%s",report_doctor, Item.report_doctor);
    pango_layout_set_text(layout,ulTemp ,-1);
    gdk_draw_layout(m_pixmapPrintImage, gc_report, 380, 1200, layout);

    g_object_unref(layout);
    g_object_unref(gc_report);
    for (int j = 0; j < numOfimage; j++)
      g_object_unref(selectImg[j]);
 }

int ViewPrintPreview::SaveReport_To_Print(const char *path)
{
    GError *error = NULL;
    int fd = open(path, O_RDWR|O_CREAT|O_TRUNC, 00777);
    PRINTF("file fd No %d\n", fd);

    if (fd < 0)
    {
        return (-1);
    }
    GdkPixbuf *pixbuf = gdk_pixbuf_get_from_drawable(NULL,
            m_pixmapPrintImage,
            gdk_colormap_get_system(),
            0, 0,
            0, 0,
            REPORT_WIDTH, REPORT_HEIGHT);
    if (!gdk_pixbuf_save (pixbuf, path, "bmp", &error, NULL))
    {
        PRINTF("%s\n", error->message);
        g_object_unref(pixbuf);
        g_error_free(error);
    }
    m_imgPath = path;
    close(fd);
    return (0);
}

char *ViewPrintPreview::GetReportPageName(int page)
{
	char *pathname = (char *)malloc(512);
	sprintf(pathname, "%s/report_page%d.png", m_path, page+1);
    PRINTF("----report pathname=%s\n", pathname);
	return pathname;
}

int ViewPrintPreview::SavePageByPixmap(const char *path)
{
    int ret = -1;
    GError *error = NULL;
    int fd = open(path, O_RDWR|O_CREAT|O_TRUNC, 00777);
    if (fd < 0)
    {
        return ret;
    }

    GdkPixbuf *pixbuf = gdk_pixbuf_get_from_drawable(NULL, pagepixmap, gdk_colormap_get_system(), 0, 0, 0, 0,  REPORT_WIDTH, REPORT_HEIGHT);
    if (!gdk_pixbuf_save (pixbuf, path, "png", &error, NULL))
    {
        g_error_free(error);
    }

    if (pixbuf != NULL)
        g_object_unref(pixbuf);

    close(fd);
    return (ret = 0);
}

void ViewPrintPreview::AddTextToPageByPixmap(int &x, int &y, char *text, PangoFontDescription *font)
{
    PRINTF("AddTextToPageByPixmap(enter00000:text:%s, x = %d, y = %d)\n",text,  x, y);
    int w, h; ////REPORT_WIDTH, REPORT_HEIGHT
    if(text == NULL)
    {
		char *pathname = GetReportPageName(report_page_count);
        if(0 == SavePageByPixmap(pathname))
        {
            report_page_count++;
        }
		free(pathname);

        if (pagegc != NULL)
        {
            g_object_unref(pagegc);
            pagegc = NULL;
        }

        if (pagelayout != NULL)
        {
            g_object_unref(pagelayout);
            pagelayout = NULL;
        }

        if (pagepixmap!= NULL)
        {
            g_object_unref(pagepixmap);
            pagepixmap= NULL;
        }
        return;
    }

    if (!pagepixmap)
    {
        pagepixmap = gdk_pixmap_new(NULL, REPORT_WIDTH, REPORT_HEIGHT, 24);
    }

    if(!pagegc)
    {
        pagegc = gdk_gc_new(pagepixmap);
        gdk_gc_set_foreground(pagegc, g_white);
        gdk_gc_set_function(pagegc, GDK_COPY);
        gdk_draw_rectangle(pagepixmap, pagegc, TRUE, 0, 0, REPORT_WIDTH, REPORT_HEIGHT);
        gdk_gc_set_foreground(pagegc, g_black);
        gdk_gc_set_function(pagegc, GDK_COPY);
    }

    if(NULL == pagelayout)
    {
        drawingarea = gtk_drawing_area_new();
        pagelayout = gtk_widget_create_pango_layout(drawingarea, text);
    }

    pango_layout_set_width(pagelayout, (REPORT_WIDTH-40*2)*PANGO_SCALE);
    pango_layout_set_justify(pagelayout, TRUE);
    pango_layout_set_wrap(pagelayout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_alignment(pagelayout, PANGO_ALIGN_LEFT);
    pango_layout_set_font_description(pagelayout, font);
    pango_layout_set_text(pagelayout, text, -1);
    pango_layout_get_pixel_size(pagelayout, &w, &h);

    PRINTF("AddTextToPageByPixmap(enter:text:%s,  x = %d, y = %d)\n",text,  x, y);
    char *check_doctor = _("Diagnostician: ");
    char *report_doctor = _("Physician: ");
    if(((y + h) > 1190) && (NULL == strstr(text, check_doctor)) && (NULL == strstr(text, report_doctor)))
    {
        //int maxpage = MAX_PAGE;
        char *pathname = GetReportPageName(report_page_count);
        //if(report_page_count >= MAX_PAGE)
          //  return;

        if(0 != SavePageByPixmap(pathname))
        {
            PRINTF("AddTextToPageByPixmap(SavePageByPixmap: ERROR)\n");
            if (pathname != NULL)
                free(pathname);
            return;
        }
		free(pathname);
        g_object_unref(pagegc);
        g_object_unref(pagelayout);
        g_object_unref(pagepixmap);
        pagegc = NULL;
        pagelayout = NULL;
        pagepixmap = NULL;

        report_page_count++;
      //  if(report_page_count < maxpage)
        //{
            pagepixmap = gdk_pixmap_new(NULL, REPORT_WIDTH, REPORT_HEIGHT, 24);
            pagegc = gdk_gc_new(pagepixmap);
            gdk_gc_set_foreground(pagegc, g_white);
            gdk_gc_set_function(pagegc, GDK_COPY);
            gdk_draw_rectangle(pagepixmap, pagegc, TRUE, 0, 0, REPORT_WIDTH, REPORT_HEIGHT);
            gdk_gc_set_foreground(pagegc, g_black);
            gdk_gc_set_function(pagegc, GDK_COPY);

            drawingarea = gtk_drawing_area_new();
            pagelayout = gtk_widget_create_pango_layout(drawingarea, text);
            pango_layout_set_width(pagelayout, (REPORT_WIDTH-40*2)*PANGO_SCALE);
            pango_layout_set_justify(pagelayout, TRUE);
            pango_layout_set_wrap(pagelayout, PANGO_WRAP_WORD_CHAR);
            pango_layout_set_alignment(pagelayout, PANGO_ALIGN_LEFT);
            pango_layout_set_font_description(pagelayout, font);
            pango_layout_get_pixel_size(pagelayout, &w, &h);

            x = 0;
            y = 40;
      //  }
      //  else
        //{
          //  return;
      //  }
    }
    gdk_draw_layout(pagepixmap, pagegc, x, y, pagelayout);
    x += w;
    y += 4;
   PRINTF("AddTextToPageByPixmap(exit:text:%s,  x = %d, y = %d)\n",text,  x, y);
	return;
}

void ViewPrintPreview::AddImageToPageByPixmap(int &x, int &y, GdkPixbuf *pixbuf)
{

    //REPORT_WIDTH, REPORT_HEIGHT
    if(NULL == pixbuf) return;
    int added_x = 0, added_y = 0;
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    added_x = x + width;
    added_y = y + height;
    if (!pagepixmap)
    {
        pagepixmap = gdk_pixmap_new(NULL, REPORT_WIDTH, REPORT_HEIGHT, 24);
    }

    if(!pagegc)
    {
        pagegc = gdk_gc_new(pagepixmap);
        gdk_gc_set_foreground(pagegc, g_white);
        gdk_gc_set_function(pagegc, GDK_COPY);
        gdk_draw_rectangle(pagepixmap, pagegc, TRUE, 0, 0, REPORT_WIDTH, REPORT_HEIGHT);

        gdk_gc_set_foreground(pagegc, g_black);
        gdk_gc_set_function(pagegc, GDK_COPY);
    }
    if(added_y > REPORT_HEIGHT - 30)
    {
     //n   int maxpage = MAX_PAGE;
        char *pathname = GetReportPageName(report_page_count);
        if(0 != SavePageByPixmap(pathname))
        {
            return;
        }
		free(pathname);
        g_object_unref(pagegc);
        g_object_unref(pagelayout);
        g_object_unref(pagepixmap );
        pagegc = NULL;
        pagelayout = NULL;
        pagepixmap = NULL;

        report_page_count++;
      //  if(report_page_count < maxpage)
      //  {
            pagepixmap = gdk_pixmap_new(NULL, REPORT_WIDTH, REPORT_HEIGHT, 24);

            pagegc = gdk_gc_new(pagepixmap);
            gdk_gc_set_foreground(pagegc, g_white);
            gdk_gc_set_function(pagegc, GDK_COPY);
            gdk_draw_rectangle(pagepixmap, pagegc, TRUE, 0, 0, REPORT_WIDTH, REPORT_HEIGHT);
            g_object_unref(pagegc);

            pagegc = gdk_gc_new(pagepixmap);
            gdk_gc_set_foreground(pagegc, g_black);
            gdk_gc_set_function(pagegc, GDK_COPY);

            y = 0;
            added_x = x ;//+ width;
            added_y = y;// + height;
            page_num = true;
      //  }
      //  else
        //    return;
    }
    gdk_draw_pixbuf(pagepixmap, pagegc, pixbuf, 0, 0, x, y, -1,-1, GDK_RGB_DITHER_NORMAL, 0, 0);
    y = added_y;
    x = added_x;
}

void ViewPrintPreview::InitReportPagePath(char *path)
{
    if (m_path != NULL)
    {
        free(m_path);
        m_path = NULL;
    }

	if(NULL == path)
	{
		m_path = (char *)malloc(512);
		sprintf(m_path, "%s", "./res/reportpage");
	}
	else
	{
        m_path = (char *)malloc(512);
        sprintf(m_path, "%s", path);
    }

    char cmd[512];
    if(0 == access(m_path, F_OK))//目录存在
    {
        for(int i = 0;; i++)
        {
            sprintf(cmd, "%s/report_page%d.png", m_path, i);
            if(access(cmd, F_OK) == 0)
            {

                if(g_file_test(cmd, G_FILE_TEST_EXISTS))
                {
                    if(!remove(cmd))
                        PRINTF("remove reportpage error!\n");
                }
            }
            else
                break;
        }
    }
    else
    {///>create the directory
        GFile *dir = g_file_new_for_path(m_path);
        GError *err_mkdir = NULL;
        if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
        {
            if(err_mkdir->code!=G_IO_ERROR_EXISTS)
            {
                PRINTF("g_file_make_directory %s error: %s\n", g_file_get_path(dir), err_mkdir->message);
            }
        }
        g_object_unref(dir);
    }

}

void ViewPrintPreview::CreatePages(char *path)
{
#ifdef VET
    char *report_head=_("REPORT OF ULTRASOUND");
    char *hospitalstr = _("Hospital: ");
    char *check_date = _("Exam Date:  ");
    char *id = _("ID: ");
    char *check_num = _("Exam ID:  ");
    char *name = _("Animal Name:");
    //char *name = _("Name:");
    char *sex = _("Gender: ");
    char *age = _("Age: ");
    char *section = _("Exam Type: ");
    char *ul_idea = _("Descriptor: ");
    char *comment = _("Comments: ");
    char *check_doctor = _("Diagnostician: ");
    char *report_doctor = _("Physician: ");
    char *owner=_("Owner:");
    char *species=_("Species:");
#else
    char *report_head = _("REPORT OF ULTRASOUND");
    char *hospitalstr = _("Hospital: ");
    char *check_date = _("Exam Date:  ");
    char *id = _("ID: ");
    char *check_num = _("Exam ID:  ");
    char *name = _("Name: ");
    char *sex = _("Gender: ");
    char *age = _("Age: ");
    char *section = _("Exam Type: ");
    char *ul_idea = _("Descriptor: ");
    char *comment = _("Comments: ");
    char *check_doctor = _("Diagnostician: ");
    char *report_doctor = _("Physician: ");
#endif

    struct print_re_item ReportItem;

    char idstr[50], namestr[68], check_numstr[50], sexstr[50], agestr[50], ageUnitstr[50],sectionstr[50], check_datestr[50], check_doctorstr[50], report_doctorstr[50], ul_ideastr[2000], commentstr[2000];
    char tmp[1000];
#ifdef VET
    char ownerstr[60];
    char speciesstr[60];
    string str_species;
#endif
    PatientInfo::Info report_info;
    g_patientInfo.GetInfo(report_info);
    strcpy(idstr, report_info.p.id.c_str());
#ifdef VET
    sprintf(namestr,"%s",report_info.p.animal_name.c_str());
    sprintf(ownerstr,"%s",report_info.p.owner_name.c_str());
    GetAnimalSpeciesString(report_info.p.species,str_species);
    sprintf(speciesstr,"%s",str_species.c_str());
#else
    sprintf(namestr, "%s %s %s", report_info.p.name.last.c_str(), report_info.p.name.first.c_str(), report_info.p.name.mid.c_str());
#endif

    strcpy(check_numstr, report_info.e.examNum.c_str());
#ifdef VET
    if (report_info.p.sex == 0)
        strcpy(sexstr, _("Female "));
    else if (report_info.p.sex == 1)
        strcpy(sexstr, _("Male "));
#else
	if (report_info.p.sex == 0)
        strcpy(sexstr, _("Female"));
    else if (report_info.p.sex == 1)
        strcpy(sexstr, _("Male"));
#endif
    else if(report_info.p.sex == 2)
        strcpy(sexstr, _("Other"));
    else
        strcpy(sexstr,_(" "));

    sprintf(agestr, "%d", report_info.p.age);

    if(report_info.p.ageUnit==0)
        strcpy(ageUnitstr, _("Y"));
    else if(report_info.p.ageUnit==1)
        strcpy(ageUnitstr, _("M"));
    else
        strcpy(ageUnitstr, _("D"));

    string part;
    TopArea::GetInstance()->GetCheckPart(part);
    strcpy(sectionstr, part.c_str());
    char dayTmp[20], monthTmp[20], yearTmp[20];
    itoa(report_info.e.examDate.day, dayTmp, 10);
    if (strlen(dayTmp)==1)
    {
        dayTmp[2]='\0';
        dayTmp[1] = dayTmp[0];
        dayTmp[0] = '0';
    }
    dayTmp[2]='\0';

    itoa(report_info.e.examDate.month, monthTmp, 10);
    if (strlen(monthTmp)==1)
    {
        monthTmp[2]='\0';
        monthTmp[1] = monthTmp[0];
        monthTmp[0] = '0';
    }
    monthTmp[2]='\0';

    itoa(report_info.e.examDate.year, yearTmp, 10);
    yearTmp[4]='\0';

    SysGeneralSetting sys;
    int date_format = sys.GetDateFormat();
    if (date_format == 1)
    {
        sprintf(check_datestr, "%s-%s-%s", monthTmp, dayTmp, yearTmp);
    }
    else if (date_format == 2)
    {
        sprintf(check_datestr, "%s-%s-%s", dayTmp, monthTmp, yearTmp);
    }
    else
    {
        sprintf(check_datestr, "%s-%s-%s", yearTmp, monthTmp, dayTmp);
    }

    string hospital;
    SysGeneralSetting get_hospital;
    get_hospital.GetHospital(hospital);
    sprintf(tmp, "%s", hospital.c_str());
    strcpy(check_doctorstr, report_info.e.examDoctor.c_str());
    strcpy(report_doctorstr, report_info.e.reportDoctor.c_str());
    sprintf(ul_ideastr, "%s", ViewReport::GetInstance()->GetIndication());
    sprintf(commentstr, "%s", ViewReport::GetInstance()->GetComments());

    ReportItem.id = idstr;
    ReportItem.name = namestr;
    ReportItem.check_num = check_numstr;
    ReportItem.sex = sexstr;
    ReportItem.age = agestr;
    ReportItem.ageUnit = ageUnitstr;
    ReportItem.section = sectionstr;
    ReportItem.diag_date = check_datestr;
    ReportItem.hospital = tmp;
    ReportItem.diag_doctor = check_doctorstr;
    ReportItem.report_doctor = report_doctorstr;
    ReportItem.ul_idea = ul_ideastr;
    ReportItem.comment = commentstr;
#ifdef VET
    ReportItem.owner=ownerstr;
    ReportItem.species=speciesstr;
#endif
    InitReportPagePath(path);
    report_page_count = 0;

    char ulTemp[2000];
    int x, y;
    int grap = 15;

    x = 250;
    y = 30;
    sprintf(ulTemp, "%s",report_head);
    AddTextToPageByPixmap(x, y, ulTemp, m_headFont);

    //hospital name
    x = 0;
    y = 120;
    sprintf(ulTemp, "%s%s",hospitalstr, ReportItem.hospital);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //check date
    x = 600;
    y = 120;
    sprintf(ulTemp, "%s%s",check_date, ReportItem.diag_date);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);
    y = 120;

    //draw line
    x = 0;
    y += 30;
    gdk_draw_line(pagepixmap, pagegc, 0, y, REPORT_WIDTH, y);

    //id
    x = 0;
    y = 160;
    sprintf(ulTemp, "%s%s",id, ReportItem.id);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //name
#ifdef VET
    x = 220;
#else
    x = 280;
#endif
    y = 160;
    sprintf(ulTemp, "%s%s",name, ReportItem.name);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);
#ifdef VET
  //species
    x=0;//720;
    y=195;//160;
    sprintf(ulTemp,"%s%s",species,ReportItem.species);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //owner
    x=220;//490;
    y=195;//160;
    sprintf(ulTemp,"%s%s",owner,ReportItem.owner);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);
#endif

    //sex
    x = 0;
#ifdef VET
    y = 195+35;
#else
    y = 195;
#endif
    sprintf(ulTemp, "%s%s", sex, ReportItem.sex);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //age
#ifdef VET
    x = 220;
    y = 195+35;
#else
    x = 200-30-10+10+10;
    y = 195;
#endif
    sprintf(ulTemp, "%s%s%s", age, ReportItem.age,ReportItem.ageUnit);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //section
#ifdef VET
    x = 400;
    y = 195+35;
#else
    x = 340-30-20-10+10;
    y = 195;
#endif
    sprintf(ulTemp, "%s%s",section, ReportItem.section);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //check num
#ifdef VET
    x = 720;
    y = 195+35;
#else
    x = 660+10;
    y = 195;
#endif
    sprintf(ulTemp, "%s%s",check_num, ReportItem.check_num);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //draw line
    x = 0;
    y += 30;
    gdk_draw_line(pagepixmap, pagegc, x, y, REPORT_WIDTH, y);

    //insert picture
    y += grap;
    int xy;
    xy = y;

    int max_image_count = MAX_IMAGE;
    vector<string> imageName = ViewReport::GetInstance()->GetSelectImage();
    int imageNums = imageName.size();
    if(imageNums > max_image_count)imageNums = max_image_count;

    for(int i = 0; i < imageNums; i++)
    {
        GdkPixbuf *pixbuf = NULL;
        ImgMan::ImgItem image;
        y = xy;
        if(imageNums == 1)
        {
            x = (REPORT_WIDTH - IMG_SCALE_WIDTH)/2;
        }
        else
        {
            x = (REPORT_WIDTH/2 - IMG_SCALE_WIDTH)/2 + (REPORT_WIDTH/2)*(i%2);
        }

        if(ImgMan::GetInstance()->ReadSnap(imageName[i].c_str(), &image) == 0)
        {
            //pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, IMG_SCALE_WIDTH, IMG_SCALE_HEIGHT);
            //gdk_pixbuf_scale(image.pixbuf, pixbuf, 0, 0, IMG_SCALE_WIDTH, IMG_SCALE_HEIGHT, (-80)*IMAGE_SCALE, (-120)*IMAGE_SCALE, IMAGE_SCALE, IMAGE_SCALE, GDK_INTERP_HYPER);

            //截取区域起始位置及大小，原图大小844x660
            int offset_x = 0;
            int offset_y = -100;
            int src_w = 750;
            int src_h = 560;
            //目标大小宽度固定为440，根据宽度计算出scale再计算出高度
            int dest_w = IMG_SCALE_WIDTH;
            double scale = (double)dest_w / src_w;
            int dest_h = src_h * scale;
            pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, dest_w, dest_h);
            gdk_pixbuf_scale(image.pixbuf, pixbuf, 0, 0, dest_w, dest_h, offset_x*scale, offset_y*scale, scale, scale, GDK_INTERP_HYPER);
            //也可用composite得到新的pixbuf，下面的结果与scale相同
            //gdk_pixbuf_composite(image.pixbuf, pixbuf, 0, 0, dest_w, dest_h, offset_x*scale, offset_y*scale, scale, scale, GDK_INTERP_HYPER, 255);
            //	printf("source pixbuf w = %d, h = %d\n", gdk_pixbuf_get_width(image.pixbuf), gdk_pixbuf_get_height(image.pixbuf));
            //	printf("pixbuf w = %d, h = %d\n", gdk_pixbuf_get_width(pixbuf), gdk_pixbuf_get_height(pixbuf));
            AddImageToPageByPixmap(x, y, pixbuf);

            g_object_unref(pixbuf);
            g_object_unref(image.pixbuf);
        }
        if(i%2)
        {
            xy = y + grap;
        }
        // else if(i == 4)
        if(page_num)
        {
            xy = y;
            page_num = false;
        }

    }

    //cal result
    y += grap;
    x = 0;

    int k = 0;
    CalcNode_t calcnode;
    while(ViewReport::GetInstance()->GetItemFromCalcArr(k++, &calcnode))
    {
        if(OB_M == calcnode.section)
        {
            if ((strlen(calcnode.gw)) && (strlen(calcnode.edcb)))
                sprintf(ulTemp, "%s %s GW %s EDD %s;", calcnode.title, calcnode.value, calcnode.gw, calcnode.edcb);
            else if ((strlen(calcnode.gw)) && (!strlen(calcnode.edcb)))
                sprintf(ulTemp, "%s %s GW %s;", calcnode.title, calcnode.value, calcnode.gw);
            else if ((!strlen(calcnode.gw)) && (strlen(calcnode.edcb)))
                sprintf(ulTemp, "%s %s EDD %s;", calcnode.title, calcnode.value, calcnode.edcb);
            else
                sprintf(ulTemp, "%s %s;",calcnode.title, calcnode.value);

            int w =0, h = 0;
            GtkWidget *da = gtk_drawing_area_new();
            PangoLayout *pl = gtk_widget_create_pango_layout(da, ulTemp);
            pango_layout_set_width(pl, (REPORT_WIDTH-40*2)*PANGO_SCALE);
            pango_layout_set_justify(pl, TRUE);
            pango_layout_set_wrap(pl, PANGO_WRAP_WORD_CHAR);
            pango_layout_set_alignment(pl, PANGO_ALIGN_LEFT);
            pango_layout_set_font_description(pl, m_commentTmpFont);
            pango_layout_get_pixel_size(pl, &w, &h);
            g_object_unref(pl);

            if((0 != x) && x < REPORT_WIDTH/2)
            {
                x = REPORT_WIDTH / 2 + 15;
            }
            else if((x >= REPORT_WIDTH/2) && (x+w> REPORT_WIDTH-5))
            {
                x = 0;
                y += h;
            }

            AddTextToPageByPixmap(x, y, ulTemp, m_commentTmpFont);
            if(x > REPORT_WIDTH/2)
            {
                x = 0;
                y += h;
            }
        }
    }
    for(int section = SECTION_START; section < SECTION_END; section++)
    {
        k = 0;
        while(ViewReport::GetInstance()->GetItemFromCalcArr(k++, &calcnode) && (OB_M != section))
        {
            if(section == calcnode.section)
            {
                sprintf(ulTemp, "%s %s;",calcnode.title, calcnode.value);
                int w =0, h = 0;
                GtkWidget *da = gtk_drawing_area_new();
                PangoLayout *pl = gtk_widget_create_pango_layout(da, ulTemp);
                pango_layout_set_width(pl, (REPORT_WIDTH-40*2)*PANGO_SCALE);
                pango_layout_set_justify(pl, TRUE);
                pango_layout_set_wrap(pl, PANGO_WRAP_WORD_CHAR);
                pango_layout_set_alignment(pl, PANGO_ALIGN_LEFT);
                pango_layout_set_font_description(pl, m_commentTmpFont);
                pango_layout_get_pixel_size(pl, &w, &h);
                g_object_unref(pl);

                if((0 != x) && x < REPORT_WIDTH/2)
                {
                    x = REPORT_WIDTH / 2 + 15;
                }
                else if((x >= REPORT_WIDTH/2) && (x+w> REPORT_WIDTH-5))
                {
                    x = 0;
                    y += h;
                }

                AddTextToPageByPixmap(x, y, ulTemp, m_commentTmpFont);
                if(x > REPORT_WIDTH/2)
                {
                    x = 0;
                    y += h;
                }
            }
        }
    }

    //description
    if (ViewSystem::GetInstance()->CheckFlagFromReportTemplet(OTHERS_INDICTION_SHOW_ID))
    {
        y += grap;
        x = 0;
        sprintf(ulTemp, "%s", ul_idea);
        AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

        x = 40;
        y += grap;
        sprintf(ulTemp, "%s", ReportItem.ul_idea);
        int w =0, h = 0;
        GtkWidget *da = gtk_drawing_area_new();
        PangoLayout *pl = gtk_widget_create_pango_layout(da, ulTemp);
        pango_layout_set_width(pl, (REPORT_WIDTH-40*2)*PANGO_SCALE);
        pango_layout_set_justify(pl, TRUE);
        pango_layout_set_wrap(pl, PANGO_WRAP_WORD_CHAR);
        pango_layout_set_alignment(pl, PANGO_ALIGN_LEFT);
        pango_layout_set_font_description(pl, m_commentTmpFont);
        pango_layout_get_pixel_size(pl, &w, &h);
        g_object_unref(pl);
        AddTextToPageByPixmap(x, y, ulTemp, m_commentTmpFont);
        y += h;
    }
    else
    {
        PRINTF("CreatePages(description not show)\n");
    }

    //comment
    if (ViewSystem::GetInstance()->CheckFlagFromReportTemplet(OTHERS_COMMENT_SHOW_ID))
    {
        y += grap;
        x = 0;
        sprintf(ulTemp, "%s",comment);
        AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

        y += grap;
        x = 40;
        sprintf(ulTemp, "%s",ReportItem.comment);
        int w =0, h = 0;
        GtkWidget *da = gtk_drawing_area_new();
        PangoLayout *pl = gtk_widget_create_pango_layout(da, ulTemp);
        pango_layout_set_width(pl, (REPORT_WIDTH-40*2)*PANGO_SCALE);
        pango_layout_set_justify(pl, TRUE);
        pango_layout_set_wrap(pl, PANGO_WRAP_WORD_CHAR);
        pango_layout_set_alignment(pl, PANGO_ALIGN_LEFT);
        pango_layout_set_font_description(pl, m_commentTmpFont);
        pango_layout_get_pixel_size(pl, &w, &h);
        g_object_unref(pl);
        AddTextToPageByPixmap(x, y, ulTemp, m_commentTmpFont);
        y += h;
    }
    else
    {
        PRINTF("CreatePages(comment not show)\n");
    }

    x = 0;
    y = 1190;
    gdk_draw_line(pagepixmap, pagegc, 0, y, REPORT_WIDTH, y);

    //check doctor
    sprintf(ulTemp, "%s%s",check_doctor, ReportItem.diag_doctor);
    x = 0;
    y = 1200;
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //report doctor
    sprintf(ulTemp, "%s%s",report_doctor, ReportItem.report_doctor);
    x = 380;
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //end
    AddTextToPageByPixmap(x, y, NULL, m_baseFont);
}
#ifndef VET
void ViewPrintPreview::CreatePagesForRetrieve(char *path,string indication,string comments,vector<string> imageFileName)
{
	char *report_head = _("REPORT OF ULTRASOUND");
	char *hospitalstr = _("Hospital: ");
	char *check_date = _("Exam Date:  ");
	char *id = _("ID: ");
	char *check_num = _("Exam ID:  ");
	char *name = _("Name: ");
	char *sex = _("Gender: ");
	char *age = _("Age: ");
	char *section = _("Exam Type: ");
	char *ul_idea = _("Descriptor: ");
	char *comment = _("Comments: ");
	char *check_doctor = _("Diagnostician: ");
	char *report_doctor = _("Physician: ");

    struct print_re_item ReportItem;

    char idstr[50], namestr[60], check_numstr[50], sexstr[50], agestr[50], sectionstr[50], check_datestr[50], check_doctorstr[50], report_doctorstr[50], ul_ideastr[2000], commentstr[2000];
    char tmp[1000];

    PatientInfo::Info report_info;
   // g_patientInfo.GetInfo(report_info);
    if(ViewQueryRetrieve::GetInstance()->GetRetrieveFlag())
        ViewQueryRetrieve::GetInstance()->GetInfo(report_info);
    else
        ViewDicomDataSelect::GetInstance()->GetInfo(report_info);
    strcpy(idstr, report_info.p.id.c_str());
    sprintf(namestr, "%s %s %s", report_info.p.name.last.c_str(), report_info.p.name.first.c_str(), report_info.p.name.mid.c_str());
    strcpy(check_numstr, report_info.e.examNum.c_str());
#ifdef VET
    if (report_info.p.sex == 0)
        strcpy(sexstr, _("Female "));
    else if (report_info.p.sex == 1)
        strcpy(sexstr, _("Male "));
#else
    if (report_info.p.sex == 0)
        strcpy(sexstr, _("Female"));
    else if (report_info.p.sex == 1)
        strcpy(sexstr, _("Male"));
#endif
    else
        strcpy(sexstr, _("Other"));

    string unit;
    switch(report_info.p.ageUnit)
    {
        case 0:
            unit = _("Y"); break;
        case 1:
            unit = _("M"); break;
        case 2:
            unit = _("D"); break;
        default:
            unit = _("Y"); break;

    }
    sprintf(agestr, "%d%s", report_info.p.age,unit.c_str());

    string part;
    TopArea::GetInstance()->GetCheckPart(part);
    strcpy(sectionstr, part.c_str());
    char dayTmp[20], monthTmp[20], yearTmp[20];
    //printf("year=%d month=%d day=%d\n", report_info.e.examDate.year, report_info.e.examDate.month, report_info.e.examDate.day);
    itoa(report_info.e.examDate.day, dayTmp, 10);
    if (strlen(dayTmp)==1)
    {
        dayTmp[2]='\0';
        dayTmp[1] = dayTmp[0];
        dayTmp[0] = '0';
    }
    dayTmp[2]='\0';

    itoa(report_info.e.examDate.month, monthTmp, 10);
    if (strlen(monthTmp)==1)
    {
        monthTmp[2]='\0';
        monthTmp[1] = monthTmp[0];
        monthTmp[0] = '0';
    }
    monthTmp[2]='\0';

    itoa(report_info.e.examDate.year, yearTmp, 10);
    yearTmp[4]='\0';

	SysGeneralSetting sys;
	int date_format = sys.GetDateFormat();
    if (date_format == 1)
    {
        sprintf(check_datestr, "%s-%s-%s", monthTmp, dayTmp, yearTmp);
    }
    else if (date_format == 2)
    {
        sprintf(check_datestr, "%s-%s-%s", dayTmp, monthTmp, yearTmp);
    }
    else
    {
        sprintf(check_datestr, "%s-%s-%s", yearTmp, monthTmp, dayTmp);
    }

	string hospital;
	SysGeneralSetting get_hospital;
	get_hospital.GetHospital(hospital);
    sprintf(tmp, "%s", hospital.c_str());
	strcpy(check_doctorstr, report_info.e.examDoctor.c_str());
    strcpy(report_doctorstr, report_info.e.reportDoctor.c_str());
    sprintf(ul_ideastr, "%s", indication.c_str());
    sprintf(commentstr, "%s", comments.c_str());

	ReportItem.id = idstr;
	ReportItem.name = namestr;
	ReportItem.check_num = check_numstr;
	ReportItem.sex = sexstr;
	ReportItem.age = agestr;
	ReportItem.section = sectionstr;
	ReportItem.diag_date = check_datestr;
    ReportItem.hospital = tmp;
    ReportItem.diag_doctor = check_doctorstr;
    ReportItem.report_doctor = report_doctorstr;
    ReportItem.ul_idea = ul_ideastr;
    ReportItem.comment = commentstr;

	InitReportPagePath(path);
    report_page_count = 0;

    char ulTemp[2000];
    int x, y;
    int grap = 15;

    x = 250;
    y = 30;
    sprintf(ulTemp, "%s",report_head);
    AddTextToPageByPixmap(x, y, ulTemp, m_headFont);

    //hospital name
    x = 0;
    y = 120;
    sprintf(ulTemp, "%s%s",hospitalstr, ReportItem.hospital);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //check date
    x = 600;
    y = 120;
    sprintf(ulTemp, "%s%s",check_date, ReportItem.diag_date);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //draw line
    x = 0;
    y += 30;
    gdk_draw_line(pagepixmap, pagegc, 0, y, REPORT_WIDTH, y);

    //id
    x = 0;
    y = 160;
    sprintf(ulTemp, "%s%s",id, ReportItem.id);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //name
    x = 280;
    y = 160;
    sprintf(ulTemp, "%s%s",name, ReportItem.name);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //sex
    x = 0;
    y = 195;
    sprintf(ulTemp, "%s%s", sex, ReportItem.sex);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //age
    x = 200-30-20+10+10;
    y = 195;
    sprintf(ulTemp, "%s%s", age, ReportItem.age);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //section
    x = 340-60-20+10;
    y = 195;
    sprintf(ulTemp, "%s%s",section, ReportItem.section);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //check num
    x = 660+10+10;
    y = 195;
    sprintf(ulTemp, "%s%s",check_num, ReportItem.check_num);
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //draw line
    x = 0;
    y += 30;
    gdk_draw_line(pagepixmap, pagegc, x, y, REPORT_WIDTH, y);

    //insert picture
    y += grap;
    int xy = y;

    int max_image_count = MAX_IMAGE;
	vector<string> imageName = imageFileName;
	int imageNums = imageName.size();
	if(imageNums > max_image_count)imageNums = max_image_count;

    for(int i = 0; i < imageNums; i++)
    {
		GdkPixbuf *pixbuf = NULL;
	    ImgMan::ImgItem image;
        y = xy;
        if(imageNums > 1)
        {
            x = (REPORT_WIDTH/2 - IMG_SCALE_WIDTH)/2 + (REPORT_WIDTH/2)*(i%2);
        }
        else
        {
            x = (REPORT_WIDTH - IMG_SCALE_WIDTH)/2;
        }
        char absPath[512];
        sprintf(absPath,"%s/%s",path,imageName[i].c_str());
		if(ImgMan::GetInstance()->ReadSnap(absPath, &image) == 0)
        {
			//截取区域起始位置及大小，原图大小844x660
			int offset_x = 0;
			int offset_y = -100;
			int src_w = 750;
			int src_h = 560;
			//目标大小宽度固定为440，根据宽度计算出scale再计算出高度
			int dest_w = IMG_SCALE_WIDTH;
			double scale = (double)dest_w / src_w;
			int dest_h = src_h * scale;
			pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, dest_w, dest_h);
			gdk_pixbuf_scale(image.pixbuf, pixbuf, 0, 0, dest_w, dest_h, offset_x*scale, offset_y*scale, scale, scale, GDK_INTERP_HYPER);

			//pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, IMG_SCALE_WIDTH, IMG_SCALE_HEIGHT);
			//gdk_pixbuf_scale(image.pixbuf, pixbuf, 0, 0, IMG_SCALE_WIDTH, IMG_SCALE_HEIGHT, (-80)*IMAGE_SCALE, (-120)*IMAGE_SCALE, IMAGE_SCALE, IMAGE_SCALE, GDK_INTERP_HYPER);

            AddImageToPageByPixmap(x, y, pixbuf);
            g_object_unref(pixbuf);
            g_object_unref(image.pixbuf);
        }
        if(i%2)
        {
            xy = y + grap;
        }
        if(page_num)
        {
            xy = y;
            page_num = false;
        }

    }

    //cal result
    y += grap;
    x = 0;

    int k = 0;
    CalcNode_t calcnode;
    while(ViewReport::GetInstance()->GetItemFromCalcArr(k++, &calcnode))
    {
        if(OB_M == calcnode.section)
        {
            if ((strlen(calcnode.gw)) && (strlen(calcnode.edcb)))
                sprintf(ulTemp, "%s %s GW %s EDD %s;", calcnode.title, calcnode.value, calcnode.gw, calcnode.edcb);
            else if ((strlen(calcnode.gw)) && (!strlen(calcnode.edcb)))
                sprintf(ulTemp, "%s %s GW %s;", calcnode.title, calcnode.value, calcnode.gw);
            else if ((!strlen(calcnode.gw)) && (strlen(calcnode.edcb)))
                sprintf(ulTemp, "%s %s EDD %s;", calcnode.title, calcnode.value, calcnode.edcb);
            else
                sprintf(ulTemp, "%s %s;",calcnode.title, calcnode.value);

            int w =0, h = 0;
            GtkWidget *da = gtk_drawing_area_new();
            PangoLayout *pl = gtk_widget_create_pango_layout(da, ulTemp);
            pango_layout_set_width(pl, (REPORT_WIDTH-40*2)*PANGO_SCALE);
            pango_layout_set_justify(pl, TRUE);
            pango_layout_set_wrap(pl, PANGO_WRAP_WORD_CHAR);
            pango_layout_set_alignment(pl, PANGO_ALIGN_LEFT);
            pango_layout_set_font_description(pl, m_commentTmpFont);
            pango_layout_get_pixel_size(pl, &w, &h);
            g_object_unref(pl);

            if((0 != x) && x < REPORT_WIDTH/2)
            {
                x = REPORT_WIDTH / 2 + 15;
            }
            else if((x >= REPORT_WIDTH/2) && (x+w> REPORT_WIDTH-5))
            {
                x = 0;
                y += h;
            }

            AddTextToPageByPixmap(x, y, ulTemp, m_commentTmpFont);
            if(x > REPORT_WIDTH/2)
            {
                x = 0;
                y += h;
            }
        }
    }
    for(int section = SECTION_START; section < SECTION_END; section++)
    {
        k = 0;
        while(ViewReport::GetInstance()->GetItemFromCalcArr(k++, &calcnode) && (OB_M != section))
        {
            if(section == calcnode.section)
            {
                sprintf(ulTemp, "%s %s;",calcnode.title, calcnode.value);
                int w =0, h = 0;
                GtkWidget *da = gtk_drawing_area_new();
                PangoLayout *pl = gtk_widget_create_pango_layout(da, ulTemp);
                pango_layout_set_width(pl, (REPORT_WIDTH-40*2)*PANGO_SCALE);
                pango_layout_set_justify(pl, TRUE);
                pango_layout_set_wrap(pl, PANGO_WRAP_WORD_CHAR);
                pango_layout_set_alignment(pl, PANGO_ALIGN_LEFT);
                pango_layout_set_font_description(pl, m_commentTmpFont);
                pango_layout_get_pixel_size(pl, &w, &h);
                g_object_unref(pl);

                if((0 != x) && x < REPORT_WIDTH/2)
                {
                    x = REPORT_WIDTH / 2 + 15;
                }
                else if((x >= REPORT_WIDTH/2) && (x+w> REPORT_WIDTH-5))
                {
                    x = 0;
                    y += h;
                }

                AddTextToPageByPixmap(x, y, ulTemp, m_commentTmpFont);
                if(x > REPORT_WIDTH/2)
                {
                    x = 0;
                    y += h;
                }
            }
        }
    }

    //description
    if (ViewSystem::GetInstance()->CheckFlagFromReportTemplet(OTHERS_INDICTION_SHOW_ID))
    {
        y += grap;
        x = 0;
        sprintf(ulTemp, "%s", ul_idea);
        AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

        x = 40;
        y += grap;
        sprintf(ulTemp, "%s", ReportItem.ul_idea);
        int w =0, h = 0;
        GtkWidget *da = gtk_drawing_area_new();
        PangoLayout *pl = gtk_widget_create_pango_layout(da, ulTemp);
        pango_layout_set_width(pl, (REPORT_WIDTH-40*2)*PANGO_SCALE);
        pango_layout_set_justify(pl, TRUE);
        pango_layout_set_wrap(pl, PANGO_WRAP_WORD_CHAR);
        pango_layout_set_alignment(pl, PANGO_ALIGN_LEFT);
        pango_layout_set_font_description(pl, m_commentTmpFont);
        pango_layout_get_pixel_size(pl, &w, &h);
        g_object_unref(pl);
        AddTextToPageByPixmap(x, y, ulTemp, m_commentTmpFont);
        y += h;
    }
    else
    {
        PRINTF("CreatePages(description not show)\n");
    }

    //comment
    if (ViewSystem::GetInstance()->CheckFlagFromReportTemplet(OTHERS_COMMENT_SHOW_ID))
    {
        y += grap;
        x = 0;
        sprintf(ulTemp, "%s",comment);
        AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

        y += grap;
        x = 40;
        sprintf(ulTemp, "%s",ReportItem.comment);
        int w =0, h = 0;
        GtkWidget *da = gtk_drawing_area_new();
        PangoLayout *pl = gtk_widget_create_pango_layout(da, ulTemp);
        pango_layout_set_width(pl, (REPORT_WIDTH-40*2)*PANGO_SCALE);
        pango_layout_set_justify(pl, TRUE);
        pango_layout_set_wrap(pl, PANGO_WRAP_WORD_CHAR);
        pango_layout_set_alignment(pl, PANGO_ALIGN_LEFT);
        pango_layout_set_font_description(pl, m_commentTmpFont);
        pango_layout_get_pixel_size(pl, &w, &h);
        g_object_unref(pl);
        AddTextToPageByPixmap(x, y, ulTemp, m_commentTmpFont);
        y += h;
    }
    else
    {
        PRINTF("CreatePages(comment not show)\n");
    }

    x = 0;
    y = 1190;
    gdk_draw_line(pagepixmap, pagegc, 0, y, REPORT_WIDTH, y);

    //check doctor
    sprintf(ulTemp, "%s%s",check_doctor, ReportItem.diag_doctor);
    x = 0;
    y = 1200;
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //report doctor
    sprintf(ulTemp, "%s%s",report_doctor, ReportItem.report_doctor);
    x = 380;
    AddTextToPageByPixmap(x, y, ulTemp, m_baseFont);

    //end
    AddTextToPageByPixmap(x, y, NULL, m_baseFont);

}
#endif
void ViewPrintPreview::ExposeImage(GtkWidget *widget, GdkEventExpose *event)
{
    int i;
    for(i = 0; i < report_page_count; i++)
    {
        if(report_image[i] == widget)break;
    }

   // int maxpage = MAX_PAGE;
 //   if(i >= maxpage && i >= report_page_count) return;

    cairo_t *cr;
    cr = gdk_cairo_create(widget->window);
	char *pathname = GetReportPageName(i);

    SysGeneralSetting sysGeneralSetting;
    if(sysGeneralSetting.GetPrintViewMode())
    {
        cairo_surface_t *image = cairo_image_surface_create_from_png(pathname);
        if(image)
        {
            cairo_set_source_surface(cr, image, 0, 0);
            cairo_surface_destroy(image);
        }
    }
    else
    {
        int width = 650*REPORT_WIDTH/REPORT_HEIGHT;
        int height = 650;
        GError *error = NULL;
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(pathname, &error);
        if(pixbuf && !error)
        {
            GdkPixbuf *scale_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
            gdk_cairo_set_source_pixbuf (cr, scale_pixbuf, 0, 0);
            g_object_unref(scale_pixbuf);
            g_object_unref(pixbuf);
        }
    }

    cairo_paint (cr);
    cairo_destroy(cr);
    free(pathname);
}

void ViewPrintPreview::ClickedPrintButton(GtkButton *button)
{
 //   char device[128];
//  if(PeripheralMan::GetInstance()->GetPrinterDev(device))
    {
        for(int i = 0; i < report_page_count; i++)
        {
            GError *error = NULL;
    		char *pathname = GetReportPageName(i);
            GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(pathname, &error);
    		free(pathname);
            if(pixbuf && !error)
            {
                m_imgPath = PRINTIMAGE;
                remove(m_imgPath.c_str());
                if (!gdk_pixbuf_save(pixbuf, PRINTIMAGE, "bmp", &error, NULL))
                {
                    g_error_free(error);
                }
                else
                {
                    if(MODE_PRINT_NEW)
                    {
                        Printmain::GetInstance()->GetPrintOptions();
                        const char *path = m_imgPath.c_str();
                        Printfunctions::GetInstance()->PrintFunc(path);
                    }
                    else
                        Printing();
                }
                g_object_unref(pixbuf);

            }
        }
    }
#if 0
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("No printer found."), NULL);
    }
#endif
}

void ViewPrintPreview::PrintArchiveReport(const char* path)
{
    char reportPath[256];
    strcpy(reportPath, path);
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(reportPath, &error);
    if(pixbuf && !error)
    {
        m_imgPath = PRINTIMAGE;
        remove(m_imgPath.c_str());
        if (!gdk_pixbuf_save(pixbuf, PRINTIMAGE, "bmp", &error, NULL))
        {
            g_error_free(error);
        }
        else
        {
            if(MODE_PRINT_NEW)
            {
                Printmain::GetInstance()->GetPrintOptions();
                const char *pathTmp = m_imgPath.c_str();
                Printfunctions::GetInstance()->PrintFunc(pathTmp);
            }
            else
                Printing();
        }
        g_object_unref(pixbuf);
    }
}