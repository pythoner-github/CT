#include "ViewWorkList.h"
#include "gui_global.h"
#include "gui_func.h"
#include "KeyValueOpr.h"
#include "ViewMain.h"
#include "DCMMan.h"
#include "SysGeneralSetting.h"
#include "SysDicomSetting.h"
#include "ViewCalendar.h"
#include "ViewNewPat.h"
#include <time.h>
#include "KeyFunc.h"
#include "PatientInfo.h"
#include "ViewDialog.h"
#include "Database.h"
#include "TopArea.h"
#include "MenuCalcNew.h"
#include "ViewMain.h"
#include "ViewReport.h"
#include "HintArea.h"
#include "KeyDef.h"
#include "ViewSystem.h"
#include "UpgradeMan.h"
#include "DCMMan.h"
#include "TopArea.h"

using std::vector;

ViewWorkList* ViewWorkList::m_ptrInstance = NULL;

ViewWorkList::ViewWorkList()
{
    SysGeneralSetting sys;
    m_dateFormat = sys.GetDateFormat();
    
       
    m_timeout = 0; 
    selectedIndex = 0;

}

ViewWorkList::~ViewWorkList()
{

}
#if 1
ViewWorkList* ViewWorkList::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ViewWorkList;

    return m_ptrInstance;
}
#endif
#ifndef VET
GtkWidget* ViewWorkList::CreateWorkListWin(GtkWidget *parent)
{
    GtkWidget *window_worklist;
    GtkWidget *fixed_window;
    GtkWidget *frame_query;
    GtkWidget *label_query;
    GtkWidget *fixed;
    GtkWidget *label_ID;
    GtkWidget *label_name;
    GtkWidget *label_accession;
    GtkWidget *label_request;
    GtkWidget *label_date;
    GtkWidget *label_to; 
    GtkWidget *button_query; 
    GtkWidget *button_clear;
    GtkWidget *image_new_exam;
    GtkWidget *label_new_exam;
    GtkWidget *button_new_exam;
    
    GtkWidget *image_detail;
    GtkWidget *label_detail;
    GtkWidget *button_detail;

    GtkWidget *image_quit;
    GtkWidget *label_quit;
    GtkWidget *button_quit;
    
    GtkWidget *image_transfer;
    GtkWidget *label_transfer;
    GtkWidget *button_transfer;

    GtkWidget *img_start_calendar;
    GtkWidget *eventbox_start_calendar;
    GtkWidget *img_end_calendar;
    GtkWidget *eventbox_end_calendar;


    window_worklist = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (window_worklist, 840, 640);
    gtk_window_set_title (GTK_WINDOW (window_worklist), _("Worklist"));
    gtk_window_set_transient_for(GTK_WINDOW(window_worklist), GTK_WINDOW(parent));
    gtk_window_set_modal(GTK_WINDOW(window_worklist), TRUE);
    gtk_window_set_position (GTK_WINDOW (window_worklist), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW (window_worklist), FALSE);
    g_signal_connect(window_worklist,"delete-event",G_CALLBACK(HandleCloseWindow),NULL);

    fixed_window = gtk_fixed_new ();
    gtk_widget_show (fixed_window);
    gtk_container_add (GTK_CONTAINER (window_worklist), fixed_window);

    frame_query = gtk_frame_new (NULL);
    gtk_widget_show (frame_query);
    gtk_fixed_put (GTK_FIXED (fixed_window), frame_query, 20, 10);
    gtk_widget_set_size_request (frame_query, 800, 140);
   // gtk_frame_set_label_align (GTK_FRAME (frame_query), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_query), GTK_SHADOW_IN);

    label_query = gtk_label_new (_("Query"));
    gtk_widget_show (label_query);
    gtk_frame_set_label_widget (GTK_FRAME (frame_query), label_query);
    gtk_label_set_use_markup (GTK_LABEL (label_query), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame_query), fixed);

#ifdef VET
	 label_ID = gtk_label_new (_("ID :"));
#else 
    label_ID = gtk_label_new (_("Patient ID :"));
#endif    
	gtk_widget_show (label_ID);
    gtk_fixed_put (GTK_FIXED (fixed), label_ID, 10-10, 8);
    gtk_label_set_use_markup (GTK_LABEL (label_ID), TRUE);
#ifdef VET
   gtk_widget_set_size_request (label_ID, 45, 30);
#else
    gtk_widget_set_size_request (label_ID, 100, 30);
#endif
    gtk_misc_set_alignment (GTK_MISC (label_ID), 0.9, 0.5);

    m_entry_ID = gtk_entry_new ();
    gtk_widget_show (m_entry_ID);
#ifdef VET
 	gtk_fixed_put (GTK_FIXED (fixed), m_entry_ID, 65-10, 8);
#else
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_ID, 110+8-10, 8);
#endif
    gtk_widget_set_size_request (m_entry_ID, 200-100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_ID), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ID), 9679);

#ifdef VET
    label_name = gtk_label_new (_("Animal Name :"));
#else
    label_name = gtk_label_new (_("Patient Name :"));
#endif
    gtk_widget_show (label_name);
#ifdef VET
   gtk_fixed_put (GTK_FIXED (fixed), label_name, 328-18+5-130, 8);
#else
    gtk_fixed_put (GTK_FIXED (fixed), label_name, 328+30-130, 8);
#endif
    gtk_label_set_use_markup (GTK_LABEL (label_name), TRUE);
#ifdef VET
    gtk_widget_set_size_request (label_name, 140, 30);
#else
    gtk_widget_set_size_request (label_name, 100, 30);
#endif
    gtk_misc_set_alignment (GTK_MISC (label_name), 0.9, 0.5);

    m_entry_name = gtk_entry_new ();
    gtk_widget_show (m_entry_name);
#ifdef VET
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_name, 448+15-130, 8);
#else
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_name, 428+30-130, 8);
#endif
    gtk_widget_set_size_request (m_entry_name, 200, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_name), 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_name), 9679);

    label_accession = gtk_label_new (_("Accession Number :"));
    gtk_widget_show (label_accession);
    gtk_fixed_put (GTK_FIXED (fixed), label_accession, 558-10-45, 8);
    gtk_label_set_use_markup (GTK_LABEL (label_accession), TRUE);
    gtk_widget_set_size_request (label_accession, 130+20+15, 30);
    gtk_misc_set_alignment (GTK_MISC (label_accession), 0.9, 0.5);

    m_entry_accession = gtk_entry_new ();
    gtk_widget_show (m_entry_accession);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_accession, 708-30+10-20, 8);
    gtk_widget_set_size_request (m_entry_accession, 200-100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_accession), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_accession), 9679);

    label_request = gtk_label_new (_("Requested Procedure ID :"));
    gtk_widget_show (label_request);
    gtk_fixed_put (GTK_FIXED (fixed), label_request, 0, 45);
    gtk_label_set_use_markup (GTK_LABEL (label_request), TRUE);
    gtk_widget_set_size_request (label_request, 180+10, 30);
    gtk_misc_set_alignment (GTK_MISC (label_request), 0.9, 0.5);

    m_entry_request = gtk_entry_new ();
    gtk_widget_show (m_entry_request);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_request,180+10, 45);
#ifdef VET
   gtk_widget_set_size_request (m_entry_request, 200+50-100, 30);
#else
    gtk_widget_set_size_request (m_entry_request, 200-100, 30);
  #endif 
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_request), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_request), 9679);

    label_date = gtk_label_new (_("Scheduled Date :"));
    gtk_widget_show (label_date);
    gtk_fixed_put (GTK_FIXED (fixed), label_date, 10-10, 82);
    gtk_label_set_use_markup (GTK_LABEL (label_date), TRUE);
//#ifdef VET
  	gtk_widget_set_size_request (label_date, 130+40, 30);
//#else
  //  gtk_widget_set_size_request (label_date, 130, 30);
//#endif  
  	gtk_misc_set_alignment (GTK_MISC (label_date), 0.9, 0.5);

    m_entry_date1 = gtk_entry_new ();
    gtk_widget_show (m_entry_date1);
//#ifdef VET
	gtk_fixed_put (GTK_FIXED (fixed), m_entry_date1, 140+30+10, 82);
//#else
  //  gtk_fixed_put (GTK_FIXED (fixed), m_entry_date1, 140-10, 82);
//#endif
    gtk_widget_set_size_request (m_entry_date1, 100-20, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_date1), 10);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_date1), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_date1),false);
    //g_signal_connect(G_OBJECT(m_entry_date1), "insert_text", G_CALLBACK(on_entry_insert_start_date), this);
   // g_signal_connect(G_OBJECT(m_entry_date2), "delete_text", G_CALLBACK(on_entry_delete_start_date), this);
    
    img_start_calendar = gtk_image_new_from_file ("./res/icon/Calendar.png");
    gtk_widget_show (img_start_calendar);
    eventbox_start_calendar = gtk_event_box_new ();
    gtk_widget_show (eventbox_start_calendar);
    gtk_widget_set_size_request (eventbox_start_calendar, 40, 30);
    gtk_container_add (GTK_CONTAINER (eventbox_start_calendar), img_start_calendar);
//#ifdef VET
 	gtk_fixed_put (GTK_FIXED (fixed), eventbox_start_calendar, 230+30+10, 82);
//#else
  //  gtk_fixed_put (GTK_FIXED (fixed), eventbox_start_calendar, 230-10, 82);
//#endif   
 	g_signal_connect (G_OBJECT(eventbox_start_calendar), "button_press_event", G_CALLBACK(HandleStartCalendarPress), this);
    
    label_to = gtk_label_new (_("To :"));
    gtk_widget_show (label_to);
