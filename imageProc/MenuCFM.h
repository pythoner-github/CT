#ifndef MENU_CFM_SUB_H
#define MENU_CFM_SUB_H

#define WIDTH_CFM_MENU  220
#define HEIGHT_CFM_MENU  635

#include "Def.h"
#include "CusSpin.h"

class MenuCFM
{
public:
	MenuCFM();
	~MenuCFM(){};
	void Hide(void);
	void Show(void);
	GtkWidget* Create(void);

	void Sensitive(bool on);
	void UpdateBackgroundHide(bool on);
	void UpdateBCWidth(bool on);
	void UpdateCfmCompare(bool on);
	void UpdatePdiDir(bool on);
	void UpdateThreshold(const char* str, EKnobReturn status);
	void UpdatePersist(const char* str, EKnobReturn status);
	void UpdateReject(const char* str, EKnobReturn status);
	void UpdateSmooth(const char* str, EKnobReturn status);
	void UpdateArtifact(const char* str, EKnobReturn status);
	void UpdateLabel(void);
private:
	GtkWidget *m_table;

	GtkWidget *m_labelColorhide;
	GtkWidget *m_btnColorhide;
	GtkWidget *m_labelBghide;
	GtkWidget *m_check_btn_bghide;
	GtkWidget *m_labelBC;
	GtkWidget *m_btnBC;
	GtkWidget *m_labelPdiDir;
	GtkWidget *m_check_btn_PdiDir;
    GtkWidget *m_label4BCFM;
	GtkWidget *m_btn4BCFM;
	GtkWidget *m_labelCompare;
	GtkWidget *m_btnCompare;
	GtkWidget *m_labelScale;
    GtkWidget *m_btnScale;
	GtkTreeModel *m_modelScale;
    GtkWidget *m_comboScale;
	
	CusSpin spin_threshold;
	CusSpin spin_persist;
	CusSpin spin_reject;
	CusSpin spin_smooth;
	CusSpin spin_artifact;

	bool m_sensitive;

	static GtkTreeModel *CreateScaleModel();
	void UpdateScaleModel(GtkTreeModel *model);

	static CusSpin::CusSpinItem item_threshold;
	static CusSpin::CusSpinItem item_persist;
	static CusSpin::CusSpinItem item_reject;
	static CusSpin::CusSpinItem item_smooth;
	static CusSpin::CusSpinItem item_artifact;	

	static void ChgThreshold(EKnobOper oper);
	static void ChgPersist(EKnobOper oper);
	static void ChgReject(EKnobOper oper);
	static void ChgSmooth(EKnobOper oper);
	static void ChgArtifact(EKnobOper oper);

	void ChkInvertClicked(GtkButton *button);
	void ChkColorHideClicked(GtkButton *button);
	void ChkBgHideClicked(GtkButton *button);
	void ChkBCClicked(GtkButton *button);
	void ChkPdiDirClicked(GtkButton *button);
	void Chk4BCFM(GtkButton *button);
	void ChkCompareClicked(GtkButton *button);
	void BtnComboScaleChanged(GtkComboBox *widget);
    void ForeachWidget(GtkWidget *widget);

	static void HandleChkInvert(GtkButton *button, MenuCFM *data) { data->ChkInvertClicked(button); }
	static void HandleChkColorHide(GtkButton *button, MenuCFM *data) { data->ChkColorHideClicked(button); }
	static void HandleChkBgHide(GtkButton *button, MenuCFM *data) { data->ChkBgHideClicked(button); }
	static void HandleChkBC(GtkButton *button, MenuCFM *data) { data->ChkBCClicked(button); }
	static void HandleBtn4BCFM(GtkButton *button, MenuCFM *data) { data->Chk4BCFM(button); }
	static void HandleChkPdiDir(GtkButton *button, MenuCFM *data) { data->ChkPdiDirClicked(button); }
	static void HandleChkCompare(GtkButton *button, MenuCFM *data) { data->ChkCompareClicked(button); }
    static void HandleComboScaleChanged(GtkComboBox *widget, MenuCFM *data) { data->BtnComboScaleChanged(widget); }
    static void HandleForeachWidget(gpointer element, MenuCFM *data) { data->ForeachWidget(GTK_WIDGET(element)); }

};

extern MenuCFM g_menuCFM;
#endif
