#include <gtk/gtk.h>
#include "MenuM.h"
#include "gui_func.h"
#include "gui_global.h"
#include "ImgProc2D.h"
#include "ImgProcM.h"
#include "FormatM.h"
#include "ScanMode.h"
#include "HintArea.h"
#include "SysOptions.h"
#include "ManRegister.h"

MenuM g_menuM;

CusSpin::CusSpinItem  MenuM::item_line = {N_("Line aver."), "1", OK, ChgLine};
CusSpin::CusSpinItem  MenuM::item_smooth = {N_("Smooth"), "1", OK, ChgSmooth};
CusSpin::CusSpinItem  MenuM::item_enhanceM = {N_("Enhance M"), "1", OK, ChgEnhanceM};

MenuM::MenuM(void)
{
    m_format = 1;
}

void MenuM::Hide(void)
{
    gtk_widget_hide_all(m_table);
}

void MenuM::Show(void)
{
    //UpdateDisplayL();
    gtk_widget_show_all(m_table);

    gtk_widget_hide(m_check_btn_polarity);
}

void MenuM::ForeachWidget(GtkWidget *widget)
{
	gtk_widget_set_sensitive(widget, (bool)m_sensitive);
}

void MenuM::Sensitive(bool on)
{
	m_sensitive = on;

	GList *list = NULL;
	list = gtk_container_get_children(GTK_CONTAINER(m_table));
	if(list)
	{
		g_list_foreach(list, (GFunc)(HandleForeachWidget), this);
		g_list_free(list);
	}
}

GtkWidget * MenuM::Create(void)
{
    char path[256];
    m_table = gtk_table_new(20, 1, TRUE);

// Image reverse
    // polarity
    m_labelPolarity = create_label("", 0, 0, g_lightGray, NULL);
    m_check_btn_polarity = gtk_check_button_new();
    gtk_container_add(GTK_CONTAINER(m_check_btn_polarity), m_labelPolarity);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_check_btn_polarity, 0, 1, 0, 1);
    g_signal_connect(m_check_btn_polarity, "clicked", G_CALLBACK(HandleChkPolarity), this);
    gtk_widget_show(m_check_btn_polarity);
    
    // time mark on/off
    m_labelTimemark = create_label("", 0, 0, g_lightGray, NULL);
    m_check_btn_timemark = gtk_check_button_new();
    gtk_container_add(GTK_CONTAINER(m_check_btn_timemark), m_labelTimemark);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_check_btn_timemark, 0, 1, 1, 2);
    g_signal_connect(m_check_btn_timemark, "toggled", G_CALLBACK(HandleChkTimemark), this);
    gtk_widget_show(m_check_btn_timemark);

    spin_line.SetItem(&item_line);
    GtkWidget *table_line = spin_line.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_line, 0, 1, 3, 4);
    spin_line.Show();

    // smooth 1-8
    spin_smooth.SetItem(&item_smooth);
    GtkWidget *table_smooth = spin_smooth.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_smooth, 0, 1, 4, 5);
    spin_smooth.Show();

    // enhance M 
    spin_enhanceM.SetItem(&item_enhanceM);
    GtkWidget *table_enhanceM = spin_enhanceM.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_enhanceM, 0, 1, 5, 6);
    spin_enhanceM.Show();

    //anatomic
#ifdef EMP_3410
    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[1]))
    {
        m_labelAnatomicM = create_label("", 0, 0, g_lightGray, NULL);
        GtkWidget *btn_anatomic = create_button(m_labelAnatomicM, 0, 0, g_deep);
        gtk_widget_modify_bg(btn_anatomic, GTK_STATE_INSENSITIVE, g_deepGray);
        gtk_table_attach_defaults(GTK_TABLE(m_table), btn_anatomic, 0, 1, 7, 8);
        g_signal_connect(btn_anatomic, "clicked", G_CALLBACK(HandleBtnAnatomic), this);
        gtk_button_set_focus_on_click(GTK_BUTTON(btn_anatomic), FALSE);
    }
#else
    m_labelAnatomicM = create_label("", 0, 0, g_lightGray, NULL);
    GtkWidget *btn_anatomic = create_button(m_labelAnatomicM, 0, 0, g_deep);
    gtk_widget_modify_bg(btn_anatomic, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), btn_anatomic, 0, 1, 7, 8);
    g_signal_connect(btn_anatomic, "clicked", G_CALLBACK(HandleBtnAnatomic), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_anatomic), FALSE);
