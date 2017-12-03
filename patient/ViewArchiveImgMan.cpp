/*
 * 2010, 深圳恩普电子技术有限公司
 *
 * @file: ViewArchiveImgMan.cpp
 * @brief: display image management dialog, and deal with some operation on image/video
 *
 * version: V1.0
 * date: 2010-3-25
 * @author: zhanglei
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include <dirent.h>
#include "ViewArchiveImgMan.h"
#include "gui_global.h"
#include "KeyValueOpr.h"
#include "gui_func.h"
#include "KeyDef.h"
#include "ViewMain.h"
#include "Database.h"
#include "PatientInfo.h"
#include "ImgMan.h"
#include "Replay.h"
#include "FreezeMode.h"
#include "ViewReport.h"
#include "ViewArchive.h"
#include "ViewPrintPreview.h"
#include "MenuReview.h"
#include "KeyFunc.h"
#include "Usblp.h"
#include "sysMan/SysGeneralSetting.h"
#include "AviEncDecH264.h"

// global
static gboolean TimerReviewCineAuto(gpointer data)
{
	Replay::GetInstance()->ViewOneImg();
	ViewArchiveImgMan::GetInstance()->WhenViewOneImg();
	return TRUE;
}

static gboolean HandlerDisplayCine(gpointer data)
{
	string str = (char*)data;
	ViewArchiveImgMan::GetInstance()->DisplayCine(str);
	return FALSE;
}

static gboolean ExitWindow(gpointer data)
{
    ViewArchiveImgMan *tmp;
    tmp = (ViewArchiveImgMan *)data;
    tmp->DestroyWindow();
    return FALSE;
}


///< class
ViewArchiveImgMan* ViewArchiveImgMan::m_ptrInstance = NULL;

ViewArchiveImgMan::ViewArchiveImgMan()
{
	m_ptrInstance = NULL;
	m_isUdisk = FALSE;
	m_vecExamID.clear();
	m_curPat = 0;
	m_vecFileName.clear();
	m_vecFileSel.clear();
	m_vecCineName.clear();
	m_vecCineSel.clear();
	m_curImgPage = 0;
	m_maxImgPage = 0;
	m_fileType = IMG;
	m_timer = 0;
	m_statusReplay = FALSE;
    m_scrolledwindow_orignial= NULL;
    m_scrolledwindow_auto = NULL;
    m_fixedContent = NULL;
    m_fixedReport = NULL;
    m_fixedPatInfo = NULL;
}

ViewArchiveImgMan::~ViewArchiveImgMan()
{
	if (m_ptrInstance != NULL)
	{
		delete m_ptrInstance;
	}
}
ViewArchiveImgMan* ViewArchiveImgMan::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new ViewArchiveImgMan;

	return m_ptrInstance;
}

void ViewArchiveImgMan::InitParameter(vector<string> examID, const char *selected)
{
    m_scrolledwindow_auto = NULL;
    m_scrolledwindow_orignial = NULL;
    m_fixedContent = NULL;
    m_fixedReport = NULL;
    m_fixedPatInfo = NULL;
	m_imgPixbuf = NULL;
	m_cinePixbuf = NULL;
	if(!m_isUdisk)
		m_vecExamID = examID;
	else
	{
		m_vecExamID.clear();
		m_vecExamID.push_back("0");
	}
	m_curPat = 0;
//	m_fileType = IMG;
	m_vecFileName.clear();
	m_vecFileSel.clear();
	m_curImgPage = 0;
	m_maxImgPage = 0;
	m_vecCineName.clear();
	m_vecCineSel.clear();
	m_curCinePage = 0;
	m_maxCinePage = 0;
	m_cineCur = 0;
    m_cineTotal = 0;
    m_fps = 0;
    m_timer = 0;
    m_statusReplay = FALSE;

    if(m_isUdisk)
    {
        m_fileType = IMG;
    }
    else
    {
        if(selected)
        {
            //printf("selected = %s\n", selected);
            string name = selected;
            string ext = name.substr(name.rfind('.') == string::npos ? name.length() : name.rfind('.') + 1);
            if((ext == "avi")||(ext == "cine"))
                m_fileType = CINE;
            else
                m_fileType = IMG;
        }
    }
}

gboolean ShowFirstImg(gpointer data)
{
	ViewArchiveImgMan *tmp;
	tmp = (ViewArchiveImgMan *)data;
	tmp->DisplayFirstImage();
	return FALSE;
}

gboolean ShowFirstCine(gpointer data)
{
	ViewArchiveImgMan *tmp;
	tmp = (ViewArchiveImgMan *)data;
	tmp->DisplayFirstCine();
	return FALSE;
}

gboolean ShowSelectedImg(gpointer data)
{
	int i = GPOINTER_TO_INT(data);
	ViewArchiveImgMan::GetInstance()->DisplaySelectedImage(i);
	return FALSE;
}

gboolean ShowSelectedCine(gpointer data)
{
	int i = GPOINTER_TO_INT(data);
	ViewArchiveImgMan::GetInstance()->DisplaySelectedCine(i);
	return FALSE;
}

void ViewArchiveImgMan::CreateWindow(vector<string> examID, const char *selected)
{
	GtkWidget *fixed;
	GtkWidget *frameFile;
	GtkWidget *fixedImg;
	GtkWidget *framePatSel;
	GtkWidget *fixedPat;
	GtkWidget *hboxPatSel;
	GtkWidget *btnLastPat;
	GtkWidget *btnNextPat;
	GtkWidget *hboxPatInfo;
	GtkWidget *vseparatorPat;
	GtkWidget *vboxPatStat;
	GtkWidget *vseparator2;
	GtkWidget *labelPatSel;
//	GtkWidget *labelType;
	GtkWidget *alignment;
	GtkWidget *image;
	GtkWidget *labImg;

	if(examID.size() == 0)
		m_isUdisk = TRUE;
	else
		m_isUdisk = FALSE;

	InitParameter(examID, selected);

	// main window
	m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (m_window, SCREEN_WIDTH, SCREEN_HEIGHT);
	gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
	gtk_window_set_title (GTK_WINDOW (m_window), _("Image manage"));
	gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
	gtk_window_set_decorated (GTK_WINDOW (m_window), FALSE);
	gtk_window_set_transient_for (GTK_WINDOW (m_window), GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()));

	fixed = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (m_window), fixed);

	// patient selection region
	framePatSel = gtk_frame_new (NULL);
	gtk_fixed_put (GTK_FIXED (fixed), framePatSel, 174, 5);
	gtk_widget_set_size_request (framePatSel, 830, 60);
	gtk_frame_set_shadow_type (GTK_FRAME (framePatSel), GTK_SHADOW_IN);

	alignment = gtk_alignment_new (0.1, 0.5, 0.9, 1);
	gtk_container_add (GTK_CONTAINER (framePatSel), alignment);

	fixedPat = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (alignment), fixedPat);
  
	hboxPatSel = gtk_hbox_new (TRUE, 0);
	gtk_fixed_put (GTK_FIXED (fixedPat), hboxPatSel, 0, 0);
	gtk_widget_set_size_request (hboxPatSel, 70, 35);

	btnLastPat = gtk_button_new ();
	gtk_box_pack_start (GTK_BOX (hboxPatSel), btnLastPat, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(btnLastPat), "clicked", G_CALLBACK(HandleBtnLastPatClicked), this);

	image = gtk_image_new_from_stock ("gtk-go-back", GTK_ICON_SIZE_BUTTON);
	gtk_container_add (GTK_CONTAINER (btnLastPat), image);

	btnNextPat = gtk_button_new ();
	gtk_box_pack_start (GTK_BOX (hboxPatSel), btnNextPat, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(btnNextPat), "clicked", G_CALLBACK(HandleBtnNextPatClicked), this);

	image = gtk_image_new_from_stock ("gtk-go-forward", GTK_ICON_SIZE_BUTTON);
	gtk_container_add (GTK_CONTAINER (btnNextPat), image);	 

	hboxPatInfo = gtk_hbox_new (TRUE, 0);
	gtk_fixed_put (GTK_FIXED (fixedPat), hboxPatInfo, 145, 0);
	gtk_widget_set_size_request (hboxPatInfo, 650, 35);

	m_labName = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hboxPatInfo), m_labName, FALSE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labName), 0, 0.5);

//hlx 2012.1.12
#ifdef VET
	m_labOwner = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hboxPatInfo), m_labOwner, FALSE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labOwner), 0, 0.5);

	m_labSpecies = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hboxPatInfo), m_labSpecies, FALSE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labSpecies), 0, 0.5);
#endif


	m_labSex = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hboxPatInfo), m_labSex, FALSE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labSex), 0, 0.5);

	m_labBirthday = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hboxPatInfo), m_labBirthday, FALSE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labBirthday), 0, 0.5);

	m_labExamNum = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hboxPatInfo), m_labExamNum, FALSE, FALSE, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labExamNum), 0, 0.5);	

	vseparatorPat = gtk_vseparator_new ();
	gtk_fixed_put (GTK_FIXED (fixedPat), vseparatorPat, 65, 0);
	gtk_widget_set_size_request (vseparatorPat, 20, 35);

	vboxPatStat = gtk_vbox_new (TRUE, 0);
	gtk_fixed_put (GTK_FIXED (fixedPat), vboxPatStat, 80, 0);
	gtk_widget_set_size_request (vboxPatStat, 55, 35);

	m_labStatInfo = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (vboxPatStat), m_labStatInfo, FALSE, FALSE, 0);

	vseparator2 = gtk_vseparator_new ();
	gtk_fixed_put (GTK_FIXED (fixedPat), vseparator2, 130, 0);
	gtk_widget_set_size_request (vseparator2, 20, 35);
#ifdef VET
	labelPatSel = gtk_label_new (_("<b>Animal</b>"));
#else
	labelPatSel = gtk_label_new (_("<b>Patient</b>"));
#endif
	//labelPatSel = gtk_label_new (_("<b>Patient</b>"));
	gtk_frame_set_label_widget (GTK_FRAME (framePatSel), labelPatSel);
	gtk_label_set_use_markup (GTK_LABEL (labelPatSel), TRUE);	

	// File Select 
	frameFile = gtk_frame_new (NULL);
	gtk_fixed_put (GTK_FIXED (fixed), frameFile, 20, 5);
	gtk_widget_set_size_request (frameFile, 145, 60); 
	gtk_frame_set_shadow_type (GTK_FRAME (frameFile), GTK_SHADOW_IN);

	alignment = gtk_alignment_new (0, 0.5, 0.9, 1);
	gtk_container_add (GTK_CONTAINER (frameFile), alignment);

	labImg = gtk_label_new (_("<b>Type</b>"));
	gtk_frame_set_label_widget (GTK_FRAME (frameFile), labImg);
	gtk_label_set_use_markup (GTK_LABEL (labImg), TRUE);

	fixedImg = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (alignment), fixedImg);

	m_comboType = gtk_combo_box_new_text ();
	gtk_fixed_put (GTK_FIXED (fixedImg), m_comboType, 10, 4);
	gtk_widget_set_size_request (m_comboType, 120, 30);
	gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboType), _("Image"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboType), _("Cine"));
	if(!m_isUdisk)
	{
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboType), _("Report"));
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboType), _("Info."));
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboType), m_fileType);	
	g_signal_connect(GTK_COMBO_BOX(m_comboType), "changed", G_CALLBACK(HandleComboTypeClicked), this);

	// fixed contenct
	m_fixedReport = CreateReportArea(fixed);
	
	// fixed report
	m_fixedContent = CreateImgArea(fixed);

	// fixed patient info
	m_fixedPatInfo = CreatePatInfoArea(fixed);

    SysGeneralSetting sysGeneralSetting;
    if(sysGeneralSetting.GetPrintViewMode())
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_orignial), TRUE);
    else
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_auto), TRUE);

	// show 
	g_keyInterface.Push(this);
	gtk_widget_show_all(m_window);
    HideAllFixed();
    if(m_fileType == INFO)
    {
        gtk_widget_show_all(m_fixedPatInfo);
        gtk_fixed_move(GTK_FIXED(m_fixedContent), m_hboxFunc, 734, 643);
    }
    else if(m_fileType == CINE)
    {
        gtk_widget_show_all(m_fixedContent);
        gtk_widget_hide(m_btnShow);  
        gtk_fixed_move(GTK_FIXED(m_fixedContent), m_hboxFunc, 734+120+10, 643);
        gtk_widget_queue_draw(m_drawArea);
        UpdateSnap();
        g_timeout_add(200, ShowFirstCine, this);
    }
    else if(m_fileType ==  REPORT)
    {
        gtk_widget_show_all(m_fixedReport);
        gtk_fixed_move(GTK_FIXED(m_fixedContent), m_hboxFunc, 734, 643);
  
/*    
      SysGeneralSetting sysGeneralSetting;
      if(sysGeneralSetting.GetPrintViewMode())
      {
      gtk_widget_hide_all (m_scrolledwindow_auto);
      gtk_widget_show_all (m_scrolledwindow_orignial);
      }
      else
      {
      gtk_widget_hide_all (m_scrolledwindow_orignial);
      gtk_widget_show_all (m_scrolledwindow_auto);
      }
*/
    }
    else
    {
        gtk_widget_show_all(m_fixedContent);
        gtk_fixed_move(GTK_FIXED(m_fixedContent), m_hboxFunc, 734, 643);
		DisplayCineMenu(FALSE);
		gtk_widget_queue_draw(m_drawArea);
		UpdateSnap();
		g_timeout_add(200, ShowFirstImg, this);
    }
    SetSystemCursorToCenter();

    //init
	m_curPat = 0;
	UpdateCurPat(m_vecExamID[m_curPat], m_curPat, m_vecExamID.size());
