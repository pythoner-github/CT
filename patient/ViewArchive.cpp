/**
* @file ViewArchive.h
* @brief popup Archive manage dialog, and deal user's request
*
* @version: 1.0
* @author: liudan reviseed by zhanglei 100317
*/

#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/vfs.h>
#include <errno.h>
#include "DCMMan.h"
#include "KeyValueOpr.h"
#include "ViewArchive.h"
#include "ImgMan.h"
#include "PeripheralMan.h"
#include "VideoMan.h"
#include "ViewCD.h"
#include "ViewCalendar.h"
#include "gui_global.h"
#include "gui_func.h"
#include "KeyDef.h"
#include "ViewMain.h"
#include "ViewArchiveImgMan.h"
#include "ViewUdiskDataSelect.h"
#include "ViewDialog.h"
#include "SysDicomSetting.h"
#include "TopArea.h"
#include "HintArea.h"
#include "ViewQueryRetrieve.h"
#include "ViewDicomDataSelect.h"
#include "SysGeneralSetting.h"
#include "DCMRegister.h"
#include "Replay.h"
#include "DicomSend.h"
#include "DicomSendPrinter.h"
using std::ostringstream;
using std::cout;
using std::endl;

// for statistic
#ifdef    _64_
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#define __USE_FILE_OFFSET64
#endif

ViewArchive* ViewArchive::m_ptrInstance = NULL;

ViewArchive::ViewArchive()
{
    m_curYear = 0;
    m_curMonth = 0;
    m_curDay = 0;
    m_iStart = 0;
    m_iEnd = 0;
    m_selNum = 0;
    m_vecCheckId.clear();
    m_vecFlashItem.clear();
}

ViewArchive::~ViewArchive()
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

ViewArchive* ViewArchive::GetInstance()
{
	if(m_ptrInstance == NULL)
		m_ptrInstance = new ViewArchive;

	return m_ptrInstance;
}

GtkWidget* ViewArchive::GetWindow(void)
{
    if (GTK_IS_WIDGET(m_window))
        return m_window;
    else 
        return NULL;
}

extern KeyValueOpr keyInterface;
void ViewArchive::CreateWindow(void)
{
	GtkWidget *fixedWin;
	GtkWidget *frameSearch;
	GtkWidget *alignmentSearch;
	GtkWidget *fixedSearch;
	GtkWidget *imgCalendarStart;
	GtkWidget *eventboxCalendarStart;
	GtkWidget *labelName;
    //hlx 
#ifdef VET
    GtkWidget *labelOwnerName;
    GtkWidget *labelSpecies;
#endif

	GtkWidget *labelDateStart;
	GtkWidget *labelDateEnd;
	GtkWidget *eventboxCalendarEnd;
	GtkWidget *imgCalendarEnd;
	GtkWidget *labelAge;
	GtkWidget *labelSex;
	GtkWidget *btnClear;
	GtkWidget *alignmentBtnClear;
	GtkWidget *hboxBtnClear;
	GtkWidget *imgBtnClear;
	GtkWidget *labelBtnClear;
	GtkWidget *labelID;
	GtkWidget *btnSearch;
	GtkWidget *alignmentBtnSearch;
	GtkWidget *hboxBtnSearch;
	GtkWidget *imgSearch;
	GtkWidget *labelBtnSearch;
	GtkWidget *labelSearch;
	GtkWidget *btnSelectAll;
//	GtkWidget *alignmentBtnSelectAll;
//	GtkWidget *hboxBtnSelectAll;
	GtkWidget *imgSelectAll;
	GtkWidget *labelSelectAll;
	GtkWidget *btnUnselectAll;
//	GtkWidget *alignmentBtnUnselectAll;
//	GtkWidget *hboxBtnUnselectAll;
	GtkWidget *imgUnselectAll;
	GtkWidget *labelUnselectAll;
	GtkWidget *btnSend;
//	GtkWidget *alignmentBtnSend;
//	GtkWidget *hboxBtnSend;
	GtkWidget *imgSend;
	GtkWidget *labelSend;
	GtkWidget *btnExport;
//	GtkWidget *alignmentBtnExport;
//	GtkWidget *hboxBtnExport;
	GtkWidget *imgExport;
	GtkWidget *labelExport;
	GtkWidget *btnExit;
//	GtkWidget *alignmentBtnExit;
//	GtkWidget *hboxBtnExit;
	GtkWidget *imgExit;
	GtkWidget *labelExit;
	GtkWidget *frameQSearch;
	GtkWidget *alignmentQSearch;
	GtkWidget *fixedQSearch;
	GtkWidget *labelQSearchMode;
	//GtkWidget *comboboxQSearch;
	GtkWidget *labelQSearch;
	GtkWidget *frameSource;
	GtkWidget *alignmentSource;
	GtkWidget *fixedSource;
	GtkWidget *tableSource;
	GtkWidget *labelFreeSpace;
	GtkWidget *labelEndtalSpace;
	GSList *m_radiobtnKb_group = NULL;
	GtkWidget *labelSource;
	GtkWidget *btnDisplay;
//	GtkWidget *alignmentBtnDisplay;
//	GtkWidget *hboxBtnDisplay;
	GtkWidget *imgDisplay;
	GtkWidget *labelDisplay;
	GtkWidget *btnDelete;
//	GtkWidget *alignmentBtnDelete;
//	GtkWidget *hboxBtnDelete;
    GtkWidget *imgDelete;
    GtkWidget *labelDelete;
    GtkWidget *imgUdisk;
    GtkWidget *labelUdisk;
    GtkWidget *btnUdisk;


    GtkWidget *label_query_retrieve; 
    GtkWidget *img_query_retrieve; 
    GtkWidget *button_query_retrieve; 

    GetCurrentDate(m_curYear, m_curMonth, m_curDay);
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

	//create window
	m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request (m_window, SCREEN_WIDTH, SCREEN_HEIGHT);
//	gtk_window_set_title (GTK_WINDOW (m_window), _("m_window"));
	gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
	gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
	gtk_window_set_decorated (GTK_WINDOW (m_window), FALSE);
	gtk_window_set_transient_for (GTK_WINDOW (m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));

	fixedWin = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (m_window), fixedWin);

	//create source area
	frameSource = gtk_frame_new (NULL);
	gtk_fixed_put (GTK_FIXED (fixedWin), frameSource, 20, 20);
	gtk_widget_set_size_request (frameSource, 200, 170);
	gtk_frame_set_shadow_type (GTK_FRAME (frameSource), GTK_SHADOW_ETCHED_OUT);

	labelSource = gtk_label_new (_("<b>Disk Info</b>"));
	gtk_frame_set_label_widget (GTK_FRAME (frameSource), labelSource);
	gtk_label_set_use_markup (GTK_LABEL (labelSource), TRUE);

	alignmentSource = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_container_add (GTK_CONTAINER (frameSource), alignmentSource);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignmentSource), 0, 0, 12, 0);

	fixedSource = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (alignmentSource), fixedSource);

	tableSource = gtk_table_new (6, 3, FALSE);
	gtk_fixed_put (GTK_FIXED (fixedSource), tableSource, 5, 5);
	gtk_widget_set_size_request (tableSource, 120, 145);

	labelFreeSpace = gtk_label_new (_("Free Space:"));
	gtk_table_attach (GTK_TABLE (tableSource), labelFreeSpace, 0, 3, 0, 1,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labelFreeSpace), 0, 0.5);

	m_labelFreeSize = gtk_label_new ("0Kb");
	gtk_table_attach (GTK_TABLE (tableSource), m_labelFreeSize, 0, 3, 1, 2,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labelFreeSize), 0.9, 0.5);

	labelEndtalSpace = gtk_label_new (_("Total Space:"));
	gtk_table_attach (GTK_TABLE (tableSource), labelEndtalSpace, 0, 3, 2, 3,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (labelEndtalSpace), 0, 0.5);

	m_labelEndtalSize = gtk_label_new ("0Kb");
	gtk_table_attach (GTK_TABLE (tableSource), m_labelEndtalSize, 0, 3, 3, 4,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_misc_set_alignment (GTK_MISC (m_labelEndtalSize), 0.9, 0.5);

	m_radiobtnKb = gtk_radio_button_new_with_mnemonic (NULL, "KB");
	gtk_table_attach (GTK_TABLE (tableSource), m_radiobtnKb, 0, 1, 4, 5,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtnKb), m_radiobtnKb_group);
	m_radiobtnKb_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtnKb));
	g_signal_connect(G_OBJECT(m_radiobtnKb), "toggled", G_CALLBACK(HandleBtnToggleKb), this);

	m_radiobtnMb = gtk_radio_button_new_with_mnemonic (NULL, "MB");
	gtk_table_attach (GTK_TABLE (tableSource), m_radiobtnMb, 1, 2, 4, 5,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtnMb), m_radiobtnKb_group);
	m_radiobtnKb_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtnMb));
	g_signal_connect(G_OBJECT(m_radiobtnMb), "toggled", G_CALLBACK(HandleBtnToggleMb), this);

	m_radiobtnGb = gtk_radio_button_new_with_mnemonic (NULL, "GB");
	gtk_table_attach (GTK_TABLE (tableSource), m_radiobtnGb, 2, 3, 4, 5,
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
	gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtnGb), m_radiobtnKb_group);
	m_radiobtnKb_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtnGb));
	g_signal_connect(G_OBJECT(m_radiobtnGb), "toggled", G_CALLBACK(HandleBtnToggleGb), this);

	m_progressbarSource = gtk_progress_bar_new ();
	gtk_fixed_put (GTK_FIXED (fixedSource), m_progressbarSource, 145, 5);
	gtk_widget_set_size_request (m_progressbarSource, 30, 140);
	gtk_progress_bar_set_orientation (GTK_PROGRESS_BAR (m_progressbarSource), GTK_PROGRESS_BOTTOM_TO_TOP);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (m_progressbarSource), 0.5);
	gtk_progress_bar_set_pulse_step (GTK_PROGRESS_BAR (m_progressbarSource), 0.01);
	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (m_progressbarSource), "50%");

	// init of source
//	gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxSource), 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_radiobtnGb), TRUE);
	FillDiskSpace();

	//create search area
	frameSearch = gtk_frame_new (NULL);
	gtk_fixed_put (GTK_FIXED (fixedWin), frameSearch, 230, 20);
	gtk_widget_set_size_request (frameSearch, 560, 170);
	gtk_frame_set_shadow_type (GTK_FRAME (frameSearch), GTK_SHADOW_ETCHED_OUT);

	alignmentSearch = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_container_add (GTK_CONTAINER (frameSearch), alignmentSearch);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignmentSearch), 0, 0, 12, 0);

	fixedSearch = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (alignmentSearch), fixedSearch);
    //修改 动物超 档案管理 界面
    //hlx 
#ifdef VET
    labelID = gtk_label_new (_("ID:"));
    gtk_fixed_put (GTK_FIXED (fixedSearch), labelID, 10, 5);
    //	gtk_widget_set_size_request (labelID, 90, 30);
    gtk_widget_set_size_request (labelID, 40, 30);
    gtk_misc_set_alignment (GTK_MISC (labelID), 0, 0.5);

    m_entryID = gtk_entry_new ();
    //	gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryID, 100, 5);
    gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryID, 50, 5);
    gtk_widget_set_size_request (m_entryID, 140, 30);
    gtk_entry_set_max_length(GTK_ENTRY(m_entryID), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entryID), 8226);
    g_signal_connect(G_OBJECT(m_entryID), "insert_text", G_CALLBACK(HandleEntryOnlyNumInsert), this);

    labelSex = gtk_label_new (_("Gender:"));
    //	gtk_fixed_put (GTK_FIXED (fixedSearch), labelSex, 365, 5);
    gtk_fixed_put (GTK_FIXED (fixedSearch), labelSex, 225, 5);
    //	gtk_widget_set_size_request (labelSex, 90, 30);
    gtk_widget_set_size_request (labelSex, 60, 30);
    gtk_misc_set_alignment (GTK_MISC (labelSex), 0, 0.5);

    m_comboboxSex = gtk_combo_box_new_text();
    //	gtk_fixed_put(GTK_FIXED(fixedSearch), m_comboboxSex, 425, 5);
    gtk_fixed_put(GTK_FIXED(fixedSearch), m_comboboxSex, 285, 5);
    //	gtk_widget_set_size_request(m_comboboxSex, 90, 30);
    gtk_widget_set_size_request(m_comboboxSex, 70, 30);
    gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxSex), _("All"));
#ifdef VET
    gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxSex), _("Female "));
    gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxSex), _("Male "));
#else
    gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxSex), _("Female"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxSex), _("Male"));