//#ifdef VET
   gtk_fixed_put (GTK_FIXED (fixed), label_to, 230-50+100, 82);
//#else
  //  gtk_fixed_put (GTK_FIXED (fixed), label_to, 230-10-10, 82);
//#endif
    gtk_label_set_use_markup (GTK_LABEL (label_to), TRUE);
    gtk_widget_set_size_request (label_to, 100-20, 30);
    gtk_misc_set_alignment (GTK_MISC (label_to), 0.9, 0.5);

    m_entry_date2 = gtk_entry_new ();
    gtk_widget_show (m_entry_date2);
//#ifdef VET
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_date2, 300+50, 82);
//#else
  //  gtk_fixed_put (GTK_FIXED (fixed), m_entry_date2, 300-10, 82);
//#endif
    gtk_widget_set_size_request (m_entry_date2, 100-20, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_date2), 10);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_date2), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_date2),false);

    img_end_calendar = gtk_image_new_from_file ("./res/icon/Calendar.png");
    gtk_widget_show (img_end_calendar);
    eventbox_end_calendar = gtk_event_box_new ();
    gtk_widget_show (eventbox_end_calendar);
    gtk_widget_set_size_request (eventbox_end_calendar, 40-10, 30);
    gtk_container_add (GTK_CONTAINER (eventbox_end_calendar), img_end_calendar);
//#ifdef VET
   gtk_fixed_put (GTK_FIXED (fixed), eventbox_end_calendar, 390+50, 82);
//#else
 //   gtk_fixed_put (GTK_FIXED (fixed), eventbox_end_calendar, 390, 82);
//#endif
    g_signal_connect (G_OBJECT(eventbox_end_calendar), "button_press_event", G_CALLBACK(HandleEndCalendarPress), this);

    InitScheduledDate();

    button_query = gtk_button_new_with_mnemonic (_("Query"));
    gtk_widget_show (button_query);
//#ifdef VET
 	gtk_fixed_put (GTK_FIXED (fixed), button_query, 500+58, 82);
//#else
  //  gtk_fixed_put (GTK_FIXED (fixed), button_query, 500, 82);
//#endif
    gtk_widget_set_size_request (button_query, 85, 35);
    g_signal_connect(button_query, "clicked", G_CALLBACK(HandleButtonQueryClicked), this);

    button_clear = gtk_button_new_with_mnemonic (_("Clear"));
    gtk_widget_show (button_clear);
//#ifdef VET
     gtk_fixed_put (GTK_FIXED (fixed), button_clear, 635+58, 82);
//#else
  //  gtk_fixed_put (GTK_FIXED (fixed), button_clear, 630, 82);
//#endif
    gtk_widget_set_size_request (button_clear, 85, 35);
    g_signal_connect(button_clear, "clicked", G_CALLBACK(HandleButtonClearClicked), this);

    m_label_record = gtk_label_new (_("0 record is found"));
    gtk_widget_show (m_label_record);
    gtk_fixed_put (GTK_FIXED (fixed), m_label_record, 10, 120+6);
    gtk_label_set_use_markup (GTK_LABEL (m_label_record), TRUE);
    gtk_widget_set_size_request (m_label_record, 200, 33);

    GtkWidget *scrollWin = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show (scrollWin);
    gtk_fixed_put (GTK_FIXED (fixed), scrollWin, 0, 150+10);
    gtk_widget_set_size_request (scrollWin, 780+20, 280+90);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollWin), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrollWin), GTK_SHADOW_IN);

    m_treeview_worklist = create_worklist_treeview();
	gtk_container_add(GTK_CONTAINER(scrollWin), m_treeview_worklist);
	gtk_widget_set_size_request (m_treeview_worklist, 790, 260+100);
    gtk_widget_show (m_treeview_worklist);

    SysDicomSetting sysDicomSetting;
    m_autoQueryFlag = sysDicomSetting.GetAutoQuery();

    AutoQuery();

    image_new_exam = gtk_image_new_from_stock ("gtk-new", GTK_ICON_SIZE_BUTTON);
    label_new_exam = gtk_label_new_with_mnemonic (_("Start Exam"));
    button_new_exam = create_button_icon(label_new_exam, image_new_exam);
    gtk_widget_show (button_new_exam);
    gtk_fixed_put (GTK_FIXED (fixed), button_new_exam, 50-10, 530+10);
    gtk_widget_set_size_request (button_new_exam, 85+20, 35+20);
    g_signal_connect(button_new_exam, "clicked", G_CALLBACK(HandleButtonNewExamClicked), this);

    image_transfer = gtk_image_new_from_stock ("gtk-convert", GTK_ICON_SIZE_BUTTON);
    label_transfer = gtk_label_new_with_mnemonic (_("Transfer"));
    button_transfer = create_button_icon(label_transfer, image_transfer);
    gtk_widget_show (button_transfer);
    gtk_fixed_put (GTK_FIXED (fixed), button_transfer, 180, 530+10);
    gtk_widget_set_size_request (button_transfer, 85+30, 35+20);
    g_signal_connect(button_transfer, "clicked", G_CALLBACK(HandleButtonTransferClicked), this);

    image_detail = gtk_image_new_from_stock ("gtk-info", GTK_ICON_SIZE_BUTTON);
    label_detail = gtk_label_new_with_mnemonic (_("Detail"));
    button_detail = create_button_icon(label_detail, image_detail);
    gtk_widget_show (button_detail);
    gtk_fixed_put (GTK_FIXED (fixed), button_detail, 320+10, 530+10);
    gtk_widget_set_size_request (button_detail, 85+30, 35+20);
    g_signal_connect(button_detail, "clicked", G_CALLBACK(HandleButtonDetailClicked), this);

    image_quit = gtk_image_new_from_stock ("gtk-quit", GTK_ICON_SIZE_BUTTON);
    label_quit = gtk_label_new_with_mnemonic (_("Exit"));
    button_quit = create_button_icon(label_quit, image_quit);
    gtk_fixed_put (GTK_FIXED (fixed), button_quit, 630, 530+10);
    gtk_widget_set_size_request (button_quit, 85+30, 35+20);
    g_signal_connect(button_quit, "clicked", G_CALLBACK(HandleButtonQuitClicked), this);
    m_fixed =fixed;
    
    gtk_widget_show_all(window_worklist);

    m_window = window_worklist;
    g_keyInterface.Push(this);
    SetSystemCursorToCenter();

    return window_worklist;
}
#endif
GtkWidget* ViewWorkList::create_worklist_treeview()
{
    GtkWidget *treeview;
    GtkTreeModel *model = NULL;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    treeview = gtk_tree_view_new ();
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Scheduled Date"), renderer, "text", COL_SCHEDULED_DATE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
#ifdef VET
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 170, NULL);
#else
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);
#endif
    renderer = gtk_cell_renderer_text_new();
#ifdef VET
    column = gtk_tree_view_column_new_with_attributes(_("ID"), renderer, "text", COL_PATIENT_ID, NULL);
#else
    column = gtk_tree_view_column_new_with_attributes(_("patient ID"), renderer, "text", COL_PATIENT_ID, NULL);
#endif
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Name"), renderer, "text", COL_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 80, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Gender"), renderer, "text", COL_Gender, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 80, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Accession Number"), renderer, "text", COL_ACCESSION_NUMBER, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 150, NULL);
    
    renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes(_("Birth Date"), renderer, "text", COL_BIRTH_DATE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);

    
    renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes(_("Exam Description"), renderer, "text", COL_EXAM_DESCRIPTION, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 180, NULL);

    model = create_worklist_model();
    if (model != NULL)
		gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    g_object_unref (model); 

	return treeview;
}

GtkTreeModel* ViewWorkList::create_worklist_model()
{
    GtkListStore *store;

    store = gtk_list_store_new(NUM_COLS, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING);

	return GTK_TREE_MODEL (store);
}

void ViewWorkList::InitScheduledDate()
{
    time_t now;
    struct tm *now_tm;
    char buf[50];

    time(&now);
    now_tm = localtime(&now);

    switch (m_dateFormat) {
        case 0:
            sprintf(buf, "%04d/%02d/%02d", now_tm->tm_year+1900, now_tm->tm_mon+1, now_tm->tm_mday);
            break;
        case 1:
            sprintf(buf, "%02d/%02d/%04d", now_tm->tm_mon+1, now_tm->tm_mday, now_tm->tm_year+1900);
            break;
        case 2:
            sprintf(buf, "%02d/%02d/%04d", now_tm->tm_mon+1, now_tm->tm_mday, now_tm->tm_year+1900);
            break;
        default:
            sprintf(buf, "%04d/%02d/%02d", now_tm->tm_year+1900, now_tm->tm_mon+1, now_tm->tm_mday);
            break;
    }
            sprintf(startDate, "%04d%02d%02d", now_tm->tm_year+1900, now_tm->tm_mon+1, now_tm->tm_mday);
            sprintf(endDate, "%04d%02d%02d", now_tm->tm_year+1900, now_tm->tm_mon+1, now_tm->tm_mday);
            gtk_entry_set_text(GTK_ENTRY(m_entry_date1), buf);
            gtk_entry_set_text(GTK_ENTRY(m_entry_date2), buf);

}

void ViewWorkList::StartCalendarPress(GtkWidget *widget, GdkEventButton *event)
{
	ViewCalendar::GetInstance()->CreateWindow(m_window, ViewCalendar::START, this);
}

