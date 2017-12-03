// -*- c++ -*-
#ifndef MENU_PW_SUB_H
#define MENU_PW_SUB_H

#define WIDTH_PW_MENU  220
#define HEIGHT_PW_MENU  635

#include "Def.h"
#include "CusSpin.h"

class MenuPW
{
public:
    MenuPW();
    ~MenuPW(){};
    void Hide(void);
    void Show(void);
    bool GetAutoTraceStatus(void);
    GtkWidget * Create(void);

	void Sensitive(bool on);
    void UpdateTimeRes(const char* str, EKnobReturn status);
    void UpdateAutoTrace(bool on);
    void UpdateAutoCalc(bool on);
    void UpdateTimeMark(bool on);
    void UpdateLabel(void);

private:
    GtkWidget *m_table;
    GtkWidget *m_labelTimemark;
    GtkWidget *m_check_btn_timemark;
    GtkWidget *m_labelAutoTrace;
    GtkWidget *m_chkBtnAutoTrace;
    GtkWidget *m_labelAutoCalc;
    GtkWidget *m_chkBtnAutoCalc;
    GtkWidget *m_labelAdjust;
    GtkWidget *m_btn_angle;
    GtkWidget *m_labelAngle;
    GtkWidget *m_btn_angleSub;
    GtkWidget *m_btn_angleAdd;
    CusSpin spin_timeres;
    GtkWidget *m_labelScale;
    GtkWidget *m_btnScale;
    GtkWidget *m_labelTraceType;
    GtkWidget *m_labelThreshold;
    GtkWidget *m_labelDirection;
    GtkWidget *m_labelSmooth;
    GtkWidget *m_frameFormat;

    GtkTreeModel *m_modelScale;
    GtkTreeModel *m_modelTraceType;
    GtkTreeModel *m_modelDirection;
    GtkTreeModel *m_modelThreshold;
    GtkTreeModel *m_modelSmooth;

    GtkWidget *m_comboScale;
    GtkWidget *m_comboTraceType;
    GtkWidget *m_comboDirection;
    GtkWidget *m_comboThreshold;
    GtkWidget *m_comboSmooth;

    void UpdateScaleModel(GtkTreeModel *model);
    void UpdateTraceTypeModel(GtkTreeModel *model);
    void UpdateDirectionModel(GtkTreeModel *model);
    void UpdateThresholdModel(GtkTreeModel *model);
    void UpdateSmoothModel(GtkTreeModel *model);

	static CusSpin::CusSpinItem item_timeres;
//	static CusSpin::CusSpinItem item_threshold;
//	static CusSpin::CusSpinItem item_smooth;

	bool m_sensitive;

	static void ChgTimeRes(EKnobOper oper);
	static GtkTreeModel *CreateScaleModel();
	static GtkTreeModel *CreateTraceTypeModel();
	static GtkTreeModel *CreateDirectionModel();
	static GtkTreeModel *CreateThresholdModel();
	static GtkTreeModel *CreateSmoothModel();

//signal handle
    void ChkAutoTraceClicked(GtkToggleButton *togglebutton);
    void ChkTimeMarkClicked(GtkToggleButton *togglebutton);
    void ChkAutoCalcClicked(GtkToggleButton *togglebutton);
	void BtnAdjustClicked(GtkButton *button);
	void BtnAngleSubClicked(GtkButton *button);
	void BtnAngleAddClicked(GtkButton *button);
	void BtnFormat1Clicked(GtkButton *button);
	void BtnFormat2Clicked(GtkButton *button);
	void BtnFormat3Clicked(GtkButton *button);
	void BtnFormat4Clicked(GtkButton *button);
	void BtnFormat5Clicked(GtkButton *button);
	void BtnComboScaleChanged(GtkComboBox *widget);
	void BtnComboTraceTypeChanged(GtkComboBox *widget);
	void BtnComboDirectionChanged(GtkComboBox *widget);
	void BtnComboThresholdChanged(GtkComboBox *widget);
	void BtnComboSmoothChanged(GtkComboBox *widget);
    void ForeachWidget(GtkWidget *widget);

	bool StatusChangeFormat();
    void WriteFormatPw(int format);

//signal connect
    static void HandleChkAutoTrace(GtkToggleButton *togglebutton, MenuPW *data) { data->ChkAutoTraceClicked(togglebutton); }
    static void HandleChkTimeMark(GtkToggleButton *togglebutton, MenuPW *data) { data->ChkTimeMarkClicked(togglebutton); }
    static void HandleChkAutoCalc(GtkToggleButton *togglebutton, MenuPW *data) { data->ChkAutoCalcClicked(togglebutton); }
    static void HandleBtnAdjust(GtkButton *button, MenuPW *data) { data->BtnAdjustClicked(button); }
    static void HandleBtnAngleSub(GtkButton *button, MenuPW *data) { data->BtnAngleSubClicked(button); }
    static void HandleBtnAngleAdd(GtkButton *button, MenuPW *data) { data->BtnAngleAddClicked(button); }
    static void HandleBtnFormat1(GtkButton *button, MenuPW *data) { data->BtnFormat1Clicked(button); }
    static void HandleBtnFormat2(GtkButton *button, MenuPW *data) { data->BtnFormat2Clicked(button); }
    static void HandleBtnFormat3(GtkButton *button, MenuPW *data) { data->BtnFormat3Clicked(button); }
    static void HandleBtnFormat4(GtkButton *button, MenuPW *data) { data->BtnFormat4Clicked(button); }
    static void HandleBtnFormat5(GtkButton *button, MenuPW *data) { data->BtnFormat5Clicked(button); }
    static void HandleComboScaleChanged(GtkComboBox *widget, MenuPW *data) { data->BtnComboScaleChanged(widget); }
    static void HandleComboTraceTypeChanged(GtkComboBox *widget, MenuPW *data) { data->BtnComboTraceTypeChanged(widget); }
    static void HandleComboDirectionChanged(GtkComboBox *widget, MenuPW *data) { data->BtnComboDirectionChanged(widget); }
    static void HandleComboThresholdChanged(GtkComboBox *widget, MenuPW *data) { data->BtnComboThresholdChanged(widget); }
    static void HandleComboSmoothChanged(GtkComboBox *widget, MenuPW *data) { data->BtnComboSmoothChanged(widget); }
    static void HandleForeachWidget(gpointer element, MenuPW *data) { data->ForeachWidget(GTK_WIDGET(element)); }
};

extern MenuPW g_menuPW;
extern MenuPW g_menuCW;

#endif
