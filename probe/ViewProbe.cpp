#include <gtk/gtk.h>
#include <string.h>
#include "probe/ViewProbe.h"
#include "display/gui_global.h"
#include "display/gui_func.h"
#include <stdlib.h>
#include "imageProc/ModeStatus.h"
#include "imageProc/FreezeMode.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/KeyDef.h"
#include "display/HintArea.h"
#include "ViewMain.h"
#include "imageProc/ScanMode.h"
#include "imageProc/GlobalClassMan.h"
#include "keyboard/LightDef.h"
#include "sysMan/ViewSystem.h"
#include "sysMan/UserSelect.h"
#include "display/TopArea.h"
#include "imageProc/ImgProcPw.h"
#include "imageProc/MenuPW.h"
#include "probe/BiopsyMan.h"

using std::vector;

extern MenuPW g_menuPW;
#ifdef VET
#ifdef CT_355
const char* examType[] =
{
    N_("Dog"),
    N_("Cat"),
    N_("Bovine"),
    N_("Equine"),
    N_("Swine"),
    N_("Sheep"),
    NULL
};
#else
const char* examType[] =
{
    N_("User1"),
    N_("User2"),
    N_("User3"),
    N_("User4"),
    N_("User5"),
    N_("User6"),
    N_("User7"),
    N_("User8"),
    N_("User9"),
    N_("User10"),
    NULL
};

#endif
#else
const char* examType[] =
{
    N_("Adult Abdomen"),
    N_("Adult Liver"),
    N_("Kid Abdomen"),
    N_("Adult Cardio"),
    N_("Kid Cardio"),
    N_("Mammary Glands"),
    N_("Thyroid"),
    N_("Eye Ball"),
    //N_("Small Part"),
    N_("Testicle"),
    N_("Gynecology"),
    N_("Early Pregnancy"),
    N_("Middle-late Pregnancy"),
    N_("Fetal Cardio"),
    N_("Kidney Ureter"),
    N_("Bladder Prostate"),
    N_("Carotid"),
    N_("Jugular"),
    N_("Periphery Artery"),
    N_("Periphery Vein"),
    N_("Hip Joint"),
    N_("Meniscus"),
    N_("Joint Cavity"),
    N_("Spine"),
    N_("MSK"),
    N_("TCD"),
    N_("User defined 1"),
    N_("User defined 2"),
    N_("User defined 3"),
    N_("User defined 4"),
    NULL
};

#endif

ViewProbe* ViewProbe::m_ptrInstance = NULL;

ViewProbe::~ViewProbe()
{
    if (m_ptrInstance != NULL)
    {
        delete m_ptrInstance;
        m_ptrInstance = NULL;
    }
}

ViewProbe::ViewProbe()
{
    //HintArea::GetInstance()->UpdateHint(_("[Probe Select]: Reading Probe Para..."));
    m_userItemName = NULL;
    m_treeViewIndex = -1;
    m_examItemIndex = -1;
    m_preSocketIndex = -1;
    m_tree_view_path = NULL;
    memset(m_treeViewPath, '\0', sizeof(m_treeViewPath));
    m_vecUserItemName.clear();
}

ViewProbe* ViewProbe:: GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ViewProbe;

    return m_ptrInstance;
}

gboolean HandleReadProbe(gpointer data)
{
    ViewProbe *tmp;
    tmp = (ViewProbe *)data;
    tmp->ReadProbe();
    return FALSE;
}

void ViewProbe::ReadProbe(void)
{
    if (!m_pKps.ProbeRead()) // no probe is found
    {
        HintArea::GetInstance()->UpdateHint(_("[Probe Select]: No Probe was found."), 2);
#ifdef CT_430
        if(ModeStatus::IsAllSpectrumImgMode())
        {
            ImgPw::GetInstance()->ChangeDopplerSoundStatus(true);
        }
#endif
        return ;
    }
    HintArea::GetInstance()->ClearHint();

    // display probe dialog and wait user's operation
    ProbeSocket::ProbePara* para = NULL;
    int maxSocket;
    m_pKps.GetPara(para, m_itemList, maxSocket);
    //  ASSERT(para != NULL);
    if(para == NULL)
        return;
    CreateWindow(para, m_itemList, maxSocket);
}

void ViewProbe::ReadOneProbe(int socket)
{
    if (!m_pKps.OneProbeRead(socket)) // no probe is found
    {
        HintArea::GetInstance()->UpdateHint(_("[Probe Select]: No Probe was found."), 2);
        return ;
    }
    HintArea::GetInstance()->ClearHint();

    // display probe dialog and wait user's operation
    ProbeSocket::ProbePara* para = NULL;
    int maxSocket;
    m_pKps.GetPara(para, m_itemList, maxSocket);
    //  ASSERT(para != NULL);
    if(para == NULL)
        return;
    CreateWindow(para, m_itemList, maxSocket);
}