void ViewWorkList::SetStartDate(int year, int month, int day)
{
    char buf[256];
    switch(m_dateFormat)
    {
        case 0:
            sprintf(buf,"%04d/%02d/%02d",year,month,day);
            break;
        case 1:
            sprintf(buf,"%02d/%02d/%04d",month,day,year);
            break;
        case 2:
            sprintf(buf,"%02d/%02d/%04d",day,month,year);
            break;
        default:
            sprintf(buf,"%04d/%02d/%02d",year,month,day);
            break;
    }
    sprintf(startDate,"%04d%02d%02d",year,month,day);
    gtk_entry_set_text(GTK_ENTRY(m_entry_date1),buf);

}

void ViewWorkList::EndCalendarPress(GtkWidget *widget, GdkEventButton *event)
{
	ViewCalendar::GetInstance()->CreateWindow(m_window, ViewCalendar::END, this);
}

void ViewWorkList::SetEndDate(int year, int month, int day)
{
    char buf[256];
    switch(m_dateFormat)
    {
        case 0:
            sprintf(buf,"%04d/%02d/%02d",year,month,day);
            break;
        case 1:
            sprintf(buf,"%02d/%02d/%04d",month,day,year);
            break;
        case 2:
            sprintf(buf,"%02d/%02d/%04d",day,month,year);
            break;
        default:
            sprintf(buf,"%04d/%02d/%02d",year,month,day);
            break;
    }
    sprintf(endDate,"%04d%02d%02d",year,month,day);
    gtk_entry_set_text(GTK_ENTRY(m_entry_date2),buf);
}

void ViewWorkList::ButtonClearClicked(GtkButton *button)
{
    gtk_entry_set_text(GTK_ENTRY(m_entry_ID),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_name),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_accession),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_request),"");
   // gtk_entry_set_text(GTK_ENTRY(m_entry_date1),"");
   // gtk_entry_set_text(GTK_ENTRY(m_entry_date2),"");
    InitScheduledDate();
}

void ViewWorkList::LoadPatientInfo()
{
    const char * patientID = gtk_entry_get_text(GTK_ENTRY(m_entry_ID));
    const char * patientName = gtk_entry_get_text(GTK_ENTRY(m_entry_name));
    const char * accessionNumber = gtk_entry_get_text(GTK_ENTRY(m_entry_accession));
    const char * requestedProcedureID = gtk_entry_get_text(GTK_ENTRY(m_entry_request));
    //const char * startDate = gtk_entry_get_text(GTK_ENTRY(m_entry_date1));
    //const char * endDate = gtk_entry_get_text(GTK_ENTRY(m_entry_date2));

     //printf("-----Query patientID patientName accessionNumber requestedProcedureID startDate endDate:%s %s %s %s %s %s\n",patientID, patientName, accessionNumber, requestedProcedureID ,startDate ,endDate);

	char name[256] = {0};
	snprintf(name, 256, "*%s*", patientName);
	name[255] = '\0';

     m_query = CDCMMan::GetMe()->QueryWorklist(patientID, name,accessionNumber,requestedProcedureID, startDate, endDate);
}
#ifndef VET
int StartExam(gpointer data)
{
	string errmsg;
	// archive patient info
	if (g_patientInfo.ArchivePatientInfo(errmsg)) {
        ViewNewPat::GetInstance()->UpdateStudyInfo();
        g_menuCalc.ClearAllData();
		// archive and clear image and viedo
		//g_patientInfo.ArchiveImg();
		// archive report
		g_patientInfo.ArchiveReport();
		// archive and clear image and viedo
		g_patientInfo.ArchiveImg();
		// clear patient info
		g_patientInfo.ClearAll();
        ViewNewPat::GetInstance()->ClearStudyInfo();
        // clear desctiption and comment
        ViewReport::GetInstance()->ClearIndicationandComments();
        //clear screen 
        KeyClearScreen kcs;
        kcs.Execute();
#if 0	
        if (GTK_IS_WIDGET(ViewNewPat::GetInstance()->GetWindow())) {
            if (ViewNewPat::GetInstance()->GetClearStatus())
                ViewNewPat::GetInstance()->ClearData();
            else 
                ViewNewPat::GetInstance()->ClearExamData();
        }
#endif
        setlocale(LC_NUMERIC, "en_US.UTF-8");
        if(CDCMMan::GetMe()->EndStudy())
        {
            SysDicomSetting sysDicomSetting;
            if(sysDicomSetting.GetMPPS())
            {
                if(ViewNewPat::GetInstance()->GetMPPSFlag())
                {
                    int Year, Month, Day, Hour, Minute, Second;
                    GetCurrentDateTime(Year, Month, Day, Hour, Minute, Second);

                    string year, mon, day, studyEndDate;
                    ChangeDateFormatToString(Year, Month, Day, year, mon, day); 
                    studyEndDate = year+mon+day;

                    string hour, min, sec, studyEndTime;
                    ChangeTimeFormatToString(Hour, Minute, Second, hour, min, sec); 
                    studyEndTime = hour + min + sec;

                    CDCMMan::GetMe()->EndMPPS(studyEndDate,studyEndTime);
                    ViewNewPat::GetInstance()->SetMPPSFlag(false);
                }
            }
        }

        SysGeneralSetting sysGS;
        int language= sysGS.GetLanguage();
        if (ZH == language)
        {
            setlocale(LC_NUMERIC, "zh_CN.UTF-8");
        }
        else if (RU == language)
        {
            setlocale(LC_NUMERIC, "ru_RU.UTF-8");
        }
        else if (PL == language)
        {
            setlocale(LC_NUMERIC, "pl_PL.UTF-8");
        }
        else if (FR == language)
        {
            setlocale(LC_NUMERIC, "fr_FR.UTF-8");
        }
        else if (DE == language)
        {
            setlocale(LC_NUMERIC, "de_DE.UTF-8");
        }
        else if(ES == language)
        {
            setlocale(LC_NUMERIC, "es_ES.UTF-8");
        }
        else
        {
            setlocale(LC_NUMERIC, "en_US.UTF-8");
        }

        ViewWorkList::GetInstance()->ImportPatInfo();
        //  ViewWorkList::GetInstance()->GetSelectedPatInfo();
        // ViewWorkList::GetInstance()->DestroyWin();
       // ViewNewPat::GetInstance()->DestroyWindow();
	} else {
          if (GTK_IS_WIDGET(ViewWorkList::GetInstance()->GetWindow())) {
			ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewWorkList::GetInstance()->GetWindow()), ViewDialog::ERROR, _(errmsg.c_str()), NULL);
        } 

    }
	return 0;
}
int InserInfo(gpointer data)
{
    ViewWorkList::GetInstance()->InserPatInfo(); 
    return 0;
}
void ViewWorkList::InserPatInfo()
{
	PatientInfo::Name patientName;
	ChangePersonNameFormat(m_query[selectedIndex].wlPatientName, patientName.first, patientName.last, patientName.mid);
	string doctorName;
	ChangePersonNameFormatForShow(m_query[selectedIndex].wlStudyDoctor, doctorName);
	ViewNewPat::GetInstance()->InsertPatientInfo(m_query[selectedIndex].wlPatientID.c_str(),patientName,m_query[selectedIndex].wlPatientBirthDate,m_query[selectedIndex].wlPatientAge,m_query[selectedIndex].wlPatientSex.c_str(),m_query[selectedIndex].wlPatientSize.c_str(),m_query[selectedIndex].wlPatientWeight.c_str(),m_query[selectedIndex].wlPatientAddress.c_str(),doctorName.c_str(),m_query[selectedIndex].wlStudyDescription.c_str());
    DestroyWin();
}

bool ViewWorkList::ArchiveWorkListPatInfo(bool startExam)
{
    const char* info = N_("End Exam?");
    if(startExam)
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::QUESTION, _(info), StartExam);
    else
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::QUESTION, _(info), InserInfo);

    return TRUE;
}
#endif
void ViewWorkList::ButtonQueryClicked(GtkButton *button)
{
   ClearQueryRes();

    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    gtk_list_store_clear(GTK_LIST_STORE(model));
    
    LoadPatientInfo();
    
    int num;
    char record_str[256];
    num = m_query.size(); 

    //get the number of the patient record
    sprintf(record_str,"%d %s",num,_("record is found"));
    gtk_label_set_text(GTK_LABEL(m_label_record),record_str);
   // gtk_label_set_use_markup (GTK_LABEL (m_label_record), TRUE);
    gtk_widget_show (m_label_record);

    DisplayPatientRecord();
}
#ifndef VET
void ViewWorkList::ButtonNewExamClicked(GtkButton *button)
{
    if(m_timeout > 0)
        {
            g_source_remove(m_timeout);
            m_timeout = 0;
        }

    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_worklist));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    { 
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        char *path_string = gtk_tree_path_to_string(path);
        int path_num = atoi(path_string);
        selectedIndex = path_num;
       // printf("----%s %s %s %s\n",m_query[path_num].wlPatientID.c_str(),m_query[path_num].wlPatientName.c_str(),m_query[path_num].wlPatientBirthDate.c_str(),m_query[path_num].wlPatientSex.c_str());

        
        if (g_patientInfo.GetExist()) {
            // end exam
            //KeyStartExam kee;
            // kee.Execute();
            //Archive
            ArchiveWorkListPatInfo(true);

        } 
        //printf("----%s %s %s %s\n",m_query[selectedIndex].wlPatientID.c_str(),m_query[selectedIndex].wlPatientName.c_str(),m_query[selectedIndex].wlPatientBirthDate.c_str(),m_query[selectedIndex].wlPatientSex.c_str());
        else
            ImportPatInfo();
       
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                ViewDialog::INFO, 
                _("No patient is selected!"),
                NULL);
     }
}
#endif
void ViewWorkList::ImportPatInfo()
{
    if(CDCMMan::GetMe()->IsExistedWorklistStudy(m_query[selectedIndex]))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                ViewDialog::INFO, 
                _("The exam record  has exsited, please select other record again!"),
                NULL);
        return;

    }
