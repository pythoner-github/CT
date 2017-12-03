#include <gtk/gtk.h>
#include "ImgPw.h"
#include "FormatPw.h"
#include "MenuPW.h"
#include "CusSpin.h"
#include "gui_func.h"
#include "gui_global.h"
#include "Def.h"
#include "ImgProcPw.h"
#include "ImageAreaDraw.h"
#include "ModeStatus.h"
#include "../measure/UpdateMeasure.h"
#include "../display/HintArea.h"
#include "../sysMan/SysMeasurementSetting.h"
#include "../keyboard/KeyFunc.h"
#include "SysOptions.h"
#include "SysGeneralSetting.h"
//#include "UpdateMeasure.h"
#include "MeasureCalc.h"
#include "MeasureMan.h"

/***
 * 如果在同一个容器中添加相同的两个Widgets。此时对两个中的任意一个进行了操作切换之后都只会刷新最先加入容器中的widget
 * g_menuPW and g_menuCW 是相同的两个全局变量。
 * 增加到菜单上去的时候却要分成两页，此时对CW的操作不会刷新。
 * 产生的bug是见bugFree 1377 and 1378
 * lhm 2013.10.27
 ***/
MenuPW g_menuPW;
MenuPW g_menuCW;
extern bool g_calcPwStatus;

CusSpin::CusSpinItem MenuPW::item_timeres = {N_("Time Resolution"), "1", OK, ChgTimeRes};
//CusSpin::CusSpinItem MenuPW::item_threshold = {N_("Trace Threshold"), "1", OK, ChgTraceThreshold};
//CusSpin::CusSpinItem MenuPW::item_smooth = {N_("Trace Smooth"), "1", OK, ChgTraceSmooth};

GtkTreeModel *MenuPW::CreateScaleModel()
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

void MenuPW::UpdateScaleModel(GtkTreeModel *model)
{
    GtkTreeIter iter;
    GtkListStore *store = GTK_LIST_STORE(model);

    gtk_tree_model_get_iter_first(model, &iter);
    gtk_list_store_set(store, &iter, 0, _("cm/s"), -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, _("kHz"), -1);
}

GtkTreeModel *MenuPW::CreateTraceTypeModel()
{
	GtkListStore *liststore;
	GtkTreeIter iter;
	liststore = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, _("Max"), -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, _("Mean"), -1);
//	gtk_list_store_append(liststore, &iter);
//	gtk_list_store_set(liststore, &iter, 0, _("Scatter"), -1);

	return GTK_TREE_MODEL(liststore);
}

void MenuPW::UpdateTraceTypeModel(GtkTreeModel *model)
{
    GtkTreeIter iter;
    GtkListStore *store = GTK_LIST_STORE(model);

    gtk_tree_model_get_iter_first(model, &iter);
    gtk_list_store_set(store, &iter, 0, _("Max"), -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, _("Mean"), -1);
//  gtk_tree_model_iter_next(model, &iter);
//  gtk_list_store_set(store, &iter, 0, _("Scatter"), -1);
}

GtkTreeModel *MenuPW::CreateDirectionModel()
{
	GtkListStore *liststore;
	GtkTreeIter iter;
	liststore = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, _("above"), -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, _("below"), -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, _("both"), -1);

	return GTK_TREE_MODEL(liststore);
}

void MenuPW::UpdateDirectionModel(GtkTreeModel *model)
{
    GtkTreeIter iter;
    GtkListStore *store = GTK_LIST_STORE(model);

    gtk_tree_model_get_iter_first(model, &iter);
    gtk_list_store_set(store, &iter, 0, _("above"), -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, _("below"), -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, _("both"), -1);
}


GtkTreeModel *MenuPW::CreateThresholdModel()
{
	GtkListStore *liststore;
	GtkTreeIter iter;
	liststore = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "0", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "1", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "2", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "3", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "4", -1);
/*	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "5", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "6", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "7", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "8", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "9", -1);
*/
	return GTK_TREE_MODEL(liststore);
}

