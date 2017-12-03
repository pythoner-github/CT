#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "KeyValueOpr.h"
#include "ViewCD.h"
#include "ViewDialog.h"
#include "ViewArchive.h"
#include "gui_global.h"
#include "KeyDef.h"
#include "ImgMan.h"
#include "VideoMan.h"
#include "PeripheralMan.h"
#include "CdBurn.h"

#define LIST_PATH "/tmp/pathlist"
#define TMPDIR "/tmp/tmp_mkisofs"

ViewCD* ViewCD::m_ptrInstance = NULL;

ViewCD::ViewCD()
{
    m_window = NULL;
    m_vecFolder.clear();
    m_vecPath.clear();
    m_vecFolderNew.clear();
    m_vecPathNew.clear();
    m_vecFlashItem.clear();
    m_iscd = true;
}

ViewCD::~ViewCD()
{
    if(m_ptrInstance != NULL)
        delete m_ptrInstance;
}

ViewCD* ViewCD::GetInstance()
{
    if(m_ptrInstance == NULL)
        m_ptrInstance = new ViewCD;

    return m_ptrInstance;
}

GtkWidget* ViewCD::GetWindow(void)
{
    if (GTK_IS_WIDGET(m_window))
        return m_window;
    else 
        return NULL;
}

/**
 *@brief: load patient check id when exporting patient data to peripheral equipment
 *
 */
void ViewCD::LoadFlashItem(vector<string> item)
{
    m_vecFlashItem.clear();

    vector<string>::iterator iter = item.begin();
    while (iter < item.end()) 
    {
        m_vecFlashItem.push_back((*iter).c_str());
        iter++;
    }
}

void ViewCD::add_column(GtkTreeView *treeview)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

GtkTreeModel* ViewCD::create_from_model(vector<string> vec)
{
    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(1, G_TYPE_STRING);
    if(vec.empty() || m_vecFlashItem.empty())
        return GTK_TREE_MODEL(store);

    vector<string> vec_child;
    vector<string>::iterator ite = vec.begin();
    vector<string>::iterator iterItem = m_vecFlashItem.begin();

    while (ite < vec.end()) {
        gtk_tree_store_append(store, &iter, NULL);
        gtk_tree_store_set(store, &iter,
                0, (*iterItem).c_str(),
                -1);
        vec_child.clear();
        ImgMan::GetInstance()->LoadSnap(atoi((*ite).c_str()), STORE_PATH, &vec_child);
        VideoMan::GetInstance()->LoadVideo(atoi((*ite).c_str()), STORE_PATH, &vec_child);

        vector<string>::iterator ite_child = vec_child.begin();
        while (ite_child < vec_child.end()) {
            GtkTreeIter child_iter;
            gtk_tree_store_append(store, &child_iter, &iter);
            gtk_tree_store_set(store, &child_iter,
                    0, (*ite_child).c_str(),
                    -1);
            ite_child++;
        }

        ite++;
        iterItem++;
    }
    return GTK_TREE_MODEL(store);
}