#endif
    gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxSex), _("Other"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxSex), 0);

    //age
    labelAge = gtk_label_new (_("Age:"));
    gtk_fixed_put (GTK_FIXED (fixedSearch), labelAge, 385, 5);
    gtk_widget_set_size_request (labelAge, 40, 30);
    gtk_misc_set_alignment (GTK_MISC (labelAge), 0, 0.5);

    m_entryAge = gtk_entry_new ();
    gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryAge, 425, 5);
    gtk_widget_set_size_request (m_entryAge, 35, 30);
    gtk_entry_set_max_length(GTK_ENTRY(m_entryAge), 3);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entryAge), 8226);
    g_signal_connect(G_OBJECT(m_entryAge), "insert_text", G_CALLBACK(HandleEntryOnlyNumInsert), this);

    m_comboboxAgeUnit = gtk_combo_box_new_text();
    gtk_fixed_put(GTK_FIXED(fixedSearch), m_comboboxAgeUnit, 460, 5);
    gtk_widget_set_size_request(m_comboboxAgeUnit, 55, 30);
    gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxAgeUnit), _("Y"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxAgeUnit), _("M"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxAgeUnit), _("D"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxAgeUnit), 0);

    labelName = gtk_label_new (_("Animal Name:"));
    gtk_fixed_put (GTK_FIXED (fixedSearch), labelName, 10, 40);
    gtk_widget_set_size_request (labelName, 90, 30);
    gtk_misc_set_alignment (GTK_MISC (labelName), 0, 0.5);

    m_entryName = gtk_entry_new ();
    gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryName, 100, 40);
    gtk_widget_set_size_request (m_entryName, 90, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entryName), 8226);
    gtk_entry_set_max_length(GTK_ENTRY(m_entryName), 12);

    labelOwnerName = gtk_label_new (_("Owner:"));
    gtk_fixed_put (GTK_FIXED (fixedSearch), labelOwnerName, 205, 40);
    gtk_widget_set_size_request (labelOwnerName, 60, 30);
    gtk_misc_set_alignment (GTK_MISC (labelOwnerName), 0, 0.5);

    m_entryOwnerName = gtk_entry_new ();
    gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryOwnerName, 265, 40);
    gtk_widget_set_size_request (m_entryOwnerName, 90, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entryOwnerName), 8226);
    gtk_entry_set_max_length(GTK_ENTRY(m_entryOwnerName), 12);

    labelSpecies = gtk_label_new (_("Species:"));
    gtk_fixed_put (GTK_FIXED (fixedSearch), labelSpecies, 365, 40);
    gtk_widget_set_size_request (labelSpecies, 60, 30);
    gtk_misc_set_alignment (GTK_MISC (labelSpecies), 0, 0.5);

    m_comboboxSpecies = gtk_combo_box_new_text();
    gtk_fixed_put(GTK_FIXED(fixedSearch), m_comboboxSpecies, 425, 40);
    gtk_widget_set_size_request(m_comboboxSpecies, 90, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("All"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Dog"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Cat"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Sheep"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Swine"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Bovine"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxSpecies), _("Equine"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxSpecies), 0);
#else

	labelID = gtk_label_new (_("ID:"));
	gtk_fixed_put (GTK_FIXED (fixedSearch), labelID, 8, 5);
	gtk_widget_set_size_request (labelID, 90-12, 30);
	gtk_misc_set_alignment (GTK_MISC (labelID), 0, 0.5);

	m_entryID = gtk_entry_new ();
	gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryID, 100-12, 5);
	gtk_widget_set_size_request (m_entryID, 240, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entryID), 8226);
	g_signal_connect(G_OBJECT(m_entryID), "insert_text", G_CALLBACK(HandleEntryOnlyNumInsert), this);

   labelSex = gtk_label_new (_("Gender:"));
	gtk_fixed_put (GTK_FIXED (fixedSearch), labelSex, 365, 5);
	gtk_widget_set_size_request (labelSex, 90, 30);
	gtk_misc_set_alignment (GTK_MISC (labelSex), 0, 0.5);

	m_comboboxSex = gtk_combo_box_new_text();
	gtk_fixed_put(GTK_FIXED(fixedSearch), m_comboboxSex, 425, 5);
	gtk_widget_set_size_request(m_comboboxSex, 90, 30);
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxSex), _("All"));
#ifdef VET
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxSex), _("Female "));
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxSex), _("Male "));
#else
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxSex), _("Female"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxSex), _("Male"));
#endif
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxSex), _("Other"));

	labelName = gtk_label_new (_("Name:"));
	gtk_fixed_put (GTK_FIXED (fixedSearch), labelName, 8, 40);
	gtk_widget_set_size_request (labelName, 90-12, 30);
	gtk_misc_set_alignment (GTK_MISC (labelName), 0, 0.5);

	m_entryName = gtk_entry_new ();
	gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryName, 100-12, 40);
	gtk_widget_set_size_request (m_entryName, 250-10, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entryName), 8226);
	//gtk_entry_set_visible_char (GTK_ENTRY (m_entryName), 2);

	labelAge = gtk_label_new (_("Age:"));
	gtk_fixed_put (GTK_FIXED (fixedSearch), labelAge, 365, 40);
	gtk_widget_set_size_request (labelAge, 60, 30);
	gtk_misc_set_alignment (GTK_MISC (labelAge), 0, 0.5);

	m_entryAge = gtk_entry_new ();
	gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryAge, 425, 40);
	gtk_widget_set_size_request (m_entryAge, 35, 30);
	gtk_entry_set_invisible_char (GTK_ENTRY (m_entryAge), 8226);
	g_signal_connect(G_OBJECT(m_entryAge), "insert_text", G_CALLBACK(HandleEntryOnlyNumInsert), this);

	m_comboboxAgeUnit = gtk_combo_box_new_text();
	gtk_fixed_put(GTK_FIXED(fixedSearch), m_comboboxAgeUnit, 460, 40);
	gtk_widget_set_size_request(m_comboboxAgeUnit, 55, 30);
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxAgeUnit), _("Y"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxAgeUnit), _("M"));
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxAgeUnit), _("D"));
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxAgeUnit), 0);
#endif
	labelDateStart = gtk_label_new (_("Date Start:"));
	gtk_fixed_put (GTK_FIXED (fixedSearch), labelDateStart, 8, 75);
	gtk_widget_set_size_request (labelDateStart, 90-12, 30);
	gtk_misc_set_alignment (GTK_MISC (labelDateStart), 0, 0.5);


    labelDateEnd = gtk_label_new (_("Date End:"));
    gtk_fixed_put (GTK_FIXED (fixedSearch), labelDateEnd, 8, 110);
    gtk_widget_set_size_request (labelDateEnd, 90-12+10, 30);
    gtk_misc_set_alignment (GTK_MISC (labelDateEnd), 0, 0.5);

    SysGeneralSetting *sys_date1 = new SysGeneralSetting;    
    int date_format = sys_date1->GetDateFormat();
    if(0==date_format)

    {
        m_entryYearStart = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryYearStart), 4);
        gtk_widget_set_size_request (m_entryYearStart, 45, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryYearStart, 100-12, 75);
        //g_signal_connect (G_OBJECT (m_entryYearStart), "insert-text", G_CALLBACK (HandleEntryOnlyNumInsert), this);
        g_signal_connect(G_OBJECT(m_entryYearStart), "insert_text", G_CALLBACK(on_entry_birth_date_year), this);


        GtkWidget *label1 = gtk_label_new (_("Y  "));
        gtk_widget_set_size_request (label1, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label1, 145-12, 75);

        m_entryMonthStart = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryMonthStart), 2);
        gtk_widget_set_size_request (m_entryMonthStart, 30, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryMonthStart, 155+10-12, 75);
        g_signal_connect (G_OBJECT (m_entryMonthStart), "insert-text", G_CALLBACK (on_entry_birth_date_month), this);

        GtkWidget *label2 = gtk_label_new (_("M  "));
        gtk_widget_set_size_request (label2, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label2, 185+10-12, 75);

        m_entryDayStart = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryDayStart), 2);
        gtk_widget_set_size_request (m_entryDayStart, 30, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryDayStart, 195+20-12, 75);
        g_signal_connect (G_OBJECT (m_entryDayStart), "insert-text", G_CALLBACK(on_entry_birth_date_day), this);

        GtkWidget *label13 = gtk_label_new (_("D  "));
        gtk_widget_set_size_request (label13, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label13, 227+20-12, 75);


        m_entryYearEnd = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryYearEnd), 4);
        gtk_widget_set_size_request (m_entryYearEnd, 45, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryYearEnd, 100-12, 110);
        g_signal_connect (G_OBJECT (m_entryYearEnd), "insert-text", G_CALLBACK (on_entry_birth_date_end_year), this);
        //g_signal_connect (G_OBJECT (m_entryYearEnd), "insert-text", G_CALLBACK (HandleEntryOnlyNumInsert), this); //2012.08.17 lihuamei

        GtkWidget *label3 = gtk_label_new (_("Y  "));
        gtk_widget_set_size_request (label3, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label3, 145-12, 110);

        m_entryMonthEnd = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryMonthEnd), 2);
        gtk_widget_set_size_request (m_entryMonthEnd, 30, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryMonthEnd, 155+10-12, 110);
        g_signal_connect (G_OBJECT (m_entryMonthEnd), "insert-text", G_CALLBACK (on_entry_birth_date_end_month), this);

        GtkWidget *label4 = gtk_label_new (_("M  "));
        gtk_widget_set_size_request (label4, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label4, 185+10-12, 110);

        m_entryDayEnd = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryDayEnd), 2);
        gtk_widget_set_size_request (m_entryDayEnd, 30, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryDayEnd, 195+20-12, 110);
        g_signal_connect (G_OBJECT (m_entryDayEnd), "insert-text", G_CALLBACK (on_entry_birth_date_end_day), this);

        GtkWidget *label23 = gtk_label_new (_("D  "));
        gtk_widget_set_size_request (label23, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label23, 227+20-12, 110);

    }
    else if(2==date_format)
    {
        m_entryYearStart = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryYearStart), 4);
        gtk_widget_set_size_request (m_entryYearStart, 45, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryYearStart, 195-15+20-12, 75);
        //g_signal_connect (G_OBJECT (m_entryYearStart), "insert-text", G_CALLBACK (HandleEntryOnlyNumInsert), this);
        g_signal_connect(G_OBJECT(m_entryYearStart), "insert_text", G_CALLBACK(on_entry_birth_date_year), this);


        GtkWidget *label1 = gtk_label_new (_("D  "));
        gtk_widget_set_size_request (label1, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label1, 145-15-12, 75);

        m_entryMonthStart = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryMonthStart), 2);
        gtk_widget_set_size_request (m_entryMonthStart, 30, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryMonthStart, 155-15+10-12, 75);
        g_signal_connect (G_OBJECT (m_entryMonthStart), "insert-text", G_CALLBACK (on_entry_birth_date_month), this);

        GtkWidget *label2 = gtk_label_new (_("M  "));
        gtk_widget_set_size_request (label2, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label2, 185-15+10-12, 75);

        m_entryDayStart = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryDayStart), 2);
        gtk_widget_set_size_request (m_entryDayStart, 30, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryDayStart, 100-12, 75);
        g_signal_connect (G_OBJECT (m_entryDayStart), "insert-text", G_CALLBACK(on_entry_birth_date_day), this);

        GtkWidget *label33 = gtk_label_new (_("Y  "));
        gtk_widget_set_size_request (label33, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label33, 227+20-12, 75);

        m_entryYearEnd = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryYearEnd), 4);
        gtk_widget_set_size_request (m_entryYearEnd, 45, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryYearEnd, 195-15+20-12, 110);
        g_signal_connect (G_OBJECT (m_entryYearEnd), "insert-text", G_CALLBACK (on_entry_birth_date_end_year), this);
        //g_signal_connect (G_OBJECT (m_entryYearEnd), "insert-text", G_CALLBACK (HandleEntryOnlyNumInsert), this); //2012.08.17 lihuamei

        GtkWidget *label3 = gtk_label_new (_("D  "));
        gtk_widget_set_size_request (label3, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label3, 145-15-12, 110);

        m_entryMonthEnd = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryMonthEnd), 2);
        gtk_widget_set_size_request (m_entryMonthEnd, 30, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryMonthEnd, 155-15+10-12, 110);
        g_signal_connect (G_OBJECT (m_entryMonthEnd), "insert-text", G_CALLBACK (on_entry_birth_date_end_month), this);

        GtkWidget *label4 = gtk_label_new (_("M  "));
        gtk_widget_set_size_request (label4, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label4, 185-15+10-12, 110);

        m_entryDayEnd = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryDayEnd), 2);
        gtk_widget_set_size_request (m_entryDayEnd, 30, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryDayEnd, 100-12, 110);
        g_signal_connect (G_OBJECT (m_entryDayEnd), "insert-text", G_CALLBACK (on_entry_birth_date_end_day), this);

        GtkWidget *label43 = gtk_label_new (_("Y  "));
        gtk_widget_set_size_request (label43, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label43, 227+20-12, 110);
    }
    else
    {
        m_entryYearStart = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryYearStart), 4);
        gtk_widget_set_size_request (m_entryYearStart, 45, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryYearStart, 195-15+20-12, 75);
        //g_signal_connect (G_OBJECT (m_entryYearStart), "insert-text", G_CALLBACK (HandleEntryOnlyNumInsert), this);
        g_signal_connect(G_OBJECT(m_entryYearStart), "insert_text", G_CALLBACK(on_entry_birth_date_year), this);

        GtkWidget *label1 = gtk_label_new (_("M  "));
        gtk_widget_set_size_request (label1, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label1, 145-15-12, 75);
        
        m_entryMonthStart = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryMonthStart), 2);
        gtk_widget_set_size_request (m_entryMonthStart, 30, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryMonthStart, 100-12, 75);
        g_signal_connect (G_OBJECT (m_entryMonthStart), "insert-text", G_CALLBACK (on_entry_birth_date_month), this);

        GtkWidget *label2 = gtk_label_new (_("D  "));
        gtk_widget_set_size_request (label2, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label2, 185-15+10-12, 75);

        m_entryDayStart = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryDayStart), 2);
        gtk_widget_set_size_request (m_entryDayStart, 30, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryDayStart, 155-15+10-12, 75);
        g_signal_connect (G_OBJECT (m_entryDayStart), "insert-text", G_CALLBACK(on_entry_birth_date_day), this);

        GtkWidget *label53 = gtk_label_new (_("Y  "));
        gtk_widget_set_size_request (label53, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label53, 227+20-12, 75);

        m_entryYearEnd = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryYearEnd), 4);
        gtk_widget_set_size_request (m_entryYearEnd, 45, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryYearEnd, 195-15+20-12, 110);
        g_signal_connect (G_OBJECT (m_entryYearEnd), "insert-text", G_CALLBACK (on_entry_birth_date_end_year), this);
        //g_signal_connect (G_OBJECT (m_entryYearEnd), "insert-text", G_CALLBACK (HandleEntryOnlyNumInsert), this); //2012.08.17 lihuamei

        GtkWidget *label3 = gtk_label_new (_("M  "));
        gtk_widget_set_size_request (label3, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label3, 145-15-12, 110);

        m_entryMonthEnd = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryMonthEnd), 2);
        gtk_widget_set_size_request (m_entryMonthEnd, 30, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryMonthEnd, 100-12, 110);
        g_signal_connect (G_OBJECT (m_entryMonthEnd), "insert-text", G_CALLBACK (on_entry_birth_date_end_month), this);

        GtkWidget *label4 = gtk_label_new (_("D  "));
        gtk_widget_set_size_request (label4, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label4, 185-15+10-12, 110);

        m_entryDayEnd = gtk_entry_new ();
        gtk_entry_set_max_length (GTK_ENTRY(m_entryDayEnd), 2);
        gtk_widget_set_size_request (m_entryDayEnd, 30, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), m_entryDayEnd, 155-15+10-12, 110);
        g_signal_connect (G_OBJECT (m_entryDayEnd), "insert-text", G_CALLBACK (on_entry_birth_date_end_day), this);

        GtkWidget *label13 = gtk_label_new (_("Y  "));
        gtk_widget_set_size_request (label13, 10+10, 30);
        gtk_fixed_put (GTK_FIXED (fixedSearch), label13, 227+20-12, 110);
    }
    imgCalendarStart = gtk_image_new_from_file ("./res/icon/Calendar.png");
    eventboxCalendarStart = gtk_event_box_new ();
    gtk_widget_set_size_request (eventboxCalendarStart, 40, 30);
    gtk_container_add (GTK_CONTAINER (eventboxCalendarStart), imgCalendarStart);
    gtk_fixed_put (GTK_FIXED (fixedSearch), eventboxCalendarStart, 243+30-12, 77);
    g_signal_connect (G_OBJECT (eventboxCalendarStart), 
            "button_press_event", 
            G_CALLBACK (HandleEventBoxCalendarStartPress), 
            this);

    imgCalendarEnd = gtk_image_new_from_file ("./res/icon/Calendar.png");
    eventboxCalendarEnd = gtk_event_box_new ();
    gtk_widget_set_size_request (eventboxCalendarEnd, 40, 30);
    gtk_container_add (GTK_CONTAINER (eventboxCalendarEnd), imgCalendarEnd);
    gtk_fixed_put (GTK_FIXED (fixedSearch), eventboxCalendarEnd, 243+30-12, 112);
	g_signal_connect (G_OBJECT (eventboxCalendarEnd), 
			"button_press_event", 
			G_CALLBACK (HandleEventBoxCalendarEndPress), 
			this);

	btnClear = gtk_button_new ();
	gtk_fixed_put (GTK_FIXED (fixedSearch), btnClear, 310, 80);
	gtk_widget_set_size_request (btnClear, 100, 60);
	g_signal_connect(G_OBJECT(btnClear), "clicked", G_CALLBACK(HandleBtnClearClicked), this);

	alignmentBtnClear = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_container_add (GTK_CONTAINER (btnClear), alignmentBtnClear);

	hboxBtnClear = gtk_hbox_new (FALSE, 2);
	gtk_container_add (GTK_CONTAINER (alignmentBtnClear), hboxBtnClear);

	imgBtnClear = gtk_image_new_from_stock ("gtk-clear", GTK_ICON_SIZE_BUTTON);
	gtk_box_pack_start (GTK_BOX (hboxBtnClear), imgBtnClear, FALSE, FALSE, 0);

	labelBtnClear = gtk_label_new_with_mnemonic (_("Clear"));
	gtk_box_pack_start (GTK_BOX (hboxBtnClear), labelBtnClear, FALSE, FALSE, 0);

	btnSearch = gtk_button_new ();
	gtk_fixed_put (GTK_FIXED (fixedSearch), btnSearch, 420, 80);
	gtk_widget_set_size_request (btnSearch, 100, 60);
	g_signal_connect(G_OBJECT(btnSearch), "clicked", G_CALLBACK(HandleBtnSearchClicked), this);

	alignmentBtnSearch = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_container_add (GTK_CONTAINER (btnSearch), alignmentBtnSearch);

	hboxBtnSearch = gtk_hbox_new (FALSE, 2);
	gtk_container_add (GTK_CONTAINER (alignmentBtnSearch), hboxBtnSearch);

	imgSearch = gtk_image_new_from_stock ("gtk-find", GTK_ICON_SIZE_BUTTON);
	gtk_box_pack_start (GTK_BOX (hboxBtnSearch), imgSearch, FALSE, FALSE, 0);

	labelBtnSearch = gtk_label_new_with_mnemonic (_("Search"));
	gtk_box_pack_start (GTK_BOX (hboxBtnSearch), labelBtnSearch, FALSE, FALSE, 0);

	labelSearch = gtk_label_new (_("<b>Search</b>"));
	gtk_frame_set_label_widget (GTK_FRAME (frameSearch), labelSearch);
	gtk_label_set_use_markup (GTK_LABEL (labelSearch), TRUE);

	//create quick search area
	frameQSearch = gtk_frame_new (NULL);
	gtk_fixed_put (GTK_FIXED (fixedWin), frameQSearch, 800, 20);
	gtk_widget_set_size_request (frameQSearch, 200, 170);
	gtk_frame_set_shadow_type (GTK_FRAME (frameQSearch), GTK_SHADOW_ETCHED_OUT);

	labelQSearch = gtk_label_new (_("<b>Quick Search</b>"));
	gtk_frame_set_label_widget (GTK_FRAME (frameQSearch), labelQSearch);
	gtk_label_set_use_markup (GTK_LABEL (labelQSearch), TRUE);

	alignmentQSearch = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_container_add (GTK_CONTAINER (frameQSearch), alignmentQSearch);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignmentQSearch), 0, 0, 12, 0);

	fixedQSearch = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (alignmentQSearch), fixedQSearch);

	labelQSearchMode = gtk_label_new (_("Select Method:"));
	gtk_fixed_put (GTK_FIXED (fixedQSearch), labelQSearchMode, 5, 16);
	gtk_widget_set_size_request (labelQSearchMode, 160, 30);
	gtk_misc_set_alignment (GTK_MISC (labelQSearchMode), 0, 0.5);

	comboboxQSearch = gtk_combo_box_new_text ();
	gtk_fixed_put (GTK_FIXED (fixedQSearch), comboboxQSearch, 5, 56);
	gtk_widget_set_size_request (comboboxQSearch, 160, 30);
	gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxQSearch), _("Show All"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxQSearch), _("Show Today"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxQSearch), _("Show Latest Three Days"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxQSearch), _("Show Latest Week"));
	gtk_combo_box_append_text (GTK_COMBO_BOX (comboboxQSearch), _("Show Latest Month"));
	g_signal_connect(G_OBJECT(comboboxQSearch), "changed", G_CALLBACK(HandleComboQSearch), this); 

