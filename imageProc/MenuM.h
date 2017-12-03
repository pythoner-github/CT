// -*- c++ -*-
#ifndef MENU_M_SUB_H
#define MENU_M_SUB_H

#include "Def.h"
#include "CusSpin.h"

class MenuM
{
public:
    MenuM();
    ~MenuM(){};

    GtkWidget * Create(void);
    void Hide(void);
    void Show(void);
    
	void Sensitive(bool on);
    void UpdatePolarity(bool on);
    void UpdateTimeMark(bool on);
    void UpdateLine(const char *str, EKnobReturn status);
    void UpdateEnhanceM(const char *str, EKnobReturn status);
    void UpdateSmooth(const char *str, EKnobReturn status);
    void UpdateLabel(void);
    void UpdateDisplayL(void);
private:
    GtkWidget *m_table;
    GtkWidget *m_labelPolarity;
    GtkWidget *m_labelTimemark;
    GtkWidget *m_frameFormat;
    //GtkWidget *m_btnFormat; 

    GtkWidget *m_check_btn_polarity;
    GtkWidget *m_check_btn_timemark;

    CusSpin spin_line;
    CusSpin spin_enhanceM;
    CusSpin spin_smooth;
    GtkWidget *m_labelAnatomicM;
    static CusSpin::CusSpinItem item_line;
    static CusSpin::CusSpinItem item_smooth;
    static CusSpin::CusSpinItem item_enhanceM;

	bool m_sensitive;
    int m_format;

	//signal handle
    static void ChgLine(EKnobOper oper);
    static void ChgEnhanceM(EKnobOper oper);
    static void ChgSmooth(EKnobOper oper);

    void ChkPolarity(GtkButton *button);
    void ChkTimemark(GtkButton *button);
    void BtnAnatomic(GtkButton *button);
    //lhm
    void BtnFormatClicked(GtkButton *button);
    void BtnFormat1Clicked(GtkButton *button);
    void BtnFormat2Clicked(GtkButton *button);
    void BtnFormat3Clicked(GtkButton *button);
    void BtnFormat4Clicked(GtkButton *button);
    void BtnFormat5Clicked(GtkButton *button);
    void ForeachWidget(GtkWidget *widget);
    void WriteFormatM(int format);


//signal connect
    static void HandleChkPolarity(GtkButton *button, MenuM *data) { data->ChkPolarity(button); }
    static void HandleChkTimemark(GtkButton *button, MenuM *data) { data->ChkTimemark(button); }
    static void HandleBtnAnatomic(GtkButton *button, MenuM *data) { data->BtnAnatomic(button); }

    static void HandleBtnFormat(GtkButton *button, MenuM *data) { data->BtnFormatClicked(button); }

    static void HandleBtnFormat1(GtkButton *button, MenuM *data) { data->BtnFormat1Clicked(button); }
    static void HandleBtnFormat2(GtkButton *button, MenuM *data) { data->BtnFormat2Clicked(button); }
    static void HandleBtnFormat3(GtkButton *button, MenuM *data) { data->BtnFormat3Clicked(button); }
    static void HandleBtnFormat4(GtkButton *button, MenuM *data) { data->BtnFormat4Clicked(button); }
    static void HandleBtnFormat5(GtkButton *button, MenuM *data) { data->BtnFormat5Clicked(button); }
    static void HandleForeachWidget(gpointer element, MenuM *data) { data->ForeachWidget(GTK_WIDGET(element)); }
};

extern MenuM g_menuM;

#endif