bool ViewProbe::Create(void)
{
    // get probe and item info
    HintArea::GetInstance()->UpdateHint(_("[Probe Select]: Reading Probe Para..."));
    if(ImgProcPw::GetInstance()->GetTraceStatus() && ModeStatus::IsFreezeMode())
    {
        if(g_menuPW.GetAutoTraceStatus())
            ImgProcPw::GetInstance()->SetAutoCalc(FALSE);
    }
    gdk_threads_enter();
    while(gtk_events_pending())
    {
        gtk_main_iteration();
        ///> 解决pw冻结，屏幕上有pw trace计算时，不能读探头。
#ifdef CT_430
        break;
#endif
    }
    gdk_threads_leave();

    //  g_timeout_add(100, HandleReadProbe, this);
    ReadProbe();

    return TRUE;
}
//test for A60
bool ViewProbe::Create(int socket)
{
    // get probe and item info
    HintArea::GetInstance()->UpdateHint(_("[Probe Select]: Reading Probe Para..."));

    gdk_threads_enter();
    while(gtk_events_pending())
    {
        gtk_main_iteration();
    }
    gdk_threads_leave();

    ReadOneProbe(socket);
    return TRUE;
}

void ViewProbe::CreateWindow(ProbeSocket::ProbePara* para, vector<ExamItem::EItem>* itemList, int maxSocket)
{
    GtkWidget *image_button_cancel;
    GtkWidget *label_button_cancel;
    GtkWidget *button_cancel;

    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
#ifdef CT_355
    gtk_widget_set_size_request (m_window, 550, 610);
#else
    gtk_widget_set_size_request (m_window, 730, 610);
#endif
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for (GTK_WINDOW (m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
    gtk_window_set_title (GTK_WINDOW (m_window), _("Probe Select"));
    //    gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this);

    GtkWidget *fixed_window = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (m_window), fixed_window);

    //user select
    GtkWidget *label_user_select;
    label_user_select = gtk_label_new (_("<b>Current User: </b>"));
    gtk_label_set_use_markup (GTK_LABEL (label_user_select), TRUE);
    gtk_widget_show (label_user_select);
#ifdef CT_355
    gtk_fixed_put (GTK_FIXED (fixed_window), label_user_select, 20, 530+15);
#else
    gtk_fixed_put (GTK_FIXED (fixed_window), label_user_select, 200, 530+15);
#endif
    gtk_widget_set_size_request (label_user_select, 150, 30);

    m_combobox_user_select = gtk_combo_box_new_text();
    gtk_widget_show (m_combobox_user_select);
#ifdef CT_355
    gtk_fixed_put (GTK_FIXED (fixed_window), m_combobox_user_select, 180, 530+15);
#else
    gtk_fixed_put (GTK_FIXED (fixed_window), m_combobox_user_select, 360, 530+15);
#endif
    gtk_widget_set_size_request (m_combobox_user_select, 100+60, 30);
    // g_signal_connect(GTK_COMBO_BOX(m_combobox_user_select), "changed", G_CALLBACK(HandleProbeUserChanged), this);

    UserSelect::GetInstance()->read_default_username(m_combobox_user_select);
    //UserSelect::GetInstance()->read_username(m_combobox_user_select);
    UserSelect::GetInstance()->read_username_db(USERNAME_DB, m_combobox_user_select);
    int num = UserSelect::GetInstance()->get_active_user();
    UserSelect::GetInstance()->set_active_user(m_combobox_user_select, num);

    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;
    int index_username = exam.ReadDefaultUserIndex(&ini);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_user_select), index_username);
    g_signal_connect(GTK_COMBO_BOX(m_combobox_user_select), "changed", G_CALLBACK(HandleProbeUserChanged), this);

    int x = 20, y = 20;
    //vector<string>exam_type;
    char exam_type[MAXNUMBER+20][50];

    for (int i = 0; i < maxSocket; i++, x+=180) {
        GtkWidget *vbox_probe;
        if (para[i].exist) {
            ProbeSocket::ProbePara para_cur;
            ProbeMan::GetInstance()->GetCurProbe(para_cur);
            if(strcmp(para_cur.model, para[i].model) == 0)
            {
                m_preSocketIndex = i;
                PRINTF("model:%s\n", para_cur.model);
            }
            else
                m_preSocketIndex = -1;
            CreateAllExamType(para[i].model, itemList[i], exam_type);
            //CreateExamType(itemList[i], exam_type);
            vbox_probe = CreateProbe(para[i].model, para[i].type, exam_type, i);
        }
        else {
            m_examItemIndex = -1;
            vbox_probe = CreateProbe(NULL, 0, NULL, i);
        }
        gtk_fixed_put (GTK_FIXED (fixed_window), vbox_probe, x, y);
    }

    // image_button_ok = gtk_image_new_from_stock("gtk-ok", GTK_ICON_SIZE_BUTTON);
    // label_button_ok = gtk_label_new_with_mnemonic(_("OK"));
    // button_ok = create_button_icon (label_button_ok, image_button_ok);
    // gtk_fixed_put (GTK_FIXED (fixed_window), button_ok, 460, 530);

    // image_button_preset = gtk_image_new_from_stock("gtk-preferences", GTK_ICON_SIZE_BUTTON);
    // label_button_preset = gtk_label_new_with_mnemonic(_("Exam Preset"));
    // button_preset = create_button_icon (label_button_preset, image_button_preset);
    // gtk_fixed_put (GTK_FIXED (fixed_window), button_preset, 460, 530);

    image_button_cancel = gtk_image_new_from_stock("gtk-quit", GTK_ICON_SIZE_BUTTON);
    label_button_cancel = gtk_label_new_with_mnemonic(_("Exit"));
    button_cancel = create_button_icon (label_button_cancel, image_button_cancel);
