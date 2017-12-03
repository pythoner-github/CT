#include "ViewQueryRetrieve.h"
#include <glib.h>
#include <glib/gstdio.h>
#include <time.h>
#include "gui_global.h"
#include "gui_func.h"
#include "KeyValueOpr.h"
#include "ViewMain.h"
#include "DCMMan.h"
#include "ViewDialog.h"
#include "ImgMan.h"
#include "Database.h"
#include "ViewPrintPreview.h"
#include "VideoMan.h"
#include "SysOptions.h"
#include "Replay.h"
#include "SysGeneralSetting.h"
#include "ViewCalendar.h"
#include "KeyDef.h"
#include "ViewReport.h"
#include "Replay.h"
#include "ViewSystem.h"
using std::vector;
ViewQueryRetrieve* ViewQueryRetrieve::m_ptrInstance = NULL;
PatientInfo ViewQueryRetrieve::m_patientInfo;

ViewQueryRetrieve::ViewQueryRetrieve()
{
    SysGeneralSetting sys;
    m_dateFormat = sys.GetDateFormat();

    m_isSelect = false;
    QueryRes.clear();
    vecIndex.clear();
    m_imageNum = 0;
    m_count_failed =0;
    m_count_success = 0;
    m_retrieveFlag = false;
}

ViewQueryRetrieve::~ViewQueryRetrieve()
{
    if (m_ptrInstance != NULL) 
        delete m_ptrInstance; 
}

ViewQueryRetrieve* ViewQueryRetrieve::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ViewQueryRetrieve;

    return m_ptrInstance;
}
#ifndef VET
GtkWidget* ViewQueryRetrieve::CreateQueryRetrieveWin(GtkWidget *parent)
{
    GtkWidget *window_query_retrieve;
    GtkWidget *fixed_window;
    GtkWidget *frame_query;
    GtkWidget *label_query;
    GtkWidget *fixed;
    GtkWidget *label_ID;
    GtkWidget *label_name;
    GtkWidget *label_accession;
   // GtkWidget *label_gender;
   // GtkWidget *combobox_gender;
    GtkWidget *label_exam;
    GtkWidget *label_to; 
    GtkWidget *button_query; 
    GtkWidget *button_clear;
    GtkWidget *label_source;
    GtkWidget *label_destination;
    //GtkWidget *button_retrieve; 
    GtkWidget *button_quit; 
    
    GtkWidget *img_start_calendar;
    GtkWidget *eventbox_start_calendar;
    GtkWidget *img_end_calendar;
    GtkWidget *eventbox_end_calendar;
    GtkWidget *label_device;

    window_query_retrieve = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (window_query_retrieve, 1040, 768);
    gtk_window_set_title (GTK_WINDOW (window_query_retrieve), _("Query/Retrieve"));
    gtk_window_set_transient_for(GTK_WINDOW(window_query_retrieve), GTK_WINDOW(parent));
    gtk_window_set_modal(GTK_WINDOW(window_query_retrieve), TRUE);
    gtk_window_set_position (GTK_WINDOW (window_query_retrieve), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW (window_query_retrieve), FALSE);

    fixed_window = gtk_fixed_new ();
    gtk_widget_show (fixed_window);
    gtk_container_add (GTK_CONTAINER (window_query_retrieve), fixed_window);

    frame_query = gtk_frame_new (NULL);
    gtk_widget_show (frame_query);
    gtk_fixed_put (GTK_FIXED (fixed_window), frame_query, 5+15, 5);
    gtk_widget_set_size_request (frame_query, 780+210, 120);
    gtk_frame_set_label_align (GTK_FRAME (frame_query), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_query), GTK_SHADOW_IN);

    label_query = gtk_label_new (_("<b>Query</b>"));
    gtk_widget_show (label_query);
    gtk_frame_set_label_widget (GTK_FRAME (frame_query), label_query);
    gtk_label_set_use_markup (GTK_LABEL (label_query), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame_query), fixed);

    label_ID = gtk_label_new (_("<b>ID:</b>"));
    gtk_widget_show (label_ID);
    gtk_fixed_put (GTK_FIXED (fixed), label_ID, 0, 10);
    gtk_label_set_use_markup (GTK_LABEL (label_ID), TRUE);
    gtk_widget_set_size_request (label_ID, 80, 30);

    m_entry_ID = gtk_entry_new ();
    gtk_widget_show (m_entry_ID);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_ID, 80, 10);
    gtk_widget_set_size_request (m_entry_ID, 100+100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_ID), 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ID), 9679);

    label_name = gtk_label_new (_("<b>Name:</b>"));
    gtk_widget_show (label_name);
    gtk_fixed_put (GTK_FIXED (fixed), label_name, 200+80, 10);
    gtk_label_set_use_markup (GTK_LABEL (label_name), TRUE);
    gtk_widget_set_size_request (label_name, 100, 30);

    m_entry_name = gtk_entry_new ();
    gtk_widget_show (m_entry_name);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_name, 300+80, 10);
    gtk_widget_set_size_request (m_entry_name, 100+100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_name), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_name), 9679);

    label_accession = gtk_label_new (_("<b>Accession Number:</b>"));
    gtk_widget_show (label_accession);
    gtk_fixed_put (GTK_FIXED (fixed), label_accession, 400+180, 10);
    gtk_label_set_use_markup (GTK_LABEL (label_accession), TRUE);
    gtk_widget_set_size_request (label_accession, 200, 30);

    m_entry_accession = gtk_entry_new ();
    gtk_widget_show (m_entry_accession);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_accession, 780, 10);
    gtk_widget_set_size_request (m_entry_accession, 100+100, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_accession), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_accession), 9679);
#if 0  
    label_gender = gtk_label_new (_("<b>Gender:</b>"));
    gtk_widget_show (label_gender);
    gtk_fixed_put (GTK_FIXED (fixed), label_gender, 8, 50);
    gtk_label_set_use_markup (GTK_LABEL (label_gender), TRUE);
    gtk_widget_set_size_request (label_gender, 100, 30);

    combobox_gender = gtk_combo_box_new_text ();
    gtk_widget_show(combobox_gender); 
    gtk_fixed_put (GTK_FIXED (fixed),combobox_gender, 100,50 );
    gtk_widget_set_size_request (combobox_gender, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_gender), _("male"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_gender), _("female"));