static gboolean UpdateUSBInfo(gpointer data)
{
    ViewCD *ptrCD = (ViewCD *)data;
    if(PeripheralMan::GetInstance()->CheckUSBInfo())
    {
        PeripheralMan::GetInstance()->MountUsbStorage();
        ptrCD->UpdateSize(false);
        ptrCD->SetBackupStatus(ViewCD::USB_READY);
    }
    else
        ptrCD->SetBackupStatus(ViewCD::USB_EMPTY);
    return FALSE;
}
extern KeyValueOpr keyInterface;
void ViewCD::CreateWindow(GtkWidget *parent, vector<string> vec, bool is_cd)
{
    GtkWidget *fixed;
    GtkWidget *btnAdd;
    GtkWidget *btnDel;
    GtkWidget *btnAddAll;
    GtkWidget *btnDelAll;
    GtkWidget *tableSize;
    GtkWidget *labelAvailMb;
    GtkWidget *labelAvail;
    GtkWidget *labelEstimated;
    GtkWidget *labelEstimatedMb;
    GtkWidget *labelRemainMb;
    GtkWidget *labelRemain;
    GtkWidget *btnExit;

    m_vecTotalId.clear();

    vector<string>::iterator iter = vec.begin();
    while (iter < vec.end()) 
    {
        m_vecTotalId.push_back((*iter).c_str());
        iter++;
    }

    m_iscd = is_cd;
    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, 640, 540);
    if (is_cd)
        gtk_window_set_title (GTK_WINDOW (m_window), _("Export to CD/DVD"));
    else
        gtk_window_set_title (GTK_WINDOW (m_window), _("Export to USB"));
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for (GTK_WINDOW (m_window), GTK_WINDOW (parent));
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this);

    fixed = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    /*	create file tree	*/
    GtkWidget *sw_from = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw_from), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw_from), GTK_SHADOW_OUT);
    gtk_widget_set_size_request (sw_from, 250, 390);
    gtk_fixed_put (GTK_FIXED (fixed), sw_from, 15, 25);

    GtkWidget *vp_from = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (vp_from);
    gtk_container_add (GTK_CONTAINER (sw_from), vp_from);
    GtkTreeModel *modelFrom = create_from_model(vec);
    m_treeFrom = gtk_tree_view_new_with_model (modelFrom);
    g_object_unref (modelFrom);
    add_column(GTK_TREE_VIEW(m_treeFrom));
    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeFrom), FALSE);
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (m_treeFrom), FALSE);
    gtk_container_add (GTK_CONTAINER (vp_from), m_treeFrom);

    GtkWidget *sw_to = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw_to), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw_to), GTK_SHADOW_OUT);
    gtk_widget_set_size_request (sw_to, 250, 390);
    gtk_fixed_put (GTK_FIXED (fixed), sw_to, 375, 25);

    GtkWidget *vp_to = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (vp_to);
    gtk_container_add (GTK_CONTAINER (sw_to), vp_to);

    GtkTreeStore *model = gtk_tree_store_new (1, G_TYPE_STRING);
    m_treeTo = gtk_tree_view_new_with_model (GTK_TREE_MODEL(model));
    g_object_unref (model);
    add_column(GTK_TREE_VIEW(m_treeTo));
    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeTo), FALSE);
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (m_treeTo), FALSE);
    gtk_container_add (GTK_CONTAINER (vp_to), m_treeTo);

    btnAdd = gtk_button_new_with_mnemonic (">");
    gtk_fixed_put (GTK_FIXED (fixed), btnAdd, 280, 55);
    gtk_widget_set_size_request (btnAdd, 80, 60);
    g_signal_connect(G_OBJECT(btnAdd), "clicked", G_CALLBACK(HandleBtnAddClicked), this);

    btnDel = gtk_button_new_with_mnemonic ("<");
    gtk_fixed_put (GTK_FIXED (fixed), btnDel, 280, 145);
    gtk_widget_set_size_request (btnDel, 80, 60);
    g_signal_connect(G_OBJECT(btnDel), "clicked", G_CALLBACK(HandleBtnDelClicked), this);

    btnAddAll = gtk_button_new_with_mnemonic (">>");
    gtk_fixed_put (GTK_FIXED (fixed), btnAddAll, 280, 235);
    gtk_widget_set_size_request (btnAddAll, 80, 60);
    g_signal_connect(G_OBJECT(btnAddAll), "clicked", G_CALLBACK(HandleBtnAddAllClicked), this);


    btnDelAll = gtk_button_new_with_mnemonic ("<<");
    gtk_fixed_put (GTK_FIXED (fixed), btnDelAll, 280, 325);
    gtk_widget_set_size_request (btnDelAll, 80, 60);
    g_signal_connect(G_OBJECT(btnDelAll), "clicked", G_CALLBACK(HandleBtnDelAllClicked), this);

    tableSize = gtk_table_new (6, 2, FALSE);
    gtk_fixed_put (GTK_FIXED (fixed), tableSize, 300, 430);
    gtk_widget_set_size_request (tableSize, 300, 60);

    labelAvail = gtk_label_new (_("Available:"));
    gtk_table_attach_defaults (GTK_TABLE(tableSize), labelAvail, 0, 1, 0, 1);

    m_entryAvail = gtk_entry_new ();
    gtk_table_attach_defaults (GTK_TABLE(tableSize), m_entryAvail, 1, 2, 0, 1);
    gtk_widget_set_size_request (m_entryAvail, 60, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entryAvail), 8226);

    labelAvailMb = gtk_label_new ("Mb");
    gtk_table_attach_defaults (GTK_TABLE(tableSize), labelAvailMb, 2, 3, 0, 1);

    gtk_table_set_col_spacing (GTK_TABLE(tableSize), 2, 10);

    labelEstimated = gtk_label_new (_("Estimated:"));
    gtk_table_attach_defaults (GTK_TABLE(tableSize), labelEstimated, 3, 4, 0, 1);

    m_entryEstimated = gtk_entry_new ();
    gtk_table_attach_defaults (GTK_TABLE(tableSize), m_entryEstimated, 4, 5, 0, 1);
    gtk_widget_set_size_request (m_entryEstimated, 60, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entryEstimated), 8226);

    labelEstimatedMb = gtk_label_new ("Mb");
    gtk_table_attach_defaults (GTK_TABLE(tableSize), labelEstimatedMb, 5, 6, 0, 1);

    labelRemain = gtk_label_new (_("Remain:"));
    gtk_table_attach_defaults (GTK_TABLE(tableSize), labelRemain, 0, 1, 1, 2);

    m_entryRemain = gtk_entry_new ();
    gtk_table_attach_defaults (GTK_TABLE(tableSize), m_entryRemain, 1, 2, 1, 2);
    gtk_widget_set_size_request (m_entryRemain, 60, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entryRemain), 8226);

    labelRemainMb = gtk_label_new ("Mb");
    gtk_table_attach_defaults (GTK_TABLE(tableSize), labelRemainMb, 2, 3, 1, 2);
    m_btnBackup = gtk_button_new_with_mnemonic (_("Export"));
    if (!is_cd)
    {
        g_signal_connect(G_OBJECT(m_btnBackup), "clicked", G_CALLBACK(HandleBtnExportClicked), this);
    }
    else
        g_signal_connect(G_OBJECT(m_btnBackup), "clicked", G_CALLBACK(HandleBtnBackupClicked), this);
    gtk_fixed_put (GTK_FIXED (fixed), m_btnBackup, 110, 440);
    gtk_widget_set_size_request (m_btnBackup, 80, 60);

#if 0
    btnErase = gtk_button_new_with_mnemonic (_("Erase"));
    gtk_fixed_put (GTK_FIXED (fixed), btnErase, 205, 440);
    gtk_widget_set_size_request (btnErase, 80, 60);
    g_signal_connect(G_OBJECT(btnErase), "clicked", G_CALLBACK(HandleBtnEraseClicked), this);