#ifdef CT_355
    gtk_fixed_put (GTK_FIXED (fixed_window), button_cancel, 380, 530);
#else
    gtk_fixed_put (GTK_FIXED (fixed_window), button_cancel, 590, 530);
#endif

    g_signal_connect ((gpointer) button_cancel, "clicked",
            G_CALLBACK (on_button_cancel_clicked),
            this);

    gtk_widget_show_all (m_window);
      g_keyInterface.Push(this);
    SetSystemCursorToCenter();

    return ;
}

gboolean ViewProbe::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    Destroy();
    return FALSE;
}

int ViewProbe::CreateAllExamType(const char *model, vector<ExamItem::EItem> item, char exam_type[][50])
{
    unsigned int i(0), j(0);
    if(item.size()>0)
    {
        for (i = 0; i < item.size(); i ++)
        {
            strcpy((char*)exam_type[i], _(examType[item[i]]));
        }
    }

    GetUserItemNameOfProbe(model);
    for(j = 0; j < m_vecUserItemName.size(); j++)
    {
        strcpy((char*)exam_type[i+j], m_vecUserItemName[j].c_str());
    }

    strcpy((char*)exam_type[i+j], " ");
    return (i+j);
}

void ViewProbe::GetUserItemNameOfProbe(const char* model)
{
    m_vecUserItemName.clear();

    char probelist[256];
    char useritem[256];
    char department[256];
    char firstGenItem[256];
    char src_group[256];
    char userselect[256];
    gchar* name = gtk_combo_box_get_active_text (GTK_COMBO_BOX(m_combobox_user_select));
    char username[256];
    ExamItem examitem;
    examitem.TransUserSelectForEng(name, username);
    PRINTF("username = %s\n", username);
    vector<string> useritemgroup;
    useritemgroup = examitem.GetCurrentProbeUserGroup(username);
    int group_length(0);
    group_length = useritemgroup.size();
    for (int i= 0 ; i <  group_length; i++)
    {
        sprintf(src_group ,"%s", useritemgroup[i].c_str());
        examitem.GetDefaultUserDefinedItem(src_group, userselect, username, probelist, useritem, department, firstGenItem);
        ExamPara exampara;
        exampara.name = useritem;
        exampara.index = ExamItem::USERNAME;
        PRINTF("username = %s, userslect = %s, model =%s, probelist = %s\n ", username, userselect, model, probelist);
        if(strcmp(username, userselect) == 0)
        {
            if (strcmp(model, probelist) == 0)
            {
                m_vecUserItemName.push_back(exampara.name);
            }
        }
    }
}

int ViewProbe::CreateExamType(vector<ExamItem::EItem> item, char exam_type[][50])
{
    unsigned int i = 0;
    for (i = 0; i < item.size(); i ++)
    {
        strcpy((char*)exam_type[i], _(examType[item[i]]));
    }

    strcpy((char*)exam_type[i], " ");
    return i;
}