#endif
    label_exam = gtk_label_new (_("<b>Exam Date :</b>"));
    gtk_widget_show (label_exam);
    gtk_fixed_put (GTK_FIXED (fixed), label_exam, 0, 60);
    gtk_label_set_use_markup (GTK_LABEL (label_exam), TRUE);
    gtk_widget_set_size_request (label_exam, 150, 30);

    m_entry_exam1 = gtk_entry_new ();
    gtk_widget_show (m_entry_exam1);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_exam1, 150, 60);
    gtk_widget_set_size_request (m_entry_exam1, 100+30, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_exam1), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_exam1), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_exam1),false);

    img_start_calendar = gtk_image_new_from_file ("./res/icon/Calendar.png");
    gtk_widget_show (img_start_calendar);
    eventbox_start_calendar = gtk_event_box_new ();
    gtk_widget_show (eventbox_start_calendar);
    gtk_widget_set_size_request (eventbox_start_calendar, 40, 30);
    gtk_container_add (GTK_CONTAINER (eventbox_start_calendar), img_start_calendar);
    gtk_fixed_put (GTK_FIXED (fixed), eventbox_start_calendar, 250+30, 60);
    g_signal_connect (G_OBJECT(eventbox_start_calendar), "button_press_event", G_CALLBACK(HandleStartCalendarPress), this);

    label_to = gtk_label_new (_("<b>To :</b>"));
    gtk_widget_show (label_to);
    gtk_fixed_put (GTK_FIXED (fixed), label_to, 250+60, 60);
    gtk_label_set_use_markup (GTK_LABEL (label_to), TRUE);
    gtk_widget_set_size_request (label_to, 100, 30);

    m_entry_exam2 = gtk_entry_new ();
    gtk_widget_show (m_entry_exam2);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_exam2, 400, 60);
    gtk_widget_set_size_request (m_entry_exam2, 100+30, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_exam2), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_exam2), 9679);
    gtk_editable_set_editable(GTK_EDITABLE(m_entry_exam2),false);

    img_end_calendar = gtk_image_new_from_file ("./res/icon/Calendar.png");
    gtk_widget_show (img_end_calendar);
    eventbox_end_calendar = gtk_event_box_new ();
    gtk_widget_show (eventbox_end_calendar);
    gtk_widget_set_size_request (eventbox_end_calendar, 40, 30);
    gtk_container_add (GTK_CONTAINER (eventbox_end_calendar), img_end_calendar);
    gtk_fixed_put (GTK_FIXED (fixed), eventbox_end_calendar, 530, 60);
    g_signal_connect (G_OBJECT(eventbox_end_calendar), "button_press_event", G_CALLBACK(HandleEndCalendarPress), this);

    button_query = gtk_button_new_with_mnemonic (_("Query"));
    gtk_widget_show (button_query);
    gtk_fixed_put (GTK_FIXED (fixed), button_query, 500+200, 60);
    gtk_widget_set_size_request (button_query, 100, 35);
    g_signal_connect(button_query, "clicked", G_CALLBACK(HandleButtonQueryClicked), this);

    button_clear = gtk_button_new_with_mnemonic (_("Clear"));
    gtk_widget_show (button_clear);
    gtk_fixed_put (GTK_FIXED (fixed), button_clear, 850, 60);
    gtk_widget_set_size_request (button_clear, 100, 35);
    g_signal_connect(button_clear, "clicked", G_CALLBACK(HandleButtonClearClicked), this);

    label_source = gtk_label_new (_("<b>Patient List(Source)</b>"));
    gtk_widget_show (label_source);
    gtk_fixed_put (GTK_FIXED (fixed), label_source, 10, 110);
    gtk_label_set_use_markup (GTK_LABEL (label_source), TRUE);
    gtk_widget_set_size_request (label_source, 200, 30);

    GtkWidget *scrollWin_source = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show (scrollWin_source);
    gtk_fixed_put (GTK_FIXED (fixed), scrollWin_source, 0, 140);
    gtk_widget_set_size_request (scrollWin_source, 780+210, 130+20+80);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollWin_source), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrollWin_source), GTK_SHADOW_IN);

    m_treeview_source = create_query_treeview();
	gtk_container_add(GTK_CONTAINER(scrollWin_source), m_treeview_source);
	gtk_widget_set_size_request (m_treeview_source, 780+210, 260+80);
    gtk_widget_show (m_treeview_source);
    
    m_checkbutton_select_all = gtk_check_button_new_with_mnemonic (_("Select All"));
    gtk_widget_show (m_checkbutton_select_all);
    gtk_fixed_put (GTK_FIXED (fixed), m_checkbutton_select_all, 10, 290+80);
    gtk_widget_set_size_request (m_checkbutton_select_all, 130, 50);
    g_signal_connect(G_OBJECT(m_checkbutton_select_all), "toggled", G_CALLBACK(HandleSelectAllToggled), this);

    label_device = gtk_label_new (_("<b>Destination :</b>"));
    gtk_widget_show (label_device);
    gtk_fixed_put (GTK_FIXED (fixed), label_device, 120, 300+80);
    gtk_label_set_use_markup (GTK_LABEL (label_device), TRUE);
    gtk_widget_set_size_request (label_device, 120, 30);

    m_combobox_device = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_device);
    gtk_fixed_put (GTK_FIXED (fixed), m_combobox_device, 220, 300+80);
    gtk_widget_set_size_request (m_combobox_device, 100, 30);

    vector<string> group_device_service = CDCMMan::GetMe()->GetAllRetrieveDestinationServerDevice();
    if(group_device_service.size()>0)
    {
        for(int i = 0;i<group_device_service.size();i++)
        {
            gtk_combo_box_insert_text (GTK_COMBO_BOX (m_combobox_device),i,group_device_service[i].c_str());
            if(strcmp(group_device_service[i].c_str(),"localhost")==0)
                gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_device),i);
        }
    }
    m_button_retrieve = gtk_button_new_with_mnemonic (_("Retrieve"));
    gtk_widget_show (m_button_retrieve);
    gtk_fixed_put (GTK_FIXED (fixed), m_button_retrieve, 360+20, 300+80);
    gtk_widget_set_size_request (m_button_retrieve, 85, 35);
    g_signal_connect(m_button_retrieve, "clicked", G_CALLBACK(HandleButtonRetrieveClicked), this);

    m_progress_bar = gtk_progress_bar_new();
    gtk_widget_hide(m_progress_bar);
    gtk_fixed_put(GTK_FIXED(fixed), m_progress_bar, 460+60, 300+80);
    gtk_widget_set_size_request(m_progress_bar, 250, 30);

    label_destination = gtk_label_new (_("<b>Patient List(Destination)</b>"));
    gtk_widget_show (label_destination);
    gtk_fixed_put (GTK_FIXED (fixed), label_destination, 10, 340+80);
    gtk_label_set_use_markup (GTK_LABEL (label_destination), TRUE);
    gtk_widget_set_size_request (label_destination, 200, 30);

    GtkWidget *scrollWin_destination = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show (scrollWin_destination);
    gtk_fixed_put (GTK_FIXED (fixed), scrollWin_destination, 0, 370+80);
    gtk_widget_set_size_request (scrollWin_destination, 780+210, 130+20+80);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollWin_destination), GTK_POLICY_ALWAYS , GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrollWin_destination), GTK_SHADOW_IN);

    m_treeview_detination = create_retrieve_treeview();
	gtk_container_add(GTK_CONTAINER(scrollWin_destination), m_treeview_detination);
	gtk_widget_set_size_request (m_treeview_detination, 780+210, 260+20);
    gtk_widget_show (m_treeview_detination);

    button_quit = gtk_button_new_with_mnemonic (_("Exit"));
    gtk_widget_show (button_quit);
    gtk_fixed_put (GTK_FIXED (fixed), button_quit, 630+150, 530+160);
    gtk_widget_set_size_request (button_quit, 85, 35);
    g_signal_connect(button_quit, "clicked", G_CALLBACK(HandleButtonQuitClicked), this);
    

    gtk_widget_set_sensitive(m_button_retrieve,false);
    gtk_widget_show_all(window_query_retrieve);

    m_window = window_query_retrieve;
    g_keyInterface.Push(this);
    SetSystemCursorToCenter();

    return window_query_retrieve;
}