#ifndef VET
    GetSelectedPatInfo();
	PatientInfo::Name patientName;
	ChangePersonNameFormat(m_query[selectedIndex].wlPatientName, patientName.first, patientName.last, patientName.mid);
    g_patientInfo.UpdatePatInfoToTopArea(patientName,m_query[selectedIndex].wlPatientSex,m_query[selectedIndex].wlPatientAge,m_query[selectedIndex].wlPatientID);
#endif
    DestroyWin();
    ViewNewPat::GetInstance()->DestroyWindow();

}

void ViewWorkList::ChangePersonNameFormat(string wlPersonName, string &firstName, string &lastName, string &midName)
{
	int nPos = -1;
	int nPosT = -1;

	if ((nPos = wlPersonName.find('^')) != -1)
	{
		lastName = wlPersonName.substr(0, nPos);
		if ((nPosT = wlPersonName.find('^', nPos+1)) != -1)
		{
			firstName = wlPersonName.substr(nPos+1, nPosT-nPos-1);
			midName = wlPersonName.substr(nPosT+1);
		}
		else
		{
			firstName = wlPersonName.substr(nPos+1);
			midName = "";
		}
	}
	else
	{
		lastName = wlPersonName;
		firstName = "";
		midName = "";
	}
}

void ViewWorkList::ChangePersonNameFormatForShow(string wlPersonName, string &name)
{
	string firstName, lastName, midName;
	
	ChangePersonNameFormat(wlPersonName, firstName, lastName, midName);
	SysGeneralSetting sgs;
	 int lang = sgs.GetLanguage();
	 if(!lang)
		 name = firstName + " " + midName + " " + lastName;
	 else
		 name = lastName + firstName;
}
#ifndef VET

void ViewWorkList::GetSelectedPatInfo()
{
    PatientInfo::Info info;
    g_patientInfo.GetInfo(info);
    
    char str[25]="\0";
    char buf[20]="\0";
    sprintf(buf,"%s",m_query[selectedIndex].wlPatientSex.c_str());
    
    info.p.id = m_query[selectedIndex].wlPatientID;
    
	PatientInfo::Name patientName;
	ChangePersonNameFormat(m_query[selectedIndex].wlPatientName, patientName.first, patientName.last, patientName.mid);
	info.p.name.last = patientName.last;
	info.p.name.first = patientName.first;
	info.p.name.mid = patientName.mid;

    if(strcmp(buf,"F")==0)
        info.p.sex = 0;
    else if(strcmp(buf,"M")==0)
        info.p.sex = 1;
    else if(strcmp(buf,"O")==0)
        info.p.sex = 2;

    //age
    string age = m_query[selectedIndex].wlPatientAge;

    char str_tmp1[2]="\0";
    sprintf(str,"%c%c%c",age[0],age[1],age[2]);
    info.p.age = atoi(age.c_str());

    char str_tmp[2]="\0";
    sprintf(str_tmp,"%c",age[strlen(age.c_str())-1]);
    if(strcmp(str_tmp,"Y")==0)
        info.p.ageUnit = 0;
    else if(strcmp(str_tmp,"M")==0)
        info.p.ageUnit = 1;
    else if(strcmp(str_tmp,"D")==0)
        info.p.ageUnit = 2;

    //BirthDate
    string birth_date = m_query[selectedIndex].wlPatientBirthDate;

    char birth_year[20]="\0";
    char birth_month[20]="\0";
    char birth_day[20]="\0";

    sprintf(birth_year,"%c%c%c%c",birth_date[0],birth_date[1],birth_date[2],birth_date[3]);
    sprintf(birth_month,"%c%c",birth_date[4],birth_date[5]);
    sprintf(birth_day,"%c%c",birth_date[6],birth_date[7]);
    info.p.birthDate.year =atoi(birth_year); 
    info.p.birthDate.month =atoi(birth_month); 
    info.p.birthDate.day =atoi(birth_day); 
    info.p.address = m_query[selectedIndex].wlPatientAddress;

    //Size
    char size[256];
    strcpy(size, m_query[selectedIndex].wlPatientSize.c_str());
    DotToCommaLocaleNumeric(size, sizeof(size));
    int height = atoi(size);
    info.e.height = height*100;

    //Weight
    char Weight[256];
    strcpy(Weight, m_query[selectedIndex].wlPatientWeight.c_str());
    DotToCommaLocaleNumeric(Weight,sizeof(Weight));
    float weight = atof(Weight);
    info.e.weight = weight;

    ChangePersonNameFormatForShow(m_query[selectedIndex].wlStudyDoctor, info.e.examDoctor);
    info.e.comment = m_query[selectedIndex].wlStudyDescription;


    int examYear, examMonth, examDay;
    int examHour, examMin, examSec;
    GetCurrentDateTime(examYear, examMonth, examDay, examHour, examMin, examSec);
    info.e.examDate.year = examYear;
    info.e.examDate.month = examMonth;
    info.e.examDate.day = examDay;
    info.e.examTime.hour = examHour;
    info.e.examTime.minute = examMin;
    info.e.examTime.second = examSec;

    TopArea::GetInstance()->GetCheckPart(info.e.examType);

    Database db;
    db.GetExamIDNext(info.e.examNum);

    CDCMMan::GetMe()->EditStudyInfo(EditStudyInfo(info)); 
            //MPPS 
            if(!g_patientInfo.GetExist())
            {
                SysDicomSetting sysDicomSetting;
                if(sysDicomSetting.GetMPPS())
                {
                    if(CDCMMan::GetMe()->GetDefaultMPPSServiceDevice()=="")
                    {
                        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Please Set the default MPPS service in system setting"), NULL);
                        return ;
                    }
                    CDCMMan::GetMe()->StartMPPS(GetMPPSElement(info));
                }
            }

    g_patientInfo.SetInfo(info);
    TopArea* ptrTopArea = TopArea::GetInstance();
    if(ptrTopArea->GetReadImg())
        ptrTopArea->SetReadImg(TRUE);

}
#endif
void ViewWorkList::ButtonDetailClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *num;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_worklist));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    { 
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        char *path_string = gtk_tree_path_to_string(path);
        int path_num = atoi(path_string);
       // printf("----%s %s %s %s %s %s %s %s %s %s %s\n",m_query[path_num].wlPatientID.c_str(),m_query[path_num].wlPatientName.c_str(),m_query[path_num].wlPatientBirthDate.c_str(),m_query[path_num].wlPatientSex.c_str(),m_query[path_num].wlPatientSize.c_str(),m_query[path_num].wlPatientWeight.c_str(),m_query[path_num].wlPatientAddress.c_str(),m_query[path_num].wlStudyDoctor.c_str(),m_query[path_num].wlStudyDescription.c_str(),m_query[path_num].wlRequestedProcedureID.c_str(),m_query[path_num].wlAccessionNumber.c_str(),m_query[path_num].wlScheduleDate.c_str());
        m_patDetail.CreatePatDetailWin(m_window);
        m_patDetail.ClearData();
        if(m_query.size()==0)
            return;
        else
		{
			string patientName;
			ChangePersonNameFormatForShow(m_query[path_num].wlPatientName, patientName);
			string doctorName;
			ChangePersonNameFormatForShow(m_query[path_num].wlStudyDoctor, doctorName);
            m_patDetail.InsertPatDetail(m_query[path_num].wlPatientID.c_str(),patientName.c_str(),m_query[path_num].wlPatientBirthDate,m_query[path_num].wlPatientAge,m_query[path_num].wlPatientSex.c_str(),m_query[path_num].wlPatientSize.c_str(),m_query[path_num].wlPatientWeight.c_str(),m_query[path_num].wlPatientAddress.c_str(),doctorName.c_str(),m_query[path_num].wlStudyDescription.c_str(),m_query[path_num].wlRequestedProcedureID.c_str(),m_query[path_num].wlAccessionNumber.c_str(),m_query[path_num].wlScheduleDate);
		}
    }
    else
    {
#ifdef VET
 		ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                ViewDialog::INFO, 
                _("No animal is selected!"),
                NULL);
#else
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                ViewDialog::INFO, 
                _("No patient is selected!"),
                NULL);
#endif     
   return;
    }
}
#ifndef VET
void ViewWorkList::ButtonTransferClicked(GtkButton *button)
{
    if(m_timeout > 0)
        {
            g_source_remove(m_timeout);
            m_timeout = 0;
        }

    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *num;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_worklist));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    { 
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        char *path_string = gtk_tree_path_to_string(path);
        int path_num = atoi(path_string);
        selectedIndex = path_num;
        //printf("----%s %s %s %s\n",m_query[path_num].wlPatientID.c_str(),m_query[path_num].wlPatientName.c_str(),m_query[path_num].wlPatientBirthDate.c_str(),m_query[path_num].wlPatientSex.c_str());
        if(CDCMMan::GetMe()->IsExistedWorklistStudy(m_query[path_num]))
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                    ViewDialog::INFO, 
                    _("The exam record  has exsited, please select other record again!"),
                    NULL);
            return;

        }
        else
		{
			PatientInfo::Name patientName;
			ChangePersonNameFormat(m_query[path_num].wlPatientName, patientName.first, patientName.last, patientName.mid);
			string doctorName;
			ChangePersonNameFormatForShow(m_query[path_num].wlStudyDoctor, doctorName);
			ViewNewPat::GetInstance()->InsertPatientInfo(m_query[path_num].wlPatientID.c_str(),patientName,m_query[path_num].wlPatientBirthDate,m_query[path_num].wlPatientAge,m_query[path_num].wlPatientSex.c_str(),m_query[path_num].wlPatientSize.c_str(),m_query[path_num].wlPatientWeight.c_str(),m_query[path_num].wlPatientAddress.c_str(),doctorName.c_str(),m_query[path_num].wlStudyDescription.c_str());
            ViewNewPat::GetInstance()->SetStudyInfo(m_query[path_num]);
            DestroyWin();
        }
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                ViewDialog::INFO, 
                _("No patient is selected!"),
                NULL);
      return;
    }

}
#endif
void ViewWorkList::ButtonQuitClicked(GtkButton *button)
{
    DestroyWin();
}

