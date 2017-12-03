#include <gtk/gtk.h>
#include "display/gui_global.h"
#include "display/gui_func.h"
#include "ViewMain.h"
#include "display/MenuArea.h"
#include "MenuMain.h"
#include "imageProc/Menu2D.h"
#include "imageProc/MenuM.h"
#include "imageProc/MenuPW.h"
#include "imageProc/MenuCFM.h"
#include "imageProc/MenuEFOV.h"
#include "measure/MenuMeasure.h"
#include "bodymark/MenuBDMK.h"
#include "imageProc/MenuReview.h"
#include "calcPeople/MenuCalcNew.h"
#include "comment/MenuNote.h"
#include "probe/MenuBiopsy.h"
#include "probe/MenuBiopsyBracket.h" //2016.07.26
#include "probe/MenuBiopsyVerify.h" //2016.07.26
#include "sysMan/MenuSystem.h"
#include "imageControl/Knob2D.h"
#include "imageControl/KnobM.h"
#include "imageControl/KnobPw.h"
#include "imageControl/KnobCfm.h"
#include "imageProc/ModeStatus.h"
#include "keyboard/KeyFunc.h"
#include "keyboard/LightDef.h"
#include "projectMode/MenuProjectMode.h"

MenuArea* MenuArea::m_ptrInstance = NULL;

MenuArea* MenuArea::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new MenuArea;
    return m_ptrInstance;
}

MenuArea::MenuArea()
{
    m_menuVisbible = TRUE;
    m_tableMenu = 0;
    m_menuType = D2;
    m_in2DOnly = TRUE;
}

GtkWidget* MenuArea::Create(void)
{
	const int widthMax = MENU_AREA_W - 5;
	const int heightMax = MENU_AREA_H - TOP_AREA_H;
	const int heightMaxNB = MENU_AREA_H - TOP_AREA_H - 30;
	PangoFontDescription *font = pango_font_description_from_string("WenQuanYi Zen Hei, bold, 14");
    m_labelSub = create_label(_("Sub Menu"), widthMax, TOP_AREA_H, g_white, font);
    gtk_label_set_justify(GTK_LABEL(m_labelSub), GTK_JUSTIFY_CENTER);
    gtk_fixed_put(GTK_FIXED(ViewMain::GetInstance()->GetMainWindowFixed()), m_labelSub, 0, 0);
    pango_font_description_free(font);

    m_fixedMenu = gtk_fixed_new();
    gtk_widget_set_usize(m_fixedMenu, widthMax, heightMax);

    //Main menu
    m_noteBook = gtk_notebook_new ();
    gtk_fixed_put(GTK_FIXED(m_fixedMenu), m_noteBook, 0, 0);
    gtk_widget_modify_bg(m_noteBook, GTK_STATE_NORMAL, g_deep);
    g_signal_connect(G_OBJECT(m_noteBook), "switch-page", G_CALLBACK(HandleNotebookChanged), this);
    gtk_container_set_border_width (GTK_CONTAINER (m_noteBook), 0);

    //2D menu
    GtkWidget *table2D = g_menu2D.Create();
    gtk_widget_set_usize(table2D, widthMax, heightMaxNB);
    gtk_container_add (GTK_CONTAINER (m_noteBook), table2D);

    m_label2D = gtk_label_new ("2D");
    gtk_widget_set_size_request (m_label2D, 45, -1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_noteBook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_noteBook), 0), m_label2D);

    // M Menu
    GtkWidget *tableM = g_menuM.Create();
    gtk_widget_set_usize(tableM, widthMax, heightMaxNB);
    gtk_container_add (GTK_CONTAINER (m_noteBook), tableM);

    m_labelM = gtk_label_new ("M");
    gtk_widget_set_size_request (m_labelM, 45, -1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_noteBook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_noteBook), 1), m_labelM);

    //PW menu
    GtkWidget *tablePW = g_menuPW.Create();
    gtk_widget_set_usize(tablePW, widthMax, heightMaxNB);
    gtk_container_add (GTK_CONTAINER (m_noteBook), tablePW);

    m_labelPW = gtk_label_new ("PW");
    gtk_widget_set_size_request (m_labelPW, 45, -1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_noteBook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_noteBook), 2), m_labelPW);

    // created cw menu is invalid.
    //CW menu
    GtkWidget *tableCW = g_menuCW.Create();
    gtk_widget_set_usize(tableCW, widthMax, heightMaxNB);
    gtk_container_add (GTK_CONTAINER (m_noteBook), tableCW);

    m_labelCW = gtk_label_new ("CW");
    gtk_widget_set_size_request (m_labelCW, 45, -1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_noteBook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_noteBook), 3), m_labelCW);

    //CFM menu
    GtkWidget *tableCFM = g_menuCFM.Create();
    gtk_widget_set_usize(tableCFM, widthMax, heightMaxNB);
    gtk_container_add (GTK_CONTAINER (m_noteBook), tableCFM);

    m_labelCFM = gtk_label_new ("CFM/PDI");
    gtk_widget_set_size_request (m_labelCFM, 55, -1);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_noteBook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_noteBook), 4), m_labelCFM);

    //Measure2D menu
    GtkWidget *nbMeasure = g_menuMeasure.Create();
    gtk_widget_set_usize(nbMeasure, widthMax+5, heightMax);
    gtk_fixed_put(GTK_FIXED(m_fixedMenu), nbMeasure, 0, 0);

    //BodyMark menu
    GtkWidget *fixedBDMK = g_menuBDMK.Create();
    gtk_widget_set_usize(fixedBDMK, widthMax, heightMaxNB);
    gtk_fixed_put(GTK_FIXED(m_fixedMenu), fixedBDMK, 0, 0);

    //Review menu
    GtkWidget *fixedReview = g_menuReview.Create();
    gtk_widget_set_usize(fixedReview, widthMax, heightMaxNB);
    gtk_fixed_put(GTK_FIXED(m_fixedMenu), fixedReview, 0, 0);

    //Calc menu
    GtkWidget *vboxCalc = g_menuCalc.Create();
    gtk_widget_set_usize(vboxCalc, widthMax, heightMax);
    gtk_fixed_put(GTK_FIXED(m_fixedMenu), vboxCalc, 0, 0);

    //Note menu
    GtkWidget *fixedNote = g_menuNote.Create();
    gtk_widget_set_usize(fixedNote, widthMax, heightMax);
    gtk_fixed_put(GTK_FIXED(m_fixedMenu), fixedNote, 0, 0);

	//Other menu
    GtkWidget *vboxBiopsy = g_menuBiopsy.Create();
    gtk_widget_set_usize(vboxBiopsy, widthMax, heightMaxNB);
    gtk_fixed_put(GTK_FIXED(m_fixedMenu), vboxBiopsy, 0, 0);
