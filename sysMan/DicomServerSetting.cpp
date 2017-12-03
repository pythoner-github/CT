#include "DicomServerSetting.h"
#include "gui_global.h"
#include "gui_func.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include "DCMMan.h"
#include "ViewDialog.h"
#include "ViewSystem.h"

DicomServerSetting* DicomServerSetting::m_ptrInstance = NULL;

DicomServerSetting::DicomServerSetting()
{
}

DicomServerSetting::~DicomServerSetting()
{
}

DicomServerSetting* DicomServerSetting::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new DicomServerSetting;

    return m_ptrInstance;
}

GtkWidget* DicomServerSetting::CreateDicomWindow(GtkWidget *parent)
{
    GtkWidget *fixed_server;
    GtkWidget *frame_equipment;
    GtkWidget *label_equipment;

    GtkWidget *fixed;
    GtkWidget *label_device;
    GtkWidget *label_ip;
    GtkWidget *ip_style;

    GtkWidget *button_ping;
    GtkWidget *button_add;
    GtkWidget *button_delete;

    GtkWidget *frame_server_list;
    GtkWidget *label_server_list;
    GtkWidget *fixed_list;

    fixed_server = gtk_fixed_new ();
    gtk_widget_show (fixed_server);
    
    frame_equipment = gtk_frame_new (NULL);
    gtk_widget_show (frame_equipment);
    gtk_fixed_put (GTK_FIXED (fixed_server), frame_equipment, 20, 10);
    gtk_widget_set_size_request (frame_equipment, 750, 190);
    gtk_frame_set_label_align (GTK_FRAME (frame_equipment), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_equipment), GTK_SHADOW_IN);

    label_equipment = gtk_label_new (_("<b>Device</b>"));
    gtk_widget_show (label_equipment);
    gtk_frame_set_label_widget (GTK_FRAME (frame_equipment), label_equipment);
    gtk_label_set_use_markup (GTK_LABEL (label_equipment), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame_equipment), fixed);

    label_device = gtk_label_new (_("<b>Device :</b>"));
    gtk_widget_show (label_device);
    gtk_fixed_put (GTK_FIXED (fixed), label_device, 30, 15+5);
    gtk_label_set_use_markup (GTK_LABEL (label_device), TRUE);

    m_entry_device = gtk_entry_new ();
    gtk_widget_show (m_entry_device);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_device, 130, 15);
    gtk_widget_set_size_request (m_entry_device, 200, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_device), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_device), 9679);
    
    label_ip = gtk_label_new (_("<b>IP Address :</b>"));
    gtk_widget_show (label_ip);
    gtk_fixed_put (GTK_FIXED (fixed), label_ip, 30, 70+5);
    gtk_label_set_use_markup (GTK_LABEL (label_ip), TRUE);

    m_entry_ip = gtk_entry_new ();
    gtk_widget_show (m_entry_ip);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_ip, 130, 70);
    gtk_widget_set_size_request (m_entry_ip, 200, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_ip), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ip), 9679);
        
    ip_style = gtk_label_new (_("<b>(xxx.xxx.xxx.xxx)</b>"));
    gtk_widget_show (ip_style);
    gtk_fixed_put (GTK_FIXED (fixed), ip_style ,350, 80);
    gtk_label_set_use_markup (GTK_LABEL (ip_style), TRUE);

    button_ping = gtk_button_new_with_mnemonic (_("Ping"));
    gtk_widget_show (button_ping);
    gtk_fixed_put (GTK_FIXED (fixed), button_ping, 50, 135-10);
    gtk_widget_set_size_request (button_ping, 105, 35);
    g_signal_connect(button_ping, "clicked", G_CALLBACK(HandleButtonPingClicked), this);
    
    button_add = gtk_button_new_with_mnemonic (_("Add"));
    gtk_widget_show (button_add);
    gtk_fixed_put (GTK_FIXED (fixed), button_add, 170, 135-10);
    gtk_widget_set_size_request (button_add, 105, 35);
    g_signal_connect(button_add, "clicked", G_CALLBACK(HandleButtonAddClicked), this);
    
    frame_server_list = gtk_frame_new (NULL);
    gtk_widget_show (frame_server_list);
    gtk_fixed_put (GTK_FIXED (fixed_server), frame_server_list, 20, 235-15);
    gtk_widget_set_size_request (frame_server_list, 750, 230+5);
    gtk_frame_set_label_align (GTK_FRAME (frame_server_list), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_server_list), GTK_SHADOW_IN);

    label_server_list = gtk_label_new (_("<b>Device List</b>"));
    gtk_widget_show (label_server_list);
    gtk_frame_set_label_widget (GTK_FRAME (frame_server_list), label_server_list);
    gtk_label_set_use_markup (GTK_LABEL (label_server_list), TRUE);
    
    fixed_list = gtk_fixed_new ();
    gtk_widget_show (fixed_list);
    gtk_container_add (GTK_CONTAINER (frame_server_list), fixed_list);

    GtkWidget *scrollWin = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show (scrollWin);
    gtk_fixed_put (GTK_FIXED (fixed_list), scrollWin, 0, 0);
    gtk_widget_set_size_request (scrollWin, 750-10, 170-10);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollWin), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrollWin), GTK_SHADOW_IN);
	
    m_treeview = create_server_treeview();
	gtk_container_add(GTK_CONTAINER(scrollWin), m_treeview);
	gtk_widget_set_size_request (m_treeview, 250, 120);
    gtk_widget_show (m_treeview);
    
    button_delete = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (button_delete);
    gtk_fixed_put (GTK_FIXED (fixed_list), button_delete, 50, 170-2);
    gtk_widget_set_size_request (button_delete, 105, 35);
    g_signal_connect(button_delete, "clicked", G_CALLBACK(HandleButtonDeleteClicked), this);

    return fixed_server;
}