//	DisplayCineMenu(FALSE);
	PRINTF("end of ViewArchiveImgMan: create window\n");


	if(selected)
	{
		int total;
		if(m_fileType == IMG)
		{
			total = m_vecFileName.size();
			if(total != 0)
			{
				for(int i=0; i<total; i++)
				{
					gchar *basename = g_path_get_basename(m_vecFileName[i].c_str());
					if(strcmp(basename, selected) == 0)
						g_timeout_add(200, ShowSelectedImg, GINT_TO_POINTER(i));
					g_free(basename);
				}
			}
		}
		else if(m_fileType == CINE)
		{
			total = m_vecCineName.size();
			if(total != 0)
			{
				for(int i=0; i<total; i++)
				{
					gchar *basename = g_path_get_basename(m_vecCineName[i].c_str());
					if(strcmp(basename, selected) == 0)
						g_timeout_add(200, ShowSelectedCine, GINT_TO_POINTER(i));
					g_free(basename);
				}
			}
		}
	}
//	else
//		g_timeout_add(200, ShowFirstImg, this);
}

void ViewArchiveImgMan::AutoRadioToggled(GtkToggleButton *togglebutton)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_auto))) 
    {
        SysGeneralSetting sysGeneralSetting;
        sysGeneralSetting.SetPrintViewMode(0);
        sysGeneralSetting.SyncFile();

        if(GTK_IS_WIDGET(m_scrolledwindow_orignial))
            gtk_widget_hide_all (m_scrolledwindow_orignial);
        if(!m_scrolledwindow_auto)
        {
            m_scrolledwindow_auto = CreateReport(m_vecExamID[m_curPat], 0);
        }
        if(GTK_IS_WIDGET(m_scrolledwindow_auto))
            gtk_widget_show_all(m_scrolledwindow_auto);
    } 

}

void ViewArchiveImgMan::OrignialRadioToggled(GtkToggleButton *togglebutton)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_orignial))) 
    {
        SysGeneralSetting sysGeneralSetting;
        sysGeneralSetting.SetPrintViewMode(1);
        sysGeneralSetting.SyncFile();

        if(GTK_IS_WIDGET(m_scrolledwindow_auto))
            gtk_widget_hide_all (m_scrolledwindow_auto);
        if(!m_scrolledwindow_orignial)
        {
            m_scrolledwindow_orignial = CreateReport(m_vecExamID[m_curPat], 1);
        }
        if(GTK_IS_WIDGET(m_scrolledwindow_orignial))
            gtk_widget_show_all (m_scrolledwindow_orignial);
    } 
}

GtkWidget* ViewArchiveImgMan::CreateReportArea(GtkWidget *fixed)
{
	GtkWidget* fixedReport;
	GtkWidget* hboxReportFunc;
	GtkWidget* btnPrint;
	GtkWidget* btnExitReport;

	// fixed report
	fixedReport = gtk_fixed_new ();
	gtk_fixed_put (GTK_FIXED (fixed), fixedReport, 20, 80);
	gtk_widget_set_size_request (fixedReport, 984, 688);

    //View Mode
    GtkWidget* frame_print = gtk_frame_new (NULL);
    gtk_widget_show (frame_print);
    gtk_fixed_put (GTK_FIXED (fixedReport), frame_print, 304-40, 618);
    gtk_widget_set_size_request (frame_print, 270+40, 50);
    gtk_frame_set_label_align (GTK_FRAME (frame_print), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_print), GTK_SHADOW_IN);

    GtkWidget* label_print = gtk_label_new (_("View Mode"));
    gtk_widget_show (label_print);
    gtk_frame_set_label(GTK_FRAME (frame_print), _("View Mode"));
    gtk_label_set_use_markup (GTK_LABEL (label_print), TRUE);

    GtkWidget* hbox_print = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_print);
    gtk_container_add (GTK_CONTAINER (frame_print), hbox_print);
    GSList *radiobutton_printview_group = NULL;
    m_radiobutton_auto = gtk_radio_button_new_with_mnemonic (NULL, _("Auto Size"));
    gtk_widget_show (m_radiobutton_auto);
    gtk_box_pack_start (GTK_BOX (hbox_print), m_radiobutton_auto, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_auto), radiobutton_printview_group);
    radiobutton_printview_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_auto));
    g_signal_connect((gpointer)m_radiobutton_auto, "toggled", G_CALLBACK (on_auto_radio_button_toggled), this);

    m_radiobutton_orignial= gtk_radio_button_new_with_mnemonic (NULL, _("Orignial Size"));
    gtk_widget_show (m_radiobutton_orignial);
    gtk_box_pack_start (GTK_BOX (hbox_print), m_radiobutton_orignial, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_orignial), radiobutton_printview_group);
    radiobutton_printview_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_orignial));
    g_signal_connect((gpointer)m_radiobutton_orignial, "toggled", G_CALLBACK (on_orignial_radio_button_toggled), this);
  
	hboxReportFunc = gtk_hbox_new (TRUE, 10);
	gtk_fixed_put (GTK_FIXED (fixedReport), hboxReportFunc, 734, 608);
	gtk_widget_set_size_request (hboxReportFunc, 250, 60);

	GtkWidget *image = gtk_image_new_from_stock("gtk-print", GTK_ICON_SIZE_BUTTON);
	GtkWidget *label = gtk_label_new_with_mnemonic (_("Print"));
	btnPrint = create_button_icon(label, image);
	gtk_box_pack_start(GTK_BOX (hboxReportFunc), btnPrint, FALSE, TRUE, 0);
	g_signal_connect(G_OBJECT(btnPrint), "clicked", G_CALLBACK(HandleBtnPrintClicked), this);

	image = gtk_image_new_from_stock("gtk-quit", GTK_ICON_SIZE_BUTTON);
	label = gtk_label_new_with_mnemonic (_("Exit"));
	btnExitReport = create_button_icon(label, image);
	gtk_box_pack_start(GTK_BOX (hboxReportFunc), btnExitReport, FALSE, TRUE, 0);
	g_signal_connect(G_OBJECT(btnExitReport), "clicked", G_CALLBACK(HandleBtnExitClicked), this);

	return fixedReport;
}

GtkWidget* ViewArchiveImgMan::CreateImgArea(GtkWidget *fixed)
{
	GtkWidget *fixedContent;
	GtkWidget *tableImg;
	GtkWidget *hboxImgSel;
	GtkWidget *btnLastImg;
	GtkWidget *btnNextImg;
	GtkWidget *image;
//	GtkWidget *btnSelAll;
//	GtkWidget *btnUndoSel;
//	GtkWidget *btnDelete;
	GtkWidget *btnExit;

	fixedContent = gtk_fixed_new ();
	gtk_fixed_put (GTK_FIXED (fixed), fixedContent, 20, 70);
	gtk_widget_set_size_request (fixedContent, 1004, 693);

	// create snap table
	GtkWidget* framePreview;
	framePreview = gtk_frame_new(NULL);
	gtk_fixed_put(GTK_FIXED(fixedContent), framePreview, 0, 0);
	gtk_widget_set_size_request(framePreview, 145, 688);
	gtk_frame_set_shadow_type(GTK_FRAME(framePreview), GTK_SHADOW_IN);

	GtkWidget *fixedImg = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(framePreview), fixedImg);

	tableImg = gtk_table_new (SNAP_ROW, SNAP_COL, TRUE);
	gtk_fixed_put(GTK_FIXED(fixedImg), tableImg, 0, 0);
	gtk_widget_set_size_request(tableImg, 140, 645);
	gtk_container_set_border_width(GTK_CONTAINER(tableImg), 1);

	int i, j;
	for(i = 0; i < SNAP_ROW; i ++)
	{
		for(j = 0; j < SNAP_COL; j ++)
		{
			sprintf(m_id[i*SNAP_COL+j], "%d", i*SNAP_COL+j);
			m_vbox[i*SNAP_COL+j] = CreateItem (&m_btnSnap[i*SNAP_COL+j], &m_checkBtnSnap[i*SNAP_COL+j], m_id[i*SNAP_COL+j]);
			gtk_table_attach_defaults(GTK_TABLE(tableImg), m_vbox[i*SNAP_COL+j], j, j+1, i, i+1);
		}
	}
	
	hboxImgSel = gtk_hbox_new (TRUE, 0);
	gtk_fixed_put(GTK_FIXED(fixedImg), hboxImgSel, 0, 653);
	gtk_widget_set_size_request (hboxImgSel, 140, 30);

	btnLastImg = gtk_button_new_with_mnemonic ("◀");
	gtk_box_pack_start (GTK_BOX (hboxImgSel), btnLastImg, FALSE, TRUE, 0);
	g_signal_connect(G_OBJECT(btnLastImg), "clicked", G_CALLBACK(HandleBtnLastImgPageClicked), this);

	m_labImgPage = gtk_label_new ("5/16");
	gtk_box_pack_start (GTK_BOX (hboxImgSel), m_labImgPage, FALSE, FALSE, 0);

	btnNextImg = gtk_button_new_with_mnemonic ("▶");
	gtk_box_pack_start (GTK_BOX (hboxImgSel), btnNextImg, FALSE, TRUE, 0);
	g_signal_connect(G_OBJECT(btnNextImg), "clicked", G_CALLBACK(HandleBtnNextImgPageClicked), this);

	// display large image of original size
	m_drawArea = gtk_drawing_area_new (); //800*640
	gtk_fixed_put (GTK_FIXED (fixedContent), m_drawArea, 145, 0);
	gtk_widget_set_size_request (m_drawArea, IMG_WIDTH, IMG_HEIGHT);
	g_signal_connect(G_OBJECT(m_drawArea), "configure_event", G_CALLBACK(HandleDrawAreaConfigure), this);
	g_signal_connect(G_OBJECT(m_drawArea), "expose_event", G_CALLBACK(HandleDrawAreaExpose), this);

	// cine operation region
	m_hboxCineReplay = gtk_hbox_new (FALSE, 0);
	gtk_fixed_put (GTK_FIXED (fixedContent), m_hboxCineReplay, 164, 652);
	gtk_widget_set_size_request (m_hboxCineReplay, 180, 25);

	m_btnReplay = gtk_button_new();
	gtk_box_pack_start (GTK_BOX (m_hboxCineReplay), m_btnReplay, FALSE, FALSE, 0);
	image = gtk_image_new_from_stock ("gtk-media-play", GTK_ICON_SIZE_BUTTON);
	gtk_container_add (GTK_CONTAINER (m_btnReplay), image);
	g_signal_connect(G_OBJECT(m_btnReplay), "clicked", G_CALLBACK(HandleBtnReplayClicked), this);

	m_progressBar = gtk_progress_bar_new ();
	gtk_box_pack_start (GTK_BOX (m_hboxCineReplay), m_progressBar, FALSE, FALSE, 0);
	gtk_widget_set_size_request (m_progressBar, 150, 20);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (m_progressBar), 0.0);
	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (m_progressBar), "0/0");	

	// function button region 2
	m_hboxFunc = gtk_hbox_new(TRUE, 10);
	gtk_fixed_put(GTK_FIXED(fixedContent), m_hboxFunc, 734, 643);
