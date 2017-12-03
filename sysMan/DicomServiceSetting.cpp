#include "DicomServiceSetting.h"
#include "gui_global.h"
#include "gui_func.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include "DCMMan.h"
#include "SysDicomSetting.h"
#include "ViewDialog.h"
#include "ViewSystem.h"
#include "ButtonCallBack.h"
 #include "DicomPrint.h"
//#include  "DicomSendPrinter.h"
vector<string> m_group_device_service;

const char *frames[6]={
    "10","20","50","100","200","300"
};
DicomServiceSetting* DicomServiceSetting::m_ptrInstance = NULL;

DicomServiceSetting::DicomServiceSetting()
{
    m_page_num = 0;
}

DicomServiceSetting::~DicomServiceSetting()
{
}

DicomServiceSetting* DicomServiceSetting::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new DicomServiceSetting;

    return m_ptrInstance;
}

GtkWidget* DicomServiceSetting::CreateDicomWindow(GtkWidget *parent)
{
    GtkWidget *fixed_service;
    GtkWidget *fixed_storage;
    GtkWidget *label_storage;
    GtkWidget *fixed_worklist;
    GtkWidget *label_worklist;

    GtkWidget *fixed_mpps;
    GtkWidget *label_mpps;
    GtkWidget *fixed_storage_commitment;
    GtkWidget *label_storage_commitment;
    GtkWidget *fixed_query_retrieve;
    GtkWidget *label_query_retrieve;
	GtkWidget *printFixed;
	GtkWidget *label_print;
	
    fixed_service = gtk_fixed_new ();
    gtk_widget_show (fixed_service);

    m_service_notebook = gtk_notebook_new ();
    gtk_fixed_put (GTK_FIXED (fixed_service), m_service_notebook, 18, 10-5);
    gtk_widget_set_size_request (m_service_notebook, 750, 470-15);
    // gtk_widget_set_size_request (m_notebook, w - 40, 540);
   // gtk_notebook_set_tab_pos(GTK_NOTEBOOK (m_dicom_notebook),GTK_POS_UP);
    gtk_widget_show (m_service_notebook);

    int service_notebook_page = 0; 

    // create note storage
    fixed_storage = Create_note_storage();
    gtk_container_add (GTK_CONTAINER (m_service_notebook), fixed_storage);

    label_storage = gtk_label_new (_("Storage"));
    gtk_widget_show (label_storage);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_service_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_service_notebook), service_notebook_page++), label_storage);
    
  // create note worklist
    fixed_worklist = Create_note_worklist();
    gtk_container_add (GTK_CONTAINER (m_service_notebook), fixed_worklist);

    label_worklist = gtk_label_new (_("Worklist"));
    gtk_widget_show (label_worklist);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_service_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_service_notebook), service_notebook_page++), label_worklist);

    // create note MPPS
    fixed_mpps = Create_note_mpps();
    gtk_container_add (GTK_CONTAINER (m_service_notebook), fixed_mpps);
  //  init_server_setting(NULL);

    label_mpps = gtk_label_new (_("MPPS"));
    gtk_widget_show (label_mpps);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_service_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_service_notebook), service_notebook_page++), label_mpps);
    
    // create note storage commitment 
    fixed_storage_commitment = Create_note_storage_commitment();
    gtk_container_add (GTK_CONTAINER (m_service_notebook), fixed_storage_commitment);
  //  init_server_setting(NULL);

    label_storage_commitment = gtk_label_new (_("Storage Commitment"));
    gtk_widget_show (label_storage_commitment);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_service_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_service_notebook), service_notebook_page++), label_storage_commitment);
 
// create note query/retrieve 
    fixed_query_retrieve = Create_note_query_retrieve();
    gtk_container_add (GTK_CONTAINER (m_service_notebook), fixed_query_retrieve);
  //  init_server_setting(NULL);

    label_query_retrieve = gtk_label_new (_(" Query/Retrieve"));
    gtk_widget_show (label_query_retrieve);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_service_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_service_notebook), service_notebook_page++), label_query_retrieve);

	printFixed = DicomPrint::GetInstance()->createNotePrint();
	gtk_container_add (GTK_CONTAINER (m_service_notebook), printFixed);

	label_print = gtk_label_new (_("Print"));
    gtk_widget_show (label_print);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_service_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_service_notebook), service_notebook_page++), label_print);

	
	
    g_signal_connect(G_OBJECT(m_service_notebook), "switch-page", G_CALLBACK(on_notebook_switch_page), this);

    return fixed_service;
}

void DicomServiceSetting::notebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num)
{
    m_page_num = page_num;
    switch(page_num)
    {
        case 0:
            init_storage_setting();
            break;
        case 1:
            init_worklist_setting();
            break;
        case 2:
            init_mpps_setting();
            break;
        case 3:
            init_storage_commitment_setting();
            break;
       case 4:
            init_query_retrieve_setting();
            break;
		case 5:
			DicomPrint::GetInstance()->init_print_setting();
        default:
            break;
    }
}

GtkWidget* DicomServiceSetting::Create_note_storage(void)
{
    GtkWidget *fixed_storage;
    GtkWidget *frame_equipment;
    GtkWidget *label_equipment;
    GtkWidget *fixed;
    GtkWidget *label_device;
    GtkWidget *label_name;
    GtkWidget *label_AE;
    GtkWidget *label_port;

    GtkWidget *button_storage_add;
    GtkWidget *button_storage_clear;

    GtkWidget *frame_service_list;
    GtkWidget *label_service_list;

    GtkWidget *fixed_list;

    GtkWidget *button_storage_delete;
    GtkWidget *button_storage_default;
    GtkWidget *button_storage_connect;

    GtkWidget *label_frames;

    fixed_storage = gtk_fixed_new ();
    gtk_widget_show (fixed_storage);

    frame_equipment = gtk_frame_new (NULL);
    gtk_widget_show (frame_equipment);
    gtk_fixed_put (GTK_FIXED (fixed_storage), frame_equipment, 0, 0);
    gtk_widget_set_size_request (frame_equipment, 750-10, 150);
    gtk_frame_set_label_align (GTK_FRAME (frame_equipment), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_equipment), GTK_SHADOW_IN);

    label_equipment = gtk_label_new (_("<b>Device Property</b>"));
    gtk_widget_show (label_equipment);
    gtk_frame_set_label_widget (GTK_FRAME (frame_equipment), label_equipment);
    gtk_label_set_use_markup (GTK_LABEL (label_equipment), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame_equipment), fixed);

    label_device = gtk_label_new (_("<b>Device :</b>"));
    gtk_widget_show (label_device);
    gtk_fixed_put (GTK_FIXED (fixed), label_device, 2, 20);
    gtk_label_set_use_markup (GTK_LABEL (label_device), TRUE);
    gtk_widget_set_size_request (label_device, 160, 30);

    m_combobox_device = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_device);
    gtk_fixed_put (GTK_FIXED (fixed), m_combobox_device, 160, 18);
    gtk_widget_set_size_request (m_combobox_device, 110, 30);
    g_signal_connect(G_OBJECT(m_combobox_device),"changed",G_CALLBACK(HandStorageDeviceChanged),this);
    
    label_name = gtk_label_new (_("<b>Service Name :</b>"));
    gtk_widget_show (label_name);
    gtk_fixed_put (GTK_FIXED (fixed), label_name, 2, 75);
    gtk_label_set_use_markup (GTK_LABEL (label_name), TRUE);
    gtk_widget_set_size_request (label_name, 160, 30);
    
    m_entry_name = gtk_entry_new ();
    gtk_widget_show (m_entry_name);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_name, 160, 75);
    gtk_widget_set_size_request (m_entry_name, 100+10, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_name), 45);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_name), 9679);

    label_AE = gtk_label_new (_("<b>AE Title :</b>"));
    gtk_widget_show (label_AE);
    gtk_fixed_put (GTK_FIXED (fixed), label_AE, 310-30, 20);
    gtk_label_set_use_markup (GTK_LABEL (label_AE), TRUE);
    gtk_widget_set_size_request (label_AE, 110, 30);
    
    m_entry_AE = gtk_entry_new ();
    gtk_widget_show (m_entry_AE);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_AE, 430-40, 20);
    gtk_widget_set_size_request (m_entry_AE, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_AE), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_AE), 9679);
    
    label_port = gtk_label_new (_("<b>Port :</b>"));
    gtk_widget_show (label_port);
    gtk_fixed_put (GTK_FIXED (fixed), label_port, 310-30, 75);
    gtk_label_set_use_markup (GTK_LABEL (label_port), TRUE);
    gtk_widget_set_size_request (label_port, 110, 30);

    m_entry_port = gtk_entry_new ();
    gtk_widget_show (m_entry_port);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_port, 430-40, 75);
    gtk_widget_set_size_request (m_entry_port, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_port), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_port), 9679);
    g_signal_connect(m_entry_port, "key-press-event", G_CALLBACK(HandleKeyFilter), this);

    button_storage_add = gtk_button_new_with_mnemonic (_("Add"));
    gtk_widget_show (button_storage_add);
    gtk_fixed_put (GTK_FIXED (fixed), button_storage_add, 580, 20);
    gtk_widget_set_size_request (button_storage_add, 110, 35);
    g_signal_connect(button_storage_add, "clicked", G_CALLBACK(HandleButtonStorageAddClicked), this);

    button_storage_clear = gtk_button_new_with_mnemonic (_("Clear"));
    gtk_widget_show (button_storage_clear);
    gtk_fixed_put (GTK_FIXED (fixed), button_storage_clear, 580, 75);
    gtk_widget_set_size_request (button_storage_clear, 110, 35);
    g_signal_connect(button_storage_clear, "clicked", G_CALLBACK(HandleButtonStorageClearClicked), this);

    frame_service_list = gtk_frame_new (NULL);
    gtk_widget_show (frame_service_list);
    gtk_fixed_put (GTK_FIXED (fixed_storage), frame_service_list, 0, 165);
    gtk_widget_set_size_request (frame_service_list, 750-10, 240-10);
    gtk_frame_set_label_align (GTK_FRAME (frame_service_list), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_service_list), GTK_SHADOW_IN);

    label_service_list = gtk_label_new (_("<b>Service List</b>"));
    gtk_widget_show (label_service_list);
    gtk_frame_set_label_widget (GTK_FRAME (frame_service_list), label_service_list);
    gtk_label_set_use_markup (GTK_LABEL (label_service_list), TRUE);

    fixed_list = gtk_fixed_new ();
    gtk_widget_show (fixed_list);
    gtk_container_add (GTK_CONTAINER (frame_service_list), fixed_list);

    GtkWidget *scrollWin = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show (scrollWin);
   // gtk_container_add (GTK_CONTAINER (frame_list), scrollWin);
    gtk_fixed_put (GTK_FIXED (fixed_list), scrollWin, 0, 0);
    //gtk_container_set_border_width (GTK_CONTAINER (scrollWin), 5);
    gtk_widget_set_size_request (scrollWin, 750-20, 170-10);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollWin), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrollWin), GTK_SHADOW_IN);

    m_treeview_storage = create_service_treeview();
	gtk_container_add(GTK_CONTAINER(scrollWin), m_treeview_storage);
	gtk_widget_set_size_request (m_treeview_storage, 250, 120);
    gtk_widget_show (m_treeview_storage);

    button_storage_delete = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (button_storage_delete);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_storage_delete, 30, 170);
    gtk_widget_set_size_request (button_storage_delete, 105, 35);
    g_signal_connect(button_storage_delete, "clicked", G_CALLBACK(HandleButtonStorageDeleteClicked), this);
    
    button_storage_default = gtk_button_new_with_mnemonic (_("Default"));
    gtk_widget_show (button_storage_default);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_storage_default, 150, 170);
    gtk_widget_set_size_request (button_storage_default, 105, 35);
    g_signal_connect(button_storage_default, "clicked", G_CALLBACK(HandleButtonStorageDefaultClicked), this);

    button_storage_connect = gtk_button_new_with_mnemonic (_("Connect Test"));
    gtk_widget_show (button_storage_connect);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_storage_connect, 270, 170);