void ViewQueryRetrieve::SetProgressBar(double fraction)
{
    PRINTF("---------SetProgressBar:%f\n",fraction);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(m_progress_bar), fraction);
    char text_buf[10];
    sprintf(text_buf, "%d%%", (int)(fraction*100));
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(m_progress_bar), text_buf);
    gtk_widget_show_all(m_progress_bar);
}

GtkWidget* ViewQueryRetrieve::create_query_treeview()
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
	column = gtk_tree_view_column_new_with_attributes ("", renderer, "active", COL_CHECKED_S, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 30, NULL);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("patient ID"), renderer, "text", COL_PATIENT_ID_S, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 150, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Name"), renderer, "text", COL_NAME_S, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 150, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Gender"), renderer, "text", COL_Gender_S, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Accession Number"), renderer, "text", COL_ACCESSION_NUMBER_S, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 150, NULL);
    
    renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes(_("Birth Date"), renderer, "text", COL_BIRTH_DATE_S, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 130, NULL);

    renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes(_("Exam Date"), renderer, "text", COL_EXAM_DATE_S, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 130, NULL);

    renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes(_("Exam Description"), renderer, "text", COL_EXAM_DESCRIPTION_S, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 200, NULL);

    model = create_query_model();
    if (model != NULL)
		gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    g_object_unref (model); 
    
	return treeview;
}

GtkTreeModel* ViewQueryRetrieve::create_query_model()
{
    GtkListStore *store;

    store = gtk_list_store_new(NUM_COLS_S, 
			G_TYPE_BOOLEAN, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING);

	return GTK_TREE_MODEL (store);
}
 
GtkWidget* ViewQueryRetrieve::create_retrieve_treeview()
{
    GtkWidget *treeview;
    GtkTreeModel *model = NULL;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    treeview = gtk_tree_view_new ();
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("patient ID"), renderer, "text", COL_PATIENT_ID_D, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 150, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Name"), renderer, "text", COL_NAME_D, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 150, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Gender"), renderer, "text", COL_Gender_D, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Age"), renderer, "text", COL_AGE_D, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Stature"), renderer, "text", COL_SIZE_D, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Weight"), renderer, "text", COL_WEIGHT_D, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);


    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Accession Number"), renderer, "text", COL_ACCESSION_NUMBER_D, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 150, NULL);
    
    renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes(_("Birth Date"), renderer, "text", COL_BIRTH_DATE_D, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 130, NULL);

    renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes(_("Exam Date"), renderer, "text", COL_EXAM_DATE_D, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 130, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Exam Time"), renderer, "text", COL_EXAM_TIME_D, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 130, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Exam Doctor"), renderer, "text", COL_EXAM_DOCTOR_D, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 130, NULL);

    renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes(_("Exam Description"), renderer, "text", COL_EXAM_DESCRIPTION_D, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 200, NULL);

    model = create_retrieve_model();
    if (model != NULL)
		gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    g_object_unref (model); 

	return treeview;
}

GtkTreeModel* ViewQueryRetrieve::create_retrieve_model()
{
    GtkListStore *store;

    store = gtk_list_store_new(NUM_COLS_D, 
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
			G_TYPE_STRING, 
			G_TYPE_STRING);

	return GTK_TREE_MODEL (store);
}    

