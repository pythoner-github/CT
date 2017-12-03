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
#include "ViewSystem.h"
#include "ConfigToHost.h"
#include "UserSelect.h"
#include "keyboard/KeyValueOpr.h"
#include "MenuCalcNew.h"
#include "keyboard/KeyBoard.h"

extern int s_fdcom;
ConfigToHost* ConfigToHost::m_ptrInstance = NULL;

ConfigToHost::ConfigToHost()
{
    m_listBranch = NULL;
    m_vecPath.clear();
}

ConfigToHost::~ConfigToHost()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

ConfigToHost* ConfigToHost::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ConfigToHost;

    return m_ptrInstance;
}

void ConfigToHost::CreateCalcImportWindow(GtkWindow *parent)
{
    GtkWidget *fixed;
//    GtkWidget *btnOK, *btnCancel;
    GtkWidget *swRoot, *swBranch;
    GtkTreeModel *modelRoot;
    GtkWidget *hseparator1;

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, 400, 430);
    gtk_window_set_title (GTK_WINDOW (m_window), _("Import"));
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(m_window), parent);
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    GtkWidget *label_calc_notice = gtk_label_new (_("Please select one item:"));
    gtk_misc_set_alignment (GTK_MISC(label_calc_notice), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_calc_notice), TRUE);
    gtk_widget_show (label_calc_notice);
    gtk_fixed_put (GTK_FIXED (fixed), label_calc_notice, 20, 5);
    gtk_widget_set_size_request (label_calc_notice, 300, 30);


    swRoot = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (swRoot);
    gtk_fixed_put (GTK_FIXED (fixed), swRoot, 20, 40);
    gtk_widget_set_size_request (swRoot, 360, 140);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swRoot), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (swRoot), GTK_SHADOW_IN);

    m_treeRoot = create_calc_treeview();
    gtk_widget_show (m_treeRoot);
    gtk_container_add (GTK_CONTAINER (swRoot), m_treeRoot);
    GtkTreeSelection *selectRoot = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeRoot));
    gtk_tree_selection_set_mode(selectRoot, GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(selectRoot), "changed", G_CALLBACK(on_root_calc_selection_changed), this);

    modelRoot = create_calc_root_model();
    if (modelRoot != NULL)
        gtk_tree_view_set_model (GTK_TREE_VIEW(m_treeRoot), modelRoot);
    g_object_unref (modelRoot);

 	m_chkbtnDel = gtk_check_button_new_with_mnemonic(_("Delete data from USB storage after import."));
	gtk_fixed_put (GTK_FIXED (fixed), m_chkbtnDel, 20, 190);
	gtk_widget_set_size_request (m_chkbtnDel, -1, 30);

    GtkWidget *label_notice = gtk_label_new (_("Import data to override the current exam data!"));
    gtk_misc_set_alignment (GTK_MISC(label_notice), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_notice), TRUE);
    gtk_widget_show (label_notice);
    gtk_fixed_put (GTK_FIXED (fixed), label_notice, 20, 220);
    gtk_widget_set_size_request (label_notice, 360, 60);

    m_progress_bar = gtk_progress_bar_new();
    gtk_widget_hide(m_progress_bar);
    gtk_fixed_put(GTK_FIXED(fixed), m_progress_bar, 60, 290);
    gtk_widget_set_size_request(m_progress_bar, 280, 20);

    img_right = gtk_image_new_from_stock (GTK_STOCK_APPLY, GTK_ICON_SIZE_BUTTON);
    gtk_widget_hide (img_right);
    gtk_fixed_put (GTK_FIXED (fixed), img_right, 40, 320);

    img_error = gtk_image_new_from_stock (GTK_STOCK_STOP, GTK_ICON_SIZE_BUTTON);
    gtk_widget_hide (img_error);
    gtk_fixed_put (GTK_FIXED (fixed), img_error, 40, 320);

    img_load = gtk_image_new_from_stock (GTK_STOCK_YES, GTK_ICON_SIZE_BUTTON);
    gtk_widget_hide (img_load);
    gtk_fixed_put (GTK_FIXED (fixed), img_load, 40, 320);

    label_calc_notice1 = gtk_label_new ("");
    gtk_widget_hide (label_calc_notice1);
    gtk_fixed_put (GTK_FIXED (fixed), label_calc_notice1, 55, 320);
    gtk_widget_set_size_request (label_calc_notice1, 265, 30);

    hseparator1 = gtk_hseparator_new ();
    gtk_widget_show (hseparator1);
    gtk_fixed_put (GTK_FIXED (fixed), hseparator1, 20, 360);
    gtk_widget_set_size_request (hseparator1, 360, 16);

    GtkWidget *imageOK = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
    GtkWidget *labelOK = gtk_label_new_with_mnemonic (_("Import"));
    btnOK = create_button_icon(labelOK, imageOK);
    gtk_fixed_put (GTK_FIXED (fixed), btnOK, 136, 385);
    gtk_widget_set_size_request (btnOK, 100, 30);
    g_signal_connect ( G_OBJECT(btnOK), "clicked", G_CALLBACK (on_button_calc_ok_clicked), this);

    GtkWidget *imageCancel = gtk_image_new_from_stock (GTK_STOCK_CANCEL, GTK_ICON_SIZE_BUTTON);
    GtkWidget *labelCancel = gtk_label_new_with_mnemonic (_("Cancel"));
    btnCancel = create_button_icon(labelCancel, imageCancel);
    gtk_fixed_put (GTK_FIXED (fixed), btnCancel, 280, 385);
    gtk_widget_set_size_request (btnCancel, 100, 30);
    g_signal_connect ( G_OBJECT(btnCancel), "clicked", G_CALLBACK (on_button_cancel_clicked), this);

    UpdateCalcRootModel();

    gtk_widget_show_all(m_window);
    gtk_widget_hide (img_right);
    gtk_widget_hide (img_error);
    gtk_widget_hide (img_load);

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();

}