#ifdef VET
 	 gtk_widget_set_size_request (button_storage_connect, 300, 35);
#else
    gtk_widget_set_size_request (button_storage_connect, 105+100, 35);
#endif
    g_signal_connect(button_storage_connect, "clicked", G_CALLBACK(HandleButtonStorageConnectClicked), this);

    m_checkbutton_report = gtk_check_button_new_with_mnemonic (_("Attach Structured Report"));
    gtk_widget_show (m_checkbutton_report);
    gtk_fixed_put (GTK_FIXED (fixed_storage), m_checkbutton_report, 20+5, 410-15);
    gtk_widget_set_size_request (m_checkbutton_report, 250, 30);
    g_signal_connect(G_OBJECT(m_checkbutton_report), "toggled", G_CALLBACK(HandleSendReportToggled), this);
    
    m_checkbutton_video = gtk_check_button_new_with_mnemonic (_("Allow Multiframe"));
    gtk_widget_show (m_checkbutton_video);
    gtk_fixed_put (GTK_FIXED (fixed_storage), m_checkbutton_video, 265+80-50, 410-15);
    gtk_widget_set_size_request (m_checkbutton_video, 150, 30);
    g_signal_connect(G_OBJECT(m_checkbutton_video), "toggled", G_CALLBACK(HandleSendVideoToggled), this);

    label_frames = gtk_label_new (_("Max number of frames:"));
    gtk_fixed_put (GTK_FIXED (fixed_storage), label_frames, 510-20-50, 395);
    gtk_label_set_use_markup (GTK_LABEL (label_frames), TRUE);
    gtk_widget_set_size_request (label_frames, 120+40+50, 30);
    gtk_misc_set_alignment(GTK_MISC(label_frames),0.9,0.5);
    gtk_widget_show (label_frames);

    m_combobox_video_frames = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_video_frames);
    gtk_fixed_put (GTK_FIXED (fixed_storage), m_combobox_video_frames, 620+30, 395);
    gtk_widget_set_size_request (m_combobox_video_frames, 100-20, 30);
    for(int i=0;i<=5;i++)
        gtk_combo_box_append_text(GTK_COMBO_BOX (m_combobox_video_frames),frames[i]);
    gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_video_frames),0);
    g_signal_connect(G_OBJECT(m_combobox_video_frames), "changed", G_CALLBACK(HandleComboVideoFrames), this); 



    return fixed_storage;
}

GtkWidget* DicomServiceSetting::Create_note_worklist(void)
{
    GtkWidget *fixed_worklist;
    GtkWidget *frame_worklist_equipment;
    GtkWidget *label_worklist_equipment;
    GtkWidget *fixed;
    GtkWidget *label_worklist_name;
    GtkWidget *label_worklist_device;
    GtkWidget *label_worklist_AE;
    GtkWidget *label_worklist_port;

    GtkWidget *button_worklist_add;
    GtkWidget *button_worklist_clear;

    GtkWidget *frame_worklist_list;
    GtkWidget *label_worklist_list;
    
    GtkWidget *fixed_list;

    GtkWidget *button_worklist_delete;
    GtkWidget *button_worklist_default;
    GtkWidget *button_worklist_connect;


    fixed_worklist = gtk_fixed_new ();
    gtk_widget_show (fixed_worklist);

    frame_worklist_equipment = gtk_frame_new (NULL);
    gtk_widget_show (frame_worklist_equipment);
    gtk_fixed_put (GTK_FIXED (fixed_worklist), frame_worklist_equipment, 0, 0);
    gtk_widget_set_size_request (frame_worklist_equipment, 750-10, 150);
    gtk_frame_set_label_align (GTK_FRAME (frame_worklist_equipment), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_worklist_equipment), GTK_SHADOW_IN);

    label_worklist_equipment = gtk_label_new (_("<b>Device Property</b>"));
    gtk_widget_show (label_worklist_equipment);
    gtk_frame_set_label_widget (GTK_FRAME (frame_worklist_equipment), label_worklist_equipment);
    gtk_label_set_use_markup (GTK_LABEL (label_worklist_equipment), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame_worklist_equipment), fixed);

    label_worklist_device = gtk_label_new (_("<b>Device :</b>"));
    gtk_widget_show (label_worklist_device);
    gtk_fixed_put (GTK_FIXED (fixed), label_worklist_device, 2, 20);
    gtk_label_set_use_markup (GTK_LABEL (label_worklist_device), TRUE);
    gtk_widget_set_size_request (label_worklist_device, 160, 30);
    
    m_combobox_worklist_device = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_worklist_device);
    gtk_fixed_put (GTK_FIXED (fixed), m_combobox_worklist_device, 160, 18);
    gtk_widget_set_size_request (m_combobox_worklist_device, 110, 30);
    g_signal_connect(G_OBJECT(m_combobox_worklist_device),"changed",G_CALLBACK(HandWorklistDeviceChanged),this);
    
    label_worklist_name = gtk_label_new (_("<b>Service Name :</b>"));
    gtk_widget_show (label_worklist_name);
    gtk_fixed_put (GTK_FIXED (fixed), label_worklist_name, 2, 75);
    gtk_label_set_use_markup (GTK_LABEL (label_worklist_name), TRUE);
    gtk_widget_set_size_request (label_worklist_name, 160, 30);
    
    m_entry_worklist_name = gtk_entry_new ();
    gtk_widget_show (m_entry_worklist_name);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_worklist_name, 160, 75);
    gtk_widget_set_size_request (m_entry_worklist_name, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_worklist_name), 45);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_worklist_name), 9679);

    label_worklist_AE = gtk_label_new (_("<b>AE Title :</b>"));
    gtk_widget_show (label_worklist_AE);
    gtk_fixed_put (GTK_FIXED (fixed), label_worklist_AE, 310-30, 20);
    gtk_label_set_use_markup (GTK_LABEL (label_worklist_AE), TRUE);
    gtk_widget_set_size_request (label_worklist_AE, 110, 30);
    
    m_entry_worklist_AE = gtk_entry_new ();
    gtk_widget_show (m_entry_worklist_AE);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_worklist_AE, 430-40, 20);
    gtk_widget_set_size_request (m_entry_worklist_AE, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_worklist_AE), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_worklist_AE), 9679);
    
    label_worklist_port = gtk_label_new (_("<b>Port :</b>"));
    gtk_widget_show (label_worklist_port);
    gtk_fixed_put (GTK_FIXED (fixed), label_worklist_port, 310-30, 75);
    gtk_label_set_use_markup (GTK_LABEL (label_worklist_port), TRUE);
    gtk_widget_set_size_request (label_worklist_port, 110, 30);

    m_entry_worklist_port = gtk_entry_new ();
    gtk_widget_show (m_entry_worklist_port);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_worklist_port, 430-40, 75);
    gtk_widget_set_size_request (m_entry_worklist_port, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_worklist_port), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_worklist_port), 9679);
    g_signal_connect(m_entry_worklist_port, "key-press-event", G_CALLBACK(HandleKeyFilter), this);

    button_worklist_add = gtk_button_new_with_mnemonic (_("Add"));
    gtk_widget_show (button_worklist_add);
    gtk_fixed_put (GTK_FIXED (fixed), button_worklist_add, 570+10, 20);
    gtk_widget_set_size_request (button_worklist_add, 110, 35);
    g_signal_connect(button_worklist_add, "clicked", G_CALLBACK(HandleButtonWorkListAddClicked), this);

    button_worklist_clear = gtk_button_new_with_mnemonic (_("Clear"));
    gtk_widget_show (button_worklist_clear);
    gtk_fixed_put (GTK_FIXED (fixed), button_worklist_clear, 570+10, 75);
    gtk_widget_set_size_request (button_worklist_clear, 110, 35);
    g_signal_connect(button_worklist_clear, "clicked", G_CALLBACK(HandleButtonWorkListClearClicked), this);

    frame_worklist_list = gtk_frame_new (NULL);
    gtk_widget_show (frame_worklist_list);
    gtk_fixed_put (GTK_FIXED (fixed_worklist), frame_worklist_list, 0, 165);
    gtk_widget_set_size_request (frame_worklist_list, 750-10, 240-10);
    gtk_frame_set_label_align (GTK_FRAME (frame_worklist_list), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_worklist_list), GTK_SHADOW_IN);

    label_worklist_list = gtk_label_new (_("<b>Service List</b>"));
    gtk_widget_show (label_worklist_list);
    gtk_frame_set_label_widget (GTK_FRAME (frame_worklist_list), label_worklist_list);
    gtk_label_set_use_markup (GTK_LABEL (label_worklist_list), TRUE);
 
    fixed_list = gtk_fixed_new ();
    gtk_widget_show (fixed_list);
    gtk_container_add (GTK_CONTAINER (frame_worklist_list), fixed_list);

    GtkWidget *scrollWin = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show (scrollWin);
    gtk_fixed_put (GTK_FIXED (fixed_list), scrollWin, 0, 0);
    gtk_widget_set_size_request (scrollWin, 750-20, 170-10);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollWin), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrollWin), GTK_SHADOW_IN);

    m_treeview_worklist = create_service_treeview();
	gtk_container_add(GTK_CONTAINER(scrollWin), m_treeview_worklist);
	gtk_widget_set_size_request (m_treeview_worklist, 250, 120);
    gtk_widget_show (m_treeview_worklist);

    button_worklist_delete = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (button_worklist_delete);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_worklist_delete, 30, 170);
    gtk_widget_set_size_request (button_worklist_delete, 105, 35);
    g_signal_connect(button_worklist_delete, "clicked", G_CALLBACK(HandleButtonWorkListDeleteClicked), this);

    button_worklist_default = gtk_button_new_with_mnemonic (_("Default"));
    gtk_widget_show (button_worklist_default);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_worklist_default, 150, 170);
    gtk_widget_set_size_request (button_worklist_default, 105, 35);
    g_signal_connect(button_worklist_default, "clicked", G_CALLBACK(HandleButtonWorkListDefaultClicked), this);

    button_worklist_connect = gtk_button_new_with_mnemonic (_("Connect Test"));
    gtk_widget_show (button_worklist_connect);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_worklist_connect, 270, 170);