void ViewQueryRetrieve::ToggleRecord(GtkCellRendererToggle *cell, gchar *path_str)
{
	GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(m_treeview_source));
	GtkTreeIter  iter;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
	gboolean checked;

	/* get toggled iter */
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter, COL_CHECKED_S, &checked, -1);

	/* do something with the value */
	checked ^= 1; //按位异或

	/* set new value */
    gtk_list_store_set (GTK_LIST_STORE (model), &iter, COL_CHECKED_S, checked, -1);

    gtk_widget_set_sensitive(m_button_retrieve,false);
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_device));
    bool exist = gtk_tree_model_get_iter_first(model, &iter);
    bool isSelect = false;
    while(exist)
    {
        gtk_tree_model_get(model, &iter, COL_CHECKED_S, &isSelect, -1);
        if (isSelect)
        {
            if(device != NULL)
                gtk_widget_set_sensitive(m_button_retrieve,true);
        }
        exist = gtk_tree_model_iter_next(model, &iter);
    }
	
    /* clean up */
	gtk_tree_path_free (path);
}

void ViewQueryRetrieve::StartCalendarPress(GtkWidget *widget, GdkEventButton *event)
{
	ViewCalendar::GetInstance()->CreateWindow(m_window, ViewCalendar::START, this);
}
#ifndef VET
void ViewQueryRetrieve::SetStartDate(int year, int month, int day)
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
    gtk_entry_set_text(GTK_ENTRY(m_entry_exam1),buf);

}

void ViewQueryRetrieve::EndCalendarPress(GtkWidget *widget, GdkEventButton *event)
{
	ViewCalendar::GetInstance()->CreateWindow(m_window, ViewCalendar::END, this);
}

void ViewQueryRetrieve::SetEndDate(int year, int month, int day)
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
    gtk_entry_set_text(GTK_ENTRY(m_entry_exam2),buf);
}
#endif
void ViewQueryRetrieve::ButtonQueryClicked(GtkButton *button)
{
    QueryRes.clear();
    const char *patientName = gtk_entry_get_text(GTK_ENTRY(m_entry_name));
    const char *patientID = gtk_entry_get_text(GTK_ENTRY(m_entry_ID));
    const char *accessionNumber = gtk_entry_get_text(GTK_ENTRY(m_entry_accession));
    //const char *startStudyDate = gtk_entry_get_text(GTK_ENTRY(m_entry_exam1));
    //const char *endStudyDate = gtk_entry_get_text(GTK_ENTRY(m_entry_exam2));
	char name[256] = {0};
	snprintf(name, 256, "*%s*", patientName);
	name[255] = '\0';
    EDCMReturnStatus status = CDCMMan::GetMe()->Query(name,patientID,accessionNumber,startDate,endDate,QueryRes);
    if(status == DCMSUCCESS)
    {
        GtkTreeModel *model;
        GtkTreeIter iter;

        model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_source));
        gtk_list_store_clear(GTK_LIST_STORE(model));

        for(int i= 0;i<QueryRes.size();i++)
        {
			string patientName;
			ChangePersonNameFormatForShow(QueryRes[i].qrPatientName, patientName);
            gtk_list_store_append(GTK_LIST_STORE(model), &iter);
            gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                    COL_CHECKED_S,FALSE,
                    COL_PATIENT_ID_S,QueryRes[i].qrPatientID.c_str(),
                    COL_NAME_S,patientName.c_str(),
                    COL_Gender_S,QueryRes[i].qrPatientSex.c_str(),
                    COL_ACCESSION_NUMBER_S,QueryRes[i].qrAccessionNumber.c_str(),
                    COL_BIRTH_DATE_S,QueryRes[i].qrPatientBirthDate.c_str(),
                    COL_EXAM_DATE_S,QueryRes[i].qrStudyDate.c_str(),
                    COL_EXAM_DESCRIPTION_S,QueryRes[i].qrStudyDescription.c_str(),
                    -1);
            gtk_tree_model_iter_next(model, &iter); 
        }
    }
    else if(status == DCMNODEFALUTSERVER)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                ViewDialog::INFO, 
                _("No default server!"),
                NULL);

    }
    else if(status == DCMCONNECTIONFAILURE)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                ViewDialog::INFO, 
                _("Server connection is failed!"),
                NULL);
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                ViewDialog::INFO, 
                _("Query unsuccessfully!"),
                NULL);
    }
}

void ViewQueryRetrieve::ButtonClearClicked(GtkButton *button)
{
    gtk_entry_set_text(GTK_ENTRY(m_entry_ID),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_name),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_accession),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_exam1),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_exam2),"");
}