void ViewWorkList::DisplayPatientRecord()
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    gtk_list_store_clear(GTK_LIST_STORE(model));

    for(int i= 0;i<m_query.size();i++)
    {
        //ScheduleDate
        char date_year[20] = "\0";
        char date_month[20] = "\0";
        char date_day[20] = "\0";
        char scheduleDate_buf[125] = "\0";

        if(strlen(m_query[i].wlScheduleDate.c_str())!=0)
        {

            sprintf(date_year,"%c%c%c%c",m_query[i].wlScheduleDate[0],m_query[i].wlScheduleDate[1],m_query[i].wlScheduleDate[2],m_query[i].wlScheduleDate[3]);
            sprintf(date_month,"%c%c",m_query[i].wlScheduleDate[4],m_query[i].wlScheduleDate[5]);
            sprintf(date_day,"%c%c",m_query[i].wlScheduleDate[6],m_query[i].wlScheduleDate[7]);
           switch(m_dateFormat)
           {
               case 0:
                   sprintf(scheduleDate_buf,"%s/%s/%s",date_year,date_month,date_day);
                   break;
               case 1:
                   sprintf(scheduleDate_buf,"%s/%s/%s",date_month,date_day,date_year);
                   break;
               case 2:
                   sprintf(scheduleDate_buf,"%s/%s/%s",date_day,date_month,date_year);
                   break;
               default:
                   sprintf(scheduleDate_buf,"%s/%s/%s",date_year,date_month,date_day);
                   break;
           }
        }
        else
            sprintf(scheduleDate_buf,"%s","");

        //Sex
        char sex_tmp[256]="\0";
        if (strlen(m_query[i].wlPatientSex.c_str())!=0)
        {
#ifdef VET
            if(strcmp(m_query[i].wlPatientSex.c_str(),"F")==0)
                sprintf(sex_tmp,"%s",_("Female "));
            else if(strcmp(m_query[i].wlPatientSex.c_str(),"M")==0)
                sprintf(sex_tmp,"%s",_("Male "));
#else
            if(strcmp(m_query[i].wlPatientSex.c_str(),"F")==0)
                sprintf(sex_tmp,"%s",_("Female"));
            else if(strcmp(m_query[i].wlPatientSex.c_str(),"M")==0)
                sprintf(sex_tmp,"%s",_("Male"));
#endif
            else if(strcmp(m_query[i].wlPatientSex.c_str(),"O")==0)
                sprintf(sex_tmp,"%s",_("Other"));

        }
        else
            sprintf(sex_tmp,"%s","");

        //BirthDate
        char birth_year[20] = "\0";
        char birth_month[20] = "\0";
        char birth_day[20] = "\0";
        char birthDate_buf[125] = "\0";

        if(strlen(m_query[i].wlPatientBirthDate.c_str())!=0)
        {
            sprintf(birth_year,"%c%c%c%c",m_query[i].wlPatientBirthDate[0],m_query[i].wlPatientBirthDate[1],m_query[i].wlPatientBirthDate[2],m_query[i].wlPatientBirthDate[3]);
            sprintf(birth_month,"%c%c",m_query[i].wlPatientBirthDate[4],m_query[i].wlPatientBirthDate[5]);
            sprintf(birth_day,"%c%c",m_query[i].wlPatientBirthDate[6],m_query[i].wlPatientBirthDate[7]);
#if 0
            strcat(birthDate_buf,birth_year);
            strcat(birthDate_buf,"/");
            strcat(birthDate_buf,birth_month);
            strcat(birthDate_buf,"/");
            strcat(birthDate_buf,birth_day);
#endif
            switch(m_dateFormat)
            {
                case 0:
                    sprintf(birthDate_buf,"%s/%s/%s",birth_year,birth_month,birth_day);
                   break;
               case 1:
                   sprintf(birthDate_buf,"%s/%s/%s",birth_month,birth_day,birth_year);
                   break;
               case 2:
                   sprintf(birthDate_buf,"%s/%s/%s",birth_day,birth_month,birth_year);
                   break;
               default:
                   sprintf(birthDate_buf,"%s/%s/%s",birth_year,birth_month,birth_day);
                   break;
           }

        }
        else
        {
            sprintf(birthDate_buf,"%s","");
        }

		string patientName;
		ChangePersonNameFormatForShow(m_query[i].wlPatientName, patientName);

        gtk_list_store_append(GTK_LIST_STORE(model), &iter);
        gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                COL_SCHEDULED_DATE,scheduleDate_buf,
                COL_PATIENT_ID,m_query[i].wlPatientID.c_str(),
                COL_NAME,patientName.c_str(),
                COL_Gender,sex_tmp,
                COL_ACCESSION_NUMBER,m_query[i].wlAccessionNumber.c_str(),
                COL_BIRTH_DATE,birthDate_buf,
                COL_EXAM_DESCRIPTION,m_query[i].wlStudyDescription.c_str(),
                -1);
        gtk_tree_model_iter_next(model, &iter); 
    }
    //m_query.clear();

}

gboolean UpdateAutoQuery(gpointer data)
{
	ViewWorkList *tmp;
	tmp = (ViewWorkList *)data;
	tmp->GetAndDisplayPatientRecord(); 
	return TRUE;
}

void ViewWorkList::AutoQuery()
{

    if(m_autoQueryFlag)
    {
#if 0   
    if(!(CDCMMan::GetMe()->TestLinkDefaultWorklist()))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewNewPat::GetInstance()->GetWindow()), ViewDialog::ERROR, _("Test Link is failed"), NULL);
        return;
    }
#endif

        GetAndDisplayPatientRecord();

        if(m_timeout > 0)
        {
            g_source_remove(m_timeout);
            m_timeout = 0;
        }

        m_timeout = g_timeout_add(30000, UpdateAutoQuery, this);
    }
    else
    {
        if(m_timeout > 0)
        {
            g_source_remove(m_timeout);
            m_timeout = 0;
        }
    }
}

void ViewWorkList::GetAndDisplayPatientRecord()
{
    m_query.clear();
    LoadPatientInfo();
   
    char record_str[256]="\0";
    int num = m_query.size(); 
    //get the number of the paitient record
    sprintf(record_str,"%d %s",num,_("record is found"));
    gtk_label_set_text(GTK_LABEL(m_label_record),record_str);
    // gtk_label_set_use_markup (GTK_LABEL (m_label_record), TRUE);
    gtk_widget_show (m_label_record);

    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    gtk_list_store_clear(GTK_LIST_STORE(model));

    if(m_query.size()==0)
        return;

    for(int i= 0;i<m_query.size();i++)
    {
        //ScheduleDate
        char date_year[20] = "\0";
        char date_month[20] = "\0";
        char date_day[20] = "\0";
        char scheduleDate_buf[125] = "\0";

        if(strlen(m_query[i].wlScheduleDate.c_str())!=0)
        {

            sprintf(date_year,"%c%c%c%c",m_query[i].wlScheduleDate[0],m_query[i].wlScheduleDate[1],m_query[i].wlScheduleDate[2],m_query[i].wlScheduleDate[3]);
            sprintf(date_month,"%c%c",m_query[i].wlScheduleDate[4],m_query[i].wlScheduleDate[5]);
            sprintf(date_day,"%c%c",m_query[i].wlScheduleDate[6],m_query[i].wlScheduleDate[7]);
#if 0
            strcat(scheduleDate_buf,date_year);
            strcat(scheduleDate_buf,"/");
            strcat(scheduleDate_buf,date_month);
            strcat(scheduleDate_buf,"/");
            strcat(scheduleDate_buf,date_day);
#endif
            switch(m_dateFormat)
           {
               case 0:
                   sprintf(scheduleDate_buf,"%s/%s/%s",date_year,date_month,date_day);
                   break;
               case 1:
                   sprintf(scheduleDate_buf,"%s/%s/%s",date_month,date_day,date_year);
                   break;
               case 2:
                   sprintf(scheduleDate_buf,"%s/%s/%s",date_day,date_month,date_year);
                   break;
               default:
                   sprintf(scheduleDate_buf,"%s/%s/%s",date_year,date_month,date_day);
                   break;
           }

        }
        else
            sprintf(scheduleDate_buf,"%s","");

        //Sex
        char sex_tmp[256]="\0";
        if (strlen(m_query[i].wlPatientSex.c_str())!=0)
        {
#ifdef VET
            if(strcmp(m_query[i].wlPatientSex.c_str(),"F")==0)
                sprintf(sex_tmp,"%s",_("Female "));
            else if(strcmp(m_query[i].wlPatientSex.c_str(),"M")==0)
                sprintf(sex_tmp,"%s",_("Male "));
#else
            if(strcmp(m_query[i].wlPatientSex.c_str(),"F")==0)
                sprintf(sex_tmp,"%s",_("Female"));
            else if(strcmp(m_query[i].wlPatientSex.c_str(),"M")==0)
                sprintf(sex_tmp,"%s",_("Male"));
#endif
            else if(strcmp(m_query[i].wlPatientSex.c_str(),"O")==0)
                sprintf(sex_tmp,"%s",_("Other"));

        }
        else
            sprintf(sex_tmp,"%s","");

        //BirthDate
        char birth_year[20] = "\0";
        char birth_month[20] = "\0";
        char birth_day[20] = "\0";
        char birthDate_buf[125] = "\0";

        if(strlen(m_query[i].wlPatientBirthDate.c_str())!=0)
        {
            sprintf(birth_year,"%c%c%c%c",m_query[i].wlPatientBirthDate[0],m_query[i].wlPatientBirthDate[1],m_query[i].wlPatientBirthDate[2],m_query[i].wlPatientBirthDate[3]);
            sprintf(birth_month,"%c%c",m_query[i].wlPatientBirthDate[4],m_query[i].wlPatientBirthDate[5]);
            sprintf(birth_day,"%c%c",m_query[i].wlPatientBirthDate[6],m_query[i].wlPatientBirthDate[7]);
#if 0
            strcat(birthDate_buf,birth_year);
            strcat(birthDate_buf,"/");
            strcat(birthDate_buf,birth_month);
            strcat(birthDate_buf,"/");
            strcat(birthDate_buf,birth_day);
#endif
            switch(m_dateFormat)
            {
                case 0:
                    sprintf(birthDate_buf,"%s/%s/%s",birth_year,birth_month,birth_day);
                   break;
               case 1:
                   sprintf(birthDate_buf,"%s/%s/%s",birth_month,birth_day,birth_year);
                   break;
               case 2:
                   sprintf(birthDate_buf,"%s/%s/%s",birth_day,birth_month,birth_year);
                   break;
               default:
                   sprintf(birthDate_buf,"%s/%s/%s",birth_year,birth_month,birth_day);
                   break;
           }

        }
        else
        {
            sprintf(birthDate_buf,"%s","");
        }

		string patientName;
		ChangePersonNameFormatForShow(m_query[i].wlPatientName, patientName);

        gtk_list_store_append(GTK_LIST_STORE(model), &iter);
        gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                COL_SCHEDULED_DATE,scheduleDate_buf,
                COL_PATIENT_ID,m_query[i].wlPatientID.c_str(),
                COL_NAME,patientName.c_str(),
                COL_Gender,sex_tmp,
                COL_ACCESSION_NUMBER,m_query[i].wlAccessionNumber.c_str(),
                COL_BIRTH_DATE,birthDate_buf,
                COL_EXAM_DESCRIPTION,m_query[i].wlStudyDescription.c_str(),
                -1);
        gtk_tree_model_iter_next(model, &iter); 
    }

}

