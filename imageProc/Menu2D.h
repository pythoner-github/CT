// -*- c++ -*-
#ifndef MENU_2D_SUB_H
#define MENU_2D_SUB_H

#include "Def.h"
#include "CusSpin.h"


class Menu2D
{
public:
    Menu2D();
    ~Menu2D(){}
    void Hide(void);
    void Show(void);
    GtkWidget * Create(void);

	void Sensitive(bool on);
    void UpdateLabel(void);
    void UpdatePolarity(bool on);
    void UpdateNoiseReject(bool on);
    void UpdateRotate(const char* str, EKnobReturn status);
    void UpdateFrame(const char* str, EKnobReturn status);
    void UpdateLine(const char* str, EKnobReturn status);
    void UpdateSmooth(const char* str, EKnobReturn status);
    void UpdateGamma(const char* str, EKnobReturn status);
    void UpdateImgEhn(const char* str, EKnobReturn status);

    void BtnPIP(GtkButton *button);
    void Btn4B(GtkButton *button);

private:
    GtkWidget *m_table;

    GtkWidget *m_btn4B;
    GtkWidget *m_check_btn_polarity;
    GtkWidget *m_check_btn_restric;

    GtkWidget *m_labelPolarity;
    GtkWidget *m_labelRestric;
    GtkWidget *m_labelGrayTrans;
    GtkWidget *m_labelGrayReject;
    GtkWidget *m_labelGlobalZoom;
    GtkWidget *m_labelPIP;
    GtkWidget *m_label4B;
    GtkWidget *m_labelEFOV;    
    
    CusSpin spin_rotate;
    CusSpin spin_frame;
    CusSpin spin_line;
    CusSpin spin_smooth;
    CusSpin spin_gamma;
    CusSpin spin_chroma;
    CusSpin spin_imgEhn;

	bool m_sensitive;

    static CusSpin::CusSpinItem item_rotate;
    static CusSpin::CusSpinItem item_frame;
    static CusSpin::CusSpinItem item_line;
    static CusSpin::CusSpinItem item_smooth;
    static CusSpin::CusSpinItem item_gamma;
    static CusSpin::CusSpinItem item_chroma;
    static CusSpin::CusSpinItem item_imgEhn;
	
    static void ChgRotate(EKnobOper oper);
    static void ChgFrame(EKnobOper oper);
    static void ChgLine(EKnobOper oper);
    static void ChgSmooth(EKnobOper oper);
    static void ChgGamma(EKnobOper oper);
    static void ChgImgEhn(EKnobOper oper);

//signal handle
    void ChkPolarity(GtkButton *button);
    void ChkNoiseReject(GtkButton *button);
    void BtnTrans(GtkButton *button);
    void BtnReject(GtkButton *button);
    void BtnGlobalZoom(GtkButton *button);
    void ForeachWidget(GtkWidget *widget);
    
//signal connect
    static void HandleChkPolarity(GtkButton *button, Menu2D *data) { data->ChkPolarity(button); }
    static void HandleChkRestric(GtkButton *button, Menu2D *data) { data->ChkNoiseReject(button); }
    static void HandleBtnTrans(GtkButton *button, Menu2D *data) { data->BtnTrans(button); }
    static void HandleBtnReject(GtkButton *button, Menu2D *data) { data->BtnReject(button); }
    static void HandleBtnGlobalZoom(GtkButton *button, Menu2D *data) { data->BtnGlobalZoom(button); }
    static void HandleBtnPIP(GtkButton *button, Menu2D *data) { data->BtnPIP(button); }
    static void HandleBtn4B(GtkButton *button, Menu2D *data) { data->Btn4B(button); }
    static void HandleForeachWidget(gpointer element, Menu2D *data) { data->ForeachWidget(GTK_WIDGET(element)); }

    static void HandleBtnEFOV(GtkButton *button, Menu2D *data) { data->BtnEnterEFOV(button); }
    void BtnEnterEFOV(GtkButton *button);
};

extern Menu2D g_menu2D;

#endif