GtkWidget* ViewProbe::CreateProbe(const char *probe_name, const char probeType,
        char exam_type[][50], int probe_index)
{
    GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_set_size_request (vbox, 150, 500);
    char m_probe_name[128];
    GtkWidget *label_name;

    if (probe_name)
    {
        char path[100] = {"0"};
        sprintf(path, "%s/%s", CFG_RES_PATH, PROBE_ALIAS_FILE);
        IniFile ini(path);
        char alias[56] = {"0"};
        ProbeMan::GetInstance()->ReadProbeAlias(&ini, probe_name, alias);
        //ProbeMan::GetInstance()->VerifyProbeName(probe_name, m_probe_name);
        //label_name = gtk_label_new (m_probe_name);
        label_name = gtk_label_new (alias);
    }
    else
        label_name = gtk_label_new (_("No Probe"));

    gtk_box_pack_start (GTK_BOX (vbox), label_name, FALSE, TRUE, 0);
    gtk_widget_set_size_request (label_name, -1, 30);

    GtkWidget * frame_image = gtk_frame_new (NULL);
    gtk_box_pack_start (GTK_BOX (vbox), frame_image, FALSE, TRUE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_image), GTK_SHADOW_OUT);

    char image_path[256];
    switch ((int)probeType) {
        case 'C':
        case 'c':
            sprintf(image_path, "%s/%s", CFG_RES_PATH, "res/probe/35C50K.png");
            break;
        case 'L':
        case 'l':
            {
#ifdef CT_355
                if(!strcmp(probe_name, "55L60G"))
#else
                if(!strcmp(probe_name, "55L60H"))
#endif
                sprintf(image_path, "%s/%s", CFG_RES_PATH, "res/probe/55L60H.png");
            else
                sprintf(image_path, "%s/%s", CFG_RES_PATH, "res/probe/75L40K.png");
            }
            break;
        case 'T':
        case 't':
            {
#ifdef CT_440
                    sprintf(image_path, "%s/%s", CFG_RES_PATH, "res/probe/65C10K.png");
#else
                if((strcmp(probe_name, "65C10H") == 0)||(strcmp(probe_name, "65C10E") == 0))
                    sprintf(image_path, "%s/%s", CFG_RES_PATH, "res/probe/65C10H.png");
                else
                    sprintf(image_path, "%s/%s", CFG_RES_PATH, "res/probe/65C10K.png");
#endif
            }
            break;
        case 'N':
        case 'n':
            sprintf(image_path, "%s/%s", CFG_RES_PATH, "res/probe/35C20H.png");
            break;
        case 0:
            sprintf(image_path, "%s/%s", CFG_RES_PATH, "res/probe/NoProbe.png");
            break;
        case 'P':
        case 'p':
            sprintf(image_path, "%s%s", CFG_RES_PATH, "res/probe/30P16A.png");
            break;
        case 'V':
        case 'v':
            sprintf(image_path, "%s/%s", CFG_RES_PATH, "res/probe/35D40J.png");
            break;

        default:
            sprintf(image_path, "%s/%s", CFG_RES_PATH, "res/probe/35C50K.png");
            break;
    }

    GtkWidget *image_probe = create_pixmap(image_path);
    gtk_container_add (GTK_CONTAINER (frame_image), image_probe);
    gtk_widget_set_size_request (image_probe, -1, 90);

    GtkWidget *frame_exam = gtk_frame_new (NULL);
    gtk_box_pack_start (GTK_BOX (vbox), frame_exam, TRUE, TRUE, 0);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_exam), GTK_SHADOW_OUT);

    GtkWidget *scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (frame_exam), scrolledwindow);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    if(probe_index == 0)
    {
        GtkWidget *treeview0 = CreateTreeView (exam_type, probe_index);
        gtk_container_add (GTK_CONTAINER (scrolledwindow), treeview0);
    }
  if(probe_index == 1)
    {
        GtkWidget *treeview1 = CreateTreeView (exam_type, probe_index);
        gtk_container_add (GTK_CONTAINER (scrolledwindow), treeview1);
    }
  if(probe_index == 2)
    {
        GtkWidget *treeview2 = CreateTreeView (exam_type, probe_index);
        gtk_container_add (GTK_CONTAINER (scrolledwindow), treeview2);
    }
  if(probe_index == 3)
    {
        GtkWidget *treeview3 = CreateTreeView (exam_type, probe_index);
        gtk_container_add (GTK_CONTAINER (scrolledwindow), treeview3);
    }
    return vbox;
}

GtkTreeModel* ViewProbe::CreateModel(char exam_type[][50], int probe_index)
{
    GtkTreeIter iter;
    GtkListStore *store = gtk_list_store_new(N_COLUMNS, G_TYPE_INT, G_TYPE_STRING);
    int i = 0;
    if (exam_type) {
        while (strcmp((char*)exam_type[i], " ")) {
            PRINTF("--name[%d]=%s\n", i, exam_type[i]);
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                    PROBE_INDEX, probe_index,
                    EXAM_COLUMN, *exam_type,
                    -1);
            exam_type++;
        }
    }
    return GTK_TREE_MODEL(store);
}