int ViewWorkList::GetWorkListQuery(void)
{
    int size = m_query.size();
    return size;
}

DCMMPPSELEMENT ViewWorkList::GetMPPSElement(PatientInfo::Info &info)
{
    DCMMPPSELEMENT ms;
    ms.msSpecificCharacterSet = m_query[selectedIndex].wlSpecificCharacterSet;
    ms.msPatientName = m_query[selectedIndex].wlPatientName;
    ms.msPatientID = m_query[selectedIndex].wlPatientID;
    ms.msPatientBirthDate = m_query[selectedIndex].wlPatientBirthDate;
    ms.msPatientSex = m_query[selectedIndex].wlPatientSex;
    ms.msRequestedProcedureID = m_query[selectedIndex].wlRequestedProcedureID;
    ms.msStudyInstanceUID = m_query[selectedIndex].wlStudyInstanceUID;
    ms.msAccessionNumber = m_query[selectedIndex].wlAccessionNumber;
    ms.msStudyDescription = m_query[selectedIndex].wlStudyDescription;
    ms.msRequestedProcedureDescription = m_query[selectedIndex].wlRequestedProcedureDescription;
    ms.msScheduledProcedureStepID = m_query[selectedIndex].wlScheduledProcedureStepID;

    string year, mon, day, studyDate;
    ChangeDateFormatToString(info.e.examDate.year, info.e.examDate.month, info.e.examDate.day, year, mon, day); 
    studyDate = year+mon+day;
    ms.msStudyStartDate = studyDate;

    string hour, min, sec, studyTime;
    ChangeTimeFormatToString(info.e.examTime.hour, info.e.examTime.minute, info.e.examTime.second, hour, min, sec); 
    studyTime = hour + min + sec;
    ms.msStudyStartTime = studyTime;

    return ms;
}

DCMSTUDYELEMENT ViewWorkList::EditStudyInfo(PatientInfo::Info &info)
{
    DCMSTUDYELEMENT st;
    st.stSpecificCharacterSet = m_query[selectedIndex].wlSpecificCharacterSet;
    st.stPatientName = m_query[selectedIndex].wlPatientName;
    st.stPatientID = m_query[selectedIndex].wlPatientID;
    st.stPatientBirthDate = m_query[selectedIndex].wlPatientBirthDate;
    st.stPatientSex = m_query[selectedIndex].wlPatientSex;
    st.stPatientAge = m_query[selectedIndex].wlPatientAge;
    st.stPatientWeight = m_query[selectedIndex].wlPatientWeight;
    st.stPatientSize = m_query[selectedIndex].wlPatientSize;
    st.stStudyInstanceUID = m_query[selectedIndex].wlStudyInstanceUID;
    st.stAccessionNumber = m_query[selectedIndex].wlAccessionNumber;
    st.stStudyDoctor = m_query[selectedIndex].wlStudyDoctor;
    st.stStudyDescription = m_query[selectedIndex].wlStudyDescription;
    
    string year, mon, day, studyDate;
    ChangeDateFormatToString(info.e.examDate.year, info.e.examDate.month, info.e.examDate.day, year, mon, day); 
    studyDate = year+mon+day;
    st.stStudyDate =studyDate;
    
    string hour, min, sec, studyTime;
    ChangeTimeFormatToString(info.e.examTime.hour, info.e.examTime.minute, info.e.examTime.second, hour, min, sec); 
    studyTime = hour + min + sec;
    st.stStudyTime = studyTime;

#if 0
    int examYear = info.e.examDate.year;
    int examMonth = info.e.examDate.month ;
    int examDay = info.e.examDate.day ;
    int examHour = info.e.examTime.hour;
    int examMin = info.e.examTime.minute;
    int examSec = info.e.examTime.second;
    char studyDate[125];
    char studyTime[125];
#endif
    string hospital;
    string machine;
    TopArea::GetInstance()->GetHospitalName(hospital);
    UpgradeMan::GetInstance()->GetMachineType(machine);

    //sprintf(studyDate,"%d%d%d",examYear,examMonth,examDay);
    //sprintf(studyTime,"%d%d%d",examHour,examMin,examSec);

    //st.stStudyDate =(string)studyDate;
    //st.stStudyTime = (string)studyTime;
    st.stFactoryName = "EMP";
    st.stHospital = hospital; 
    st.stMechineType = machine;
    st.stMechineSN ="";
    st.stStudyNo = atoi(info.e.examNum.c_str());
#ifndef VET
    ViewNewPat::GetInstance()->SetStudyInfo(st);
    return st;
#endif
}

void ViewWorkList::ClearQueryRes(void)
{
    m_query.clear();
    selectedIndex = 0;
}

static gboolean ExitWindow(gpointer data)
{
	ViewWorkList *tmp;
	tmp = (ViewWorkList *)data;
	tmp->DestroyWin();
	return FALSE;
}

void ViewWorkList::KeyEvent(unsigned char keyValue)
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

void ViewWorkList::CloseWindow(GtkWidget *window)
{
   // DestroyWin();
#if 1
    if(m_timeout > 0)
    {
        g_source_remove(m_timeout);
        m_timeout = 0;
    }

    if (window)
    {
        g_keyInterface.Pop();
        gtk_widget_destroy(window);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
    window = NULL;
#endif
}

void ViewWorkList::DestroyWin(void)
{
#if 1
    if(m_timeout > 0)
        {
            g_source_remove(m_timeout);
            m_timeout = 0;
        }
#endif
    if (m_window)
    {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
    m_window = NULL;
}

//PatDetail* PatDetail::m_ptrInstance = NULL;

PatDetail::PatDetail()
{

}

PatDetail::~PatDetail()
{
    //if (m_ptrInstance != NULL)
     //   delete m_ptrInstance;
}
#if 0
PatDetail* PatDetail::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new PatDetail;
    return m_ptrInstance;
}
#endif

GtkWidget* PatDetail::CreatePatDetailWin(GtkWidget *parent)
{
    GtkWidget *window_detail;
    GtkWidget *fixed_detail;
    GtkWidget *frame_detail;
    GtkWidget *label_detail;

    GtkWidget *fixed;
    GtkWidget *label_ID;
    GtkWidget *label_name;
    GtkWidget *label_accession;
    GtkWidget *label_birth_date;
    GtkWidget *label_age;
    GtkWidget *label_request;
    GtkWidget *label_sex;
    GtkWidget *label_date;
    GtkWidget *label_doctor;
    GtkWidget *label_stature;
    GtkWidget *label_weight;
    GtkWidget *label_address;

    GtkWidget *label_comment;
    GtkWidget *scrolledwindow_comment;

    GtkWidget *image_quit;
    GtkWidget *label_quit;
    GtkWidget *button_quit;

    window_detail = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (window_detail, 840, 640);
#ifdef VET
    gtk_window_set_title (GTK_WINDOW (window_detail), _("Animal Detail"));
#else
    gtk_window_set_title (GTK_WINDOW (window_detail), _("Patient Detail"));
#endif
    gtk_window_set_transient_for(GTK_WINDOW(window_detail), GTK_WINDOW(parent));
    gtk_window_set_modal(GTK_WINDOW(window_detail), TRUE);
    gtk_window_set_position (GTK_WINDOW (window_detail), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW (window_detail), FALSE);

    fixed_detail = gtk_fixed_new ();
    gtk_widget_show (fixed_detail);
    gtk_container_add (GTK_CONTAINER (window_detail), fixed_detail);

    frame_detail = gtk_frame_new (NULL);
    gtk_widget_show (frame_detail);
    gtk_fixed_put (GTK_FIXED (fixed_detail), frame_detail, 10+10, 5+20);
    gtk_widget_set_size_request (frame_detail, 800, 600-200);
   // gtk_frame_set_label_align (GTK_FRAME (frame_detail), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_detail), GTK_SHADOW_IN);

    label_detail = gtk_label_new (_("<b>Detail</b>"));
    gtk_widget_show (label_detail);
    gtk_frame_set_label_widget (GTK_FRAME (frame_detail), label_detail);
    gtk_label_set_use_markup (GTK_LABEL (label_detail), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame_detail), fixed);

#ifdef VET
	label_ID = gtk_label_new (_("ID :"));
#else
    label_ID = gtk_label_new (_("Patient ID :"));
#endif
    gtk_widget_show (label_ID);
    gtk_fixed_put (GTK_FIXED (fixed), label_ID, 10-10, 8);
    gtk_label_set_use_markup (GTK_LABEL (label_ID), TRUE);
    gtk_widget_set_size_request (label_ID, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_ID), 0.9, 0.5);

    m_entry_ID = gtk_entry_new ();
    gtk_widget_show (m_entry_ID);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_ID, 110-10, 8);
    gtk_widget_set_size_request (m_entry_ID, 100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_ID), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ID), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_ID),false);
    //gtk_widget_set_sensitive(m_entry_ID,false);
    