//-------------------------2016.07.26--------------------------------------//
    GtkWidget *vboxBiopsyBracket = g_menuBiopsyBracket.Create();
    gtk_widget_set_usize(vboxBiopsyBracket, widthMax, heightMaxNB);
    gtk_fixed_put(GTK_FIXED(m_fixedMenu), vboxBiopsyBracket, 0, 0);

	GtkWidget *vboxBiopsyVerify = g_menuBiopsyVerify.Create();
    gtk_widget_set_usize(vboxBiopsyVerify, widthMax, heightMaxNB);
    gtk_fixed_put(GTK_FIXED(m_fixedMenu), vboxBiopsyVerify, 0, 0);

//-------------------------------------------------------------------//
	GtkWidget *tableSystem = g_menuSystem.Create();
    gtk_widget_set_usize(tableSystem, widthMax, heightMaxNB);
    gtk_fixed_put(GTK_FIXED(m_fixedMenu), tableSystem, 0, 0);

    GtkWidget *tableEFOV = g_menuEFOV.Create();
    gtk_widget_set_usize(tableEFOV, widthMax, heightMaxNB);
    gtk_fixed_put(GTK_FIXED(m_fixedMenu), tableEFOV, 0, 0);

#ifdef EMP_PROJECT
    //ProjectMode menu
    GtkWidget *fixedProjectMode = g_menuProjectMode.Create();
    gtk_widget_set_usize(fixedProjectMode, widthMax, heightMax);
    gtk_fixed_put(GTK_FIXED(m_fixedMenu), fixedProjectMode, 0, 0);
#endif

    return m_fixedMenu;
}