void MenuPW::UpdateThresholdModel(GtkTreeModel *model)
{
    GtkTreeIter iter;
    GtkListStore *store = GTK_LIST_STORE(model);

    gtk_tree_model_get_iter_first(model, &iter);
    gtk_list_store_set(store, &iter, 0, "0", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "1", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "2", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "3", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "4", -1);
/*  gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "5", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "6", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "7", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "8", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "9", -1);
	*/
}

GtkTreeModel *MenuPW::CreateSmoothModel()
{
	GtkListStore *liststore;
	GtkTreeIter iter;
	liststore = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "0", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "1", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "2", -1);
/*	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "3", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "4", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "5", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "6", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "7", -1);
	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set(liststore, &iter, 0, "8", -1);
*/
	return GTK_TREE_MODEL(liststore);
}

void MenuPW::UpdateSmoothModel(GtkTreeModel *model)
{
    GtkTreeIter iter;
    GtkListStore *store = GTK_LIST_STORE(model);

    gtk_tree_model_get_iter_first(model, &iter);
    gtk_list_store_set(store, &iter, 0, "0", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "1", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "2", -1);
/*    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "3", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "4", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "5", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "6", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "7", -1);
    gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, 0, "8", -1);
*/
}

MenuPW::MenuPW(void)
{
	m_table = 0;
}

void MenuPW::Hide(void)
{
    gtk_combo_box_popdown(GTK_COMBO_BOX(m_comboScale));
    gtk_combo_box_popdown(GTK_COMBO_BOX(m_comboTraceType));
    gtk_combo_box_popdown(GTK_COMBO_BOX(m_comboDirection));
    gtk_combo_box_popdown(GTK_COMBO_BOX(m_comboThreshold));
    gtk_combo_box_popdown(GTK_COMBO_BOX(m_comboSmooth));
    gtk_widget_hide_all(m_table);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_chkBtnAutoCalc)))
    {
        ImgProcPw::GetInstance()->SetAutoCalc(FALSE);
        //UpdateAutoCalc(FALSE);
    }
}

void MenuPW::Show(void)
{
    gtk_widget_show_all(m_table);

    gtk_widget_hide(m_btn_angle);
    gtk_widget_hide(m_labelAngle);
    gtk_widget_hide(m_btn_angleAdd);
    gtk_widget_hide(m_btn_angleSub);
    /*
       if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_chkBtnAutoTrace)))
       gtk_widget_hide(m_chkBtnAutoCalc);
     */
    gtk_widget_hide(m_chkBtnAutoTrace);
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_chkBtnAutoCalc)))
    {
        ImgProcPw::GetInstance()->SetAutoCalc(TRUE);
    }


    SysGeneralSetting sysGeneralSetting;
    int index_direction = sysGeneralSetting.GetDirection();
    gtk_combo_box_set_active (GTK_COMBO_BOX (m_comboDirection), index_direction);
    int index_scaleunit = sysGeneralSetting.GetScaleUnit();
    gtk_combo_box_set_active (GTK_COMBO_BOX (m_comboScale), index_scaleunit);
    int index_tracetype = sysGeneralSetting.GetTraceType();
    gtk_combo_box_set_active (GTK_COMBO_BOX (m_comboTraceType), index_tracetype);
    int index_threshold = sysGeneralSetting.GetThreshold();
    gtk_combo_box_set_active (GTK_COMBO_BOX (m_comboThreshold), index_threshold);
    int index_smooth = sysGeneralSetting.GetSmooth();
    gtk_combo_box_set_active (GTK_COMBO_BOX (m_comboSmooth), index_smooth);
}

bool MenuPW::GetAutoTraceStatus(void)
{
    return (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_chkBtnAutoCalc)));
}

void MenuPW::ForeachWidget(GtkWidget *widget)
{
	if(widget != m_btnScale && widget != m_comboScale
		&& widget != m_chkBtnAutoTrace && widget != m_chkBtnAutoCalc)
		gtk_widget_set_sensitive(widget, (bool)m_sensitive);
}