unsigned int ViewQueryRetrieve::GetDCMStudyElement(DCMSTUDYELEMENT element)
{
    PatientInfo::Info info;
    info.p.id = element.stPatientID;
	string firstName, midName, lastName;
	ChangePersonNameFormat(element.stPatientName, firstName, lastName, midName);
    info.p.name.last = lastName;
    info.p.name.first = firstName;
    info.p.name.mid = midName;
    
    if(element.stPatientSex.c_str() !=NULL)
    {
        string sex = element.stPatientSex;
        if(strcmp(sex.c_str(),"F")==0)
            info.p.sex = 0;
        else if(strcmp(sex.c_str() , "M")==0)
            info.p.sex = 1;
        else if(strcmp(sex.c_str() , "O")==0)
            info.p.sex = 2;
        else if(sex.length()==0)
            info.p.sex = -1;
        else
            info.p.sex = -1;

    }
    else
        info.p.sex = -1;

    if(strlen(element.stPatientAge.c_str())!=0)
    {
        string age = element.stPatientAge;
        int age_len = strlen(age.c_str());
        char str[256]="\0";
        sprintf(str,"%c%c%c",age[0],age[1],age[2]);
        info.p.age = atoi(str);

        char str_tmp[2];
        sprintf(str_tmp,"%c",age[age_len-1]);
        if(strcmp(str_tmp,"Y")==0)
            info.p.ageUnit = 0;
        else if(strcmp(str_tmp,"M")==0)
            info.p.ageUnit = 1;
        else if(strcmp(str_tmp,"D")==0)
            info.p.ageUnit = 2;
    }
    else
    {
        info.p.age = 0;
        info.p.ageUnit = 0;
    }

    if(strlen(element.stPatientBirthDate.c_str())!=0)
    {
        string birthDate = element.stPatientBirthDate;
        char birth_year[100] = "\0";
        char birth_month[100] = "\0";
        char birth_day[100] = "\0";
        sprintf(birth_year,"%c%c%c%c",birthDate[0],birthDate[1],birthDate[2],birthDate[3]);
        sprintf(birth_month,"%c%c",birthDate[4],birthDate[5]);
        sprintf(birth_day,"%c%c",birthDate[6],birthDate[7]);
        info.p.birthDate.year = atoi(birth_year);
        info.p.birthDate.month = atoi(birth_month);
        info.p.birthDate.day = atoi(birth_day);
    }
    else
    {
        info.p.birthDate.year =0;
        info.p.birthDate.month = 0;
        info.p.birthDate.day = 0;
    }

    info.p.telephone ="";
    info.p.address = "";
    info.p.comment = "";

    //Size
    char sizeT[256];
    strcpy(sizeT,element.stPatientSize.c_str());
    DotToCommaLocaleNumeric(sizeT, sizeof(sizeT));

    //weight
    char weightT[256];
    strcpy(weightT,element.stPatientWeight.c_str());
    DotToCommaLocaleNumeric(weightT, sizeof(weightT));

    if(sizeT!= 0)
        info.e.height = int(atof(sizeT)*100);
    else
        info.e.height = 0.0;
    if(weightT!= 0)
        info.e.weight = atof(weightT);
    else
        info.e.weight = 0.0;

    info.e.BSA =0.0;
    if(strlen(element.stStudyDate.c_str())!=0)
    {
        string studyDate = element.stStudyDate;
        char exam_year[100] = "\0";
        char exam_month[100] = "\0";
        char exam_day[100] = "\0";

        sprintf(exam_year,"%c%c%c%c",studyDate[0],studyDate[1],studyDate[2],studyDate[3]);
        sprintf(exam_month,"%c%c",studyDate[4],studyDate[5]);
        sprintf(exam_day,"%c%c",studyDate[6],studyDate[7]);
        info.e.examDate.year = atoi(exam_year);
        info.e.examDate.month = atoi(exam_month);
        info.e.examDate.day = atoi(exam_day);
    }
    else
    {
        info.e.examDate.year = 0;
        info.e.examDate.month =0;
        info.e.examDate.day = 0;
    }

    if(strlen(element.stStudyTime.c_str())!=0)
    {
        string studyTime= element.stStudyTime;
        char exam_hour[100] = "\0";
        char exam_minute[100] = "\0";
        char exam_second[100] = "\0";
        
        sprintf(exam_hour,"%c%c",studyTime[0],studyTime[1]);
        sprintf(exam_minute,"%c%c",studyTime[2],studyTime[3]);
        sprintf(exam_second,"%c%c",studyTime[4],studyTime[5]);
        info.e.examTime.hour = atoi(exam_hour);
        info.e.examTime.second = atoi(exam_second);
        info.e.examTime.minute = atoi(exam_minute);
    }
    else
    {
        info.e.examTime.hour = 0;
        info.e.examTime.second = 0;
        info.e.examTime.minute = 0;
    }

	ChangePersonNameFormatForShow(element.stStudyDoctor, info.e.examDoctor);
    info.e.comment = element.stStudyDescription;
    info.e.reportDoctor = "";

    //ob
    info.ob.LMPDate.year = 0;
    info.ob.LMPDate.month = 0;
    info.ob.LMPDate.day = 0;

    info.ob.OvulateDate.year = 0;
    info.ob.OvulateDate.month = 0;
    info.ob.OvulateDate.day = 0;

    info.ob.pregCount = -1;
    info.ob.abnormalPregCount = -1;
    info.ob.fetusCount = -1;
    info.ob.deliveryCount = -1;
    info.ob.miscarryCount = -1;

    //CE
    info.car.HR = -1;
    info.car.pressureHigh = -1;
    info.car.pressureLow = -1;

    //URO
    info.uro.PSA = -1;

    m_ptrInstance->m_info = info;
    m_ptrInstance->m_studyNo= element.stStudyNo; 
#if 0
    string errmsg;
    string studyNo;
    Database db;
	if (db.ArchivePat(info,errmsg))
    {
		db.GetExamIDCurrent(info.e.examNum);
        printf("Achieve successfully!\n");
    }
    else
    {

        printf("Achieve unsuccessfully!\n");
    }
    m_patientInfo.SetInfo(info);
    m_ptrInstance->m_studyNo = atoi(info.e.examNum.c_str());
#endif
    Database db;
    db.GetExamIDCurrent(info.e.examNum);
    m_ptrInstance->m_studyNo = atoi(info.e.examNum.c_str())+1;

    return m_ptrInstance->m_studyNo;
}

void ViewQueryRetrieve::CreateExamDir(int examNum)
{
    gchar *path;
	gchar buf[100];

    char strExamNum[100];
    sprintf(strExamNum, "%d", examNum);
	path = g_build_path(G_DIR_SEPARATOR_S, STORE_PATH, strExamNum, NULL);
	if (g_access(path, F_OK) == 0)
	{
		PRINTF("-----------------------folder of store path %d is exist, path = %s\n", examNum, path);
		sprintf(buf, "rm -rf %s\n", path);
		FileMan fm;
		if (fm.DelDirectory(buf) == -1)
		{
			perror("rmdir fail!");
			PRINTF("delete folder of store path 0 error\n");
			g_free(path);	
			return;
		}
	}

	if (g_mkdir(path, 0755) == -1)
	{
		PRINTF("create folder of store path %d error\n", examNum);
	}
	
	g_free(path);

}