#ifdef EMP_3410
    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[0]))
#else
        if(CDCMRegister::GetMe()->IsAuthorize())
#endif
    {
        //create button area
        labelDisplay = gtk_label_new_with_mnemonic (_("Details"));
        imgDisplay = gtk_image_new_from_stock (GTK_STOCK_JUMP_TO, GTK_ICON_SIZE_BUTTON);
        btnDisplay = create_button_icon(labelDisplay, imgDisplay);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnDisplay, 20, 200);
        g_signal_connect(G_OBJECT(btnDisplay), "clicked", G_CALLBACK(HandleBtnDisplayClicked), this);

        labelDelete = gtk_label_new_with_mnemonic (_("Delete"));
        imgDelete = gtk_image_new_from_stock (GTK_STOCK_DELETE, GTK_ICON_SIZE_BUTTON);
        btnDelete = create_button_icon(labelDelete, imgDelete);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnDelete, 20, 270-10);
        g_signal_connect(G_OBJECT(btnDelete), "clicked", G_CALLBACK(HandleBtnDeleteClicked), this);

        labelSelectAll = gtk_label_new_with_mnemonic (_("Select All"));
        imgSelectAll = gtk_image_new_from_stock (GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON);
        btnSelectAll = create_button_icon(labelSelectAll, imgSelectAll);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnSelectAll, 20, 340-20);
        g_signal_connect(G_OBJECT(btnSelectAll), "clicked", G_CALLBACK(HandleBtnSelectAllClicked), this);

        labelUnselectAll = gtk_label_new_with_mnemonic (_("Deselect"));
        imgUnselectAll = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON);
        btnUnselectAll = create_button_icon(labelUnselectAll, imgUnselectAll);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnUnselectAll, 20, 410-30);
        g_signal_connect(G_OBJECT(btnUnselectAll), "clicked", G_CALLBACK(HandleBtnUnselectAllClicked), this);
		
		DicomSend::GetInstance()->createPopup(_("DICOM Server"),ViewArchive::HandleBtnSendDicomClicked);
		
	 	DicomSend::GetInstance()->createPopup(_("DICOM Printer"),DicomSendPrinter::HandleDicomSendPrinterClicked);
		
		labelSend = gtk_label_new_with_mnemonic (_("SendTo DICOM"));
        imgSend = gtk_image_new_from_stock (GTK_STOCK_REVERT_TO_SAVED, GTK_ICON_SIZE_BUTTON);
        btnSend = create_button_icon(labelSend, imgSend);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnSend, 20, (480-40));
        //g_signal_connect(G_OBJECT(btnSend), "clicked", G_CALLBACK(HandleBtnSendClicked), this);
        g_signal_connect(G_OBJECT(btnSend), "event", G_CALLBACK(DicomSend::HandleDicomBtnExportEvent), this);
        
#ifndef VET 
        label_query_retrieve= gtk_label_new_with_mnemonic (_("Query/Retrieve"));
        img_query_retrieve = gtk_image_new_from_stock (GTK_STOCK_CONVERT, GTK_ICON_SIZE_BUTTON);
        button_query_retrieve= create_button_icon(label_query_retrieve, img_query_retrieve);
        gtk_fixed_put (GTK_FIXED (fixedWin), button_query_retrieve, 20, 550-50);
        g_signal_connect(button_query_retrieve, "clicked", G_CALLBACK(HandleBtnQueryRetrieveClicked), this);
#endif
        labelExport = gtk_label_new_with_mnemonic (_("Export Data"));
        imgExport = gtk_image_new_from_stock (GTK_STOCK_SAVE, GTK_ICON_SIZE_BUTTON);
        btnExport = create_button_icon(labelExport, imgExport);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnExport, 20, 620-60);
        g_signal_connect(G_OBJECT(btnExport), "event", G_CALLBACK(HandleBtnExportEvent), this);

        //labelUdisk = gtk_label_new_with_mnemonic (_("Browse\nUdisk Image"));
        labelUdisk = gtk_label_new_with_mnemonic (_("Import Data"));
        gtk_label_set_justify (GTK_LABEL(labelUdisk), GTK_JUSTIFY_CENTER);
        imgUdisk = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
        btnUdisk = create_button_icon(labelUdisk, imgUdisk);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnUdisk, 20, 688-70);
        g_signal_connect(G_OBJECT(btnUdisk), "event", G_CALLBACK(HandleBtnUdiskEvent), this);

        labelExit = gtk_label_new_with_mnemonic (_("Exit"));
        imgExit = gtk_image_new_from_stock ("gtk-quit", GTK_ICON_SIZE_BUTTON);
        btnExit = create_button_icon(labelExit, imgExit);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnExit, 20, 760-80);
        g_signal_connect(G_OBJECT(btnExit), "clicked", G_CALLBACK(HandleBtnExitClicked), this);

        //Create udisk 
        m_menu_udisk = gtk_menu_new();

        GtkWidget *itemEmp = gtk_menu_item_new_with_label(_("Browse Udisk Image"));
        gtk_menu_shell_append(GTK_MENU_SHELL(m_menu_udisk), itemEmp);
        g_signal_connect(G_OBJECT (itemEmp), "activate", G_CALLBACK(HandleMenuItemEmpActivate), this);
        gtk_widget_show(itemEmp);

        GtkWidget *itemDicom = gtk_menu_item_new_with_label(_("Import DICOM File"));
        gtk_menu_shell_append(GTK_MENU_SHELL(m_menu_udisk), itemDicom);
        g_signal_connect(G_OBJECT (itemDicom), "activate", G_CALLBACK(HandleMenuItemDicomActivate), this);
        gtk_widget_show(itemDicom);
    }
    else
    {
        //create button area
        labelDisplay = gtk_label_new_with_mnemonic (_("Details"));
        imgDisplay = gtk_image_new_from_stock (GTK_STOCK_JUMP_TO, GTK_ICON_SIZE_BUTTON);
        btnDisplay = create_button_icon(labelDisplay, imgDisplay);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnDisplay, 20, 200);
        g_signal_connect(G_OBJECT(btnDisplay), "clicked", G_CALLBACK(HandleBtnDisplayClicked), this);

        labelDelete = gtk_label_new_with_mnemonic (_("Delete"));
        imgDelete = gtk_image_new_from_stock (GTK_STOCK_DELETE, GTK_ICON_SIZE_BUTTON);
        btnDelete = create_button_icon(labelDelete, imgDelete);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnDelete, 20, 270+10);
        g_signal_connect(G_OBJECT(btnDelete), "clicked", G_CALLBACK(HandleBtnDeleteClicked), this);

        labelSelectAll = gtk_label_new_with_mnemonic (_("Select All"));
        imgSelectAll = gtk_image_new_from_stock (GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON);
        btnSelectAll = create_button_icon(labelSelectAll, imgSelectAll);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnSelectAll, 20, 340+20);
        g_signal_connect(G_OBJECT(btnSelectAll), "clicked", G_CALLBACK(HandleBtnSelectAllClicked), this);

        labelUnselectAll = gtk_label_new_with_mnemonic (_("Deselect"));
        imgUnselectAll = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON);
        btnUnselectAll = create_button_icon(labelUnselectAll, imgUnselectAll);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnUnselectAll, 20, 410+30);
        g_signal_connect(G_OBJECT(btnUnselectAll), "clicked", G_CALLBACK(HandleBtnUnselectAllClicked), this);
#if 0
        labelSend = gtk_label_new_with_mnemonic (_("SendTo DICOM"));
        imgSend = gtk_image_new_from_stock (GTK_STOCK_CONVERT, GTK_ICON_SIZE_BUTTON);
        btnSend = create_button_icon(labelSend, imgSend);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnSend, 20, 480);
        g_signal_connect(G_OBJECT(btnSend), "clicked", G_CALLBACK(HandleBtnSendClicked), this);
#endif
        labelExport = gtk_label_new_with_mnemonic (_("Export Data"));
        imgExport = gtk_image_new_from_stock (GTK_STOCK_SAVE, GTK_ICON_SIZE_BUTTON);
        btnExport = create_button_icon(labelExport, imgExport);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnExport, 20, 550-30);
        g_signal_connect(G_OBJECT(btnExport), "event", G_CALLBACK(HandleBtnExportEvent), this);

        labelUdisk = gtk_label_new_with_mnemonic (_("Browse\nUdisk Image"));
        gtk_label_set_justify (GTK_LABEL(labelUdisk), GTK_JUSTIFY_CENTER);
        imgUdisk = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
        btnUdisk = create_button_icon(labelUdisk, imgUdisk);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnUdisk, 20, 620-20);
        g_signal_connect(G_OBJECT(btnUdisk), "clicked", G_CALLBACK(HandleBtnUdiskClicked), this);

        labelExit = gtk_label_new_with_mnemonic (_("Exit"));
        imgExit = gtk_image_new_from_stock ("gtk-quit", GTK_ICON_SIZE_BUTTON);
        btnExit = create_button_icon(labelExit, imgExit);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnExit, 20, 688-10);
        g_signal_connect(G_OBJECT(btnExit), "clicked", G_CALLBACK(HandleBtnExitClicked), this);
    }
    //create popup-menu
	m_menu = gtk_menu_new();

	GtkWidget *itemFlash = gtk_menu_item_new_with_label(_("Export to USB storage"));
	gtk_menu_shell_append(GTK_MENU_SHELL(m_menu), itemFlash);
	g_signal_connect(G_OBJECT (itemFlash), "activate", G_CALLBACK(HandleMenuItemFlashActivate), this);
	gtk_widget_show(itemFlash);

	GtkWidget *itemCD = gtk_menu_item_new_with_label(_("Export to CD/DVD"));
	gtk_menu_shell_append(GTK_MENU_SHELL(m_menu), itemCD);
	g_signal_connect(G_OBJECT (itemCD), "activate", G_CALLBACK(HandleMenuItemCDActivate), this);
	gtk_widget_show(itemCD);

	//create record area
	GtkWidget *scrollWin = gtk_scrolled_window_new(NULL, NULL);
	gtk_fixed_put (GTK_FIXED (fixedWin), scrollWin, 150, 200);

	m_treeview = create_record_treeview();
	gtk_container_add(GTK_CONTAINER(scrollWin), m_treeview);
	gtk_widget_set_size_request (m_treeview, 834, 528);

	//create preivew
	m_framePreview = CreatePreview();
	gtk_fixed_put (GTK_FIXED (fixedWin), m_framePreview, 150, 620);

	gtk_widget_show_all (m_window);
	gtk_widget_hide_all (m_framePreview);
	
	g_keyInterface.Push(this);
	SetSystemCursorToCenter();
}

void ViewArchive::DestroyWindow(void)
{
	ClearPreview();

	if(GTK_IS_WIDGET(m_window))
	{
		g_keyInterface.Pop();
		gtk_widget_destroy(m_window);
		DicomSend::GetInstance()->destroyItem();

		if (g_keyInterface.Size() == 1)
			SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
        m_window = NULL;
	}
}

static gboolean ExitWindow(gpointer data)
{
    ViewArchive *tmp;
    tmp = (ViewArchive *)data;
    tmp->DestroyWindow();
    return FALSE;
}

void ViewArchive::KeyEvent(unsigned char keyValue)
{
	FakeXEvent::KeyEvent(keyValue);

	switch(keyValue)
	{
		case KEY_ESC:
		case KEY_ARCHIVE:
		    g_timeout_add(100, ExitWindow, this);
		    FakeEscKey();
		    break;

		default:
			break;
	}
}

void ViewArchive::DisplaySearchResult(vector<Database::NewPatSearchResult> &Result)
{
	int len = Result.size();
	int i;
	GtkTreeModel *model;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
	gtk_list_store_clear(GTK_LIST_STORE(model));

	for (i = 0; i < len; i ++)
	{
        /*
#ifdef VET
		cout << "name =" << Result[i].animal_name << endl;
		cout << "id = " << Result[i].ID << endl;
		cout << "sex = " << Result[i].gender << endl;
		cout << "examDate = " << Result[i].examDate << endl;
		cout << "examType = " << Result[i].examType << endl; 
#else
		cout << "name =" << Result[i].name << endl;
		cout << "id = " << Result[i].ID << endl;
		cout << "sex = " << Result[i].gender << endl;
		cout << "examDate = " << Result[i].examDate << endl;
		cout << "examType = " << Result[i].examType << endl; 
#endif
        */
#ifndef VET 
        int studyNo = atoi(Result[i].examID.c_str());
        bool value = CDCMMan::GetMe()->GetStudyBackupStatus(studyNo);
        string backupStatus = "";
        if(value)
            backupStatus = _("Yes");
        else
            backupStatus = " ";
#endif
		GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE(model), &iter);	

#ifdef VET
        gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                COL_CHECKED, FALSE,
                COL_NAME, Result[i].animal_name.c_str(),  /*COL_ID, Result[i].ID.c_str(), COL_NAME, Result[i].animal_name.c_str(),*/
                COL_ID, Result[i].ID.c_str(),
                COL_OWNER, Result[i].owner_name.c_str(),
                COL_VARIETY, Result[i].species.c_str(),
                COL_SEX, Result[i].gender.c_str(),
                COL_BIRTHDAY, Result[i].birthDate.c_str(),
                COL_AGE, Result[i].age.c_str(),
                COL_EXAMDATE, Result[i].examDate.c_str(),
                COL_EXAMTYPE, Result[i].examType.c_str(),
                COL_EXAMID, Result[i].examID.c_str(),
                -1);
#else
        gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                COL_CHECKED, FALSE,
                COL_NAME, Result[i].name.c_str(),
                COL_ID, Result[i].ID.c_str(),
                COL_SEX, Result[i].gender.c_str(),
                COL_BIRTHDAY, Result[i].birthDate.c_str(),
                COL_AGE, Result[i].age.c_str(),
                COL_EXAMDATE, Result[i].examDate.c_str(),
                COL_EXAMTYPE, Result[i].examType.c_str(),
                COL_EXAMID, Result[i].examID.c_str(),
                COL_BACKUPSTATUS,backupStatus.c_str(),
                -1);

#endif
	}
}

void ViewArchive::GetTimeBefore(int daysBefore, int &year, int &mon, int &day)
{
	string sYear, sMon, sDay;

	time_t now;
	time(&now);
	
	now -= daysBefore*24*60*60;

	struct tm* ct;
	ct = localtime(&now);

	year = ct->tm_year + 1900;
	mon = ct->tm_mon + 1;
	day = ct->tm_mday;
}

/*
 * @brief fill diskspace info when diskType and radioButton actived is changed
 * 
 * @para diskType[in] type of disk which will be statisticed, rang(HARDDISK, FLASH)
 * @para radioButton[in] index of radio button which is selected, range(UNIT_KB, UNIT_MB)
 */
void ViewArchive::FillDiskSpace(void)
{
	// get disk type
	const char* path = STORE_PATH;
#if 0
	int diskType;
	diskType = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxSource));
	if (diskType == -1)
	{
		PRINTF("Invalid disk type\n");
		return;
	}

	const char* path = "/mnt/harddisk";
	if (diskType == HARDDISK)
		path = "/mnt/harddisk";
		//path = "/boot";
	else if (diskType == FLASH)
		path = "/mnt/udisk";
//		path = "/media/disk";
		//path = "/home";
#endif

//	PRINTF("DISK space: path = %s\n", path);
	// get disk space
	struct statfs stat;
	if (statfs(path, &stat) == -1)
	{
		perror("statfs error:");
		return;
	}

	//PRINTF("+++++++++++++++++++++total blocks = %ld, free block = %ld, avail block = %ld, size of block size = %d\n", stat.f_blocks, stat.f_bfree, stat.f_bavail, stat.f_bsize);
	unsigned long totalSize;
	unsigned long availSize;
	float devide = 1.0;
	gchar buf[100];
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtnKb))) //Kb
	{
		devide = 1024;
		totalSize = stat.f_blocks / devide * stat.f_bsize;
		availSize = stat.f_bavail / devide * stat.f_bsize;
		
		// display
		sprintf(buf, "%ld KB\n", totalSize);
		gtk_label_set_text(GTK_LABEL(m_labelEndtalSize), buf);
		sprintf(buf, "%ld KB\n", availSize);
		gtk_label_set_text(GTK_LABEL(m_labelFreeSize), buf);
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtnMb))) //Mb
	{
		devide = 1024*1024;
		totalSize = stat.f_blocks / devide * stat.f_bsize;
		availSize = stat.f_bavail / devide * stat.f_bsize;

		// display
		sprintf(buf, "%ld MB\n", totalSize);
		gtk_label_set_text(GTK_LABEL(m_labelEndtalSize), buf);
		sprintf(buf, "%ld MB\n", availSize);
		gtk_label_set_text(GTK_LABEL(m_labelFreeSize), buf);
	}
	else //Gb
	{
		devide = 1024*1024*1024;
		totalSize = stat.f_blocks / devide * stat.f_bsize;
		availSize = stat.f_bavail / devide * stat.f_bsize;

		// display
		sprintf(buf, "%ld GB\n", totalSize);
		gtk_label_set_text(GTK_LABEL(m_labelEndtalSize), buf);
		sprintf(buf, "%ld GB\n", availSize);
		gtk_label_set_text(GTK_LABEL(m_labelFreeSize), buf);
	}

	gdouble fraction = (totalSize - availSize ) / (gdouble)totalSize;
	sprintf(buf, "%d%%\n", (int)(fraction*100));
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(m_progressbarSource), fraction);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(m_progressbarSource), buf);

}
void ViewArchive::EventBoxCalendarStartPress(GtkWidget *widget, GdkEventButton *event)
{
	ViewCalendar::GetInstance()->CreateWindow(m_window, ViewCalendar::START, this);
}

