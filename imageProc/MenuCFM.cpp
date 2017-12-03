#include <gtk/gtk.h>
#include "MenuCFM.h"
#include "Menu2D.h"
#include "gui_func.h"
#include "gui_global.h"
#include "Def.h"
#include "ImgProcCfm.h"
#include "ImgCfm.h"
#include "ScanMode.h"
#include "FreezeMode.h"
#include "ModeStatus.h"
#include "../display/ImageAreaDraw.h"
#include "../keyboard/MultiFuncFactory.h"

MenuCFM g_menuCFM;

CusSpin::CusSpinItem MenuCFM::item_threshold = {N_("Threshold"), "1", OK, ChgThreshold};
CusSpin::CusSpinItem MenuCFM::item_persist = {N_("Persistence"), "1", OK, ChgPersist};
CusSpin::CusSpinItem MenuCFM::item_reject = {N_("Color Reject"), "1", OK, ChgReject};
CusSpin::CusSpinItem MenuCFM::item_smooth = {N_("Smooth"), "1", OK, ChgSmooth};
CusSpin::CusSpinItem MenuCFM::item_artifact = {N_("Artifact"), "1", OK, ChgArtifact};

MenuCFM::MenuCFM(void)
{
	m_table = 0;
}

void MenuCFM::Hide(void)
{
    gtk_combo_box_popdown(GTK_COMBO_BOX(m_comboScale));
    gtk_widget_hide_all(m_table);
}

void MenuCFM::Show(void)
{
    //再此赋值，目的是切换到cfm模式下的时候阈值和彩色抑制不可操作
    spin_threshold.SetItem(&item_threshold);
    spin_reject.SetItem(&item_reject);

    gtk_widget_show_all(m_table);

    //if (ModeStatus::IsPDIMode()) {
     //   gtk_widget_hide(m_btnCompare);
    //}
    if (ModeStatus::IsCFMMode() || ModeStatus::IsCFMCompareMode()) {
        gtk_widget_hide(m_check_btn_PdiDir);
    }
    if (ModeStatus::IsSpectrumColorMode()) {
        gtk_widget_hide(m_btnCompare);
        gtk_widget_hide(m_check_btn_PdiDir);
        gtk_widget_hide(m_btn4BCFM);
    }

    if (ModeStatus::IsPDIMode() || ModeStatus::IsPDICompareMode()) {
        gtk_widget_hide(m_btnScale);
        gtk_widget_hide(m_comboScale);
    }
}

void MenuCFM::ForeachWidget(GtkWidget *widget)
{
	if (widget != m_btn4BCFM &&
	    widget != m_btnCompare &&
	    widget != m_btnScale && widget != m_comboScale)
		gtk_widget_set_sensitive(widget, (bool)m_sensitive);
}

void MenuCFM::Sensitive(bool on)
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

GtkWidget* MenuCFM::Create(void)
{
    m_table = gtk_table_new(20, 8, TRUE);
	
    // Background Hide 
    m_labelBghide = create_label("", 0, 0, g_lightGray, NULL);
    m_check_btn_bghide = gtk_check_button_new();
    gtk_container_add(GTK_CONTAINER(m_check_btn_bghide), m_labelBghide);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_check_btn_bghide, 0, 8, 1, 2);
    g_signal_connect(m_check_btn_bghide, "clicked", G_CALLBACK(HandleChkBgHide), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_check_btn_bghide), FALSE);
    gtk_widget_show(m_check_btn_bghide);

    // B/C
    m_labelBC = create_label("", 0, 0, g_lightGray, NULL);
    m_btnBC = create_button(m_labelBC, 0, 0, g_deep);
    gtk_widget_modify_bg(m_btnBC, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_btnBC, 0, 8, 2, 3);
    g_signal_connect(m_btnBC, "clicked", G_CALLBACK(HandleChkBC), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_btnBC), FALSE);
    gtk_widget_show(m_btnBC);

    // Color Hide 
    m_labelColorhide = create_label("", 0, 0, g_lightGray, NULL);
    m_btnColorhide = create_button(m_labelColorhide, 0, 0, g_deep);
    gtk_widget_modify_bg(m_btnColorhide, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_btnColorhide, 0, 8, 3, 4);
    g_signal_connect(m_btnColorhide, "clicked", G_CALLBACK(HandleChkColorHide), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_btnColorhide), FALSE);
    gtk_widget_show(m_btnColorhide);

    // Scale Unit
    m_labelScale = create_label("", 0, 0, g_lightGray, NULL);
    m_btnScale = create_button(m_labelScale, 0, 0, g_deep);
    gtk_widget_modify_bg(m_btnScale, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_btnScale, 0, 5, 4, 5);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_btnScale), FALSE);

    m_modelScale= CreateScaleModel();
    m_comboScale = create_combobox(0, 0, "text", m_modelScale);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_comboScale, 5, 8, 4, 5);
    g_signal_connect(m_comboScale, "changed", G_CALLBACK(HandleComboScaleChanged), this);

    // Persistence
    spin_persist.SetItem(&item_persist);
    GtkWidget *table_persist= spin_persist.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_persist, 0, 8, 5, 6);
    spin_persist.Show();

    // Threshold
    spin_threshold.SetItem(&item_threshold);
    GtkWidget *table_threshold = spin_threshold.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_threshold, 0, 8, 6, 7);
    spin_threshold.Show();

    // Color Reject
    spin_reject.SetItem(&item_reject);
    GtkWidget *table_reject = spin_reject.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_reject, 0, 8, 7, 8);
    spin_reject.Show();

    // Smooth
    spin_smooth.SetItem(&item_smooth);
    GtkWidget *table_smooth = spin_smooth.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_smooth, 0, 8, 8, 9);
    spin_smooth.Show();

    // Artifact
    spin_artifact.SetItem(&item_artifact);
    GtkWidget *table_artifact = spin_artifact.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_artifact, 0, 8, 9, 10);
    spin_artifact.Show();

