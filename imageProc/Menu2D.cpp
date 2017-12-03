#include <gtk/gtk.h>
#include "Menu2D.h"
#include "gui_func.h"
#include "gui_global.h"
#include "Img2D.h"
#include "Measure2D.h"
#include "ImgProc2D.h"
#include "MultiFuncFactory.h"
#include "HintArea.h"
#include "../keyboard/KeyFunc.h"
#include "MeasureFactory.h"
#include "CDrawIMT.h"
#include "ViewReport.h"
#include "ViewArchive.h"
#include "ViewSystem.h"

Menu2D g_menu2D;

CusSpin::CusSpinItem Menu2D::item_rotate= {N_("Rotate"), "0", OK, ChgRotate};
CusSpin::CusSpinItem Menu2D::item_frame = {N_("Frame aver."), "1", OK, ChgFrame};
CusSpin::CusSpinItem Menu2D::item_line = {N_("Line aver."), "1", OK, ChgLine};
CusSpin::CusSpinItem Menu2D::item_smooth = {N_("Smooth"), "1", OK, ChgSmooth};
CusSpin::CusSpinItem Menu2D::item_gamma = {N_("Gamma"), "1", OK, ChgGamma};
CusSpin::CusSpinItem Menu2D::item_imgEhn = {N_("ePure"), "0", OK, ChgImgEhn};

Menu2D::Menu2D(void)
{
	m_table = 0;
}

void Menu2D::Hide(void)
{
	gtk_widget_hide_all(m_table);
}

void Menu2D::Show(void)
{
	gtk_widget_show_all(m_table);

    if (ModeStatus::IsSpectrumColorMode()) 
    {
        gtk_widget_hide(m_btn4B);
    }

    gtk_widget_hide(m_check_btn_polarity);
}

void Menu2D::ForeachWidget(GtkWidget *widget)
{
	if(widget != m_btn4B)
		gtk_widget_set_sensitive(widget, (bool)m_sensitive);
}

void Menu2D::Sensitive(bool on)
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

GtkWidget* Menu2D::Create(void)
{
    m_table = gtk_table_new(20, 1, TRUE);

    //polarity
    m_labelPolarity = create_label("", 0, 0, g_lightGray, NULL);
    m_check_btn_polarity = gtk_check_button_new();
    gtk_container_add(GTK_CONTAINER(m_check_btn_polarity), m_labelPolarity);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_check_btn_polarity, 0, 1, 0, 1);
    g_signal_connect(m_check_btn_polarity, "clicked", G_CALLBACK(HandleChkPolarity), this);
    gtk_widget_show(m_check_btn_polarity);

    // restric on/off
    m_labelRestric = create_label("", 0, 0, g_lightGray, NULL);
    m_check_btn_restric = gtk_check_button_new();
    gtk_container_add(GTK_CONTAINER(m_check_btn_restric), m_labelRestric);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_check_btn_restric, 0, 1, 1, 2);
    g_signal_connect(m_check_btn_restric, "clicked", G_CALLBACK(HandleChkRestric), this);
    gtk_widget_show(m_check_btn_polarity);

    // rotate 90/270/OFF
    spin_rotate.SetItem(&item_rotate);
    GtkWidget *table_rotate= spin_rotate.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_rotate, 0, 1, 3, 4);
    spin_rotate.Show();

    // frame aver. 1-8
    spin_frame.SetItem(&item_frame);
    GtkWidget *table_frame = spin_frame.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_frame, 0, 1, 4, 5);
    spin_frame.Show();
	
    // line aver. 1-4
    spin_line.SetItem(&item_line);
    GtkWidget *table_line = spin_line.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_line, 0, 1, 5, 6);
    spin_line.Show();

    // smooth 1-8
    spin_smooth.SetItem(&item_smooth);
    GtkWidget *table_smooth = spin_smooth.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_smooth, 0, 1, 6, 7);
    spin_smooth.Show();

    //gamma 1-8
    spin_gamma.SetItem(&item_gamma);
    GtkWidget *table_gamma = spin_gamma.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_gamma, 0, 1, 7, 8);
    spin_gamma.Show();

    //image ehn
    spin_imgEhn.SetItem(&item_imgEhn);
    GtkWidget *table_imgEhn = spin_imgEhn.Create();
    gtk_table_attach_defaults(GTK_TABLE(m_table), table_imgEhn, 0, 1, 8, 9);
    spin_imgEhn.Show();

    //gray transform
    m_labelGrayTrans = create_label("", 0, 0, g_lightGray, NULL);
    GtkWidget *btn_gray_trans = create_button(m_labelGrayTrans, 0, 0, g_deep);
    gtk_widget_modify_bg(btn_gray_trans, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), btn_gray_trans, 0, 1, 9, 10);
    g_signal_connect(btn_gray_trans, "clicked", G_CALLBACK(HandleBtnTrans), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_gray_trans), FALSE);
