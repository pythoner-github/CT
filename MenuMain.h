// -*- c++ -*-
#ifndef __MENU_MAIN_H__
#define __MENU_MAIN_H__

#define WIDTH_MAIN_MENU 220
#define HEIGHT_MAIN_MENU 635

#include "MenuArea.h"

class MenuMain
{
public:
    MenuMain();
    ~MenuMain(){};

    GtkWidget * Create(void);
    void UpdateLabel(void);
    void Hide(void);
    void Show(void);
private:
    GtkWidget *m_tableMain;
    MenuArea* ptrMenuArea;
    GtkWidget *m_label2d;
    GtkWidget *m_labelM;
    GtkWidget *m_labelPw;
    GtkWidget *m_labelCw;
    GtkWidget *m_labelCfm;
    GtkWidget *m_labelPdi;
    GtkWidget *m_labelNote;
    GtkWidget *m_labelArrow;
    GtkWidget *m_labelBDMK;
    GtkWidget *m_labelMeasure;
    GtkWidget * m_labelReview;
    GtkWidget * m_labelCalc;
    GtkWidget * m_labelBiopsy;

//signal handle
    void Btn2DClicked(GtkButton *button);
    void BtnMClicked(GtkButton *button);
    void BtnPWClicked(GtkButton *button);
    void BtnCWClicked(GtkButton *button);
    void BtnCFMClicked(GtkButton *button);
    void BtnMeasureClicked(GtkButton *button);
    void BtnNoteClicked(GtkButton *button);
    void BtnBDMKClicked(GtkButton *button);
    void BtnReviewClicked(GtkButton *button);
    void BtnCalcClicked(GtkButton *button);
    void BtnBiopsyClicked(GtkButton *button);

//signal connect
    static void Handle2DClicked(GtkButton *button, MenuMain *data) { data->Btn2DClicked(button); }
    static void HandleMClicked(GtkButton *button, MenuMain *data) { data->BtnMClicked(button); }
    static void HandlePWClicked(GtkButton *button, MenuMain *data) { data->BtnPWClicked(button); }
    static void HandleCWClicked(GtkButton *button, MenuMain *data) { data->BtnCWClicked(button); }
    static void HandleCFMClicked(GtkButton *button, MenuMain *data) { data->BtnCFMClicked(button); }
    static void HandleMeasureClicked(GtkButton *button, MenuMain *data) { data->BtnMeasureClicked(button); }
    static void HandleNoteClicked(GtkButton *button, MenuMain *data) { data->BtnNoteClicked(button); }
    static void HandleBDMKClicked(GtkButton *button, MenuMain *data) { data->BtnBDMKClicked(button); }
    static void HandleReviewClicked(GtkButton *button, MenuMain *data) { data->BtnReviewClicked(button); }
    static void HandleCalcClicked(GtkButton *button, MenuMain *data) { data->BtnCalcClicked(button); }
    static void HandleBiopsyClicked(GtkButton *button, MenuMain *data) { data->BtnBiopsyClicked(button); }
};

extern MenuMain g_menuMain;

#endif