#if defined(K24C)
    //m_label4BCFM = create_label("", 0, 0, g_lightGray, NULL);
    //GtkWidget *btn_4B= gtk_button_new();
    //gtk_widget_modify_bg(btn_4B, GTK_STATE_NORMAL, g_deep);
    //gtk_container_add(GTK_CONTAINER(btn_4B), m_label4BCFM);
    //gtk_table_attach_defaults(GTK_TABLE(m_table), btn_4B, 0, 8, 12, 13);
    //g_signal_connect(btn_4B, "clicked", G_CALLBACK(HandleBtn4BCFM), this);
    //gtk_button_set_focus_on_click(GTK_BUTTON(btn_4B), FALSE);
#else
    // 4B
    m_label4BCFM = create_label("", 0, 0, g_lightGray, NULL);
    m_btn4BCFM = create_button(m_label4BCFM, 0, 0, g_deep);
    gtk_widget_modify_bg(m_btn4BCFM, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_btn4BCFM, 0, 8, 12, 13);
    g_signal_connect(m_btn4BCFM, "clicked", G_CALLBACK(HandleBtn4BCFM), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_btn4BCFM), FALSE);

#endif
    // 2D+2D/CFM
    m_labelCompare = create_label("", 0, 0, g_lightGray, NULL);
    m_btnCompare = create_button(m_labelCompare, 0, 0, g_deep);
    gtk_widget_modify_bg(m_btnCompare, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_btnCompare, 0, 8, 13, 14);
    g_signal_connect(m_btnCompare, "clicked", G_CALLBACK(HandleChkCompare), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_btnCompare), FALSE);
    gtk_widget_show(m_btnCompare);

    // enter dir pdi
    m_labelPdiDir = create_label("", 0, 0, g_lightGray, NULL);
    m_check_btn_PdiDir = gtk_check_button_new();
    gtk_container_add(GTK_CONTAINER(m_check_btn_PdiDir), m_labelPdiDir);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_check_btn_PdiDir, 0, 8, 14, 15);
    g_signal_connect(m_check_btn_PdiDir, "clicked", G_CALLBACK(HandleChkPdiDir), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_check_btn_PdiDir), FALSE);
    gtk_widget_show(m_check_btn_PdiDir);
    UpdateLabel();

    return m_table;
}

void MenuCFM::UpdateLabel(void)
{
    gtk_label_set_text(GTK_LABEL(m_labelColorhide), _("Color Hide"));
    gtk_label_set_text(GTK_LABEL(m_labelBghide), _("Background Hide"));
    gtk_label_set_text(GTK_LABEL(m_labelBC), _("B/C"));
    gtk_label_set_text(GTK_LABEL(m_labelCompare), _("2D+2D/Color"));
    gtk_label_set_text(GTK_LABEL(m_label4BCFM), _("4B"));
    gtk_label_set_text(GTK_LABEL(m_labelPdiDir), _("Direction PDI"));
    gtk_label_set_text(GTK_LABEL(m_labelScale), _("Scale Unit"));
    UpdateScaleModel(m_modelScale);
    spin_persist.UpdateLabel();
    spin_threshold.UpdateLabel();
    spin_reject.UpdateLabel();
    spin_smooth.UpdateLabel();
    spin_artifact.UpdateLabel();
}