//	gtk_button_set_focus_on_click(GTK_BUTTON(btn_gray_trans), true);//test-2016.08.11

    //gray rejection
    m_labelGrayReject = create_label("", 0, 0, g_lightGray, NULL);
    GtkWidget *btn_gray_reject = create_button(m_labelGrayReject, 0, 0, g_deep);
    gtk_widget_modify_bg(btn_gray_reject, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), btn_gray_reject, 0, 1, 10, 11);
    g_signal_connect(btn_gray_reject, "clicked", G_CALLBACK(HandleBtnReject), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_gray_reject), FALSE);

    // global zoom
    m_labelGlobalZoom = create_label("", 0, 0, g_lightGray, NULL);
    GtkWidget *btn_global_zoom = create_button(m_labelGlobalZoom, 0, 0, g_deep);
    gtk_widget_modify_bg(btn_global_zoom, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), btn_global_zoom, 0, 1, 12, 13);
    g_signal_connect(btn_global_zoom, "clicked", G_CALLBACK(HandleBtnGlobalZoom), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_global_zoom), FALSE);

    // PIP zoom
    m_labelPIP = create_label("", 0, 0, g_lightGray, NULL);
    GtkWidget *btn_PIP = create_button(m_labelPIP, 0, 0, g_deep);
    gtk_widget_modify_bg(btn_PIP, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), btn_PIP, 0, 1, 13, 14);
    g_signal_connect(btn_PIP, "clicked", G_CALLBACK(HandleBtnPIP), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_PIP), FALSE);

#if defined(K24C)
    //m_label4B = create_label("", 0, 0, g_lightGray, NULL);
    //GtkWidget *btn_4B= gtk_button_new();
    //gtk_widget_modify_bg(btn_4B, GTK_STATE_NORMAL, g_deep);
    //gtk_container_add(GTK_CONTAINER(btn_4B), m_label4B);
    //gtk_table_attach_defaults(GTK_TABLE(m_table), btn_4B, 0, 1, 14, 15);
    //g_signal_connect(btn_4B, "clicked", G_CALLBACK(HandleBtn4B), this);
    //gtk_button_set_focus_on_click(GTK_BUTTON(btn_4B), FALSE);
#else
    // 4B 
#ifndef EMP_355
    m_label4B = create_label("", 0, 0, g_lightGray, NULL);
    m_btn4B = create_button(m_label4B, 0, 0, g_deep);
    gtk_widget_modify_bg(m_btn4B, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_btn4B, 0, 1, 14, 15);
    g_signal_connect(m_btn4B, "clicked", G_CALLBACK(HandleBtn4B), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(m_btn4B), FALSE);
#endif
#endif 

    // EFOV
#ifdef EMP_3410
    if(CManRegister::GetInstance()->IsAuthorize("eView"))
    {
        m_labelEFOV = create_label("", 0, 0, g_lightGray, NULL);
        GtkWidget *btn_efov_mode = create_button(m_labelEFOV, 0, 0, g_deep);
        gtk_widget_modify_bg(btn_efov_mode, GTK_STATE_INSENSITIVE, g_deepGray);
        gtk_table_attach_defaults(GTK_TABLE(m_table), btn_efov_mode, 0, 1, 16, 17);
        g_signal_connect(btn_efov_mode, "clicked", G_CALLBACK(HandleBtnEFOV), this);
        gtk_button_set_focus_on_click(GTK_BUTTON(btn_efov_mode), FALSE);
    }