void ConfigToHost::CreateWindow(GtkWindow *parent)
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

void ConfigToHost::DestroyWindow(void)
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



    if(GTK_IS_WIDGET(m_window)) {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        m_window = NULL;
    }
}

gboolean ConfigToHost::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWindow();
    return FALSE;
}

void ConfigToHost::KeyEvent(unsigned char keyValue)
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

void ConfigToHost::HideOKAndCancelClicked()
{
    gtk_widget_set_sensitive(btnOK, FALSE);
    gtk_widget_set_sensitive(btnCancel, FALSE);
}

void ConfigToHost::OKAndCancelClicked()
{
    gtk_widget_set_sensitive(btnOK, TRUE);
    gtk_widget_set_sensitive(btnCancel, TRUE);

}

vector<int> vecAdd;
int num;
int item_num_exist;
bool waittime = true;
pthread_t pidRenameWin;
static gboolean LoadSelectedCalcData()
{
    UserSelect::GetInstance()->create_import_tmp_dir();

    int cond = 0;
    int count = 1;
    int total = 0;
    char str_info[256],result1[256];
    FileMan fm;
    PeripheralMan *ptr = PeripheralMan::GetInstance();

    vector<string> vec = ConfigToHost::GetInstance()->GetSelectedVec();

    cancellable = g_cancellable_new();

    //list all string for test
    vector<string>::iterator ite = vec.begin();
    total = vec.size() / 2;
    while(ite < vec.end() && !cond)
    {
        //Update info
        gchar *basename = g_path_get_basename((*ite).c_str());

        gchar *destPath;
        gchar *destPath_other = NULL;
        char name[50]={0};

        destPath = g_build_path(G_DIR_SEPARATOR_S, CALC_TMP_DATA_PATH, basename, NULL);

        // delete the '.ini'
        strcpy(name, basename);
        int i;
        for (i =0; i < 50; i++)
        {
            if ((name[i] == '.') && (name[i+1] == 'i') && (name[i+2] =='n') && (name[i+3] == 'i'))
            {
                name[i] = name[i+1] = name[i+2] = name[i+3] = '\0';
                    break;
                }
            }
        g_free(basename);
        //	PRINTF("Dest Path: %s\n", destPath);
        GFile *src = g_file_new_for_path((*ite).c_str());
        GFile *dest = g_file_new_for_path(destPath);
        g_free(destPath);
        GFile *dest_other = NULL;
        if(destPath_other != NULL)
        {
            dest_other = g_file_new_for_path(destPath_other);
            g_free(destPath_other);
        }

        GError *err = NULL;
        int ret = g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, cancellable, NULL, NULL, &err);

        if(dest_other != NULL)
        {
            int ret_other = g_file_copy(src, dest_other, G_FILE_COPY_OVERWRITE, cancellable, progress_callback, NULL, &err);
            g_object_unref(dest_other);
            if(!ret_other)
            {
                PRINTF("g_file_copy error: %s\n", err->message);
                switch(err->code)
                {
                    case G_IO_ERROR_NO_SPACE:
                        sprintf(result1, _("Failed to load data from USB storage!\nError: No space left on storage."));
                        break;
                    case G_IO_ERROR_CANCELLED:
                        sprintf(result1, _("Failed to load data from USB storage!\nError: Operation was cancelled."));
                        break;
                    default:
                        sprintf(result1, _("Failed to load data from USB storage!"));
                        break;
                }
                cond = -1;
            g_error_free(err);
            break;
          }
        }
        g_object_unref(src);
        g_object_unref(dest);
        if(!ret)
        {
            PRINTF("g_file_copy error: %s\n", err->message);
            switch(err->code)
            {
                case G_IO_ERROR_NO_SPACE:
                    sprintf(result1, _("Failed to load data from USB storage!\nError: No space left on storage."));
                    break;
                case G_IO_ERROR_CANCELLED:
                    sprintf(result1, _("Failed to load data from USB storage!\nError: Operation was cancelled."));
                    break;
                default:
                    sprintf(result1, _("Failed to load data from USB storage!"));
                    break;
            }
            cond = -1;
            g_error_free(err);
            break;
        }
        ite++;
    }

    if(!cond)
    {
        ConfigToHost::GetInstance()->DeleteUdiskFile();
        vecAdd.clear();

        char path1[256];
        sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path1);
        ExamItem exam;
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);
        char path2[256];
        char path3[256];
        char userselectname[256];
        char userselectname1[256];