void MenuPW::Sensitive(bool on)
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
GtkWidget* MenuPW::Create(void)
{
	char path[256];
    m_table = gtk_table_new(20, 8, TRUE);

    // Time Mark
    m_labelTimemark = create_label("", 0, 0, g_lightGray, NULL);
    m_check_btn_timemark = gtk_check_button_new();
    gtk_container_add(GTK_CONTAINER(m_check_btn_timemark), m_labelTimemark);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_check_btn_timemark, 0, 8, 0, 1);
    g_signal_connect(m_check_btn_timemark, "clicked", G_CALLBACK(HandleChkTimeMark), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_check_btn_timemark), FALSE);
    gtk_widget_show(m_check_btn_timemark);

    // Auto Calculate
    m_labelAutoCalc = create_label("", 0, 0, g_lightGray, NULL);
    m_chkBtnAutoCalc = gtk_check_button_new();
    gtk_container_add(GTK_CONTAINER(m_chkBtnAutoCalc), m_labelAutoCalc);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_chkBtnAutoCalc, 0, 8, 1, 2);
    g_signal_connect(m_chkBtnAutoCalc, "clicked", G_CALLBACK(HandleChkAutoCalc), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_chkBtnAutoCalc), FALSE);
    gtk_widget_show(m_chkBtnAutoCalc);

    // Trace status
    m_labelAutoTrace = create_label("", 0, 0, g_lightGray, NULL);
    m_chkBtnAutoTrace = gtk_check_button_new();
    gtk_container_add(GTK_CONTAINER(m_chkBtnAutoTrace), m_labelAutoTrace);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_chkBtnAutoTrace, 0, 8, 2, 3);
    g_signal_connect(m_chkBtnAutoTrace, "clicked", G_CALLBACK(HandleChkAutoTrace), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_chkBtnAutoTrace), FALSE);
    gtk_widget_hide(m_chkBtnAutoTrace);
	
	// Fast Angle
    m_labelAngle = create_label("", 0, 0, g_lightGray, NULL);
    m_btn_angle = create_button(m_labelAngle, 0, 0, g_deep);
	gtk_widget_modify_bg(m_btn_angle, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_btn_angle, 0, 6, 4, 5);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_btn_angle), FALSE);
	
    GtkWidget *label_angleSub = create_label("-", 0, 0, g_lightGray, NULL);
    m_btn_angleSub = create_button(label_angleSub, 0, 0, g_deep);
	gtk_widget_modify_bg(m_btn_angleSub, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_btn_angleSub, 6, 7, 4, 5);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_btn_angleSub), FALSE);
    g_signal_connect(m_btn_angleSub, "clicked", G_CALLBACK(HandleBtnAngleSub), this);

    GtkWidget *label_angleAdd = create_label("+", 0, 0, g_lightGray, NULL);
    m_btn_angleAdd = create_button(label_angleAdd, 0, 0, g_deep);
	gtk_widget_modify_bg(m_btn_angleAdd, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_btn_angleAdd, 7, 8, 4, 5);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_btn_angleAdd), FALSE);
    g_signal_connect(m_btn_angleAdd, "clicked", G_CALLBACK(HandleBtnAngleAdd), this);

    // Time Resolution
    spin_timeres.SetItem(&item_timeres);
    GtkWidget *table_timeres= spin_timeres.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_timeres, 0, 8, 5, 6);
    spin_timeres.Show();

    // Scale Unit
    m_labelScale = create_label("", 0, 0, g_lightGray, NULL);
    m_btnScale = create_button(m_labelScale, 0, 0, g_deep);
	gtk_widget_modify_bg(m_btnScale, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_btnScale, 0, 5, 6, 7);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_btnScale), FALSE);

    m_modelScale = CreateScaleModel();
    m_comboScale = create_combobox(0, 0, "text", m_modelScale);
    SysGeneralSetting sysGeneralSetting;
    int scaleunit = sysGeneralSetting.GetScaleUnit();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboScale), scaleunit);

    gtk_table_attach_defaults(GTK_TABLE(m_table), m_comboScale, 5, 8, 6, 7);
    g_signal_connect(m_comboScale, "changed", G_CALLBACK(HandleComboScaleChanged), this);
	
    // Auto Trace-Type
    m_labelTraceType = create_label("", 0, 0, g_lightGray, NULL);
    GtkWidget *btn_tracetype= create_button(m_labelTraceType, 0, 0, g_deep);
	gtk_widget_modify_bg(btn_tracetype, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), btn_tracetype, 0, 5, 7, 8);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_tracetype), FALSE);

    m_modelTraceType = CreateTraceTypeModel();
    m_comboTraceType = create_combobox(0, 0, "text", m_modelTraceType);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_comboTraceType, 5, 8, 7, 8);
    g_signal_connect(m_comboTraceType, "changed", G_CALLBACK(HandleComboTraceTypeChanged), this);

    // Auto Trace-Direction
    m_labelDirection = create_label("", 0, 0, g_lightGray, NULL);
    GtkWidget *btn_direction = create_button(m_labelDirection, 0, 0, g_deep);
	gtk_widget_modify_bg(btn_direction, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), btn_direction, 0, 5, 8, 9);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_direction), FALSE);

    m_modelDirection = CreateDirectionModel();
    m_comboDirection = create_combobox(0, 0, "text", m_modelDirection);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_comboDirection, 5, 8, 8, 9);
    g_signal_connect(m_comboDirection, "changed", G_CALLBACK(HandleComboDirectionChanged), this);
	
    // Auto Trace-Threshold
    m_labelThreshold = create_label("", 0, 0, g_lightGray, NULL);
    GtkWidget *btn_threshold = create_button(m_labelThreshold, 0, 0, g_deep);
	gtk_widget_modify_bg(btn_threshold, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), btn_threshold, 0, 5, 9, 10);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_threshold), FALSE);

    m_modelThreshold = CreateThresholdModel();
    m_comboThreshold = create_combobox(0, 0, "text", m_modelThreshold);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_comboThreshold, 5, 8, 9, 10);
    g_signal_connect(m_comboThreshold, "changed", G_CALLBACK(HandleComboThresholdChanged), this);
	
    // Auto Trace-Smooth
    m_labelSmooth = create_label("", 0, 0, g_lightGray, NULL);
    GtkWidget *btn_smooth = create_button(m_labelSmooth, 0, 0, g_deep);
	gtk_widget_modify_bg(btn_smooth, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), btn_smooth, 0, 5, 10, 11);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_smooth), FALSE);

    m_modelSmooth = CreateSmoothModel();
    m_comboSmooth = create_combobox(0, 0, "text", m_modelSmooth);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_comboSmooth, 5, 8, 10, 11);
    gtk_combo_box_set_active (GTK_COMBO_BOX (m_comboSmooth), 1);
    g_signal_connect(m_comboSmooth, "changed", G_CALLBACK(HandleComboSmoothChanged), this);
	
    // Display Format
    m_frameFormat = gtk_frame_new("");
    gtk_frame_set_label_align(GTK_FRAME(m_frameFormat), 0.5, 0.5);
    gtk_frame_set_shadow_type(GTK_FRAME (m_frameFormat), GTK_SHADOW_IN);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_frameFormat, 0, 8, 12, 17);

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
	
    UpdateLabel();

    return m_table;
}