//	gtk_widget_set_size_request(m_hboxFunc, 380, 45);
	gtk_widget_set_size_request(m_hboxFunc, -1, 45);

	//show current 
	image = gtk_image_new_from_stock("gtk-yes", GTK_ICON_SIZE_BUTTON);
	GtkWidget *label = gtk_label_new_with_mnemonic (_("Slide Show"));
	m_btnShow = create_button_icon(label, image);
	gtk_box_pack_start (GTK_BOX (m_hboxFunc), m_btnShow, FALSE, TRUE, 0);
	g_signal_connect(G_OBJECT(m_btnShow), "clicked", G_CALLBACK(HandleBtnShowClicked), this);

	//exit image
	GtkWidget *imageExit = gtk_image_new_from_stock("gtk-quit", GTK_ICON_SIZE_BUTTON);
	GtkWidget *labelExit = gtk_label_new_with_mnemonic (_("Exit"));
	btnExit = create_button_icon(labelExit, imageExit);
	gtk_box_pack_start(GTK_BOX(m_hboxFunc), btnExit, FALSE, TRUE, 0);
	g_signal_connect(G_OBJECT(btnExit), "clicked", G_CALLBACK(HandleBtnExitClicked), this);

	return fixedContent;
}

// create patient info dialog
GtkWidget* ViewArchiveImgMan::CreatePatInfoArea(GtkWidget *fixed)
{
	GtkWidget *fixedPatInfo = gtk_fixed_new ();
	gtk_fixed_put (GTK_FIXED (fixed), fixedPatInfo, 20, 70);
	gtk_widget_set_size_request (fixedPatInfo, 1004, 683);

	// function btn
	GtkWidget *hboxPatInfoFunc = gtk_hbox_new (FALSE, 0);
	gtk_fixed_put (GTK_FIXED (fixedPatInfo), hboxPatInfoFunc, 864, 618);
	gtk_widget_set_size_request (hboxPatInfoFunc, 120, 60);

	GtkWidget *image = gtk_image_new_from_stock("gtk-quit", GTK_ICON_SIZE_BUTTON);
	GtkWidget *label = gtk_label_new_with_mnemonic (_("Exit"));
	GtkWidget *btnExitPatInfo = create_button_icon(label, image);
	gtk_box_pack_start(GTK_BOX(hboxPatInfoFunc), btnExitPatInfo, FALSE, TRUE, 0);
	g_signal_connect(G_OBJECT(btnExitPatInfo), "clicked", G_CALLBACK(HandleBtnExitClicked), this);

	//patient info
	GtkWidget *frameGenInfo = gtk_frame_new (NULL);
	gtk_fixed_put (GTK_FIXED (fixedPatInfo), frameGenInfo, 0, 10);
	gtk_widget_set_size_request (frameGenInfo, 984, 250);
	gtk_frame_set_shadow_type (GTK_FRAME (frameGenInfo), GTK_SHADOW_OUT);

	GtkWidget *fixedGenInfo = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (frameGenInfo), fixedGenInfo);

	GtkWidget *table5 = gtk_table_new (3, 4, FALSE);
	gtk_fixed_put (GTK_FIXED (fixedGenInfo), table5, 10, 10);
	gtk_widget_set_size_request (table5, 959, 100);
//hlx 2012
#ifdef VET
	GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox3, 0, 1, 0, 1,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

	GtkWidget *labName = gtk_label_new (_("Animal Name:  "));
	gtk_box_pack_start (GTK_BOX (hbox3), labName, FALSE, FALSE, 0);

	m_labEntryName = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox3), m_labEntryName, FALSE, FALSE, 0);

	GtkWidget *hbox31 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox31, 0, 1, 1, 2,//0, 1, 0, 1,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

	GtkWidget *lableSpecies = gtk_label_new (_("Species: "));
	gtk_box_pack_start (GTK_BOX (hbox31), lableSpecies, FALSE, FALSE, 0);

	m_labEntrySpecies = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox31), m_labEntrySpecies, FALSE, FALSE, 0);

	//
	GtkWidget *hbox4 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox4, 0, 1, 2, 3,//0, 1, 1, 2,
			//(GtkAttachOptions) (GTK_EXPAND),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

	GtkWidget *labExamID = gtk_label_new (_("Exam ID:  "));
	gtk_box_pack_start (GTK_BOX (hbox4), labExamID, FALSE, FALSE, 0);

	m_labEntryExamID = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox4), m_labEntryExamID, FALSE, FALSE, 0);

	//
	GtkWidget *hbox5 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox5, 0, 1, 3, 4,//0, 1, 2, 3,
            //(GtkAttachOptions) (GTK_EXPAND),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

	GtkWidget *labPatID = gtk_label_new (_("ID:  "));
	gtk_box_pack_start (GTK_BOX (hbox5), labPatID, FALSE, FALSE, 0);

	m_labEntryPatID = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox5), m_labEntryPatID, FALSE, FALSE, 0);

	//
	//add
	GtkWidget *hbox61 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox61, 1, 2, 0, 1,
		//(GtkAttachOptions) (GTK_EXPAND),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget* labOwner = gtk_label_new (_("Owner:  "));
	gtk_box_pack_start (GTK_BOX (hbox61), labOwner, FALSE, FALSE, 0);

	m_labEntryOwner = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox61), m_labEntryOwner, FALSE, FALSE, 0);

	GtkWidget *hbox6 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox6, 1, 2, 1, 2,//1, 2, 0, 1,
			//(GtkAttachOptions) (GTK_EXPAND ),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget* labAge = gtk_label_new (_("Age:  "));
	gtk_box_pack_start (GTK_BOX (hbox6), labAge, FALSE, FALSE, 0);

	m_labEntryAge = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox6), m_labEntryAge, FALSE, FALSE, 0);

	//
	GtkWidget *hbox7 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox7, 1, 2, 2, 3,//1, 2, 1, 2,
			//(GtkAttachOptions) (GTK_EXPAND),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labGender = gtk_label_new (_("Gender:  "));
	gtk_box_pack_start (GTK_BOX (hbox7), labGender, FALSE, FALSE, 0);

	m_labEntryGender = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox7), m_labEntryGender, FALSE, FALSE, 0);

	//
	GtkWidget *hbox8 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox8, 1, 2, 3, 4,//1, 2, 2, 3,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labBirth = gtk_label_new (_("Date of Birth:  "));
	gtk_box_pack_start (GTK_BOX (hbox8), labBirth, FALSE, FALSE, 0);

	m_labEntryBirth = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox8), m_labEntryBirth, FALSE, FALSE, 0);

	//
	GtkWidget *hbox9 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox9, 2, 3, 0, 1,
			//(GtkAttachOptions) (GTK_EXPAND),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labExamDate = gtk_label_new (_("Exam Date:  "));
	gtk_box_pack_start (GTK_BOX (hbox9), labExamDate, FALSE, FALSE, 0);

	m_labEntryExamDate = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox9), m_labEntryExamDate, FALSE, FALSE, 0);

	//
	GtkWidget *hbox10 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox10, 2, 3, 1, 2,
			//(GtkAttachOptions) (GTK_EXPAND),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labExamTime = gtk_label_new (_("Exam Time:  "));
	gtk_box_pack_start (GTK_BOX (hbox10), labExamTime, FALSE, FALSE, 0);

	m_labEntryExamTime = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox10), m_labEntryExamTime, FALSE, FALSE, 0);

	//
	GtkWidget *hbox11 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox11, 2, 3, 2, 3,
			//(GtkAttachOptions) (GTK_EXPAND),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labExamType = gtk_label_new (_("Exam Type:  "));
	gtk_box_pack_start (GTK_BOX (hbox11), labExamType, FALSE, FALSE, 0);

	m_labEntryExamType = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox11), m_labEntryExamType, FALSE, FALSE, 0);

	//
	GtkWidget *hbox12 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox12, 3, 4, 0, 1,
			//(GtkAttachOptions) (GTK_EXPAND),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labPhysician = gtk_label_new (_("Physician:  "));
	gtk_box_pack_start (GTK_BOX (hbox12), labPhysician, FALSE, FALSE, 0);

	m_labEntryPhysician = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox12), m_labEntryPhysician, FALSE, FALSE, 0);

	//
	GtkWidget *hbox13 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox13, 3, 4, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labDiagnositic = gtk_label_new (_("Diagnostician:  "));
	gtk_box_pack_start (GTK_BOX (hbox13), labDiagnositic, FALSE, FALSE, 0);

	m_labEntryDiagnostic = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox13), m_labEntryDiagnostic, FALSE, FALSE, 0);
#else
	//
	GtkWidget *hbox3 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox3, 0, 1, 0, 1,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

	GtkWidget *labName = gtk_label_new (_("Name:  "));
	gtk_box_pack_start (GTK_BOX (hbox3), labName, FALSE, FALSE, 0);

	m_labEntryName = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox3), m_labEntryName, FALSE, FALSE, 0);

	//
	GtkWidget *hbox4 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox4, 0, 1, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

	GtkWidget *labExamID = gtk_label_new (_("Exam ID:  "));
	gtk_box_pack_start (GTK_BOX (hbox4), labExamID, FALSE, FALSE, 0);

	m_labEntryExamID = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox4), m_labEntryExamID, FALSE, FALSE, 0);

	//
	GtkWidget *hbox5 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox5, 0, 1, 2, 3,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

	GtkWidget *labPatID = gtk_label_new (_("Patient ID:  "));
	gtk_box_pack_start (GTK_BOX (hbox5), labPatID, FALSE, FALSE, 0);

	m_labEntryPatID = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox5), m_labEntryPatID, FALSE, FALSE, 0);

	//
	GtkWidget *hbox6 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox6, 1, 2, 0, 1,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget* labAge = gtk_label_new (_("Age:  "));
	gtk_box_pack_start (GTK_BOX (hbox6), labAge, FALSE, FALSE, 0);

	m_labEntryAge = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox6), m_labEntryAge, FALSE, FALSE, 0);

	//
	GtkWidget *hbox7 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox7, 1, 2, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labGender = gtk_label_new (_("Gender:  "));
	gtk_box_pack_start (GTK_BOX (hbox7), labGender, FALSE, FALSE, 0);

	m_labEntryGender = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox7), m_labEntryGender, FALSE, FALSE, 0);

	//
	GtkWidget *hbox8 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox8, 1, 2, 2, 3,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labBirth = gtk_label_new (_("Date of Birth:  "));
	gtk_box_pack_start (GTK_BOX (hbox8), labBirth, FALSE, FALSE, 0);

	m_labEntryBirth = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox8), m_labEntryBirth, FALSE, FALSE, 0);

	//
	GtkWidget *hbox9 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox9, 2, 3, 0, 1,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labExamDate = gtk_label_new (_("Exam Date:  "));
	gtk_box_pack_start (GTK_BOX (hbox9), labExamDate, FALSE, FALSE, 0);

	m_labEntryExamDate = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox9), m_labEntryExamDate, FALSE, FALSE, 0);

	//
	GtkWidget *hbox10 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox10, 2, 3, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labExamTime = gtk_label_new (_("Exam Time:  "));
	gtk_box_pack_start (GTK_BOX (hbox10), labExamTime, FALSE, FALSE, 0);

	m_labEntryExamTime = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox10), m_labEntryExamTime, FALSE, FALSE, 0);

	//
	GtkWidget *hbox11 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox11, 2, 3, 2, 3,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labExamType = gtk_label_new (_("Exam Type:  "));
	gtk_box_pack_start (GTK_BOX (hbox11), labExamType, FALSE, FALSE, 0);

	m_labEntryExamType = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox11), m_labEntryExamType, FALSE, FALSE, 0);

	//
	GtkWidget *hbox12 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox12, 3, 4, 0, 1,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labPhysician = gtk_label_new (_("Physician:  "));
	gtk_box_pack_start (GTK_BOX (hbox12), labPhysician, FALSE, FALSE, 0);

	m_labEntryPhysician = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox12), m_labEntryPhysician, FALSE, FALSE, 0);

	//
	GtkWidget *hbox13 = gtk_hbox_new (FALSE, 0);
	gtk_table_attach (GTK_TABLE (table5), hbox13, 3, 4, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (GTK_FILL), 0, 0);

	GtkWidget *labDiagnositic = gtk_label_new (_("Diagnostician:  "));
	gtk_box_pack_start (GTK_BOX (hbox13), labDiagnositic, FALSE, FALSE, 0);

	m_labEntryDiagnostic = gtk_label_new ("");
	gtk_box_pack_start (GTK_BOX (hbox13), m_labEntryDiagnostic, FALSE, FALSE, 0);