#ifdef VET
        sprintf(path2, "%s%s", CALC_TMP_DATA_PATH, "/VetCalcSetting.ini");
        sprintf(path3, "%s%s", CALC_TMP_DATA_PATH, "/VetCalcItemSetting.ini");

#else
        sprintf(path2, "%s%s", CALC_TMP_DATA_PATH, "/CalcSetting.ini");
        sprintf(path3, "%s%s", CALC_TMP_DATA_PATH, "/CalcItemSetting.ini");
#endif
        FileMan f;
        if(strcmp(username.c_str(), "System Default") == 0)
        {
            sprintf(userselectname, "%s%s", CFG_RES_PATH, CALC_FILE);
            sprintf(userselectname1, "%s%s", CFG_RES_PATH, CALC_ITEM_PATH);
        }
        else
        {
            sprintf(userselectname, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, username.c_str(), ".ini");
            sprintf(userselectname1, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, username.c_str(), ".ini");
        }

        IniFile ini_add2(userselectname1);
        IniFile *ptrIni_add2 = &ini_add2;
        int ItemAllNum;
        ItemAllNum=ptrIni_add2->ReadInt("MaxNumber", "Number");

        IniFile ini_add(path2);
        IniFile *ptrIni_add = &ini_add;

        vector<string> itemgroup_name;
        itemgroup_name = ptrIni_add->GetGroupName();
        string itemgroup = itemgroup_name[0];
        int num_add;
        num_add = ptrIni_add->ReadInt(itemgroup.c_str(), "Number");
        if(num_add != 0)
        {
            for(int i=1;i<=num_add;i++)
            {
                char CalcNumber[256];
                sprintf(CalcNumber, "Calc%d", i);
                int item_num = ptrIni_add->ReadInt(itemgroup.c_str(), CalcNumber);
                if(item_num < (USER_START - BASIC_MEA_END))
                {
                    vecAdd.push_back(item_num);
                }
                else
                {
                    bool rename_no = true;
                    IniFile ini_add1(path3);
                    IniFile *ptrIni_add1 = &ini_add1;
                    char CustomEtype[256];
                    sprintf(CustomEtype, "CustomEtype-%d",item_num);
                    string item_name = ptrIni_add1->ReadString(CustomEtype, "Name");
                    int method_index = ptrIni_add1->ReadInt(CustomEtype, "Method");
                    int type_index = ptrIni_add1->ReadInt(CustomEtype, "MeasureType");
                    string department_in = ptrIni_add1->ReadString(CustomEtype, "Department");

                    IniFile ini_add4(userselectname1);
                    IniFile *ptrIni_add4 = &ini_add4;
                    vector<string> useritemgroup;
                    useritemgroup = ptrIni_add4->GetGroupName();
                    char src_group[256];
                    int group_length(0);
                    group_length = useritemgroup.size();
                   for (int i = 0 ; i <  group_length; i++)
                    {
                        sprintf(src_group ,"%s", useritemgroup[i].c_str());
                        int number1;
                        number1 = ptrIni_add4->ReadInt(src_group, "Number");
                        for(int i=1;i<=number1;i++)
                        {
                            char CalcNumber[256];
                            sprintf(CalcNumber, "Calc%d", i);
                            item_num_exist = ptrIni_add4->ReadInt(src_group, CalcNumber);
                            string item_name1;
                            if(item_num_exist < (USER_START - BASIC_MEA_END))
                                item_name1= CalcSetting::GetInstance()->ItemMenuTransEnglish(item_num_exist);
                            else
                            {
                                item_name1 = CalcSetting::GetInstance()->CustomItemTransName(item_num_exist);
                            }
                            if(strcmp(item_name1.c_str(), item_name.c_str()) == 0)
                            {
                                rename_no = false;
                                char rename[256];
                                char renametrans[256];
                                strcpy(renametrans,  _("Item has existed, \nClicked Import will rename the item and continue, \nClicked Cancel will quit and continue!"));
                                sprintf(rename , "%s %s", item_name.c_str(), renametrans);
                                GtkWidget *dialog = gtk_dialog_new_with_buttons("Rename",
                                        GTK_WINDOW(ConfigToHost::GetInstance()->GetWindow()),
                                        GTK_DIALOG_MODAL ,
                                        GTK_STOCK_OK, GTK_RESPONSE_OK,
                                        GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
                                        NULL);
                                gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

                                GtkWidget *notice = gtk_label_new(rename);
                                gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox), notice);
                                gtk_widget_show_all(dialog);

                                gint result = gtk_dialog_run (GTK_DIALOG (dialog));

                                switch (result)
                                {
                                    case GTK_RESPONSE_OK:
                                        CustomCalc::GetInstance()->ButtonImportNameOK();
                                        break;
                                    case GTK_RESPONSE_CANCEL:
                                    default:
                                        break;
                                }
                                gtk_widget_destroy(dialog);
                            }
                        }
                    }
                   if(rename_no)
                       CustomCalc::GetInstance()->ImportWrite(item_name, item_num);
                }
            }
        }
        CustomCalc::GetInstance()->ImportSuccess();
    }
    else
    {
        ConfigToHost::GetInstance()->ExportErrorInfoNotice(result1);
        ConfigToHost::GetInstance()->OKAndCancelClicked();
    }

    return FALSE;
}

