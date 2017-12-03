#include <gtk/gtk.h>
#include "MenuMain.h"
#include "NoteArea.h"
#include "gui_func.h"
#include "gui_global.h"
#include "MenuArea.h"
#include "MultiFuncFactory.h"
#include "FreezeMode.h"

MenuMain g_menuMain;

MenuMain::MenuMain(void)
{
    m_tableMain = 0;
    ptrMenuArea = MenuArea::GetInstance();
}

void MenuMain::Hide(void)
{
    gtk_widget_hide_all(m_tableMain);
}

void MenuMain::Show(void)
{
    gtk_widget_show_all(m_tableMain);
}

GtkWidget * MenuMain::Create(void)
{
    m_tableMain = gtk_table_new(20, 1, TRUE); 

    GtkWidget *btn_2d;
    m_label2d = create_label("", 0, 0, g_white, NULL);
    btn_2d = create_button(m_label2d, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_2d, 0, 1, 1, 2);
    g_signal_connect(G_OBJECT(btn_2d), "clicked", G_CALLBACK(Handle2DClicked), this);

    GtkWidget *btn_m;
    m_labelM = create_label("", 0, 0, g_white, NULL);
    btn_m = create_button(m_labelM, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_m, 0, 1, 2, 3);
    g_signal_connect(G_OBJECT(btn_m), "clicked", G_CALLBACK(HandleMClicked), this);

    GtkWidget *btn_pw;
    m_labelPw = create_label("", 0, 0, g_white, NULL);
    btn_pw = create_button(m_labelPw, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_pw, 0, 1, 3, 4);
    g_signal_connect(G_OBJECT(btn_pw), "clicked", G_CALLBACK(HandlePWClicked), this);

    GtkWidget *btn_cw;
    m_labelCw = create_label("", 0, 0, g_white, NULL);
    btn_cw = create_button(m_labelCw, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_cw, 0, 1, 4, 5);
    g_signal_connect(G_OBJECT(btn_cw), "clicked", G_CALLBACK(HandleCWClicked), this);

    GtkWidget *btn_cfm;
    m_labelCfm = create_label("", 0, 0, g_white, NULL);
    btn_cfm = create_button(m_labelCfm, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_cfm, 0, 1, 5, 6);
    g_signal_connect(G_OBJECT(btn_cfm), "clicked", G_CALLBACK(HandleCFMClicked), this);

    GtkWidget *btn_pdi;
    m_labelPdi = create_label("", 0, 0, g_white, NULL);
    btn_pdi = create_button(m_labelPdi, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_pdi, 0, 1, 6, 7);

    GtkWidget *btn_note;
    m_labelNote = create_label("", 0, 0, g_white, NULL);
    btn_note = create_button(m_labelNote, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_note, 0, 1, 7, 8);
    g_signal_connect(G_OBJECT(btn_note), "clicked", G_CALLBACK(HandleNoteClicked), this);

    GtkWidget *btn_arrow;
    m_labelArrow = create_label("", 0, 0, g_white, NULL);
    btn_arrow = create_button(m_labelArrow, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_arrow, 0, 1, 8, 9);

    GtkWidget *btn_body_mark;
    m_labelBDMK = create_label("", 0, 0, g_white, NULL);
    btn_body_mark = create_button(m_labelBDMK, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_body_mark, 0, 1, 9, 10);
    g_signal_connect(G_OBJECT(btn_body_mark), "clicked", G_CALLBACK(HandleBDMKClicked), this);
    
    GtkWidget *btn_measure;
    m_labelMeasure = create_label("", 0, 0, g_white, NULL);
    btn_measure = create_button(m_labelMeasure, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_measure, 0, 1, 10, 11);
    g_signal_connect(G_OBJECT(btn_measure), "clicked", G_CALLBACK(HandleMeasureClicked), this);

    GtkWidget *btn_review;
    m_labelReview = create_label("", 0, 0, g_white, NULL);
    btn_review = create_button(m_labelReview, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_review, 0, 1, 11, 12);
    g_signal_connect(G_OBJECT(btn_review), "clicked", G_CALLBACK(HandleReviewClicked), this);

    GtkWidget *btn_calc;
    m_labelCalc = create_label("", 0, 0, g_white, NULL);
    btn_calc = create_button(m_labelCalc, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_calc, 0, 1, 12, 13);
    g_signal_connect(G_OBJECT(btn_calc), "clicked", G_CALLBACK(HandleCalcClicked), this);

    GtkWidget *btn_biopsy;
    m_labelBiopsy = create_label("", 0, 0, g_white, NULL);
    btn_biopsy = create_button(m_labelBiopsy, 0, 0, g_deep);
    gtk_table_attach_defaults(GTK_TABLE(m_tableMain), btn_biopsy, 0, 1, 13, 14);
    g_signal_connect(G_OBJECT(btn_biopsy), "clicked", G_CALLBACK(HandleBiopsyClicked), this);

    UpdateLabel();

    gtk_widget_set_usize(m_tableMain, WIDTH_MAIN_MENU, HEIGHT_MAIN_MENU);

    return m_tableMain;
}