void ViewArchive::EventBoxCalendarEndPress(GtkWidget *widget, GdkEventButton *event)
{
	ViewCalendar::GetInstance()->CreateWindow(m_window, ViewCalendar::END, this);
}

#if 1
static gboolean LoadUdiskData(gpointer data)
{
	PeripheralMan *ptr = PeripheralMan::GetInstance();

	if(!ptr->CheckUsbStorageState())
	{
		ViewDialog::GetInstance()->Destroy();
		ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
				ViewDialog::ERROR,
				_("No USB storage is found!"), 
				NULL);
		return FALSE;
	}
	else
	{
		if(!ptr->MountUsbStorage())
		{
			ViewDialog::GetInstance()->Destroy();
			ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
					ViewDialog::ERROR,
					_("Failed to mount USB storage!"), 
					NULL);
			return FALSE;
		}
	}

	//copy show image/cine to harddisk
	char *src = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, NULL);
	char *dest = g_build_path(G_DIR_SEPARATOR_S, SLIDE_PATH, NULL);
	FileMan fm;
	fm.DelDirectory(dest);
	int ret = fm.CopyDirectory(src, dest);
	g_free(src);
	g_free(dest);
	ptr->UmountUsbStorage();
	ViewDialog::GetInstance()->Destroy();
	if(ret < 0)
	{
		ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
				ViewDialog::INFO,
				_("Failed to load data from USB storage!"), 
				NULL);
		return FALSE;
	}
	
	vector<string> vecPatExamID;
	vecPatExamID.clear();
	ViewArchiveImgMan::GetInstance()->CreateWindow(vecPatExamID);

	return FALSE;
}
#endif

void SelectUdiskData(void)
{
	PeripheralMan *ptr = PeripheralMan::GetInstance();

	if(!ptr->CheckUsbStorageState())
	{
		ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
				ViewDialog::ERROR,
				_("No USB storage is found!"), 
				NULL);
		return;
	}
	else
	{
		if(!ptr->MountUsbStorage())
		{
			ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
					ViewDialog::ERROR,
					_("Failed to mount USB storage!"), 
					NULL);
			return;
		}
	}

	ViewUdiskDataSelect::GetInstance()->CreateWindow(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()));

	ptr->UmountUsbStorage();
}

void ViewArchive::BtnUdiskClicked(GtkButton *button)
{
	SelectUdiskData();

#ifdef EMP_3410
    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[0]))
#else
        if(CDCMRegister::GetMe()->IsAuthorize())
#endif
    {
        PRINTF("Load From U disk!\n");
        ViewHintDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()),
                _("Please wait, loading data from USB storage..."));

        g_timeout_add(500, LoadUdiskData, this);
    }
}

void ViewArchive::BtnClearClicked(GtkButton *button)
{
	gtk_entry_set_text(GTK_ENTRY(m_entryID), "");
	gtk_entry_set_text(GTK_ENTRY(m_entryName), "");
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxSex), -1);
	gtk_entry_set_text(GTK_ENTRY(m_entryAge), "");
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxAgeUnit), 0);
	gtk_entry_set_text(GTK_ENTRY(m_entryYearStart), "");
	gtk_entry_set_text(GTK_ENTRY(m_entryMonthStart), "");
	gtk_entry_set_text(GTK_ENTRY(m_entryDayStart), "");
	gtk_entry_set_text(GTK_ENTRY(m_entryYearEnd), "");
	gtk_entry_set_text(GTK_ENTRY(m_entryMonthEnd), "");
	gtk_entry_set_text(GTK_ENTRY(m_entryDayEnd), "");
#ifdef VET 
    //clear  Owner,species,Gender
    gtk_entry_set_text(GTK_ENTRY(m_entryOwnerName),"");
    //gtk_entry_set_text(GTK_ENTRY(m_entrySpecies), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxSpecies), 0);
#endif
}

void ViewArchive::BtnSearchClicked(GtkButton *button)
{
    char buf[40];

    int combQItem = gtk_combo_box_get_active(GTK_COMBO_BOX (comboboxQSearch)); 
    if (combQItem != -1)
        gtk_combo_box_set_active(GTK_COMBO_BOX (comboboxQSearch),-1); //设置"快速搜索"方式为null
    
	// get search conditions
	Database::NewPatSearchTerm searchInfo;
	GetEntryTextForDB(m_entryID, searchInfo.id);
//	GetEntryTextForDB(m_entryName, searchInfo.totalName);

#ifdef VET
    GetEntryTextForDB(m_entryName, searchInfo.animal_name);
    GetEntryTextForDB(m_entryOwnerName, searchInfo.owner_name);
    int species;
    species = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxSpecies))-1;
    if (species < 0)
        sprintf(buf, "%%");
    else
        sprintf(buf, "%d", species);
    searchInfo.species = buf;	
#else
	string pat_name;
	GetEntryTextForDB(m_entryName, pat_name);
	if (pat_name == "%") {
		searchInfo.totalName = pat_name;
	} else {
		string::size_type pos = 0;
		while ((pos = pat_name.find_first_of(' ', pos)) != string::npos) {
			pat_name[pos] = '%';
			++pos;
		}
		searchInfo.totalName = "%" + pat_name + "%";
	}
#endif
	int gender;
	gender = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxSex)) - 1;
	if (gender < 0)
		sprintf(buf, "%%");
	else
		sprintf(buf, "%d", gender);
	searchInfo.gender = buf;
	
	GetEntryTextForDB(m_entryAge, searchInfo.age);
        if (searchInfo.age != "%") {
            int ageUnit = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxAgeUnit));
            sprintf(buf, "%d", ageUnit);
        } else {
            sprintf(buf, "%%");
        }
	searchInfo.ageUnit = buf;
	
	// examDateBegin
	string year, mon, day;
	GetEntryTextForDB(m_entryYearStart, year);
	if (strcmp(year.c_str(), "%") == 0) //if value is not set
		year = "0000";
	GetEntryTextForDB(m_entryMonthStart, mon);
	if (strcmp(mon.c_str(), "%") == 0)
		mon = "00";
	GetEntryTextForDB(m_entryDayStart, day);
	if (strcmp(day.c_str(), "%") == 0)
		day = "00";
	searchInfo.examDateStartYear = year;
	searchInfo.examDateStartMonth = mon;
	searchInfo.examDateStartDay = day;

    // examDateEnd
    int iYear, iMon, iDay;
    GetCurrentDate(iYear, iMon, iDay);
    string cYear, cMon, cDay;
    ChangeDateFormatToString(iYear, iMon, iDay, cYear, cMon, cDay);
    GetEntryTextForDB(m_entryYearEnd, year);
    if (strcmp(year.c_str(), "%") == 0) // if value is not set
        year = cYear;
    GetEntryTextForDB(m_entryMonthEnd, mon);
    if (strcmp(mon.c_str(), "%") == 0) 
        mon = cMon;
    GetEntryTextForDB(m_entryDayEnd, day);
    if (strcmp(day.c_str(), "%") == 0) 
        day = cDay;
    searchInfo.examDateEndYear= year;
    searchInfo.examDateEndMonth = mon;
    searchInfo.examDateEndDay = day;

    // search patient info according to conditions that user set
    vector<Database::NewPatSearchResult> dbResult;
    Database db;
    db.ArchiveSearch(searchInfo, dbResult);

    // display 
    DisplaySearchResult(dbResult);
}

vector<string> ViewArchive::GetSelExamID(void)
{
	vector<string> vec;
	vec.clear();

	// get all selected patient's id from tree model
	GtkTreeModel *model;
	gboolean exist;
	GtkTreeIter iter;
	gboolean isSelect;
	char* temp;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
	exist = gtk_tree_model_get_iter_first(model, &iter);
	while(exist)
	{
		gtk_tree_model_get(model, &iter, COL_CHECKED, &isSelect, -1);
		if (isSelect)
		{
			//delete from database
			gtk_tree_model_get(model, &iter, COL_EXAMID, &temp, -1);
			string examID = temp;
            //for (int ii = 0; ii <strlen(temp); ii++)
            // printf("select the ID is %c\n",temp[ii]);
			free(temp);
			
			vec.push_back(examID);
		}
		exist = gtk_tree_model_iter_next(model, &iter);
	}
	PRINTF("ViewArchive:: size of selected patient is %d\n", vec.size());

	return vec;
}

void ViewArchive::BtnDisplayClicked(GtkButton *button)
{
	vector<string> vecPatExamID = GetSelExamID();
	
	// display image management dialog
	if (vecPatExamID.size() == 0)
	{
		ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("No record is selected!"), NULL);
	}
	else
	{
		ViewArchiveImgMan::GetInstance()->CreateWindow(vecPatExamID);
		PRINTF("exit ViewArchive.cpp : displayClicked\n");
	}
}
#ifndef VET
void ViewArchive::BtnQueryRetrieveClicked(GtkButton *button)
{
    string device = CDCMMan::GetMe()->GetDefaultQueryRetrieveServiceDevice();
    if (device == "")
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), ViewDialog::ERROR, _("Please Set the default query/retrieve service in system setting"), NULL);
        return ;
    }
    else
        ViewQueryRetrieve::GetInstance()->CreateQueryRetrieveWin(m_window);
}
#endif
int SureToDelete(gpointer data)
{
	GtkTreeView *treeview;
	GtkTreeModel *model;
	gboolean exist;
	GtkTreeIter iter;
	gboolean isSelect;
	char* sid;

	treeview = GTK_TREE_VIEW(ViewArchive::GetInstance()->GetTreeView());
	model = gtk_tree_view_get_model(treeview);
	exist = gtk_tree_model_get_iter_first(model, &iter);
	while(exist)
	{
		gtk_tree_model_get(model, &iter, ViewArchive::COL_CHECKED, &isSelect, -1);
		if (isSelect)
		{
			gtk_tree_model_get(model, &iter, ViewArchive::COL_EXAMID, &sid, -1);

			//delete from model
			exist = gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

			//delete user image
			char *path = g_build_filename(STORE_PATH, sid, NULL);
			FileMan fm;
			if(fm.DelDirectory(path) < 0)
			{
				g_free(path);
				free(sid);
				continue;
			}
			g_free(path);

			//delete from database
			string examID = sid;
			Database db;
			db.DeleteRecord(examID);

            int studyID = atoi(sid);
            if(CDCMMan::GetMe()->DeleteStudy(studyID))
            {
                PRINTF("------DeleteStudy successfully\n");
            }	

			free(sid);
		}
		else
		{
			exist = gtk_tree_model_iter_next(model, &iter);
		}
	}

	GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
	ViewArchive::GetInstance()->TreeSelectionChanged(selection);

	return 0;
}
	
/* 
 * @brief seek out the selected item, then delete it according examID
 */
void ViewArchive::BtnDeleteClicked(GtkButton *button)
{
	vector<string> vecPatExamID = GetSelExamID();
	if (vecPatExamID.size() == 0)
	{
		ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("No Record is selected!"), NULL);
	}
	else
	{
		const char* info = N_("Warnning: The patient data will be permanently deleted!\nSure to delete?");
		ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::QUESTION, _(info), SureToDelete);
	}
}

void ViewArchive::BtnSelectAllClicked(GtkButton *button)
{
	gboolean valid;
	GtkTreeIter iter;
	GtkTreeModel *model;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
	valid = gtk_tree_model_get_iter_first(model, &iter);
	while(valid)
	{
		gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_CHECKED, TRUE, -1);
		valid = gtk_tree_model_iter_next (model, &iter);
	}
}

void ViewArchive::BtnUnselectAllClicked(GtkButton *button)
{
	gboolean valid;
	GtkTreeIter iter;
	GtkTreeModel *model;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
	valid = gtk_tree_model_get_iter_first(model, &iter);
	while(valid)
	{
		gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_CHECKED, FALSE, -1);
		valid = gtk_tree_model_iter_next (model, &iter);
	}
}

void ViewArchive::BtnComboboxQSearch(GtkComboBox *combobox)
{
    int item = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));

    string dateBegin, dateEnd;
    int year, mon, day;
    string sYear, sMon, sDay;
    Database::NewPatSearchTerm searchInfo;
    /**
     * 清空搜索方式下的起始时间和结束时间“
     * 2012.08.29
     * lihaumei
     */
    if (item !=-1)
    {
        gtk_entry_set_text(GTK_ENTRY(m_entryYearStart), "");
        gtk_entry_set_text(GTK_ENTRY(m_entryMonthStart), "");
        gtk_entry_set_text(GTK_ENTRY(m_entryDayStart), "");
        gtk_entry_set_text(GTK_ENTRY(m_entryYearEnd), "");
        gtk_entry_set_text(GTK_ENTRY(m_entryMonthEnd), "");
        gtk_entry_set_text(GTK_ENTRY(m_entryDayEnd), "");
    }

    GetCurrentDate(year, mon, day);
    ChangeDateFormatToString(year, mon, day, sYear, sMon, sDay);
    searchInfo.examDateEndYear = sYear;
    searchInfo.examDateEndMonth = sMon;
    searchInfo.examDateEndDay = sDay;
    switch(item)
	{
		case -1:
			return;
			break;

		case 0: //show all
			ChangeDateFormatToString(0, 0, 0, sYear, sMon, sDay);
			break;

		case 1: // show today
			break;

        case 2: //show latest three days
			GetTimeBefore(3, year, mon, day);
			ChangeDateFormatToString(year, mon, day, sYear, sMon, sDay);
			break;

		case 3: // show latest week
			//GetTimeBefore(14, year, mon, day);
			GetTimeBefore(7, year, mon, day);
			ChangeDateFormatToString(year, mon, day, sYear, sMon, sDay);
			break;

		case 4: // show latest month
			GetTimeBefore(30, year, mon, day);
			ChangeDateFormatToString(year, mon, day, sYear, sMon, sDay);
			break;

		default:
			break;
	}
	searchInfo.examDateStartYear = sYear;
	searchInfo.examDateStartMonth = sMon;
	searchInfo.examDateStartDay = sDay;

	// search
    vector<Database::NewPatSearchResult> dbResult;
	Database db;
	db.QuichSearch(searchInfo, dbResult);

    //display
    DisplaySearchResult(dbResult);

}

#if 0
void ViewArchive::BtnComboboxSource(GtkComboBox *combobox)
{
	FillDiskSpace();
}

void ViewArchive::BtnToggleKb(GtkToggleButton *toggleButton)
{
	FillDiskSpace();
}
void ViewArchive::BtnToggleMb(GtkToggleButton *toggleButton)
{
	FillDiskSpace();
}
#endif

static GCancellable* cancellable = NULL;
static int CancelCopyToFlash(gpointer data)
{
    PRINTF("Cancel copy!\n");
    g_cancellable_cancel(cancellable);
    return 0;
}

gboolean SendToDicom(gpointer data)
{
	ViewArchive *tmp;
	tmp = (ViewArchive *)data;
	tmp->GetSelToDicom();
	return FALSE;
}

void ViewArchive::BtnSendClicked()
{
	vector<string> vecPatExamID = GetSelExamID();
	if (vecPatExamID.size() == 0)
	{
		ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("No Record is selected!"), NULL);
	}
	else
	{
           // ViewHintDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), _("Sending..."));
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
                ViewDialog::PRG_CANCEL,
                _("Sending..."), 
                CancelCopyToFlash);

        g_timeout_add(500, SendToDicom, this);
    }
}

int ViewArchive::BtnExportEvent(GtkWidget *widget, GdkEvent *event)
{
	if (event->type == GDK_BUTTON_PRESS) {
		GdkEventButton *bevent = (GdkEventButton *) event; 
		gtk_menu_popup (GTK_MENU (m_menu), NULL, NULL, NULL, NULL,
				bevent->button, bevent->time);
		return TRUE;
	}
	return FALSE;
}

int ViewArchive::BtnImportEvent(GtkWidget *widget, GdkEvent *event)
{
	if (event->type == GDK_BUTTON_PRESS) {
		GdkEventButton *bevent = (GdkEventButton *) event; 
		gtk_menu_popup (GTK_MENU (m_menu_udisk), NULL, NULL, NULL, NULL,
				bevent->button, bevent->time);
		return TRUE;
	}
	return FALSE;
}

void ViewArchive::BtnExitClicked(GtkButton *button)
{
    DestroyWindow();
}