#endif
	//
	//GtkWidget *labComment = gtk_label_new (_("Comment:"));
	//gtk_fixed_put (GTK_FIXED (fixedGenInfo), labComment, 16, 448);
	//gtk_widget_set_size_request (labComment, 80, 30);

	GtkWidget *hboxComment = gtk_hbox_new (FALSE, 0);
	gtk_fixed_put (GTK_FIXED (fixedGenInfo), hboxComment, 50, 130);
	gtk_widget_set_size_request (hboxComment, 884, 80);

	GtkWidget *labCommentText = gtk_label_new (_("Comment:  "));
	gtk_box_pack_start (GTK_BOX (hboxComment), labCommentText, FALSE, FALSE, 0);
	gtk_misc_set_alignment (GTK_MISC (labCommentText), 0, 0.5);

	GtkWidget *scrolwinComment = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_start (GTK_BOX (hboxComment), scrolwinComment, TRUE, TRUE, 0);
	gtk_widget_set_size_request (scrolwinComment, 800, 80);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolwinComment), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolwinComment), GTK_SHADOW_OUT);

	m_textviewComment = gtk_text_view_new ();
	gtk_container_add (GTK_CONTAINER (scrolwinComment), m_textviewComment);
	gtk_text_view_set_editable(GTK_TEXT_VIEW (m_textviewComment), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(m_textviewComment), GTK_WRAP_WORD_CHAR);

	GtkWidget *labGenInfo = gtk_label_new (_("<b>General Information</b>"));
	gtk_frame_set_label_widget (GTK_FRAME (frameGenInfo), labGenInfo);
	gtk_label_set_use_markup (GTK_LABEL (labGenInfo), TRUE);

	// notebook
	GtkWidget *notebook = gtk_notebook_new ();
	gtk_fixed_put (GTK_FIXED (fixedPatInfo), notebook, 0, 270);
	gtk_widget_set_size_request (notebook, 984, 338);

	GtkWidget *fixedTabGen = gtk_fixed_new ();
//hlx
#ifndef VET
	gtk_container_add (GTK_CONTAINER (notebook), fixedTabGen);
#endif
	//gtk_container_add (GTK_CONTAINER (notebook), fixedTabGen);

	GtkWidget *tableGen1 = gtk_table_new (3, 2, TRUE);
	gtk_fixed_put (GTK_FIXED (fixedTabGen), tableGen1, 50, 30);
	gtk_widget_set_size_request (tableGen1, 300, 100);
	gtk_table_set_row_spacings (GTK_TABLE (tableGen1), 10);
	gtk_table_set_col_spacings (GTK_TABLE (tableGen1), 10);

	GtkWidget *labWeight = gtk_label_new (_("Weight:"));
	gtk_table_attach (GTK_TABLE (tableGen1), labWeight, 0, 1, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labWeight), 0, 0.5);

	m_labEntryGenWeight = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableGen1), m_labEntryGenWeight, 1, 2, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryGenWeight), 0, 0.5);

	m_labEntryGenBSA = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableGen1), m_labEntryGenBSA, 1, 2, 2, 3,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryGenBSA), 0, 0.5);

	GtkWidget *labGenStature = gtk_label_new (_("Stature:"));
	gtk_table_attach (GTK_TABLE (tableGen1), labGenStature, 0, 1, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labGenStature), 0, 0.5);

	m_labEntryGenStature = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableGen1), m_labEntryGenStature, 1, 2, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryGenStature), 0, 0.5);

	GtkWidget *labGenBSA = gtk_label_new (_("BSA:  "));
	gtk_table_attach (GTK_TABLE (tableGen1), labGenBSA, 0, 1, 2, 3,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labGenBSA), 0, 0.5);

	GtkWidget *labTabGen = gtk_label_new (_("General"));
#ifdef VET
#else
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 0), labTabGen);
#endif
	//
	GtkWidget *fixedTabOB = gtk_fixed_new ();
#ifdef VET
#else
	gtk_container_add (GTK_CONTAINER (notebook), fixedTabOB);
#endif
	GtkWidget *tableOB2 = gtk_table_new (5, 2, FALSE);
	gtk_fixed_put (GTK_FIXED (fixedTabOB), tableOB2, 550, 30);
	gtk_widget_set_size_request (tableOB2, 300, 150);
	gtk_table_set_row_spacings (GTK_TABLE (tableOB2), 5);
	gtk_table_set_col_spacings (GTK_TABLE (tableOB2), 10);

	GtkWidget *labOBGravida = gtk_label_new (_("Gravida:"));
	gtk_table_attach (GTK_TABLE (tableOB2), labOBGravida, 0, 1, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labOBGravida), 0, 0.5);

	GtkWidget *labOBEctopic = gtk_label_new (_("Ectopic:"));
	gtk_table_attach (GTK_TABLE (tableOB2), labOBEctopic, 0, 1, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labOBEctopic), 0, 0.5);

	GtkWidget *labOBGestations = gtk_label_new (_("Gestations:"));
	gtk_table_attach (GTK_TABLE (tableOB2), labOBGestations, 0, 1, 2, 3,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labOBGestations), 0, 0.5);

	GtkWidget *labelObPara = gtk_label_new (_("Para:"));
	gtk_table_attach (GTK_TABLE (tableOB2), labelObPara, 0, 1, 3, 4,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labelObPara), 0, 0.5);

	GtkWidget *labOBAborta = gtk_label_new (_("Aborta:"));
	gtk_table_attach (GTK_TABLE (tableOB2), labOBAborta, 0, 1, 4, 5,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labOBAborta), 0, 0.5);

	m_labEntryOBGravida = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableOB2), m_labEntryOBGravida, 1, 2, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryOBGravida), 0, 0.5);

	m_labEntryOBEctopic = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableOB2), m_labEntryOBEctopic, 1, 2, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryOBEctopic), 0, 0.5);

	m_labEntryOBGest = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableOB2), m_labEntryOBGest, 1, 2, 2, 3,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryOBGest), 0, 0.5);

	m_labEntryOBPara = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableOB2), m_labEntryOBPara, 1, 2, 3, 4,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryOBPara), 0, 0.5);

	m_labEntryOBAborta = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableOB2), m_labEntryOBAborta, 1, 2, 4, 5,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryOBAborta), 0, 0.5);

	GtkWidget *tableOB1 = gtk_table_new (2, 2, FALSE);
	gtk_fixed_put (GTK_FIXED (fixedTabOB), tableOB1, 50, 30);
	gtk_widget_set_size_request (tableOB1, 300, 70);
	gtk_table_set_row_spacings (GTK_TABLE (tableOB1), 5);
	gtk_table_set_col_spacings (GTK_TABLE (tableOB1), 10);

	GtkWidget *labOBOvuTime = gtk_label_new (_("Ovulatory Data:"));
	gtk_table_attach (GTK_TABLE (tableOB1), labOBOvuTime, 0, 1, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labOBOvuTime), 0, 0.5);

	GtkWidget *labOBLMP = gtk_label_new (_("LMP:"));
	gtk_table_attach (GTK_TABLE (tableOB1), labOBLMP, 0, 1, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labOBLMP), 0, 0.5);

	m_labEntryOBLMP = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableOB1), m_labEntryOBLMP, 1, 2, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryOBLMP), 0, 0.5);

	m_labEntryOBOvuTime = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableOB1), m_labEntryOBOvuTime, 1, 2, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryOBOvuTime), 0, 0.5);

	GtkWidget *labTabOB = gtk_label_new (_("OB"));
#ifdef VET
#else
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 1), labTabOB);
#endif
	

	//
	GtkWidget *fixedTabCard = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (notebook), fixedTabCard);

	GtkWidget *tableCar1 = gtk_table_new (2, 2, FALSE);
	gtk_fixed_put (GTK_FIXED (fixedTabCard), tableCar1, 50, 30);
	gtk_widget_set_size_request (tableCar1, 300, 70);
	gtk_table_set_row_spacings (GTK_TABLE (tableCar1), 10);
	gtk_table_set_col_spacings (GTK_TABLE (tableCar1), 10);

	GtkWidget *labCarBP = gtk_label_new (_("HR:"));
	gtk_table_attach (GTK_TABLE (tableCar1), labCarBP, 0, 1, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labCarBP), 0, 0.5);

	GtkWidget *label65 = gtk_label_new (_("BP(High/Low):"));
	gtk_table_attach (GTK_TABLE (tableCar1), label65, 0, 1, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	
	m_labEntryCarHR = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableCar1), m_labEntryCarHR, 1, 2, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryCarHR), 0, 0.5);

	m_labEntryCarBP = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableCar1), m_labEntryCarBP, 1, 2, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryCarBP), 0, 0.5);

	//
	GtkWidget *labTabCard = gtk_label_new (_("CARD"));

#ifdef VET
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 0), labTabCard);
#else
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 2), labTabCard);
#endif
	GtkWidget *fixedTabUro = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (notebook), fixedTabUro);

	GtkWidget *tableUro1 = gtk_table_new (1, 2, FALSE);
	gtk_fixed_put (GTK_FIXED (fixedTabUro), tableUro1, 50, 30);
	gtk_widget_set_size_request (tableUro1, 300, 30);
	gtk_table_set_row_spacings (GTK_TABLE (tableUro1), 10);
	gtk_table_set_col_spacings (GTK_TABLE (tableUro1), 10);

	GtkWidget *labUroPSA = gtk_label_new (_("PSA:"));
	gtk_table_attach (GTK_TABLE (tableUro1), labUroPSA, 0, 1, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labUroPSA), 0, 0.5);

	m_labEntryUroPSA = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableUro1), m_labEntryUroPSA, 1, 2, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryUroPSA), 0, 0.5);

	//
	GtkWidget *labTabUro = gtk_label_new (_("URO"));
#ifdef VET
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 1), labTabUro);
#else
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 3), labTabUro);
#endif
	GtkWidget *fixedTabOther = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (notebook), fixedTabOther);

	GtkWidget *tableOther1 = gtk_table_new (2, 2, FALSE);
	gtk_fixed_put (GTK_FIXED (fixedTabOther), tableOther1, 50, 30);
	gtk_widget_set_size_request (tableOther1, 400, 70);
	gtk_table_set_row_spacings (GTK_TABLE (tableOther1), 10);
	gtk_table_set_col_spacings (GTK_TABLE (tableOther1), 10);

#ifdef VET
	GtkWidget *labOtherTele = gtk_label_new (_("Owner Telephone:"));
#else
	GtkWidget *labOtherTele = gtk_label_new (_("Telephone:"));
#endif
	gtk_table_attach (GTK_TABLE (tableOther1), labOtherTele, 0, 1, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labOtherTele), 0, 0.5);

	m_labEntryOtherTele = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableOther1), m_labEntryOtherTele, 1, 2, 0, 1,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryOtherTele), 0, 0.5);
#ifdef VET
	GtkWidget *labOtherAddr = gtk_label_new (_("Owner Address:"));
#else
	GtkWidget *labOtherAddr = gtk_label_new (_("Address:"));
#endif
	gtk_table_attach (GTK_TABLE (tableOther1), labOtherAddr, 0, 1, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labOtherAddr), 0, 0.5);

	m_labEntryOtherAddress = gtk_label_new ("");
	gtk_table_attach (GTK_TABLE (tableOther1), m_labEntryOtherAddress, 1, 2, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labEntryOtherAddress), 0, 0.5);

	GtkWidget *labTabOther = gtk_label_new (_("Other"));
