#include <gtk/gtk.h>
#include "MenuEFOV.h"
#include "Menu2D.h"
#include "../display/gui_global.h"
#include "../display/gui_func.h"

MenuEFOV g_menuEFOV;

MenuEFOV::MenuEFOV(void)
{
    m_table = 0;
}

void MenuEFOV::Hide(void)
{
    gtk_widget_hide_all(m_table);
}

void MenuEFOV::Show(void)
{
    gtk_widget_show_all(m_table);
}

void MenuEFOV::ShowPrepare(void)
{
    Show();
    gtk_widget_hide_all(m_fixedCapture);
    gtk_widget_show_all(m_fixedPrepare);

}

void MenuEFOV::ShowView(void)
{
    Hide();
}

void MenuEFOV::ShowReview(void)
{
    Hide();
}

void MenuEFOV::ShowCapture(void)
{
    Show();
    gtk_widget_hide_all(m_fixedPrepare);
    gtk_widget_show_all(m_fixedCapture);
}

GtkWidget* MenuEFOV::Create(void)
{
    m_table = gtk_table_new(20, 1, TRUE);

    m_fixedPrepare = gtk_fixed_new();
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_fixedPrepare, 0, 1, 0, 5);
  
    m_labelPress = create_label("", 100, 30, g_lightGray, NULL);
    gtk_misc_set_alignment (GTK_MISC (m_labelPress), 0.1, 0.5);
    gtk_fixed_put(GTK_FIXED(m_fixedPrepare), m_labelPress, 5, 0);
#if (defined (EMP_322) || defined(EMP_313))
    GtkWidget *image_update = create_pixmap("./res/icon/set.png");
#else
    GtkWidget *image_update = create_pixmap("./res/icon/update.png");
#endif
    //GtkWidget *image_update = create_pixmap("./res/icon/update.png");
    gtk_fixed_put (GTK_FIXED (m_fixedPrepare), image_update, 40, 30);
    gtk_widget_set_size_request (image_update, 100, 40);
    gtk_misc_set_alignment (GTK_MISC (image_update), 0.1, 0.5);

    m_labelPrepareHint = create_label("", 150, 60, g_lightGray, NULL);
    gtk_fixed_put(GTK_FIXED(m_fixedPrepare), m_labelPrepareHint, 5, 70);
    gtk_misc_set_alignment (GTK_MISC (m_labelPrepareHint), 0.1, 0.5);

    m_fixedCapture = gtk_fixed_new();
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_fixedCapture, 0, 1, 0, 12);

    m_labelPress2 = create_label("", 100, 30, g_lightGray, NULL);
    gtk_misc_set_alignment (GTK_MISC (m_labelPress2), 0.1, 0.5);
    gtk_fixed_put(GTK_FIXED(m_fixedCapture), m_labelPress2, 5, 0);

    GtkWidget *image_freeze = create_pixmap("./res/icon/freeze.png");
    gtk_fixed_put (GTK_FIXED (m_fixedCapture), image_freeze, 40, 30);
    gtk_widget_set_size_request (image_freeze, 100, 55);
    gtk_misc_set_alignment (GTK_MISC (image_freeze), 0.1, 0.5);

    m_labelCaptureHint = create_label("", 150, 60, g_lightGray, NULL);
    gtk_fixed_put(GTK_FIXED(m_fixedCapture), m_labelCaptureHint, 5, 85);
    gtk_misc_set_alignment (GTK_MISC (m_labelCaptureHint), 0.1, 0.5);

    m_labelPress3 = create_label("", 100, 30, g_lightGray, NULL);
    gtk_misc_set_alignment (GTK_MISC (m_labelPress3), 0.1, 0.5);
    gtk_fixed_put(GTK_FIXED(m_fixedCapture), m_labelPress3, 5, 170);
#if (defined (EMP_322) || defined(EMP_313))
    GtkWidget *image_update2 = create_pixmap("./res/icon/set.png");
#else
GtkWidget *image_update2 = create_pixmap("./res/icon/update.png");

#endif
   // GtkWidget *image_update2 = create_pixmap("./res/icon/update.png");
    gtk_fixed_put (GTK_FIXED (m_fixedCapture), image_update2, 40, 200);
    gtk_widget_set_size_request (image_update2, 100, 55);
    gtk_misc_set_alignment (GTK_MISC (image_update2), 0.1, 0.5);

    m_labelCaptureHint2 = create_label("", 150, 60, g_lightGray, NULL);
    gtk_fixed_put(GTK_FIXED(m_fixedCapture), m_labelCaptureHint2, 5, 255);
    gtk_misc_set_alignment (GTK_MISC (m_labelCaptureHint2), 0.1, 0.5);

    UpdateLabel();
//    gtk_widget_set_usize(m_table, WIDTH_2D_MENU, HEIGHT_2D_MENU);
    return m_table;
}
    
void MenuEFOV::UpdateLabel(void)
{
    gtk_label_set_markup(GTK_LABEL(m_labelPress), _(" <b>Press</b>"));
    gtk_label_set_markup(GTK_LABEL(m_labelPress2), _(" <b>Press</b>"));
    gtk_label_set_markup(GTK_LABEL(m_labelPress3), _(" <b>Press</b>"));
    gtk_label_set_markup(GTK_LABEL(m_labelPrepareHint), _(" <b>button to enter the\n \"Capture Status\"</b>"));
    gtk_label_set_markup(GTK_LABEL(m_labelPrepareHint), _(" <b>button to enter the\n \"Capture Status\"</b>"));
    gtk_label_set_markup(GTK_LABEL(m_labelCaptureHint), _(" <b>button to enter the\n \"View Status\"</b>"));
    gtk_label_set_markup(GTK_LABEL(m_labelCaptureHint2), _(" <b>button to enter the\n \"Prepare Status\"</b>"));
}
