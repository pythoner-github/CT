#include <dirent.h>
#include "gui_func.h"
#include "gui_global.h"
#include "FileMan.h"
#include "KeyValueOpr.h"
#include "KeyDef.h"
#include "FakeXEvent.h"
#include "PeripheralMan.h"
#include "ViewDialog.h"
#include "ViewArchive.h"
#include "ViewArchiveImgMan.h"
#include "ConfigToUSB.h"
#include "ViewSystem.h"

ConfigToUSB* ConfigToUSB::m_ptrInstance = NULL;

ConfigToUSB::ConfigToUSB()
{
    m_listBranch = NULL;
    m_vecPath.clear();
}

ConfigToUSB::~ConfigToUSB()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

ConfigToUSB* ConfigToUSB::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ConfigToUSB;

    return m_ptrInstance;
}

void ConfigToUSB::CreateWindow(GtkWindow *parent)
{
    GtkWidget *fixed;
    GtkWidget *btnOK, *btnCancel;
    GtkWidget *swRoot, *swBranch;
    GtkTreeModel *modelRoot;

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, 640, 480);
    gtk_window_set_title (GTK_WINDOW (m_window), _("Data Selection"));
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(m_window), parent);
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    swRoot = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (swRoot);
    gtk_fixed_put (GTK_FIXED (fixed), swRoot, 20, 10);
    gtk_widget_set_size_request (swRoot, 200, 300);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swRoot), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (swRoot), GTK_SHADOW_IN);

    m_treeRoot = create_treeview(0);
    gtk_widget_show (m_treeRoot);
    gtk_container_add (GTK_CONTAINER (swRoot), m_treeRoot);
    GtkTreeSelection *selectRoot = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeRoot));
    gtk_tree_selection_set_mode(selectRoot, GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(selectRoot), "changed", G_CALLBACK(on_root_selection_changed), this);

    modelRoot = create_root_model();
    if (modelRoot != NULL)
        gtk_tree_view_set_model (GTK_TREE_VIEW(m_treeRoot), modelRoot);
    g_object_unref (modelRoot); 

    swBranch = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (swBranch);
    gtk_fixed_put (GTK_FIXED (fixed), swBranch, 220, 10);
    gtk_widget_set_size_request (swBranch, 400, 300);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swBranch), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (swBranch), GTK_SHADOW_IN);

    m_treeBranch = create_treeview(1);
    gtk_widget_show (m_treeBranch);
    gtk_container_add (GTK_CONTAINER (swBranch), m_treeBranch);

  
    GtkWidget *imageOK = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
    GtkWidget *labelOK = gtk_label_new_with_mnemonic (_("OK"));
    btnOK = create_button_icon(labelOK, imageOK);
    gtk_fixed_put (GTK_FIXED (fixed), btnOK, 360, 410);
    g_signal_connect ( G_OBJECT(btnOK), "clicked", G_CALLBACK (on_button_ok_clicked), this);

    GtkWidget *imageCancel = gtk_image_new_from_stock (GTK_STOCK_CANCEL, GTK_ICON_SIZE_BUTTON);
    GtkWidget *labelCancel = gtk_label_new_with_mnemonic (_("Cancel"));
    btnCancel = create_button_icon(labelCancel, imageCancel);
    gtk_fixed_put (GTK_FIXED (fixed), btnCancel, 500, 410);
    g_signal_connect ( G_OBJECT(btnCancel), "clicked", G_CALLBACK (on_button_cancel_clicked), this);

    UpdateRootModel();

    gtk_widget_show_all(m_window);

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();

}

void ConfigToUSB::DestroyWindow(void)
{
    if(g_list_length(m_listBranch) > 0)
    {
        //clean the data in list
        m_listBranch = g_list_first(m_listBranch);
        while(m_listBranch)
        {
            GtkTreeModel *model = NULL;
            if(m_listBranch->data)
                model = GTK_TREE_MODEL(m_listBranch->data);
            m_listBranch = g_list_remove(m_listBranch, GTK_TREE_MODEL(m_listBranch->data));
            if(model)
                g_object_unref(model);
        }
        //	PRINTF("%s: List length is %d\n", __FUNCTION__, g_list_length(m_listBranch));
        g_list_free(m_listBranch);
        m_listBranch = NULL;
    }

    //	m_vecPath.clear();

    if(GTK_IS_WIDGET(m_window)) {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        m_window = NULL;
    }
}

gboolean ConfigToUSB::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWindow();
    return FALSE;
}

void ConfigToUSB::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);

    switch(keyValue) {
        case KEY_ESC:
            BtnCancelClicked(NULL);
            break;
        default:
            break;
    }
}

static GCancellable* cancellable = NULL;

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