#ifdef VET
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 2), labTabOther);
#else
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 4), labTabOther);
#endif
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);

	return fixedPatInfo;
}

void ViewArchiveImgMan::KeyEvent(unsigned char keyValue)
{
	FakeXEvent::KeyEvent(keyValue);

	switch(keyValue)
	{
		case KEY_ESC:
		    g_timeout_add(100, ExitWindow, this);
		    FakeEscKey();
			break;

		default:
			break;
	}
}
void ViewArchiveImgMan::WhenViewOneImg(void)
{
	if (m_cineTotal <= 1)
		return;

	if (m_cineCur < (m_cineTotal - 1))
		m_cineCur ++;
	else
		m_cineCur = 0;
	//PRINTF("cine info: cur = %d, total = %d\n", m_cineCur, m_cineTotal);

	UpdateCineInfo(m_cineCur, m_cineTotal);
}

void ViewArchiveImgMan::DrawImgData(void* pBits, int nWidth, int nHeight)
{
	m_ptrInstance->DrawNewImgData(pBits, nWidth, nHeight);
}

///>private
void ViewArchiveImgMan::DrawNewImgData(void* pBits, int nWidth, int nHeight)
{
	memcpy(m_cineData, pBits, nWidth*nHeight*3);
	gtk_widget_queue_draw(m_drawArea);
}
void ViewArchiveImgMan::DestroyWindow(void)
{
	// do something clean after cine operation
	Replay::GetInstance()->SetDefaultFrameFunc();
	Replay::GetInstance()->ExitReadSnap();
	Replay::GetInstance()->ExitReadCine();
	Replay::GetInstance()->DisplayReplayBar();
	ExitCine();

	// destroy window
	if(GTK_IS_WIDGET(m_window))
	{
		gtk_widget_destroy(m_window);
		g_keyInterface.Pop();
        m_window = NULL;
	}
}

/*
 * @brief create snap button and snap check button
 *
 * @para button[out] button created
 * @para checkButton[out] check button created
 * @para id[in] id distribute to button and check button created
 */
GtkWidget* ViewArchiveImgMan::CreateItem(GtkWidget **button, GtkWidget **checkButton, char *id)
{
	GtkWidget *vbox;
//	GtkWidget *hbox;

	vbox = gtk_vbox_new (FALSE, 0);

	*button = gtk_button_new ();
	gtk_box_pack_start (GTK_BOX (vbox), *button, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(*button), "clicked", G_CALLBACK(HandleSnapImgClicked), this);
	gtk_button_set_relief (GTK_BUTTON (*button), GTK_RELIEF_NONE);	
	g_object_set_data(G_OBJECT(*button), "id", id);

#if 0
	hbox = gtk_hbox_new (TRUE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

	*checkButton = gtk_check_button_new_with_mnemonic ("");
	gtk_box_pack_start (GTK_BOX (hbox), *checkButton, FALSE, FALSE, 0);	
	g_signal_connect(GTK_OBJECT(*checkButton), "clicked", G_CALLBACK(HandleCheckBtnClicked), this);
	g_object_set_data(G_OBJECT(*checkButton), "id", id);
#endif

	return vbox;
}

/*
 * @brief get patient info according examID, and then update patient info and statistic info
 * 
 * @para examID[in] examNum of current patient(string)
 * @para curPat[in] index of current patient, start from 0
 * @para totalPat[in] total quantity of patient, start from 1
 */
void ViewArchiveImgMan::UpdateCurPat(string examID, int curPat, int totalPat)
{
	gchar buf[256];
	PRINTF("examID = %s, curPat = %d, totalPat = %d\n", examID.c_str(), curPat, totalPat);

	if(!m_isUdisk)
	{
		// get all patient info from database, according to examID
		Database::NewPatSearchResult result;
		Database db;
		db.CommonSearch(examID, result);

		// update curent patient information
#ifdef VET
        sprintf(buf, "%s: %s  ", _("Name"), result.animal_name.c_str());
        gtk_label_set_text(GTK_LABEL(m_labName), buf);

//	float result[MEA_TIMES_MULTI];
		sprintf(buf, "%s: %s  ", _("Owner"), result.owner_name.c_str());
		gtk_label_set_text(GTK_LABEL(m_labOwner), buf);

		sprintf(buf, "%s: %s  ", _("Species"), result.species.c_str());
		gtk_label_set_text(GTK_LABEL(m_labSpecies), buf);
#else
		sprintf(buf, "%s: %s", _("Name"), result.name.c_str());
		gtk_label_set_text(GTK_LABEL(m_labName), buf);
#endif
		sprintf(buf, "%s: %s", _("Gender"), result.gender.c_str());
		gtk_label_set_text(GTK_LABEL(m_labSex), buf);
#ifdef VET
	sprintf(buf, "%s: %s  ", _("Bir."), result.birthDate.c_str());
		gtk_label_set_text(GTK_LABEL(m_labBirthday), buf);
#else
		sprintf(buf, "%s: %s", _("Date of Birth"), result.birthDate.c_str());
		gtk_label_set_text(GTK_LABEL(m_labBirthday), buf);
#endif
		sprintf(buf, "%s: %s", _("Exam ID"), result.examID.c_str());
		gtk_label_set_text(GTK_LABEL(m_labExamNum), buf);
#if 0
		sprintf(buf, "ExamType: %s", result.examType.c_str());
		gtk_label_set_text(GTK_LABEL(m_labExamType), buf);
#endif
	}

	// update statistic info
	sprintf(buf, "%d/%d", curPat+1, totalPat);
	gtk_label_set_text(GTK_LABEL(m_labStatInfo), buf);

	// update image info
	m_vecFileName.clear();
	GetSnapsName(examID, m_vecFileName);
	sort(m_vecFileName.begin(), m_vecFileName.end(), FileMan::SortByMTime);
	m_vecCineName.clear();
	GetCinesName(examID, m_vecCineName);
	sort(m_vecCineName.begin(), m_vecCineName.end(), FileMan::SortByMTime);

	// update select status, unselect nothing
	m_vecFileSel.clear();
	unsigned int i;
	for (i = 0; i < m_vecFileName.size(); i ++)
	{
		m_vecFileSel.push_back(FALSE);
	}
	m_vecCineSel.clear();
	for (i = 0; i < m_vecCineName.size(); i ++)
	{
		m_vecCineSel.push_back(FALSE);
	}

	// update image area(display snap)
	int size = m_vecFileName.size();
	int temp = size / (SNAP_COL * SNAP_ROW);
	m_maxImgPage = (size % (SNAP_COL * SNAP_ROW)) == 0? temp : (temp+1); 	
	m_curImgPage =  0;
	size = m_vecCineName.size();
	temp = size / (SNAP_COL * SNAP_ROW);
	m_maxCinePage = (size % (SNAP_COL * SNAP_ROW)) == 0? temp : (temp+1); 
	m_curCinePage = 0;

	UpdateSnap();

	if(!m_isUdisk)
    {
        // update report
        UpdateReport(examID);
        
        // update patient info
        UpdatePatInfo(examID);
	}
}

/*
 * @brief display report of current patient
 */
void ViewArchiveImgMan::UpdateReport(string examID)
{
    if(m_scrolledwindow_orignial) 
    {
        gtk_widget_destroy(GTK_WIDGET(m_scrolledwindow_orignial));
        m_scrolledwindow_orignial = NULL;
    }
    if(m_scrolledwindow_auto) 
    {
        gtk_widget_destroy(GTK_WIDGET(m_scrolledwindow_auto));
        m_scrolledwindow_auto = NULL;
    }
    
    m_scrolledwindow_auto = CreateReport(examID, 0);
    m_scrolledwindow_orignial= CreateReport(examID, 1);
    
    SysGeneralSetting sysGeneralSetting;
    int mode = sysGeneralSetting.GetPrintViewMode();
    
    if(mode)
    {
        gtk_widget_hide_all(m_scrolledwindow_auto);
        gtk_widget_show_all(m_scrolledwindow_orignial);
    }
    else
    {
        gtk_widget_hide_all(m_scrolledwindow_orignial); 
        gtk_widget_show_all(m_scrolledwindow_auto);
    }     
}

GtkWidget* ViewArchiveImgMan::CreateReport(string examID, int mode)
{
    int grap_y = 20;
    GtkWidget *viewport;
    GtkWidget *fixed;
    char reportName[256];
    char *reportPath  = NULL;
    int i = atoi(examID.c_str());
        
    GtkWidget *scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);
    gtk_widget_set_size_request (scrolledwindow, 984, 598);
    if(GTK_IS_WIDGET(m_fixedReport))
        gtk_fixed_put (GTK_FIXED (m_fixedReport), scrolledwindow, 0, 0);

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER(scrolledwindow), viewport);

    fixed = gtk_fixed_new();
    gtk_widget_show(fixed);
    gtk_container_add (GTK_CONTAINER(viewport), fixed);

    reportPath = ViewReport::GetInstance()->GetArchivePath(i);
    if (reportPath != NULL)
    {
        strcpy(m_archivePath, reportPath);
        if(mode)
        {
            for(int i = 0;; i++)
            {
                sprintf(reportName, "%sreport_page%d.png", reportPath, i+1);
                if(access(reportName, F_OK) == 0) // 判断文件是不是存在
                {
                    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(reportName, NULL);
                    GtkWidget *print_image = gtk_image_new_from_pixbuf(pixbuf);
                    gtk_fixed_put(GTK_FIXED(fixed), print_image, 38, (REPORT_HEIGHT + grap_y)*i);
                    g_object_unref(pixbuf);
                }
                else
                    break;
            }
        }
        else
        {
            for(int i = 0;; i++)
            {
                int width = 595*REPORT_WIDTH/REPORT_HEIGHT;
                int height = 595;
                GError *error = NULL;
                sprintf(reportName, "%sreport_page%d.png", reportPath, i+1);
                if(access(reportName, F_OK) == 0) // 判断文件是不是存在
                {
                    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(reportName, &error);
                    if(pixbuf && !error)
                    {
                        GdkPixbuf *scale_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);
                        GtkWidget *print_image = gtk_image_new_from_pixbuf(scale_pixbuf);
                        gtk_fixed_put(GTK_FIXED(fixed), print_image, (980-width)/2, (height + grap_y)*i);   
                        g_object_unref(scale_pixbuf);  
                        g_object_unref(pixbuf);
                    }
                }
                else
                    break;
            }
        }
    }

	g_free(reportPath);
    gtk_widget_hide_all(scrolledwindow);
    return scrolledwindow;
}

/*
 * @brief display all snap in current page, according to the selection of combox(image or cine)
 */
void ViewArchiveImgMan::UpdateSnap(void)
{
	if (m_fileType == IMG)
		UpdateImg();
	else if (m_fileType == CINE)
		UpdateCine();
}

/*
 * @brief display all image snap(preview) in current page
 */
void ViewArchiveImgMan::UpdateImg(void)
{
	int i;
	int sumOfPage = SNAP_ROW * SNAP_COL;
	int size = m_vecFileName.size();
	ImgMan::ImgItem imgItem;
	GtkWidget *image;
	GdkPixbuf *pixbuf = NULL;	
	gchar buf[20];

	// display current page info
	if (size == 0)
		sprintf(buf, "%d/%d", m_curImgPage, m_maxImgPage);
	else
		sprintf(buf, "%d/%d", m_curImgPage+1, m_maxImgPage);
	gtk_label_set_text(GTK_LABEL(m_labImgPage), buf);

	HideAllBtn();

	// no image return
	if (size == 0)
		return;

	// display snap in current page
	for (i = 0; i < sumOfPage; i ++)
	{
		if ((m_curImgPage * sumOfPage + i) > (size-1))
			break;
		ImgMan::GetInstance()->ReadSnap(m_vecFileName[m_curImgPage*sumOfPage+i].c_str(), &imgItem);

		pixbuf = gdk_pixbuf_scale_simple(imgItem.pixbuf, SNAP_W, SNAP_H, GDK_INTERP_BILINEAR);
		g_object_unref(imgItem.pixbuf);
		if (pixbuf != NULL)
		{
			image = gtk_image_new_from_pixbuf(pixbuf);
			gtk_button_set_image(GTK_BUTTON(m_btnSnap[i]), image);
			gtk_widget_show(m_vbox[i]);
			gchar *basename = g_path_get_basename(m_vecFileName[m_curImgPage*sumOfPage+i].c_str());
			gtk_widget_set_tooltip_text(m_vbox[i], basename);
			g_free(basename);

			g_object_unref(pixbuf);
		}
	}

#if 0
	// display check button
	for (i = 0; i < sumOfPage; i ++)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkBtnSnap[i]), m_vecFileSel[m_curImgPage*sumOfPage + i]);
	}