void MenuPW::UpdateLabel(void)
{
    gtk_label_set_text(GTK_LABEL(m_labelTimemark), _("Time Mark"));
    gtk_label_set_text(GTK_LABEL(m_labelAutoTrace), _("Auto Trace"));
    gtk_label_set_text(GTK_LABEL(m_labelAutoCalc), _("Auto Calculate"));
    // gtk_label_set_text(GTK_LABEL(m_labelAdjust), _("adjust"));
    gtk_label_set_text(GTK_LABEL(m_labelAngle), _("Fast Angle"));
    spin_timeres.UpdateLabel();
    gtk_label_set_text(GTK_LABEL(m_labelScale), _("Scale Unit"));
    UpdateScaleModel(m_modelScale);
    gtk_label_set_text(GTK_LABEL(m_labelTraceType), _("Trace-Type"));
    UpdateTraceTypeModel(m_modelTraceType);
    gtk_label_set_text(GTK_LABEL(m_labelDirection), _("Trace-Direction"));
    UpdateDirectionModel(m_modelDirection);
    gtk_label_set_text(GTK_LABEL(m_labelThreshold), _("Trace-Threshold"));
    UpdateThresholdModel(m_modelThreshold);
    gtk_label_set_text(GTK_LABEL(m_labelSmooth), _("Trace-Smooth"));
    UpdateSmoothModel(m_modelSmooth);
    gtk_frame_set_label(GTK_FRAME(m_frameFormat), _("Display Format"));
}

