#include "ButtonCallBack.h"
#include <glib/gi18n.h>
#include "DCMMan.h"
#include <stdlib.h>
#include "ViewDialog.h"
#include "ViewSystem.h"
#define _DEBUG_ 1


ButtonCallBack *ButtonCallBack::m_pInstance = NULL;
ButtonCallBack::ButtonCallBack()
{



}

ButtonCallBack::~ButtonCallBack()
{


}

ButtonCallBack * ButtonCallBack::GetInstance()
{

	if(NULL == m_pInstance)
	{
		m_pInstance = new ButtonCallBack();
	}
	return m_pInstance;
}
void ButtonCallBack::PrintDeviceChanged(GtkComboBox *combobox,GtkWidget *m_combobox_worklist_device,GtkWidget *m_entry_worklist_name)
{
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_worklist_device));
    if(device != NULL)
    {
        
        char tmp_serviceName[256]="\0"; 
        
            sprintf(tmp_serviceName,"%s",device);
            strcat(tmp_serviceName,"-");
            strcat(tmp_serviceName,_("Print"));
        
        gtk_entry_set_text(GTK_ENTRY(m_entry_worklist_name),tmp_serviceName);
    }
}
static gboolean WorklistConnectTimeout(gpointer data)
{
   // DicomServiceSetting *pdata = (DicomServiceSetting *)data;
    
    char info[256]= "\0";
    //if(CDCMMan::GetMe()->TestLinkWorklist(ButtonCallBack::GetInstance()->m_selectedDevice))
    if(CDCMMan::GetMe()->TestLinkPrint(ButtonCallBack::GetInstance()->m_selectedDevice))
        {
            //PRINTF("Dicom test link OK!!\n");
            sprintf(info, _("Connection test successfully!"));
        }
        else
        {
            //PRINTF("Dicom test link FAIL!!\n");
            sprintf(info, _("Connection test fails!"));
        }

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                info,
                NULL);

}

void ButtonCallBack::ButtonPrintConnectClicked(GtkButton *button,GtkWidget *m_treeview_worklist)
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


bool ButtonCallBack::ButtonPrintDeleteClicked(GtkButton *button,GtkWidget *m_treeview_worklist)
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

        //CDCMMan::GetMe()->DeleteWorklistService(device);
		 CDCMMan::GetMe()->DeletePrintService(device);
		 CDCMMan::GetMe()->DeletePrintServiceConfiguration(device);
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
		return TRUE;
    }
    else
    {

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            _("No device is selected!"),
            NULL);
		return FALSE;

    }
}

void ButtonCallBack::ButtonPrintDefaultClicked(GtkButton *button,GtkWidget *m_treeview_worklist)
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

       // CDCMMan::GetMe()->SetDefaultWorklistService(device);
      CDCMMan::GetMe()->SetDefaultPrintService(device);

    }
    else
    {

        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            _("No device is selected!"),
            NULL);

    }

}






void ButtonCallBack::ButtonPrintClearClicked(GtkButton *button,GtkWidget *m_combobox_worklist_device,GtkWidget *m_entry_worklist_name,GtkWidget *m_entry_worklist_AE,GtkWidget *m_entry_worklist_port)
{

    //gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_worklist_device));
    //gtk_combo_box_remove_text(GTK_COMBO_BOX(m_combobox_worklist_device),index);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_worklist_device),-1);
    gtk_entry_set_text(GTK_ENTRY(m_entry_worklist_name),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_worklist_AE),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_worklist_port),"");
	

}

char * ButtonCallBack::GtkTreeDeviceGet(GtkWidget *m_treeview_worklist)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
    gboolean exist = FALSE;
	char *device;
	bool ret;
	GtkTreeSelection *selected;
	selected= gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_worklist));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    exist = gtk_tree_model_get_iter_first(model, &iter);
	while(exist)
	{
		ret = gtk_tree_selection_iter_is_selected (selected,&iter);
		if(ret == TRUE)
		{
			gtk_tree_model_get(model,&iter,COL_STORAGE_DEVICE,&device,-1);
			break;
		}
		exist = gtk_tree_model_iter_next(model, &iter);
				
	}
	return device;
}

char * ButtonCallBack::GtkTreeServiceGet(GtkWidget *m_treeview_worklist)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
    gboolean exist = FALSE;
	char *service;
	bool ret;
	GtkTreeSelection *selected;
	selected= gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_worklist));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    exist = gtk_tree_model_get_iter_first(model, &iter);
	while(exist)
	{
		ret = gtk_tree_selection_iter_is_selected (selected,&iter);
		if(ret == TRUE)
		{
			gtk_tree_model_get(model,&iter,COL_SERVICE_NAME,&service,-1);
			break;
		}
		exist = gtk_tree_model_iter_next(model, &iter);
				
	}
	return service;

}

char * ButtonCallBack::GtkTreeAEGet(GtkWidget *m_treeview_worklist)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
    gboolean exist = FALSE;
	char *ae;
	bool ret;
	GtkTreeSelection *selected;
	selected= gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_worklist));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    exist = gtk_tree_model_get_iter_first(model, &iter);
	while(exist)
	{
		ret = gtk_tree_selection_iter_is_selected (selected,&iter);
		if(ret == TRUE)
		{
			gtk_tree_model_get(model,&iter,COL_AE_TITLE,&ae,-1);
			break;
		}
		exist = gtk_tree_model_iter_next(model, &iter);
				
	}

	return ae;

}

char * ButtonCallBack::GtkTreePortGet(GtkWidget *m_treeview_worklist)
{	
	GtkTreeModel *model;
	GtkTreeIter iter;
    gboolean exist = FALSE;
	char *port;
	
	bool ret;
	GtkTreeSelection *selected;
	selected= gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_worklist));
	
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_worklist));
    exist = gtk_tree_model_get_iter_first(model, &iter);
	
	
	while(exist)
	{
		ret = gtk_tree_selection_iter_is_selected (selected,&iter);
		if(ret == TRUE)
		{
			gtk_tree_model_get(model,&iter,COL_PORT,&port,-1);
			break;
		}
		exist = gtk_tree_model_iter_next(model, &iter);
				
	}
    

	return port;

}
void ButtonCallBack::ButtonPrintAddClicked(GtkButton *button,GtkWidget *m_combobox_worklist_device,GtkWidget *m_entry_worklist_AE,GtkWidget *m_entry_worklist_port,GtkWidget *m_entry_worklist_name,GtkWidget *m_treeview_worklist)
{
printf("===========================================================================\n");
    char *device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_worklist_device));
    const char *ae = gtk_entry_get_text(GTK_ENTRY(m_entry_worklist_AE));
    const char *port = gtk_entry_get_text(GTK_ENTRY(m_entry_worklist_port));
    

    if(ae[0] == '\0' ||port[0] == '\0'||device[0] == '\0')
    {
		CDCMMan::GetMe()->DeletePrintService(device);
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO, 
                _("Device ,AE or Port is empty, please input!"),
                NULL);

        return;
    }

    char tmp_serviceName[256];
   
        sprintf(tmp_serviceName,"%s",device);
        strcat(tmp_serviceName,"-");
        strcat(tmp_serviceName,_("Print"));
   
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
			CDCMMan::GetMe()->DeletePrintService(device);
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

   
    if(!CDCMMan::GetMe()->AddPrintService(device, tmp_serviceName, ae, atoi(port)))
    {
		CDCMMan::GetMe()->DeletePrintService(device);
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
printf("===========================================================================\n");
    g_free(device);
	

}