#endif
}

/*
 * @brief display all cine snap(preview) in current page
 */
void ViewArchiveImgMan::UpdateCine(void)
{
	int i, j;
	int sumOfPage = SNAP_ROW * SNAP_COL;
	int size = m_vecCineName.size();
	VideoMan::VideoItem videoItem;
	GtkWidget *image;
	GdkPixbuf *pixbuf = NULL;	
	GdkPixbuf *pixbuf_new = NULL;	
	gchar buf[20];

	// display current page info
	if (size == 0)
		sprintf(buf, "%d/%d", m_curCinePage, m_maxCinePage);
	else
		sprintf(buf, "%d/%d", m_curCinePage+1, m_maxCinePage);
	gtk_label_set_text(GTK_LABEL(m_labImgPage), buf);

	HideAllBtn();
	
	if (size == 0)
		return;

	// display snap in current page
//	for (i = 0; i < sumOfPage; i ++)
	for (i=m_curCinePage*sumOfPage, j=0; i<size; i++, j++)
	{
//		if ((m_curCinePage * sumOfPage + i) > (size-1))
		if (j >= sumOfPage)
			break;
		PRINTF("current page cine : %s\n", m_vecCineName[i].c_str());
		if (VideoMan::GetInstance()->ReadVideoInfo(m_vecCineName[i].c_str(), &videoItem) != 0)
			continue;

		pixbuf = gdk_pixbuf_new_from_data(videoItem.data, 
				GDK_COLORSPACE_RGB, FALSE, 8,
				videoItem.width, videoItem.height,
				videoItem.width * 3, 
				NULL, NULL);

		pixbuf_new = gdk_pixbuf_scale_simple(pixbuf, CINE_SNAP_W, CINE_SNAP_H, GDK_INTERP_BILINEAR);
		g_free(videoItem.data);
		g_object_unref(pixbuf);
		if (pixbuf_new != NULL)
		{
			image = gtk_image_new_from_pixbuf(pixbuf_new);
			gtk_button_set_image(GTK_BUTTON(m_btnSnap[j]), image);
			gtk_widget_show(m_vbox[j]);
			gchar *basename = g_path_get_basename(m_vecCineName[i].c_str());
			gtk_widget_set_tooltip_text(m_vbox[j], basename);
			g_free(basename);

			g_object_unref(pixbuf_new);
		}
	}

#if 0
	// display check button
	for (i = 0; i < sumOfPage; i ++)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkBtnSnap[i]), m_vecCineSel[m_curCinePage*sumOfPage + i]);
	}
#endif
}

void ViewArchiveImgMan::UpdatePatInfo(string examID)
{
	string strTemp;
	char buf[100];
	PatientInfo::Info info;
	Database db;	


SysGeneralSetting sgs;
int lang=sgs.GetLanguage();
//SysGeneralSetting *sgs = new SysGeneralSetting;
	db.AllInfoSearch(examID, info);
#ifdef VET
	strTemp = info.p.animal_name;
	gtk_label_set_text(GTK_LABEL(m_labEntryName), strTemp.c_str());
//hlx
	strTemp = info.p.owner_name;
	gtk_label_set_text(GTK_LABEL(m_labEntryOwner), strTemp.c_str());

//	strTemp = info.p.species;
	GetAnimalSpeciesString(info.p.species, strTemp);
	gtk_label_set_text(GTK_LABEL(m_labEntrySpecies), strTemp.c_str());
#else
 //SysGeneralSetting *sgs = new SysGeneralSetting;
    if (lang) //chinese
	    strTemp = info.p.name.last + info.p.name.mid + info.p.name.first;
    else    //englist
	    strTemp = info.p.name.first + info.p.name.mid + info.p.name.last;
    
	gtk_label_set_text(GTK_LABEL(m_labEntryName), strTemp.c_str());
#endif


	//strTemp = info.p.name.first + info.p.name.mid + info.p.name.last;
	//gtk_label_set_text(GTK_LABEL(m_labEntryName), strTemp.c_str());

	gtk_label_set_text(GTK_LABEL(m_labEntryExamID), info.e.examNum.c_str());
	gtk_label_set_text(GTK_LABEL(m_labEntryPatID), info.p.id.c_str());
	
	GenAgeStr(info.p.age, info.p.ageUnit, strTemp);
	gtk_label_set_text(GTK_LABEL(m_labEntryAge), strTemp.c_str());

	GetSexString(info.p.sex, strTemp);
	gtk_label_set_text(GTK_LABEL(m_labEntryGender), strTemp.c_str());

	GetDateString(info.p.birthDate.year, info.p.birthDate.month, info.p.birthDate.day, strTemp);
	gtk_label_set_text(GTK_LABEL(m_labEntryBirth), strTemp.c_str());

	GetDateString(info.e.examDate.year, info.e.examDate.month, info.e.examDate.day, strTemp);
	gtk_label_set_text(GTK_LABEL(m_labEntryExamDate), strTemp.c_str());

	GetTimeString(info.e.examTime.hour, info.e.examTime.minute, info.e.examTime.second, strTemp);
	gtk_label_set_text(GTK_LABEL(m_labEntryExamTime), strTemp.c_str());

	gtk_label_set_text(GTK_LABEL(m_labEntryExamType), info.e.examType.c_str());
	gtk_label_set_text(GTK_LABEL(m_labEntryPhysician), info.e.examDoctor.c_str());
	gtk_label_set_text(GTK_LABEL(m_labEntryDiagnostic), info.e.reportDoctor.c_str());
	gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textviewComment)), info.e.comment.c_str(), -1);
#ifdef VET
#else
	if (info.e.weight*100 >= 0)
	{
		sprintf(buf, "%.2fkg", info.e.weight);
		gtk_label_set_text(GTK_LABEL(m_labEntryGenWeight), buf);
	}

	if (info.e.height >= 0)
	{
		sprintf(buf, "%dcm", info.e.height);
		gtk_label_set_text(GTK_LABEL(m_labEntryGenStature), buf);
	}

	if (info.e.BSA*100 >= 0)
	{
		sprintf(buf, "%.2fm²", info.e.BSA);
		gtk_label_set_text(GTK_LABEL(m_labEntryGenBSA), buf);
	}

	if (info.ob.LMPDate.year > 0)
	{
		GetDateString(info.ob.LMPDate.year, info.ob.LMPDate.month, info.ob.LMPDate.day, strTemp);
		gtk_label_set_text(GTK_LABEL(m_labEntryOBLMP), strTemp.c_str());
	}

	if (info.ob.OvulateDate.year > 0)
	{
		GetDateString(info.ob.OvulateDate.year, info.ob.OvulateDate.month, info.ob.OvulateDate.day, strTemp);
		gtk_label_set_text(GTK_LABEL(m_labEntryOBOvuTime), strTemp.c_str());
	}

	if (info.ob.pregCount > 0)
	{
		sprintf(buf, "%d", info.ob.pregCount);
		gtk_label_set_text(GTK_LABEL(m_labEntryOBGravida), buf);
	}

	if (info.ob.abnormalPregCount> 0)
	{
		sprintf(buf, "%d", info.ob.abnormalPregCount);
		gtk_label_set_text(GTK_LABEL(m_labEntryOBEctopic), buf);
	}

	if (info.ob.fetusCount > 0)
	{
		sprintf(buf, "%d", info.ob.fetusCount);
		gtk_label_set_text(GTK_LABEL(m_labEntryOBGest), buf);
	}

	if (info.ob.deliveryCount > 0)
	{
		sprintf(buf, "%d", info.ob.deliveryCount);
		gtk_label_set_text(GTK_LABEL(m_labEntryOBPara), buf);
	}

	if (info.ob.miscarryCount > 0)
	{
		sprintf(buf, "%d", info.ob.miscarryCount);
		gtk_label_set_text(GTK_LABEL(m_labEntryOBAborta), buf);
	}
#endif
	if (info.car.HR > 0)
	{
		sprintf(buf, "%db/min", info.car.HR);
		gtk_label_set_text(GTK_LABEL(m_labEntryCarHR), buf);
	}

	if ((info.car.pressureHigh > 0) && (info.car.pressureLow))
	{
		sprintf(buf, "%d/%dmmHg", info.car.pressureHigh, info.car.pressureLow);
		gtk_label_set_text(GTK_LABEL(m_labEntryCarBP), buf);
	}

	if (info.uro.PSA > 0)
	{
		sprintf(buf, "%dng/ml", info.uro.PSA);
		gtk_label_set_text(GTK_LABEL(m_labEntryUroPSA), buf);
	}

	gtk_label_set_text(GTK_LABEL(m_labEntryOtherTele), info.p.telephone.c_str());
	gtk_label_set_text(GTK_LABEL(m_labEntryOtherAddress), info.p.address.c_str());

}

void ViewArchiveImgMan::DisplayFirstImage()
{
	if(m_vecFileName.size() != 0)
		DisplayImage(m_vecFileName[0]);
}

void ViewArchiveImgMan::DisplayFirstCine()
{
	if(m_vecCineName.size() != 0)
	{
		DrawLoadingInfo();
		g_timeout_add(50, HandlerDisplayCine, (char*)(m_vecCineName[0].c_str()));
	//	DisplayCine(m_vecCineName[0]);
	}
}

void ViewArchiveImgMan::DisplaySelectedImage(int i)
{
	DisplayImage(m_vecFileName[i]);
}

void ViewArchiveImgMan::DisplaySelectedCine(int i)
{
	DrawLoadingInfo();
	g_timeout_add(50, HandlerDisplayCine, (char*)(m_vecCineName[i].c_str()));
}

/*
 * @brief display large image, full path name "fileName", in draw area 
 *
 * @para fileName[in] file name with full path
 */
void ViewArchiveImgMan::DisplayImage(string fileName)
{
	PRINTF("begin display image: fileName = %s\n", fileName.c_str());
	ImgMan::ImgItem imgItem;
	ImgMan::GetInstance()->ReadSnap(fileName.c_str(), &imgItem);

	if (m_imgPixbuf != NULL)
	{
		g_object_unref(m_imgPixbuf);
		m_imgPixbuf = NULL;
	}
	m_imgPixbuf = gdk_pixbuf_scale_simple(imgItem.pixbuf, IMG_WIDTH, IMG_HEIGHT, GDK_INTERP_BILINEAR); 
	g_object_unref(imgItem.pixbuf);
	gtk_widget_queue_draw(m_drawArea);
}

/*
 * @brief display cine's first large image, full path name "cineName", in draw area 
 *
 * @para cineName[in] file name with full path
 */
void ViewArchiveImgMan::DisplayCine(string cineName)
{
	// read video
	VideoMan::VideoItem videoItem;
	if (VideoMan::GetInstance()->ReadVideoInfo(cineName.c_str(), &videoItem) != 0)
		return;
	memcpy(m_cineData, videoItem.data, CINE_WIDTH * CINE_HEIGHT * 3);
    g_free(videoItem.data);
    videoItem.data = NULL;

	// update draw area
	gtk_widget_queue_draw(m_drawArea);

	// update cine menu info
	DisplayCineMenu(TRUE);
	m_cineCur = 0;
	m_cineTotal = videoItem.frames;
	m_fps = videoItem.frame_rate;
	UpdateCineInfo(m_cineCur, m_cineTotal);

	// read data to replay area
	ReadCineToReplay(&videoItem);
}

/*
 * @brief get snap name, and store into vec(m_vecFileName), according to examID(folder saved image is named by examID)
 *
 * @para examID[in] exam ID
 * @para vec[out] vector saved all snap's name of patient whose exam id is examID
 */