void ViewProbe::AddColumn(GtkTreeView *treeview)
{
    GtkCellRenderer *renderer_text;
    GtkTreeViewColumn *column_text;

    renderer_text = gtk_cell_renderer_text_new(); // GtkCellRendererText
    column_text = gtk_tree_view_column_new_with_attributes("EXAM_NAME", // column name
            renderer_text,    // the cell renderer type
            "text", EXAM_COLUMN, // renderer attribute
            NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column_text);
}

GtkWidget* ViewProbe::CreateTreeView(char exam_type[][50], int probe_index)
{
   //GtkWidget *treeview;
    GtkTreeModel *model;
    model = CreateModel(exam_type, probe_index);
    GetCurExamItemIndex(exam_type, probe_index);
    PRINTF("index:%d\n", probe_index);
    if(probe_index== 0)
    {
        treeview0 = gtk_tree_view_new_with_model(model);
        AddColumn(GTK_TREE_VIEW(treeview0));

        gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeview0), FALSE);
        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview0), FALSE);

        GtkTreeSelection *select;
        select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview0));
        gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
        GTK_WIDGET_UNSET_FLAGS (treeview0, GTK_CAN_FOCUS);
        //char *path_string = gtk_tree_path_to_string(path);

        if((m_preSocketIndex == 0) && (m_examItemIndex >=0))
        {
            char tmp[10];
            sprintf(tmp, "%d", m_examItemIndex);
            sprintf(m_treeViewPath, "%d", m_examItemIndex);
            GtkTreePath *path = gtk_tree_path_new_from_string(tmp);
            if(path != NULL)
            {
                gtk_tree_view_set_cursor(GTK_TREE_VIEW(treeview0), path, NULL, TRUE);
                gtk_tree_path_free(path);
                m_treeViewIndex = 0;
            }
        }
        g_signal_connect(treeview0, "focus-out-event", G_CALLBACK(HandleTreeViewFocusOut), this);
        //g_signal_connect(select, "changed", G_CALLBACK(HandleTreeSelectionChanged), this);
        g_signal_connect(treeview0, "button-press-event", G_CALLBACK(HandleTreeViewButtonClick), this);

        g_object_unref(model);
        return treeview0;
    }
    if(probe_index== 1)
    {
        treeview1 = gtk_tree_view_new_with_model(model);
        AddColumn(GTK_TREE_VIEW(treeview1));

        gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeview1), FALSE);
        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview1), FALSE);

        GtkTreeSelection *select;
        select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1));
        gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
        GTK_WIDGET_UNSET_FLAGS (treeview1, GTK_CAN_FOCUS);

        if((m_preSocketIndex == 1) && (m_examItemIndex >=0))
        {
            char tmp[10];
            sprintf(tmp, "%d", m_examItemIndex);
            sprintf(m_treeViewPath, "%d", m_examItemIndex);
            GtkTreePath *path = gtk_tree_path_new_from_string(tmp);

            if(path != NULL)
            {
                gtk_tree_view_set_cursor(GTK_TREE_VIEW(treeview1), path, NULL, TRUE);
                gtk_tree_path_free(path);
                m_treeViewIndex = 1;
            }
        }

        g_signal_connect(treeview1, "focus-out-event", G_CALLBACK(HandleTreeViewFocusOut), this);
        //g_signal_connect(select, "changed", G_CALLBACK(HandleTreeSelectionChanged), this);
        g_signal_connect(treeview1, "button-press-event", G_CALLBACK(HandleTreeViewButtonClick), this);

        g_object_unref(model);
        return treeview1;
    }
    if(probe_index== 2)
    {
        treeview2 = gtk_tree_view_new_with_model(model);
        AddColumn(GTK_TREE_VIEW(treeview2));

        gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeview2), FALSE);
        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview2), FALSE);

        GtkTreeSelection *select;
        select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview2));
        gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
        GTK_WIDGET_UNSET_FLAGS (treeview2, GTK_CAN_FOCUS);
        if((m_preSocketIndex == 2) && (m_examItemIndex >=0))
        {
            char tmp[10];
            sprintf(tmp, "%d", m_examItemIndex);
            sprintf(m_treeViewPath, "%d", m_examItemIndex);
            GtkTreePath *path = gtk_tree_path_new_from_string(tmp);

            if(path != NULL)
            {
                gtk_tree_view_set_cursor(GTK_TREE_VIEW(treeview2), path, NULL, TRUE);
                gtk_tree_path_free(path);
                m_treeViewIndex = 2;
            }
        }

        g_signal_connect(treeview2, "focus-out-event", G_CALLBACK(HandleTreeViewFocusOut), this);
        //g_signal_connect(select, "changed", G_CALLBACK(HandleTreeSelectionChanged), this);
        g_signal_connect(treeview2, "button-press-event", G_CALLBACK(HandleTreeViewButtonClick), this);

        g_object_unref(model);
        return treeview2;
    }
    if(probe_index== 3)
    {
        treeview3 = gtk_tree_view_new_with_model(model);
        AddColumn(GTK_TREE_VIEW(treeview3));

        gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeview3), FALSE);
        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview3), FALSE);

        GtkTreeSelection *select;
        select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview3));
        gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
        GTK_WIDGET_UNSET_FLAGS (treeview3, GTK_CAN_FOCUS);
        if((m_preSocketIndex == 3) && (m_examItemIndex >=0))
        {
            char tmp[10];
            sprintf(tmp, "%d", m_examItemIndex);
            sprintf(m_treeViewPath, "%d", m_examItemIndex);
            GtkTreePath *path = gtk_tree_path_new_from_string(tmp);

            if(path != NULL)
            {
                gtk_tree_view_set_cursor(GTK_TREE_VIEW(treeview3), path, NULL, TRUE);
                gtk_tree_path_free(path);
                m_treeViewIndex = 3;
            }
        }

        g_signal_connect(treeview3, "focus-out-event", G_CALLBACK(HandleTreeViewFocusOut), this);
        //g_signal_connect(select, "changed", G_CALLBACK(HandleTreeSelectionChanged), this);
        g_signal_connect(treeview3, "button-press-event", G_CALLBACK(HandleTreeViewButtonClick), this);

        g_object_unref(model);
        return treeview3;
    }
}