static gboolean LoadSelectedData(gpointer data)
{
    int cond = 0;
    int count = 1;
    int total = 0;
    char str_info[256], result[256];
    FileMan fm;
    PeripheralMan *ptr = PeripheralMan::GetInstance();

    vector<string> vec = ConfigToUSB::GetInstance()->GetSelectedVec();

    if(!ptr->CheckUsbStorageState())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::ERROR,
                _("No USB storage found!"), 
                NULL);
        return FALSE;
    }
    else
    {
        if(!ptr->MountUsbStorage())
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                    ViewDialog::ERROR,
                    _("Failed to mount USB storage!"), 
                    NULL);
            return FALSE;
        }
    }

    cancellable = g_cancellable_new();

    //list all string for test
    vector<string>::iterator ite = vec.begin();
    total = vec.size() / 2; // 1 image = 1 pic + 1 ini
    while(ite < vec.end() && !cond)
    {
        PRINTF("Send file: %s\n", (*ite).c_str());
        GFile *fAbs = g_file_new_for_path((*ite).c_str());
        GFile *fParent = g_file_get_parent(fAbs);
        g_object_unref(fAbs);
        gchar *strDestParent = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, g_file_get_basename(fParent), NULL);
        g_object_unref(fParent);
        GFile *fDest = g_file_new_for_path(strDestParent);

        //create the parent directory
        GError *err_mkdir = NULL;
        if(!g_file_make_directory_with_parents(fDest, NULL, &err_mkdir))
        {
            if(err_mkdir->code!=G_IO_ERROR_EXISTS)
            {
                PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
                sprintf(result, _("Failed to send data to USB storage!\nError: Failed to create directory."));
                cond = -1;
                g_error_free(err_mkdir);
                g_object_unref(fDest);
                g_free(strDestParent);
                break;
            }
        }
        g_object_unref(fDest);

        //Update info
        gchar *basename = g_path_get_basename((*ite).c_str());
        if(fm.CompareSuffix(basename, "ini") != 0)
        {
            sprintf(str_info, "%s %s   %d/%d\n%s", _("Loading..."), basename, count, total, _("Please wait..."));
            ViewDialog::GetInstance()->SetText(str_info);
            ViewDialog::GetInstance()->SetProgressBar(0);
            count++;
        }

        //Perform copy operation
        gchar *destPath = g_build_path(G_DIR_SEPARATOR_S, strDestParent, basename, NULL);
        g_free(strDestParent);
        g_free(basename);
        PRINTF("Dest Path: %s\n", destPath);
        GFile *src = g_file_new_for_path((*ite).c_str());
        GFile *dest = g_file_new_for_path(destPath);
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
                    sprintf(result, _("Failed to send data to USB storage!\nError: No space left on storage."));
                    break;
                case G_IO_ERROR_CANCELLED:
                    sprintf(result, _("Failed to send data to USB storage!\nError: Operation was cancelled."));
                    break;
                default:
                    sprintf(result, _("Failed to send data to USB storage!"));
                    break;
            }
            cond = -1;
            g_error_free(err);
            break;
        }
        ite++;
    }

    ptr->UmountUsbStorage();
    ViewDialog::GetInstance()->Destroy();

    //Handle result
    if(!cond)
    {
        sprintf(result, _("Success to export to USB storage."));
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO,
                result, 
                NULL);

    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::INFO,
                result, 
                NULL);
    }

    return FALSE;
}

static int CancelLoadUSB(gpointer data)
{
    PRINTF("Cancel copy!\n");
    g_cancellable_cancel(cancellable);
    return 0;
}

void ConfigToUSB::BtnOKClicked(GtkButton *button)
{
    //Copy the selected file to SLIDE_PATH
    if(GetAllSelectPath())
    {
        g_timeout_add(1000, LoadSelectedData, NULL);

        //	PRINTF("Load From U disk!\n");
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
                ViewDialog::PRG_CANCEL, 
                _("Please wait, sending data to USB storage..."),
                CancelLoadUSB);
    }
    DestroyWindow();
}

void ConfigToUSB::BtnCancelClicked(GtkButton *button)
{
    DestroyWindow();
}

//type: 0=root, 1=branch
GtkWidget* ConfigToUSB::create_treeview(gint type)
{
    GtkWidget *treeview;
    GtkCellRenderer *render;
    GtkTreeViewColumn *col;

    treeview = gtk_tree_view_new ();
    g_object_set(G_OBJECT(treeview), 
            "enable-search", FALSE, 
            "headers-visible", FALSE, 
            "rules-hint", TRUE, 
            NULL);

    render = gtk_cell_renderer_toggle_new();
    g_signal_connect (G_OBJECT(render), "toggled", G_CALLBACK (on_data_toggled), this);
    col = gtk_tree_view_column_new_with_attributes("", render, "active", COL_CHECKED, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);
    g_object_set_data(G_OBJECT(render), "type", GINT_TO_POINTER(type));

    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("Name"), render, "text", COL_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

    return treeview;
}

