#include <gtk/gtk.h>
#include <string.h>
#include "gui_func.h"
#include "KnobMenu.h"
#include "gui_global.h"
#include "GlobalClassMan.h"
#include "ViewMain.h"
KnobMenu* KnobMenu::m_ptrInstance = NULL;

KnobMenu* KnobMenu::GetInstance()
{
    if (m_ptrInstance == NULL)
	m_ptrInstance  = new KnobMenu;
    return m_ptrInstance;
}

KnobMenu::KnobMenu(void)
{
    m_tableKnob = 0;
    m_knobType = D2;
}

void KnobMenu::SetItem(KnobItem *item, unsigned int total_item, EKnobType type)
{
    m_KnobItem = item;
	if ((total_item % KNOB_NUM) == 0)
		m_MaxLevel = (total_item/ KNOB_NUM) - 1;
	else
		m_MaxLevel = total_item/ KNOB_NUM;
    m_CurLevel = 0;
    m_knobType = type;
    Update();
}

KnobMenu::KnobItem* KnobMenu::GetItem(void)
{
    return m_KnobItem;
}

GtkWidget * KnobMenu::Create(void)
{
#if defined (EMP_322)
   GtkWidget *m_tableKnob = gtk_table_new(1, 20, TRUE);
   m_labelLeft = create_label("≪", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelLeft, GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelLeft, GTK_STATE_ACTIVE, g_lightGray);
   GtkWidget *btn_left = create_button(m_labelLeft, 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), btn_left, 9, 10, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (btn_left, GTK_CAN_FOCUS);
   gtk_widget_modify_bg(btn_left, GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(btn_left, GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(btn_left, GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelKnob[0] = create_label("knob1", 0, 0, g_lightGray, NULL);
   gtk_label_set_justify(GTK_LABEL(m_labelKnob[0]), GTK_JUSTIFY_CENTER);
   gtk_widget_modify_fg(m_labelKnob[0], GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelKnob[0], GTK_STATE_ACTIVE, g_lightGray);
   m_btnKnob[0] = create_button(m_labelKnob[0], 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob),m_btnKnob[0], 0, 3, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (m_btnKnob[0], GTK_CAN_FOCUS);
   gtk_widget_modify_bg(m_btnKnob[0], GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(m_btnKnob[0], GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(m_btnKnob[0], GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelKnob[1] = create_label("knob2", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelKnob[1], GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelKnob[1], GTK_STATE_ACTIVE, g_lightGray);
   gtk_label_set_justify(GTK_LABEL(m_labelKnob[1]), GTK_JUSTIFY_CENTER);
   m_btnKnob[1] = create_button(m_labelKnob[1], 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[1], 3, 6, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (m_btnKnob[1], GTK_CAN_FOCUS);
   gtk_widget_modify_bg(m_btnKnob[1], GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(m_btnKnob[1], GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(m_btnKnob[1], GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelKnob[2] = create_label("knob3", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelKnob[2], GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelKnob[2], GTK_STATE_ACTIVE, g_lightGray);
   gtk_label_set_justify(GTK_LABEL(m_labelKnob[2]), GTK_JUSTIFY_CENTER);
   m_btnKnob[2] = create_button(m_labelKnob[2], 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[2], 6, 9, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (m_btnKnob[2], GTK_CAN_FOCUS);
   gtk_widget_modify_bg(m_btnKnob[2], GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(m_btnKnob[2], GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(m_btnKnob[2], GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelKnob[3] = create_label("knob4", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelKnob[3], GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelKnob[3], GTK_STATE_ACTIVE, g_lightGray);
   gtk_label_set_justify(GTK_LABEL(m_labelKnob[3]), GTK_JUSTIFY_CENTER);
   m_btnKnob[3] = create_button(m_labelKnob[3], 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[3], 11, 14, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (m_btnKnob[3], GTK_CAN_FOCUS);
   gtk_widget_modify_bg(m_btnKnob[3], GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(m_btnKnob[3], GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(m_btnKnob[3], GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelKnob[4] = create_label("knob5", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelKnob[4], GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelKnob[4], GTK_STATE_ACTIVE, g_lightGray);
   gtk_label_set_justify(GTK_LABEL(m_labelKnob[4]), GTK_JUSTIFY_CENTER);
   m_btnKnob[4] = create_button(m_labelKnob[4], 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[4], 14, 17, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (m_btnKnob[4], GTK_CAN_FOCUS);
   gtk_widget_modify_bg(m_btnKnob[4], GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(m_btnKnob[4], GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(m_btnKnob[4], GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelKnob[5] = create_label("knob6", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelKnob[5], GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelKnob[5], GTK_STATE_ACTIVE, g_lightGray);
   gtk_label_set_justify(GTK_LABEL(m_labelKnob[5]), GTK_JUSTIFY_CENTER);
   m_btnKnob[5] = create_button(m_labelKnob[5], 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[5], 17, 20, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (m_btnKnob[5], GTK_CAN_FOCUS);
   gtk_widget_modify_bg(m_btnKnob[5], GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(m_btnKnob[5], GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(m_btnKnob[5], GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelRight = create_label("≫", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelRight, GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelRight, GTK_STATE_ACTIVE, g_lightGray);
   GtkWidget *btn_right = create_button(m_labelRight, 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), btn_right, 10, 11, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (btn_right, GTK_CAN_FOCUS);
   gtk_widget_modify_bg(btn_right, GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(btn_right, GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(btn_right, GTK_STATE_INSENSITIVE, g_deepGray);

   gtk_widget_set_usize(m_tableKnob, WIDTH_KNOB_MENU, HEIGHT_KNOB_MENU);
#elif defined (EMP_313)
   GtkWidget *m_tableKnob = gtk_table_new(1, 20, TRUE);


   m_labelLeft = create_label("≪", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelLeft, GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelLeft, GTK_STATE_ACTIVE, g_lightGray);
   GtkWidget *btn_left = create_button(m_labelLeft, 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), btn_left, 0, 1, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (btn_left, GTK_CAN_FOCUS);
   gtk_widget_modify_bg(btn_left, GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(btn_left, GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(btn_left, GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelKnob[0] = create_label("knob1", 0, 0, g_lightGray, NULL);
   gtk_label_set_justify(GTK_LABEL(m_labelKnob[0]), GTK_JUSTIFY_CENTER);
   gtk_widget_modify_fg(m_labelKnob[0], GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelKnob[0], GTK_STATE_ACTIVE, g_lightGray);
   m_btnKnob[0] = create_button(m_labelKnob[0], 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob),m_btnKnob[0], 1, 4, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (m_btnKnob[0], GTK_CAN_FOCUS);
   gtk_widget_modify_bg(m_btnKnob[0], GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(m_btnKnob[0], GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(m_btnKnob[0], GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelKnob[1] = create_label("knob2", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelKnob[1], GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelKnob[1], GTK_STATE_ACTIVE, g_lightGray);
   gtk_label_set_justify(GTK_LABEL(m_labelKnob[1]), GTK_JUSTIFY_CENTER);
   m_btnKnob[1] = create_button(m_labelKnob[1], 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[1], 4, 7, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (m_btnKnob[1], GTK_CAN_FOCUS);
   gtk_widget_modify_bg(m_btnKnob[1], GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(m_btnKnob[1], GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(m_btnKnob[1], GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelKnob[2] = create_label("knob3", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelKnob[2], GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelKnob[2], GTK_STATE_ACTIVE, g_lightGray);
   gtk_label_set_justify(GTK_LABEL(m_labelKnob[2]), GTK_JUSTIFY_CENTER);
   m_btnKnob[2] = create_button(m_labelKnob[2], 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[2], 7, 10, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (m_btnKnob[2], GTK_CAN_FOCUS);
   gtk_widget_modify_bg(m_btnKnob[2], GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(m_btnKnob[2], GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(m_btnKnob[2], GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelKnob[3] = create_label("knob4", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelKnob[3], GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelKnob[3], GTK_STATE_ACTIVE, g_lightGray);
   gtk_label_set_justify(GTK_LABEL(m_labelKnob[3]), GTK_JUSTIFY_CENTER);
   m_btnKnob[3] = create_button(m_labelKnob[3], 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[3], 10, 13, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (m_btnKnob[3], GTK_CAN_FOCUS);
   gtk_widget_modify_bg(m_btnKnob[3], GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(m_btnKnob[3], GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(m_btnKnob[3], GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelKnob[4] = create_label("knob5", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelKnob[4], GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelKnob[4], GTK_STATE_ACTIVE, g_lightGray);
   gtk_label_set_justify(GTK_LABEL(m_labelKnob[4]), GTK_JUSTIFY_CENTER);
   m_btnKnob[4] = create_button(m_labelKnob[4], 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[4], 13, 16, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (m_btnKnob[4], GTK_CAN_FOCUS);
   gtk_widget_modify_bg(m_btnKnob[4], GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(m_btnKnob[4], GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(m_btnKnob[4], GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelKnob[5] = create_label("knob6", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelKnob[5], GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelKnob[5], GTK_STATE_ACTIVE, g_lightGray);
   gtk_label_set_justify(GTK_LABEL(m_labelKnob[5]), GTK_JUSTIFY_CENTER);
   m_btnKnob[5] = create_button(m_labelKnob[5], 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[5], 16, 19, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (m_btnKnob[5], GTK_CAN_FOCUS);
   gtk_widget_modify_bg(m_btnKnob[5], GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(m_btnKnob[5], GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(m_btnKnob[5], GTK_STATE_INSENSITIVE, g_deepGray);

   m_labelRight = create_label("≫", 0, 0, g_lightGray, NULL);
   gtk_widget_modify_fg(m_labelRight, GTK_STATE_PRELIGHT, g_lightGray);
   gtk_widget_modify_fg(m_labelRight, GTK_STATE_ACTIVE, g_lightGray);
   GtkWidget *btn_right = create_button(m_labelRight, 0, 0, g_deep);
   gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), btn_right, 19, 20, 0, 1);
   GTK_WIDGET_UNSET_FLAGS (btn_right, GTK_CAN_FOCUS);
   gtk_widget_modify_bg(btn_right, GTK_STATE_PRELIGHT, g_deep);
   gtk_widget_modify_bg(btn_right, GTK_STATE_ACTIVE, g_deep);
   gtk_widget_modify_bg(btn_right, GTK_STATE_INSENSITIVE, g_deepGray);


    gtk_widget_set_usize(m_tableKnob, WIDTH_KNOB_MENU, HEIGHT_KNOB_MENU);
#else

    GtkWidget *m_tableKnob = gtk_table_new(1, 22, TRUE);
    m_labelLeft = create_label("≪", 0, 0, g_lightGray, NULL);
    gtk_widget_modify_fg(m_labelLeft, GTK_STATE_PRELIGHT, g_lightGray);
    gtk_widget_modify_fg(m_labelLeft, GTK_STATE_ACTIVE, g_lightGray);
    GtkWidget *btn_left = create_button(m_labelLeft, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), btn_left, 0, 1, 0, 1);
    GTK_WIDGET_UNSET_FLAGS (btn_left, GTK_CAN_FOCUS);
    gtk_widget_modify_bg(btn_left, GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(btn_left, GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(btn_left, GTK_STATE_INSENSITIVE, g_deepGray);


    m_labelKnob[0] = create_label("knob1", 0, 0, g_lightGray, NULL);
    gtk_label_set_justify(GTK_LABEL(m_labelKnob[0]), GTK_JUSTIFY_CENTER);
    gtk_widget_modify_fg(m_labelKnob[0], GTK_STATE_PRELIGHT, g_lightGray);
    gtk_widget_modify_fg(m_labelKnob[0], GTK_STATE_ACTIVE, g_lightGray);
    m_btnKnob[0] = create_button(m_labelKnob[0], 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[0], 1, 5, 0, 1);
    GTK_WIDGET_UNSET_FLAGS (m_btnKnob[0], GTK_CAN_FOCUS);
    gtk_widget_modify_bg(m_btnKnob[0], GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(m_btnKnob[0], GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(m_btnKnob[0], GTK_STATE_INSENSITIVE, g_deepGray);

    m_labelKnob[1] = create_label("knob2", 0, 0, g_lightGray, NULL);
    gtk_widget_modify_fg(m_labelKnob[1], GTK_STATE_PRELIGHT, g_lightGray);
    gtk_widget_modify_fg(m_labelKnob[1], GTK_STATE_ACTIVE, g_lightGray);
	gtk_label_set_justify(GTK_LABEL(m_labelKnob[1]), GTK_JUSTIFY_CENTER);
	m_btnKnob[1] = create_button(m_labelKnob[1], 0, 0, g_deep);
	gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[1], 5, 9, 0, 1);
	GTK_WIDGET_UNSET_FLAGS (m_btnKnob[1], GTK_CAN_FOCUS);
	gtk_widget_modify_bg(m_btnKnob[1], GTK_STATE_PRELIGHT, g_deep);
	gtk_widget_modify_bg(m_btnKnob[1], GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(m_btnKnob[1], GTK_STATE_INSENSITIVE, g_deepGray);

	m_labelKnob[2] = create_label("knob3", 0, 0, g_lightGray, NULL);
	gtk_widget_modify_fg(m_labelKnob[2], GTK_STATE_PRELIGHT, g_lightGray);
	gtk_widget_modify_fg(m_labelKnob[2], GTK_STATE_ACTIVE, g_lightGray);
	gtk_label_set_justify(GTK_LABEL(m_labelKnob[2]), GTK_JUSTIFY_CENTER);
	m_btnKnob[2] = create_button(m_labelKnob[2], 0, 0, g_deep);
	gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[2], 9, 13, 0, 1);
	GTK_WIDGET_UNSET_FLAGS (m_btnKnob[2], GTK_CAN_FOCUS);
	gtk_widget_modify_bg(m_btnKnob[2], GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(m_btnKnob[2], GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(m_btnKnob[2], GTK_STATE_INSENSITIVE, g_deepGray);

	m_labelKnob[3] = create_label("knob4", 0, 0, g_lightGray, NULL);
	gtk_widget_modify_fg(m_labelKnob[3], GTK_STATE_PRELIGHT, g_lightGray);
	gtk_widget_modify_fg(m_labelKnob[3], GTK_STATE_ACTIVE, g_lightGray);
	gtk_label_set_justify(GTK_LABEL(m_labelKnob[3]), GTK_JUSTIFY_CENTER);
	m_btnKnob[3] = create_button(m_labelKnob[3], 0, 0, g_deep);
	gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[3], 13, 17, 0, 1);
	GTK_WIDGET_UNSET_FLAGS (m_btnKnob[3], GTK_CAN_FOCUS);
	gtk_widget_modify_bg(m_btnKnob[3], GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(m_btnKnob[3], GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(m_btnKnob[3], GTK_STATE_INSENSITIVE, g_deepGray);

	m_labelKnob[4] = create_label("knob5", 0, 0, g_lightGray, NULL);
	gtk_widget_modify_fg(m_labelKnob[4], GTK_STATE_PRELIGHT, g_lightGray);
	gtk_widget_modify_fg(m_labelKnob[4], GTK_STATE_ACTIVE, g_lightGray);
	gtk_label_set_justify(GTK_LABEL(m_labelKnob[4]), GTK_JUSTIFY_CENTER);
	m_btnKnob[4] = create_button(m_labelKnob[4], 0, 0, g_deep);
	gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), m_btnKnob[4], 17, 21, 0, 1);
	GTK_WIDGET_UNSET_FLAGS (m_btnKnob[4], GTK_CAN_FOCUS);
	gtk_widget_modify_bg(m_btnKnob[4], GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(m_btnKnob[4], GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(m_btnKnob[4], GTK_STATE_INSENSITIVE, g_deepGray);

	m_labelRight = create_label("≫", 0, 0, g_lightGray, NULL);
    gtk_widget_modify_fg(m_labelRight, GTK_STATE_PRELIGHT, g_lightGray);
    gtk_widget_modify_fg(m_labelRight, GTK_STATE_ACTIVE, g_lightGray);
    GtkWidget *btn_right = create_button(m_labelRight, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableKnob), btn_right, 21, 22, 0, 1);
    GTK_WIDGET_UNSET_FLAGS (btn_right, GTK_CAN_FOCUS);
   	gtk_widget_modify_bg(btn_right, GTK_STATE_PRELIGHT, g_deep);
    gtk_widget_modify_bg(btn_right, GTK_STATE_ACTIVE, g_deep);
    gtk_widget_modify_bg(btn_right, GTK_STATE_INSENSITIVE, g_deepGray);

    gtk_widget_set_usize(m_tableKnob, WIDTH_KNOB_MENU, HEIGHT_KNOB_MENU);
#endif
    return m_tableKnob;
}

void KnobMenu::PageUp(void)
{
    if (m_CurLevel > 0)
	--m_CurLevel;
    Update();
}

void KnobMenu::PageDown(void)
{
    if (m_CurLevel < m_MaxLevel)
	++m_CurLevel;
    Update();
}

void KnobMenu::Refresh(void)
{
    FormatCfm::EFormatCfm formatCfm = FormatCfm::GetInstance()->GetFormat();
    Format2D::EFormat2D format2D = Format2D::GetInstance()->GetFormat();

    // samples
    DSCCONTROLATTRIBUTES* m_ptrDscPara = DscMan::GetInstance()->GetDscPara();
    bool cfmIsDirection = m_ptrDscPara->dcaCFMIsDirection;

    gtk_widget_queue_draw(m_labelLeft);
    gtk_widget_queue_draw(m_labelRight);
    char type = GlobalClassMan::GetInstance()->GetProbeType();
    int i;
    for (i = 0; i < KNOB_NUM; i ++)
    {
        gtk_widget_queue_draw(m_labelKnob[i]);
        gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_lightGray);

        if((type == 'L') || (type == 'l'))
        {
            if(strcmp(m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Scan Angle") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
            if(strcmp(m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "EFVI") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
        }
        else
        {
            if(strcmp( m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Steer") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
            if(strcmp(m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "TP-View") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
        }
        if((type != 'P') && (type != 'p'))
        {
            if(strcmp( m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Scan Line") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
        }

        if(!cfmIsDirection && (ScanMode::GetInstance()->GetScanMode() != ScanMode::PWPDI))
        {
            if(strcmp( m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Baseline") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
        }

        if((ScanMode::GetInstance()->GetScanMode() == ScanMode::CFM)
                ||(ScanMode::GetInstance()->GetScanMode() == ScanMode::PDI)
                ||(ScanMode::GetInstance()->GetScanMode() == ScanMode::PDI_VS_2D))
        {
            if(strcmp( m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Simult") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
        }
#ifdef EMP_355
        if(ViewMain::GetInstance()->GetModeIsFlag())
        {
            if(strcmp( m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "SV Length") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
            if(strcmp( m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "HPRF") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
            if(strcmp( m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Simult") == 0)
                gtk_widget_modify_fg(m_labelKnob[i], GTK_STATE_NORMAL, g_deepGray);
		}
#endif
    }
}

//◀ ◁ ▶ ▷
void KnobMenu::Display(KnobItem item, char *buf, const char *tmp)
{
    if (strcmp(item.name, "") == 0){
        sprintf(buf, " ");
        return ;
    }

    if (item.status == MIN) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n%s\n◁  %s  ▶", _(item.name), tmp, _(item.value));
        else
            sprintf(buf, "%s\n%s\n◁  %s  ▶", _(item.name), tmp, item.value);
    }

    else if (item.status == MAX) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n%s\n◀  %s  ▷", _(item.name), tmp, _(item.value));
        else
            sprintf(buf, "%s\n%s\n◀  %s  ▷", _(item.name), tmp, item.value);
    }
    else if (item.status == OK) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n%s\n◀  %s  ▶", _(item.name), tmp, _(item.value));
        else
            sprintf(buf, "%s\n%s\n◀  %s  ▶", _(item.name), tmp, item.value);
    }
    else if (item.status == PRESS) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n%s\n【  %s  】", _(item.name), tmp, _(item.value));
        else
            sprintf(buf, "%s\n%s\n【  %s  】", _(item.name), tmp, item.value);
    }
    else if (item.status == ERROR) {
        if (strcmp(item.value, ""))
            sprintf(buf, "<span color='#505050'>%s\n%s\n   %s   </span>", _(item.name), tmp, _(item.value));
        else
            sprintf(buf, "<span color='#505050'>%s\n%s\n   %s   </span>", _(item.name), tmp, item.value);
    }
}

/*void KnobMenu::Display(KnobItem item, char *buf, char *tmp)
{
    if (strcmp(item.name, "") == 0){
	sprintf(buf, " ");
    return ;
    }

    if (item.status == MIN) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n◁  %s  ▶\n%s", _(item.name),  _(item.value),tmp);
        else
            sprintf(buf, "%s\n◁  %s  ▶\n%s", _(item.name),  item.value,tmp);
    }
    else if (item.status == MAX) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n◀  %s  ▷\n%s", _(item.name),  _(item.value),tmp);
        else
            sprintf(buf, "%s\n◀  %s  ▷\n%s", _(item.name),  item.value,tmp);
    }
    else if (item.status == OK) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n◀  %s  ▶\n%s", _(item.name),  _(item.value),tmp);
        else
            sprintf(buf, "%s\n◀  %s  ▶\n%s", _(item.name),  item.value,tmp);
    }
    else if (item.status == PRESS) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n【  %s  】\n%s", _(item.name),  _(item.value),tmp);
        else
            sprintf(buf, "%s\n【  %s  】\n%s", _(item.name),  item.value,tmp);
    }
    else if (item.status == ERROR) {
        if (strcmp(item.value, ""))
            sprintf(buf, "<span color='#505050'>%s\n   %s   </span>\n%s", _(item.name), _(item.value),tmp);
        else
            sprintf(buf, "<span color='#505050'>%s\n   %s   </span>\n%s", _(item.name),  item.value, tmp);
    }
}

void KnobMenu::Display(KnobItem item, char *buf, const char *tmp)
{
    printf("tmp:%s\n", tmp);
    if (strcmp(item.name, "") == 0){
	sprintf(buf, " ");
    return ;
    }

    if (item.status == MIN) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n%s\n◁  %s  ▶", tmp, _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n%s\n◁  %s  ▶", tmp,_(item.name),  item.value);
    }
    else if (item.status == MAX) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n%s\n◀  %s  ▷", tmp,_(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n%s\n◀  %s  ▷", tmp,_(item.name),  item.value);
    }
    else if (item.status == OK) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n%s\n◀  %s  ▶", tmp,_(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n%s\n◀  %s  ▶", tmp,_(item.name),  item.value);
    }
    else if (item.status == PRESS) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n%s\n【  %s  】", tmp,_(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n%s\n【  %s  】", tmp,_(item.name),  item.value);
    }
    else if (item.status == ERROR) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n<span color='#505050'>%s\n   %s   </span>", tmp, _(item.name), _(item.value));
        else
            sprintf(buf, "%s\n<span color='#505050'>%s\n   %s   </span>", tmp,_(item.name),  item.value);
    }
}
*/

/*void KnobMenu::Display(KnobItem item, char *buf)
{
    if (strcmp(item.name, "") == 0){
	sprintf(buf, " ");
    return ;
    }
#if 1
    if (item.status == MIN) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n\n◁  %s  ▶",  _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n\n◁  %s  ▶", _(item.name),  item.value);
    }
    else if (item.status == MAX) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n\n◀  %s  ▷", _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n\n◀  %s  ▷", _(item.name),  item.value);
    }
    else if (item.status == OK) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n\n◀  %s  ▶", _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n\n◀  %s  ▶", _(item.name),  item.value);
    }
    else if (item.status == PRESS) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n\n【  %s  】", _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n\n【  %s  】", _(item.name),  item.value);
    }
    else if (item.status == ERROR) {
        if (strcmp(item.value, ""))
            sprintf(buf, "<span color='#505050'>%s\n\n   %s   </span>",_(item.name), _(item.value));
        else
            sprintf(buf, "<span color='#505050'>%s\n\n   %s   </span>", _(item.name),  item.value);
    }
#else
    if (item.status == MIN) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n◁  %s  ▶",  _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n◁  %s  ▶", _(item.name),  item.value);
    }
    else if (item.status == MAX) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n◀  %s  ▷", _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n◀  %s  ▷", _(item.name),  item.value);
    }
    else if (item.status == OK) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n◀  %s  ▶", _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n◀  %s  ▶", _(item.name),  item.value);
    }
    else if (item.status == PRESS) {
        if (strcmp(item.value, ""))
            sprintf(buf, "%s\n【  %s  】", _(item.name),  _(item.value));
        else
            sprintf(buf, "%s\n【  %s  】", _(item.name),  item.value);
    }
    else if (item.status == ERROR) {
        if (strcmp(item.value, ""))
            sprintf(buf, "<span color='#505050'>%s\n   %s   </span>",_(item.name), _(item.value));
        else
            sprintf(buf, "<span color='#505050'>%s\n   %s   </span>", _(item.name),  item.value);
    }
#endif
}
*/
void KnobMenu::Update(void)
{
    char buf[80];
    char tmp[20];
    memset(buf, 0, sizeof(buf));
    memset(tmp, 0, sizeof(tmp));
    int i;
    for (i = 0; i < KNOB_NUM; i ++)
    {

#ifdef EMP_3410
        if(!CManRegister::GetInstance()->IsAuthorize("eFci"))
            if(strcmp(m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Freq. Compound") == 0)
            {
                m_KnobItem[m_CurLevel * KNOB_NUM + i].name = "";
                strcpy(m_KnobItem[m_CurLevel * KNOB_NUM + i].value,  "");
                m_KnobItem[m_CurLevel * KNOB_NUM + i].status = ERROR;
                m_KnobItem[m_CurLevel * KNOB_NUM + i].pf = NULL;
                m_KnobItem[m_CurLevel * KNOB_NUM + i].pfPress = NULL;
            }
#endif
        if(strcmp(m_KnobItem[m_CurLevel * KNOB_NUM + i].name, "Focus") == 0)
            sprintf(tmp, "%s", _("FocusNum"));
        else
            sprintf(tmp, "");

        Display(m_KnobItem[m_CurLevel * KNOB_NUM + i], buf, tmp);
        //Display(m_KnobItem[m_CurLevel * KNOB_NUM + i], buf);
        gtk_label_set_markup(GTK_LABEL(m_labelKnob[i]), buf);
    }

    if (m_CurLevel == 0 && m_MaxLevel == 0) {
        gtk_label_set_text(GTK_LABEL(m_labelLeft), "");
        gtk_label_set_text(GTK_LABEL(m_labelRight), "");
    }
    else if (m_CurLevel == 0 && m_MaxLevel > 0) {
        gtk_label_set_text(GTK_LABEL(m_labelLeft), "");
        gtk_label_set_text(GTK_LABEL(m_labelRight), ">>");
    }
    else if (m_CurLevel > 0 && m_CurLevel == m_MaxLevel) {
        gtk_label_set_text(GTK_LABEL(m_labelLeft), "<<");
	gtk_label_set_text(GTK_LABEL(m_labelRight), "");
    }
    else if (m_CurLevel > 0 && m_CurLevel < m_MaxLevel) {
	gtk_label_set_text(GTK_LABEL(m_labelLeft), "<<");
	gtk_label_set_text(GTK_LABEL(m_labelRight), ">>");
    }

    Refresh();
}


/*
 * @brief set new knob value, replace the old value
 *
 * @para index[in] index of current knob item in knob menu
 * @para value[in] string of new value
 * @para status[in] status of current item knob
 */
void KnobMenu::SetValue(int index, const char* value, EKnobReturn status)
{
    sprintf(m_KnobItem[index].value, "%s", value);
    m_KnobItem[index].status = status;
	Update();
}

void KnobMenu::Knob1_Screw(int cw)
{
    int index = 0;
	index = KNOB_NUM * m_CurLevel + 0;

    if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR)
	return ;

    if (cw == 1) {
	(*(m_KnobItem[index].pf))(ADD);
    }
    else if (cw == 0) {
	(*(m_KnobItem[index].pf))(SUB);
    }
}

void KnobMenu::Knob2_Screw(int cw)
{
    int index = 0;
	index = KNOB_NUM * m_CurLevel + 1;

    if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR)
	return ;

    if (cw == 1) {
	(*(m_KnobItem[index].pf))(ADD);
    }
    else if (cw == 0) {
	(*(m_KnobItem[index].pf))(SUB);
    }
}
void KnobMenu::Knob3_Screw(int cw)
{
    int index = 0;
	index = KNOB_NUM * m_CurLevel + 2;

    if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR)
	return ;

    if (cw == 1) {
	(*(m_KnobItem[index].pf))(ADD);
    }
    else if (cw == 0) {
	(*(m_KnobItem[index].pf))(SUB);
    }

}
void KnobMenu::Knob4_Screw(int cw)
{
    int index = 0;
	index = KNOB_NUM * m_CurLevel + 3;

    if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR)
	return ;

    if (cw == 1) {
	(*(m_KnobItem[index].pf))(ADD);
    }
    else if (cw == 0) {
	(*(m_KnobItem[index].pf))(SUB);
    }

}
void KnobMenu::Knob5_Screw(int cw)
{
    int index = 0;
	index = KNOB_NUM * m_CurLevel + 4;

    if (m_KnobItem[index].pf == NULL || m_KnobItem[index].status == ERROR)
	return ;

    if (cw == 1) {
	(*(m_KnobItem[index].pf))(ADD);
    }
    else if (cw == 0) {
	(*(m_KnobItem[index].pf))(SUB);
    }
}

void KnobMenu::Knob1_Press(void)
{
    int index = 0;
	index = KNOB_NUM * m_CurLevel + 0;

    if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR)
	return ;

	(*(m_KnobItem[index].pfPress))();
}

void KnobMenu::Knob2_Press(void)
{
    int index = 0;
	index = KNOB_NUM * m_CurLevel + 1;

    if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR)
	return ;

	(*(m_KnobItem[index].pfPress))();

}
void KnobMenu::Knob3_Press(void)
{
    int index = 0;
	index = KNOB_NUM * m_CurLevel + 2;

    if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR)
	return ;

	(*(m_KnobItem[index].pfPress))();

}
void KnobMenu::Knob4_Press(void)
{
    int index = 0;
	index = KNOB_NUM * m_CurLevel + 3;

    if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR)
	return ;

	(*(m_KnobItem[index].pfPress))();
}
void KnobMenu::Knob5_Press(void)
{
    int index = 0;
	index = KNOB_NUM * m_CurLevel + 4;

    if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR)
	return ;

	(*(m_KnobItem[index].pfPress))();
}
void KnobMenu::Knob6_Press(void)
{
    int index = 0;
	index = KNOB_NUM * m_CurLevel + 5;

    if (m_KnobItem[index].pfPress == NULL || m_KnobItem[index].status == ERROR)
	return ;

	(*(m_KnobItem[index].pfPress))();
}