MenuArea::EMenuType MenuArea::GetMenuType(void)
{
    return m_menuType;
}

void MenuArea::SwitchMenu(EMenuType type)
{

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
		ShowProjectModeMenu();
        return;
    }
#endif

    switch(type)
    {
        case MAIN:
            ShowMainMenu();
            break;
        case D2:
            Show2DMenu();
            break;
        case M:
            ShowMMenu();
            break;
        case PW:
            ShowPWMenu();
            break;
        case CW:
            ShowCWMenu();
            break;
        case CFM:
            ShowCFMMenu();
            break;
        case PDI:
            ShowCFMMenu();
            break;
         case PWCFM:
            ShowPWCFMMenu(TRUE);
            break;
        case CWCFM:
            ShowCWCFMMenu(TRUE);
            break;
        case MEASURE:
            ShowMeasureMenu();
            break;
        case BDMK:
            ShowBDMKMenu();
            break;
        case REVIEW:
            ShowReviewMenu();
			KeyReview::m_menuRead = TRUE; // must after ShowReviewMenu(HideAllMenu), be care of "HideMenuReview"
            break;
        case CALC:
            ShowCalcMenu();
            break;
        case NOTE:
            ShowNoteMenu();
            break;
            // case BIOPSY:
            //     ShowBiopsyMenu();
            //     break;
        case SYSTEM:
            ShowSystemMenu();
            break;
        case EFOV:
            {
                ScanMode::EFOVStatus status = ScanMode::GetInstance()->GetEFOVStatus();
                switch (status) {
                    case ScanMode::PREPARE:
                        ShowEFOVPrepare();
                        break;
                    case ScanMode::CAPTURE:
                        ShowEFOVCapture();
                        break;
                    case ScanMode::VIEW:
                        ShowEFOVView();
                        break;
                    case ScanMode::REVIEW:
                        ShowEFOVReview();
                        break;
                }
            }
            break;
        default:
            break;
    }
}

void MenuArea::UpdateSubMenu(void)
{
    if(m_labelSub == NULL)
        return;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
		gtk_label_set_text(GTK_LABEL(m_labelSub), _("Project Mode"));
        return;
    }
#endif

    switch (m_menuType) {
		case MAIN:
			gtk_label_set_text(GTK_LABEL(m_labelSub), "");
			break;
		case D2:
			gtk_label_set_text(GTK_LABEL(m_labelSub), _("2D Mode"));
			break;
		case M:
			gtk_label_set_text(GTK_LABEL(m_labelSub), _("M Mode"));
			break;
		case PW:
			gtk_label_set_text(GTK_LABEL(m_labelSub), _("PW Mode"));
			break;
		case CW:
			gtk_label_set_text(GTK_LABEL(m_labelSub), _("CW Mode"));
			break;
		case CFM:
			gtk_label_set_text(GTK_LABEL(m_labelSub), "Color Mode");
			break;
		case PDI:
			gtk_label_set_text(GTK_LABEL(m_labelSub), "Color Mode");
			break;
        case PWCFM:
            gtk_label_set_text(GTK_LABEL(m_labelSub), _("PW Color Mode"));
            break;
        case CWCFM:
            gtk_label_set_text(GTK_LABEL(m_labelSub), _("CW Color Mode"));
			break;
		case MEASURE:
			gtk_label_set_text(GTK_LABEL(m_labelSub), _("Measure"));
			break;
		case BDMK:
			gtk_label_set_text(GTK_LABEL(m_labelSub), _("BodyMark"));
			break;
		case REVIEW:
			gtk_label_set_text(GTK_LABEL(m_labelSub), _("Review"));
			break;
		case CALC:
			gtk_label_set_text(GTK_LABEL(m_labelSub), _("Calculate"));
			break;
		case NOTE:
			gtk_label_set_text(GTK_LABEL(m_labelSub), _("Note"));
			break;
		case SYSTEM:
			gtk_label_set_text(GTK_LABEL(m_labelSub), _("System Setting"));
			break;
		default:
			break;
	}
}

void MenuArea::UpdateLabel(void)
{

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        g_menuProjectMode.UpdateLabel();
    }