GtkTreeModel* ConfigToUSB::create_root_model()
{
    GtkListStore *store;

    store = gtk_list_store_new(NUM_COLS, G_TYPE_BOOLEAN, G_TYPE_STRING);

    return GTK_TREE_MODEL (store);
}

void ConfigToUSB::ToggleData(GtkCellRendererToggle *cell, gchar *path_str)
{
    GtkWidget *treeview;
    gint type = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "type"));
    if(type == 0)
        treeview = m_treeRoot;
    else if(type == 1)
        treeview = m_treeBranch;
    else
        return;

    //	PRINTF("Toggle path: %s\n", path_str);
    GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(treeview));
    GtkTreeIter iter;
    GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
    gboolean checked;

    /* get toggled iter */
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_tree_model_get (model, &iter, COL_CHECKED, &checked, -1);

    /* do something with the value */
    checked ^= 1; //按位异或

    /* set new value */
    gtk_list_store_set (GTK_LIST_STORE (model), &iter, COL_CHECKED, checked, -1);

    if(type == 0 ) //Root
    {
        gint rows = atoi(gtk_tree_path_to_string(path));
        UpdateBranchModel(rows);
        GList *list = g_list_first(m_listBranch);
        SetAllToggleValue(GTK_TREE_MODEL(g_list_nth_data(list, rows)), checked);
    }
    else if(type == 1) //Branch
    {
        GtkTreeIter i;
        GtkTreeModel *m;
        if(!checked)
        {
            GtkTreeSelection *s = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeRoot));
            if(gtk_tree_selection_get_selected(s, &m, &i))
                gtk_list_store_set(GTK_LIST_STORE(m), &i, COL_CHECKED, checked, -1);
        }
        else
        {
            //check all status
            gboolean checkall = TRUE;
            m = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeBranch));
            gboolean valid = gtk_tree_model_get_iter_first(m, &i);
            while(valid)
            {
                gboolean val;
                gtk_tree_model_get(m, &i, COL_CHECKED, &val, -1);
                if(!val)
                    checkall = FALSE;
                valid = gtk_tree_model_iter_next(m, &i);
            }
            if(checkall)
            {
                GtkTreeSelection *s = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeRoot));
                if(gtk_tree_selection_get_selected(s, &m, &i))
                    gtk_list_store_set(GTK_LIST_STORE(m), &i, COL_CHECKED, TRUE, -1);
            }
        }
    }

    /* clean up */
    gtk_tree_path_free (path);
}

void ConfigToUSB::UpdateRootModel(void)
{
    DIR *dir;
    struct dirent *ent;

    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeRoot)));
    GtkTreeIter iter;

    dir = opendir(USERCONFIG_PARENT_PATH);
    if(!dir)
    {
        perror("open path error");
        return;
    }

    while((ent = readdir(dir)) != NULL)
    {
        PRINTF("DIR: %s\n", ent->d_name);
        if(ent->d_name[0]=='.')
            continue;
        //	if(ent->d_type==DT_DIR)
        gchar *tmpPath = g_build_path(G_DIR_SEPARATOR_S, USERCONFIG_PARENT_PATH, ent->d_name, NULL);
        //export file of default dir or  userconfig dir 
        if(g_file_test(tmpPath, G_FILE_TEST_IS_DIR) && ((strcmp(ent->d_name, "userconfig") == 0) || (strcmp(ent->d_name, "default") == 0)))
        {
            //	PRINTF("DIR: %s\n", ent->d_name);
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                    COL_CHECKED, FALSE,
                    COL_NAME, ent->d_name,
                    -1);
            GtkTreeModel *model = GTK_TREE_MODEL(LoadBranchModel(ent->d_name));
            m_listBranch = g_list_append(m_listBranch, model);
        }
        g_free(tmpPath);
    }
    closedir(dir);
}

// rows: the row's number in the Root model
void ConfigToUSB::UpdateBranchModel(gint rows)
{
    GList *list = g_list_first(m_listBranch);
    gtk_tree_view_set_model (GTK_TREE_VIEW(m_treeBranch), GTK_TREE_MODEL(g_list_nth_data(list, rows)));
}