#endif
#if 0
    //display format
    SysOptions sysoption;
    m_format = sysoption.GetDisplayFormatM() + 1;
    m_frameFormat = gtk_frame_new("");
    gtk_frame_set_label_align(GTK_FRAME(m_frameFormat), 0.5, 0.5);
    gtk_frame_set_shadow_type(GTK_FRAME (m_frameFormat), GTK_SHADOW_IN);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_frameFormat, 0, 1, 11, 13);

    sprintf(path, "%sres/btn_format/%d.jpg", CFG_RES_PATH, m_format);
    GtkWidget *image1 = gtk_image_new_from_file(path);
    m_btnFormat = create_button(NULL, 0, 0, g_deep);
    gtk_widget_modify_bg(m_btnFormat, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_widget_set_size_request(m_btnFormat, 1, 2);
    gtk_button_set_image(GTK_BUTTON(m_btnFormat), image1);
    gtk_container_add (GTK_CONTAINER (m_frameFormat), m_btnFormat);
#endif

    // display format
#if 1
    m_frameFormat = gtk_frame_new("");
    gtk_frame_set_label_align(GTK_FRAME(m_frameFormat), 0.5, 0.5);
    gtk_frame_set_shadow_type(GTK_FRAME (m_frameFormat), GTK_SHADOW_IN);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_frameFormat, 0, 1, 11, 16);
    
    GtkWidget *tableFormat = gtk_table_new (2, 3, TRUE);
    gtk_container_add (GTK_CONTAINER (m_frameFormat), tableFormat);
    gtk_container_set_border_width (GTK_CONTAINER (tableFormat), 2);
    gtk_table_set_row_spacings (GTK_TABLE (tableFormat), 2);
    gtk_table_set_col_spacings (GTK_TABLE (tableFormat), 2);

    sprintf(path, "%s/%s", CFG_RES_PATH, "res/btn_format/1.jpg");
    GtkWidget *image1 = gtk_image_new_from_file(path);
    GtkWidget *btn_format1 = create_button(NULL, 0, 0, g_deep);
    gtk_widget_modify_bg(btn_format1, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_button_set_image(GTK_BUTTON(btn_format1), image1);
    gtk_table_attach_defaults(GTK_TABLE(tableFormat), btn_format1, 0, 1, 0, 1);
    g_signal_connect(btn_format1, "clicked", G_CALLBACK(HandleBtnFormat1), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_format1), FALSE);

    sprintf(path, "%s/%s", CFG_RES_PATH, "res/btn_format/2.jpg");
    GtkWidget *image2 = gtk_image_new_from_file(path);
    GtkWidget *btn_format2 = create_button(NULL, 0, 0, g_deep);
    gtk_widget_modify_bg(btn_format2, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_button_set_image(GTK_BUTTON(btn_format2), image2);
    gtk_table_attach_defaults(GTK_TABLE(tableFormat), btn_format2, 1, 2, 0, 1);
    g_signal_connect(btn_format2, "clicked", G_CALLBACK(HandleBtnFormat2), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_format2), FALSE);

    sprintf(path, "%s/%s", CFG_RES_PATH, "res/btn_format/3.jpg");
    GtkWidget *image3 = gtk_image_new_from_file(path);
    GtkWidget *btn_format3 = create_button(NULL, 0, 0, g_deep);
    gtk_widget_modify_bg(btn_format3, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_button_set_image(GTK_BUTTON(btn_format3), image3);
    gtk_table_attach_defaults(GTK_TABLE(tableFormat), btn_format3, 2, 3, 0, 1);
    g_signal_connect(btn_format3, "clicked", G_CALLBACK(HandleBtnFormat3), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_format3), FALSE);

    sprintf(path, "%s/%s", CFG_RES_PATH, "res/btn_format/4.jpg");
    GtkWidget *image4 = gtk_image_new_from_file(path);
    GtkWidget *btn_format4 = create_button(NULL, 0, 0, g_deep);
    gtk_widget_modify_bg(btn_format4, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_button_set_image(GTK_BUTTON(btn_format4), image4);
    gtk_table_attach_defaults(GTK_TABLE(tableFormat), btn_format4, 0, 1, 1, 2);
    g_signal_connect(btn_format4, "clicked", G_CALLBACK(HandleBtnFormat4), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_format4), FALSE);

    sprintf(path, "%s/%s", CFG_RES_PATH, "res/btn_format/5.jpg");
    GtkWidget *image5 = gtk_image_new_from_file(path);
    GtkWidget *btn_format5 = create_button(NULL, 0, 0, g_deep);
    gtk_widget_modify_bg(btn_format5, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_button_set_image(GTK_BUTTON(btn_format5), image5);
    gtk_table_attach_defaults(GTK_TABLE(tableFormat), btn_format5, 1, 2, 1, 2);
    g_signal_connect(btn_format5, "clicked", G_CALLBACK(HandleBtnFormat5), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_format5), FALSE);
#endif
    UpdateLabel();

    return m_table;
}