GtkWidget* DicomServerSetting::create_server_treeview()
{
	GtkWidget *treeview;
    GtkTreeModel *model = NULL;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    treeview = gtk_tree_view_new ();
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);

	renderer = gtk_cell_renderer_toggle_new ();
	//g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK (HandleToggleRecord), this);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Device"), renderer, "text", COL_DEVICE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 180, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("IP Address"), renderer, "text", COL_IP, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);
    
    model = create_device_model();
    if (model != NULL)
		gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    g_object_unref (model); 

	return treeview;
}

GtkTreeModel* DicomServerSetting::create_device_model()
{
    GtkListStore *store;

    store = gtk_list_store_new(NUM_COLS, 
			G_TYPE_STRING, 
			G_TYPE_STRING);

	return GTK_TREE_MODEL (store);
}

void DicomServerSetting::GetSingleServerAttribute(string device, string ip, void *data)
{
    DicomServerSetting *tmp;
    tmp = (DicomServerSetting *)data;

    //tmp->m_group_device_server.clear();

    tmp->m_group_device_server.push_back(device);
    tmp->m_group_ip_server.push_back(ip);

    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(tmp->m_treeview));
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
        gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                COL_DEVICE, device.c_str(),
                COL_IP,ip.c_str(),
                -1);

        gtk_tree_model_iter_next(model, &iter); 
}

void DicomServerSetting::init_server_setting(void)
{
    GtkTreeModel *model;
        
    m_group_device_server.clear();
    
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
    gtk_list_store_clear(GTK_LIST_STORE(model));
    CDCMMan::GetMe()->GetAllServer(GetSingleServerAttribute, this);
}

void DicomServerSetting::ButtonPingClicked(GtkButton *button)
{
    const char * ip = gtk_entry_get_text(GTK_ENTRY(m_entry_ip));
    const char * device = gtk_entry_get_text(GTK_ENTRY(m_entry_device));
   if(ip[0] == '\0' || device[0] == '\0')
   {
       ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            _("Device or IP is empty, please input!"),
            NULL);
       return;
   }

    ViewHintDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            _("Ping..."));

    g_timeout_add(2000, PingTimeout, this);
}

gboolean DicomServerSetting::PingTimeout(gpointer data)
{
    DicomServerSetting *pData = (DicomServerSetting *)data;
    const char * ip = gtk_entry_get_text(GTK_ENTRY(pData->m_entry_ip));
    char strHint[256] = {0};
    if (CDCMMan::GetMe()->Ping((char *)ip))
        sprintf(strHint, "%s %s\n", _("Success to ping ip"), ip);
    else
        sprintf(strHint, "%s %s\n", _("Failed to ping ip"), ip);

    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            strHint,
            NULL);

    return FALSE;
}

void DicomServerSetting::ButtonAddClicked(GtkButton *button)
{
    const char * device = gtk_entry_get_text(GTK_ENTRY(m_entry_device));
    const char * ip = gtk_entry_get_text(GTK_ENTRY(m_entry_ip));
    if(strcmp(device,"localhost")==0)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("'localhost' has been used, please input again!"),
                NULL);
        return;

    }   

    if(ip[0] == '\0' || device[0] == '\0')
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("Device or IP is empty, please input!"),
                NULL);
        return;
    }

    char device_tmp[256];
    char ip_tmp[256];
    for(int i=0;i<m_group_device_server.size();i++)
    {
        sprintf(device_tmp,"%s",m_group_device_server[i].c_str());
        sprintf(ip_tmp,"%s",m_group_ip_server[i].c_str());
        if((strcmp(device,device_tmp)==0)||(strcmp(ip,ip_tmp)==0))
        {
            PRINTF("------add failed\n");
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            _("Add failed,device or IP has been existed\n"),
            NULL);
            return;
        }
    }

    if(!CDCMMan::GetMe()->AddServer(device,ip))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            _("Add failed,device or IP has been existed\n"),
            NULL);
        return;
    }

    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_DEVICE, device,
            COL_IP,ip,
            -1);

    m_group_device_server.push_back(device);
    m_group_ip_server.push_back(ip);

    gtk_tree_model_iter_next(model, &iter);
 
}

void DicomServerSetting::ButtonDeleteClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    char *device;
    char *ip;
        
    vector<string>::iterator Iter;

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
    if(gtk_tree_selection_get_selected(selection,NULL,&iter))
    {
        gtk_tree_model_get(model, &iter,
                COL_DEVICE,&device,
                COL_IP,&ip,
                -1);

        for(Iter = m_group_device_server.begin();Iter < m_group_device_server.end();Iter++ )
        {
            if (strcmp(device,(*Iter).c_str())==0)
                m_group_device_server.erase(Iter);
        }

            gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

        //CDCMMan::GetMe()->DeleteServer(device);
        printf("-----------Delete:%d %s\n",CDCMMan::GetMe()->DeleteServer(device),device);
    }
}