string ViewQueryRetrieve::GetDCMImageElement(DCMIMAGEELEMENT element,unsigned char *pImageBuf, unsigned long bufLen)
{
    m_ptrInstance->CreateExamDir(m_ptrInstance->m_studyNo);
    
    time_t at;
    at = time(&at);

    struct tm* ct;
    ct = localtime(&at);

    //char fileName[256];
    char *fileName = (char*)malloc(256);
    memset(fileName, 0, 15);
    sprintf(fileName, "%d%d%d%d%d%d%d%d%d%d%d%d", (ct->tm_year+1900), (ct->tm_mon+1)/10, (ct->tm_mon+1)%10, (ct->tm_mday)/10, (ct->tm_mday)%10, (ct->tm_hour)/10, (ct->tm_hour)%10, (ct->tm_min)/10, (ct->tm_min)%10, (ct->tm_sec)/10, (ct->tm_sec)%10, m_ptrInstance->m_imageNum);
    m_ptrInstance->m_imageNum++;
    if(element.imgFrameNumber>1)
    {
        SysOptions so;
        int format = so.GetCineFormat();

        m_ptrInstance->deq.clear();
        int videoFormat = so.GetCineFormat();
        VideoMan::VideoItem item;
        //item.path =(string)fileName;
        if(videoFormat == 0)
                item.format = VideoMan::AVI;
        else
            item.format = VideoMan::CINE;
        item.data = pImageBuf;

        for(int i = 0;i<element.imgFrameNumber;i++)
        {
            Image::AutoTracePara para;
            Image image((unsigned int*)pImageBuf, para);
            pImageBuf=pImageBuf+IMG_AREA_W*IMG_AREA_H*IMG_BPP;
            m_ptrInstance->deq.push_back(image);
        }
        item.deq = &(m_ptrInstance->deq);
        item.begin = 0;
        item.frames = element.imgFrameNumber;
        item.frame_rate = Replay::GetInstance()->GetImgFps();
        item.width = element.imgWidth;
        item.height = element.imgHeight;
        VideoMan::GetInstance()->SaveVideoForRetrieve(m_ptrInstance->m_studyNo, fileName, STORE_PATH, &item);
        m_ptrInstance->deq.clear();

        switch(format)
        {
            case VideoMan::CINE:
                strcat(fileName, ".cine");
                break;
            case VideoMan::AVI:
                strcat(fileName, ".avi");
                break;
            default:
                strcat(fileName, ".avi");
                break;
        }
    }
    else if (element.imgFrameNumber == 1)
    {
        SysOptions so;
        int format = so.GetImageFormat();

        ImgMan::ImgItem item;
        item.width = element.imgWidth;
        item.height = element.imgHeight;
        item.data = pImageBuf;
        ScanMode::SpecialMeasurePara para;
        ScanMode::GetInstance()->GetSpecialMeasurePara(&para);
        item.para = para;

        ImgMan::ImgItem item_frm;
        int format_bak = ImgMan::GetInstance()->GetImgFormat();

        if(ImgMan::GetInstance()->SaveSnapForRetrieve(m_ptrInstance->m_studyNo, fileName, STORE_PATH, &item) == 0)
        {
            ImgMan::GetInstance()->SetImgFormat(ImgMan::FRM);
            ImgMan::GetInstance()->SaveSnapForRetrieve(m_ptrInstance->m_studyNo, fileName, STORE_PATH, &item_frm);
            ImgMan::GetInstance()->SetImgFormat(format_bak);
        }
        switch(format)
        {
            case ImgMan::FRM:
                strcat(fileName, ".frm");
                break;
            case ImgMan::EMP:
                strcat(fileName, ".emp");
                break;
            case ImgMan::BMP:
                strcat(fileName, ".bmp");
                break;
            case ImgMan::JPEG:
                strcat(fileName, ".jpg");
                break;
            default:
                strcat(fileName, ".bmp");
                break;
        }
    }
    
    string part = element.imgStudyPart;
    if(strcmp(part.c_str(),"ABDOMEN") ==0)
        m_ptrInstance->m_info.e.examType = _("Abdomen");
    else if(strcmp(part.c_str(),"HEART") ==0)
        m_ptrInstance->m_info.e.examType = _("Cardiac");
    else if(strcmp(part.c_str(),"HIP") ==0)
        m_ptrInstance->m_info.e.examType = _("Hip Joint");
    else if(strcmp(part.c_str(),"BREAST") ==0)
        m_ptrInstance->m_info.e.examType = _("Mammary Glands");
    else 
        m_ptrInstance->m_info.e.examType = _("Abdomen");

    string imgFileName = (string)fileName;
    free(fileName);
    return imgFileName;
}

void ViewQueryRetrieve::GetDCMSRElement(DCMSRELEMENT element)
{
    m_ptrInstance->m_srIndication = element.srIndication;
    m_ptrInstance->m_srComments = element.srComments;
    m_ptrInstance->m_srReferenceImageFilename = element.srReferenceImageFilename;

#if 0
    gchar deststr[512];
    sprintf(deststr, "%s/%d", STORE_PATH, m_ptrInstance->m_studyNo);
    ViewPrintPreview::GetInstance()->CreatePagesForRetrieve(deststr,element.srIndication,element.srComments,element.srReferenceImageFilename);
#endif
}


void ViewQueryRetrieve::SetProgressPos(int nPos)
{
    gdouble frac;
    int size = m_ptrInstance->vecIndex.size();
    if(size >= 2)
    {
        gdouble x1 = m_ptrInstance->m_itemIndex/(gdouble)size;
        gdouble x2 = 1/(gdouble)size;
        frac = x1 + nPos*0.01*x2;
    }
    else
    {
        frac = nPos*0.01;
    }
    if(frac>= 0 && frac <= 1.0)
    {
        m_ptrInstance->SetProgressBar(frac);
        while (gtk_events_pending())
            gtk_main_iteration();
    }
}

void ViewQueryRetrieve::ChkSelectAllToggled(GtkToggleButton *togglebutton)
{
    if(QueryRes.size()==0)
        return;
    GtkTreeModel *model;
    gboolean exist;
    GtkTreeIter iter;
    m_isSelect = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_select_all));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_source));
    if(m_isSelect)
    {
        exist = gtk_tree_model_get_iter_first(model, &iter);
        while(exist)
        {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_CHECKED_S, TRUE, -1);
            exist = gtk_tree_model_iter_next(model, &iter);
        }
        gtk_widget_set_sensitive(m_button_retrieve,true);
    }
    else
    {
        gtk_widget_set_sensitive(m_button_retrieve,false);
        exist = gtk_tree_model_get_iter_first(model, &iter);
        while(exist)
        {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_CHECKED_S, false, -1);
            exist = gtk_tree_model_iter_next(model, &iter);
        }
    }
}