static gboolean LoadSelectedData(gpointer data)
{
    int cond = 0;
    int count = 1;
    int total = 0;
    char str_info[256], result[256];
    FileMan fm;
    PeripheralMan *ptr = PeripheralMan::GetInstance();
    vector<string> vec = ConfigToHost::GetInstance()->GetSelectedVec();
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
    total = vec.size() / 2;
    char usb_file_path[256];
    while(ite < vec.end() && !cond)
    {
        //Update info
        gchar *basename = g_path_get_basename((*ite).c_str());
        PRINTF("basename:%s\n", basename);

        // only display .ini file
        if(fm.CompareSuffix(basename, "ini") != 0)
        {
            sprintf(str_info, "%s %s   %d/%d\n%s", _("Loading..."), basename, count, total, _("Please wait..."));
            ViewDialog::GetInstance()->SetText(str_info);
            ViewDialog::GetInstance()->SetProgressBar(0);
            count++;
        }
        //lhm20140925
        gchar *destPath;
        gchar *destPath_other = NULL;
#ifdef VET
        sprintf(usb_file_path, "%s/default/VetItemPara.ini",UDISK_DATA_PATH);

#else
        sprintf(usb_file_path, "%s/default/ItemPara.ini",UDISK_DATA_PATH);
#endif
        char name[50]={0};
        if(strcmp(usb_file_path, (*ite).c_str()) == 0) //default目录下文件路径
        {
            destPath = g_build_path(G_DIR_SEPARATOR_S, USERCONFIG_PARENT_PATH, basename, NULL);
            destPath_other = g_build_path(G_DIR_SEPARATOR_S, EXAM_FILE_OTHER, NULL);
            // delete the '.ini'
            strcpy(name, "System Default");
        }
        else
        {
            destPath = g_build_path(G_DIR_SEPARATOR_S, USERCONFIG_PATH, basename, NULL);

            // delete the '.ini'
            strcpy(name, basename);
            int i;
            for (i =0; i < 50; i++)
            {
                if ((name[i] == '.') && (name[i+1] == 'i') && (name[i+2] =='n') && (name[i+3] == 'i'))
                {
                    name[i] = name[i+1] = name[i+2] = name[i+3] = '\0';
                    break;
                }
            }
            if (UserSelect::GetInstance()->username_unique_db(USERNAME_DB, name))
            {
                UserSelect::GetInstance()->insert_username_db(USERNAME_DB, name);
                ViewSystem::GetInstance()->ShowList(name);
                int last =  UserSelect::GetInstance()->get_active_user();
                ViewSystem::GetInstance()->set_active_user(last);
            }
        }
        g_free(basename);
        //	PRINTF("Dest Path: %s\n", destPath);
        GFile *src = g_file_new_for_path((*ite).c_str());
        GFile *dest = g_file_new_for_path(destPath);
        g_free(destPath);
        GFile *dest_other = NULL;
        if(destPath_other != NULL)
        {
            dest_other = g_file_new_for_path(destPath_other);
            g_free(destPath_other);
        }

        GError *err = NULL;
        int ret = g_file_copy(src, dest, G_FILE_COPY_OVERWRITE, cancellable, progress_callback, NULL, &err);

        if(dest_other != NULL)
        {
            int ret_other = g_file_copy(src, dest_other, G_FILE_COPY_OVERWRITE, cancellable, progress_callback, NULL, &err);
            g_object_unref(dest_other);
            if(!ret_other)
            {
                PRINTF("g_file_copy error: %s\n", err->message);
                switch(err->code)
                {
                    case G_IO_ERROR_NO_SPACE:
                        sprintf(result, _("Failed to load data from USB storage!\nError: No space left on storage."));
                        break;
                    case G_IO_ERROR_CANCELLED:
                        sprintf(result, _("Failed to load data from USB storage!\nError: Operation was cancelled."));
                        break;
                    default:
                        sprintf(result, _("Failed to load data from USB storage!"));
                        break;
                }
                cond = -1;
            g_error_free(err);
            break;
          }
        }
        g_object_unref(src);
        g_object_unref(dest);
        if(!ret)
        {
            PRINTF("g_file_copy error: %s\n", err->message);
            switch(err->code)
            {
                case G_IO_ERROR_NO_SPACE:
                    sprintf(result, _("Failed to load data from USB storage!\nError: No space left on storage."));
                    break;
                case G_IO_ERROR_CANCELLED:
                    sprintf(result, _("Failed to load data from USB storage!\nError: Operation was cancelled."));
                    break;
                default:
                    sprintf(result, _("Failed to load data from USB storage!"));
                    break;
            }
            cond = -1;
            g_error_free(err);
            break;
        }
        ViewSystem::GetInstance()->CreateDefineItemFormUsbToHost(name);
        ite++;
    }

    ptr->UmountUsbStorage();
    ViewDialog::GetInstance()->Destroy();

    //Handle result
    if(!cond)
    {
        sprintf(result, _("Success to load data from USB storage."));
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
                ViewDialog::INFO,
                result,
                NULL);
        ViewSystem::GetInstance()->UpdateUserItem();
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

static int CancelLoadHost(gpointer data)
{
    PRINTF("Cancel copy!\n");
    g_cancellable_cancel(cancellable);
    return 0;
}

void ConfigToHost::BtnOKClicked(GtkButton *button)
{
    //Copy the selected file to host
    if(GetAllSelectPath())
    {
        g_timeout_add(1000, LoadSelectedData, NULL);

        PRINTF("Load From U disk!\n");
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
                ViewDialog::PRG_CANCEL,
                _("Please wait, loading data from USB storage..."),
                CancelLoadHost);
    }
    DestroyWindow();
}