void ViewArchive::SetStartDate(int year, int month, int day)
{
    /*clear end date*/
    gtk_entry_set_text(GTK_ENTRY(m_entryYearEnd), "");
    gtk_entry_set_text(GTK_ENTRY(m_entryMonthEnd), "");
    gtk_entry_set_text(GTK_ENTRY(m_entryDayEnd), "");
    m_iEnd = 0;

    guint i;
    int pos, icurrent;
    char tmp[10];
    char str[2];
    time_t now;
    struct tm *tm_now;
    m_iStart = year * 10000 + month * 100 + day * 1;
    now = time(NULL);
    tm_now = localtime(&now);
    icurrent = (tm_now->tm_year + 1900) * 10000 + (tm_now->tm_mon + 1) * 100 + tm_now->tm_mday * 1;
    if (m_iEnd == 0)
        m_iEnd = icurrent;
    if ((m_iStart > m_iEnd) || (m_iStart > icurrent))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                ViewDialog::ERROR,
                _("The time input is not correct!"), 
                NULL);
        m_iStart = 0;
        return;

    }
    else
    {
        pos = 0;
        gtk_entry_set_text(GTK_ENTRY(m_entryYearStart), "");
        sprintf(tmp, "%d", year);
        for(i=0; i<strlen(tmp); i++)
        {
            sprintf(str, "%c", tmp[i]);
            gtk_editable_insert_text(GTK_EDITABLE(m_entryYearStart), str, 1, &pos);
        }

        pos = 0;
        gtk_entry_set_text(GTK_ENTRY(m_entryMonthStart), "");
        sprintf(tmp, "%d", month);
        for(i=0; i<strlen(tmp); i++)
        {
            sprintf(str, "%c", tmp[i]);
            gtk_editable_insert_text(GTK_EDITABLE(m_entryMonthStart), str, 1, &pos);
        }

        pos = 0;
        gtk_entry_set_text(GTK_ENTRY(m_entryDayStart), "");
        sprintf(tmp, "%d", day);
        for(i=0; i<strlen(tmp); i++)
        {
            sprintf(str, "%c", tmp[i]);
            gtk_editable_insert_text(GTK_EDITABLE(m_entryDayStart), str, 1, &pos);
        }
    }
    //set up the same date
    SetEndDate(year, month, day);
}

void ViewArchive::SetEndDate(int year, int month, int day)
{
	guint i; 
    int pos, iCurrent;
    char tmp[10];
    char str[2];
    time_t now;
    struct tm *tm_now;
    now = time(NULL);
    tm_now = localtime(&now);
    iCurrent =  (tm_now->tm_year + 1900) * 10000 + (tm_now->tm_mon + 1) * 100 + tm_now->tm_mday * 1;
    m_iEnd = year * 10000 + month * 100 + day * 1;
    if (( m_iStart > m_iEnd) || (m_iEnd > iCurrent))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                ViewDialog::ERROR,
                _("The time input is not correct!"), 
                NULL);
        m_iEnd = 0;
    }
    else 
    {
        pos = 0;
        gtk_entry_set_text(GTK_ENTRY(m_entryYearEnd), "");
        sprintf(tmp, "%d", year);
        for(i=0; i<strlen(tmp); i++)
        {
            sprintf(str, "%c", tmp[i]);
            gtk_editable_insert_text(GTK_EDITABLE(m_entryYearEnd), str, 1, &pos);
        }

        pos = 0;
        gtk_entry_set_text(GTK_ENTRY(m_entryMonthEnd), "");
        sprintf(tmp, "%d", month);
        for(i=0; i<strlen(tmp); i++)
        {
            sprintf(str, "%c", tmp[i]);
            gtk_editable_insert_text(GTK_EDITABLE(m_entryMonthEnd), str, 1, &pos);
        }

        pos = 0;
        gtk_entry_set_text(GTK_ENTRY(m_entryDayEnd), "");
        sprintf(tmp, "%d", day);
        for(i=0; i<strlen(tmp); i++)
        {
            sprintf(str, "%c", tmp[i]);
            gtk_editable_insert_text(GTK_EDITABLE(m_entryDayEnd), str, 1, &pos);
        }
    }
}

void ViewArchive::BirthDateYearInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");

    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    if (old_str.length() == 4) {
        int year = atoi(old_str.c_str());
        if (year < 1900) {
            *position = 4;      // 移动光标之输入框末尾
           // gtk_entry_set_text(GTK_ENTRY(m_entryBirthYear), "1900"); //2012.08.17
            gtk_entry_set_text(GTK_ENTRY(m_entryYearStart), "1900");
            year = 1900;
        } else if (year > m_curYear) {
            char curYear[5];
            sprintf(curYear, "%d", m_curYear);
            gtk_entry_set_text(GTK_ENTRY(m_entryYearStart), curYear);
            year = m_curYear;
            *position = 4;      // 移动光标之输入框末尾
        }
        gtk_widget_grab_focus(m_entryMonthStart);
    }
    return;
}

void ViewArchive::BirthDateMonthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    if (old_str.length() != 0) {
        int month = atoi(old_str.c_str());
        if (month < 1) {
            g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
            return ;
        } else if (month > 12) {
#if 0
            char curMonth[5];
            sprintf(curMonth, "%d", m_curMonth);
            gtk_entry_set_text(GTK_ENTRY(m_entryMonthStart), "12");
            month = m_curMonth;
#endif
            gtk_entry_set_text(GTK_ENTRY(m_entryMonthStart), "12");
            month = 12;
            *position = 2;      // 移动光标之输入框末尾
        }
        if (old_str.length() == 2)
            gtk_widget_grab_focus(m_entryDayStart);
    }
    return;
}

void ViewArchive::BirthDateDayInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    int day = atoi(old_str.c_str());
    if (day < 1) {
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
        return ;
    }
    const char *year_text = gtk_entry_get_text(GTK_ENTRY(m_entryYearStart));
    const char *month_text = gtk_entry_get_text(GTK_ENTRY(m_entryMonthStart));
    if (strlen(year_text) == 0 || strlen(year_text) < 4 || strlen(month_text) == 0) {
        if (day > 31) {
            *position = 2;      // 移动光标之输入框末尾
            gtk_entry_set_text(GTK_ENTRY(m_entryDayStart), "31");
        }
        return ;
    } else {
        int year = atoi(year_text);
        int month = atoi(month_text);
        int last_day = g_date_get_days_in_month (GDateMonth(month), GDateYear(year));
        if (day > last_day) {
           char str_last_day[3];
            sprintf(str_last_day, "%d", last_day);
            gtk_entry_set_text(GTK_ENTRY(m_entryDayStart), str_last_day);
            day = last_day;
            *position = 2;      // 移动光标之输入框末尾
        }
    }
    return;
}

void ViewArchive::BirthDateEndYearInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");

    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    if (old_str.length() == 4) {
        int year = atoi(old_str.c_str());
        if (year < 1900) {
            *position = 4;      // 移动光标之输入框末尾
            gtk_entry_set_text(GTK_ENTRY(m_entryYearEnd), "1900");
            year = 1900;
        } else if (year > m_curYear) {
            char curYear[5];
            sprintf(curYear, "%d", m_curYear);
            gtk_entry_set_text(GTK_ENTRY(m_entryYearEnd), curYear);
            year = m_curYear;
            *position = 4;      // 移动光标之输入框末尾
        }
        gtk_widget_grab_focus(m_entryMonthEnd);
    }
    return;
}

void ViewArchive::BirthDateEndMonthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    if (old_str.length() != 0) {
        int month = atoi(old_str.c_str());
        if (month < 1) {
            g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
            return ;
        } else if (month > 12) {
#if 0
            char curMonth[5];
            sprintf(curMonth, "%d", m_curMonth);
            gtk_entry_set_text(GTK_ENTRY(m_entryMonthStart), "12");
            month = m_curMonth;
#endif
            gtk_entry_set_text(GTK_ENTRY(m_entryMonthEnd), "12");
            month = 12;
            *position = 2;      // 移动光标之输入框末尾
        }
        if (old_str.length() == 2)
            gtk_widget_grab_focus(m_entryDayEnd);
    }
    return;
}

void ViewArchive::BirthDateEndDayInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    int day = atoi(old_str.c_str());

    if (day < 1) {
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
        return ;
    }
    const char *year_text = gtk_entry_get_text(GTK_ENTRY(m_entryYearEnd));
    const char *month_text = gtk_entry_get_text(GTK_ENTRY(m_entryMonthEnd));
    if (strlen(year_text) == 0 || strlen(year_text) < 4 || strlen(month_text) == 0) {
        if (day > 31) {
            *position = 2;      // 移动光标之输入框末尾
            gtk_entry_set_text(GTK_ENTRY(m_entryDayEnd), "31");
        }
        return ;
    } else {
        int year = atoi(year_text);
        int month = atoi(month_text);
        int last_day = g_date_get_days_in_month (GDateMonth(month), GDateYear(year));
        if (day > last_day) {
           char str_last_day[3];
            sprintf(str_last_day, "%d", last_day);
            gtk_entry_set_text(GTK_ENTRY(m_entryDayEnd), str_last_day);
            day = last_day;
            *position = 2;      // 移动光标之输入框末尾
        }
    }
    return;
}

void ViewArchive::EntryOnlyNumInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if(new_text_length != 1)
        gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
    else
    {
        if (!g_ascii_isalnum(*new_text))
        {
            if((g_ascii_strcasecmp(new_text, "-") !=0) && (g_ascii_strcasecmp(new_text, "_") !=0) &&  (g_ascii_strcasecmp(new_text, "/") !=0))
                g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
        }

        //	if(!g_ascii_isdigit(*new_text))
        //		gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
    }
}

GtkWidget* ViewArchive::create_record_treeview()
{
	GtkWidget *treeview;
    GtkTreeModel *model = NULL;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    treeview = gtk_tree_view_new ();
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);

	renderer = gtk_cell_renderer_toggle_new ();
	g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK (HandleToggleRecord), this);
	column = gtk_tree_view_column_new_with_attributes ("", renderer, "active", COL_CHECKED, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 30, NULL);
#ifdef VET
    renderer = gtk_cell_renderer_text_new();
    //column = gtk_tree_view_column_new_with_attributes(_("ID"), renderer, "text", COL_ID, NULL);
    column = gtk_tree_view_column_new_with_attributes(_("Animal Name"), renderer, "text", COL_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("ID"), renderer, "text", COL_ID, NULL);
    //column = gtk_tree_view_column_new_with_attributes(_("Animal Name"), renderer, "text", COL_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Owner"), renderer, "text", COL_OWNER, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Species"), renderer, "text", COL_VARIETY, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 60, NULL);
#else
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Name"), renderer, "text", COL_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 180, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("ID"), renderer, "text", COL_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120+10, NULL);
#endif
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Gender"), renderer, "text", COL_SEX, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 60, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Date of Birth"), renderer, "text", COL_BIRTHDAY, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",100, NULL);//80

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Age"), renderer, "text", COL_AGE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 40, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Exam Date"), renderer, "text", COL_EXAMDATE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 80, NULL);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Exam Type"), renderer, "text", COL_EXAMTYPE, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column), "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 160,NULL);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Exam ID"),renderer, "text", COL_EXAMID, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column), "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 80, NULL);

    renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Backup Status"),renderer, "text", COL_BACKUPSTATUS, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column), "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 130, NULL);

#if 0
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Doctor"), renderer, "text", COL_DOCTOR, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);
#endif
    model = create_record_model();
    if (model != NULL)
		gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    g_object_unref (model); 

	GtkTreeSelection *select;
    select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
    gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
    g_signal_connect(select, "changed", G_CALLBACK(HandleTreeSelectionChanged), this);

	return treeview;
}

GtkTreeModel* ViewArchive::create_record_model()
{
	GtkListStore *store;
 //hlx 2012.1.4
#ifdef VET
    store = gtk_list_store_new(NUM_COLS,
            G_TYPE_BOOLEAN,
            G_TYPE_STRING, 
            G_TYPE_STRING, 
            G_TYPE_STRING, 
            G_TYPE_STRING, 
            G_TYPE_STRING, 
            G_TYPE_STRING, 
            G_TYPE_STRING, 
            G_TYPE_STRING,
            G_TYPE_STRING,
	         G_TYPE_STRING, 
            G_TYPE_STRING);
#else
    store = gtk_list_store_new(NUM_COLS, 
			G_TYPE_BOOLEAN, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING);
#endif
#if 0
    GtkTreeIter iter;
	gtk_list_store_append (store, &iter);
	gtk_list_store_set(store, &iter,
			   COL_CHECKED, FALSE,
			   COL_NAME, "James",
			   COL_ID, "000000",
			   COL_SEX, "male",
			   COL_AGE, "30",
			   COL_EXAMDATE, "2009/10/20",
			   COL_EXAMTYPE, "OB.",
			   COL_EXAMID, "1234567890",
			   COL_DOCTOR, "Kate",
			   -1);
#endif
#if 0
	gtk_list_store_append (store, &iter);
	gtk_list_store_set(store, &iter,
			   COL_CHECKED, FALSE,
			   COL_SID, "1",
			   COL_NAME, "Test",
			   COL_ID, "000001",
			   COL_SEX, "male",
			   COL_AGE, "23",
			   COL_EXAMDATE, "2009/11/18",
			   COL_DOCTOR, "Kate",
			   -1);
	gtk_list_store_append (store, &iter);
	gtk_list_store_set(store, &iter,
			   COL_CHECKED, FALSE,
			   COL_SID, "2",
			   COL_NAME, "Test",
			   COL_ID, "000001",
			   COL_SEX, "male",
			   COL_AGE, "43",
			   COL_EXAMDATE, "2009/11/28",
			   COL_DOCTOR, "Kate",
			   -1);
	gtk_list_store_append (store, &iter);
	gtk_list_store_set(store, &iter,
			   COL_CHECKED, FALSE,
			   COL_SID, "3",
			   COL_NAME, "Test",
			   COL_ID, "000001",
			   COL_SEX, "male",
			   COL_AGE, "43",
			   COL_EXAMDATE, "2009/11/28",
			   COL_DOCTOR, "Kate",
			   -1);

#endif

	return GTK_TREE_MODEL (store);
}

void ViewArchive::ToggleRecord(GtkCellRendererToggle *cell, gchar *path_str)
{
	GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(m_treeview));
	GtkTreeIter  iter;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
	gboolean checked;

	/* get toggled iter */
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter, COL_CHECKED, &checked, -1);

	/* do something with the value */
	checked ^= 1; //按位异或

	/* set new value */
	gtk_list_store_set (GTK_LIST_STORE (model), &iter, COL_CHECKED, checked, -1);

	/* clean up */
	gtk_tree_path_free (path);
}

void ViewArchive::MenuItemFlashActivate(GtkMenuItem *menuitem)
{
    //GetSelToFlash();
    //和光盘做一样的处理，出现文件选择框
    vector<string> vec;
	gboolean valid;
	gboolean value;
	gchar *sid;
    gchar *name;
    gchar *id;
	GtkTreeIter iter;
	GtkTreeModel *model;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
	valid = gtk_tree_model_get_iter_first(model, &iter);

    char name_id[256] = {0};
    m_vecCheckId.clear();
    m_vecFlashItem.clear();
	vec.clear();
	while(valid)
	{
		gtk_tree_model_get(model, &iter, COL_CHECKED, &value, -1);
		if(value)
		{
			gtk_tree_model_get(model, &iter, COL_EXAMID, &sid, -1);
			gtk_tree_model_get(model, &iter, COL_NAME, &name, -1);
			gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
            //strcpy(name_id, name);
            //strcat(name_id, id);
            sprintf(name_id, "%s%s", name, id);
			m_vecFlashItem.push_back(name_id);
            m_vecCheckId.push_back(sid);
            vec.push_back(sid);
			g_free(name);
            g_free(id);
            g_free(sid);
        }
		valid = gtk_tree_model_iter_next (model, &iter);
	}

    if(vec.empty())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                ViewDialog::ERROR,
                _("No record is selected!"), 
                NULL);
        return;
    }
    ViewCD::GetInstance()->LoadFlashItem(m_vecFlashItem);
    m_custom_win.CreateWin(m_window);
    //ViewCD::GetInstance()->CreateWindow(m_window, vec, false);
}

void ViewArchive::MenuItemCDActivate(GtkMenuItem *menuitem)
{
    vector<string> vec;
    gboolean valid;
    gboolean value;
    gchar *sid; 
    gchar *name;
    gchar *id;
    GtkTreeIter iter;
    GtkTreeModel *model;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
    valid = gtk_tree_model_get_iter_first(model, &iter);

    char name_id[256] = {0};
    m_vecCheckId.clear();
    m_vecFlashItem.clear();
    vec.clear();
    while(valid)
    {
        gtk_tree_model_get(model, &iter, COL_CHECKED, &value, -1);
        if(value)
        {
            gtk_tree_model_get(model, &iter, COL_EXAMID, &sid, -1);
            gtk_tree_model_get(model, &iter, COL_NAME, &name, -1);
            gtk_tree_model_get(model, &iter, COL_ID, &id, -1);
            sprintf(name_id, "%s%s", name, id);
            m_vecFlashItem.push_back(name_id);
            m_vecCheckId.push_back(sid);
            vec.push_back(sid);
			g_free(name);
            g_free(id);
            g_free(sid);
        }
        valid = gtk_tree_model_iter_next (model, &iter);
    }

    if(vec.empty())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                ViewDialog::ERROR,
                _("Please select the list you want to export the data!"), 
                NULL);
        return;
    }

    ViewCD::GetInstance()->LoadFlashItem(m_vecFlashItem);
    ViewCD::GetInstance()->CreateWindow(m_window, vec, true);
}