void ViewQueryRetrieve::ButtonRetrieveClicked(GtkButton *button)
{
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_device));

    vecIndex.clear();
    GtkTreeModel *model;
    gboolean exist;
    GtkTreeIter iter;
    gboolean isSelect;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_source));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        gtk_tree_model_get(model, &iter, COL_CHECKED_S, &isSelect, -1);
        if (isSelect)
        {
            GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
            char *path_string = gtk_tree_path_to_string(path);
            int path_num = atoi(path_string);
            int index = QueryRes[path_num].qrItemIndex;

            vecIndex.push_back(index);
        }
        exist = gtk_tree_model_iter_next(model, &iter);
    }

    SetProgressBar(0.0);
    while (gtk_events_pending())
        gtk_main_iteration();
    int size = vecIndex.size();
    if((size>0)&&(device != NULL))
    {
        Replay::GetInstance()->ClearCurReplayData();
        Replay::GetInstance()->DisplayReplayBar();

        for(int i=0;i<size;i++)
        {
            m_ptrInstance->m_itemIndex = i;
            InsertPatInfo(vecIndex[i],device,size);
        }
       if(size >1)
       {
        char info[256];
        sprintf(info, "%s\n%s %d\n%s %d", _("Retrieve finished!"), _("Success:"), m_count_success,  _("Fail:"), m_count_failed);

        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
            ViewDialog::INFO, 
            info,
            NULL);
        m_count_failed =0;
        m_count_success = 0;
       }
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                ViewDialog::INFO, 
                _("No record or device is selected!"),
                NULL);
        return;
    }
#if 0
    if(m_isSelect)
    {
        int size = m_ptrInstance->QueryRes.size();
        if((device != NULL)&&(size>0))
        {
            for(int i=0;i<size;i++)
            {
                m_ptrInstance->m_itemIndex = i;
                InsertPatInfo(i,(string)device);
            }
        }
        else
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                    ViewDialog::INFO, 
                    _("No record or server existed!"),
                    NULL);
            return;
        }
    }
    else
    {
        GtkTreeModel *model;
        gboolean exist;
        GtkTreeIter iter;
        gboolean isSelect;

        model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_source));
        exist = gtk_tree_model_get_iter_first(model, &iter);
        while(exist)
        {
            gtk_tree_model_get(model, &iter, COL_CHECKED, &isSelect, -1);
            if (isSelect)
            {
                GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
                char *path_string = gtk_tree_path_to_string(path);
                int path_num = atoi(path_string);
                int index = QueryRes[path_num].qrItemIndex;

                vecIndex.push_back(index);
            }
            exist = gtk_tree_model_iter_next(model, &iter);
        }

        int size = vecIndex.size();
        if((size>0)&&(device != NULL))
        {
            for(int i=0;i<size;i++)
            {
                InsertPatInfo(vecIndex[i],device);
            }
        }
        else
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                    ViewDialog::INFO, 
                    _("No record or device is selected!"),
                    NULL);
            return;
        }
    }
#endif
}