void ViewProbe::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);

    switch(keyValue)
    {
        case KEY_ESC:
            //  case KEY_PROBE: //避免快速频繁按下Probe时，窗口堆栈出错
            BtnCancelClicked(NULL);
            break;

        default:
            break;
    }
}

void ViewProbe::Destroy(void)
{
    gtk_combo_box_popdown(GTK_COMBO_BOX(m_combobox_user_select));
#ifndef CT_430
    m_pKps.ActiveHV(TRUE);
#endif
#ifdef CT_430
    if(ModeStatus::IsAllSpectrumImgMode())
    {
        ImgPw::GetInstance()->ChangeDopplerSoundStatus(true);
    }
#endif
    if(GTK_IS_WIDGET(m_window))
    {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
        m_window = NULL;
    }

    if(ImgProcPw::GetInstance()->GetTraceStatus() && ModeStatus::IsFreezeMode())
    {
        if(g_menuPW.GetAutoTraceStatus())
            ImgProcPw::GetInstance()->SetAutoCalc(TRUE);
    }
}

    namespace {
        gboolean DestroyWindow(gpointer data)
        {
            ViewProbe *tmp;
            tmp = (ViewProbe *)data;
            tmp->Destroy();
            return FALSE;
        }
    }

void ViewProbe::BtnCancelClicked(GtkButton *button)
{
#if (defined(CT_322) || defined(CT_313) || (CT_430) ||(CT_355))
    m_probe_index = ProbeMan::GetInstance()->GetCurProbeSocket();
    ProbeMan::GetInstance()->SetProbeSocket(m_probe_index);
#endif
    Destroy();
}

// void ViewProbe::BtnOKClicked(GtkButton *button)
// {
// g_keyInterface.Pop();
// gtk_widget_destroy(m_window);
// PRINTF("PROBE index = %d\n", m_probeIndex);
// PRINTF("item index = %d\n", m_itemIndex);
// m_pKps.ProbeInit(m_probeIndex, (ExamItem::EItem)m_itemIndex);
// if (g_keyInterface.Size() == 1)
//     SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
// }