void ViewArchive::MenuItemEmpActivate(GtkMenuItem *menuitem)
{
	SelectUdiskData();
}

void ViewArchive::MenuItemDicomActivate(GtkMenuItem *menuitem)
{
   PeripheralMan *ptr = PeripheralMan::GetInstance();
    if(!ptr->CheckUsbStorageState())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                ViewDialog::ERROR,
                _("No USB storage found!"), 
                NULL);
        return;
    }
    else
    {
        if(!ptr->MountUsbStorage())
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                    ViewDialog::ERROR,
                    _("Failed to mount USB storage!"), 
                    NULL);
            return;
        }
    }
#ifndef VET
    ViewDicomDataSelect::GetInstance()->CreateWindow(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()));
#endif
    ptr->UmountUsbStorage();
}

void ViewArchive::PROGRESSSTATUS(int nPos)
{
    vector<string> vecPatExamID = m_ptrInstance->GetSelExamID();
    int size = vecPatExamID.size();
    double frac;
    if(size>1)
    {
        float x1 = m_ptrInstance->m_selNum/(float)size;
        float x2 = 1/(float)size;
        frac = x1 + nPos*0.01*x2;
    }
    else
        frac = nPos*0.01;
	gdk_threads_enter();
    ViewDialog::GetInstance()->SetProgressBar(frac);
    while(gtk_events_pending())
        gtk_main_iteration();
	gdk_threads_leave();

}

void ViewArchive::GetSelToDicom()
{
    vector<string> vec;
    gboolean valid;
    gboolean value;
    gchar *sid, *name, *id, *part;
    GtkTreeIter iter;
    GtkTreeModel *model;
    int count_success = 0;
    int count_failed = 0;

    string device = CDCMMan::GetMe()->GetDefaultStorageServiceDevice();
    if (device == "")
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), ViewDialog::ERROR, _("Please Set the default storage service in system setting"), NULL);
        return ;
    }
    Replay::GetInstance()->ClearCurReplayData();
    Replay::GetInstance()->DisplayReplayBar();
    m_selNum = 0;
    //ViewDialog::GetInstance()->SetProgressBar(0);
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
    valid = gtk_tree_model_get_iter_first(model, &iter);
    while(valid)
    {
        gtk_tree_model_get(model, &iter, COL_CHECKED, &value, -1);
        if(value)
        {
            vec.clear();
            gtk_tree_model_get(model, &iter, 
                    COL_NAME, &name,
                    COL_ID, &id,
                    COL_EXAMTYPE, &part,
                    COL_EXAMID, &sid,
                    -1);

            // get Image and video's full name(path + picture's name)
            ImgMan::GetInstance()->LoadSnap(atoi(sid), STORE_PATH, &vec);
            VideoMan::GetInstance()->LoadVideo(atoi(sid), STORE_PATH, &vec);

            vector<string>::iterator ite;
            char strTemp[256];
            for(ite = vec.begin(); ite < vec.end(); ite++)
            {
                sprintf(strTemp, "%s/%s/%s", STORE_PATH,  sid, (*ite).c_str());
                *ite = strTemp;
            }

            int studyNo = atoi(sid);
            char strSrcDir[256];
            sprintf(strSrcDir,"%s/%s",STORE_PATH,  sid);

            string strDesDir = STORE_PATH;;
            strDesDir += "/tmp";


            GFile *dir = g_file_new_for_path(strDesDir.c_str());
            GError *err_mkdir = NULL;
            if(!g_file_make_directory(dir, NULL, &err_mkdir))
            {
                if(err_mkdir->code==G_IO_ERROR_NOT_FOUND || err_mkdir->code==G_IO_ERROR_NOT_SUPPORTED)
                {
                    PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
                    g_error_free(err_mkdir);
                    g_object_unref(dir);
                    return;
                }
            }
            g_object_unref(dir);

            SysDicomSetting sysDicomSetting;
            bool onStorageCommitment = sysDicomSetting.GetStorageCommitment();
            bool enableStorageSR = sysDicomSetting.GetSendReport();
            bool enableStorageMultiFrame = sysDicomSetting.GetSendVideo();
            int maxFrameNumber=0;
            if(enableStorageMultiFrame)
                maxFrameNumber = sysDicomSetting.GetVideoFrames();
            else
                maxFrameNumber =0;

            if(CDCMMan::GetMe()->SendStudy(studyNo,strSrcDir,strDesDir,onStorageCommitment,enableStorageSR,enableStorageMultiFrame,maxFrameNumber,PROGRESSSTATUS))
            {
                m_selNum++;
                count_success ++;
                PRINTF("send dicom successfully!\n");
            }
            else
            {
                count_failed ++;
                PRINTF("send dicom failed!\n");
            }

            bool value = CDCMMan::GetMe()->GetStudyBackupStatus(studyNo);
            string backupStatus = "";
            if(value)
                backupStatus = _("Yes");
            else
                backupStatus = " ";

            gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                    COL_BACKUPSTATUS, backupStatus.c_str(),
                    -1);

            g_free(sid);
        }
        valid = gtk_tree_model_iter_next (model, &iter);
    }

    ViewDialog::GetInstance()->Destroy();
    char info[256];
    sprintf(info, "%s\n%s %d\n%s %d", _("Sending finished!"), _("Success:"), count_success,  _("Fail:"), count_failed);
    
    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            info,
            NULL);

            /* vector<string> vec;
               gboolean valid;
               gboolean value;
               gchar *sid, *name, *id, *part;
               GtkTreeIter iter;
    GtkTreeModel *model;
    CDCMData dcmData;
    int count_success = 0;
    int count_failed = 0;

    string device = CDCMMan::GetMe()->GetDefaultStorageServiceDevice();
    if (device == "")
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), ViewDialog::ERROR, _("Please Set the default storage service in system setting"), NULL);
        return ;
    }
#if 1

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
    valid = gtk_tree_model_get_iter_first(model, &iter);
    while(valid)
    {
        gtk_tree_model_get(model, &iter, COL_CHECKED, &value, -1);
        if(value)
        {
            vec.clear();
            gtk_tree_model_get(model, &iter, 
                    COL_NAME, &name,
                    COL_ID, &id,
                    COL_EXAMTYPE, &part,
                    COL_EXAMID, &sid,
                    -1);

            // get Image and video's full name(path + picture's name)
            ImgMan::GetInstance()->LoadSnap(atoi(sid), STORE_PATH, &vec);
            VideoMan::GetInstance()->LoadVideo(atoi(sid), STORE_PATH, &vec);

            vector<string>::iterator ite;
            char strTemp[256];
            for(ite = vec.begin(); ite < vec.end(); ite++)
            {
                sprintf(strTemp, "%s/%s/%s", STORE_PATH,  sid, (*ite).c_str());
                *ite = strTemp;
            }

            string examID = sid;
            PatientInfo::Info info;
            Database db;
            db.AllInfoSearch(examID, info);
            string year, mon, day, date;

            //the format of every parameter see include file.
            dcmData.SetName(name);
            dcmData.SetID(id);

            ChangeDateFormatToString(info.p.birthDate.year, info.p.birthDate.month, info.p.birthDate.day, year, mon, day); 
            date = year+mon+day;
            dcmData.SetBirthDate(date);	//YMD 19990807

            string age = "";
			string ageUnit = "";
			if (info.p.ageUnit == 0)
				ageUnit = "Y";
			else if (info.p.ageUnit == 1)
				ageUnit = "M";
			else if (info.p.ageUnit == 2)
				ageUnit = "D";

			char ageTemp[4];
			if (info.p.age > 999 || info.p.age <= 0)
				age = "";
			else
			{
				sprintf(ageTemp, "%03d", info.p.age);
				age = ageTemp + ageUnit;
			}
            dcmData.SetAge(age);

            string sex;
            if (info.p.sex == 0)
                dcmData.SetSex("F");	//F/M/O
            else if (info.p.sex == 1)
                dcmData.SetSex("M");	//F/M/O
            else
                dcmData.SetSex("O");	//F/M/O

            ChangeDateFormatToString(info.e.examDate.year, info.e.examDate.month, info.e.examDate.day, year, mon, day); 
            date = year + mon + day;
            dcmData.SetStudyDate(date);

            string hour, min, sec, time;
            ChangeTimeFormatToString(info.e.examTime.hour, info.e.examTime.minute, info.e.examTime.second, hour, min, sec); 
            time = hour + min + sec;
            dcmData.SetStudyTime(time);		//HMS

    
  			   dcmData.SetStudyDr(info.e.examDoctor); // to be continu

            dcmData.SetSID(sid);

 		//		dcmData.SetPart(part);	//OB/GY/UR/CA/AB/SP
			string partTemp = "";
			if (strcmp(part, _("Abdomen")) == 0 || strcmp(part, _("Adult Abdomen")) == 0 || strcmp(part, _("Adult Liver")) == 0 || strcmp(part, _("Kid Abdomen")) == 0 || strcmp(part, _("Kidney Ureter")) == 0
					 || strcmp(part, _("Bladder Prostate")) == 0 || strcmp(part, _("Abdo-adult")) == 0 || strcmp(part, _("Abdo-liver")) == 0 || strcmp(part, _("Abdo-kid")) == 0)
				partTemp = "ABDOMEN";
			else if (strcmp(part, _("Cardiac")) == 0 || strcmp(part, _("Adult Cardio")) == 0 || strcmp(part, _("Kid Cardio")) == 0 || strcmp(part, _("Fetus Cardio")) == 0 || strcmp(part, _("Car-adult")) == 0 || strcmp(part, _("Car-kid")) == 0 || strcmp(part, _("Car-fetus")) == 0) 
				partTemp = "HEART";
			else if (strcmp(part, _("Hip Joint")) == 0 || strcmp(part, _("Hip-joint")) == 0)
				partTemp = "HIP";
			else if (strcmp(part, _("Mammary Glands")) == 0 || strcmp(part, _("Glands")) == 0)
				partTemp = "BREAST";
			else
				partTemp = "";
            dcmData.SetPart(partTemp);	


            string hospital;
            TopArea::GetInstance()->GetHospitalName(hospital);
            dcmData.SetHospital(hospital); //nanshan hospital

            dcmData.SetMechineType("G70");
            dcmData.SetMechineSN("20090001");

            string strDir = STORE_PATH;;
            strDir += "/tmp";
            GFile *dir = g_file_new_for_path(strDir.c_str());
            GError *err_mkdir = NULL;
            if(!g_file_make_directory(dir, NULL, &err_mkdir))
            {
                if(err_mkdir->code==G_IO_ERROR_NOT_FOUND || err_mkdir->code==G_IO_ERROR_NOT_SUPPORTED)
                {
                    PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
                    g_error_free(err_mkdir);
                    g_object_unref(dir);
                    return;
                }
            }
            g_object_unref(dir);

            dcmData.SetImageDirectory(strDir);
            dcmData.SetAllImage(vec);

            if(CDCMMan::GetMe()->Send(dcmData))
            {
                count_success ++;
                PRINTF("send dicom successfully!\n");
            }
            else
            {
                count_failed ++;
                PRINTF("send dicom failed!\n");
            }

            g_free(sid);
            g_free(name);
            g_free(id);
        }
        valid = gtk_tree_model_iter_next (model, &iter);
    }

    char info[256];
    sprintf(info, "%s\n%s %d\n%s %d", _("Sending finished!"), _("Success:"), count_success,  _("Fail:"), count_failed);
    //	ViewDialog::GetInstance()->Destroy();
    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            info,
            NULL);
#endif*/
}

//static GCancellable* cancellable = NULL;

static void progress_callback(goffset current, goffset total, gpointer data)
{
    if(g_cancellable_is_cancelled(cancellable))
        return;

    double prac = (double)current/total;
    //	PRINTF("prac = %f\n", prac);
    if(prac >= 0 && prac <= 1.0)
    {
        gdk_threads_enter();	
        ViewDialog::GetInstance()->SetProgressBar(prac);
        while(gtk_events_pending())
            gtk_main_iteration();
        gdk_threads_leave();	
    }
    else
        PRINTF("fraction out of range!\n");
}
#if 0
static gboolean SendToFlash(gpointer data)
{
    int cond = 0;
    int count = 1;
    int total = 0;
    char str_info[256], result[256];
    vector<string> vecSid, vecName, vec_abs;
    PeripheralMan *ptr = PeripheralMan::GetInstance();
    ViewArchive *ptrArch = ViewArchive::GetInstance();

    if(!ptr->CheckUsbStorageState())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                ViewDialog::ERROR,
                _("No USB storage found!"), 
                NULL);
        return FALSE;
    }
    else
    {
        if(!ptr->MountUsbStorage())
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                    ViewDialog::ERROR,
                    _("Failed to mount USB storage!"), 
                    NULL);
            return FALSE;
        }
    }

    vecSid = ptrArch->GetSelSid();
    vecName = ptrArch->GetSelName();

    cancellable = g_cancellable_new();

    int n = 2;

    FileMan fm;
    vector<string>::iterator iteSid = vecSid.begin();
    vector<string>::iterator iteName = vecName.begin();
    while (iteSid < vecSid.end() && !cond) {
        vec_abs.clear();
        VideoMan::GetInstance()->LoadVideo(atoi((*iteSid).c_str()), STORE_PATH, &vec_abs);
        ImgMan::GetInstance()->LoadSnap(atoi((*iteSid).c_str()), STORE_PATH, &vec_abs);
        total = vec_abs.size();

        //check the dest dir, if not exist create it
        //	sprintf(str_path, "%s/%s", UDISK_PATH, (*iteSid).c_str());
        gchar *path = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, (*iteSid).c_str(), NULL);
        GFile *dir = g_file_new_for_path(path);
        g_free(path);
        GError *err_mkdir = NULL;
        if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
        {
            if(err_mkdir->code!=G_IO_ERROR_EXISTS)
            {
                PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
                sprintf(result, _("Failed to export to USB storage!\nError: Failed to create directory."));
                cond = -1;
                g_error_free(err_mkdir);
                g_object_unref(dir);
                break;
            }
        }
        g_object_unref(dir);

        //copy the file in vec_abs to the dest dir
        vector<string>::iterator ite_abs;
        for(ite_abs = vec_abs.begin(); ite_abs < vec_abs.end(); ite_abs++)
        {
            for(int i=0; i<n; i++)
            {
                gchar name[256];
                switch(i)
                {
                    case 0:	//BMP,JPG,EMP
                        sprintf(name , "%s", (*ite_abs).c_str());
                        break;
                    case 1:	//INI
                        fm.GetIniFilePath((*ite_abs).c_str(), name);
                        break;
                    default:
                        sprintf(name , "%s", (*ite_abs).c_str());
                        break;
                }

                //	sprintf(str_info, "%s %s   %d/%d\n%s", _("Exporting"), (*iteName).c_str(), count, total, _("Please wait..."));
                sprintf(str_info, "%s %s   %d/%d\n%s", _("Exporting"), name, count, total, _("Please wait..."));
                ViewDialog::GetInstance()->SetText(str_info);
                ViewDialog::GetInstance()->SetProgressBar(0);

                gchar *srcPath = g_build_filename(STORE_PATH, (*iteSid).c_str(), name, NULL);
                gchar *destPath = g_build_filename(UDISK_DATA_PATH, (*iteSid).c_str(), name, NULL);
                PRINTF("%s: destPath=%s\n", __FUNCTION__, destPath);
                GFile *src = g_file_new_for_path(srcPath);
                GFile *dest = g_file_new_for_path(destPath);
                g_free(srcPath);
                g_free(destPath);

                GError *err = NULL;
                int ret = g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, cancellable, progress_callback, NULL, &err);
                g_object_unref(src);
                g_object_unref(dest);
                if(!ret)
                {
                    PRINTF("g_file_copy error: %s\n", err->message);
                    switch(err->code)
                    {
                        case G_IO_ERROR_NO_SPACE:
                            sprintf(result, _("Failed to export to USB storage!\nError: No space left on storage."));
                            break;
                        case G_IO_ERROR_CANCELLED:
                            sprintf(result, _("Failed to export to USB storage!\nError: Operation was cancelled."));
                            break;
                        default:
                            sprintf(result, _("Failed to export to USB storage!"));
                            break;
                    }
                    cond = -1;
                    g_error_free(err);
                    break;
                }
            }
            count++;
        }
        iteSid++;
        iteName++;
        count = 1;
    }

    ptr->UmountUsbStorage();
    ViewDialog::GetInstance()->Destroy();

    if(!cond)
    {
        gchar *array = NULL;
        gchar *basePath = g_path_get_basename(UDISK_DATA_PATH);
        vector<string>::iterator Sid = vecSid.begin();
        while (Sid < vecSid.end()) 
        {
            gchar *path = g_build_path("/", basePath, (*Sid).c_str(), NULL);
            if(!array)
            {
                array = g_strdup(path);
            }
            else
            {
                gchar *ptr = g_strjoin(", ", array, path, NULL);
                g_free(array);
                array = ptr;
            }
            g_free(path);
            Sid++;
        }
        sprintf(result, "%s %s", _("Success to export to USB storage.\nPath: "), array);
        g_free(basePath);
        //	g_free(storePath);
    }
    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
            ViewDialog::INFO,
            result, 
            NULL);

    return FALSE;
}
#endif
#if 0
static int CancelCopyToFlash(gpointer data)
{
    PRINTF("Cancel copy!\n");
    g_cancellable_cancel(cancellable);
    return 0;
}
#endif
#if 0
//考虑将该函数中前半部分与MenuItemCDActivate中前半部分写成一个函数
void ViewArchive::GetSelToFlash()
{
    gboolean valid;
    gboolean value;
    gchar *sid, *name;
    GtkTreeIter iter;
    GtkTreeModel *model;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
    valid = gtk_tree_model_get_iter_first(model, &iter);

    m_vecFlashSid.clear();

    while(valid)
    {
        gtk_tree_model_get(model, &iter, COL_CHECKED, &value, -1);
        if(value)
        {
            gtk_tree_model_get(model, &iter, COL_EXAMID, &sid, COL_NAME, &name, -1);
            m_vecFlashSid.push_back(sid);
            m_vecFlashName.push_back(name);
            g_free(sid);
            g_free(name);
        }
        valid = gtk_tree_model_iter_next (model, &iter);
    }

    if(m_vecFlashSid.empty())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                ViewDialog::ERROR,
                _("Please select the wanted data from the list  to be exported!"), 
                NULL);
        return;
    }

    g_timeout_add(1000, SendToFlash, this);

    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
            ViewDialog::PRG_CANCEL,
            _("Please wait..."), 
            CancelCopyToFlash);
}
#endif