GtkTreeModel* ConfigToUSB::LoadBranchModel(gchar *branch)
{
    DIR *dir;
    struct dirent *ent;
    gchar *path = g_build_path(G_DIR_SEPARATOR_S, USERCONFIG_PARENT_PATH, branch, NULL);

    GtkListStore *store = gtk_list_store_new(NUM_COLS, G_TYPE_BOOLEAN, G_TYPE_STRING);
    gtk_list_store_clear(store);
    GtkTreeIter iter;

    dir = opendir(path);
    if(!dir)
    {
        perror("open path error");
        return NULL;
    }

    FileMan fm;
    gboolean flag = 0;
    while((ent = readdir(dir)) != NULL)
    {
        if(ent->d_name[0]=='.')
            continue;
        //	if(ent->d_type==DT_REG)
        gchar *tmpPath = g_build_path(G_DIR_SEPARATOR_S, path, ent->d_name, NULL);
        if(g_file_test(tmpPath, G_FILE_TEST_IS_REGULAR))
        {
            //	if(fm.CompareSuffix(ent->d_name, "jpg")==0 || fm.CompareSuffix(ent->d_name, "bmp")==0 || fm.CompareSuffix(ent->d_name, "emp")==0 || fm.CompareSuffix(ent->d_name, "avi")==0 || fm.CompareSuffix(ent->d_name, "cine")==0)
            if (fm.CompareSuffix(ent->d_name, "ini")==0)
            {
                //	PRINTF("	FILE: %s\n", ent->d_name);
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter,
                        COL_CHECKED, FALSE,
                        COL_NAME, ent->d_name,
                        -1);
                flag = 1;
            }
        }
        g_free(tmpPath);
    }
    closedir(dir);
    g_free(path);

    if(flag)
        return GTK_TREE_MODEL(store);
    else
        return NULL;
}

void ConfigToUSB::RootSelectionChanged(GtkTreeSelection *selection)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *text;

    if(gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gtk_tree_model_get(model, &iter, COL_NAME, &text, -1);
        GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
        //	PRINTF("Selection path: %s\n", gtk_tree_path_to_string(path));
        UpdateBranchModel(atoi(gtk_tree_path_to_string(path)));
        gtk_tree_path_free (path);
        g_free(text);
    }
}

void ConfigToUSB::SetAllToggleValue(GtkTreeModel *model, gboolean value)
{
    gboolean valid;
    GtkTreeIter iter;

    if(!model)
        return;

    valid = gtk_tree_model_get_iter_first(model, &iter);

    while(valid)
    {
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_CHECKED, value, -1);
        valid = gtk_tree_model_iter_next(model, &iter);
    }
}

gboolean ConfigToUSB::CheckBranchStauts(void)
{
    return FALSE;
}

gboolean ConfigToUSB::GetAllSelectPath(void)
{
    m_vecPath.clear();

    gchar *nameRoot, *nameBranch;
    gboolean checkedRoot, checkedBranch;
    gboolean validRoot, validBranch;
    GtkTreeModel *modelRoot, *modelBranch;
    GtkTreeIter iterRoot, iterBranch;
    FileMan fm;

    modelRoot = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeRoot));
    validRoot = gtk_tree_model_get_iter_first(modelRoot, &iterRoot);

    if(g_list_length(m_listBranch) > 0)
    {
        GList *list = g_list_first(m_listBranch);
        while(list)
        {
            //	PRINTF("New Branch\n");
            if(!validRoot || !list->data)
            {
                validRoot = gtk_tree_model_iter_next(modelRoot, &iterRoot);
                list = list->next;
                continue;
            }

            gtk_tree_model_get(modelRoot, &iterRoot, COL_CHECKED, &checkedRoot, COL_NAME, &nameRoot, -1);
            //	PRINTF("Root Name=%s\n", nameRoot);

            modelBranch = GTK_TREE_MODEL(list->data);
            validBranch = gtk_tree_model_get_iter_first(modelBranch, &iterBranch);
            //check all iter in branch list
            while(validBranch)
            {
                gtk_tree_model_get(modelBranch, &iterBranch, COL_CHECKED, &checkedBranch, COL_NAME, &nameBranch, -1);
                //	PRINTF("Branch Name=%s, Checked=%d\n", nameBranch, checkedBranch);
                if(checkedBranch)
                {
                    gchar *path = g_build_path(G_DIR_SEPARATOR_S, USERCONFIG_PARENT_PATH, nameRoot, nameBranch, NULL);
                    //		PRINTF("Push to vector: %s\n", path);
                    m_vecPath.push_back(path);
                    gchar pathIni[255];
                    fm.GetIniFilePath(path, pathIni);
                    //			PRINTF("Ini path is: %s\n", pathIni);
                    m_vecPath.push_back(pathIni);

                    g_free(path);
                }
                validBranch = gtk_tree_model_iter_next(modelBranch, &iterBranch);
                g_free(nameBranch);
            }

            validRoot = gtk_tree_model_iter_next(modelRoot, &iterRoot);
            g_free(nameRoot);
            list = list->next;
        }
    }

    if(!m_vecPath.empty())
        return TRUE;
    else
        return FALSE;
}

#if 0
void ConfigToUSB::BtnSelectAllClicked(GtkButton *button)
{
}

void ConfigToUSB::BtnDeselectClicked(GtkButton *button)
{
}
#endif