#ifdef VET
 	 gtk_widget_set_size_request (button_worklist_connect, 195, 35);
#else
    gtk_widget_set_size_request (button_worklist_connect, 105+100, 35);
#endif
    g_signal_connect(button_worklist_connect, "clicked", G_CALLBACK(HandleButtonWorkListConnectClicked), this);

    m_checkbutton_auto_query = gtk_check_button_new_with_mnemonic (_("Auto Query"));
    gtk_widget_show (m_checkbutton_auto_query);
    gtk_fixed_put (GTK_FIXED (fixed_worklist), m_checkbutton_auto_query, 20+5, 410-15);
#ifdef VET
    gtk_widget_set_size_request (m_checkbutton_auto_query, 200, 30);
#else
    gtk_widget_set_size_request (m_checkbutton_auto_query, 100+50, 30);
#endif
    g_signal_connect(G_OBJECT(m_checkbutton_auto_query), "toggled", G_CALLBACK(HandleAutoQueryToggled), this);

    return fixed_worklist;
}

GtkWidget* DicomServiceSetting::Create_note_mpps(void)
{
    GtkWidget *fixed_mpps;
    GtkWidget *frame_device_quality;
    GtkWidget *label_device_quality;
    GtkWidget *fixed;
    GtkWidget *label_device;
    GtkWidget *label_mpps_name;
    GtkWidget *label_AE;
    GtkWidget *label_port;

    GtkWidget *button_mpps_add;
    GtkWidget *button_mpps_clear;

    GtkWidget *frame_service_list;
    GtkWidget *label_service_list;
    
    GtkWidget *fixed_list;

    GtkWidget *button_mpps_delete;
    GtkWidget *button_mpps_default;
    GtkWidget *button_mpps_connect;


    fixed_mpps = gtk_fixed_new ();
    gtk_widget_show (fixed_mpps);

    frame_device_quality = gtk_frame_new (NULL);
    gtk_widget_show (frame_device_quality);
    gtk_fixed_put (GTK_FIXED (fixed_mpps), frame_device_quality, 0, 0);
    gtk_widget_set_size_request (frame_device_quality, 750-10, 150);
    gtk_frame_set_label_align (GTK_FRAME (frame_device_quality), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_device_quality), GTK_SHADOW_IN);

    label_device_quality = gtk_label_new (_("<b>Device Property</b>"));
    gtk_widget_show (label_device_quality);
    gtk_frame_set_label_widget (GTK_FRAME (frame_device_quality), label_device_quality);
    gtk_label_set_use_markup (GTK_LABEL (label_device_quality), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame_device_quality), fixed);

    label_device = gtk_label_new (_("<b>Device :</b>"));
    gtk_widget_show (label_device);
    gtk_fixed_put (GTK_FIXED (fixed), label_device, 2, 20);
    gtk_label_set_use_markup (GTK_LABEL (label_device), TRUE);
    gtk_widget_set_size_request (label_device, 160, 30);

    m_combobox_mpps_device = gtk_combo_box_new_text();
    gtk_widget_show (m_combobox_mpps_device);
    gtk_fixed_put (GTK_FIXED (fixed), m_combobox_mpps_device, 160, 18);
    gtk_widget_set_size_request (m_combobox_mpps_device, 110, 30);
    g_signal_connect(G_OBJECT(m_combobox_mpps_device),"changed",G_CALLBACK(HandMPPSDeviceChanged),this);

    label_mpps_name = gtk_label_new (_("<b>Service Name :</b>"));
    gtk_widget_show (label_mpps_name);
    gtk_fixed_put (GTK_FIXED (fixed), label_mpps_name, 2, 75);
    gtk_label_set_use_markup (GTK_LABEL (label_mpps_name), TRUE);
    gtk_widget_set_size_request (label_mpps_name, 160, 30);
    
    m_entry_mpps_name = gtk_entry_new ();
    gtk_widget_show (m_entry_mpps_name);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_mpps_name, 160, 75);
    gtk_widget_set_size_request (m_entry_mpps_name, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_mpps_name), 45);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_mpps_name), 9679);

    label_AE = gtk_label_new (_("<b>AE Title :</b>"));
    gtk_widget_show (label_AE);
    gtk_fixed_put (GTK_FIXED (fixed), label_AE, 310-30, 20);
    gtk_label_set_use_markup (GTK_LABEL (label_AE), TRUE);
    gtk_widget_set_size_request (label_AE, 110, 30);
    
    m_entry_mpps_AE = gtk_entry_new ();
    gtk_widget_show (m_entry_mpps_AE);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_mpps_AE, 430-40, 20);
    gtk_widget_set_size_request (m_entry_mpps_AE, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_mpps_AE), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_mpps_AE), 9679);
    
    label_port = gtk_label_new (_("<b>Port :</b>"));
    gtk_widget_show (label_port);
    gtk_fixed_put (GTK_FIXED (fixed), label_port, 310-30, 75);
    gtk_label_set_use_markup (GTK_LABEL (label_port), TRUE);
    gtk_widget_set_size_request (label_port, 110, 30);

    m_entry_mpps_port = gtk_entry_new ();
    gtk_widget_show (m_entry_mpps_port);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_mpps_port, 430-40, 75);
    gtk_widget_set_size_request (m_entry_mpps_port, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_mpps_port), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_mpps_port), 9679);

    button_mpps_add = gtk_button_new_with_mnemonic (_("Add"));
    gtk_widget_show (button_mpps_add);
    gtk_fixed_put (GTK_FIXED (fixed), button_mpps_add, 580, 20);
    gtk_widget_set_size_request (button_mpps_add, 110, 35);
    g_signal_connect(button_mpps_add, "clicked", G_CALLBACK(HandleButtonMPPSAddClicked), this);

    button_mpps_clear = gtk_button_new_with_mnemonic (_("Clear"));
    gtk_widget_show (button_mpps_clear);
    gtk_fixed_put (GTK_FIXED (fixed), button_mpps_clear, 580, 75);
    gtk_widget_set_size_request (button_mpps_clear, 110, 35);
    g_signal_connect(button_mpps_clear, "clicked", G_CALLBACK(HandleButtonMPPSClearClicked), this);

    frame_service_list = gtk_frame_new (NULL);
    gtk_widget_show (frame_service_list);
    gtk_fixed_put (GTK_FIXED (fixed_mpps), frame_service_list, 0, 165);
    gtk_widget_set_size_request (frame_service_list, 750-10, 240-10);
    gtk_frame_set_label_align (GTK_FRAME (frame_service_list), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_service_list), GTK_SHADOW_IN);

    label_service_list = gtk_label_new (_("<b>Service List</b>"));
    gtk_widget_show (label_service_list);
    gtk_frame_set_label_widget (GTK_FRAME (frame_service_list), label_service_list);
    gtk_label_set_use_markup (GTK_LABEL (label_service_list), TRUE);
 
    fixed_list = gtk_fixed_new ();
    gtk_widget_show (fixed_list);
    gtk_container_add (GTK_CONTAINER (frame_service_list), fixed_list);

    GtkWidget *scrollWin = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show (scrollWin);
    gtk_fixed_put (GTK_FIXED (fixed_list), scrollWin, 0, 0);
    gtk_widget_set_size_request (scrollWin, 750-20, 170-10);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollWin), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrollWin), GTK_SHADOW_IN);

    m_treeview_mpps = create_service_treeview();
	gtk_container_add(GTK_CONTAINER(scrollWin), m_treeview_mpps);
	gtk_widget_set_size_request (m_treeview_mpps, 250, 120);
    gtk_widget_show (m_treeview_mpps);

    button_mpps_delete = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (button_mpps_delete);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_mpps_delete, 30, 170);
    gtk_widget_set_size_request (button_mpps_delete, 105, 35);
    g_signal_connect(button_mpps_delete, "clicked", G_CALLBACK(HandleButtonMPPSDeleteClicked), this);

    button_mpps_default = gtk_button_new_with_mnemonic (_("Default"));
    gtk_widget_show (button_mpps_default);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_mpps_default, 150, 170);
    gtk_widget_set_size_request (button_mpps_default, 85+20, 35);
    g_signal_connect(button_mpps_default, "clicked", G_CALLBACK(HandleButtonMPPSDefaultClicked), this);

    button_mpps_connect = gtk_button_new_with_mnemonic (_("Connect Test"));
    gtk_widget_show (button_mpps_connect);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_mpps_connect, 270, 170);
    gtk_widget_set_size_request (button_mpps_connect, 105+100, 35);
    g_signal_connect(button_mpps_connect, "clicked", G_CALLBACK(HandleButtonMPPSConnectClicked), this);

    m_checkbutton_mpps = gtk_check_button_new_with_mnemonic (_("Send MPPS after start exam and end exam"));
    gtk_widget_show (m_checkbutton_mpps);
    gtk_fixed_put (GTK_FIXED (fixed_mpps), m_checkbutton_mpps, 25, 395);
    gtk_widget_set_size_request (m_checkbutton_mpps, 300+200, 30);
    g_signal_connect(G_OBJECT(m_checkbutton_mpps), "toggled", G_CALLBACK(HandleMPPSToggled), this);

    return fixed_mpps;

}