void ViewQueryRetrieve::InsertPatInfo(int index ,string device,int num)
{
   m_ptrInstance->m_srIndication = "";
   m_ptrInstance->m_srComments = "";
   m_ptrInstance->m_srReferenceImageFilename.clear();

   if(num ==1)
    {
        EDCMReturnStatus status = CDCMMan::GetMe()->Retrieve(index,(string)device,GetDCMStudyElement,GetDCMImageElement,GetDCMSRElement, SetProgressPos);
        if(status == DCMSUCCESS)
        {
            string errmsg;
            string studyNo;
            Database db;
            if (db.ArchivePat(m_ptrInstance->m_info,errmsg))
            {
                db.GetExamIDCurrent(m_ptrInstance->m_info.e.examNum);
                PRINTF("Achieve successfully!\n");
            }
            else
            {
                PRINTF("Achieve unsuccessfully!\n");
            }
            SetRetrieveFlag(true);
            m_patientInfo.SetInfoQA(m_ptrInstance->m_info);

            gchar deststr[512];
            sprintf(deststr, "%s/%d", STORE_PATH, m_ptrInstance->m_studyNo);

           // ViewPrintPreview *ptrVPrintPreview = ViewPrintPreview::GetInstance();
           // delete ptrVPrintPreview;

            ViewPrintPreview::GetInstance()->CreatePagesForRetrieve(deststr,m_ptrInstance->m_srIndication,m_ptrInstance->m_srComments,m_ptrInstance->m_srReferenceImageFilename);

          //  delete ViewPrintPreview::GetInstance();
            //m_patientInfo.ClearAll();
            ViewReport::GetInstance()->ClearIndicationandComments();

            GtkTreeModel *model;
            GtkTreeIter iter;

            int time = atoi(QueryRes[index].qrStudyTime.c_str());
            char studyTime[125]="\0";
            sprintf(studyTime,"%d",time);

			string patientName;
			ChangePersonNameFormatForShow(QueryRes[index].qrPatientName, patientName);
			string doctorName;
			ChangePersonNameFormatForShow(QueryRes[index].qrStudyDoctor, doctorName);

            //Size
            char sizeT[256];
            strcpy(sizeT,QueryRes[index].qrPatientSize.c_str());
            DotToCommaLocaleNumeric(sizeT, sizeof(sizeT));

            //weight
            char weightT[256];
            strcpy(weightT,QueryRes[index].qrPatientWeight.c_str());
            DotToCommaLocaleNumeric(weightT, sizeof(weightT));

            model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_detination));
            gtk_list_store_append(GTK_LIST_STORE(model), &iter);
            gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                    COL_PATIENT_ID_D,QueryRes[index].qrPatientID.c_str(),
                    COL_NAME_D,patientName.c_str(),
                    COL_Gender_D,QueryRes[index].qrPatientSex.c_str(),
                    COL_AGE_D,QueryRes[index].qrPatientAge.c_str(),
                    COL_SIZE_D,sizeT,
                    COL_WEIGHT_D,weightT,
                    COL_ACCESSION_NUMBER_D,QueryRes[index].qrAccessionNumber.c_str(),
                    COL_BIRTH_DATE_D,QueryRes[index].qrPatientBirthDate.c_str(),
                    COL_EXAM_DATE_D,QueryRes[index].qrStudyDate.c_str(),
                    COL_EXAM_TIME_D,studyTime,
                    COL_EXAM_DOCTOR_D,doctorName.c_str(),
                    COL_EXAM_DESCRIPTION_D,QueryRes[index].qrStudyDescription.c_str(),
                    -1);

            gtk_tree_model_iter_next(model, &iter); 
        }
        else if(status == DCMINVALIDQRDESTSERVER)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                    ViewDialog::INFO, 
                    _("Destination server is invalid!"),
                    NULL);
        }
        else if(status == DCMRETRIEVEFAILURE)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                    ViewDialog::INFO, 
                    _("Retrieve failedly!"),
                    NULL);
        }
        else if(status == DCMNONSUPPORTSTUDYROOT)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                    ViewDialog::INFO, 
                    _("Not support study root!"),
                    NULL);
        }
        else if(status == DCMNODEFALUTSERVER)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                    ViewDialog::INFO, 
                    _("No default server!"),
                    NULL);

        }
        else if(status == DCMCONNECTIONFAILURE)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                    ViewDialog::INFO, 
                    _("Server connection is failed!"),
                    NULL);
        }
        else if(status == DCMSTUDYEXISTED)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                    ViewDialog::INFO, 
                    _("Study has existed!"),
                    NULL);
        }
        else
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), 
                    ViewDialog::INFO, 
                    _("Retrieve unsuccessfully!"),
                    NULL);
        }
    }
    else
    {

        EDCMReturnStatus status = CDCMMan::GetMe()->Retrieve(index,(string)device,GetDCMStudyElement,GetDCMImageElement,GetDCMSRElement, SetProgressPos);
        if(status == DCMSUCCESS)
        {
            m_count_success++;
            string errmsg;
            string studyNo;
            Database db;
            if (db.ArchivePat(m_ptrInstance->m_info,errmsg))
            {
                db.GetExamIDCurrent(m_ptrInstance->m_info.e.examNum);
                PRINTF("Achieve successfully!\n");
            }
            else
            {
                PRINTF("Achieve unsuccessfully!\n");
            }
            SetRetrieveFlag(true);
            m_patientInfo.SetInfoQA(m_ptrInstance->m_info);

            gchar deststr[512];
            sprintf(deststr, "%s/%d", STORE_PATH, m_ptrInstance->m_studyNo);

            //ViewPrintPreview *ptrVPrintPreview = ViewPrintPreview::GetInstance();
            //delete ptrVPrintPreview;

            ViewPrintPreview::GetInstance()->CreatePagesForRetrieve(deststr,m_ptrInstance->m_srIndication,m_ptrInstance->m_srComments,m_ptrInstance->m_srReferenceImageFilename);

            //delete ViewPrintPreview::GetInstance();
            //m_patientInfo.ClearAll();
            ViewReport::GetInstance()->ClearIndicationandComments();

            GtkTreeModel *model;
            GtkTreeIter iter;

            int time = atoi(QueryRes[index].qrStudyTime.c_str());
            char studyTime[125]="\0";
            sprintf(studyTime,"%d",time);

			string patientName;
			ChangePersonNameFormatForShow(QueryRes[index].qrPatientName, patientName);
			string doctorName;
			ChangePersonNameFormatForShow(QueryRes[index].qrStudyDoctor, doctorName);
            //Size
            char sizeT[256];
            strcpy(sizeT,QueryRes[index].qrPatientSize.c_str());
            DotToCommaLocaleNumeric(sizeT, sizeof(sizeT));

            //weight
            char weightT[256];
            strcpy(weightT,QueryRes[index].qrPatientWeight.c_str());
            DotToCommaLocaleNumeric(weightT, sizeof(weightT));

            model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_detination));
            gtk_list_store_append(GTK_LIST_STORE(model), &iter);
            gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                    COL_PATIENT_ID_D,QueryRes[index].qrPatientID.c_str(),
                    COL_NAME_D,patientName.c_str(),
                    COL_Gender_D,QueryRes[index].qrPatientSex.c_str(),
                    COL_AGE_D,QueryRes[index].qrPatientAge.c_str(),
                    COL_SIZE_D,sizeT,
                    COL_WEIGHT_D,weightT,
                    COL_ACCESSION_NUMBER_D,QueryRes[index].qrAccessionNumber.c_str(),
                    COL_BIRTH_DATE_D,QueryRes[index].qrPatientBirthDate.c_str(),
                    COL_EXAM_DATE_D,QueryRes[index].qrStudyDate.c_str(),
                    COL_EXAM_TIME_D,studyTime,
                    COL_EXAM_DOCTOR_D,doctorName.c_str(),
                    COL_EXAM_DESCRIPTION_D,QueryRes[index].qrStudyDescription.c_str(),
                    -1);

            gtk_tree_model_iter_next(model, &iter); 
        }
        else
        {
            m_count_failed ++;
        }

    }
    m_imageNum = 0;
}

void ViewQueryRetrieve::SetRetrieveFlag(bool retrieveFlag)
{
    m_retrieveFlag = retrieveFlag;
}

bool ViewQueryRetrieve::GetRetrieveFlag(void)
{
    return m_retrieveFlag;
}

void ViewQueryRetrieve::ButtonQuitClicked(GtkButton *button)
{
    DestroyWin();
}

void ViewQueryRetrieve::DestroyWin(void)
{
    QueryRes.clear();
    vecIndex.clear(); 
    if (m_window)
    {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
    m_window = NULL;
}

static gboolean ExitWindow(gpointer data)
{
    ViewQueryRetrieve *tmp;
    tmp = (ViewQueryRetrieve *)data;
    tmp->DestroyWin();
    return FALSE;
}
#ifndef VET
void ViewQueryRetrieve::KeyEvent(unsigned char keyValue)
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
#endif
void ViewQueryRetrieve::ChangePersonNameFormat(string wlPersonName, string &firstName, string &lastName, string &midName)
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

void ViewQueryRetrieve::ChangePersonNameFormatForShow(string wlPersonName, string &name)
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
#endif