static gboolean ExportToFlashNew(gpointer data)
{
    int cond = 0;
    int count = 1;
    int total = 0;
    char str_info[256], result[256];
    vector<string> vecSid, vecName, vec_abs;
    vector<string> vecSidSrc, vecNameSrc, vec_abs_src;
    PeripheralMan *ptr = PeripheralMan::GetInstance();

    if(!ptr->CheckUsbStorageState())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                ViewDialog::ERROR,
                _("No USB storage found!"), 
                NULL);
        return FALSE;
    }
    else
    {
        if(!ptr->MountUsbStorage())
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                    ViewDialog::ERROR,
                    _("Failed to mount USB storage!"), 
                    NULL);
            return FALSE;
        }
    }
    ///>原目录
    vector<string> folder_src = ViewCD::GetInstance()->GetSelFolderSrc();
    vector<string> path_src = ViewCD::GetInstance()->GetSelPathSrc();
    ///>目标目录
    vector<string> folder = ViewCD::GetInstance()->GetSelFolder();
    vector<string> path = ViewCD::GetInstance()->GetSelPath();

    ///>目标目录和目标文件路径
    vector<string>::iterator it;	
    for(it=folder.begin(); it<folder.end(); it++)
    {
        gchar *basename = g_path_get_basename((*it).c_str());
        vecSid.push_back(basename);
        //g_print("dest folder name= %s\n", basename);
        g_free(basename);
    }
    gchar tmp[256]={0};
    for(it=path.begin(); it<path.end(); it++)
    {
        gchar *basename = g_path_get_basename((*it).c_str());
        gchar *dirname = g_path_get_dirname((*it).c_str());
        gchar *parent = g_path_get_basename(dirname);
        sprintf(tmp, "%s/%s", parent, basename);
        vecName.push_back(tmp);
        //g_print("dest file path= %s\n", tmp);
        g_free(basename);
        g_free(dirname);
        g_free(parent);
    }

    ///>原目录和原文件路径
    for(it=folder_src.begin(); it<folder_src.end(); it++)
    {
        gchar *basename = g_path_get_basename((*it).c_str());
        vecSidSrc.push_back(basename);
        //g_print("src folder name= %s\n", basename);
        g_free(basename);
    }
    for(it=path_src.begin(); it<path_src.end(); it++)
    {
        gchar *basename = g_path_get_basename((*it).c_str());
        gchar *dirname = g_path_get_dirname((*it).c_str());
        gchar *parent = g_path_get_basename(dirname);
        sprintf(tmp, "%s/%s", parent, basename);
        vecNameSrc.push_back(tmp);
        //g_print("file path= %s\n", tmp);
        g_free(basename);
        g_free(dirname);
        g_free(parent);
    }

    cancellable = g_cancellable_new();

    int n = 2;

    ///>/mnt/udisk/目录下建立目标目录
    FileMan fm;
    vector<string>::iterator iteSid = vecSid.begin();
    while (iteSid < vecSid.end() && !cond) 
    {
        gchar *path = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, (*iteSid).c_str(), NULL);
        GFile *dir = g_file_new_for_path(path);
        //g_print("makde dir path= %s\n", path);
        g_free(path);
        GError *err_mkdir = NULL;
        if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
        {
            PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
            if(err_mkdir->code!=G_IO_ERROR_EXISTS)
            {
                sprintf(result, _("Failed to export to USB storage!\nError: Failed to create directory."));
                cond = -1;
                g_error_free(err_mkdir);
                g_object_unref(dir);
                break;
            }
        }
        g_object_unref(dir);
        iteSid++;
    }

    vector<string>::iterator ite_name;
    vector<string>::iterator ite_name_src;
    total = vecName.size();
    count = 1;
    for(ite_name = vecName.begin(), ite_name_src = vecNameSrc.begin(); ite_name < vecName.end(), ite_name_src < vecNameSrc.end(); ite_name++, ite_name_src++)
    {
        for(int i=0; i<n; i++)
        {
            gchar name_dest[256];
            gchar name_src[256];
            switch(i)
            {
                case 0:	//BMP,JPG,EMP
                    sprintf(name_dest , "%s", (*ite_name).c_str());
                    sprintf(name_src , "%s", (*ite_name_src).c_str());
                    break;
                case 1:	//INI
                    fm.GetIniFilePath((*ite_name).c_str(), name_dest);
                    fm.GetIniFilePath((*ite_name_src).c_str(), name_src);
                    break;
                default:
                    sprintf(name_dest , "%s", (*ite_name).c_str());
                    sprintf(name_src , "%s", (*ite_name_src).c_str());
                    break;
            }

            sprintf(str_info, "%s %s   %d/%d\n%s", _("Exporting"), name_dest, count, total, _("Please wait..."));
            ViewDialog::GetInstance()->SetText(str_info);
            ViewDialog::GetInstance()->SetProgressBar(0);

            gchar *srcPath = g_build_filename(STORE_PATH, name_src, NULL);
            gchar *destPath = g_build_filename(UDISK_DATA_PATH, name_dest, NULL);
            GFile *src = g_file_new_for_path(srcPath);
            GFile *dest = g_file_new_for_path(destPath);
            //g_print("srcPath= %s destPath=%s \n", srcPath, destPath);
            g_free(srcPath);
            g_free(destPath);

            GError *err = NULL;
            int ret = g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, cancellable, progress_callback, NULL, &err);
            g_object_unref(src);
            g_object_unref(dest);
            if(!ret)
            {
                PRINTF("g_file_copy error: %s\n", err->message);
                switch(err->code)
                {
                    case G_IO_ERROR_NO_SPACE:
                        sprintf(result, _("Failed to export to USB storage!\nError: No space left on storage."));
                        break;
                    case G_IO_ERROR_CANCELLED:
                        sprintf(result, _("Failed to export to USB storage!\nError: Operation was cancelled."));
                        break;
                    default:
                        sprintf(result, _("Failed to export to USB storage!"));
                        break;
                }
                cond = -1;
                g_error_free(err);
                break;
            }
        }
        count++;
    }

    ptr->UmountUsbStorage();
    ViewDialog::GetInstance()->Destroy();

    if(!cond)
    {
        gchar *array = NULL;
        gchar *basepath = g_path_get_basename(UDISK_DATA_PATH);
        vector<string>::iterator sid = vecSid.begin();
        while (sid < vecSid.end()) 
        {
            gchar *path = g_build_path("/", basepath, (*sid).c_str(), NULL);
            if(!array)
            {
                array = g_strdup(path);
            }
            else
            {
                gchar *ptr = g_strjoin(", ", array, path, NULL);
                g_free(array);
                array = ptr;
            }
            g_free(path);
            sid++;
        }
        sprintf(result, "%s %s", _("Success to export to USB storage.\nPath: "), array);
        g_free(basepath);
    }
    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
            ViewDialog::INFO,
            result, 
            NULL);
   
    return false;
}

static gboolean ExportToFlash(gpointer data)
{
    int cond = 0;
    int count = 1;
    int total = 0;
    char str_info[256], result[256];
    vector<string> vecSid, vecName, vec_abs;
    PeripheralMan *ptr = PeripheralMan::GetInstance();

    if(!ptr->CheckUsbStorageState())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                ViewDialog::ERROR,
                _("No USB storage found!"), 
                NULL);
        return FALSE;
    }
    else
    {
        if(!ptr->MountUsbStorage())
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                    ViewDialog::ERROR,
                    _("Failed to mount USB storage!"), 
                    NULL);
            return FALSE;
        }
    }
    vector<string> folder = ViewCD::GetInstance()->GetSelFolderSrc();
    vector<string> path = ViewCD::GetInstance()->GetSelPathSrc();
    vector<string>::iterator it;	
    for(it=folder.begin(); it<folder.end(); it++)
    {
        gchar *basename = g_path_get_basename((*it).c_str());
        vecSid.push_back(basename);
        g_free(basename);
    }
    gchar tmp[256]={0};
    for(it=path.begin(); it<path.end(); it++)
    {
        gchar *basename = g_path_get_basename((*it).c_str());
        gchar *dirname = g_path_get_dirname((*it).c_str());
        gchar *parent = g_path_get_basename(dirname);
        sprintf(tmp, "%s/%s", parent, basename);
        vecName.push_back(tmp);
        g_free(basename);
        g_free(dirname);
        g_free(parent);
    }

    cancellable = g_cancellable_new();

    int n = 2;

    FileMan fm;
    //vecSid.sort(vecSid.begin(), vecSid.end());
    vector<string>::iterator iteSid = vecSid.begin();
    while (iteSid < vecSid.end() && !cond) 
    {
        gchar *path = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, (*iteSid).c_str(), NULL);
        GFile *dir = g_file_new_for_path(path);
        g_free(path);
        GError *err_mkdir = NULL;
        if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
        {
            PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
            if(err_mkdir->code!=G_IO_ERROR_EXISTS)
            {
                sprintf(result, _("Failed to export to USB storage!\nError: Failed to create directory."));
                cond = -1;
                g_error_free(err_mkdir);
                g_object_unref(dir);
                break;
            }
        }
        g_object_unref(dir);
        iteSid++;
    }

    vector<string>::iterator ite_name;
    //vecName.sort(vecName.begin(), vecName.end());
    total = vecName.size();//+1;
    count = 1;
    for(ite_name = vecName.begin(); ite_name < vecName.end(); ite_name++)
    {
        for(int i=0; i<n; i++)
        {
            gchar name[256];
            switch(i)
            {
                case 0:	//BMP,JPG,EMP
                    sprintf(name , "%s", (*ite_name).c_str());
                    break;
                case 1:	//INI
                    fm.GetIniFilePath((*ite_name).c_str(), name);
                    break;
                default:
                    sprintf(name , "%s", (*ite_name).c_str());
                    break;
            }

            //	sprintf(str_info, "%s %s   %d/%d\n%s", _("Exporting"), (*iteName).c_str(), count, total, _("Please wait..."));
            sprintf(str_info, "%s %s   %d/%d\n%s", _("Exporting"), name, count, total, _("Please wait..."));
            ViewDialog::GetInstance()->SetText(str_info);
            ViewDialog::GetInstance()->SetProgressBar(0);

            //gchar *srcPath = g_build_filename(STORE_PATH, (*iteSid).c_str(), name, NULL);
            //gchar *destPath = g_build_filename(UDISK_DATA_PATH, (*iteSid).c_str(), name, NULL);
            gchar *srcPath = g_build_filename(STORE_PATH, name, NULL);
            gchar *destPath = g_build_filename(UDISK_DATA_PATH, name, NULL);
            GFile *src = g_file_new_for_path(srcPath);
            GFile *dest = g_file_new_for_path(destPath);
            //g_print("srcPath= %s destPath=%s \n", srcPath, destPath);
            g_free(srcPath);
            g_free(destPath);

            GError *err = NULL;
            int ret = g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, cancellable, progress_callback, NULL, &err);
            g_object_unref(src);
            g_object_unref(dest);
            if(!ret)
            {
                PRINTF("g_file_copy error: %s\n", err->message);
                switch(err->code)
                {
                    case G_IO_ERROR_NO_SPACE:
                        sprintf(result, _("Failed to export to USB storage!\nError: No space left on storage."));
                        break;
                    case G_IO_ERROR_CANCELLED:
                        sprintf(result, _("Failed to export to USB storage!\nError: Operation was cancelled."));
                        break;
                    default:
                        sprintf(result, _("Failed to export to USB storage!"));
                        break;
                }
                cond = -1;
                g_error_free(err);
                break;
            }
        }
        count++;
    }

    ptr->UmountUsbStorage();
    ViewDialog::GetInstance()->Destroy();

    if(!cond)
    {
        gchar *array = NULL;
        gchar *basepath = g_path_get_basename(UDISK_DATA_PATH);
        vector<string>::iterator sid = vecSid.begin();
        while (sid < vecSid.end()) 
        {
            gchar *path = g_build_path("/", basepath, (*sid).c_str(), NULL);
            if(!array)
            {
                array = g_strdup(path);
            }
            else
            {
                gchar *ptr = g_strjoin(", ", array, path, NULL);
                g_free(array);
                array = ptr;
            }
            g_free(path);
            sid++;
        }
        sprintf(result, "%s %s", _("Success to export to USB storage.\nPath: "), array);
        g_free(basepath);
        //	g_free(storepath);
    }
    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
            ViewDialog::INFO,
            result, 
            NULL);
   
    return false;
}

void ViewArchive::StartUsbExport(GtkWidget *parent)
{
    //g_timeout_add(1000, ExportToFlash, this);
    g_timeout_add(1000, ExportToFlashNew, this);
    ViewDialog::GetInstance()->Create(GTK_WINDOW(parent),
            ViewDialog::PRG_CANCEL,
            _("Please wait..."), 
            CancelCopyToFlash);
}

string ViewArchive::GetExamType()
{
    string part = "";
    string partTemp = "";
    TopArea::GetInstance()->GetCheckPart(part);
    if (strcmp(part.c_str(), _("Abdomen")) == 0 || strcmp(part.c_str(), _("Adult Abdomen")) == 0 || strcmp(part.c_str(), _("Adult Liver")) == 0 || strcmp(part.c_str(), _("Kid Abdomen")) == 0 || strcmp(part.c_str(), _("Kidney Ureter")) == 0
            || strcmp(part.c_str(), _("Bladder Prostate")) == 0 || strcmp(part.c_str(), _("Abdo-adult")) == 0 || strcmp(part.c_str(), _("Abdo-liver")) == 0 || strcmp(part.c_str(), _("Abdo-kid")) == 0)
				partTemp = "ABDOMEN";
			else if (strcmp(part.c_str(), _("Cardiac")) == 0 || strcmp(part.c_str(), _("Adult Cardio")) == 0 || strcmp(part.c_str(), _("Kid Cardio")) == 0 || strcmp(part.c_str(), _("Fetus Cardio")) == 0 || strcmp(part.c_str(), _("Car-adult")) == 0 || strcmp(part.c_str(), _("Car-kid")) == 0 || strcmp(part.c_str(), _("Car-fetus")) == 0) 
				partTemp = "HEART";
			else if (strcmp(part.c_str(), _("Hip Joint")) == 0 || strcmp(part.c_str(), _("Hip-joint")) == 0)
				partTemp = "HIP";
			else if (strcmp(part.c_str(), _("Mammary Glands")) == 0 || strcmp(part.c_str(), _("Glands")) == 0)
				partTemp = "BREAST";
			else
				partTemp = "";
            return partTemp;
}

CustomType* CustomType::m_ptrInstance1 = NULL;

CustomType::CustomType()
{
    m_exportNum =0;
}

CustomType::~CustomType()
{
    if (m_ptrInstance1 != NULL)
        delete m_ptrInstance1;
}

CustomType* CustomType::GetInstance()
{
    if (m_ptrInstance1 == NULL)
        m_ptrInstance1 = new CustomType;
    return m_ptrInstance1;
}