GtkWidget* DicomServiceSetting::Create_note_storage_commitment(void)
{
    GtkWidget *fixed_storage_commitment;
    GtkWidget *frame_device_quality;
    GtkWidget *label_device_quality;
    GtkWidget *fixed;
    GtkWidget *label_device;
    GtkWidget *label_name;
    GtkWidget *label_AE;
    GtkWidget *label_port;

    GtkWidget *button_storage_commitment_add;
    GtkWidget *button_storage_commitment;

    GtkWidget *frame_service_list;
    GtkWidget *label_service_list;
    
    GtkWidget *fixed_list;

    GtkWidget *button_storage_commitment_delete;
    GtkWidget *button_storage_commitment_default;
    GtkWidget *button_storage_commitment_connect;


    fixed_storage_commitment = gtk_fixed_new ();
    gtk_widget_show (fixed_storage_commitment);

    frame_device_quality = gtk_frame_new (NULL);
    gtk_widget_show (frame_device_quality);
    gtk_fixed_put (GTK_FIXED (fixed_storage_commitment), frame_device_quality, 0, 0);
    gtk_widget_set_size_request (frame_device_quality, 750-10, 150);
    gtk_frame_set_label_align (GTK_FRAME (frame_device_quality), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_device_quality), GTK_SHADOW_IN);

    label_device_quality = gtk_label_new (_("<b>Device Property</b>"));
    gtk_widget_show (label_device_quality);
    gtk_frame_set_label_widget (GTK_FRAME (frame_device_quality), label_device_quality);
    gtk_label_set_use_markup (GTK_LABEL (label_device_quality), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame_device_quality), fixed);

    label_device = gtk_label_new (_("<b>Device :</b>"));
    gtk_widget_show (label_device);
    gtk_fixed_put (GTK_FIXED (fixed), label_device, 2, 20);
    gtk_label_set_use_markup (GTK_LABEL (label_device), TRUE);
    gtk_widget_set_size_request (label_device, 160, 30);

    m_combobox_storage_commitment_device = gtk_combo_box_new_text();
    gtk_widget_show (m_combobox_storage_commitment_device);
    gtk_fixed_put (GTK_FIXED (fixed), m_combobox_storage_commitment_device, 160, 18);
    gtk_widget_set_size_request (m_combobox_storage_commitment_device, 110, 30);
    g_signal_connect(G_OBJECT(m_combobox_storage_commitment_device),"changed",G_CALLBACK(HandStorageCommitmentDeviceChanged),this);

    label_name = gtk_label_new (_("<b>Service Name :</b>"));
    gtk_widget_show (label_name);
    gtk_fixed_put (GTK_FIXED (fixed), label_name, 2, 75);
    gtk_label_set_use_markup (GTK_LABEL (label_name), TRUE);
    gtk_widget_set_size_request (label_name, 160, 30);
    
    m_entry_storage_commitment_name = gtk_entry_new ();
    gtk_widget_show (m_entry_storage_commitment_name);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_storage_commitment_name, 160, 75);
    gtk_widget_set_size_request (m_entry_storage_commitment_name, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_storage_commitment_name), 45);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_storage_commitment_name), 9679);

    label_AE = gtk_label_new (_("<b>AE Title :</b>"));
    gtk_widget_show (label_AE);
    gtk_fixed_put (GTK_FIXED (fixed), label_AE, 310-30, 20);
    gtk_label_set_use_markup (GTK_LABEL (label_AE), TRUE);
    gtk_widget_set_size_request (label_AE, 110, 30);
    
    m_entry_storage_commitment_AE = gtk_entry_new ();
    gtk_widget_show (m_entry_storage_commitment_AE);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_storage_commitment_AE, 430-40, 20);
    gtk_widget_set_size_request (m_entry_storage_commitment_AE, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_storage_commitment_AE), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_storage_commitment_AE), 9679);
    
    label_port = gtk_label_new (_("<b>Port :</b>"));
    gtk_widget_show (label_port);
    gtk_fixed_put (GTK_FIXED (fixed), label_port, 310-30, 75);
    gtk_label_set_use_markup (GTK_LABEL (label_port), TRUE);
    gtk_widget_set_size_request (label_port, 110, 30);

    m_entry_storage_commitment_port = gtk_entry_new ();
    gtk_widget_show (m_entry_storage_commitment_port);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_storage_commitment_port, 430-40, 75);
    gtk_widget_set_size_request (m_entry_storage_commitment_port, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_storage_commitment_port), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_storage_commitment_port), 9679);

    button_storage_commitment_add = gtk_button_new_with_mnemonic (_("Add"));
    gtk_widget_show (button_storage_commitment_add);
    gtk_fixed_put (GTK_FIXED (fixed), button_storage_commitment_add, 580, 20);
    gtk_widget_set_size_request (button_storage_commitment_add, 110, 35);
    g_signal_connect(button_storage_commitment_add, "clicked", G_CALLBACK(HandleButtonStorageCommitmentAddClicked), this);

    button_storage_commitment = gtk_button_new_with_mnemonic (_("Clear"));
    gtk_widget_show (button_storage_commitment);
    gtk_fixed_put (GTK_FIXED (fixed), button_storage_commitment, 580, 75);
    gtk_widget_set_size_request (button_storage_commitment, 110, 35);
    g_signal_connect(button_storage_commitment, "clicked", G_CALLBACK(HandleButtonStorageCommitmentClearClicked), this);

    frame_service_list = gtk_frame_new (NULL);
    gtk_widget_show (frame_service_list);
    gtk_fixed_put (GTK_FIXED (fixed_storage_commitment), frame_service_list, 0, 165);
    gtk_widget_set_size_request (frame_service_list, 750-10, 240-10);
    gtk_frame_set_label_align (GTK_FRAME (frame_service_list), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_service_list), GTK_SHADOW_IN);

    label_service_list = gtk_label_new (_("<b>Service List</b>"));
    gtk_widget_show (label_service_list);
    gtk_frame_set_label_widget (GTK_FRAME (frame_service_list), label_service_list);
    gtk_label_set_use_markup (GTK_LABEL (label_service_list), TRUE);
 
    fixed_list = gtk_fixed_new ();
    gtk_widget_show (fixed_list);
    gtk_container_add (GTK_CONTAINER (frame_service_list), fixed_list);

    GtkWidget *scrollWin = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show (scrollWin);
    gtk_fixed_put (GTK_FIXED (fixed_list), scrollWin, 0, 0);
    gtk_widget_set_size_request (scrollWin, 750-20, 170-10);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollWin), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrollWin), GTK_SHADOW_IN);

    m_treeview_storage_commitment = create_service_treeview();
	gtk_container_add(GTK_CONTAINER(scrollWin), m_treeview_storage_commitment);
	gtk_widget_set_size_request (m_treeview_storage_commitment, 250, 120);
    gtk_widget_show (m_treeview_storage_commitment);

    button_storage_commitment_delete = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (button_storage_commitment_delete);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_storage_commitment_delete, 30, 170);
    gtk_widget_set_size_request (button_storage_commitment_delete, 85+20, 35);
    g_signal_connect(button_storage_commitment_delete, "clicked", G_CALLBACK(HandleButtonStorageCommitmentDeleteClicked), this);

    button_storage_commitment_default = gtk_button_new_with_mnemonic (_("Default"));
    gtk_widget_show (button_storage_commitment_default);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_storage_commitment_default, 150, 170);
    gtk_widget_set_size_request (button_storage_commitment_default, 85+20, 35);
    g_signal_connect(button_storage_commitment_default, "clicked", G_CALLBACK(HandleButtonStorageCommitmentDefaultClicked), this);

    button_storage_commitment_connect = gtk_button_new_with_mnemonic (_("Connect Test"));
    gtk_widget_show (button_storage_commitment_connect);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_storage_commitment_connect, 270, 170);
    gtk_widget_set_size_request (button_storage_commitment_connect, 105+100, 35);
    g_signal_connect(button_storage_commitment_connect, "clicked", G_CALLBACK(HandleButtonStorageCommitmentConnectClicked), this);

    m_checkbutton_storage_commitment = gtk_check_button_new_with_mnemonic (_("Send storage commitment after storage"));
    gtk_widget_show (m_checkbutton_storage_commitment);
    gtk_fixed_put (GTK_FIXED (fixed_storage_commitment), m_checkbutton_storage_commitment, 25, 395);
    gtk_widget_set_size_request (m_checkbutton_storage_commitment, 300+200, 30);
    g_signal_connect(G_OBJECT(m_checkbutton_storage_commitment), "toggled", G_CALLBACK(HandleStorageCommitmentToggled), this);


    return fixed_storage_commitment;
}