#endif

    //    g_menuMain.UpdateLabel();
    g_menu2D.UpdateLabel();
    g_menuM.UpdateLabel();
    g_menuPW.UpdateLabel();
    g_menuCW.UpdateLabel();
    g_menuCFM.UpdateLabel();
    g_menuMeasure.UpdateLabel();
    g_menuBDMK.UpdateLabel();
    g_menuReview.UpdateLabel();
    g_menuCalc.UpdateLabel();
    g_menuNote.UpdateLabel();
    g_menuBiopsy.UpdateLabel();
    g_menuSystem.UpdateLabel();
    g_menuEFOV.UpdateLabel();
	g_menuBiopsy.UpdateLabel();//2016.11.02
	g_menuBiopsyVerify.UpdateLabel();//2016.11.02
    UpdateSubMenu();
}

void MenuArea::HideCurMenuChild(void)
{

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
    }
    else
    {
		g_menuProjectMode.Hide();
    }
#endif

    switch(m_menuType)
    {
        case MAIN:
            //	g_menuMain.Hide();
            break;
        case D2:
            g_menu2D.Hide();
            break;
        case M:
            g_menuM.Hide();
            break;
        case PW:
            g_menuPW.Hide();
            break;
        case CW:
            g_menuCW.Hide();
            break;
        case CFM:
        case PDI:
        case PWCFM:
        case CWCFM:
            g_menuCFM.Hide();
            break;
        case MEASURE:
            g_menuMeasure.Hide();
            break;
        case BDMK:
            break;
        case REVIEW:
            g_menuReview.Hide();
            break;
        case CALC:
            g_menuCalc.Hide();
            break;
        case NOTE:
            g_menuNote.Hide();
            break;
        case SYSTEM:
            g_menuSystem.Hide();
            break;
        default:
            break;
    }
}

void MenuArea::ShowMenu(void)
{
    m_menuVisbible = TRUE;
    gtk_widget_show(m_fixedMenu);
    ViewMain::GetInstance()->ShowMenu();
}

void MenuArea::HideMenu(void)
{
    m_menuVisbible = FALSE;
    gtk_widget_hide(m_fixedMenu);
    ViewMain::GetInstance()->HideMenu();
}

#ifdef EMP_PROJECT
void MenuArea::ShowProjectModeMenu(void)
{
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        m_menuTypePre = m_menuType;
        m_menuType = PROJECTMODE;
        HideAllOtherMenu();
        gtk_label_set_text(GTK_LABEL(m_labelSub), _("Project Mode"));
        g_menuProjectMode.Show();
    }
}
void MenuArea::HideProjectModeMenu(void)
{
    g_menuProjectMode.Hide();
    SwitchMenu(m_menuType = m_menuTypePre);
}
#endif

void MenuArea::ShowMainMenu(void)
{
    m_menuType = MAIN;

#ifdef EMP_PROJECT
   if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), "");
    //    g_menuMain.Show();
    //    gtk_widget_hide(m_btnExit);
}

void MenuArea::Show2DMenu(void)
{
    m_in2DOnly = true;
    m_menuType = D2;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("2D Mode"));
    gtk_notebook_set_show_tabs (GTK_NOTEBOOK(m_noteBook), TRUE);
    g_menu2D.Show();
    //    gtk_widget_show(m_noteBook);
    //    gtk_widget_show(m_btnExit);
}

void MenuArea::ShowMMenu(void)
{
    m_in2DOnly = false;
    m_menuType = M;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("M Mode"));
    gtk_notebook_set_show_tabs (GTK_NOTEBOOK(m_noteBook), TRUE);
    g_menu2D.Show();
    g_menuM.Show();
    gtk_notebook_set_current_page(GTK_NOTEBOOK(m_noteBook), 1);
}

void MenuArea::ShowPWMenu(void)
{
    m_in2DOnly = false;
    m_menuType = PW;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
#ifdef EMP_355
	if(ViewMain::GetInstance()->GetModeIsFlag())
#else
    if (ModeStatus::IsCWImgMode())
#endif
    {
        gtk_label_set_text(GTK_LABEL(m_labelSub), _("CW Mode"));
        gtk_label_set_text(GTK_LABEL(m_labelPW), _("CW"));
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(m_labelSub), _("PW Mode"));
        gtk_label_set_text(GTK_LABEL(m_labelPW), _("PW"));
    }
    gtk_notebook_set_show_tabs (GTK_NOTEBOOK(m_noteBook), TRUE);
    g_menu2D.Show();
    g_menuPW.Show();
    gtk_notebook_set_current_page(GTK_NOTEBOOK(m_noteBook), 2);
}