#ifdef VET
 	label_name = gtk_label_new (_("Animal Name:"));
#else
    label_name = gtk_label_new (_("Patient Name :"));
#endif
    gtk_widget_show (label_name);
    gtk_fixed_put (GTK_FIXED (fixed), label_name, 200-20, 8);
    gtk_label_set_use_markup (GTK_LABEL (label_name), TRUE);
#ifdef VET
 	 gtk_widget_set_size_request (label_name, 130+10+5, 30);
#else
    gtk_widget_set_size_request (label_name, 130+10, 30);
#endif
    gtk_misc_set_alignment (GTK_MISC (label_name), 0.9, 0.5);

    m_entry_name = gtk_entry_new ();
    gtk_widget_show (m_entry_name);
#ifdef VET
 	gtk_fixed_put (GTK_FIXED (fixed), m_entry_name, 330-10+5, 8);
    gtk_widget_set_size_request (m_entry_name, 200-20, 30);
#else
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_name, 330-10, 8);
    gtk_widget_set_size_request (m_entry_name, 200, 30);
#endif
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_name), 45);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_name), 9679);
    gtk_editable_set_editable(GTK_EDITABLE (m_entry_name),false);
    //gtk_widget_set_sensitive(m_entry_name,false);

    label_accession = gtk_label_new (_("Accession Number :"));
    gtk_widget_show (label_accession);
    gtk_fixed_put (GTK_FIXED (fixed), label_accession, 510-10-30, 8);
    gtk_label_set_use_markup (GTK_LABEL (label_accession), TRUE);
    gtk_widget_set_size_request (label_accession, 170+30, 30);
    gtk_misc_set_alignment (GTK_MISC (label_accession), 0.9, 0.5);

    m_entry_accession = gtk_entry_new ();
    gtk_widget_show (m_entry_accession);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_accession, 660+10, 8);
    gtk_widget_set_size_request (m_entry_accession, 100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_accession), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_accession), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_accession),false);
    //gtk_widget_set_sensitive(m_entry_accession,false);

    label_birth_date = gtk_label_new (_("Birth Date:"));
    gtk_widget_show (label_birth_date);
    gtk_fixed_put (GTK_FIXED (fixed), label_birth_date, 0, 45);
    gtk_label_set_use_markup (GTK_LABEL (label_birth_date), TRUE);
    gtk_widget_set_size_request (label_birth_date, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_birth_date), 0.9, 0.5);

    m_entry_birth_date = gtk_entry_new ();
    gtk_widget_show (m_entry_birth_date);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_birth_date, 100, 45);
    gtk_widget_set_size_request (m_entry_birth_date, 100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_birth_date), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_birth_date), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_birth_date),false);
    //gtk_widget_set_sensitive(m_entry_birth_date,false);
    
    label_age = gtk_label_new (_("Age:"));
    gtk_widget_show (label_age);
    gtk_fixed_put (GTK_FIXED (fixed), label_age, 200-20, 45);
    gtk_label_set_use_markup (GTK_LABEL (label_age), TRUE);
    gtk_widget_set_size_request (label_age, 100+40, 30);
    gtk_misc_set_alignment (GTK_MISC (label_age), 0.9, 0.5);

    m_entry_age = gtk_entry_new ();
    gtk_widget_show (m_entry_age);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_age, 300+20, 45);
    gtk_widget_set_size_request (m_entry_age, 100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_age), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_age), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_age),false);
    //gtk_widget_set_sensitive(m_entry_age,false);
  
    label_request = gtk_label_new (_("Requested Procedure ID :"));
    gtk_widget_show (label_request);
    gtk_fixed_put (GTK_FIXED (fixed), label_request, 420+10, 45);
    gtk_label_set_use_markup (GTK_LABEL (label_request), TRUE);
    gtk_widget_set_size_request (label_request, 200+10, 30);
    gtk_misc_set_alignment (GTK_MISC (label_request), 0.9, 0.5);

    m_entry_request = gtk_entry_new ();
    gtk_widget_show (m_entry_request);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_request, 620+20, 45);
    gtk_widget_set_size_request (m_entry_request, 100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_request), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_request), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_request),false);
   // gtk_widget_set_sensitive(m_entry_request,false);

    label_sex = gtk_label_new (_("Gender :"));
    gtk_widget_show (label_sex);
    gtk_fixed_put (GTK_FIXED (fixed), label_sex, 0, 82);
    gtk_label_set_use_markup (GTK_LABEL (label_sex), TRUE);
    gtk_widget_set_size_request (label_sex, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_sex), 0.9, 0.5);

    m_entry_sex = gtk_entry_new ();
    gtk_widget_show (m_entry_sex);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_sex, 100, 82);
    gtk_widget_set_size_request (m_entry_sex, 100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_sex), 10);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_sex), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_sex),false);
   // gtk_widget_set_sensitive(m_entry_sex,false);

    label_date = gtk_label_new (_("Scheduled Date :"));
    gtk_widget_show (label_date);
    gtk_fixed_put (GTK_FIXED (fixed), label_date, 200-20, 82);
    gtk_label_set_use_markup (GTK_LABEL (label_date), TRUE);
    gtk_widget_set_size_request (label_date, 150-10, 30);
    gtk_misc_set_alignment (GTK_MISC (label_date), 0.9, 0.5);

    m_entry_date = gtk_entry_new ();
    gtk_widget_show (m_entry_date);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_date, 350-30, 82);
    gtk_widget_set_size_request (m_entry_date, 100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_date), 10);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_date), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_date),false);
   // gtk_widget_set_sensitive(m_entry_date,false);
    
    label_doctor = gtk_label_new (_("Diagnostician :"));
    gtk_widget_show (label_doctor);
    gtk_fixed_put (GTK_FIXED (fixed), label_doctor, 450, 82);
    gtk_label_set_use_markup (GTK_LABEL (label_doctor), TRUE);
    gtk_widget_set_size_request (label_doctor, 150, 30);
    gtk_misc_set_alignment (GTK_MISC (label_doctor), 0.9, 0.5);

    m_entry_doctor = gtk_entry_new ();
    gtk_widget_show (m_entry_doctor);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_doctor, 600, 82);
    gtk_widget_set_size_request (m_entry_doctor, 100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_doctor), 45);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_doctor), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_doctor),false);
   // gtk_widget_set_sensitive(m_entry_doctor,false);

    label_stature = gtk_label_new (_("Stature :"));
    gtk_widget_show (label_stature);
    gtk_fixed_put (GTK_FIXED (fixed), label_stature, 0, 119);
    gtk_label_set_use_markup (GTK_LABEL (label_stature), TRUE);
    gtk_widget_set_size_request (label_stature, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_stature), 0.9, 0.5);

    m_entry_stature = gtk_entry_new ();
    gtk_widget_show (m_entry_stature);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_stature, 100, 119);
    gtk_widget_set_size_request (m_entry_stature, 100-40, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_stature), 10);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_stature), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_stature),false);
  //  gtk_widget_set_sensitive(m_entry_stature,false);

    label_weight = gtk_label_new (_("Weight :"));
    gtk_widget_show (label_weight);
    gtk_fixed_put (GTK_FIXED (fixed), label_weight, 160, 119);
    gtk_label_set_use_markup (GTK_LABEL (label_weight), TRUE);
    gtk_widget_set_size_request (label_weight, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_weight), 0.9, 0.5);

    m_entry_weight = gtk_entry_new ();
    gtk_widget_show (m_entry_weight);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_weight, 260, 119);
    gtk_widget_set_size_request (m_entry_weight, 70, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_weight), 10);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_weight), 9679);
    gtk_editable_set_editable(GTK_EDITABLE (m_entry_weight),false);
  //  gtk_widget_set_sensitive(m_entry_weight,false);

    label_address = gtk_label_new (_("Address :"));
    gtk_widget_show (label_address);
    gtk_fixed_put (GTK_FIXED (fixed), label_address, 340, 119);
    gtk_label_set_use_markup (GTK_LABEL (label_address), TRUE);
    gtk_widget_set_size_request (label_address, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_address), 0.9, 0.5);

    m_entry_address = gtk_entry_new ();
    gtk_widget_show (m_entry_address);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_address,440, 119);
    gtk_widget_set_size_request (m_entry_address, 100+230, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_address), 100);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_address), 9679);
    gtk_editable_set_editable(GTK_EDITABLE (m_entry_address),false);
  //  gtk_widget_set_sensitive(m_entry_address,false);

    label_comment = gtk_label_new (_(" Comment "));
    gtk_widget_show (label_comment);
    gtk_fixed_put (GTK_FIXED (fixed), label_comment, 20,170);
    gtk_widget_set_size_request (label_comment, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_comment), 0.1, 0.5);

    scrolledwindow_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_comment);
    gtk_fixed_put (GTK_FIXED (fixed), scrolledwindow_comment, 20, 207);
    gtk_widget_set_size_request (scrolledwindow_comment, 760, 230-100);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_comment), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_comment), GTK_SHADOW_OUT);

    m_textview_comment = gtk_text_view_new ();
    gtk_widget_show (m_textview_comment);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (m_textview_comment), GTK_WRAP_WORD);
    gtk_container_add (GTK_CONTAINER (scrolledwindow_comment), m_textview_comment);
    gtk_text_view_set_editable(GTK_TEXT_VIEW (m_textview_comment),false);
   // gtk_widget_set_sensitive(m_textview_comment,false);
    
    image_quit = gtk_image_new_from_stock ("gtk-quit", GTK_ICON_SIZE_BUTTON);
    label_quit = gtk_label_new_with_mnemonic (_("Exit"));
    button_quit = create_button_icon(label_quit, image_quit);
    gtk_widget_show (button_quit);
    gtk_fixed_put (GTK_FIXED (fixed_detail), button_quit, 530+150, 430+20);
    gtk_widget_set_size_request (button_quit, 85, 35);
    g_signal_connect(button_quit, "clicked", G_CALLBACK(HandleButtonQuitClicked), this);

    gtk_widget_show_all(window_detail);
    
    m_window_detail = window_detail;
    g_keyInterface.Push(this);
    SetSystemCursorToCenter();

    return window_detail;
}