void ViewProbe::TreeSelectionChanged(GtkTreeSelection *selection)
{
    //triged this single
    //GtkTreeView *treeview;
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreePath *path;
    //only probe select can make this change
    gchar* name = gtk_combo_box_get_active_text (GTK_COMBO_BOX(m_combobox_user_select));
    if (name != NULL)
    {
        int num = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_user_select));

        UserSelect::GetInstance()->save_cur_username(name);
        char username[256];
        char str_path[256] = {'\0'};
        sprintf(str_path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini(str_path);
        ExamItem exam;
        exam.TransUserSelectForEng(name, username);
        exam.WriteDefaultUserSelect(&ini, username);
        exam.WriteDefaultUserIndex(&ini, num);
        memset(user_configure, 0, USERCONFIG_LEN);
        if (num > 0)
        {
            sprintf(user_configure, "%s%s%s", "userconfig/", name, ".ini");
        }
        else
        {
#ifdef VET
            sprintf(user_configure, "%s", "VetItemPara.ini");

#else
            sprintf(user_configure, "%s", "ItemPara.ini");
#endif
        }
    }

    // treeview = gtk_tree_selection_get_tree_view(selection);
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        char str_path[256] = {'\0'};
        sprintf(str_path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini(str_path);
        ProbeSocket::ProbePara para;
        ExamItem exam;
        //ProbeMan::GetInstance()->GetCurProbe(para);

        gtk_tree_model_get(model, &iter, PROBE_INDEX, &m_probe_index, -1);
        PRINTF("### m_probe_index = %d ###\n", m_probe_index);
        m_probeIndex = m_probe_index;

        path = gtk_tree_model_get_path(model, &iter);
        char *path_string = gtk_tree_path_to_string(path);
        PRINTF("path:%s\n", path_string);
        int i = 0;
        i = atoi(path_string);
        int itemNumOfProbe = m_itemList[m_probeIndex].size();
        if(i >= itemNumOfProbe)
        {
            gtk_tree_model_get(model, &iter, EXAM_COLUMN, &m_userItemName, -1);
            m_itemNameUserDef = m_userItemName;
            PRINTF("m_userItemName=%s\n", m_userItemName);
            m_itemIndex = 0;
            m_pKps.UserItemOfProbeInit(m_probeIndex, (ExamItem::EItem)m_itemIndex, m_userItemName);
            exam.WriteUserItemFlag(&ini, true);
            exam.WriteDefaultProbeItemName(&ini, m_userItemName);
        }
        else
        {
            m_itemIndex = m_itemList[m_probeIndex][i];
            m_pKps.ProbeInit(m_probeIndex, (ExamItem::EItem)m_itemIndex);
            exam.WriteUserItemFlag(&ini, false);
            exam.WriteDefaultProbeItemName(&ini, examType[m_itemIndex]);
        }
        gtk_tree_path_free (path);
        //m_pKps.ProbeInit(m_probeIndex, (ExamItem::EItem)m_itemIndex);
        int counts = 0;
        while(gtk_events_pending())
        {
            gtk_main_iteration();
            counts++;
            if(counts > 15)
                break;
        }
        g_timeout_add(500, DestroyWindow, this);

        ProbeMan::GetInstance()->GetCurProbe(para);

        //write default probe model
        ProbeMan::GetInstance()->WriteDefaultProbe((char*)para.model, &ini);

        //write selected item to file
        exam.WriteDefaultProbeItem(&ini, m_itemIndex);
    }
}

void ViewProbe::ProbeUserChanged(GtkWidget *widget)
{
    // display probe dialog and wait user's operation
    ProbeSocket::ProbePara* para = NULL;
    int maxSocket;

      m_pKps.GetPara(para, m_itemList, maxSocket);
    //  ASSERT(para != NULL);
     if(para == NULL)
          return;
     else
     {
         char exam_type[MAXNUMBER+100][50];
         GtkTreeModel *model;
         for (int i = 0; i < maxSocket; i++) {
             if (para[i].exist) {
                 CreateAllExamType(para[i].model, m_itemList[i], exam_type);
                 GetCurExamItemIndex(exam_type, i);
                 model = CreateModel(exam_type, i);
                 if(i==0)
                 {
                     gtk_tree_view_set_model(GTK_TREE_VIEW(treeview0), model);
                     gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview0));
                 }
                 if(i==1)
                 {
                     gtk_tree_view_set_model(GTK_TREE_VIEW(treeview1), model);
                     gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview1));
                 }
                 if(i==2)
                 {
                     gtk_tree_view_set_model(GTK_TREE_VIEW(treeview2), model);
                     gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview2));
                 }
                 if(i==3)
                 {
                     gtk_tree_view_set_model(GTK_TREE_VIEW(treeview3), model);
                     gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview3));
                 }

             }
             else {
                 model = CreateModel(NULL, i);
             }
         }
     }
}

void ViewProbe::TreeViewFocusOut(GtkWidget *widget, GdkEventFocus *event)
{
    GtkTreeSelection* select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
    gtk_tree_selection_unselect_all(select);
}