#endif

    btnExit = gtk_button_new_with_mnemonic (_("Exit"));
    gtk_fixed_put (GTK_FIXED (fixed), btnExit, 15, 440);
    gtk_widget_set_size_request (btnExit, 80, 60);
    g_signal_connect(G_OBJECT(btnExit), "clicked", G_CALLBACK(HandleBtnExitClicked), this);

    /* status bar */
    m_statusbar = gtk_statusbar_new ();
    gtk_fixed_put (GTK_FIXED (fixed), m_statusbar, 0, 520);
    gtk_widget_set_size_request (m_statusbar, 640, 20);

    g_keyInterface.Push(this);
    gtk_widget_show_all(m_window);

    if (is_cd)
    {
        UpdateSize(true);
        if(PeripheralMan::GetInstance()->CheckDiscInfo())
            SetBackupStatus(READY);
        else
            SetBackupStatus(EMPTY);
    }
    else
    {
        SetBackupStatus(ViewCD::USB_SCAN);
        g_timeout_add(50, UpdateUSBInfo, this);
#if 0
        UpdateSize(false);
        if(PeripheralMan::GetInstance()->CheckUSBInfo())
        {
            printf("read\n");
            SetBackupStatus(USB_READY);
        }
        else
            SetBackupStatus(USB_EMPTY);
#endif
    }
}

gboolean ViewCD::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWindow();
    return FALSE;
}

void ViewCD::DestroyWindow()
{
    if(GTK_IS_WIDGET(m_window))
    {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        m_window = NULL;
    }
    if (!m_iscd)
        PeripheralMan::GetInstance()->UmountUsbStorage();
}

void ViewCD::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);

    switch(keyValue)
    {
        case KEY_ESC:
            DestroyWindow();
            break;

        default:
            break;
    }
}

void ViewCD::SetEstimatedSize(float size)
{
    char str[50];
    sprintf(str, "%.2f", size);
    gtk_entry_set_text(GTK_ENTRY(m_entryEstimated), str);
}

void ViewCD::SetRemainSize(float size)
{
    char str[50];
    sprintf(str, "%.2f", size);
    gtk_entry_set_text(GTK_ENTRY(m_entryRemain), str);
}

void ViewCD::SetAvailSize(float size)
{
    char str[50];
    sprintf(str, "%.2f", size);
    gtk_entry_set_text(GTK_ENTRY(m_entryAvail), str);
}

void ViewCD::UpdateSize(bool is_cd)
{
    gboolean valid1, valid2;
    GtkTreeIter iter1, iter2;
    gchar *val1, *val2, *folder, *file;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeTo));

    m_vecFolder.clear();
    m_vecPath.clear();
    if(!m_vecPathNew.empty())
        m_vecPathNew.clear();
    valid1 = gtk_tree_model_get_iter_first(model, &iter1);
    vector<string>::iterator iter = m_vecFolderNew.begin();
    while(valid1)
    {
        gtk_tree_model_get(model, &iter1, 0, &val1, -1);
        folder = g_build_filename(STORE_PATH, val1, NULL);
        //	PRINTF("folder: %s\n", folder);
        m_vecFolder.push_back(folder);
        g_free(folder);

        valid2 = gtk_tree_model_iter_children(model, &iter2, &iter1);
        while(valid2)
        {
            gtk_tree_model_get(model, &iter2, 0, &val2, -1);
            file = g_build_filename(STORE_PATH, val1, val2, NULL);
            gchar *file_tmp = g_build_filename(STORE_PATH, (*iter).c_str(), val2, NULL);
            m_vecPath.push_back(file);
            m_vecPathNew.push_back(file_tmp);
           g_free(file);
           g_free(file_tmp);

            valid2 = gtk_tree_model_iter_next(model, &iter2);
        }
        valid1 =  gtk_tree_model_iter_next(model, &iter1);
        iter++;
    }

    m_estimated = 0;
    vector<string>::iterator it;
    //for(it=m_vecPath.begin(); it<m_vecPath.end(); it++)
    for(it=m_vecPathNew.begin(); it<m_vecPathNew.end(); it++)
    {
        struct stat buf;
        if( stat((*it).c_str(), &buf)==0 )
        {
            m_estimated += buf.st_size;
            //	PRINTF("file: %s, size: %d\n", (*it).c_str(), buf.st_size);
        }
    }
	if(m_estimated > 0)
		m_estimated = m_estimated + m_estimated / 10;

	PRINTF("Estimated size: %llu\n", m_estimated);

    SetEstimatedSize( (float)m_estimated/1024/1024 );

    if (is_cd)
    {
        //clear Replay data for exe cmd
        Replay::GetInstance()->ClearCurReplayData();
        Replay::GetInstance()->DisplayReplayBar();

        PeripheralMan::DiscInfo info;
        if(PeripheralMan::GetInstance()->GetDiscInfo(&info))
        {
            if(info.capacity > info.data_size)
                m_available = info.capacity - info.data_size;
            else
                m_available = 0;
            if(m_available >= m_estimated)
            {
                m_remain = m_available - m_estimated;
                SetBackupStatus(READY);
            }
            else
            {
                m_remain = 0;
                SetBackupStatus(OVER);
            }
            SetRemainSize( (double)m_remain/1024/1024 );
            SetAvailSize( (double)m_available/1024/1024 );
            PRINTF("disc capacity=%llu, data_size=%llu, total available=%llu, remain=%llu\n", info.capacity, info.data_size, m_available, m_remain);
        }
        else
        {
            SetRemainSize(0);
            SetAvailSize(0);
        }
    }
    else
    {
        PeripheralMan::StorageInfo info;
        if(PeripheralMan::GetInstance()->GetUSBInfo(&info))
        {
            if(info.capacity > info.data_size)
                m_available = info.capacity - info.data_size;
            else
                m_available = 0;
            if(m_available >= m_estimated)
            {
                m_remain = m_available - m_estimated;
                SetBackupStatus(USB_READY);
            }
            else
            {
                m_remain = 0;
                SetBackupStatus(USB_OVER);
            }
            SetRemainSize( (double)m_remain/1024/1024 );
            SetAvailSize( (double)m_available/1024/1024 );
            PRINTF("disc capacity=%llu, data_size=%llu, total available=%llu, remain=%llu\n", info.capacity, info.data_size, m_available, m_remain);
        }
        else
        {
            SetRemainSize(0);
            SetAvailSize(0);
        }
    }
}