void PatDetail::InsertPatDetail(const char *ID,const char *name,string birthDate,string age,const char *sex,const char *size,const char *weight,const char *address,const char *doctor,const char *description,const char *requestedID,const char *accessionNumber,string scheduleDate)
{
    int dateFormat = ViewWorkList::GetInstance()->GetDateFormat();

    char size_tmp[100]="\0";
    char weight_tmp[100]="\0";
#if 0
    int birth_pos = 0;
    int pos = 0;
#endif
    int date_pos = 0;

           
    gtk_entry_set_text(GTK_ENTRY(m_entry_ID),ID);
    gtk_entry_set_text(GTK_ENTRY(m_entry_name),name);
    gtk_entry_set_text(GTK_ENTRY(m_entry_accession),accessionNumber);
 
    //birthDate
    if(strlen(birthDate.c_str())!=0)
    {
        char birth_year[100] = "\0";
        char birth_month[100] = "\0";
        char birth_day[100] = "\0";
        char buf[100] = "\0";

        sprintf(birth_year,"%c%c%c%c",birthDate[0],birthDate[1],birthDate[2],birthDate[3]);
        sprintf(birth_month,"%c%c",birthDate[4],birthDate[5]);
        sprintf(birth_day,"%c%c",birthDate[6],birthDate[7]);
#if 0
        strcat(buf,birth_year);
        strcat(buf,"/");
        strcat(buf,birth_month);
        strcat(buf,"/");
        strcat(buf,birth_day);
#endif
        switch(dateFormat)
            {
                case 0:
                    sprintf(buf,"%s/%s/%s",birth_year,birth_month,birth_day);
                   break;
               case 1:
                   sprintf(buf,"%s/%s/%s",birth_month,birth_day,birth_year);
                   break;
               case 2:
                   sprintf(buf,"%s/%s/%s",birth_day,birth_month,birth_year);
                   break;
               default:
                   sprintf(buf,"%s/%s/%s",birth_year,birth_month,birth_day);
                   break;
           }

        gtk_entry_set_text(GTK_ENTRY(m_entry_birth_date), buf);
    }

    char str[100];
    //age
    if(strlen(age.c_str())!= 0)
    {

        char buf[100] = "\0";
        char tmp[100] = "\0";
        sprintf(str,"%c%c%c",age[0],age[1],age[2]);

        sprintf(tmp,"%d",atoi(str));
        strcat(buf,tmp);
        
        char str_tmp[100]="\0";
        int age_len = strlen(age.c_str());
        sprintf(str_tmp,"%c",age[age_len-1]);
        if(strcmp(str_tmp,"Y")==0)
            strcat(buf,_("Year"));
        else if(strcmp(str_tmp,"M")==0)
            strcat(buf,_("Month"));
        else if(strcmp(str_tmp,"D")==0)
            strcat(buf,_("Day"));

        gtk_entry_set_text(GTK_ENTRY(m_entry_age),buf);
    }
    
    gtk_entry_set_text(GTK_ENTRY(m_entry_request),requestedID);

    //sex
    if(sex !=NULL)
    {
#ifdef VET
        if(strcmp(sex ,"F")==0)
            gtk_entry_set_text(GTK_ENTRY(m_entry_sex),_("Female "));
        else if(strcmp(sex , "M")==0)
            gtk_entry_set_text(GTK_ENTRY(m_entry_sex),_("Male "));
#else
        if(strcmp(sex ,"F")==0)
            gtk_entry_set_text(GTK_ENTRY(m_entry_sex),_("Female"));
        else if(strcmp(sex , "M")==0)
            gtk_entry_set_text(GTK_ENTRY(m_entry_sex),_("Male"));
#endif
        else if(strcmp(sex , "O")==0)
            gtk_entry_set_text(GTK_ENTRY(m_entry_sex),_("Other"));

    }
    else
        gtk_entry_set_text(GTK_ENTRY(m_entry_sex),_("Other"));

    //scheduleDate
    if(strlen(scheduleDate.c_str())!=0)
    {
        char date_year[256] = "\0";
        char date_month[100] = "\0";
        char date_day[100] = "\0";
        char buf[100] = "\0";

        sprintf(date_year,"%c%c%c%c",scheduleDate[0],scheduleDate[1],scheduleDate[2],scheduleDate[3]);
        sprintf(date_month,"%c%c",scheduleDate[4],scheduleDate[5]);
        sprintf(date_day,"%c%c",scheduleDate[6],scheduleDate[7]);
#if 0
        strcat(buf,date_year);
        strcat(buf,"/");
        strcat(buf,date_month);
        strcat(buf,"/");
        strcat(buf,date_day);
#endif
        switch(dateFormat)
           {
               case 0:
                   sprintf(buf,"%s/%s/%s",date_year,date_month,date_day);
                   break;
               case 1:
                   sprintf(buf,"%s/%s/%s",date_month,date_day,date_year);
                   break;
               case 2:
                   sprintf(buf,"%s/%s/%s",date_day,date_month,date_year);
                   break;
               default:
                   sprintf(buf,"%s/%s/%s",date_year,date_month,date_day);
                   break;
           }

        gtk_entry_set_text(GTK_ENTRY(m_entry_date), buf);
       
    }
   
    gtk_entry_set_text(GTK_ENTRY(m_entry_doctor),doctor);

    char sizeT[256];
    strcpy(sizeT,size);
    char weightT[256];
    strcpy(weightT,weight);

    if(sizeT !=NULL)
    {
        DotToCommaLocaleNumeric(sizeT, sizeof(sizeT));

        float stature = atof(sizeT);
        int stature_real=stature*100;
        sprintf(size_tmp,"%dcm",stature_real);
        gtk_entry_set_text(GTK_ENTRY(m_entry_stature),size_tmp);
    }

    if(weightT != NULL)
    {
        char weight_buf[30]="\0";
        DotToCommaLocaleNumeric(weightT, sizeof(weightT));

        float weight_tmp = atof(weightT);
        sprintf(weight_buf,"%.1fkg",weight_tmp);
        gtk_entry_set_text(GTK_ENTRY(m_entry_weight),weight_buf);
    }

    gtk_entry_set_text(GTK_ENTRY(m_entry_address),address);

    if(description != NULL)
    {
        GtkTextBuffer *buffer;
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment));
        gtk_text_buffer_set_text(buffer, description, -1);
    }
}

void PatDetail::ClearData()
{
    gtk_entry_set_text(GTK_ENTRY(m_entry_ID), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_name), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_accession), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_birth_date), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_age),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_request),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_sex),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_date), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_doctor),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_stature),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_weight),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_address),"");
    
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment));
    gtk_text_buffer_set_text(buffer, "", -1);

}

#if 1
static gboolean ExitDetailWindow(gpointer data)
{
	PatDetail *tmp;
	tmp = (PatDetail *)data;
	tmp->DestroyPatDetailWin();
	return FALSE;
}

void PatDetail::KeyEvent(unsigned char keyValue)
{
	FakeXEvent::KeyEvent(keyValue);

	switch(keyValue)
	{
		case KEY_ESC:
			g_timeout_add(100, ExitDetailWindow, this);
			FakeEscKey();
			break;

		default:
			break;
	}
}
#endif
void PatDetail::DestroyPatDetailWin(void)
{
    if (m_window_detail)
    {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window_detail);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
    m_window_detail = NULL;
}

void PatDetail::ButtonQuitClicked(GtkButton *button)
{
    DestroyPatDetailWin();
}