void MenuMain::UpdateLabel(void)
{
    gtk_label_set_text(GTK_LABEL(m_label2d), _("2D Post-process"));
    gtk_label_set_text(GTK_LABEL(m_labelM), _("M Post-process"));
    gtk_label_set_text(GTK_LABEL(m_labelPw), _("PW Post-process"));
    gtk_label_set_text(GTK_LABEL(m_labelCw), _("CW Post-process"));
    gtk_label_set_text(GTK_LABEL(m_labelCfm), _("CFM Post-process"));
    gtk_label_set_text(GTK_LABEL(m_labelPdi), _("PDI Post-process"));
    gtk_label_set_text(GTK_LABEL(m_labelNote), _("Note"));
    gtk_label_set_text(GTK_LABEL(m_labelArrow), _("Arrow"));
    gtk_label_set_text(GTK_LABEL(m_labelBDMK), _("Body Mark"));
    gtk_label_set_text(GTK_LABEL(m_labelMeasure), _("Measure"));
    gtk_label_set_text(GTK_LABEL(m_labelReview), _("Review"));
    gtk_label_set_text(GTK_LABEL(m_labelCalc), _("Calculate"));
    gtk_label_set_text(GTK_LABEL(m_labelBiopsy), _("Biopsy"));
}

void MenuMain::Btn2DClicked(GtkButton *button)
{
    Hide();
    ptrMenuArea->Show2DMenu();
}

void MenuMain::BtnMClicked(GtkButton *button)
{
    Hide();
    ptrMenuArea->ShowMMenu();
}

void MenuMain::BtnPWClicked(GtkButton *button)
{
    Hide();
    ptrMenuArea->ShowPWMenu();
}

void MenuMain::BtnCWClicked(GtkButton *button)
{
    Hide();
    ptrMenuArea->ShowCWMenu();
}

void MenuMain::BtnCFMClicked(GtkButton *button)
{
    Hide();
    ptrMenuArea->ShowCFMMenu();
}

void MenuMain::BtnNoteClicked(GtkButton *button)
{
    Hide();
//	ptrMenuArea->ShowNoteMenu();
	MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::TEXT);
	NoteArea *m_ptrNoteArea = NoteArea::GetInstance();
	m_ptrNoteArea->Show();
}

void MenuMain::BtnBDMKClicked(GtkButton *button)
{
	Hide();
	ptrMenuArea->ShowBDMKMenu();
}

void MenuMain::BtnMeasureClicked(GtkButton *button)
{
    Hide();
    ptrMenuArea->ShowMeasureMenu();
}

void MenuMain::BtnReviewClicked(GtkButton *button)
{
	FreezeMode::EFreezeMode freezemode = FreezeMode::GetInstance()->GetFreezeMode();
	if(freezemode != FreezeMode::FREEZE)
	{
		//add code to show hint
		PRINTF("Please frozen first!\n");
		return;
	}
	
    Hide();
    ptrMenuArea->ShowReviewMenu();
}

void MenuMain::BtnCalcClicked(GtkButton *button)
{
    Hide();
    ptrMenuArea->ShowCalcMenu();
}

void MenuMain::BtnBiopsyClicked(GtkButton *button)
{
    Hide();
//    ptrMenuArea->ShowBiopsyMenu();
}