void MenuM::UpdateLabel(void)
{
    gtk_label_set_text(GTK_LABEL(m_labelPolarity), _("Polarity"));
    gtk_label_set_text(GTK_LABEL(m_labelTimemark), _("Time Mark"));
    spin_line.UpdateLabel();
    spin_smooth.UpdateLabel();
    spin_enhanceM.UpdateLabel();
#ifdef EMP_3410
    if(CManRegister::GetInstance()->IsAuthorize("Anatomic M"))
        gtk_label_set_text(GTK_LABEL(m_labelAnatomicM), _("Anatomic M"));
#else
    gtk_label_set_text(GTK_LABEL(m_labelAnatomicM), _("Anatomic M"));
#endif
    gtk_frame_set_label(GTK_FRAME(m_frameFormat), _("Display Format"));
}

void MenuM::UpdatePolarity(bool on)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_check_btn_polarity),on);
}
void MenuM::UpdateTimeMark(bool on)
{
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_check_btn_timemark), on);
}
void MenuM::UpdateLine(const char* str, EKnobReturn status)
{
    spin_line.SetValue(str, status);
}

void MenuM::UpdateEnhanceM(const char* str, EKnobReturn status)
{
    spin_enhanceM.SetValue(str, status);
}

void MenuM::UpdateSmooth(const char* str, EKnobReturn status)
{
    spin_smooth.SetValue(str, status);
}

void MenuM::BtnAnatomic(GtkButton *button)
{
	ScanMode::GetInstance()->EnterAnatomicM();
}

///> private
void MenuM::ChkPolarity(GtkButton *button)
{
	gboolean value;
	value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
	ImgProc2D::GetInstance()->ChangePolarity(value);
}
void MenuM::ChkTimemark(GtkButton *button)
{
	gboolean value;
	value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
	ImgProcM::GetInstance()->ChangeTimeMark(value);
}

void MenuM::ChgLine(EKnobOper oper)
{
	ImgProc2D::GetInstance()->ChangeLineAver(oper);
}

void MenuM::ChgEnhanceM(EKnobOper oper)
{
	ImgProcM::GetInstance()->ChangeEnhanceM(oper);
}

void MenuM::ChgSmooth(EKnobOper oper)
{
	ImgProcM::GetInstance()->ChangeMSmooth(oper);
}

//lhm
void  MenuM::UpdateDisplayL(void)
{
#if 0
    char path[256];
    SysOptions sysoption;
    m_format = sysoption.GetDisplayFormatM() + 1;
    sprintf(path, "%sres/btn_format/%d.jpg", CFG_RES_PATH, m_format);
    GtkWidget *image1 = gtk_image_new_from_file(path);
    gtk_widget_modify_bg(m_btnFormat, GTK_STATE_INSENSITIVE, g_deep);
    gtk_button_set_image(GTK_BUTTON(m_btnFormat), image1);
    g_signal_connect(m_btnFormat, "clicked", G_CALLBACK(HandleBtnFormat), this);
#endif
}

void MenuM::BtnFormatClicked(GtkButton *button)
{
    if (ModeStatus::IsUnFreezeMode())
    {
        if (ModeStatus::IsMImgMode())
            FormatM::GetInstance()->ChangeFormat(FormatM::EFormatM(m_format - 1));
        else
            HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in current mode."), 2);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
    }
}

void MenuM::BtnFormat1Clicked(GtkButton *button)
{
    if (ModeStatus::IsUnFreezeMode())
    {
        if (ModeStatus::IsMImgMode())
        {
            FormatM::GetInstance()->ChangeFormat(FormatM::M_TOTAL);
            WriteFormatM(FormatM::M_TOTAL);
        }
        else
            HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in current mode."), 2);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
    }
}

void MenuM::BtnFormat2Clicked(GtkButton *button)
{
    if (ModeStatus::IsUnFreezeMode())
    {
        FormatM::GetInstance()->ChangeFormat(FormatM::BM11_UD);
        WriteFormatM(FormatM::BM11_UD);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
    }
}

void MenuM::BtnFormat3Clicked(GtkButton *button)
{
    if (ModeStatus::IsUnFreezeMode())
    {
        FormatM::GetInstance()->ChangeFormat(FormatM::BM21_UD);
        WriteFormatM(FormatM::BM21_UD);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
    }    
}

void MenuM::BtnFormat4Clicked(GtkButton *button)
{
    if (ModeStatus::IsUnFreezeMode())
    {
        FormatM::GetInstance()->ChangeFormat(FormatM::BM12_UD);
        WriteFormatM(FormatM::BM12_UD);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
    }    
}

void MenuM::BtnFormat5Clicked(GtkButton *button)
{
    if (ModeStatus::IsUnFreezeMode())
    {
        FormatM::GetInstance()->ChangeFormat(FormatM::BM11_LR);
        WriteFormatM(FormatM::BM11_LR);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
    }    
}

void MenuM::WriteFormatM(int format)
{
    SysOptions sp;
    //write to config file
    sp.SetDisplayFormatM(format);
    sp.SyncFile();
}