GtkWidget* DicomServiceSetting::Create_note_query_retrieve(void)
{
    GtkWidget *fixed_query_retrieve;
    GtkWidget *frame_device_quality;
    GtkWidget *label_device_quality;
    GtkWidget *fixed;
    GtkWidget *label_device;
    GtkWidget *label_query_retrieve_name;
    GtkWidget *label_AE;
    GtkWidget *label_port;

    GtkWidget *button_query_retrieve_add;
    GtkWidget *button_query_retrieve_clear;

    GtkWidget *frame_service_list;
    GtkWidget *label_service_list;
    
    GtkWidget *fixed_list;

    GtkWidget *button_query_retrieve_delete;
    GtkWidget *button_query_retrieve_default;
    GtkWidget *button_query_retrieve_connect;


    fixed_query_retrieve = gtk_fixed_new ();
    gtk_widget_show (fixed_query_retrieve);

    frame_device_quality = gtk_frame_new (NULL);
    gtk_widget_show (frame_device_quality);
    gtk_fixed_put (GTK_FIXED (fixed_query_retrieve), frame_device_quality, 0, 0);
    gtk_widget_set_size_request (frame_device_quality, 750-10, 150);
    gtk_frame_set_label_align (GTK_FRAME (frame_device_quality), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_device_quality), GTK_SHADOW_IN);

    label_device_quality = gtk_label_new (_("<b>Device Property</b>"));
    gtk_widget_show (label_device_quality);
    gtk_frame_set_label_widget (GTK_FRAME (frame_device_quality), label_device_quality);
    gtk_label_set_use_markup (GTK_LABEL (label_device_quality), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame_device_quality), fixed);

    label_device = gtk_label_new (_("<b>Device :</b>"));
    gtk_widget_show (label_device);
    gtk_fixed_put (GTK_FIXED (fixed), label_device, 2, 20);
    gtk_label_set_use_markup (GTK_LABEL (label_device), TRUE);
    gtk_widget_set_size_request (label_device, 160, 30);

    m_combobox_query_retrieve_device = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_query_retrieve_device);
    gtk_fixed_put (GTK_FIXED (fixed), m_combobox_query_retrieve_device, 160, 18);
    gtk_widget_set_size_request (m_combobox_query_retrieve_device, 110, 30);
    g_signal_connect(G_OBJECT(m_combobox_query_retrieve_device),"changed",G_CALLBACK(HandQueryRetrieveDeviceChanged),this);

    label_query_retrieve_name = gtk_label_new (_("<b>Service Name :</b>"));
    gtk_widget_show (label_query_retrieve_name);
    gtk_fixed_put (GTK_FIXED (fixed), label_query_retrieve_name, 2, 75);
    gtk_label_set_use_markup (GTK_LABEL (label_query_retrieve_name), TRUE);
    gtk_widget_set_size_request (label_query_retrieve_name, 160, 30);
    
    m_entry_query_retrieve_name = gtk_entry_new ();
    gtk_widget_show (m_entry_query_retrieve_name);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_query_retrieve_name, 160, 75);
    gtk_widget_set_size_request (m_entry_query_retrieve_name, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_query_retrieve_name), 45);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_query_retrieve_name), 9679);

    label_AE = gtk_label_new (_("<b>AE Title :</b>"));
    gtk_widget_show (label_AE);
    gtk_fixed_put (GTK_FIXED (fixed), label_AE, 310-30, 20);
    gtk_label_set_use_markup (GTK_LABEL (label_AE), TRUE);
    gtk_widget_set_size_request (label_AE, 110, 30);
    
    m_entry_query_retrieve_AE = gtk_entry_new ();
    gtk_widget_show (m_entry_query_retrieve_AE);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_query_retrieve_AE, 430-40, 20);
    gtk_widget_set_size_request (m_entry_query_retrieve_AE, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_query_retrieve_AE), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_query_retrieve_AE), 9679);
    
    label_port = gtk_label_new (_("<b>Port :</b>"));
    gtk_widget_show (label_port);
    gtk_fixed_put (GTK_FIXED (fixed), label_port, 310-30, 75);
    gtk_label_set_use_markup (GTK_LABEL (label_port), TRUE);
    gtk_widget_set_size_request (label_port, 110, 30);

    m_entry_query_retrieve_port = gtk_entry_new ();
    gtk_widget_show (m_entry_query_retrieve_port);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_query_retrieve_port, 430-40, 75);
    gtk_widget_set_size_request (m_entry_query_retrieve_port, 110, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_query_retrieve_port), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_query_retrieve_port), 9679);

    button_query_retrieve_add = gtk_button_new_with_mnemonic (_("Add"));
    gtk_widget_show (button_query_retrieve_add);
    gtk_fixed_put (GTK_FIXED (fixed), button_query_retrieve_add, 580, 20);
    gtk_widget_set_size_request (button_query_retrieve_add, 110, 35);
    g_signal_connect(button_query_retrieve_add, "clicked", G_CALLBACK(HandleButtonQueryRetrieveAddClicked), this);

    button_query_retrieve_clear = gtk_button_new_with_mnemonic (_("Clear"));
    gtk_widget_show (button_query_retrieve_clear);
    gtk_fixed_put (GTK_FIXED (fixed), button_query_retrieve_clear, 580, 75);
    gtk_widget_set_size_request (button_query_retrieve_clear, 110, 35);
    g_signal_connect(button_query_retrieve_clear, "clicked", G_CALLBACK(HandleButtonQueryRetrieveClearClicked), this);

    frame_service_list = gtk_frame_new (NULL);
    gtk_widget_show (frame_service_list);
    gtk_fixed_put (GTK_FIXED (fixed_query_retrieve), frame_service_list, 0, 165);
    gtk_widget_set_size_request (frame_service_list, 750-10, 240-10);
    gtk_frame_set_label_align (GTK_FRAME (frame_service_list), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_service_list), GTK_SHADOW_IN);

    label_service_list = gtk_label_new (_("<b>Service List</b>"));
    gtk_widget_show (label_service_list);
    gtk_frame_set_label_widget (GTK_FRAME (frame_service_list), label_service_list);
    gtk_label_set_use_markup (GTK_LABEL (label_service_list), TRUE);
 
    fixed_list = gtk_fixed_new ();
    gtk_widget_show (fixed_list);
    gtk_container_add (GTK_CONTAINER (frame_service_list), fixed_list);

    GtkWidget *scrollWin = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show (scrollWin);
    gtk_fixed_put (GTK_FIXED (fixed_list), scrollWin, 0, 0);
    gtk_widget_set_size_request (scrollWin, 750-20, 170-10);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollWin), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrollWin), GTK_SHADOW_IN);

    m_treeview_query_retrieve = create_service_treeview();
	gtk_container_add(GTK_CONTAINER(scrollWin), m_treeview_query_retrieve);
	gtk_widget_set_size_request (m_treeview_query_retrieve, 250, 120);
    gtk_widget_show (m_treeview_query_retrieve);

    button_query_retrieve_delete = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (button_query_retrieve_delete);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_query_retrieve_delete, 30, 170);
    gtk_widget_set_size_request (button_query_retrieve_delete, 85+20, 35);
    g_signal_connect(button_query_retrieve_delete, "clicked", G_CALLBACK(HandleButtonQueryRetrieveDeleteClicked), this);

    button_query_retrieve_default = gtk_button_new_with_mnemonic (_("Default"));
    gtk_widget_show (button_query_retrieve_default);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_query_retrieve_default, 150, 170);
    gtk_widget_set_size_request (button_query_retrieve_default, 85+20, 35);
    g_signal_connect(button_query_retrieve_default, "clicked", G_CALLBACK(HandleButtonQueryRetrieveDefaultClicked), this);

    button_query_retrieve_connect = gtk_button_new_with_mnemonic (_("Connect Test"));
    gtk_widget_show (button_query_retrieve_connect);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_query_retrieve_connect, 270, 170);
    gtk_widget_set_size_request (button_query_retrieve_connect, 105+100, 35);
    g_signal_connect(button_query_retrieve_connect, "clicked", G_CALLBACK(HandleButtonQueryRetrieveConnectClicked), this);

    return fixed_query_retrieve;
}


GtkWidget* DicomServiceSetting::create_service_treeview()
{
    GtkWidget *treeview;
    GtkTreeModel *model = NULL;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    treeview = gtk_tree_view_new ();
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Device"), renderer, "text", COL_STORAGE_DEVICE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Service Name"), renderer, "text", COL_SERVICE_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 200, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("AE Title"), renderer, "text", COL_AE_TITLE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Port"), renderer, "text", COL_PORT, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Default"), renderer, "text", COL_DEFAULT, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);

    model = create_service_model();
    if (model != NULL)
		gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    g_object_unref (model); 

	return treeview;
}

GtkTreeModel* DicomServiceSetting::create_service_model()
{
    GtkListStore *store;

    store = gtk_list_store_new(NUM_COLS, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING, 
			G_TYPE_STRING);

	return GTK_TREE_MODEL (store);
}

void DicomServiceSetting::GetSingleServiceAttribute(string device, string serviceName,string aeTitle, int port, bool isDefault,void *data)
{
    char tmp_port[256];
    char tmp_isDefault[20];
    char tmp_device[256];
    sprintf(tmp_port,"%d",port);
    sprintf(tmp_device,"%s",device.c_str());
    
    if (isDefault)
        sprintf(tmp_isDefault,"%s",_("Yes"));
    else
        sprintf(tmp_isDefault,"%s"," ");
  
    DicomServiceSetting *tmp;
    tmp = (DicomServiceSetting *)data;
    
    //printf("---------GetSingleServiceAttribute:%s %d\n",device.c_str(),tmp->m_page_num);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gboolean exist = FALSE;
    char *tmp_device1;
    
    switch(tmp->m_page_num)
    {
        case 0:
            {
                model = gtk_tree_view_get_model(GTK_TREE_VIEW(tmp->m_treeview_storage));
                exist = gtk_tree_model_get_iter_first(model, &iter);
                while(exist)
                {
                    gtk_tree_model_get(model,&iter,
                            COL_STORAGE_DEVICE,&tmp_device1,
                            -1);

                    if (strcmp(tmp_device,tmp_device1)==0)
                    {
                        //printf("Add failed\n");
                        return;
                    }
                    exist = gtk_tree_model_iter_next(model, &iter); 
                }
                gtk_list_store_append(GTK_LIST_STORE(model), &iter);
                gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                        COL_STORAGE_DEVICE, device.c_str(),
                            COL_SERVICE_NAME,serviceName.c_str(),
                            COL_AE_TITLE,aeTitle.c_str(),
                            COL_PORT,tmp_port,
                            COL_DEFAULT,tmp_isDefault,
                            -1);

                    gtk_tree_model_iter_next(model, &iter);
            }   

            break;
        case 1:
            {
                model = gtk_tree_view_get_model(GTK_TREE_VIEW(tmp->m_treeview_worklist));
                exist = gtk_tree_model_get_iter_first(model, &iter);
                while(exist)
                {
                    gtk_tree_model_get(model,&iter,
                            COL_STORAGE_DEVICE,&tmp_device1,
                            -1);

                    if (strcmp(tmp_device,tmp_device1)==0)
                    {
                        //printf("Add failed\n");
                        return;
                    }
                    exist = gtk_tree_model_iter_next(model, &iter); 
                }
                gtk_list_store_append(GTK_LIST_STORE(model), &iter);
                gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                        COL_STORAGE_DEVICE, device.c_str(),
                            COL_SERVICE_NAME,serviceName.c_str(),
                            COL_AE_TITLE,aeTitle.c_str(),
                            COL_PORT,tmp_port,
                            COL_DEFAULT,tmp_isDefault,
                            -1);

                    gtk_tree_model_iter_next(model, &iter);
            }   
            break;
        case 2:
            {
                model = gtk_tree_view_get_model(GTK_TREE_VIEW(tmp->m_treeview_mpps));
                exist = gtk_tree_model_get_iter_first(model, &iter);
                while(exist)
                {
                    gtk_tree_model_get(model,&iter,
                            COL_STORAGE_DEVICE,&tmp_device1,
                            -1);

                    if (strcmp(tmp_device,tmp_device1)==0)
                    {
                        //printf("Add failed\n");
                        return;
                    }
                    exist = gtk_tree_model_iter_next(model, &iter); 
                }
                gtk_list_store_append(GTK_LIST_STORE(model), &iter);
                gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                        COL_STORAGE_DEVICE, device.c_str(),
                            COL_SERVICE_NAME,serviceName.c_str(),
                            COL_AE_TITLE,aeTitle.c_str(),
                            COL_PORT,tmp_port,
                            COL_DEFAULT,tmp_isDefault,
                            -1);

                    gtk_tree_model_iter_next(model, &iter);
            }   
            break;
        case 3:
            {
                model = gtk_tree_view_get_model(GTK_TREE_VIEW(tmp->m_treeview_storage_commitment));
                exist = gtk_tree_model_get_iter_first(model, &iter);
                while(exist)
                {
                    gtk_tree_model_get(model,&iter,
                            COL_STORAGE_DEVICE,&tmp_device1,
                            -1);

                    if (strcmp(tmp_device,tmp_device1)==0)
                    {
                        //printf("Add failed\n");
                        return;
                    }
                    exist = gtk_tree_model_iter_next(model, &iter); 
                }
                gtk_list_store_append(GTK_LIST_STORE(model), &iter);
                gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                        COL_STORAGE_DEVICE, device.c_str(),
                            COL_SERVICE_NAME,serviceName.c_str(),
                            COL_AE_TITLE,aeTitle.c_str(),
                            COL_PORT,tmp_port,
                            COL_DEFAULT,tmp_isDefault,
                            -1);

                    gtk_tree_model_iter_next(model, &iter);
            }   
            break;
        case 4:
            {
                model = gtk_tree_view_get_model(GTK_TREE_VIEW(tmp->m_treeview_query_retrieve));
                exist = gtk_tree_model_get_iter_first(model, &iter);
                while(exist)
                {
                    gtk_tree_model_get(model,&iter,
                            COL_STORAGE_DEVICE,&tmp_device1,
                            -1);

                    if (strcmp(tmp_device,tmp_device1)==0)
                    {
                        //printf("Add failed\n");
                        return;
                    }
                    exist = gtk_tree_model_iter_next(model, &iter); 
                }
                gtk_list_store_append(GTK_LIST_STORE(model), &iter);
                gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                        COL_STORAGE_DEVICE, device.c_str(),
                        COL_SERVICE_NAME,serviceName.c_str(),
                        COL_AE_TITLE,aeTitle.c_str(),
                        COL_PORT,tmp_port,
                        COL_DEFAULT,tmp_isDefault,
                        -1);

                gtk_tree_model_iter_next(model, &iter);
            }   
            break;

        default:
            break;

    }
}