///> private
GtkTreeModel *MenuCFM::CreateScaleModel()
{
	GtkListStore *liststore;
	GtkTreeIter iter;
	liststore = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, _("cm/s"), -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, _("kHz"), -1);

	return GTK_TREE_MODEL(liststore);
}

void MenuCFM::UpdateScaleModel(GtkTreeModel *model)
{
    GtkTreeIter iter;
    GtkListStore *store = GTK_LIST_STORE(model);

    gtk_tree_model_get_iter_first(model, &iter);
    gtk_list_store_set(store, &iter, 0, _("cm/s"), -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, _("kHz"), -1);
}

void MenuCFM::UpdateBackgroundHide(bool on)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_check_btn_bghide),on);
}
void MenuCFM::UpdateBCWidth(bool on)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_btnBC),on);
}
void MenuCFM::UpdateCfmCompare(bool on)
{
#if 0
	if(GTK_IS_TOGGLE_BUTTON(m_btnCompare))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_btnCompare),on);
#endif
}

void MenuCFM::UpdatePdiDir(bool on)
{
	if(GTK_IS_TOGGLE_BUTTON(m_check_btn_PdiDir))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_check_btn_PdiDir),on);
}

void MenuCFM::UpdateThreshold(const char* str, EKnobReturn status)
{
	spin_threshold.SetValue(str, status);
}
void MenuCFM::UpdatePersist(const char* str, EKnobReturn status)
{
	spin_persist.SetValue(str, status);
}
void MenuCFM::UpdateReject(const char* str, EKnobReturn status)
{
	spin_reject.SetValue(str, status);
}
void MenuCFM::UpdateSmooth(const char* str, EKnobReturn status)
{
	spin_smooth.SetValue(str, status);
}
void MenuCFM::UpdateArtifact(const char* str, EKnobReturn status)
{
	spin_artifact.SetValue(str, status);
}

///> private
void MenuCFM::ChkColorHideClicked(GtkButton *button)
{
	ImgProcCfm::GetInstance()->ChangeColorHide();
}

void MenuCFM::ChkBgHideClicked(GtkButton *button)
{
	gboolean value;
	value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
	ImgProcCfm::GetInstance()->ChangeBackgroundHide(value);
}

void MenuCFM::Chk4BCFM(GtkButton *button)
{
    g_menu2D.Btn4B(button);
   //ScanMode::GetInstance()->EnterCwCfmFromCfm();
}

void MenuCFM::ChkBCClicked(GtkButton *button)
{
    //ScanMode::GetInstance()->UpdateCwCfmFromInit();
	ImgCfm::GetInstance()->BCWidth();
}
void MenuCFM::ChkPdiDirClicked(GtkButton *button)
{
	if (ModeStatus::IsPDIMode() || ModeStatus::IsPDICompareMode())
	{
		gboolean value;
		value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));

		ImgCfm::GetInstance()->SwitchPdiAndDir(value);
	}
}

void MenuCFM::ChkCompareClicked(GtkButton *button)
{
    if (ModeStatus::IsCFMMode())
        ScanMode::GetInstance()->EnterCfmVs2D();
    else if (ModeStatus::IsPDIMode())
        ScanMode::GetInstance()->EnterPdiVs2D();
}

void MenuCFM::BtnComboScaleChanged(GtkComboBox *widget)
{
    ImageAreaDraw::GetInstance()->SetColorScaleUnit((ImageAreaDraw::SCALEUNIT)gtk_combo_box_get_active(widget));
}

void MenuCFM::ChgThreshold(EKnobOper oper)
{
	ImgProcCfm::GetInstance()->ChangeThreshold(oper);
}
void MenuCFM::ChgPersist(EKnobOper oper)
{
	ImgProcCfm::GetInstance()->ChangePersist(oper);
}
void MenuCFM::ChgReject(EKnobOper oper)
{
	ImgProcCfm::GetInstance()->ChangeReject(oper);
}
void MenuCFM::ChgSmooth(EKnobOper oper)
{
	ImgProcCfm::GetInstance()->ChangeSmooth(oper);
}

void MenuCFM::ChgArtifact(EKnobOper oper)
{
	ImgProcCfm::GetInstance()->ChangeArtifact(oper);
}