void ViewCD::UpdateStatusBar(const char* msg)
{
    gtk_statusbar_pop(GTK_STATUSBAR(m_statusbar), 0); /* clear any preview message */

    gtk_statusbar_push(GTK_STATUSBAR(m_statusbar), 0, msg);
}

void ViewCD::SetBackupStatus(guchar status)
{
   // printf("_%s, %d\n", __FUNCTION__, status);
    switch(status)
    {
        case READY:
        case USB_READY:
            UpdateStatusBar(_("Ready!"));
            gtk_widget_set_sensitive(m_btnBackup, TRUE);
            break;
        case SCAN:
        case USB_SCAN:
            if (m_iscd)
                UpdateStatusBar(_("Getting the media info, Please wait..."));
            else
                UpdateStatusBar(_("Getting the USB info, Please wait..."));
            gtk_widget_set_sensitive(m_btnBackup, FALSE);
            break;
        case EMPTY:
        case USB_EMPTY:
            if (m_iscd)
                UpdateStatusBar(_("Please insert disc!"));
            else
                UpdateStatusBar(_("Please insert one USB!"));
            gtk_widget_set_sensitive(m_btnBackup, FALSE);
            break;
        case OVER:
        case USB_OVER:
            if (m_iscd)
                UpdateStatusBar(_("Beyond the available capacity of the disc!"));
            else
                UpdateStatusBar(_("Beyond the available capacity of the USB!"));
            gtk_widget_set_sensitive(m_btnBackup, FALSE);
    }
}

void ViewCD::AppendParent(const char* parent, vector<string> child)
{
    gboolean valid, exist;
    GtkTreeIter iter_parent, iter_child;
    gchar *val;
    GtkTreeStore *store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeTo)));

    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter_parent);
    exist = FALSE;
    // Check whether parent exist
    while(valid)
    {
        gtk_tree_model_get(GTK_TREE_MODEL(store), &iter_parent, 0, &val, -1);
        if(strcmp(parent, val)==0)
        {
            exist = TRUE;
            break;
        }
        valid =  gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter_parent);
    }
    if(exist)
    {
        // remove the exist then append new 
        gtk_tree_store_remove (store, &iter_parent);
    }
    gtk_tree_store_append(store, &iter_parent, NULL);
    gtk_tree_store_set(store, &iter_parent,
            0, parent,
            -1);

    vector<string>::iterator it;
    for(it=child.begin(); it < child.end(); it++)
    {
        gtk_tree_store_append(store, &iter_child, &iter_parent);
        gtk_tree_store_set(store, &iter_child,
                0, (*it).c_str(),
                -1);
    }
}

void ViewCD::AppendChild(const char* parent, const char* child)
{
    gboolean valid, exist;
    GtkTreeIter iter_parent, iter_child;
    gchar *val;
    GtkTreeStore *store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeTo)));

    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter_parent);
    exist = FALSE;
    // Check whether parent exist
    while(valid)
    {
        gtk_tree_model_get(GTK_TREE_MODEL(store), &iter_parent, 0, &val, -1);
        if(strcmp(parent, val)==0)
        {
            exist = TRUE;
            break;
        }
        valid =  gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter_parent);
    }

    if(!exist)
    {
        gtk_tree_store_append(store, &iter_parent, NULL);
        gtk_tree_store_set(store, &iter_parent,
                0, parent,
                -1);
    }

    valid = gtk_tree_model_iter_children(GTK_TREE_MODEL(store), &iter_child, &iter_parent);
    exist = FALSE;
    // Check whether child exist
    while(valid)
    {
        gtk_tree_model_get(GTK_TREE_MODEL(store), &iter_child, 0, &val, -1);
        if(strcmp(child, val)==0)
        {
            exist = TRUE;
            break;
        }
        valid =  gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter_child);
    }

    if(!exist)
    {
        gtk_tree_store_append(store, &iter_child, &iter_parent);
        gtk_tree_store_set(store, &iter_child,
                0, child,
                -1);
    }
}