void ConfigToHost::SetProgressBar(double fraction)
{
    //gtk_progress_bar_set_pulse_step(GTK_PROGRESS_BAR(m_progress_bar), 0.1);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(m_progress_bar), fraction);
    char text_buf[10];
    sprintf(text_buf, "%d%%", (int)(fraction*100));
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(m_progress_bar), text_buf);
    gtk_widget_show(m_progress_bar);
}


void ConfigToHost::ExportRightInfoNotice(char *result)
{
    gtk_label_set_text(GTK_LABEL(label_calc_notice1), (_(result)));
    gtk_label_set_line_wrap_mode(GTK_LABEL(label_calc_notice1), PANGO_WRAP_WORD);
    gtk_widget_hide (img_error);
    gtk_widget_hide (img_load);
    gtk_widget_show(label_calc_notice1);
    gtk_widget_show(img_right);
}

void ConfigToHost::ExportErrorInfoNotice(char *result)
{
    gtk_label_set_text(GTK_LABEL(label_calc_notice1), (_(result)));
    gtk_label_set_line_wrap_mode(GTK_LABEL(label_calc_notice1), PANGO_WRAP_WORD);
    gtk_widget_hide (img_right);
    gtk_widget_hide (img_load);
    gtk_widget_show(label_calc_notice1);
    gtk_widget_show(img_error);
}