void DicomServiceSetting::init_service_setting()
{
    if (m_page_num == 1)
        init_worklist_setting();
    gtk_notebook_set_current_page(GTK_NOTEBOOK (m_service_notebook),1);       
}

void DicomServiceSetting::init_storage_setting()
{
    vector<string> group_device_service = CDCMMan::GetMe()->GetAllDevice();
    bool exist = false;
    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_combo_box_get_model(GTK_COMBO_BOX (m_combobox_device));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        char *path_string = gtk_tree_model_get_string_from_iter(model,&iter);
        int path_num = atoi(path_string);

        exist = gtk_tree_model_iter_next(model, &iter); 
        gtk_combo_box_remove_text (GTK_COMBO_BOX (m_combobox_device),path_num);
    }

    for(int i = 0;i<group_device_service.size();i++)
    {
        gtk_combo_box_insert_text (GTK_COMBO_BOX (m_combobox_device),i,group_device_service[i].c_str());
    }
    SysDicomSetting sysDicomSetting;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON( m_checkbutton_report),sysDicomSetting.GetSendReport());
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON( m_checkbutton_video),sysDicomSetting.GetSendVideo());
    int frames = sysDicomSetting.GetVideoFrames();
    switch(frames)
    {
        case 10:
            gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_video_frames),0);
            break;
        case 20:
            gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_video_frames),1);
            break;
        case 50:
            gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_video_frames),2);
            break;
        case 100:
            gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_video_frames),3);
            break;
        case 200:
            gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_video_frames),4);
            break;
        case 300:
            gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_video_frames),6);
            break;
        default:
            gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_video_frames),0);
            break;
    }
    CDCMMan::GetMe()->GetAllStorageService(GetSingleServiceAttribute, this);
}

void DicomServiceSetting::init_worklist_setting()
{
    vector<string> group_device_service = CDCMMan::GetMe()->GetAllDevice();
    bool exist = false;
    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_combo_box_get_model(GTK_COMBO_BOX (m_combobox_worklist_device));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        char *path_string = gtk_tree_model_get_string_from_iter(model,&iter);
        int path_num = atoi(path_string);

        exist = gtk_tree_model_iter_next(model, &iter); 
        gtk_combo_box_remove_text (GTK_COMBO_BOX (m_combobox_worklist_device),path_num);
    }

    for(int i = 0;i<group_device_service.size();i++)
    {
        gtk_combo_box_insert_text (GTK_COMBO_BOX (m_combobox_worklist_device),i,group_device_service[i].c_str());
    }

    SysDicomSetting sysDicomSetting;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON( m_checkbutton_auto_query),sysDicomSetting.GetAutoQuery());
    CDCMMan::GetMe()->GetAllWorklistService(GetSingleServiceAttribute, this);
}

void DicomServiceSetting::init_mpps_setting()
{
    vector<string> group_device_service = CDCMMan::GetMe()->GetAllDevice();
    bool exist = false;
    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_combo_box_get_model(GTK_COMBO_BOX (m_combobox_mpps_device));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        char *path_string = gtk_tree_model_get_string_from_iter(model,&iter);
        int path_num = atoi(path_string);

        exist = gtk_tree_model_iter_next(model, &iter); 
        gtk_combo_box_remove_text (GTK_COMBO_BOX (m_combobox_mpps_device),path_num);
    }

    for(int i = 0;i<group_device_service.size();i++)
    {
        gtk_combo_box_insert_text (GTK_COMBO_BOX (m_combobox_mpps_device),i,group_device_service[i].c_str());
    }

    SysDicomSetting sysDicomSetting;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON( m_checkbutton_mpps),sysDicomSetting.GetMPPS());
    CDCMMan::GetMe()->GetAllMPPSService(GetSingleServiceAttribute, this);
}

void DicomServiceSetting::init_storage_commitment_setting()
{
    vector<string> group_device_service = CDCMMan::GetMe()->GetAllDevice();
    bool exist = false;
    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_combo_box_get_model(GTK_COMBO_BOX (m_combobox_storage_commitment_device));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        char *path_string = gtk_tree_model_get_string_from_iter(model,&iter);
        int path_num = atoi(path_string);

        exist = gtk_tree_model_iter_next(model, &iter); 
        gtk_combo_box_remove_text (GTK_COMBO_BOX (m_combobox_storage_commitment_device),path_num);
    }

    for(int i = 0;i<group_device_service.size();i++)
    {
        gtk_combo_box_insert_text (GTK_COMBO_BOX (m_combobox_storage_commitment_device),i,group_device_service[i].c_str());
    }

    SysDicomSetting sysDicomSetting;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_storage_commitment),sysDicomSetting.GetStorageCommitment());
    CDCMMan::GetMe()->GetAllStorageCommitService(GetSingleServiceAttribute, this);
}

void DicomServiceSetting::init_query_retrieve_setting()
{
    vector<string> group_device_service = CDCMMan::GetMe()->GetAllDevice();
    bool exist = false;
    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_combo_box_get_model(GTK_COMBO_BOX (m_combobox_query_retrieve_device));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        char *path_string = gtk_tree_model_get_string_from_iter(model,&iter);
        int path_num = atoi(path_string);

        exist = gtk_tree_model_iter_next(model, &iter); 
        gtk_combo_box_remove_text (GTK_COMBO_BOX (m_combobox_query_retrieve_device),path_num);
    }

    for(int i = 0;i<group_device_service.size();i++)
    {
        gtk_combo_box_insert_text (GTK_COMBO_BOX (m_combobox_query_retrieve_device),i,group_device_service[i].c_str());
    }

    CDCMMan::GetMe()->GetAllQueryRetrieveService(GetSingleServiceAttribute, this);
}

void DicomServiceSetting::StorageDeviceChanged(GtkComboBox *combobox)
{
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_device));
    if(device != NULL)
    {
        
        char tmp_serviceName[256]="\0"; 
        
            sprintf(tmp_serviceName,"%s",device);
            strcat(tmp_serviceName,"-");
            strcat(tmp_serviceName,_("Storage"));
        
        gtk_entry_set_text(GTK_ENTRY(m_entry_name),tmp_serviceName);
    }
}

void DicomServiceSetting::ButtonStorageAddClicked(GtkButton *button)
{
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_device));
    const char *ae = gtk_entry_get_text(GTK_ENTRY(m_entry_AE));
    const char *port = gtk_entry_get_text(GTK_ENTRY(m_entry_port));
    

    if(ae[0] == '\0' ||port[0] == '\0'||device == NULL)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("Device ,AE or Port is empty, please input!"),
                NULL);
        return;
    }

    char tmp_serviceName[256];
    
        sprintf(tmp_serviceName,"%s",device);
        strcat(tmp_serviceName,"-");
        strcat(tmp_serviceName,_("Storage"));
   
    char device_tmp0[256];
    char *device_tmp1;
    sprintf(device_tmp0,"%s",device);

    

    GtkTreeModel *model;
    GtkTreeIter iter;
    gboolean exist = FALSE;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_storage));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device_tmp1,
                -1);

        if(device_tmp1 == NULL)
            return;

        if (strcmp(device_tmp0,device_tmp1)==0)
        {
            //printf("Add failed\n");
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                    ViewDialog::INFO, 
                    _("Add failed,device has been existed\n"),
                    NULL);

            g_free(device);
            return;
        }

        exist = gtk_tree_model_iter_next(model, &iter); 
    }
    if(!CDCMMan::GetMe()->AddStorageService(device, tmp_serviceName, ae, atoi(port)))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("Add failed,device has been existed\n"),
                NULL);

        g_free(device);
        return;
    }
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_STORAGE_DEVICE, device,
            COL_SERVICE_NAME,tmp_serviceName,
            COL_AE_TITLE,ae,
            COL_PORT,port,
            -1);
   // m_group_device_service.push_back(device);
    gtk_tree_model_iter_next(model, &iter); 
    g_free(device);

}

void DicomServiceSetting::ButtonStorageDeleteClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *device;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_storage));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_storage));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
        CDCMMan::GetMe()->DeleteStorageService(device);

        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
}

void DicomServiceSetting::ButtonStorageClearClicked(GtkButton *button)
{
   // gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_device));
   // gtk_combo_box_remove_text(GTK_COMBO_BOX(m_combobox_device),index);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_device),-1);
    gtk_entry_set_text(GTK_ENTRY(m_entry_AE),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_name),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_port),"");
}

void DicomServiceSetting::ButtonStorageDefaultClicked(GtkButton *button)
{  
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeIter iter0;
    GtkTreeSelection *selection;
    gboolean exist = false;
    char *device;
    char *isDefault;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_storage));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_storage));
    
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        exist = gtk_tree_model_get_iter_first(model, &iter0);
        while(exist)
        {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter0,
                    COL_DEFAULT, " ",
                    -1);

            exist = gtk_tree_model_iter_next(model, &iter0); 
        }
        gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                COL_DEFAULT, _("Yes"),
                -1);

        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
        CDCMMan::GetMe()->SetDefaultStorageService(device);
    }
    else
    {

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("No device is selected!"),
                NULL);
    }
}

void DicomServiceSetting::ButtonStorageConnectClicked(GtkButton *button)
{
    
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *device;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_storage));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_storage));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
        
        m_selectedDevice = device;

        ViewHintDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                _("Connect..."));

        g_timeout_add(2000, StorageConnectTimeout, this);
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("No device is selected!"),
                NULL);

    }
}

gboolean DicomServiceSetting::StorageConnectTimeout(gpointer data)
{
    DicomServiceSetting *pdata = (DicomServiceSetting *)data;
    
    char info[256]= "\0";
    if(CDCMMan::GetMe()->TestLinkStorage(pdata->m_selectedDevice))
        {
            PRINTF("Dicom test link OK!!\n");
            sprintf(info, _("Connection test successfully!"));
        }
        else
        {
            PRINTF("Dicom test link FAIL!!\n");
            sprintf(info, _("Connection test fails!"));
        }
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                info,
                NULL);
}