void ViewCD::BtnAddClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter1, iter2;
    gboolean valid;
    char *val1, *val2;
    GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(m_treeFrom));
    if (gtk_tree_selection_get_selected(selection, &model, &iter1) != TRUE)
        return;

    GtkTreePath *treePath = gtk_tree_model_get_path(model, &iter1);
    char *path_string = gtk_tree_path_to_string(treePath);
    int row = atoi(path_string);
    vector<string>::iterator iter = find(m_vecFolderNew.begin(), m_vecFolderNew.end(), m_vecFolderNew[row]);
    if(iter == m_vecFolderNew.end())
        m_vecFolderNew.push_back(m_vecTotalId[row]);
    gtk_tree_path_free (treePath);
    g_free(path_string);

    gtk_tree_model_get(model, &iter1, 0, &val1, -1);
    if( gtk_tree_model_iter_has_child(model, &iter1) )
    {
        vector<string> vec;
        valid = gtk_tree_model_iter_children(model, &iter2, &iter1);
        while(valid)
        {
            gtk_tree_model_get(model, &iter2, 0, &val2, -1);
            vec.push_back(val2);
            valid = gtk_tree_model_iter_next(model, &iter2);
        }
        AppendParent(val1, vec);
    }
    else
    {
        if( gtk_tree_model_iter_parent(model, &iter2, &iter1) )	
        {
            gtk_tree_model_get(model, &iter2, 0, &val2, -1);
            AppendChild(val2, val1);
        }
    }

    UpdateSize(m_iscd);
}

void ViewCD::BtnAddAllClicked(GtkButton *button)
{
    BtnDelAllClicked(NULL);

    m_vecFolderNew.clear();
    vector<string>::iterator iter = m_vecTotalId.begin();
    while (iter < m_vecTotalId.end()) 
    {
        m_vecFolderNew.push_back((*iter).c_str());
        iter++;
    }

    GtkTreeIter iter1, iter2;
    char *val1, *val2;
    gboolean valid1, valid2;
    vector<string> vec;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeFrom));

    valid1 = gtk_tree_model_get_iter_first(model, &iter1);
    while(valid1)
    {
        vec.clear();
        gtk_tree_model_get(model, &iter1, 0, &val1, -1);
        valid2 = gtk_tree_model_iter_children(model, &iter2, &iter1);
        while(valid2)
        {
            gtk_tree_model_get(model, &iter2, 0, &val2, -1);
            vec.push_back(val2);
            valid2 = gtk_tree_model_iter_next(model, &iter2);
            AppendParent(val1, vec);
        }
        valid1 = gtk_tree_model_iter_next(model, &iter1);
    }

    UpdateSize(m_iscd);
}

void ViewCD::BtnDelClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter1, iter2;

    GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(m_treeTo));
    if (gtk_tree_selection_get_selected(selection, &model, &iter1) != TRUE)
        return;

    if( gtk_tree_model_iter_parent(model, &iter2, &iter1) )
    {
        gtk_tree_store_remove (GTK_TREE_STORE(model), &iter1);
        if( ! gtk_tree_model_iter_has_child(model, &iter2) )
            gtk_tree_store_remove (GTK_TREE_STORE(model), &iter2);
    }
    else
    {
        GtkTreePath *treePath = gtk_tree_model_get_path(model, &iter1);
        char *path_string = gtk_tree_path_to_string(treePath);
        int row = atoi(path_string);
        vector<string>::iterator iter = find(m_vecFolderNew.begin(), m_vecFolderNew.end(), m_vecFolderNew[row]);
        if(iter != m_vecFolderNew.end())
            m_vecFolderNew.erase(iter);	//find and erase it from the vector
        gtk_tree_path_free (treePath);
        g_free(path_string);

        gtk_tree_store_remove (GTK_TREE_STORE(model), &iter1);
    }

    UpdateSize(m_iscd);
}

void ViewCD::BtnDelAllClicked(GtkButton *button)
{
    if(!m_vecFolderNew.empty())
        m_vecFolderNew.clear();

    if(!m_vecPathNew.empty())
        m_vecPathNew.clear();

    GtkTreeStore *store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeTo)));
    gtk_tree_store_clear(store);


    UpdateSize(m_iscd);
}

gboolean ViewCD::PrepareData(vector<string> folder, vector<string> path)
{
    char buf[256];
    vector<string>::iterator it;	
    struct stat filestat;

    FILE *fp = fopen(LIST_PATH, "w");

    if(stat(TMPDIR, &filestat) != 0)
    {
        if(mkdir(TMPDIR, 0755) != 0)
        {
            perror("mkir error: ");
            return FALSE;
        }
    }
    else
    {
        if(S_ISREG(filestat.st_mode))
        {
            if(unlink(TMPDIR) != 0)
            {
                perror("unlink error: ");
                return FALSE;
            }
            if(mkdir(TMPDIR, 0755) != 0)
            {
                perror("mkdir error: ");
                return FALSE;
            }
        }
    }

    for(it=folder.begin(); it<folder.end(); it++)
    {
        gchar *basename = g_path_get_basename((*it).c_str());
        sprintf(buf, "/%s=%s\n", basename, TMPDIR);
        PRINTF("buf: %s\n", buf);
        fputs(buf, fp);
        g_free(basename);
    }

    for(it=path.begin(); it<path.end(); it++)
    {
        gchar *basename = g_path_get_basename((*it).c_str());
        gchar *dirname = g_path_get_dirname((*it).c_str());
        gchar *parent = g_path_get_basename(dirname);
        //	PRINTF("basename: %s, dirname: %s, parent: %s\n", basename, dirname, parent);

        sprintf(buf, "%s//%s=%s/%s\n", parent, basename, dirname, basename);
        PRINTF("buf: %s\n", buf);
        fputs(buf, fp);
        g_free(basename);
        g_free(dirname);
        g_free(parent);
    }

    fclose(fp);
    return TRUE;
}