#else
    m_labelEFOV = create_label("", 0, 0, g_lightGray, NULL);
    GtkWidget *btn_efov_mode = create_button(m_labelEFOV, 0, 0, g_deep);
    gtk_widget_modify_bg(btn_efov_mode, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_table_attach_defaults(GTK_TABLE(m_table), btn_efov_mode, 0, 1, 16, 17);
    g_signal_connect(btn_efov_mode, "clicked", G_CALLBACK(HandleBtnEFOV), this);
    gtk_button_set_focus_on_click(GTK_BUTTON(btn_efov_mode), FALSE);
#endif

    UpdateLabel();

    return m_table;
}

void Menu2D::UpdateLabel(void)
{
    gtk_label_set_text(GTK_LABEL(m_labelPolarity), _("Polarity"));
    gtk_label_set_text(GTK_LABEL(m_labelRestric), _("Noise Reject"));
    spin_rotate.UpdateLabel();
    spin_frame.UpdateLabel();
    spin_line.UpdateLabel();
    spin_smooth.UpdateLabel();
    spin_gamma.UpdateLabel();
    spin_imgEhn.UpdateLabel();
    gtk_label_set_text(GTK_LABEL(m_labelGrayTrans), _("Gray Trans."));
    gtk_label_set_text(GTK_LABEL(m_labelGrayReject), _("Gray Rejection"));
    gtk_label_set_text(GTK_LABEL(m_labelGlobalZoom), _("Global Zoom"));
    gtk_label_set_text(GTK_LABEL(m_labelPIP), _("Picture In Picture"));
    gtk_label_set_text(GTK_LABEL(m_label4B), _("4B"));
#ifdef EMP_3410
    if(CManRegister::GetInstance()->IsAuthorize("eView"))
        gtk_label_set_text(GTK_LABEL(m_labelEFOV), _("eView Mode"));
#else
    gtk_label_set_text(GTK_LABEL(m_labelEFOV), _("eView Mode"));
#endif
}

void Menu2D::UpdatePolarity(bool on)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_check_btn_polarity),on);
}
void Menu2D::UpdateNoiseReject(bool on)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_check_btn_restric),on);
}
void Menu2D::UpdateFrame(const char* str, EKnobReturn status)
{
    spin_frame.SetValue(str, status);
}

void Menu2D::UpdateRotate(const char* str, EKnobReturn status)
{
	spin_rotate.SetValue(str, status);
}

void Menu2D::UpdateLine(const char* str, EKnobReturn status)
{
	spin_line.SetValue(str, status);
}

void Menu2D::UpdateSmooth(const char* str, EKnobReturn status)
{
	spin_smooth.SetValue(str, status);
}

void Menu2D::UpdateGamma(const char* str, EKnobReturn status)
{
	spin_gamma.SetValue(str, status);
}

void Menu2D::UpdateImgEhn(const char* str, EKnobReturn status)
{
	spin_imgEhn.SetValue(str, status);
}

///> private
void Menu2D::ChkPolarity(GtkButton *button)
{
	gboolean value;
	value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
	ImgProc2D::GetInstance()->ChangePolarity(value);
}

void Menu2D::ChkNoiseReject(GtkButton *button)
{
	gboolean value;
	value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
	ImgProc2D::GetInstance()->ChangeNoiseReject(value);
}

void Menu2D::BtnTrans(GtkButton *button)
{    
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::GRAY_TRANS);
	//KeyEndExam kee;
	//kee.Execute();
}

void Menu2D::BtnReject(GtkButton *button)
{
   MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::GRAY_REJECT);

   //ViewArchive::GetInstance()->CreateWindow();
}

void Menu2D::BtnGlobalZoom(GtkButton *button)
{
    int rotate = ImgProc2D::GetInstance()->GetRotate();
	ModeStatus ms;
    if (ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B) && ms.IsUnFreezeMode())
    {
        if (rotate == 0)
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::GLOBAL_ZOOM);
        else
            HintArea::GetInstance()->UpdateHint(_("[Global Zoom]: Invalid when image is rotated."), 1);
    }
    else
    {
#if (defined(EMP_322) || defined(EMP_313))
        HintArea::GetInstance()->UpdateHint(_("[Global Zoom]: Only valid in B mode and UnFreeze status."), 1);
#else
        HintArea::GetInstance()->UpdateHint(_("[Global Zoom]: Only valid in 2D B mode and UnFreeze status."), 1);
#endif 
    }
}