void MenuArea::ShowCWMenu(void)
{
    m_in2DOnly = false;
    m_menuType = CW;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("CW Mode"));
    gtk_notebook_set_show_tabs (GTK_NOTEBOOK(m_noteBook), TRUE);
    g_menu2D.Show();
    g_menuCW.Show();
    gtk_notebook_set_current_page(GTK_NOTEBOOK(m_noteBook), 3);
}

void MenuArea::ShowCFMMenu(void)
{
    m_in2DOnly = false;
    m_menuType = CFM;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("Color Mode"));
    gtk_notebook_set_show_tabs (GTK_NOTEBOOK(m_noteBook), TRUE);
    g_menu2D.Show();
    g_menuCFM.Show();
    gtk_notebook_set_current_page(GTK_NOTEBOOK(m_noteBook), 4);
}

void MenuArea::ShowCWCFMMenu(bool currentCw)
{
    m_in2DOnly = false;
    m_menuType = CWCFM;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    if (currentCw) {
    	gtk_label_set_text(GTK_LABEL(m_labelSub), _("CW Color Mode"));
    	gtk_notebook_set_show_tabs (GTK_NOTEBOOK(m_noteBook), TRUE);
    	g_menu2D.Show();
    	g_menuCW.Show();
    	g_menuCFM.Show();
    	gtk_notebook_set_current_page(GTK_NOTEBOOK(m_noteBook), 3);
    } else {
    	gtk_label_set_text(GTK_LABEL(m_labelSub), _("Color CW Mode"));
    	gtk_notebook_set_show_tabs (GTK_NOTEBOOK(m_noteBook), TRUE);
    	g_menu2D.Show();
    	g_menuCW.Show();
    	g_menuCFM.Show();
    	gtk_notebook_set_current_page(GTK_NOTEBOOK(m_noteBook), 4);
    }
    PRINTF("-------------menu type = %d\n", m_menuType);
}
void MenuArea::ShowPWCFMMenu(bool currentPw)
{
    m_in2DOnly = false;
    m_menuType = PWCFM;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    if (currentPw) {
#ifdef EMP_355
    if(ViewMain::GetInstance()->GetModeIsFlag())
#else
        if(ModeStatus::IsCWImgMode())
#endif
        {
            gtk_label_set_text(GTK_LABEL(m_labelPW), _("CW"));
            gtk_label_set_text(GTK_LABEL(m_labelSub), _("CW Color Mode"));
        }
        else
        {
            gtk_label_set_text(GTK_LABEL(m_labelSub), _("PW Color Mode"));
            gtk_label_set_text(GTK_LABEL(m_labelPW), _("PW"));
        }
        gtk_notebook_set_show_tabs (GTK_NOTEBOOK(m_noteBook), TRUE);
    	g_menu2D.Show();
    	g_menuPW.Show();
    	g_menuCFM.Show();
    	gtk_notebook_set_current_page(GTK_NOTEBOOK(m_noteBook), 2);
    } else {
#ifdef EMP_355
 		if(ViewMain::GetInstance()->GetModeIsFlag())
        {
            gtk_label_set_text(GTK_LABEL(m_labelPW), _("CW"));
            gtk_label_set_text(GTK_LABEL(m_labelSub), _("Color CW Mode"));
        }
#else
        if(ModeStatus::IsCWImgMode())
            gtk_label_set_text(GTK_LABEL(m_labelSub), _("Color CW Mode"));
#endif
        else
            gtk_label_set_text(GTK_LABEL(m_labelSub), _("Color PW Mode"));
        gtk_notebook_set_show_tabs (GTK_NOTEBOOK(m_noteBook), TRUE);
    	g_menu2D.Show();
    	g_menuPW.Show();
    	g_menuCFM.Show();
    	gtk_notebook_set_current_page(GTK_NOTEBOOK(m_noteBook), 4);
    }
    PRINTF("-------------menu type = %d\n", m_menuType);
}

void MenuArea::ShowMeasureMenu(void)
{
    m_menuType = MEASURE;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("Measure"));