#if 0
gboolean ViewCD::MakeIsoCallback(GIOChannel *source, GIOCondition condition)
{
    gchar *buffer = NULL, *f;
    GIOStatus status;

    if (condition == G_IO_HUP || condition == G_IO_ERR) 
    {
        perror("MakeIso GIOChannel error:");
        m_cont = 0;
        return TRUE;
    }

    status = g_io_channel_read_line(source, &buffer, NULL, NULL, NULL);
    if (!buffer || status == G_IO_STATUS_ERROR || status == G_IO_STATUS_AGAIN)
    {
        return TRUE;
    }
    PRINTF("%s: %s", __FUNCTION__, buffer);

    if (strstr(buffer, MKISOFS_ESTIMATE) && buffer[3]=='.') //progress of mkisofs
    {
        buffer[3] = 0;
        f = buffer;

        while (*f==' ' || *f==9) f++;	
        PRINTF("mkisofs progress: %s\n", f);

#if 0
        gdouble pct = 0.01 * atof(f);
        gtk_progress_bar_set_fraction(progressbar2, pct);
        g_snprintf(Lsbuf, sizeof(Lsbuf)-1, "%s%%", f);
        gtk_progress_bar_set_text(Lprogressbar2, Lsbuf);
        /* avancement total */
        Ltava = (1.0 / (*Ltodo)) * ((*Ldone)-1+Lpct);
        gtk_progress_bar_set_fraction(Lprogressbar, Ltava);
        g_snprintf(Lsbuf2, sizeof(Lsbuf2)-1, "%.0f%%", Ltava*100);
        gtk_progress_bar_set_text(Lprogressbar, Lsbuf2);
#endif
    }
    else if(strstr(buffer, MKISOFS_ERROR) && !strstr(buffer, MKISOFS_IGNORE))	//error with mkisofs
    {
        m_cont = 0;
        if (!strstr(buffer, MKISOFS_NOSPACE)) 
        {
            PRINTF("Not enough free disk space to create iso image!\n");
            g_free(buffer);
            return FALSE;
        }
    }
    else if(!strncmp(buffer, MKISOFS_OK, strlen(MKISOFS_OK))) 
    {
        m_cont = 0;
        PRINTF("mkisofs progress: 100%%\n");
#if 0
        gtk_progress_bar_set_fraction(Lprogressbar2, 1);
        gtk_progress_bar_set_text(Lprogressbar2, "100%");
        Ltava = (1.0 / (*Ltodo)) * (*Ldone);
        gtk_progress_bar_set_fraction(Lprogressbar, Ltava);
        g_snprintf(Lsbuf2, sizeof(Lsbuf2)-1, "%.0f%%", Ltava*100);
        gtk_progress_bar_set_text(Lprogressbar, Lsbuf2);
#endif
    }

    g_free(buffer);
    return TRUE;
}

gboolean HandleMakeIsoCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
    return ViewCD::GetInstance()->MakeIsoCallback(source, condition);
}

gboolean ViewCD::MakeIso()
{
    gboolean status = FALSE;
    gint cmdstatus = 0;
    gint g_out, g_err, arg, pid;
    guint event, eventerr;
    gchar *cmdline, **cmd;
    GIOChannel *comerr, *com;
    //	gint64 last_track_addr, next_writable_addr;

    if(!PeripheralMan::GetInstance()->CheckDisc())
        return FALSE;

    PeripheralMan::DiscInfo info;
    if(!PeripheralMan::GetInstance()->GetDiscInfo(&info))
        return FALSE;

    if(info.is_blank)
        cmdline = g_strdup_printf("/usr/bin/mkisofs -r -J -o %s -graft-points -path-list %s", ISOPATH, PATHLIST);
    else
    {
        if(info.last_track_addr < info.next_writable_addr && info.next_writable_addr)
            cmdline = g_strdup_printf("/usr/bin/mkisofs -r -J -o %s -C %lld,%lld -M %s -graft-points -path-list %s", 
                    ISOPATH, info.last_track_addr, info.next_writable_addr, info.device.c_str(), PATHLIST);
        else
            return FALSE;
    }
    PRINTF("cmdline: %s\n", cmdline);

    status = g_shell_parse_argv(cmdline, &arg, &cmd, NULL);
    g_free(cmdline);
    if(status == FALSE)
    {
        PRINTF("MakeISO error: shell pares argv\n");
        return FALSE;
    }
    //	PRINTF("cmd: %s arg num=%d\n", cmd, arg);

    GError *error = NULL;
    status = g_spawn_async_with_pipes(NULL, cmd, NULL, 
            (GSpawnFlags ) (G_SPAWN_DO_NOT_REAP_CHILD), 
            NULL, NULL, &pid, NULL, &g_out, &g_err, &error);
    g_strfreev(cmd);
    if(status == FALSE)
    {
        PRINTF("MakeISO error: %s\n", error->message);
        perror("g_spawn_async_with_pipes");
        return FALSE;
    }

    m_cont = 1;

    com = g_io_channel_unix_new(g_out);
    g_io_channel_set_encoding(com, NULL, NULL);
    g_io_channel_set_flags(com, G_IO_FLAG_NONBLOCK, NULL);
    event = g_io_add_watch (com, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
            HandleMakeIsoCallback, NULL);

    comerr = g_io_channel_unix_new(g_err);
    g_io_channel_set_encoding(comerr, NULL, NULL);
    g_io_channel_set_flags(comerr, G_IO_FLAG_NONBLOCK, NULL);
    eventerr = g_io_add_watch (comerr, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
            HandleMakeIsoCallback, NULL);

    while(m_cont)
    {
        gtk_main_iteration();
    }

    /* exit mkisofs process	*/
    waitpid(pid, &cmdstatus, 0);
    if ((WIFEXITED(cmdstatus) && WEXITSTATUS(cmdstatus)!=0) //command exit normal with error
            || (!(WIFEXITED(cmdstatus))))	//command exit abnormal
    {
        PRINTF("mkisofs command exit with error\n");
        return FALSE;
    }
    PRINTF("mkisofs finished\n");

    g_source_remove(eventerr);
    g_source_remove(event);

    g_io_channel_shutdown(comerr, FALSE, NULL);
    g_io_channel_unref(comerr);  
    g_io_channel_shutdown(com, FALSE, NULL);
    g_io_channel_unref(com);
    g_spawn_close_pid(pid);

    return TRUE;
}

