#include "ViewCalcDialog.h"
#include "KeyValueOpr.h"
#include "gui_global.h"
#include "gui_func.h"
#include "ViewMain.h"

ViewCalcDialog* ViewCalcDialog::m_ptrInstance = NULL;

ViewCalcDialog* ViewCalcDialog::GetInstance()
{
    if (m_ptrInstance == NULL)
	m_ptrInstance = new ViewCalcDialog;
    return m_ptrInstance;
}

ViewCalcDialog::ViewCalcDialog(void)
{
    m_window = 0;
    m_entry_manual_input = 0;
    m_radiobutton_top = 0;
    m_radiobutton_bottom = 0;
    m_radiobutton_manual = 0;

    m_radiobutton_5 = 0;
    m_radiobutton_10 = 0;
    m_radiobutton_15 = 0;
    m_radiobutton_20 = 0;
}

ViewCalcDialog::~ViewCalcDialog(void)
{
    if (m_ptrInstance != NULL) 
	delete m_ptrInstance; 
}

void ViewCalcDialog::Create(ECalcDialogType type)
{
	GtkWidget *fixed;
	GtkWidget *button_title;
	GtkWidget *label_select;

	GtkWidget *label_unit;
	GtkWidget *button_ok;
	GtkWidget *button_cancel;

	m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (m_window, 360, 230);
	gtk_window_set_title (GTK_WINDOW (m_window), "window");
	gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_MOUSE);
	gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
	gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
	gtk_window_set_decorated (GTK_WINDOW (m_window), FALSE);
	gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
	g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this);

	fixed = gtk_fixed_new ();
	gtk_widget_show (fixed);
	gtk_container_add (GTK_CONTAINER (m_window), fixed);

	button_title = gtk_button_new_with_mnemonic ("Color Aliasing Velocity");
	gtk_widget_show (button_title);
	gtk_fixed_put (GTK_FIXED (fixed), button_title, 0, 0);
	gtk_widget_set_size_request (button_title, 360, 30);

	if (type == CAV)
		label_select = gtk_label_new ("Select a Color Aliasing Velocity :");
	else
		label_select = gtk_label_new ("Select a RA Pressure :");
	gtk_widget_show (label_select);
	gtk_fixed_put (GTK_FIXED (fixed), label_select, 0, 35);
	gtk_widget_set_size_request (label_select, 360, 30);
	gtk_misc_set_alignment (GTK_MISC (label_select), 0, 0.5);

	if (type == CAV) {
		GSList *radiobutton_CAV_group = NULL;

		m_radiobutton_top = gtk_radio_button_new_with_mnemonic (NULL, "Top aliasing velocity");
		gtk_widget_show (m_radiobutton_top);
		gtk_fixed_put (GTK_FIXED (fixed), m_radiobutton_top, 5, 70);
		gtk_widget_set_size_request (m_radiobutton_top, 160, 30);
		gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_top), radiobutton_CAV_group);
		radiobutton_CAV_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_top));

		m_radiobutton_bottom = gtk_radio_button_new_with_mnemonic (NULL, "Bottom aliasing velocity");
		gtk_widget_show (m_radiobutton_bottom);
		gtk_fixed_put (GTK_FIXED (fixed), m_radiobutton_bottom, 175, 70);
		gtk_widget_set_size_request (m_radiobutton_bottom, 185, 30);
		gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_bottom), radiobutton_CAV_group);
		radiobutton_CAV_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_bottom));

		m_radiobutton_manual = gtk_radio_button_new_with_mnemonic (NULL, "Manual Entry :");
		gtk_widget_show (m_radiobutton_manual);
		gtk_fixed_put (GTK_FIXED (fixed), m_radiobutton_manual, 5, 100);
		gtk_widget_set_size_request (m_radiobutton_manual, 120, 30);
		gtk_button_set_focus_on_click (GTK_BUTTON (m_radiobutton_manual), FALSE);
		gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_manual), radiobutton_CAV_group);
		radiobutton_CAV_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_manual));
		g_signal_connect ((gpointer) m_radiobutton_manual, "toggled", G_CALLBACK (on_radio_manual_toggled), this);
	} else if (type == RA) {
		GSList *radiobutton_RA_group = NULL;

		m_radiobutton_5 = gtk_radio_button_new_with_mnemonic (NULL, "5 mmHg");
		gtk_widget_show (m_radiobutton_5);
		gtk_fixed_put (GTK_FIXED (fixed), m_radiobutton_5, 5, 70);
		gtk_widget_set_size_request (m_radiobutton_5, 120, 30);
		gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_5), radiobutton_RA_group);
		radiobutton_RA_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_5));

		m_radiobutton_10 = gtk_radio_button_new_with_mnemonic (NULL, "10 mmHg");
		gtk_widget_show (m_radiobutton_10);
		gtk_fixed_put (GTK_FIXED (fixed), m_radiobutton_10, 175, 70);
		gtk_widget_set_size_request (m_radiobutton_10, 120, 30);
		gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_10), radiobutton_RA_group);
		radiobutton_RA_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_10));

		m_radiobutton_15 = gtk_radio_button_new_with_mnemonic (NULL, "15 mmHg");
		gtk_widget_show (m_radiobutton_15);
		gtk_fixed_put (GTK_FIXED (fixed), m_radiobutton_15, 5, 100);
		gtk_widget_set_size_request (m_radiobutton_15, 120, 30);
		gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_15), radiobutton_RA_group);
		radiobutton_RA_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_15));

		m_radiobutton_20 = gtk_radio_button_new_with_mnemonic (NULL, "20 mmHg");
		gtk_widget_show (m_radiobutton_20);
		gtk_fixed_put (GTK_FIXED (fixed), m_radiobutton_20, 175, 100);
		gtk_widget_set_size_request (m_radiobutton_20, 120, 30);
		gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_20), radiobutton_RA_group);
		radiobutton_RA_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_20));

		m_radiobutton_manual = gtk_radio_button_new_with_mnemonic (NULL, "Manual Entry :");
		gtk_widget_show (m_radiobutton_manual);
		gtk_fixed_put (GTK_FIXED (fixed), m_radiobutton_manual, 5, 130);
		gtk_widget_set_size_request (m_radiobutton_manual, 120, 30);
		gtk_button_set_focus_on_click (GTK_BUTTON (m_radiobutton_manual), FALSE);
		gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_manual), radiobutton_RA_group);
		radiobutton_RA_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_manual));
		g_signal_connect ((gpointer) m_radiobutton_manual, "toggled", G_CALLBACK (on_radio_manual_toggled), this);
	}

	m_entry_manual_input = gtk_entry_new ();
	gtk_widget_show (m_entry_manual_input);
	if (type == CAV)
		gtk_fixed_put (GTK_FIXED (fixed), m_entry_manual_input, 175, 100);
	else 
		gtk_fixed_put (GTK_FIXED (fixed), m_entry_manual_input, 175, 130);
	gtk_widget_set_size_request (m_entry_manual_input, 100, 30);
	g_signal_connect(G_OBJECT(m_entry_manual_input), "insert_text", G_CALLBACK(on_entry_manual_input), this);
	gtk_widget_set_sensitive (m_entry_manual_input, FALSE);

	label_unit = gtk_label_new ("cm/s");
	gtk_widget_show (label_unit);
	if (type == CAV)
		gtk_fixed_put (GTK_FIXED (fixed), label_unit, 275, 100);
	else if (type == RA)
		gtk_fixed_put (GTK_FIXED (fixed), label_unit, 275, 130);
	gtk_widget_set_size_request (label_unit, 50, 30);
	gtk_misc_set_alignment (GTK_MISC (label_unit), 0.2, 0.5);

	if (type == CAV) {
		GtkWidget *label_hint;
		label_hint = gtk_label_new ("Enter a value between 0 and 500 cm/s.");
		gtk_widget_show (label_hint);
		gtk_fixed_put (GTK_FIXED (fixed), label_hint, 0, 140);
		gtk_widget_set_size_request (label_hint, 360, 30);
		gtk_misc_set_alignment (GTK_MISC (label_hint), 0, 0.5);
	}

	button_ok = gtk_button_new_with_mnemonic ("OK");
	gtk_widget_show (button_ok);
	gtk_fixed_put (GTK_FIXED (fixed), button_ok, 190, 190);
	gtk_widget_set_size_request (button_ok, 65, 30);
	g_signal_connect ((gpointer) button_ok, "clicked", G_CALLBACK (on_button_ok_clicked), this);

	button_cancel = gtk_button_new_with_mnemonic ("Cancel");
	gtk_widget_show (button_cancel);
	gtk_fixed_put (GTK_FIXED (fixed), button_cancel, 280, 190);
	gtk_widget_set_size_request (button_cancel, 65, 30);
	g_signal_connect ((gpointer) button_cancel, "clicked", G_CALLBACK (on_button_cancel_clicked), this);

	gtk_widget_show(m_window);

	g_keyInterface.Push(this);
	SetSystemCursorToCenter();
}