void ConfigToHost::ExportLoadInfoNotice(char *result)
{
    gtk_label_set_text(GTK_LABEL(label_calc_notice1), (_(result)));
    gtk_label_set_line_wrap_mode(GTK_LABEL(label_calc_notice1), PANGO_WRAP_WORD);
    gtk_widget_hide (img_right);
    gtk_widget_hide (img_error);
    gtk_widget_show(label_calc_notice1);
    gtk_widget_show(img_load);

}

char RootName[256];

void ConfigToHost::DeleteUdiskFile()
{
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_chkbtnDel)))
    {
        FileMan f;
        char path4[256];
        sprintf(path4, "%s%s%s%s", UDISK_DATA_PATH,"/", RootName, "/");
        PRINTF("PATH4=%s\n", path4);
        f.DelDirectory(path4);
    }
}

void ConfigToHost::BtnCalcImportOKClicked(GtkButton *button)
{
    //Copy the selected file to host
    if(GetAllCalcSelectPath())
    {
        HideOKAndCancelClicked();
        ExportLoadInfoNotice(_("Loading......"));
        //g_timeout_add(1000, LoadSelectedCalcData, NULL);
        LoadSelectedCalcData();
    }
}


void ConfigToHost::BtnCancelClicked(GtkButton *button)
{
    DestroyWindow();
}

//type: 0=root, 1=branch
GtkWidget* ConfigToHost::create_treeview(gint type)
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

GtkWidget* ConfigToHost::create_calc_treeview()
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

    render = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes(_("Name"), render, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col);

    return treeview;
}

GtkTreeModel* ConfigToHost::create_calc_root_model()
{
    GtkListStore *store;

    store = gtk_list_store_new(1, G_TYPE_STRING);

    return GTK_TREE_MODEL (store);
}

GtkTreeModel* ConfigToHost::create_root_model()
{
    GtkListStore *store;

    store = gtk_list_store_new(NUM_COLS, G_TYPE_BOOLEAN, G_TYPE_STRING);

    return GTK_TREE_MODEL (store);
}

void ConfigToHost::ToggleData(GtkCellRendererToggle *cell, gchar *path_str)
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

void ConfigToHost::UpdateCalcRootModel(void)
{
    DIR *dir;
    struct dirent *ent;

    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeRoot)));
    GtkTreeIter iter;

    dir = opendir(UDISK_DATA_PATH);
    if(!dir)
    {
        perror("open udisk path error");
        return;
    }

    while((ent = readdir(dir)) != NULL)
    {
        if(ent->d_name[0]=='.')
            continue;
        //	if(ent->d_type==DT_DIR)
        gchar *tmpPath = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, ent->d_name, NULL);
        PRINTF("DIR: %s\n", tmpPath);
        if(g_file_test(tmpPath, G_FILE_TEST_IS_DIR))
        {
            PRINTF("DIR: %s\n", ent->d_name);
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                    0, ent->d_name,
                    -1);
           // GtkTreeModel *model = GTK_TREE_MODEL(LoadBranchModel(ent->d_name));
         //   m_listBranch = g_list_append(m_listBranch, model);
        }
        g_free(tmpPath);
    }
    closedir(dir);
}