gboolean ViewCD::BurnCDCallback(GIOChannel *source, GIOCondition condition)
{
    gchar *buffer = NULL;
    GIOStatus status;

    if (condition == G_IO_HUP || condition == G_IO_ERR) 
    {
        perror("BurnCD GIOChannel error:");
        m_cont = 0;
        return TRUE;
    }

    status = g_io_channel_read_line(source, &buffer, NULL, NULL, NULL);
    if (!buffer || status == G_IO_STATUS_ERROR || status == G_IO_STATUS_AGAIN)
    {
        return TRUE;
    }
    PRINTF("%s: %s", __FUNCTION__, buffer);

    return TRUE;
}

gboolean HandleBurnCDCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
    return ViewCD::GetInstance()->BurnCDCallback(source, condition);
}

gboolean ViewCD::BurnCD(const char *device)
{
    gboolean status = FALSE;
    gchar *cmdline, **cmd;
    gint cmdstatus = 0;
    gint g_out, g_err, pid, arg;
    guint event, eventerr;
    GIOChannel *comerr, *com;

    cmdline = g_strdup_printf("/usr/bin/cdrecord -v dev=%s -multi -eject %s", device, ISOPATH);

    status = g_shell_parse_argv(cmdline, &arg, &cmd, NULL);
    g_free(cmdline);
    if(status == FALSE)
    {
        PRINTF("BurnCD error: shell pares argv\n");
        return FALSE;
    }
    //	PRINTF("cmd: %s arg num=%d\n", cmd, arg);

    status = g_spawn_async_with_pipes(NULL, cmd, NULL, 
            (GSpawnFlags ) (G_SPAWN_DO_NOT_REAP_CHILD), 
            NULL, NULL, &pid, NULL, &g_out, &g_err, NULL);
    g_strfreev(cmd);
    if(status == FALSE)
    {
        PRINTF("BurnCD error: g_spawn_async_with_pipes\n");
        return FALSE;
    }

    m_cont = 1;

    com = g_io_channel_unix_new(g_out);
    g_io_channel_set_encoding(com, NULL, NULL);
    g_io_channel_set_flags(com, G_IO_FLAG_NONBLOCK, NULL);
    event = g_io_add_watch (com, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
            HandleBurnCDCallback, NULL);

    comerr = g_io_channel_unix_new(g_err);
    g_io_channel_set_encoding(comerr, NULL, NULL);
    g_io_channel_set_flags(comerr, G_IO_FLAG_NONBLOCK, NULL);
    eventerr = g_io_add_watch (comerr, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
            HandleBurnCDCallback, NULL);

    while(m_cont)
    {
        gtk_main_iteration();
    }

    /* exit mkisofs process	*/
    waitpid(pid, &cmdstatus, 0);
    if ((WIFEXITED(cmdstatus) && WEXITSTATUS(cmdstatus)!=0) //command exit normal with error
            || (!(WIFEXITED(cmdstatus))))	//command exit abnormal
    {
        PRINTF("BurnCD command exit with error\n");
        return FALSE;
    }
    PRINTF("BurnCD finished\n");

    g_source_remove(eventerr);
    g_source_remove(event);

    g_io_channel_shutdown(comerr, FALSE, NULL);
    g_io_channel_unref(comerr);  
    g_io_channel_shutdown(com, FALSE, NULL);
    g_io_channel_unref(com);
    g_spawn_close_pid(pid);

    return TRUE;
}

gboolean ViewCD::BurnDVDCallback(GIOChannel *source, GIOCondition condition)
{
    gchar *buffer = NULL;
    GIOStatus status;

    if (condition == G_IO_HUP || condition == G_IO_ERR) 
    {
        perror("BurnDVD GIOChannel error:");
        m_cont = 0;
        return TRUE;
    }

    status = g_io_channel_read_line(source, &buffer, NULL, NULL, NULL);
    if (!buffer || status == G_IO_STATUS_ERROR || status == G_IO_STATUS_AGAIN)
    {
        return TRUE;
    }
    PRINTF("%s: %s", __FUNCTION__, buffer);

    return TRUE;
}

gboolean HandleBurnDVDCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
    return ViewCD::GetInstance()->BurnDVDCallback(source, condition);
}