void ViewArchiveImgMan::GetSnapsName(string examID, vector<string> &vec)
{
	unsigned int i;
	char *path;

	if(!m_isUdisk)
	{
		ImgMan::GetInstance()->LoadSnap(atoi(examID.c_str()), STORE_PATH, &vec);
		// get full path file name
		for (i = 0; i < vec.size(); i ++)
		{
			path = g_build_filename(STORE_PATH, examID.c_str(), vec[i].c_str(), NULL);
			vec[i] = path;
			g_free(path);
		}
	}
	else
	{
		struct dirent *ent;
		DIR *dir = opendir(SLIDE_PATH);
		if(!dir)
		{
			perror("opendir error");
			return;
		}

		vector<string> vec_tmp;
		while( (ent = readdir(dir)) != NULL)
		{
			if(ent->d_name[0]=='.')
				continue;
//			if(ent->d_type==DT_DIR)
			gchar *absPath = g_build_path(G_DIR_SEPARATOR_S, SLIDE_PATH, ent->d_name, NULL);
			if(g_file_test(absPath, G_FILE_TEST_IS_DIR))
			{
				//get file name by directory name
				vec_tmp.clear();

                if(strcmp(ent->d_name, "3D")==0) // for 3D4D image
                {
                    if(ImgMan::GetInstance()->LoadSnap(absPath, &vec_tmp) != 0)
                    {
                        g_free(absPath);
                        continue;
                    }
                    //get full path file name
                    for (i = 0; i < vec_tmp.size(); i ++)
                    {
                        path = g_build_filename(SLIDE_PATH, ent->d_name, vec_tmp[i].c_str(), NULL);
                        vec_tmp[i] = path;
                        g_free(path);
                    }
                }
                else // for normal image
                {
                    //if(ImgMan::GetInstance()->LoadSnap(atoi(ent->d_name), SLIDE_PATH, &vec_tmp) != 0)
                    if(ImgMan::GetInstance()->LoadSnap(absPath, &vec_tmp) != 0)
                    {
                        g_free(absPath);
                        continue;
                    }
                    //get full path file name
                    for (i = 0; i < vec_tmp.size(); i ++)
                    {
                        path = g_build_filename(SLIDE_PATH, ent->d_name, vec_tmp[i].c_str(), NULL);
                        vec_tmp[i] = path;
                        g_free(path);
                    }
                }
                //add all file name to dest vector
                for(vector<string>::iterator it=vec_tmp.begin(); it!=vec_tmp.end(); ++it)
                    vec.push_back(*it);
            }
			g_free(absPath);
		}
		closedir(dir);
	}
}

/*
 * @brief get cine name, and store into vec(m_vecCineName), according to examID(folder saved image is named by examID)
 *
 * @para examID[in] exam ID
 * @para vec[out] vector saved all snap's name of patient whose exam id is examID
 */
void ViewArchiveImgMan::GetCinesName(string examID, vector<string> &vec)
{
	unsigned int i;
	char *path;

	if(!m_isUdisk)
	{
		VideoMan::GetInstance()->LoadVideo(atoi(examID.c_str()), STORE_PATH, &vec);
		// get full path file name
		unsigned int i;
		for (i = 0; i < vec.size(); i ++)
		{
			path = g_build_filename(STORE_PATH, examID.c_str(), vec[i].c_str(), NULL);
			PRINTF("cine path = %s\n", path);
			vec[i] = path;
			g_free(path);
		}
	}
	else
	{
		struct dirent *ent;
		DIR *dir = opendir(SLIDE_PATH);
		if(!dir)
		{
			perror("opendir error");
			return;
		}

		vector<string> vec_tmp;
		while( (ent = readdir(dir)) != NULL)
		{
			if(ent->d_name[0]=='.')
				continue;
		//	if(ent->d_type==DT_DIR)
			gchar *absPath = g_build_path(G_DIR_SEPARATOR_S, SLIDE_PATH, ent->d_name, NULL);
			if(g_file_test(absPath, G_FILE_TEST_IS_DIR))
			{
				//get file name by directory name
				vec_tmp.clear();

                if(strcmp(ent->d_name, "3D")==0) // for 3D4D cine 
                {
                    if(VideoMan::GetInstance()->LoadVideo(absPath, &vec_tmp) != 0)
                    {
                        g_free(absPath);
                        continue;
                    }
                    //get full path file name
                    for (i = 0; i < vec_tmp.size(); i ++)
                    {
                        path = g_build_filename(SLIDE_PATH, ent->d_name, vec_tmp[i].c_str(), NULL);
                        vec_tmp[i] = path;
                        g_free(path);
                    }
                }
                else // for normal cine
                {
                    if(VideoMan::GetInstance()->LoadVideo(absPath, &vec_tmp)  != 0)
                    {
                        g_free(absPath);
                        continue;
                    }
                    //VideoMan::GetInstance()->LoadVideo(atoi(ent->d_name), SLIDE_PATH, &vec_tmp);
                    //get full path file name
                    for (i = 0; i < vec_tmp.size(); i ++)
                    {
                        path = g_build_filename(SLIDE_PATH, ent->d_name, vec_tmp[i].c_str(), NULL);
                        vec_tmp[i] = path;
                        g_free(path);
                    }
                }
				//add all file name to dest vector
				for(vector<string>::iterator it=vec_tmp.begin(); it!=vec_tmp.end(); ++it)
					vec.push_back(*it);
			}
			g_free(absPath);
		}
		closedir(dir);
	}
}

void ViewArchiveImgMan::HideAllBtn(void)
{
	int i;
	for (i = 0; i < SNAP_ROW * SNAP_COL; i ++)
	{
		gtk_widget_hide(m_vbox[i]);
	}
}

void ViewArchiveImgMan::ReadCineToReplay(VideoMan::VideoItem *videoItem)
{
	// enter cine status for image man
	Replay::GetInstance()->EnterReadCine();
	Replay::GetInstance()->SetUpdateFrameFunc(DrawImgData);


    if (videoItem->format == VideoMan::AVI)
    {
        ReadAviData(videoItem->path, NULL,  videoItem->frames, videoItem->width, videoItem->height, VideoMan::SaveReplay);
    }
    else
    {

        // read cine data to mem
        FILE* fd = fopen(videoItem->path, "rb");
        if(fd == NULL)
        {
            perror("fopen error!");
            return;
        }

        int frameSize = videoItem->width * videoItem->height * 3;
        unsigned char buf[frameSize];
        int i;

        for(i = 0; i < videoItem->frames; i ++)
        {
            if(! VideoMan::GetInstance()->ReadVideoData(fd, buf, videoItem->format, i, videoItem->width, videoItem->height))
            {
                PRINTF("ReadVideoData Error!\n");
                break;
            }

            // add image to replay memory
            Image::AutoTracePara para;
            Image image((unsigned int*)buf, para);
            Replay::GetInstance()->AddOneCineImg(image);
        }

	fclose(fd);	
    }
	// prepare for replay
	Replay::GetInstance()->PrepareForReplay();	
}

void ViewArchiveImgMan::DisplayCineMenu(bool yes)
{
	if (yes)
		gtk_widget_show_all(m_hboxCineReplay);
	else
		gtk_widget_hide_all(m_hboxCineReplay);
}


/*
 * @brief update cine info in cine menu(progress bar and label)
 *
 * @para current[in] current index of replay image, start from 0
 * @para total[in] total sum of image that can be replayed, start from 1
 */
void ViewArchiveImgMan::UpdateCineInfo(int current, int total)
{
	gchar buf[10];

	sprintf(buf, "%d/%d", current+1, total);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(m_progressBar), buf);

	gdouble frac = (gdouble)current / total;
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(m_progressBar), frac);
}

void ViewArchiveImgMan::EnterCine(void)
{
	GtkWidget *image;

	if (!m_statusReplay)
	{
		m_statusReplay = TRUE;

		// view
		image = gtk_image_new_from_stock ("gtk-media-pause", GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image(GTK_BUTTON(m_btnReplay), image);

		// start timer
		guint interval = 1000 / (float)m_fps; // unit: 1/1000 s
		//guint interval = 100; 

        if (m_timer > 0)
        {
            if (!g_source_remove(m_timer))
            {
				PRINTF("remove timer error\n");
            }
            m_timer = 0;
        }
        
        m_timer = g_timeout_add(interval, TimerReviewCineAuto, NULL);
	}
}
void ViewArchiveImgMan::ExitCine(void)
{
	GtkWidget *image;

	if (m_statusReplay)
	{
		m_statusReplay = FALSE;

		// view
		image = gtk_image_new_from_stock ("gtk-media-play", GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image(GTK_BUTTON(m_btnReplay), image);

		// end timer
		if (m_timer > 0)
		{
			if (!g_source_remove(m_timer))
			{
				PRINTF("remove timer error\n");
			}
			m_timer = 0;
		}
	}
}

void ViewArchiveImgMan::HideAllFixed(void)
{
    if(GTK_IS_WIDGET(m_fixedContent))
        gtk_widget_hide_all(m_fixedContent);
    if(GTK_IS_WIDGET(m_fixedReport))
        gtk_widget_hide_all(m_fixedReport);
    if(GTK_IS_WIDGET(m_fixedPatInfo))
        gtk_widget_hide_all(m_fixedPatInfo);
}

void ViewArchiveImgMan::BtnExitClicked(GtkButton *button)
{
	DestroyWindow();
    m_scrolledwindow_auto = NULL;
    m_scrolledwindow_orignial = NULL;
}
void ViewArchiveImgMan::BtnEditClicked(GtkButton* button)
{
}

#if 0
void ViewArchiveImgMan::BtniShowUdiskClicked(GtkButton* button)
{
	// exit current dialog
	DestroyWindow();

	// exit ViewArchive
	ViewArchive::GetInstance()->DestroyWindow();

	// enter menuReview
	g_menuReview.SetPatientID(0);
	g_menuReview.SetImgPath((const char*)UDISK_SLIDE_PATH);
	KeyReView km;
	km.ExitMenuReivew();
	km.Execute();
}
#endif

void ViewArchiveImgMan::BtniShowClicked(GtkButton* button)
{
	// exit current dialog
	DestroyWindow();

	// exit ViewArchive
	ViewArchive::GetInstance()->DestroyWindow();

	KeyReview km;
	km.ExitMenuReivew();
	// enter menuReview
	g_menuReview.SetCurType(MenuReview::REVIEW_SNAP); //change to snap view for slide
	if(!m_isUdisk)
	{
		guint id = atoi(m_vecExamID[m_curPat].c_str());
		g_menuReview.SetPatientID(id);
		g_menuReview.SetImgPath((const char*) STORE_PATH);
		g_menuReview.SetLimit(1);
	}
	else
	{
		g_menuReview.SetPatientID(0);
		g_menuReview.SetImgPath((const char*) SLIDE_PATH);
		g_menuReview.SetLimit(2);
	}
 
	km.Execute();
}

void ViewArchiveImgMan::BtnDeleteClicked(GtkButton* button)
{
	int size;
	int i;
	const char* ptrName;

	if (m_fileType == IMG)
	{
		size = m_vecFileSel.size();
		for ( i = 0; i < size; i ++)
		{
			if (m_vecFileSel[i])
			{
				ptrName = m_vecFileName[i].c_str();	
				ImgMan::GetInstance()->DeleteSnap(ptrName); 
			}
		}
	}
	else if (m_fileType == CINE)
	{
		PRINTF("begin delete\n");
		size = m_vecCineSel.size();
		for ( i = 0; i < size; i ++)
		{
			PRINTF("SIZE = %d\n", size);
			if (m_vecCineSel[i])
			{
				ptrName = m_vecCineName[i].c_str();	
				PRINTF("delete %d is select, name = %s\n", i, m_vecCineName[i].c_str());
				VideoMan::GetInstance()->DeleteVideo(ptrName);
			}
		}
	}
	else
	{
		return;
	}

	UpdateCurPat(m_vecExamID[m_curPat], m_curPat, m_vecExamID.size());
}
void ViewArchiveImgMan::BtnUnselAllClicked(GtkButton* button)
{
	int i;
	int size;
//	int sumOfPage = SNAP_ROW * SNAP_COL;

	if (m_fileType == IMG) //img
	{
		size = m_vecFileSel.size();
		for ( i = 0; i < size; i ++)
		{
			m_vecFileSel[i] = FALSE;
		}
	}
	else if (m_fileType == CINE)//cine
	{
		size = m_vecCineSel.size();
		for ( i = 0; i < size; i ++)
		{
			m_vecFileSel[i] = FALSE;
		}
	}
	else
	{
		return;
	}

#if 0
	// update check button
	for ( i = 0; i < sumOfPage; i ++)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkBtnSnap[i]), FALSE);
	}
