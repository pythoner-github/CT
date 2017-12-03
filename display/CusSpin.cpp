#include <gtk/gtk.h>
#include "CusSpin.h"
#include "gui_func.h"
#include "gui_global.h"
#include "ModeStatus.h"

CusSpin::CusSpin(void)
{
    m_cusspin = 0;
}

CusSpin::~CusSpin(void)
{
	gtk_widget_destroy(m_cusspin);
}

void CusSpin::Show(void)
{
	gtk_widget_show_all(m_cusspin);
   }

//◀  ▶ ◁   ▷
GtkWidget* CusSpin::Create()
{
    m_cusspin = gtk_table_new(1, 4, FALSE);
    gtk_widget_set_usize(m_cusspin, -1, -1);
	
    m_labelText = create_label("", 0, 0, g_lightGray, NULL);
    gtk_widget_modify_fg(m_labelText, GTK_STATE_PRELIGHT, g_lightGray);
    gtk_widget_modify_fg(m_labelText, GTK_STATE_ACTIVE, g_lightGray);
    GtkWidget *btn = create_button(m_labelText, 100, 0, g_deep); // 128
    gtk_widget_modify_bg(btn, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_cusspin), btn, 0, 1, 0, 1);
    GTK_WIDGET_UNSET_FLAGS (btn, GTK_CAN_FOCUS);
    gtk_widget_modify_bg(btn, GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(btn, GTK_STATE_ACTIVE, g_deep);

    m_labelSub = create_label("◀", 0, 0, g_black, NULL);
    m_btnSub = create_button(m_labelSub, 18, 0, g_white); // 20
    gtk_widget_modify_bg(m_btnSub, GTK_STATE_INSENSITIVE, g_white);
    gtk_table_attach_defaults(GTK_TABLE(m_cusspin), m_btnSub, 1, 2, 0, 1);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_btnSub), FALSE);
    g_signal_connect(m_btnSub, "clicked", G_CALLBACK(HandleBtnSub), this);
	
    m_entry = gtk_entry_new();
    gtk_widget_set_usize(m_entry, 30, -1); // 32
    gtk_entry_set_editable(GTK_ENTRY(m_entry), FALSE);
    GTK_WIDGET_UNSET_FLAGS (m_entry, GTK_CAN_FOCUS);
    gtk_table_attach_defaults(GTK_TABLE(m_cusspin), m_entry, 2, 3, 0, 1);
	
    m_labelAdd = create_label("▶", 0, 0, g_black, NULL);
    m_btnAdd = create_button(m_labelAdd, 18, 0, g_white); // 25
    gtk_widget_modify_bg(m_btnAdd, GTK_STATE_INSENSITIVE, g_white);
    gtk_table_attach_defaults(GTK_TABLE(m_cusspin), m_btnAdd, 3, 4, 0, 1);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_btnAdd), FALSE);
    g_signal_connect(m_btnAdd, "clicked", G_CALLBACK(HandleBtnAdd), this);

    UpdateLabel();

    if(m_item->val)
        SetValue(m_item->val, m_item->status);
    if((strcmp(m_item->name, "Threshold") == 0)|| (strcmp(m_item->name, "Color Reject") == 0))
    {
        gtk_widget_modify_fg(m_labelText,GTK_STATE_NORMAL, g_deepGray );
        gtk_label_set_text(GTK_LABEL(m_labelAdd), "▷");
        gtk_label_set_text(GTK_LABEL(m_labelSub), "◁");
        gtk_widget_set_sensitive(m_btnAdd, FALSE);
        gtk_widget_set_sensitive(m_btnSub, FALSE);
    }

    return m_cusspin;
}

void CusSpin::UpdateLabel(void)
{
    gtk_label_set_text(GTK_LABEL(m_labelText), _(m_item->name));
}

void CusSpin::SetValue(const char* str, EKnobReturn flag)
{
	if(str)
		gtk_entry_set_text(GTK_ENTRY(m_entry), str);

	switch(flag)
	{
		case OK:
			gtk_label_set_text(GTK_LABEL(m_labelSub), "◀");
			gtk_label_set_text(GTK_LABEL(m_labelAdd), "▶");
			gtk_widget_set_sensitive(m_btnSub, true);
			gtk_widget_set_sensitive(m_btnAdd, true);
			break;
		case MIN:
			gtk_label_set_text(GTK_LABEL(m_labelSub), "◁");
			gtk_label_set_text(GTK_LABEL(m_labelAdd), "▶");
			gtk_widget_set_sensitive(m_btnSub, false);
			gtk_widget_set_sensitive(m_btnAdd, true);
			break;
		case MAX:
			gtk_label_set_text(GTK_LABEL(m_labelSub), "◀");
			gtk_label_set_text(GTK_LABEL(m_labelAdd), "▷");
			gtk_widget_set_sensitive(m_btnSub, true);
			gtk_widget_set_sensitive(m_btnAdd, false);
			break;
		case ERROR:
			break;
		case PRESS:
			break;
	}
    
    if((strcmp(m_item->name, "Threshold") == 0)|| (strcmp(m_item->name, "Color Reject") == 0))
    {
        gtk_label_set_text(GTK_LABEL(m_labelAdd), "▷");
        gtk_label_set_text(GTK_LABEL(m_labelSub), "◁");
        gtk_widget_set_sensitive(m_btnAdd, FALSE);
        gtk_widget_set_sensitive(m_btnSub, FALSE);
    }
}