void DicomServiceSetting::ChkBtnSendReportToggled(GtkToggleButton *togglebutton)
{
    SysDicomSetting sysDicomSetting;
    sysDicomSetting.SetSendReport(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton)));
    sysDicomSetting.SyncFile();
}

void DicomServiceSetting::ChkBtnSendVideoToggled(GtkToggleButton *togglebutton)
{
    SysDicomSetting sysDicomSetting;
    sysDicomSetting.SetSendVideo(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton)));
    sysDicomSetting.SyncFile();
}

void DicomServiceSetting::BtnComboboxVideoFrames(GtkComboBox *combobox)
{
    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
    SysDicomSetting sysDicomSetting;
    sysDicomSetting.SetVideoFrames(atoi(frames[index]));
    sysDicomSetting.SyncFile();

}

void DicomServiceSetting::WorklistDeviceChanged(GtkComboBox *combobox)
{
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_worklist_device));
    if(device != NULL)
    {
        
        char tmp_serviceName[256]="\0"; 
        
        
            sprintf(tmp_serviceName,"%s",device);
            strcat(tmp_serviceName,"-");
            strcat(tmp_serviceName,_("Worklist"));
        
        gtk_entry_set_text(GTK_ENTRY(m_entry_worklist_name),tmp_serviceName);
    }
}

void DicomServiceSetting::ButtonWorkListAddClicked(GtkButton *button)
{
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_worklist_device));
    const char *ae = gtk_entry_get_text(GTK_ENTRY(m_entry_worklist_AE));
    const char *port = gtk_entry_get_text(GTK_ENTRY(m_entry_worklist_port));
    
    if(ae[0] == '\0' ||port[0] == '\0'||device == NULL)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("Device ,AE or Port is empty, please input!"),
                NULL);
        return;
    }

    char tmp_serviceName[256];
    
        sprintf(tmp_serviceName,"%s",device);
        strcat(tmp_serviceName,"-");
        strcat(tmp_serviceName,_("Worklist"));

    char device_tmp0[256];
    char *device_tmp1;
    sprintf(device_tmp0,"%s",device);
    
    GtkTreeModel *model;
    GtkTreeIter iter;
    gboolean exist = FALSE;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device_tmp1,
                -1);

        if(device_tmp1 == NULL)
            return;

        if (strcmp(device_tmp0,device_tmp1)==0)
        {
            //printf("Add failed\n");
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                    ViewDialog::INFO, 
                    _("Add failed,device has been existed\n"),
                    NULL);

            g_free(device);
            return;
        }

        exist = gtk_tree_model_iter_next(model, &iter); 
    }

    if(!CDCMMan::GetMe()->AddWorklistService(device, tmp_serviceName, ae, atoi(port)))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("Add failed,device has been existed\n"),
                NULL);

        g_free(device);
        return;
    }
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_STORAGE_DEVICE, device,
            COL_SERVICE_NAME,tmp_serviceName,
            COL_AE_TITLE,ae,
            COL_PORT,port,
            -1);

   // m_group_device_service.push_back(device);
    gtk_tree_model_iter_next(model, &iter); 

    g_free(device);
}

void DicomServiceSetting::ButtonWorkListClearClicked(GtkButton *button)
{
    //gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_worklist_device));
    //gtk_combo_box_remove_text(GTK_COMBO_BOX(m_combobox_worklist_device),index);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_worklist_device),-1);
    gtk_entry_set_text(GTK_ENTRY(m_entry_worklist_name),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_worklist_AE),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_worklist_port),"");

}

void DicomServiceSetting::ButtonWorkListDeleteClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;

    char *device;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_worklist));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
	
        CDCMMan::GetMe()->DeleteWorklistService(device);
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
    else
    {

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            _("No device is selected!"),
            NULL);
    }
}

void DicomServiceSetting::ButtonWorkListDefaultClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeIter iter0;
    GtkTreeSelection *selection;
    gboolean exist = false;

    char *device;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_worklist));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        exist = gtk_tree_model_get_iter_first(model, &iter0);
        while(exist)
        {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter0,
                    COL_DEFAULT, " ",
                    -1);

            exist = gtk_tree_model_iter_next(model, &iter0); 
        }
        gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                COL_DEFAULT, _("Yes"),
                -1);

        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
        CDCMMan::GetMe()->SetDefaultWorklistService(device);
    }
    else
    {

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            _("No device is selected!"),
            NULL);
    }

}

void DicomServiceSetting::ButtonWorkListConnectClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *device;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_worklist));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
        m_selectedDevice = device;

        ViewHintDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                _("Connect..."));

        g_timeout_add(2000, WorklistConnectTimeout, this);

    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            _("No device is selected!"),
            NULL);
    }
}

gboolean DicomServiceSetting::WorklistConnectTimeout(gpointer data)
{
    DicomServiceSetting *pdata = (DicomServiceSetting *)data;
    
    char info[256]= "\0";
    if(CDCMMan::GetMe()->TestLinkWorklist(pdata->m_selectedDevice))
        {
            PRINTF("Dicom test link OK!!\n");
            sprintf(info, _("Connection test successfully!"));
        }
        else
        {
            PRINTF("Dicom test link FAIL!!\n");
            sprintf(info, _("Connection test fails!"));
        }
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                info,
                NULL);
}


void DicomServiceSetting::ChkBtnAutoQueryToggled(GtkToggleButton *togglebutton)
{
    string device = CDCMMan::GetMe()->GetDefaultWorklistServiceDevice();
    if (device == "")
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), ViewDialog::ERROR, _("Please Set the default Worklist server in system setting"), NULL);
        return ;
    }

    SysDicomSetting sysDicomSetting;
    sysDicomSetting.SetAutoQuery(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton)));
    sysDicomSetting.SyncFile();

}

void DicomServiceSetting::GetGroupDevice(void)
{
    m_group_device_service = CDCMMan::GetMe()->GetAllDevice();
}

//MPPS
void DicomServiceSetting::MPPSDeviceChanged(GtkComboBox *combobox)
{
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_mpps_device));
    if(device != NULL)
    {
       
        char tmp_serviceName[256]="\0"; 
        
            sprintf(tmp_serviceName,"%s",device);
            strcat(tmp_serviceName,"-");
            strcat(tmp_serviceName,_("MPPS"));
        
        gtk_entry_set_text(GTK_ENTRY(m_entry_mpps_name),tmp_serviceName);
    }
}

void DicomServiceSetting::ButtonMPPSAddClicked(GtkButton *button)
{
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_mpps_device));
    const char *ae = gtk_entry_get_text(GTK_ENTRY(m_entry_mpps_AE));
    const char *port = gtk_entry_get_text(GTK_ENTRY(m_entry_mpps_port));
    

    if(ae[0] == '\0' ||port[0] == '\0'||device == NULL)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("Device ,AE or Port is empty, please input!"),
                NULL);
        return;
    }

    char tmp_serviceName[256];
    
        sprintf(tmp_serviceName,"%s",device);
        strcat(tmp_serviceName,"-");
        strcat(tmp_serviceName,_("MPPS"));
   

    char device_tmp0[256];
    char *device_tmp1;
    sprintf(device_tmp0,"%s",device);

    GtkTreeModel *model;
    GtkTreeIter iter;
    gboolean exist = FALSE;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_mpps));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device_tmp1,
                -1);

        if(device_tmp1 == NULL)
            return;

        if (strcmp(device_tmp0,device_tmp1)==0)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                    ViewDialog::INFO, 
                    _("Add failed,device has been existed\n"),
                    NULL);

            g_free(device);
            return;
        }

        exist = gtk_tree_model_iter_next(model, &iter); 
    }
    if(!CDCMMan::GetMe()->AddMPPSService(device, tmp_serviceName, ae, atoi(port)))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("Add failed,device has been existed\n"),
                NULL);

        g_free(device);
        return;
    }
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_STORAGE_DEVICE, device,
            COL_SERVICE_NAME,tmp_serviceName,
            COL_AE_TITLE,ae,
            COL_PORT,port,
            -1);
   // m_group_device_service.push_back(device);
    gtk_tree_model_iter_next(model, &iter); 
    g_free(device);

}

void DicomServiceSetting::ButtonMPPSClearClicked(GtkButton *button)
{
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_mpps_device),-1);
    gtk_entry_set_text(GTK_ENTRY(m_entry_mpps_name),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_mpps_AE),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_mpps_port),"");
}

void DicomServiceSetting::ButtonMPPSDeleteClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *device;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_mpps));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_mpps));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
        if(CDCMMan::GetMe()->DeleteMPPSService(device))
            gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
    else
    {

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            _("No device is selected!"),
            NULL);
    }

}

void DicomServiceSetting::ButtonMPPSDefaultClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *device;
    GtkTreeIter iter0;
    gboolean exist = false;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_mpps));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_mpps));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        exist = gtk_tree_model_get_iter_first(model, &iter0);
        while(exist)
        {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter0,
                    COL_DEFAULT, " ",
                    -1);

            exist = gtk_tree_model_iter_next(model, &iter0); 
        }

        gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                COL_DEFAULT, _("Yes"),
                -1);

        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
       CDCMMan::GetMe()->SetDefaultMPPSService(device);
    }
    else
    {

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            _("No device is selected!"),
            NULL);
    }

}

void DicomServiceSetting::ButtonMPPSConnectClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *device;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_mpps));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_mpps));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
        
        m_selectedDevice = device;

        ViewHintDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                _("Connect..."));

        g_timeout_add(2000, MPPSConnectTimeout, this);
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("No device is selected!"),
                NULL);
    }

}

gboolean DicomServiceSetting::MPPSConnectTimeout(gpointer data)
{
    DicomServiceSetting *pdata = (DicomServiceSetting *)data;
    
    char info[256]= "\0";
    if(CDCMMan::GetMe()->TestLinkMPPS(pdata->m_selectedDevice))
        {
            PRINTF("Dicom test link OK!!\n");
            sprintf(info, _("Connection test successfully!"));
        }
        else
        {
            PRINTF("Dicom test link FAIL!!\n");
            sprintf(info, _("Connection test fails!"));
        }
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                info,
                NULL);
}


void DicomServiceSetting::ChkBtnMPPSToggled(GtkToggleButton *togglebutton)
{
 /*   string device = CDCMMan::GetMe()->GetDefaultMPPSServiceDevice();
    if (device == "")
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), ViewDialog::ERROR, _("Please Set the default MPPS server in system setting"), NULL);
        return ;
    }
*/
    SysDicomSetting sysDicomSetting;
    sysDicomSetting.SetMPPS(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton)));
    sysDicomSetting.SyncFile();


     string device = CDCMMan::GetMe()->GetDefaultMPPSServiceDevice();
    if (device == "")
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), ViewDialog::ERROR, _("Please Set the default MPPS server in system setting"), NULL);
        return ;
    }

}