void ViewProbe::TreeViewBtnClicked(GtkWidget *widget, GdkEventButton *event)
{
    if (event->window == gtk_tree_view_get_bin_window (GTK_TREE_VIEW(widget)) && event->type == GDK_BUTTON_PRESS && event->button == 1)
    {
        int x, y;
        GtkTreeModel *model;
        GtkTreeIter iter;
        GtkTreePath *path;
        //only probe select can make this change
        gchar* name = gtk_combo_box_get_active_text (GTK_COMBO_BOX(m_combobox_user_select));
        if (name != NULL)
        {
            int num = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_user_select));
            UserSelect::GetInstance()->save_cur_username(name);
            char username[256];
            char str_path[256] = {'\0'};
            sprintf(str_path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
            IniFile ini(str_path);
            ExamItem exam;
            exam.TransUserSelectForEng(name, username);
            exam.WriteDefaultUserSelect(&ini, username);
            exam.WriteDefaultUserIndex(&ini, num);
            memset(user_configure, 0, USERCONFIG_LEN);
            if (num > 0)
            {
                sprintf(user_configure, "%s%s%s", "userconfig/", name, ".ini");
            }
            else
            {
#ifdef VET
                sprintf(user_configure, "%s", "VetItemPara.ini");
#else
                sprintf(user_configure, "%s", "ItemPara.ini");
#endif
            }
        }

        gtk_tree_view_convert_widget_to_bin_window_coords (GTK_TREE_VIEW(widget), event->x, event->y, &x, &y);
        model = gtk_tree_view_get_model (GTK_TREE_VIEW(widget));
        if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(widget), x, y, &path, NULL, NULL, NULL))
        {
            char str_path[256] = {'\0'};
            sprintf(str_path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
            IniFile ini(str_path);
            ProbeSocket::ProbePara para;
            ExamItem exam;

            if (gtk_tree_model_get_iter(model, &iter, path))
            {
                gtk_tree_model_get(model, &iter, PROBE_INDEX, &m_probe_index, -1);
                PRINTF("### m_probe_index = %d ###\n", m_probe_index);
                m_probeIndex = m_probe_index;

                char *path_string = gtk_tree_path_to_string(path);
                PRINTF("path:%s\n", path_string);
                int i = 0;
                i = atoi(path_string);
                PRINTF("--itemSize=%d i=%d--\n", m_itemList[m_probeIndex].size(), i);
                int itemNumOfProbe = m_itemList[m_probeIndex].size();
                if(i >= itemNumOfProbe)
                {
                    gtk_tree_model_get(model, &iter, EXAM_COLUMN, &m_userItemName, -1);
                    m_itemNameUserDef = m_userItemName;
                    m_itemIndex = 0;
                    m_pKps.UserItemOfProbeInit(m_probeIndex, (ExamItem::EItem)m_itemIndex, m_userItemName);
                    exam.WriteUserItemFlag(&ini, true);
                    exam.WriteDefaultProbeItemName(&ini, m_userItemName);
                }
                else
                {
                    m_itemIndex = m_itemList[m_probeIndex][i];
                    PRINTF("itemIndex:%d\n", m_itemIndex);
                    m_pKps.ProbeInit(m_probeIndex, (ExamItem::EItem)m_itemIndex);
                    exam.WriteUserItemFlag(&ini, false);
                    exam.WriteDefaultProbeItemName(&ini, examType[m_itemIndex]);
                }
                gtk_tree_path_free (path);
                int counts = 0;
                while(gtk_events_pending())
                {
                    gtk_main_iteration();
                    counts++;
                    if(counts > 15)
                        break;
                }
                g_timeout_add(500, DestroyWindow, this);

                ProbeMan::GetInstance()->GetCurProbe(para);
                ProbeMan::GetInstance()->GetCurProbe(para);

                //write default probe model
                ProbeMan::GetInstance()->WriteDefaultProbe((char*)para.model, &ini);
                //write selected item to file
                exam.WriteDefaultProbeItem(&ini, m_itemIndex);

                //update biopsy bracket type to solve the problem of unfresh biopsy bracket.
                BiopsyMan::GetInstance()->SetCurBioBracketAndAngleTypeOfProbeChanged();//2016.11.06--hy
            }
        }
    }
}

const gchar* ViewProbe::GetUserName(void)
{
    return  gtk_combo_box_get_active_text (GTK_COMBO_BOX(m_combobox_user_select));
}

void ViewProbe::GetCurExamItemIndex(char exam_type[][50], int probe_index)
{
    int i = 0;
    m_examItemIndex = 0;
    string str_check_park("");
    TopArea::GetInstance()->GetCheckPart(str_check_park);
    if(exam_type)
    {
        while (strcmp((char*)exam_type[i], " "))
        {
            if(m_preSocketIndex == probe_index)
            {
                if(strcmp(str_check_park.c_str(), (char*) exam_type[i]))
                {
                    PRINTF("check_park:%s\n", str_check_park.c_str());
                    m_examItemIndex++;
                }
                else
                {
                    PRINTF("index:%d\n", m_examItemIndex);
                    break;
                }
            }
            exam_type++;
        }
    }
}