#if defined(EMP_322)
    g_keyInterface.CtrlLight(TRUE,LIGHT_MEASURE);
#endif
    g_menuMeasure.Show();
}

void MenuArea::ShowBDMKMenu(void)
{
    m_menuType = BDMK;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif
    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("BodyMark"));
#if defined(EMP_322)
    g_keyInterface.CtrlLight(TRUE,LIGHT_BODYMARK);
#endif
    g_menuBDMK.Show();
}

void MenuArea::ShowReviewMenu(void)
{
    m_menuType = REVIEW;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("Review"));
    g_menuReview.Show();
}

void MenuArea::ShowCalcMenu(void)
{
    m_menuType = CALC;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("Calculate"));
#if (defined(EMP_322) || defined(EMP_313))
    g_keyInterface.CtrlLight(TRUE,LIGHT_CALC);
#endif
    g_menuCalc.Show();
}

void MenuArea::ShowNoteMenu(void)
{
    PRINTF("==============enter shownotemenu");
    m_menuType = NOTE;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("Note"));
  // ShowMenu();
    g_menuNote.Show();

}

void MenuArea::ShowBiopsyMenu(void)
{
    m_menuType = BIOPSY;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
   // gtk_label_set_text(GTK_LABEL(m_labelSub), _("System Setting\n\nBiopsy Setting"));
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("Biopsy"));
    g_menuBiopsy.Show();
}
void MenuArea::ShowBioBracketMenu(void)
{
	m_menuType = BIOPSYBRACKET;
    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("Biopsy Bracket"));
    g_menuBiopsyBracket.Show();
}
void MenuArea::ShowBioVerifyMenu(void)
{
    m_menuType = BIOPSYVERIFY;
    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("Verify"));
    g_menuBiopsyVerify.Show();
}

void MenuArea::ShowSystemMenu(void)
{
    m_menuType = SYSTEM;

#ifdef EMP_PROJECT
  if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("System Setting"));
    g_menuSystem.Show();
    PRINTF("-------------------------------show system menu\n");
}

void MenuArea::ShowEFOVPrepare(void)
{
    m_menuType = EFOV;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("eView Mode\n\nPrepare Status"));
    g_menuEFOV.ShowPrepare();
}
void MenuArea::ShowEFOVCapture(void)
{
    m_menuType = EFOV;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("eView Mode\n\nCapture Status"));
    g_menuEFOV.ShowCapture();
}

void MenuArea::ShowEFOVView(void)
{
    m_menuType = EFOV;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("eView Mode\n\nView Status"));
    g_menuEFOV.ShowView();
}

void MenuArea::ShowEFOVReview(void)
{
    m_menuType = EFOV;

#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        ShowProjectModeMenu();
        return;
    }
#endif

    HideAllOtherMenu();
    gtk_label_set_text(GTK_LABEL(m_labelSub), _("eView Mode\n\nReview Status"));
    printf("---%s---\n", __FUNCTION__);
    g_menuEFOV.ShowReview();
}

void MenuArea::HideAllOtherMenu(void)
{
    if(m_menuType != D2)
        g_menu2D.Hide();
    if(m_menuType != M)
        g_menuM.Hide();
    if(m_menuType != PW)
        g_menuPW.Hide();
    if(m_menuType != CW)
        g_menuCW.Hide();
    if(m_menuType != CFM)
        g_menuCFM.Hide();
    if(m_menuType != BDMK)
        g_menuBDMK.Hide();
    if(m_menuType != MEASURE)
        g_menuMeasure.Hide();
    if(m_menuType != REVIEW)
        g_menuReview.Hide();
    if(m_menuType != CALC)
        g_menuCalc.Hide();
    if(m_menuType != NOTE)
        g_menuNote.Hide();
    if(m_menuType != MAIN);
    //	g_menuMain.Hide();
    if(m_menuType != BIOPSY)
        g_menuBiopsy.Hide();
    if(m_menuType != SYSTEM)
        g_menuSystem.Hide();
    if (m_menuType != EFOV)
        g_menuEFOV.Hide();
	//-----------------------------//
	 if(m_menuType != BIOPSYBRACKET)
        g_menuBiopsyBracket.Hide();
	 if(m_menuType != BIOPSYVERIFY)
        g_menuBiopsyVerify.Hide();
	//-------------------------------------//

#ifdef EMP_PROJECT
  g_menuProjectMode.Hide();
#endif

    DarkFucusLight();
}