void DicomServiceSetting::StorageCommitmentDeviceChanged(GtkComboBox *combobox)
{
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_storage_commitment_device));
    if(device != NULL)
    {
        
        char tmp_serviceName[256]="\0"; 
        
        
            sprintf(tmp_serviceName,"%s",device);
            strcat(tmp_serviceName,"-");
            strcat(tmp_serviceName,_("StorageCommitment"));
        
        gtk_entry_set_text(GTK_ENTRY(m_entry_storage_commitment_name),tmp_serviceName);
    }
}

void DicomServiceSetting::ButtonStorageCommitmentAddClicked(GtkButton *button)
{
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_storage_commitment_device));
    const char *ae = gtk_entry_get_text(GTK_ENTRY(m_entry_storage_commitment_AE));
    const char *port = gtk_entry_get_text(GTK_ENTRY(m_entry_storage_commitment_port));
    

    if(ae[0] == '\0' ||port[0] == '\0'||device == NULL)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("Device ,AE or Port is empty, please input!"),
                NULL);
        return;
    }

    char tmp_serviceName[256];
    
        sprintf(tmp_serviceName,"%s",device);
        strcat(tmp_serviceName,"-");
        strcat(tmp_serviceName,_("StorageCommitment"));
    

    char device_tmp0[256];
    char *device_tmp1;
    sprintf(device_tmp0,"%s",device);

    GtkTreeModel *model;
    GtkTreeIter iter;
    gboolean exist = FALSE;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_storage_commitment));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device_tmp1,
                -1);

        if(device_tmp1 == NULL)
            return;

        if (strcmp(device_tmp0,device_tmp1)==0)
        {
            //printf("Add failed\n");
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                    ViewDialog::INFO, 
                    _("Add failed,device has been existed\n"),
                    NULL);

            g_free(device);
            return;
        }

        exist = gtk_tree_model_iter_next(model, &iter); 
    }
    if(!CDCMMan::GetMe()->AddStorageCommitService(device, tmp_serviceName, ae, atoi(port)))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("Add failed,device has been existed\n"),
                NULL);

        g_free(device);
        return;
    }
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_STORAGE_DEVICE, device,
            COL_SERVICE_NAME,tmp_serviceName,
            COL_AE_TITLE,ae,
            COL_PORT,port,
            -1);
   //m_group_device_service.push_back(device);
    gtk_tree_model_iter_next(model, &iter); 
    g_free(device);

}

void DicomServiceSetting::ButtonStorageCommitmentClearClicked(GtkButton *button)
{
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_storage_commitment_device),-1);
    gtk_entry_set_text(GTK_ENTRY(m_entry_storage_commitment_name),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_storage_commitment_AE),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_storage_commitment_port),"");
}

void DicomServiceSetting::ButtonStorageCommitmentDeleteClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *device;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_storage_commitment));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_storage_commitment));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
      if(CDCMMan::GetMe()->DeleteStorageCommitService(device))
            gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
    else
    {

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            _("No device is selected!"),
            NULL);
    }
}

void DicomServiceSetting::ButtonStorageCommitmentDefaultClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *device;
    GtkTreeIter iter0;
    gboolean exist = false;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_storage_commitment));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_storage_commitment));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        exist = gtk_tree_model_get_iter_first(model, &iter0);
        while(exist)
        {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter0,
                    COL_DEFAULT, " ",
                    -1);

            exist = gtk_tree_model_iter_next(model, &iter0); 
        }
        
        gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                COL_DEFAULT, _("Yes"),
                -1);

        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
        CDCMMan::GetMe()->SetDefaultStorageCommitService(device);
    }
    else
    {

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            _("No device is selected!"),
            NULL);
    }
}

void DicomServiceSetting::ButtonStorageCommitmentConnectClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *device;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_storage_commitment));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_storage_commitment));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
        
        m_selectedDevice = device;

        ViewHintDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                _("Connect..."));

        g_timeout_add(2000, StorageCommitmentConnectTimeout, this);
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("No device is selected!"),
                NULL);
    }
}

gboolean DicomServiceSetting::StorageCommitmentConnectTimeout(gpointer data)
{
    DicomServiceSetting *pdata = (DicomServiceSetting *)data;
    
    char info[256]= "\0";
    if(CDCMMan::GetMe()->TestLinkStorageCommit(pdata->m_selectedDevice))
        {
            PRINTF("Dicom test link OK!!\n");
            sprintf(info, _("Connection test successfully!"));
        }
        else
        {
            PRINTF("Dicom test link FAIL!!\n");
            sprintf(info, _("Connection test fails!"));
        }
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                info,
                NULL);
}

void DicomServiceSetting::ChkBtnStorageCommitmentToggled(GtkToggleButton *togglebutton)
{
    string device = CDCMMan::GetMe()->GetDefaultStorageCommitServiceDevice();
    if (device == "")
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), ViewDialog::ERROR, _("Please Set the default Storage Commitment server in system setting"), NULL);
        return ;
    }

    SysDicomSetting sysDicomSetting;
    sysDicomSetting.SetStorageCommitment(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton)));
    sysDicomSetting.SyncFile();
}

//QueryRetrieve
void DicomServiceSetting::QueryRetrieveDeviceChanged(GtkComboBox *combobox)
{
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_query_retrieve_device));
    if(device != NULL)
    {
        
        char tmp_serviceName[256]="\0"; 
       
            sprintf(tmp_serviceName,"%s",device);
            strcat(tmp_serviceName,"-");
            strcat(tmp_serviceName,_("QueryRetrieve"));
       
        gtk_entry_set_text(GTK_ENTRY(m_entry_query_retrieve_name),tmp_serviceName);
    }
}
void DicomServiceSetting::ChangeServiceNoteBook(int page)
{
	 gtk_notebook_set_current_page(GTK_NOTEBOOK(m_service_notebook), page);

}
void DicomServiceSetting::ButtonQueryRetrieveAddClicked(GtkButton *button)
{
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_query_retrieve_device));
    const char *ae = gtk_entry_get_text(GTK_ENTRY(m_entry_query_retrieve_AE));
    const char *port = gtk_entry_get_text(GTK_ENTRY(m_entry_query_retrieve_port));
    

    if(ae[0] == '\0' ||port[0] == '\0'||device == NULL)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("Device ,AE or Port is empty, please input!"),
                NULL);
        return;
    }

    char tmp_serviceName[256];
    
        sprintf(tmp_serviceName,"%s",device);
        strcat(tmp_serviceName,"-");
        strcat(tmp_serviceName,_("QueryRetrieve"));
   
    char device_tmp0[256];
    char *device_tmp1;
    sprintf(device_tmp0,"%s",device);

    

    GtkTreeModel *model;
    GtkTreeIter iter;
    gboolean exist = FALSE;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_query_retrieve));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device_tmp1,
                -1);

        if(device_tmp1 == NULL)
            return;

        if (strcmp(device_tmp0,device_tmp1)==0)
        {
            //printf("Add failed\n");
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                    ViewDialog::INFO, 
                    _("Add failed,device has been existed\n"),
                    NULL);

            g_free(device);
            return;
        }

        exist = gtk_tree_model_iter_next(model, &iter); 
    }
    if(!CDCMMan::GetMe()->AddQueryRetrieveService(device, tmp_serviceName, ae, atoi(port)))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("Add failed,device has been existed\n"),
                NULL);

        g_free(device);
        return;
    }
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_STORAGE_DEVICE, device,
            COL_SERVICE_NAME,tmp_serviceName,
            COL_AE_TITLE,ae,
            COL_PORT,port,
            -1);
   // m_group_device_service.push_back(device);
    gtk_tree_model_iter_next(model, &iter); 
    g_free(device);

}

void DicomServiceSetting::ButtonQueryRetrieveClearClicked(GtkButton *button)
{
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_query_retrieve_device),-1);
    gtk_entry_set_text(GTK_ENTRY(m_entry_query_retrieve_name),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_query_retrieve_AE),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_query_retrieve_port),"");

}

void DicomServiceSetting::ButtonQueryRetrieveDeleteClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *device;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_query_retrieve));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_query_retrieve));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
        CDCMMan::GetMe()->DeleteQueryRetrieveService(device);

        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }

}

void DicomServiceSetting::ButtonQueryRetrieveDefaultClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeIter iter0;
    GtkTreeSelection *selection;
    gboolean exist = false;
    char *device;
    char *isDefault;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_query_retrieve));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_query_retrieve));
    
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        exist = gtk_tree_model_get_iter_first(model, &iter0);
        while(exist)
        {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter0,
                    COL_DEFAULT, " ",
                    -1);

            exist = gtk_tree_model_iter_next(model, &iter0); 
        }
        gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                COL_DEFAULT, _("Yes"),
                -1);

        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
        CDCMMan::GetMe()->SetDefaultQueryRetrieveService(device);
    }
    else
    {

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("No device is selected!"),
                NULL);
    }

}

void DicomServiceSetting::ButtonQueryRetrieveConnectClicked(GtkButton *button)
{
    
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *device;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_query_retrieve));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_query_retrieve));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&device,
                -1);
        
        m_selectedDevice = device;

        ViewHintDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                _("Connect..."));

        g_timeout_add(2000, QueryRetrieveConnectTimeout, this);
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("No device is selected!"),
                NULL);

    }

}

gboolean DicomServiceSetting::QueryRetrieveConnectTimeout(gpointer data)
{
    DicomServiceSetting *pdata = (DicomServiceSetting *)data;
    
    char info[256]= "\0";
    if(CDCMMan::GetMe()->TestLinkQueryRetrieve(pdata->m_selectedDevice))
        {
            PRINTF("Dicom test link OK!!\n");
            sprintf(info, _("Connection test successfully!"));
        }
        else
        {
            PRINTF("Dicom test link FAIL!!\n");
            sprintf(info, _("Connection test fails!"));
        }
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                info,
                NULL);
}

gboolean DicomServiceSetting::KeyFilter(GtkWidget *entry, GdkEventKey *event)
{
        switch(event->keyval)
        {
                case GDK_0:
                case GDK_1:
                case GDK_2:
                case GDK_3:
                case GDK_4:
                case GDK_5:
                case GDK_6:
                case GDK_7:
                case GDK_8:
                case GDK_9:
                case GDK_KP_0:
                case GDK_KP_1:
                case GDK_KP_2:
                case GDK_KP_3:
                case GDK_KP_4:
                case GDK_KP_5:
                case GDK_KP_6:
                case GDK_KP_7:
                case GDK_KP_8:
                case GDK_KP_9:
                case GDK_BackSpace:
                case GDK_Left:
                case GDK_Up:
                case GDK_Right:
                case GDK_Down:
                case GDK_Delete:
                case GDK_Home:
                case GDK_End:
				case GDK_Tab:
                        return FALSE;
                default:
                        return TRUE;
        }
}