void MenuPW::UpdateTimeRes(const char* str, EKnobReturn status)
{
	spin_timeres.SetValue(str, status);
}

void MenuPW::UpdateAutoTrace(bool on)
{
    //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_chkBtnAutoTrace), on);
}

void MenuPW::UpdateAutoCalc(bool on)
{
    ImgProcPw::GetInstance()->SetAutoCalc(on);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_chkBtnAutoCalc), on);
}

void MenuPW::UpdateTimeMark(bool on)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_check_btn_timemark),on);
}

///> private
void MenuPW::ChgTimeRes(EKnobOper oper)
{
	ImgProcPw::GetInstance()->ChangeTimeSmooth(oper);
}

void MenuPW::ChkAutoTraceClicked(GtkToggleButton *togglebutton)
{
    gboolean value = gtk_toggle_button_get_active(togglebutton);
      
    // if (!value) {
    //     UpdateMeasure update;
    //     update.ClearLast();
    //     ImageArea::GetInstance()->ClearTrace();
    // }
    if (value) {
        gtk_widget_show(m_chkBtnAutoCalc);
    } else {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_chkBtnAutoCalc), FALSE);
        gtk_widget_hide(m_chkBtnAutoCalc);
        ImageArea::GetInstance()->ClearTrace();
    }
    ImgProcPw::GetInstance()->SetTraceStatus(value);
    // ImgProcPw::GetInstance()->SetDscTraceStatus(TRUE);
}

void MenuPW::ChkTimeMarkClicked(GtkToggleButton *togglebutton)
{
    gboolean value = gtk_toggle_button_get_active(togglebutton);
    ImgProcPw::GetInstance()->ChangeTimeMark(value);
}

void MenuPW::ChkAutoCalcClicked(GtkToggleButton *togglebutton)
{
    MeasureCalc calc;
    UpdateMeasure update;

    gboolean value = gtk_toggle_button_get_active(togglebutton);

    ImgProcPw::GetInstance()->SetTraceStatus(value);

    if(ModeStatus::IsUnFreezeMode())
        ImgProcPw::GetInstance()->SetAutoCalc(value);

    if(!value)
    {
        ImgProcPw::GetInstance()->SetAutoCalc(value);

        //测量增加到测量管理中
        if(g_calcPwStatus)
        {
            update.ClearLast();
            MeasureMan::GetInstance()->ClearLast();
            POINT psP, edP;
            psP.x = edP.x = 0;
            psP.y = edP.y = IMAGE_H/2;
            ImageAreaDraw::GetInstance()->SetPsEdValue(psP, edP, 1);
        }
        else
            ImageAreaDraw::GetInstance()->ClearAutoCalcPara(); //只是显示，没有增加到测量管理中

        ImageArea::GetInstance()->ClearTrace();
    }
}

void MenuPW::BtnAdjustClicked(GtkButton *button)
{
    PRINTF("HIGH-Q adjust clicked\n");
}

void MenuPW::BtnAngleSubClicked(GtkButton *button)
{
	ImgPw::GetInstance()->ChangeCorrectAngleFast(SUB);
}

void MenuPW::BtnAngleAddClicked(GtkButton *button)
{
	ImgPw::GetInstance()->ChangeCorrectAngleFast(ADD);
}	

void MenuPW::BtnFormat1Clicked(GtkButton *button)
{
    if (ModeStatus::IsUnFreezeMode())
    {
        if(StatusChangeFormat())
        {
            FormatPw::GetInstance()->ChangeFormat(FormatPw::P_TOTAL);
            WriteFormatPw(FormatPw::P_TOTAL);
        }
        else 
            HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in current mode."), 2);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
    }
}

void MenuPW::BtnFormat2Clicked(GtkButton *button)
{
    if (ModeStatus::IsUnFreezeMode())
    {
        if(StatusChangeFormat())
        {
            FormatPw::GetInstance()->ChangeFormat(FormatPw::BP11_UD);
            WriteFormatPw(FormatPw::BP11_UD);
        }
        else 
            HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in current mode."), 2);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
    }
}