GtkWidget* CustomType::CreateWin(GtkWidget *parent)
{
    m_ptrInstance1 = this;
    GtkWidget *window_type;
    GtkWidget *fixed1;
    GtkWidget *label_type;
    GtkWidget *vbox_type;
    GSList *radiobutton_data_type_group = NULL;

    GtkWidget *label_ok;
    GtkWidget *image_ok;
    GtkWidget *btnOk;
    
    GtkWidget *label_cancel;
    GtkWidget *image_cancel;
    GtkWidget *btnCancel;
   
    window_type = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (window_type, 400-20, 280-40);
    gtk_window_set_title (GTK_WINDOW (window_type), _("Export Setting"));
    gtk_window_set_transient_for(GTK_WINDOW(window_type), GTK_WINDOW(parent));
    gtk_window_set_modal(GTK_WINDOW(window_type), TRUE);
    gtk_window_set_position (GTK_WINDOW (window_type), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW (window_type), FALSE);

    fixed1 = gtk_fixed_new ();
    gtk_widget_show (fixed1);
    gtk_container_add (GTK_CONTAINER (window_type), fixed1);

    label_type = gtk_label_new (_("Please select export data type"));
    gtk_widget_show (label_type);
    gtk_fixed_put (GTK_FIXED (fixed1), label_type, 10, 15);
    gtk_widget_set_size_request (label_type, 300, 40);

    label_type = gtk_label_new (_("Data Format:"));
    gtk_widget_show (label_type);
    gtk_fixed_put (GTK_FIXED (fixed1), label_type, 20, 40+20);
    gtk_widget_set_size_request (label_type, 120, 30);

    vbox_type = gtk_vbox_new (TRUE, 0);
    gtk_widget_show (vbox_type);
    gtk_fixed_put (GTK_FIXED (fixed1), vbox_type, 150, 40+20);

#ifdef EMP_3410
    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[0]))
#else
        if(CDCMRegister::GetMe()->IsAuthorize())
#endif
    {
        m_radiobutton_dicom = gtk_radio_button_new_with_mnemonic (NULL, _("DICOM"));
        gtk_widget_show (m_radiobutton_dicom);
        gtk_box_pack_start(GTK_BOX (vbox_type), m_radiobutton_dicom, FALSE, FALSE, 4);
        gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_dicom), radiobutton_data_type_group);
        radiobutton_data_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_dicom));
        g_signal_connect((gpointer)m_radiobutton_dicom, "toggled", G_CALLBACK (on_dicom_radio_button_toggled), this);
    }
 
    m_radiobutton_emp = gtk_radio_button_new_with_mnemonic (NULL, _("JPEG/BMP/EMP/AVI/CINE"));
    gtk_widget_show (m_radiobutton_emp);
    gtk_box_pack_start (GTK_BOX (vbox_type), m_radiobutton_emp, FALSE, FALSE, 5);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_emp), radiobutton_data_type_group);
    radiobutton_data_type_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_emp));
    g_signal_connect((gpointer)m_radiobutton_emp, "toggled", G_CALLBACK (on_emp_radio_button_toggled), this);

	label_ok = gtk_label_new_with_mnemonic (_("OK"));
    image_ok = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
	btnOk = create_button_icon(label_ok, image_ok);
	gtk_fixed_put (GTK_FIXED (fixed1), btnOk, 40, 180 - 40);
	g_signal_connect(G_OBJECT(btnOk), "clicked", G_CALLBACK(HandleBtnOkClicked), this);
    
    label_cancel = gtk_label_new_with_mnemonic (_("Cancel"));
    image_cancel = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
	btnCancel = create_button_icon(label_cancel, image_cancel);
	gtk_fixed_put (GTK_FIXED (fixed1), btnCancel, 220, 180 - 40);
	g_signal_connect(G_OBJECT(btnCancel), "clicked", G_CALLBACK(HandleBtnCancelClicked), this);

    gtk_widget_show_all(window_type);
    
    m_window = window_type;
    g_signal_connect(G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this); 
    g_keyInterface.Push(this);
    SetSystemCursorToCenter();

    return window_type;
}

void CustomType::DicomRadioToggled(GtkToggleButton *togglebutton)
{

}

void CustomType::EmpRadioToggled(GtkToggleButton *togglebutton)
{

}

gboolean ExportStudyForDicom(gpointer data)
{
	CustomType *tmp;
	tmp = (CustomType *)data;
	tmp->ExportStudy();
	return FALSE;
}

void CustomType::ExportStudy(void)
{
#if 1
    PeripheralMan *ptr = PeripheralMan::GetInstance();

    if(!ptr->CheckUsbStorageState())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                ViewDialog::ERROR,
                _("No USB storage found!"), 
                NULL);
        return ;
    }
    else
    {
        if(!ptr->MountUsbStorage())
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                    ViewDialog::ERROR,
                    _("Failed to mount USB storage!"), 
                    NULL);
            return ;
        }
    }
#endif
    Replay::GetInstance()->ClearCurReplayData();
    Replay::GetInstance()->DisplayReplayBar();

    vector<string> vec = ViewArchive::GetInstance()->GetSelExamID();
    int size = vec.size();
    int count_success =0;
    int count_fail = 0;

    if(size ==1)
    {
        int studyNo = atoi(vec[0].c_str());
        char strSrcDir[256];
        sprintf(strSrcDir,"%s/%s",STORE_PATH,vec[0].c_str());
        string destDirStorageMedia = UDISK_PATH;
        EDCMReturnStatus status = CDCMMan::GetMe()->ExportStudy(studyNo,strSrcDir,destDirStorageMedia,PROGRESSSTATUS);
        //EDCMReturnStatus status = CDCMMan::GetMe()->ExportStudy(studyNo,strSrcDir,destDirStorageMedia,NULL);
        if(status == DCMSUCCESS)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                    ViewDialog::INFO, 
                    _("Export data successfully!"),
                    NULL);
        }
        else if(status == DCMSTUDYEXISTED)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                    ViewDialog::INFO, 
                    _("Study Info has existed!"),
                    NULL);
        }
        else if (status == DCMINVALIDSTORAGEDEVICE)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                    ViewDialog::INFO, 
                    _("Invalid storage device!"),
                    NULL);
        }
        else if (status == DCMEXPORTFAILURE)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                    ViewDialog::INFO, 
                    _("Export data failure!"),
                    NULL);
        }
        else if(status == DCMNOENOUGHSPACE)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                    ViewDialog::INFO, 
                    _("No enough space!"),
                    NULL);
        }
        else
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                    ViewDialog::INFO, 
                    _("Export data failure!"),
                    NULL);
        }
    }
    else if (size>1)
    {
        int count_success =0;
        int count_failed = 0;
        for(int i=0;i<size;i++)
        {
            m_exportNum = i;
            int studyNo = atoi(vec[i].c_str());
            char strSrcDir[256];
            sprintf(strSrcDir,"%s/%s",STORE_PATH,vec[i].c_str());
            string destDirStorageMedia = UDISK_PATH;
            EDCMReturnStatus status = CDCMMan::GetMe()->ExportStudy(studyNo,strSrcDir,destDirStorageMedia,PROGRESSSTATUS);
            //ViewDialog::GetInstance()->Destroy();
            if(status == DCMSUCCESS)
            {
                count_success++;
                
            }
            else
            {
                count_failed++;
            }
        }
        char info[256];
        sprintf(info, "%s\n%s %d\n%s %d", _("Exporting finished!"), _("Success:"), count_success,  _("Fail:"), count_failed);

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                info,
                NULL);
    }
    ptr->UmountUsbStorage();

}

void CustomType::PROGRESSSTATUS(int nPos)
{
    vector<string> vecPatExamID = ViewArchive::GetInstance()->GetSelExamID();
    int size = vecPatExamID.size();
    double frac;
    if(size>=2)
    {
        float x1 =m_ptrInstance1->m_exportNum/(float)size;
        float x2 = 1/(float)size;
        frac = x1 + nPos*0.01*x2;
    }
    else
        frac = nPos*0.01;
	gdk_threads_enter();
    ViewDialog::GetInstance()->SetProgressBar(frac);
    while(gtk_events_pending())
        gtk_main_iteration();
	gdk_threads_leave();

}

void CustomType::BtnOkClicked(GtkButton *button)
{
#ifdef EMP_3410
    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[0]))
#else
        if(CDCMRegister::GetMe()->IsAuthorize())
#endif
    {

        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_dicom)))
        {
            DestroyWin();
            g_timeout_add(100, ExportStudyForDicom, this);
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewArchive::GetInstance()->GetWindow()),
                    ViewDialog::PROGRESS,
                    _("Please wait..."), 
                    NULL);
        }
        else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_emp)))
        {
            DestroyWin();
            //ViewCD::GetInstance()->CreateWindow(ViewArchive::GetInstance()->GetWindow(), ViewArchive::GetInstance()->GetItemFlash(), false);
            ViewCD::GetInstance()->CreateWindow(ViewArchive::GetInstance()->GetWindow(), ViewArchive::GetInstance()->GetCheckId(), false);
        }
    }
        else
        {
            if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_emp)))
            {
                DestroyWin();
                //ViewCD::GetInstance()->CreateWindow(ViewArchive::GetInstance()->GetWindow(), ViewArchive::GetInstance()->GetItemFlash(), false);
                ViewCD::GetInstance()->CreateWindow(ViewArchive::GetInstance()->GetWindow(), ViewArchive::GetInstance()->GetCheckId(), false);
            }
        }
}

void CustomType::BtnCancelClicked(GtkButton *button)
{
    DestroyWin();
}

void CustomType::DestroyWin(void)
{
    if(GTK_IS_WIDGET(m_window))
    {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
}

gboolean CustomType::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWin();
    return FALSE;
}

static gboolean DestroyWindow(gpointer data)
{
    CustomType *tmp;
    tmp = (CustomType *)data;
    tmp->DestroyWin();
    return FALSE;
}

void CustomType::KeyEvent(unsigned char keyValue)
{
	FakeXEvent::KeyEvent(keyValue);

	switch(keyValue)
	{
		case KEY_ESC:
		    g_timeout_add(100, DestroyWindow, this);
		    FakeEscKey();
		    break;

		default:
			break;
	}
}

GtkWidget* ViewArchive::CreatePreview(void)
{
	GtkWidget *frame = gtk_frame_new (NULL);
	gtk_widget_set_size_request (frame, 834, 130);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);

	GtkWidget *fixed = gtk_fixed_new ();
	gtk_container_add (GTK_CONTAINER (frame), fixed);

	GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_set_size_request (sw, 820, 120);
	gtk_fixed_put (GTK_FIXED (fixed), sw, 5, 5);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_ALWAYS, GTK_POLICY_NEVER);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw), GTK_SHADOW_IN);

	m_viewportPreview = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (m_viewportPreview);
	gtk_container_add (GTK_CONTAINER (sw), m_viewportPreview);

	m_tablePreview = gtk_table_new (1, 1, TRUE);
	gtk_widget_show (m_tablePreview);
	gtk_container_add (GTK_CONTAINER(m_viewportPreview), m_tablePreview);

	return frame;
}

void ViewArchive::ShowPreview(void)
{
	gtk_widget_set_size_request (m_treeview, 834, 400);
	gtk_widget_show_all (m_framePreview);
}

void ViewArchive::HidePreview(void)
{
	gtk_widget_set_size_request (m_treeview, 834, 528);
	gtk_widget_hide_all (m_framePreview);
}

void ViewArchive::TreeSelectionChanged(GtkTreeSelection *selection)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *sid;

    if(gtk_tree_selection_get_selected(selection, &model, &iter))
    {
		gtk_tree_model_get(model, &iter, COL_EXAMID, &sid, -1);
        GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
        gtk_tree_path_free (path);

		//Show Preview
		LoadPreview(atoi(sid));

        g_free(sid);
    }
	else
	{
		HidePreview();
	}
}

void ViewArchive::ClearPreview()
{
	int i, total; 
	m_vecImage.clear();
	total = m_vecItem.size();
	for(i=0; i<total; i++)
		gtk_widget_destroy(m_vecItem[i]);
	m_vecItem.clear();
}

void ViewArchive::LoadPreview(int sid)
{
	//clear previous
	ClearPreview();

	//load new
	ImgMan::GetInstance()->LoadSnap(sid, STORE_PATH, &m_vecImage);
	VideoMan::GetInstance()->LoadVideo(sid, STORE_PATH, &m_vecImage);
	sort(m_vecImage.begin(), m_vecImage.end(), Sort);
	//printf("Sid=%d, size=%d\n", sid, m_vecImage.size());

	//
	GtkWidget *item;
	int total = m_vecImage.size();
	if(total > 0)
	{
		gtk_table_resize(GTK_TABLE(m_tablePreview), 1, total);
		for (int i=0; i<total; i++)
		{
			item = CreateImageItem(sid, i);
			if(item)
			{
				gtk_table_attach(GTK_TABLE(m_tablePreview), item, i, 1+i, 0, 2, (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 5, 5);
				m_vecItem.push_back(item);
			}
		}
		ShowPreview();
	}
	else
	{
		HidePreview();
	}

}

bool ViewArchive::Sort(const string s1, const string s2)
{
	int i;
	int len;
	char str1[255], str2[255];

	len = strlen(s1.c_str()); 
	for(i=0; i<len; i++)
	{
		if(s1.c_str()[i]=='.')
			break;
	}
	strncpy(str1, s1.c_str(), i);
	str1[i] = '\0';

	len = strlen(s2.c_str()); 
	for(i=0; i<len; i++)
	{
		if(s2.c_str()[i]=='.')
			break;
	}
	strncpy(str2, s2.c_str(), i);
	str2[i] = '\0';

	return atof(str1) > atof(str2);
//	return atoi(s1.c_str()) < atoi(s2.c_str());int i;
}

GtkWidget* ViewArchive::CreateImageItem(int sid, int id)
{
	GtkWidget *image;
	GtkWidget *button;
	GdkPixbuf *pixbuf = NULL;

	string ext = m_vecImage[id].substr(m_vecImage[id].rfind('.') == string::npos ? m_vecImage[id].length() : m_vecImage[id].rfind('.') + 1);
	//printf("Image %d: %s\n", id, m_vecImage[id].c_str());
	//printf("ext : %s\n", ext.c_str());

	if(ext == "avi" || ext == "cine")
	{
		VideoMan::VideoItem video_item;
		if(VideoMan::GetInstance()->ReadVideoInfo(sid, m_vecImage[id].c_str(), STORE_PATH, &video_item) != 0)
		{
			PRINTF("%s: ReadVideoInfo Error!\n", __FUNCTION__);
			return NULL;
		}
		GdkPixbuf *pb = gdk_pixbuf_new_from_data(video_item.data, GDK_COLORSPACE_RGB, 
				false, 8, video_item.width, video_item.height, video_item.width*3, NULL, NULL);
		pixbuf = gdk_pixbuf_scale_simple(pb, SNAP_W, SNAP_H, GDK_INTERP_BILINEAR);
		GdkPixmap *pixmap = gdk_pixmap_new(NULL, SNAP_W, SNAP_H, 24);
		GdkGC *gc = gdk_gc_new(pixmap);
		gdk_draw_pixbuf(pixmap, gc, pixbuf, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
		GdkPixbuf *film	= gdk_pixbuf_new_from_file("res/film.png", NULL);
		int w = gdk_pixbuf_get_width(film);
		gdk_draw_pixbuf(pixmap, gc, film, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);
		gdk_draw_pixbuf(pixmap, gc, film, 0, 0, SNAP_W-w, 0, -1, -1, GDK_RGB_DITHER_NONE, 0, 0);

		g_object_unref(pb);
		g_object_unref(pixbuf);
		g_object_unref(film);
		g_free(video_item.data);

		image = gtk_image_new();
		gtk_image_set_from_pixmap(GTK_IMAGE(image), pixmap, NULL);
		g_object_unref(pixmap);
	}
	else
	{
		ImgMan::ImgItem img_item;
		if(ImgMan::GetInstance()->ReadSnap(sid, m_vecImage[id].c_str(), STORE_PATH, &img_item) != 0)
		{
			PRINTF("%s: ReadSnap %s Error!\n", __FUNCTION__, m_vecName[i].c_str());
			return NULL;
		}
		pixbuf = gdk_pixbuf_scale_simple(img_item.pixbuf, SNAP_W, SNAP_H, GDK_INTERP_BILINEAR);
		g_object_unref(img_item.pixbuf);

		image = gtk_image_new();
		gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
		g_object_unref(pixbuf);
	}

	button = gtk_button_new();
	g_object_set_data(G_OBJECT(button), "sid", GINT_TO_POINTER(sid));
	g_object_set_data(G_OBJECT(button), "filename", (void*)(m_vecImage[id].c_str()));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(HandleBtnPreviewClicked), this);
	gtk_button_set_image(GTK_BUTTON(button), image);
	gtk_widget_show(button);

	return button;
}

void ViewArchive::BtnPreviewClicked(GtkButton *button)
{
//	BtnDisplayClicked(NULL);
	vector<string> vec;
	char buf[100];
	string str;
	int sid = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "sid"));
	sprintf(buf, "%d", sid);
	str = buf;
	vec.push_back(str);
	char *filename = (char*)(g_object_get_data(G_OBJECT(button), "filename"));

	ViewArchiveImgMan::GetInstance()->CreateWindow(vec, filename);
}