void Menu2D::BtnPIP(GtkButton *button)
{
	ModeStatus ms;
    if (ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B) && ms.IsUnFreezeMode())
    {
        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::PIP_ZOOM);
    }
	else
    {
#if (defined(EMP_322) || defined(EMP_313))
		HintArea::GetInstance()->UpdateHint(_("[PIP]: Only valid in B mode and UnFreeze status."), 1);
#else
        HintArea::GetInstance()->UpdateHint(_("[PIP]: Only valid in 2D B mode and UnFreeze status."), 1);
#endif
    }
}

void Menu2D::Btn4B(GtkButton *button)
{
    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM)
    {
        ModeStatus s;
        int mode = s.GetScanMode();
        if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
        {
            Img2D::GetInstance()->ExitTpView();
            Img2D::GetInstance()->ExitEFVI();
        }
        if (mode == ScanMode::D2)
        {
            Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
            if (format != Format2D::B4)
                Format2D::GetInstance()->ChangeFormat(Format2D::B4);	
            else 
                Format2D::GetInstance()->SwitchB4();	
        }
        else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
        {
            FormatCfm::EFormatCfm format = FormatCfm::GetInstance()->GetFormat();
            if (format != FormatCfm::B4)
                FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B4);	
            else
                FormatCfm::GetInstance()->SwitchB4();
        }
        else if (mode == ScanMode::CFM_VS_2D)
        {
            ScanMode::GetInstance()->EnterCfm();
            FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B4);	
        }
        else if (mode == ScanMode::PDI_VS_2D)
        {
            ScanMode::GetInstance()->EnterPdi();
            FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B4);	
        }
    }
}

void Menu2D::ChgRotate(EKnobOper oper)
{
	ModeStatus ms;
	if ((ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B)))
    {
#if 0
        if (CDrawIMT::GetInstance()->UpdateIMTStatus())
            HintArea::GetInstance()->UpdateHint(_("[Rotate]: Invalid when IMTMeasure is open."), 1);
        else
            ImgProc2D::GetInstance()->ChangeRotate(oper);
#endif 
        ImgProc2D::GetInstance()->ChangeRotate(oper);
    }
    else
    { 
#if (defined(EMP_322) || defined(EMP_313))
        HintArea::GetInstance()->UpdateHint(_("[Rotate]: Only valid in B mode."), 1);
#else
        HintArea::GetInstance()->UpdateHint(_("[Rotate]: Only valid in 2D B mode."), 1);
#endif
    }
}

void Menu2D::ChgFrame(EKnobOper oper)
{
	ImgProc2D::GetInstance()->ChangeFrameAver(oper);
}

void Menu2D::ChgLine(EKnobOper oper)
{
	ImgProc2D::GetInstance()->ChangeLineAver(oper);
}

void Menu2D::ChgSmooth(EKnobOper oper)
{
	ImgProc2D::GetInstance()->ChangeSmooth(oper);
}

void Menu2D::ChgGamma(EKnobOper oper)
{
	ImgProc2D::GetInstance()->ChangeGamma(oper);
}

void Menu2D::ChgImgEhn(EKnobOper oper)
{
	ImgProc2D::GetInstance()->ChangeImgEhn(oper);
}

void Menu2D::BtnEnterEFOV(GtkButton *button)
{
    ModeStatus ms;
    if (DscMan::GetInstance()->GetDscPara()->dcaProbeType == 'L' && ms.IsUnFreezeMode() && ModeStatus::IsD2Mode())
    {
        // exit local zoom if in it
        KeyLocalZoom klz;
        klz.ExitLocalZoom();

        // exit Tp-View if in it
        Img2D::GetInstance()->ExitTpView();

        // enter EFOV mode
        ScanMode::GetInstance()->EnterEFOV();
    }
    else
    {
#if (defined(EMP_322) || defined(EMP_313))
        HintArea::GetInstance()->UpdateHint(_("[eView]: Only valid in B/BB mode, unfreeze status, linear probe."), 2);
#else
        HintArea::GetInstance()->UpdateHint(_("[eView]: Only valid in 2D mode, unfreeze status, linear probe."), 2);
#endif
        //HintArea::GetInstance()->UpdateHint(_("[eView]: Only valid when probe type is linear and unfreeze status."), 1);
    }
 }