void MenuArea::GetMenuIndex(int &index)
{
    index = gtk_notebook_get_current_page(GTK_NOTEBOOK(m_noteBook));
}

void MenuArea::NotebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num)
{
    if (m_in2DOnly)
        return ;

    ModeStatus ms;
    if (ms.IsFreezeMode() || ms.IsAutoReplayMode())
        return ;

    switch (page_num) {
        case 0:
            KnobD2Create();
            break;
        case 1:
            if (m_menuType == M)
                KnobMCreate();
            break;
        case 2:
            if ((m_menuType == PW) || (m_menuType == PWCFM) || (m_menuType == CWCFM))
                KnobPwCreate();
            break;
        case 3:
            if ((m_menuType == CW) ||(m_menuType == CWCFM))
                KnobPwCreate();
            break;
        case 4:
            if ((m_menuType == CFM) || (m_menuType == PDI) || (m_menuType == PWCFM) || (m_menuType == CWCFM))
                KnobCfmCreate();
            break;
    }
}

// extern func
void MenuShowUndo()
{
    ModeStatus s;
    ScanMode::EScanMode mode = s.GetScanMode();
    MenuArea::EMenuType type = MenuArea::D2;

	if(Replay::GetInstance()->GetReadSnapStatus())
	{
		type = MenuArea::REVIEW;
	}
	else
	{
		switch(mode)
		{
			case ScanMode::D2:
			case ScanMode::M_INIT:
			case ScanMode::PW_INIT:
			case ScanMode::CW_INIT:
				type = MenuArea::D2;
				break;

			case ScanMode::M:
			case ScanMode::ANATOMIC_M:
				type = MenuArea::M;
				break;

			case ScanMode::PW:
			case ScanMode::PW_SIMULT:
				type = MenuArea::PW;
				break;

			case ScanMode::CW:
				type = MenuArea::CW;
				break;

			case ScanMode::CFM:
			case ScanMode::PDI:
			case ScanMode::PWCFM_INIT:
			case ScanMode::PWPDI_INIT:
			case ScanMode::CFM_VS_2D:
			case ScanMode::PDI_VS_2D:
            case ScanMode::CWCFM_INIT:
            case ScanMode::CWPDI_INIT:
				type = MenuArea::CFM;
				break;

			case ScanMode::PWCFM:
			case ScanMode::PWCFM_SIMULT:
			case ScanMode::PWPDI:
			case ScanMode::PWPDI_SIMULT:
				type = MenuArea::PWCFM;
				break;

            case ScanMode::CWCFM:
			case ScanMode::CWPDI:
				type = MenuArea::CWCFM;
				break;

			case ScanMode::EFOV:
				type = MenuArea::EFOV;
				break;
			default:
				type = MenuArea::D2;
				break;
		}
	}
    MenuArea::GetInstance()->SwitchMenu(type);
}

MenuArea::EMenuType MenuArea::GetNotebookType(void)
{
    int page_index = gtk_notebook_get_current_page(GTK_NOTEBOOK(m_noteBook));
    if (page_index == -1) {
        return m_menuType;
    }
    GtkWidget *child = gtk_notebook_get_nth_page(GTK_NOTEBOOK(m_noteBook), page_index);
    const char *label_text = gtk_notebook_get_menu_label_text(GTK_NOTEBOOK(m_noteBook),child);
    if(label_text == NULL)
        return m_menuType;
    if (strcmp(label_text, gtk_label_get_text(GTK_LABEL(m_label2D))) == 0) {
        return D2;
    } else if (strcmp(label_text, gtk_label_get_text(GTK_LABEL(m_labelM))) == 0) {
        return M;
    } else if (strcmp(label_text, gtk_label_get_text(GTK_LABEL(m_labelPW))) == 0) {
        return PW;
    } else if (strcmp(label_text, gtk_label_get_text(GTK_LABEL(m_labelCW))) == 0) {
        return CW;
    } else if (strcmp(label_text, gtk_label_get_text(GTK_LABEL(m_labelCFM))) == 0) {
        return CFM;
    } else
        return m_menuType;
}