#endif
}
void ViewArchiveImgMan::BtnSelAllClicked(GtkButton* button)
{
	int i;
	int size;
//	int sumOfPage = SNAP_ROW * SNAP_COL;

	if (m_fileType == IMG) //img
	{
		size = m_vecFileSel.size();
		for ( i = 0; i < size; i ++)
		{
			m_vecFileSel[i] = TRUE;
		}
	}
	else if (m_fileType == CINE)//cine
	{
		size = m_vecCineSel.size();
		for ( i = 0; i < size; i ++)
		{
			m_vecFileSel[i] = TRUE;
		}
	}
	else
	{
		return;
	}

#if 0
	// update check button
	for ( i = 0; i < sumOfPage; i ++)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkBtnSnap[i]), TRUE);
	}
#endif
}
void ViewArchiveImgMan::BtnLastPatClicked(GtkButton* button)
{
	int size = m_vecExamID.size();
	if (size == 0)
	{
		PRINTF("No patient is exist\n");
		return;
	}

	if (m_curPat <= 0)
	{
		PRINTF("this is the first one\n");
		return;
	}

	m_curPat --;
	UpdateCurPat(m_vecExamID[m_curPat], m_curPat, size);
	
	// exit cine if it is on
	ExitCine();
}
void ViewArchiveImgMan::BtnNextPatClicked(GtkButton* button)
{
	int size = m_vecExamID.size();
	if (size == 0)
	{
		PRINTF("No patient is exist\n");
		return;
	}

	PRINTF("Exam ID vector's size is %d\n", size);
	if (m_curPat >= (size-1))
	{
		PRINTF("this is the last one\n");
		return;
	}

  	m_curPat ++;
	UpdateCurPat(m_vecExamID[m_curPat], m_curPat, size);

	// exit cine if it is on
	ExitCine();
}
void ViewArchiveImgMan::BtnLastImgPageClicked(GtkButton* button)
{
	int size;

	if (m_fileType == IMG)
	{
		size = m_vecFileName.size();
		if (size == 0)
		{
			PRINTF("no image stored for this patient\n");
			return;
		}

		if (m_curImgPage > 0)
			m_curImgPage --;
	}
	else if (m_fileType == CINE)
	{
		size = m_vecCineName.size();
		if (size == 0)
		{
			PRINTF("no cine stored for this patient\n");
			return;
		}

		if (m_curCinePage > 0)
			m_curCinePage --;
	}
	else
	{
		return;
	}

	UpdateSnap();
}
void ViewArchiveImgMan::BtnNextImgPageClicked(GtkButton* button)
{
	int size;

	if (m_fileType == IMG)
	{
		size = m_vecFileName.size();

		if (size == 0)
		{
			PRINTF("no image stored for this patient\n");
			return;
		}

		if (m_curImgPage < (m_maxImgPage - 1))
			m_curImgPage ++;
	}
	else if (m_fileType == CINE)
	{
		size = m_vecCineName.size();

		if (size == 0)
		{
			PRINTF("no image stored for this patient\n");
			return;
		}

		if (m_curCinePage < (m_maxCinePage - 1))
			m_curCinePage ++;
	}
	else
	{
		return;
	}

	UpdateSnap();
}
void ViewArchiveImgMan::BtnSnapImgClicked(GtkButton* button)
{
	int id = atoi((char*)g_object_get_data(G_OBJECT(button), "id"));

	if (m_fileType == IMG)
	{
		//get button pressed
		id += m_curImgPage * (SNAP_COL * SNAP_ROW);

		// display large image 
		DisplayImage(m_vecFileName[id]);
	}
	else if (m_fileType == CINE)
	{
		ExitCine();

		//get button pressed
		id += m_curCinePage * (SNAP_COL * SNAP_ROW);

		// display large image 
		memset(m_cineData, 0, CINE_HEIGHT * CINE_WIDTH * 3); //清除图像去数据，防止Tips隐藏时刷新图像
		DrawLoadingInfo();
		g_timeout_add(50, HandlerDisplayCine, (char*)(m_vecCineName[id].c_str()));
	//	DisplayCine(m_vecCineName[id]);
	}
}

void ViewArchiveImgMan::ChkBtnClicked(GtkButton* button)
{
	int id = atoi((char*)g_object_get_data(G_OBJECT(button), "id"));

	if (m_fileType == IMG)
	{
		id += m_curImgPage * (SNAP_COL * SNAP_ROW);

		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
			m_vecFileSel[id] = TRUE;
		else
			m_vecFileSel[id] = FALSE;
	}
	else if (m_fileType == CINE)
	{
		id += m_curCinePage * (SNAP_COL * SNAP_ROW);		

		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
			m_vecCineSel[id] = TRUE;
		else
			m_vecCineSel[id] = FALSE;

		PRINTF("CHECK BUTTON m_vecFineSel[%d] = %d\n", id, (int)m_vecCineSel[id]);
	}
}

void ViewArchiveImgMan::ComboTypeClicked(GtkComboBox *combobox)
{
	int index;

	index = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
	if (index == -1)
		return;
	
	//exit cine
	ExitCine();

	if (index == IMG) //img
	{
		m_fileType = IMG;
		HideAllFixed();
		gtk_widget_show_all(m_fixedContent);
		gtk_fixed_move(GTK_FIXED(m_fixedContent), m_hboxFunc, 734, 643);
		DisplayCineMenu(FALSE);
		gtk_widget_queue_draw(m_drawArea);
		UpdateSnap();
		g_timeout_add(200, ShowFirstImg, this);
	}
	else if (index == CINE) //cine
	{
		m_fileType = CINE;
		HideAllFixed();
		gtk_widget_show_all(m_fixedContent);
		gtk_widget_hide(m_btnShow);
		gtk_fixed_move(GTK_FIXED(m_fixedContent), m_hboxFunc, 734+120+10, 643);
		gtk_widget_queue_draw(m_drawArea);
		UpdateSnap();
		g_timeout_add(200, ShowFirstCine, this);
	}
	else if (index == REPORT) //report
	{
		m_fileType = REPORT;
		HideAllFixed();
		gtk_widget_show_all(m_fixedReport);
		gtk_fixed_move(GTK_FIXED(m_fixedContent), m_hboxFunc, 734, 643);

        SysGeneralSetting sysGeneralSetting;
        if(sysGeneralSetting.GetPrintViewMode())
        {
            gtk_widget_hide_all (m_scrolledwindow_auto);
            gtk_widget_show_all (m_scrolledwindow_orignial);
        }
        else
        {
            gtk_widget_hide_all (m_scrolledwindow_orignial);
            gtk_widget_show_all (m_scrolledwindow_auto);
        }
	}
	else if (index == INFO) // patient info
	{
		m_fileType = INFO;
		HideAllFixed();
		gtk_widget_show_all(m_fixedPatInfo);
		gtk_fixed_move(GTK_FIXED(m_fixedContent), m_hboxFunc, 734, 643);
	}
}

void ViewArchiveImgMan::BtnReplayClicked(GtkButton* button)
{
	if (m_cineTotal <= 1)
		return;

	PRINTF("-------STATUS = %d\n", m_statusReplay);
	if (m_statusReplay)
	{
		ExitCine();
	}
	else
	{
		EnterCine();
	}
}
void ViewArchiveImgMan::BtnPrintClicked(GtkButton* button)
{
	//char *reportName = ViewReport::GetInstance()->LoadReport(atoi(m_vecExamID[m_curPat].c_str()));

    //ViewPrintPreview::GetInstance()->CreateWindow(reportName, m_window);
    //	g_free(reportName);

#if 0
    ViewPrintPreview::GetInstance()->ClickedPrintButton(button);
#endif
    char reportName[256];
    for(int a = 1; ; a++)
    {
        sprintf(reportName, "%sreport_page%d.png", m_archivePath, a);
        if(access(reportName, F_OK) == 0) // 判断文件是不是存在
        {
            ViewPrintPreview::GetInstance()->PrintArchiveReport(reportName);
        }
        else
            break;
    }
}

void ViewArchiveImgMan::DrawLoadingInfo()
{
	gdk_draw_rectangle(m_drawArea->window, 
			m_drawArea->style->black_gc,
			TRUE, 
			0, 0,
			m_drawArea->allocation.width,
			m_drawArea->allocation.height);

	const char *info = N_("Loading...");
	PangoLayout *layout = gtk_widget_create_pango_layout(m_drawArea, _(info));
	PangoFontDescription *font = pango_font_description_from_string("DejaVu Sans Mono, Book, 10");
	pango_layout_set_font_description(layout, font);
	pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
	int w, h, x, y;
	pango_layout_get_pixel_size(layout, &w, &h);
//	PRINTF("w = %d, h = %d\n", w, h);
	if( m_drawArea->allocation.width > w && m_drawArea->allocation.height > h)
	{
		x = (m_drawArea->allocation.width - w) / 2;
		y = (m_drawArea->allocation.height - h) / 2;
	//	PRINTF("x = %d, y = %d\n", x, y);
		gdk_draw_layout(m_drawArea->window, m_drawArea->style->white_gc, x, y, layout);
	}
	g_object_unref(layout);
}

void ViewArchiveImgMan::DrawAreaConfigure(GtkWidget *widget, GdkEventConfigure *event)
{
	memset(m_imgData, 0, IMG_HEIGHT * IMG_WIDTH * 3);
    m_imgPixbuf = gdk_pixbuf_new_from_data((unsigned char*)m_imgData, 
					   GDK_COLORSPACE_RGB, FALSE, 8,
					   IMG_WIDTH, IMG_HEIGHT,
					   IMG_WIDTH * 3,
					   NULL, NULL);

	memset(m_cineData, 0, CINE_HEIGHT * CINE_WIDTH * 3);
	m_cinePixbuf = gdk_pixbuf_new_from_data((unsigned char*)m_cineData, 
					   GDK_COLORSPACE_RGB, FALSE, 8,
					   CINE_WIDTH, CINE_HEIGHT,
					   CINE_WIDTH * 3,
					   NULL, NULL);
}
void ViewArchiveImgMan::DrawAreaExpose(GtkWidget *widget, GdkEventExpose *event)
{
    for (int i = 0;; i++)
    {
        char reportName[256];
        sprintf(reportName, "%sreport_page%d.png", m_archivePath, i+1);
        if(access(reportName, F_OK) == 0) 
        {
            if (widget == m_imagePage[i])
            {
                cairo_t *cr;  
                cr = gdk_cairo_create(widget->window);  
                cairo_surface_t *image = cairo_image_surface_create_from_png(reportName);
                if(image)
                {
                    cairo_set_source_surface(cr, image, 0, 0);
                    cairo_surface_destroy(image);
                }
                cairo_paint (cr);
                cairo_destroy(cr);
                return;
            }
        }
        else
            break;
    }
    int ox = (IMG_WIDTH - CINE_WIDTH) / 2;
    int oy = (IMG_HEIGHT - CINE_HEIGHT ) / 2;

    if (m_fileType == IMG)
	{
		if (m_imgPixbuf != NULL)
		{
			gdk_draw_pixbuf(widget->window, 
					widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
					m_imgPixbuf,
					0, 0,
					0, 0,
					IMG_WIDTH, IMG_HEIGHT,
					GDK_RGB_DITHER_NORMAL,
					0, 0);
		}
	}
	else if (m_fileType == CINE)
	{
		gdk_draw_rectangle(widget->window, 
				widget->style->black_gc,
				TRUE, 
				0, 0,
				widget->allocation.width,
				widget->allocation.height);

		if (m_cinePixbuf != NULL)
		{
			gdk_draw_pixbuf(widget->window, 
					widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
					m_cinePixbuf,
					0, 0,
					ox, oy,
					CINE_WIDTH, CINE_HEIGHT,
					GDK_RGB_DITHER_NORMAL,
					0, 0);
		}
	}
}