void MenuPW::BtnFormat3Clicked(GtkButton *button)
{
    if (ModeStatus::IsUnFreezeMode())
    {
        if(StatusChangeFormat())
        {
            FormatPw::GetInstance()->ChangeFormat(FormatPw::BP21_UD);
            WriteFormatPw(FormatPw::BP21_UD);
        }
        else 
            HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in current mode."), 2);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
    }
}

void MenuPW::BtnFormat4Clicked(GtkButton *button)
{
    if (ModeStatus::IsUnFreezeMode())
    {
        if(StatusChangeFormat())
        {
            FormatPw::GetInstance()->ChangeFormat(FormatPw::BP12_UD);
            WriteFormatPw(FormatPw::BP12_UD);
        }
        else 
            HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in current mode."), 2);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
    }
}

void MenuPW::BtnFormat5Clicked(GtkButton *button)
{
    if (ModeStatus::IsUnFreezeMode())
    {
        if(StatusChangeFormat())
        {
            FormatPw::GetInstance()->ChangeFormat(FormatPw::BP11_LR);
            WriteFormatPw(FormatPw::BP11_LR);
        }
        else 
            HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is disable in current mode."), 2);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Format]: Format change is invalid in freeze mode."), 2);
    }
}

void MenuPW::BtnComboScaleChanged(GtkComboBox *widget)
{
    SysGeneralSetting sysGeneralSetting;
    int scaleunit = gtk_combo_box_get_active(widget);
    sysGeneralSetting.SetScaleUnit(scaleunit);
    sysGeneralSetting.SyncFile();

    ImageAreaDraw *m_ptrImgDraw = ImageAreaDraw::GetInstance();
    m_ptrImgDraw->SetPwScaleUnit(ImageAreaDraw::SCALEUNIT(gtk_combo_box_get_active(widget)));
}

void MenuPW::BtnComboTraceTypeChanged(GtkComboBox *widget)
{
    SysGeneralSetting sysGeneralSetting;
    int tracetype= gtk_combo_box_get_active(widget);
    sysGeneralSetting.SetTraceType(tracetype);
    sysGeneralSetting.SyncFile();

    ImgProcPw::GetInstance()->SetTraceType(gtk_combo_box_get_active(widget));
}
void MenuPW::BtnComboDirectionChanged(GtkComboBox *widget)
{
    SysGeneralSetting sysGeneralSetting;
    int direction = gtk_combo_box_get_active(widget);
    sysGeneralSetting.SetDirection(direction);
    sysGeneralSetting.SyncFile();

    ImgProcPw::GetInstance()->SetTraceDirection(gtk_combo_box_get_active(widget));
}
void MenuPW::BtnComboThresholdChanged(GtkComboBox *widget)
{
    SysGeneralSetting sysGeneralSetting;
    int threshold = gtk_combo_box_get_active(widget);
    sysGeneralSetting.SetThreshold(threshold);
    sysGeneralSetting.SyncFile();

    ImgProcPw::GetInstance()->SetTraceThreshold(gtk_combo_box_get_active(widget));
}
void MenuPW::BtnComboSmoothChanged(GtkComboBox *widget)
{
    SysGeneralSetting sysGeneralSetting;
    int smooth = gtk_combo_box_get_active(widget);
    sysGeneralSetting.SetSmooth(smooth);
    sysGeneralSetting.SyncFile();

	ImgProcPw::GetInstance()->SetTraceSmooth(gtk_combo_box_get_active(widget));
}

bool MenuPW::StatusChangeFormat()
{
	ModeStatus modeStatus;
	ScanMode::EScanMode mode = modeStatus.GetScanMode();
	if ((mode == ScanMode::PW) || (mode == ScanMode::PWCFM) || (mode == ScanMode::PWPDI) || (mode == ScanMode::PW_SIMULT)
			|| (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT)
            || (mode == ScanMode::CW) || (mode == ScanMode::CWCFM) || (mode == ScanMode::CWPDI) ) 
		return TRUE;
	else
		return FALSE;
}

/*@brief: write Pw format to config file
 * @para format[in]: PW format
 */
void MenuPW::WriteFormatPw(int format)
{
    SysOptions sysOpt;
    sysOpt.SetDisplayFormatPW(format);
    sysOpt.SyncFile();
}