void ConfigToHost::UpdateRootModel(void)
{
    DIR *dir;
    struct dirent *ent;

    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeRoot)));
    GtkTreeIter iter;

    dir = opendir(UDISK_DATA_PATH);
    if(!dir)
    {
        perror("open udisk path error");
        return;
    }

    while((ent = readdir(dir)) != NULL)
    {
        if(ent->d_name[0]=='.')
            continue;
        //	if(ent->d_type==DT_DIR)
        gchar *tmpPath = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, ent->d_name, NULL);
        PRINTF("DIR: %s\n", tmpPath);
        if(g_file_test(tmpPath, G_FILE_TEST_IS_DIR))
        {
            PRINTF("DIR: %s\n", ent->d_name);
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
void ConfigToHost::UpdateBranchModel(gint rows)
{
    GList *list = g_list_first(m_listBranch);
    gtk_tree_view_set_model (GTK_TREE_VIEW(m_treeBranch), GTK_TREE_MODEL(g_list_nth_data(list, rows)));
}

GtkTreeModel* ConfigToHost::LoadBranchModel(gchar *branch)
{
    DIR *dir;
    struct dirent *ent;
    gchar *path = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, branch, NULL);

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
            if(fm.CompareSuffix(ent->d_name, "ini")==0)
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
void ConfigToHost::RootCalcSelectionChanged(GtkTreeSelection *selection)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *text;

    if(gtk_tree_selection_get_selected(selection, &model, &iter))
    {
       /* gboolean valid;
        GtkTreeIter iter1;

        valid = gtk_tree_model_get_iter_first(model, &iter1);
        while(valid)
        {
            gtk_list_store_set(GTK_LIST_STORE(model), &iter1, 0, FALSE, -1);

            valid = gtk_tree_model_iter_next (model, &iter1);
        }
*/
        gtk_tree_model_get(model, &iter, 0, &text, -1);
        //GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
        PRINTF("Selection path: %s\n", text);
        strcpy(RootName, text);
        /*UpdateBranchModel(atoi(gtk_tree_path_to_string(path)));
        gtk_tree_selection_get_selected(selection, &model, &iter);
        */
       // gtk_tree_path_free (path);
        g_free(text);

    }
}


void ConfigToHost::RootSelectionChanged(GtkTreeSelection *selection)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *text;
    gchar *text1;

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

void ConfigToHost::SetAllToggleValue(GtkTreeModel *model, gboolean value)
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

gboolean ConfigToHost::CheckBranchStauts(void)
{
    return FALSE;
}

gboolean ConfigToHost::GetAllCalcSelectPath(void)
{
    m_vecPath.clear();

    PRINTF("Root Name=%s\n", RootName);
    char path[256];
    char path1[256];
#ifdef VET
 sprintf(path, "%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", RootName, "/VetCalcSetting.ini");
    sprintf(path1, "%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", RootName, "/VetCalcItemSetting.ini");

#else
    sprintf(path, "%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", RootName, "/CalcSetting.ini");
    sprintf(path1, "%s%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", RootName, "/CalcItemSetting.ini");
#endif
    m_vecPath.push_back(path);
    m_vecPath.push_back(path1);

    if(!m_vecPath.empty())
        return TRUE;
    else
        return FALSE;
}


gboolean ConfigToHost::GetAllSelectPath(void)
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
                    gchar *path = g_build_path(G_DIR_SEPARATOR_S, UDISK_DATA_PATH, nameRoot, nameBranch, NULL);
                    PRINTF("Push to vector: %s\n", path);
                    m_vecPath.push_back(path);
                    gchar pathIni[255];
                    fm.GetIniFilePath(path, pathIni);
                    PRINTF("Ini path is: %s\n", pathIni);
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
void ConfigToHost::BtnSelectAllClicked(GtkButton *button)
{
}

void ConfigToHost::BtnDeselectClicked(GtkButton *button)
{
}
#endif