gboolean ViewCD::BurnDVD(gboolean is_blank, const char *device)
{
    gboolean status = FALSE;
    gchar *cmdline, **cmd;
    gint cmdstatus = 0;
    gint g_out, g_err, pid, arg;
    guint event, eventerr;
    GIOChannel *comerr, *com;

    if(is_blank)
        cmdline = g_strdup_printf("/usr/bin/growisofs -Z %s -R -J -graft-points -path-list %s", device, PATHLIST);
    else
        cmdline = g_strdup_printf("/usr/bin/growisofs -M %s -R -J -graft-points -path-list %s", device, PATHLIST);

    status = g_shell_parse_argv(cmdline, &arg, &cmd, NULL);
    g_free(cmdline);
    if(status == FALSE)
    {
        PRINTF("BurnDVD error: shell pares argv\n");
        return FALSE;
    }
    //	PRINTF("cmd: %s arg num=%d\n", cmd, arg);

    status = g_spawn_async_with_pipes(NULL, cmd, NULL, 
            (GSpawnFlags ) (G_SPAWN_DO_NOT_REAP_CHILD), 
            NULL, NULL, &pid, NULL, &g_out, &g_err, NULL);
    g_strfreev(cmd);
    if(status == FALSE)
    {
        PRINTF("BurnDVD error: g_spawn_async_with_pipes\n");
        return FALSE;
    }

    m_cont = 1;

    com = g_io_channel_unix_new(g_out);
    g_io_channel_set_encoding(com, NULL, NULL);
    g_io_channel_set_flags(com, G_IO_FLAG_NONBLOCK, NULL);
    event = g_io_add_watch (com, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
            HandleBurnDVDCallback, NULL);

    comerr = g_io_channel_unix_new(g_err);
    g_io_channel_set_encoding(comerr, NULL, NULL);
    g_io_channel_set_flags(comerr, G_IO_FLAG_NONBLOCK, NULL);
    eventerr = g_io_add_watch (comerr, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
            HandleBurnDVDCallback, NULL);

    while(m_cont)
    {
        gtk_main_iteration();
    }

    /* exit mkisofs process	*/
    waitpid(pid, &cmdstatus, 0);
    if ((WIFEXITED(cmdstatus) && WEXITSTATUS(cmdstatus)!=0) //command exit normal with error
            || (!(WIFEXITED(cmdstatus))))	//command exit abnormal
    {
        PRINTF("BurnDVD command exit with error\n");
        return FALSE;
    }
    PRINTF("BurnDVD finished\n");

    g_source_remove(eventerr);
    g_source_remove(event);

    g_io_channel_shutdown(comerr, FALSE, NULL);
    g_io_channel_unref(comerr);  
    g_io_channel_shutdown(com, FALSE, NULL);
    g_io_channel_unref(com);
    g_spawn_close_pid(pid);

    return TRUE;
}

#endif
// gboolean TimeoutHintDialog(gpointer data)
// {
//     ViewHintDialog::GetInstance()->Create(GTK_WINDOW(ViewCD::GetInstance()->GetWindow()), _((char*)data));

//     return FALSE;
// }

gboolean TimeoutInfoDialog(gpointer data)
{
    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewCD::GetInstance()->GetWindow()),
            ViewDialog::INFO,
            _((char*)data),
            NULL);

    return FALSE;
}

gboolean TimeoutUpdateDialog(gpointer data)
{
    ViewDialog::GetInstance()->SetText(_((char*)data));

    return FALSE;
}

void ViewCD::HintDialog(guint timeout, int type, const char *info)
{
    // if(type==ViewDialog::HINT)
    // 	g_timeout_add(timeout, TimeoutHintDialog, (void*)info);
    if(type==ViewDialog::INFO)
        g_timeout_add(timeout, TimeoutInfoDialog, (void*)info);
    else if(type < 0)
        g_timeout_add(timeout, TimeoutUpdateDialog, (void*)info);
    else
        g_timeout_add(timeout, TimeoutInfoDialog, (void*)info);
}

void ViewCD::BtnBackupClicked(GtkButton *button)
{
    if(m_vecFolder.size()==0 || m_vecPath.size()==0)
    {
        PRINTF("No file selected!\n");
        const char *buf_info = "No any files selected!\nPlease select files to export first.";
        HintDialog(10, (int)ViewDialog::INFO, buf_info);
        return;
    }

    if(!PrepareData(m_vecFolder, m_vecPath))
    {
        PRINTF("PrepareData Error!\n");
        const char *buf_info = "Failed to prepare data!";
        HintDialog(10, (int)ViewDialog::INFO, buf_info);
        return;
    }

    if(m_remain <= 0)
    {
        PRINTF("The remain space not enough!\n");
        const char *buf_info = "No enough space!";
        HintDialog(10, (int)ViewDialog::INFO, buf_info);
        return;
    }

    CdBurn::GetInstance()->StartBurn(LIST_PATH);
}


void ViewCD::BtnExportClicked(GtkButton *button)
{
    if(m_vecFolder.size()==0 || m_vecPath.size()==0)
    {
        PRINTF("No file selected!\n");
        const char *buf_info = "No any files selected!\nPlease select files to export first.";
        HintDialog(10, (int)ViewDialog::INFO, buf_info);
        return;
    }

    if(m_remain <= 0)
    {
        PRINTF("The remain space not enough!\n");
        const char *buf_info = "No enough space!";
        HintDialog(10, (int)ViewDialog::INFO, buf_info);
        return;
    }
  
    ViewArchive::GetInstance()->StartUsbExport(GetWindow());
}

void ViewCD::BtnEraseClicked(GtkButton *button)
{
}

void ViewCD::BtnExitClicked(GtkButton *button)
{
    DestroyWindow();
}