gboolean ViewCalcDialog::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
	Destroy();
	return FALSE;
}

void ViewCalcDialog::BtnOkClicked(GtkButton *button)
{
	if (m_radiobutton_top && m_radiobutton_bottom) {
		gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_top));
		gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_bottom));
		PRINTF("enter CAV!\n");
	} else if (m_radiobutton_5 && m_radiobutton_10 && m_radiobutton_15 && m_radiobutton_20) {
		gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_5));
		gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_10));
		gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_15));
		gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_20));
		PRINTF("enter RA!\n");
	}

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_manual))) {
		const char * text = gtk_entry_get_text (GTK_ENTRY(m_entry_manual_input));
		PRINTF("%s\n", text);
	}

	Destroy();
}

void ViewCalcDialog::BtnCancelClicked(GtkButton *button)
{
	Destroy();
}

void ViewCalcDialog::Destroy(void)
{
	gtk_widget_destroy (m_window);
	m_window = 0;
	m_entry_manual_input = 0;
	m_radiobutton_top = 0;
	m_radiobutton_bottom = 0;
	m_radiobutton_manual = 0;

	m_radiobutton_5 = 0;
	m_radiobutton_10 = 0;
	m_radiobutton_15 = 0;
	m_radiobutton_20 = 0;

	g_keyInterface.Pop();
}

void ViewCalcDialog::RadioManualToggled(GtkToggleButton *togglebutton)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton))) {
		gtk_widget_set_sensitive (m_entry_manual_input, TRUE);
		gtk_widget_grab_focus(m_entry_manual_input);
	}
	else
		gtk_widget_set_sensitive (m_entry_manual_input, FALSE);
}

void ViewCalcDialog::EntryNumInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
	if (!g_ascii_isdigit(*new_text))
		g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
	return;
}
