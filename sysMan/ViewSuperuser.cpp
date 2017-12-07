#include <gtk/gtk.h>

#include "sysMan/ViewSuperuser.h"
#include "ViewMain.h"
#include "display/gui_func.h"
#include "keyboard/KeyDef.h"
#include "probe/ProbeMan.h"
#include "keyboard/MultiFuncFactory.h"
#include "imageControl/Img2D.h"
#include "imageProc/ModeStatus.h"
#include "keyboard/KeyFunc.h"
#include "sysMan/ScreenSaver.h"
#include <iostream>
#include "sysMan/SysGeneralSetting.h"
#include "display/ImageAreaPara.h"
#include "projectMode/KnobProjectMode.h"
#include "display/MenuArea.h"
#include "display/TopArea.h"
#include "periDevice/MonitorControl.h"
#ifdef CT_PROJECT
#include "projectMode/ProjectCalc2D.h"
#endif
using namespace std;

#if defined(CT_360)
std::string ViewSuperuser::m_probeName[WPORBE_NUM] =
{
    "3.5CV", "6.5VMC", "7.5LVS", "3.5MC", "30P16A", "6.5MC", "35D40J(SNR)", "35D40J(NDK)"
};
#elif defined(CT_161)
std::string ViewSuperuser::m_probeName[WPORBE_NUM] =
{
    "3.5CV", "6.5VMC", "7.5LVS", "3.5MC", "30P16A"
};
#elif (defined(CT_322) || defined(CT_313))
std::string ViewSuperuser::m_probeName[WPORBE_NUM] =
{
    "35C50J", "75L40J", "65C10J", "35C20G", "65C20G", "90L40J", "35D40J(SNR)", "35D40J(NDK)", "30P16A"
};
#elif defined(CT_430)
std::string ViewSuperuser::m_probeName[WPORBE_NUM] =
{
    "35C60E", "65L40E", "65C10E", "65C15D", "35D50D"
};
#elif defined(CT_440)
std::string ViewSuperuser::m_probeName[WPORBE_NUM] =
{
    "35C60E", "65C10E", "65L40E",  "65C15D"
};
#elif (defined(CT_355))
#ifdef VET
std::string ViewSuperuser::m_probeName[WPORBE_NUM] =
{
    "35C50L(APX)", "35C50L(EMP)", "35C20I", "70L40J", "75L40J(APX)", "75L40J(EMP)", "70L60J", "90L40J", "65C10L", "65C15E(APX)", "65C15E(JR)", "30P16B", "10L25K", "65C10I", "55L60G"
};
#else
std::string ViewSuperuser::m_probeName[WPORBE_NUM] =
{
    "35C50L(APX)", "35C50L(EMP)", "35C20I", "70L40J", "75L40J(APX)", "75L40J(EMP)", "70L60J", "90L40J", "65C10L", "65C15E(APX)", "65C15E(JR)", "30P16B", "10L25K", "65C10I"
};
#endif
#else
#ifdef VET
std::string ViewSuperuser::m_probeName[WPORBE_NUM] =
{
    "35C50K", "65C10K", "75L40K", "35C20H", "65C15D", "30P16A", "90L25K", "10L25J", "35D40J(SNR)", "35D40J(NDK)", "55L60H"
};
#else
std::string ViewSuperuser::m_probeName[WPORBE_NUM] =
{
    "35C50K", "65C10K", "75L40K", "35C20H", "65C15D", "30P16A", "90L25K", "10L25J", "35D40J(SNR)", "35D40J(NDK)", "65C10H"
};
#endif
#endif

ViewSuperuser* ViewSuperuser::m_ptrInstance = NULL;
ViewSuperuser::ViewSuperuser()
{
    m_vecAuthenInfo.clear();
    m_statusAuthen = TRUE;
    m_timer = 0;
    m_statusDemo = TRUE;
    m_probeType = 0;
    m_window = NULL;

#ifdef TRANSDUCER
   m_tranPressCorrect = 0;
#endif

#ifdef CT_PROJECT
   m_statusProjectMode = false;
#endif

 memset(m_probeModel, 0, 32);
   memset(m_probeAlias, 0, sizeof(m_probeAlias));
   if(!m_strProbeAlias.empty())
       m_strProbeAlias.clear();
}

ViewSuperuser::~ViewSuperuser()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

ViewSuperuser* ViewSuperuser::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ViewSuperuser;

    return m_ptrInstance;
}

void ViewSuperuser::DestroyWindow(void)
{
    if(!m_strProbeAlias.empty())
        m_strProbeAlias.clear();

    if(GTK_IS_WIDGET(m_window)) {
        Exit();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1)
        {
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
        }
        m_window = NULL;
    }
}

gboolean ViewSuperuser::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWindow();
    return FALSE;
}

void ViewSuperuser::Authenticate(void)
{
    PRINTF("--------------enter authenticate\n");
    if (m_timer > 0)
    {
        g_source_remove(m_timer);
        m_timer = 0;
        g_keyInterface.Pop();
    }

    g_keyInterface.Push(this);
    m_vecAuthenInfo.clear();
    m_statusAuthen = TRUE;
    m_statusDemo = TRUE;

#ifdef CT_PROJECT
    m_statusProjectMode = false;
#endif

    m_timer = g_timeout_add(1000, HandleAuthen, NULL);
}

void ViewSuperuser::CreateWindow(void)
{
    GtkWidget *fixed;
    GtkWidget *labelExit;
    GtkWidget *imageExit;
    GtkWidget *btnExit;

    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

    const int WINDOW_W = 480+140;
    const int WINDOW_H = 640;
    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, WINDOW_W, WINDOW_H);
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_title (GTK_WINDOW (m_window), "超级用户工具");
    gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(HandleWindowDeleteEvent), this);

    fixed = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    const int NOTEBOOK_W = WINDOW_W - 40;
    const int NOTEBOOK_H = 540 + 20;
    m_notebook = gtk_notebook_new();
    gtk_widget_show (m_notebook);
    gtk_fixed_put (GTK_FIXED (fixed), m_notebook, 20, 10);
    gtk_widget_set_size_request (m_notebook, NOTEBOOK_W, NOTEBOOK_H);

    GtkWidget* tableHint = gtk_table_new(1, 1, FALSE);
    gtk_table_set_row_spacing(GTK_TABLE(tableHint), 0, 10);
    gtk_table_set_col_spacings(GTK_TABLE(tableHint), 10);
    gtk_container_set_border_width(GTK_CONTAINER(tableHint), 10);
    gtk_widget_set_size_request(tableHint, 380, 30);
    gtk_fixed_put(GTK_FIXED(fixed), tableHint, 20, 600);

    m_labelHint = gtk_label_new("");
    gtk_table_attach (GTK_TABLE (tableHint), m_labelHint, 0, 1, 0, 1,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);

    //> create note probe
    int notebook_counter(0);
    GtkWidget *fixed_probe;
    fixed_probe = CreateNoteProbe();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_probe);
    GtkWidget *label_probe = gtk_label_new ("探头");
    gtk_widget_show (label_probe);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_probe);

    //> create note monitor
    GtkWidget *fixed_monitor;
    fixed_monitor = CreateNoteMonitor();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_monitor);
    GtkWidget *label_monitor = gtk_label_new ("显示器");
    gtk_widget_show (label_monitor);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_monitor);

#ifdef CT_PROJECT
    //>create note project
    GtkWidget *fixed_project;
    fixed_project = CreateNoteProject();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_project);

    GtkWidget *label_project = gtk_label_new ("工程模式");
    gtk_widget_show (label_project);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_project);
#endif

#ifdef TRANSDUCER
    labelExit = gtk_label_new_with_mnemonic("确定");
#else
    labelExit = gtk_label_new_with_mnemonic("退出");
#endif
    imageExit = gtk_image_new_from_stock(GTK_STOCK_QUIT, GTK_ICON_SIZE_BUTTON);
    btnExit = create_button_icon(labelExit, imageExit);
    gtk_fixed_put (GTK_FIXED (fixed), btnExit, 340+140, 570);
    g_signal_connect (G_OBJECT(btnExit), "clicked", G_CALLBACK(HandleBtnExitClicked), this);

    gtk_widget_show_all(m_window);
    g_keyInterface.Pop();

    //clear
    if (m_vecAuthenInfo.size())
      m_vecAuthenInfo.clear();

    m_statusAuthen = false;

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
    return;
}

void ViewSuperuser::CreateDemoWindow(void)
{
    GtkWidget *fixed;
    GtkWidget *labelDemo;
    GtkWidget *imageDemo;
    GtkWidget *btnEnterDemo;

    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, 480, 320);
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_title (GTK_WINDOW (m_window), "进入演示");
    gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(HandleWindowDeleteEvent), this);

    fixed = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    labelDemo = gtk_label_new_with_mnemonic("进入演示功能");
    imageDemo = gtk_image_new_from_stock(GTK_STOCK_YES, GTK_ICON_SIZE_BUTTON);
    btnEnterDemo = create_button_icon(labelDemo, imageDemo);
    g_signal_connect (G_OBJECT(btnEnterDemo), "clicked", G_CALLBACK(HandleBtnEnterDemoClicked), this);
    gtk_fixed_put(GTK_FIXED(fixed),btnEnterDemo, 40, 20);

    gtk_widget_show_all(m_window);

    m_statusAuthen = false;

    g_keyInterface.Pop();

    //clear
    if (m_vecAuthenInfo.size())
      m_vecAuthenInfo.clear();

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
}

GtkWidget *ViewSuperuser::create_probe_treeview(void)
{
    GtkWidget *treeview;
    GtkTreeModel *model;
    GtkCellRenderer *render;
    GtkTreeViewColumn *column;

    treeview = gtk_tree_view_new();
    g_object_set(G_OBJECT(treeview),
            "enable-search", FALSE,
            "headers-visible", FALSE,
            "sensitive", FALSE,
            NULL);

    render = gtk_cell_renderer_text_new();
#ifdef CT_355
    gtk_cell_renderer_set_fixed_size(render, -1, 33);
#else
    gtk_cell_renderer_set_fixed_size(render, -1, 25);
#endif
    column = gtk_tree_view_column_new_with_attributes("Porbe", render, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    model = create_probe_model();
    if(model != NULL)
        gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), model);
    g_object_unref(model);

    return treeview;
}

GtkTreeModel* ViewSuperuser::create_probe_model(void)
{
    GtkListStore  *store;
    GtkTreeIter    iter;

    store = gtk_list_store_new(1, G_TYPE_STRING);
    for (unsigned int i = 0; i < ProbeMan::MAX_SOCKET; i++) {
        gtk_list_store_append (store, &iter);
        gtk_list_store_set(store, &iter, 0, "No Probe", -1);
    }
    return GTK_TREE_MODEL (store);
}

void ViewSuperuser::MachineChanged(GtkComboBox *combobox)
{
}

void ViewSuperuser::ProbeChanged(GtkComboBox *combobox)
{
    m_probeType = gtk_combo_box_get_active(combobox);

    int i;
    char index[5];
    char name[20];
    for (i = 0; i < ProbeMan::MAX_SOCKET; i ++)
    {
        sprintf(index, "%d", i);
        sprintf(name, "%d: 无探头", i+1);
        UpdateProbeStatus(index, name);
    }
}

void ViewSuperuser::BtnWriteClicked(GtkButton *button)
{
    ASSERT(m_probeType < WPORBE_NUM);
    gtk_label_set_text(GTK_LABEL(m_labelHint), "探头参数写入中...");
    g_timeout_add(5, HandleWriteProbe, NULL);
}

void ViewSuperuser::BtnExitClicked(GtkButton *button)
{
    DestroyWindow();
}

#ifdef TRANSDUCER
void ViewSuperuser::SpinbuttonPressAdjust(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    char str[256];
    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    double pressad = atof(old_str.c_str());
    int press1 = (int) (pressad * 1000); //atoi(old_str.c_str());

    SysGeneralSetting sysGel;
    sysGel.SetPressAdjust(press1);
    sysGel.SyncFile();

    if (press1 < -20 || press1 > 20)
    {
        PRINTF("++++press is beyond value\n");
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    }

    return ;
}
#endif

void ViewSuperuser::ChgAperture(GtkComboBox *combobox)
{
    bool on;
    if (gtk_combo_box_get_active(combobox) == 0)
        on = FALSE;
    else
        on = TRUE;

    Img2D::GetInstance()->EnterSingleAperture(on);
}

gboolean ViewSuperuser::IsAuthenValid(void)
{
    m_timer = 0;
    if (m_vecAuthenInfo.empty() || ((!m_vecAuthenInfo.empty()) && m_vecAuthenInfo[0] != 'e'))
    {
        PRINTF("not authen\n");
        Exit();
    }
    else
    {
        PRINTF("authen\n");
    }

    return FALSE;
}

gboolean ViewSuperuser::WriteProbe(void)
{
    ProbeMan::GetInstance()->WriteProbe(m_probeType);

    sleep(1);
    ProbeSocket::ProbePara para[ProbeMan::MAX_SOCKET];
    ProbeMan::GetInstance()->GetAllProbe(para);
    int i;
    char index[5];
    char name[20];
    for (i = 0; i < ProbeMan::MAX_SOCKET; i ++)
    {
        sprintf(index, "%d", i);
        if (para[i].exist)
            sprintf(name, "%d: %s", i+1, para[i].model);
        else
            sprintf(name, "%d: 无探头", i+1);
        UpdateProbeStatus(index, name);
    }
    gtk_label_set_text(GTK_LABEL(m_labelHint), "");

    return FALSE;
}

bool ViewSuperuser::ConfirmAuthen(void)
{
    bool ret = false;
    unsigned char info[AUTHEN_NUM+1];
    int len = m_vecAuthenInfo.size();
    int size = (len > AUTHEN_NUM)?AUTHEN_NUM:len;
    memset(info, 0, sizeof(info));
    for (int i = 0; i < size; i++)
    {
        info[i] = m_vecAuthenInfo[len-size+i];
    }

      // compare
    if (strcmp((const char*)info, "emperor1997") == 0)
    {
        if (m_statusAuthen)
        {
            m_vecAuthenInfo.clear();
            CreateWindow();
            ret = true;
        }
    }
    return ret;
}

bool ViewSuperuser::ConfirmDemo(void)
{
    bool ret = false;
    unsigned char info[DEMO_NUM+1];
    int len = m_vecAuthenInfo.size();
    int size = (len > DEMO_NUM)?DEMO_NUM:len;
    memset(info, 0, sizeof(info));
    for (int i = 0; i < size; i++)
    {
        info[i] = m_vecAuthenInfo[len-size+i];
    }

      // compare
    if(strcmp((const char*)info, "e1997") == 0)
    {
        if (m_statusDemo)
        {
            m_vecAuthenInfo.clear();
            CreateDemoWindow();
            ret = true;
        }
    }
    return ret;
}

void ViewSuperuser::Exit(void)
{
    g_keyInterface.Pop();
}
/*
 * socket: a string to the path, "0" means the fisrt socket
 * status : a string to show
 */
gboolean ViewSuperuser::UpdateProbeStatus(const char* socket, const char* status)
{
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeviewProbe));

    GtkTreeIter iter;
    if(gtk_tree_model_get_iter_from_string(model, &iter, socket))
    {
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, status, -1);
        return TRUE;
    }
    else
        return FALSE;
}

static gboolean ExitWindow(gpointer data)
{
    ViewSuperuser *tmp;
    tmp = (ViewSuperuser*)data;
    tmp->DestroyWindow();
    return FALSE;
}
void ViewSuperuser::KeyEvent(unsigned char keyValue)
{
    switch(keyValue)
    {
        case KEY_ESC:
            g_timeout_add(100, ExitWindow, this);
            FakeEscKey();
            break;

        case KEY_SHIFT_CTRL:
            break;

        default:
            FakeXEvent::KeyEvent(keyValue);
            break;

#if 0

            if (m_statusAuthen)
            {
                Exit();
            }
            else if(m_statusDemo)
            {
                g_timeout_add(100, ExitWindow, this);
            }
            else
            {
                //DestroyWindow();
                g_timeout_add(100, ExitWindow, this);
                FakeEscKey();
            }
            break;

        default:
            m_vecAuthenInfo.push_back(keyValue);
            if(!ConfirmAuthen()&& !ConfirmDemo())
            {
                FakeXEvent::KeyEvent(keyValue);
            }
            break;
#endif
    }
}
//test
void ViewSuperuser::BtnEnterDemoClicked(GtkButton *button)
{
    m_statusAuthen = false;
    m_statusDemo = false;
    ViewMain::GetInstance()->MenuReviewCallBack();
    DestroyWindow();
}

void ViewSuperuser::ExitDemo()
{
    ViewMain::GetInstance()->MenuReviewCallBack();
    m_statusDemo = TRUE;
}

/**
 * test parameter of monitor
 */
void ViewSuperuser::BtnChgMonitorClicked(GtkButton *button)
{
    MonitorControl mc;
    mc.AutoAdjust();
    ScreenSaver::GetInstance()->EnterScreenSaver2();
}

void ViewSuperuser::BtnChgMonitorClicked2(GtkButton *button)
{
    MonitorControl mc;
    mc.AutoBalance();
    ScreenSaver::GetInstance()->EnterScreenSaver3();
}

#ifdef CT_PROJECT
/**
 * test for projectMode
 */
void ViewSuperuser::BtnProjectModeClicked(GtkButton *button)
{
    m_statusProjectMode = !m_statusProjectMode;
    if(!m_statusProjectMode)
    {
        MenuArea::GetInstance()->HideProjectModeMenu();
        KnobProjectMode::GetInstance()->HideKnob();
    }
    else
    {
        MenuArea::GetInstance()->ShowProjectModeMenu();
        KnobProjectMode::GetInstance()->ShowKnob();

        int harmonicIndex = Img2D::GetInstance()->GetHarmonicFreqIndex();
        int freqIndex = Img2D::GetInstance()->GetFreqIndex();
        string probe_type = TopArea::GetInstance()->GetProbeType();
        if(Img2D::GetInstance()->GetHarmonicStatus())
            ProjectCalc2D::GetInstance()->RefreshProjectCalc(probe_type.c_str(), harmonicIndex, true);
        else
            ProjectCalc2D::GetInstance()->RefreshProjectCalc(probe_type.c_str(), freqIndex, false);

            ProjectCalc2D::GetInstance()->InitKnobPara();
    }

    HintArea::GetInstance()->UpdateHintArea();
    DestroyWindow();
}
#endif

GtkWidget* ViewSuperuser::CreateNoteProbe(void)
{
    GtkWidget *fixed = gtk_fixed_new();
    gtk_widget_show (fixed);

    GtkWidget *frameProbe = gtk_frame_new (NULL);
    gtk_fixed_put (GTK_FIXED (fixed), frameProbe, 20, 10);
    gtk_widget_set_size_request (frameProbe, 440, 140);
    gtk_frame_set_shadow_type (GTK_FRAME (frameProbe), GTK_SHADOW_OUT);

    GtkWidget *labelProbe = gtk_label_new ("探头工具");
    gtk_frame_set_label_widget (GTK_FRAME (frameProbe), labelProbe);
    gtk_label_set_use_markup (GTK_LABEL (labelProbe), TRUE);

    GtkWidget *tableProbe = gtk_table_new(3, 2, FALSE);
    gtk_table_set_row_spacing(GTK_TABLE(tableProbe), 0, 10);
    gtk_table_set_col_spacings(GTK_TABLE(tableProbe), 10);
    gtk_container_set_border_width(GTK_CONTAINER(tableProbe), 10);
    gtk_container_add (GTK_CONTAINER (frameProbe), tableProbe);

    GtkWidget *comboboxMachine = gtk_combo_box_new_text();
    gtk_table_attach_defaults(GTK_TABLE(tableProbe), comboboxMachine, 0, 1, 0, 1);

#if defined(CT_360)
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "G60");
#elif defined(CT_161)
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "EMP-3000");
#elif defined(CT_322)
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "EMP-2900Plus");
#elif defined(CT_313)
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "EMP-2800");
#elif (defined(CT_430)|| defined(CT_440))
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "A60");
#elif defined(CT_355)
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "G30Plus");
#else
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxMachine), "G70");
#endif

    gtk_combo_box_set_active(GTK_COMBO_BOX(comboboxMachine), 0);
    g_signal_connect (G_OBJECT(comboboxMachine), "changed", G_CALLBACK(HandleMachineChanged), this);

    GtkWidget *comboboxProbe = gtk_combo_box_new_text();
    gtk_table_attach_defaults(GTK_TABLE(tableProbe), comboboxProbe, 0, 1, 1, 2);
    int i;
    for (i = 0; i < WPORBE_NUM; i ++)
    {
        gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxProbe), m_probeName[i].c_str());
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(comboboxProbe), 0);
    m_probeType = 0;
    g_signal_connect (G_OBJECT(comboboxProbe), "changed", G_CALLBACK(HandleProbeChanged), this);

    GtkWidget *labelWrite = gtk_label_new_with_mnemonic("写探头");
    GtkWidget *imageWrite = gtk_image_new_from_stock(GTK_STOCK_EDIT, GTK_ICON_SIZE_BUTTON);
    GtkWidget *btnWrite = create_button_icon(labelWrite, imageWrite);
    gtk_table_attach_defaults(GTK_TABLE(tableProbe), btnWrite, 1, 2, 0, 2);
    g_signal_connect (G_OBJECT(btnWrite), "clicked", G_CALLBACK(HandleBtnWriteClicked), this);

    m_treeviewProbe = create_probe_treeview();
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(m_treeviewProbe), GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    gtk_table_attach_defaults(GTK_TABLE(tableProbe), m_treeviewProbe, 2, 3, 0, 2);

    // single aperture
    GtkWidget *frameAperture = gtk_frame_new (NULL);
    gtk_fixed_put (GTK_FIXED (fixed), frameAperture, 20, 180);
    gtk_widget_set_size_request (frameAperture, 200-90, 70);
    gtk_frame_set_shadow_type (GTK_FRAME (frameAperture), GTK_SHADOW_OUT);

    GtkWidget *labelAperutre = gtk_label_new ("暗道测试工具");
    gtk_frame_set_label_widget (GTK_FRAME (frameAperture), labelAperutre);
    gtk_label_set_use_markup (GTK_LABEL (labelAperutre), TRUE);

    GtkWidget *tableAperture = gtk_table_new(1, 1, FALSE);
    gtk_table_set_row_spacing(GTK_TABLE(tableAperture), 0, 10);
    gtk_table_set_col_spacings(GTK_TABLE(tableAperture), 10);
    gtk_container_set_border_width(GTK_CONTAINER(tableAperture), 10);
    gtk_container_add (GTK_CONTAINER (frameAperture), tableAperture);

    GtkWidget *comboboxAperture = gtk_combo_box_new_text();
    gtk_table_attach_defaults(GTK_TABLE(tableAperture), comboboxAperture, 0, 1, 0, 1);
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxAperture), "OFF");
    gtk_combo_box_append_text(GTK_COMBO_BOX(comboboxAperture), "ON");
    gtk_combo_box_set_active(GTK_COMBO_BOX(comboboxAperture), -1);
    g_signal_connect (G_OBJECT(comboboxAperture), "changed", G_CALLBACK(HandleChgAperture), this);

#ifdef TRANSDUCER
   // Transducer
    SysGeneralSetting *sysGel = new SysGeneralSetting;
    int index_press = sysGel->GetPressAjust();

    GtkWidget *framePress = gtk_frame_new (NULL);
    gtk_widget_show (framePress);
    //gtk_fixed_put (GTK_FIXED (fixed), framePress, 20, 290+78);//90
    gtk_fixed_put (GTK_FIXED (fixed), framePress, 20+200-90+10, 180);//90
    gtk_widget_set_size_request (framePress, 440-40, 70); //550
    gtk_frame_set_shadow_type (GTK_FRAME (framePress), GTK_SHADOW_OUT);

    GtkWidget *labelPress = gtk_label_new ("压力校准");
    gtk_widget_show (labelPress);
    gtk_frame_set_label_widget (GTK_FRAME (framePress), labelPress);
    gtk_label_set_use_markup (GTK_LABEL (labelPress), TRUE);

    GtkWidget *fixedPress = gtk_fixed_new ();
    gtk_widget_show (fixedPress);
    gtk_container_add (GTK_CONTAINER (framePress), fixedPress);

    GtkWidget *tablePress = gtk_table_new (1, 2, FALSE); //14
    gtk_widget_show (tablePress);
    gtk_fixed_put (GTK_FIXED (fixedPress), tablePress, 5, 10);
    gtk_widget_set_size_request (tablePress, 200, 20);//520
    gtk_table_set_row_spacings (GTK_TABLE(tablePress), 10);

    // press adjust
    GtkWidget *labelPressAdjust = gtk_label_new ("校准值");
    gtk_widget_show (labelPressAdjust);
    gtk_widget_set_size_request (labelPressAdjust, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (tablePress), labelPressAdjust, 0, 1, 0, 1);

    GtkObject *spinButtonPressAdjust = gtk_adjustment_new (0, -0.02, 0.02, 0.001, 0.001, 0);
    GtkWidget *m_spinbuttonPressAdjust= gtk_spin_button_new (GTK_ADJUSTMENT (spinButtonPressAdjust), 1, 3);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbuttonPressAdjust), (double)index_press/1000.0);
    gtk_widget_show (m_spinbuttonPressAdjust);
    gtk_widget_set_size_request (m_spinbuttonPressAdjust, -1, 27);
    gtk_table_attach_defaults (GTK_TABLE (tablePress), m_spinbuttonPressAdjust, 1, 2, 0, 1);
    g_signal_connect(G_OBJECT(m_spinbuttonPressAdjust), "insert_text", G_CALLBACK(on_spinbutton_press_adjust), this);

    GtkWidget *label_press = gtk_label_new ("（默认为0，范围正负0.02）");
    gtk_widget_show (label_press);
    gtk_fixed_put (GTK_FIXED (fixedPress), label_press, 220, 10);
    gtk_widget_set_size_request (label_press, 200, 30);
    gtk_misc_set_alignment (GTK_MISC (label_press), 0, 0.5);
#endif

    ///> probe alias
    GtkWidget *frameAlias = gtk_frame_new (NULL);
    gtk_widget_show (frameAlias);
    gtk_fixed_put (GTK_FIXED (fixed), frameAlias, 20, 180+70+20);
    gtk_widget_set_size_request (frameAlias, 450, 230);
    gtk_frame_set_shadow_type (GTK_FRAME (frameAlias), GTK_SHADOW_OUT);

    GtkWidget *labelAlias = gtk_label_new ("探头别名");
    gtk_widget_show (labelAlias);
    gtk_frame_set_label_widget (GTK_FRAME (frameAlias), labelAlias);
    gtk_label_set_use_markup (GTK_LABEL (labelAlias), TRUE);

    GtkWidget *fixedProbeAlias = gtk_fixed_new ();
    gtk_widget_show (fixedProbeAlias);
    gtk_container_add (GTK_CONTAINER (frameAlias), fixedProbeAlias);

    ///>创建列表模式(数据库)
    GtkListStore *store = gtk_list_store_new(END_COLUMN, G_TYPE_STRING, G_TYPE_STRING);

    ///>向模式(数据库)中添加数据
    char path[100] = {"0"};
    sprintf(path, "%s/%s", CFG_RES_PATH, PROBE_ALIAS_FILE);
    IniFile ini(path);
    char alias[56] = {"0"};

    GtkTreeIter iter;
    for(int i = 0; i < NUM_PROBE; i++)
    {
        ProbeMan::GetInstance()->ReadProbeAlias(&ini, PROBE_LIST[i].c_str(), alias);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                EPM_COLUMN, PROBE_LIST[i].c_str(),
                EPA_COLUMN, alias,
                -1);
        memset(alias, 0, sizeof(alias));
    }
    ///>创建一个视图
    m_treeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(m_treeView), FALSE);//关闭列表搜索功能
    GtkWidget *scrollWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_fixed_put(GTK_FIXED(fixedProbeAlias), scrollWindow, 10, 5);
    gtk_widget_set_size_request (scrollWindow, 280, 200);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollWindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrollWindow), GTK_SHADOW_IN);
    gtk_container_set_border_width (GTK_CONTAINER (scrollWindow), 5);
    gtk_container_add(GTK_CONTAINER(scrollWindow), m_treeView);

    GtkCellRenderer *render = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(render, 0.5, 0.5);
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("探头型号", render, "text", EPM_COLUMN, NULL);
    gtk_tree_view_column_set_alignment(column, 0.5);
    gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeView), column);
    g_object_set(G_OBJECT(column), "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);

    m_render = gtk_cell_renderer_text_new();
    g_object_set(m_render, "editable", FALSE, NULL);
    gtk_cell_renderer_set_alignment(m_render, 0.5, 0.5);
    column = gtk_tree_view_column_new_with_attributes("别名", m_render, "text", EPA_COLUMN, NULL);
    gtk_tree_view_column_set_alignment(column, 0.5);
    gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeView), column);

    GtkTreeSelection *selected;
    selected = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeView));
    gtk_tree_selection_set_mode (gtk_tree_view_get_selection(GTK_TREE_VIEW (m_treeView)), GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(selected), "changed", G_CALLBACK(HandleTreeSelectionChanged), this);

    g_object_unref(G_OBJECT(store));//视图销毁数据也销毁

    m_btn_edit = gtk_button_new_with_mnemonic("编辑");
    gtk_widget_set_size_request(m_btn_edit, 100, 50);
    gtk_widget_modify_bg(m_btn_edit, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_widget_set_sensitive(m_btn_edit, false);
    gtk_fixed_put(GTK_FIXED(fixedProbeAlias),m_btn_edit, 280+30, 33);
    g_signal_connect ((gpointer) m_btn_edit, "clicked",
    //g_signal_connect (m_btn_edit, "clicked",
            G_CALLBACK (on_button_edit_clicked),
            this);

    GtkWidget *btn_factory = gtk_button_new_with_mnemonic("恢复出厂设置");
    gtk_widget_set_size_request(btn_factory, 100, 50);
    gtk_fixed_put(GTK_FIXED(fixedProbeAlias), btn_factory, 280+30, 80+33);
    g_signal_connect ((gpointer) btn_factory, "clicked",
            G_CALLBACK (on_button_factory_clicked),
            this);

    return fixed;
}

GtkWidget* ViewSuperuser::CreateNoteMonitor(void)
{
    GtkWidget *fixed = gtk_fixed_new();
    gtk_widget_show (fixed);

    //test the parameter of monitor
    GtkWidget *btnMonitor = gtk_button_new_with_mnemonic("显示器测试");
    g_signal_connect(G_OBJECT(btnMonitor), "clicked", G_CALLBACK(HandleBtnChgMonitorClicked), this);
    gtk_fixed_put (GTK_FIXED (fixed), btnMonitor, 20, 10);
    gtk_widget_set_size_request (btnMonitor, 180, 68);

    GtkWidget *btnMonitor2 = gtk_button_new_with_mnemonic("显示器测试 2");
    g_signal_connect(G_OBJECT(btnMonitor2), "clicked", G_CALLBACK(HandleBtnChgMonitorClicked2), this);
    gtk_fixed_put (GTK_FIXED (fixed), btnMonitor2, 20, 10+68+20);
    gtk_widget_set_size_request (btnMonitor2, 180, 68);

    return fixed;
}

#ifdef CT_PROJECT
GtkWidget* ViewSuperuser::CreateNoteProject(void)
{
    GtkWidget *fixed = gtk_fixed_new();
    gtk_widget_show (fixed);

    GtkWidget *btnProjectMode;
    if(m_statusProjectMode)
        btnProjectMode = gtk_button_new_with_mnemonic("关闭工程模式");
    else
        btnProjectMode = gtk_button_new_with_mnemonic("开启工程模式");
    g_signal_connect(G_OBJECT(btnProjectMode), "clicked", G_CALLBACK(HandleProjectModeClicked), this);
    gtk_fixed_put (GTK_FIXED (fixed), btnProjectMode, 20, 10);
    gtk_widget_set_size_request (btnProjectMode, 180, 68);

    return fixed;
}
#endif

void ViewSuperuser::TreeSelectionChanged(GtkTreeSelection *selection)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    //如果没有选中任何结点,直接返回
    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE)
    return;

    //g_print("model=%s, len=%d, alias=%s, len=%d, alias size=%d\n", m_probeModel, strlen(m_probeModel), m_probeAlias, strlen(m_probeAlias), sizeof(m_probeAlias));
    if(strlen(m_probeModel)!=0 & strlen(m_probeAlias)!=0)
    {
        ///> write probe alias to config file
        char file_path[256]  = {"0"};
        sprintf(file_path, "%s/%s", CFG_RES_PATH, PROBE_ALIAS_FILE);
        IniFile ini(file_path);
        ProbeMan::GetInstance()->WriteProbeAlias(&ini, (char*)m_probeModel, (char*)m_probeAlias);
        memset(m_probeModel, 0, 32);
        if(!m_strProbeAlias.empty())
            m_strProbeAlias.clear();
        memset(m_probeAlias, 0, sizeof(m_probeAlias));
    }

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    int tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);

    /* get toggled iter */
    gchar *probeModel;
    gtk_tree_model_get (model, &iter, 0, &probeModel, -1);
    //sprintf(m_probeModel, "%s", probeModel);
    strcpy(m_probeModel, g_strdup(probeModel));
    g_object_set(m_render, "editable", FALSE, NULL);
    gtk_widget_set_sensitive(m_btn_edit, TRUE);
}

/**
 *@brief: edit probe alias
 *
 */
void ViewSuperuser::BtnEditClicked(GtkButton *button)
{
    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeView));
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE)
    {
        PRINTF("edit return");
        return;
    }

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);

    g_object_set(m_render, "editable", TRUE, NULL);
   // g_signal_connect(m_render, "editing_started", G_CALLBACK(on_entry_probe_alias_insert), this);//>会导致新建病人等界面出现编辑时候的地一个字符.
    g_signal_connect(m_render, "edited", G_CALLBACK(on_edited_cell_callback), this);

    //设置编辑状态
    GtkTreeViewColumn *column_tree_view;
    column_tree_view = gtk_tree_view_get_column(GTK_TREE_VIEW(m_treeView), 1);

    gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(m_treeView),
                         path,
                         column_tree_view,
                         m_render,
                         TRUE);
    gtk_tree_path_free (path);
}

/**
 *@brief: probe alias the same as probe model
 *
 */
void ViewSuperuser::BtnFactoryClicked(GtkButton *button)
{
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeView));
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first(model, &iter);

    gchar *alias;
    gtk_tree_model_get (model, &iter, 1, &alias, -1);

    for(int i = 0; i < NUM_PROBE; i++)
    {
        gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                EPM_COLUMN, PROBE_LIST[i].c_str(),
                EPA_COLUMN, PROBE_LIST[i].c_str(),
                -1);
        gtk_tree_model_iter_next(model, &iter);
    }

    ///> write probe alias to config file
    char file_path[256]  = {"0"};
    sprintf(file_path, "%s/%s", CFG_RES_PATH, PROBE_ALIAS_FILE);
    IniFile ini(file_path);
    for(int i = 0; i < NUM_PROBE; i++)
        ProbeMan::GetInstance()->WriteProbeAlias(&ini, (char*)PROBE_LIST[i].c_str(), (char*)PROBE_LIST[i].c_str());
}

void ViewSuperuser:: EditedCellChanged(GtkCellRendererText *cell, gchar *path_string, gchar *new_text)
{
    //GtkTreeModel *model = (GtkTreeModel*)data;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeView));
    GtkTreePath *path;
    path = gtk_tree_path_new_from_string(path_string);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, path);

    if (strlen(new_text) != 0)
    {
        char *tmp_text = g_strdup(new_text);
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, 1, tmp_text, -1);
        g_free(tmp_text);
    }
    gtk_tree_path_free(path);
    sprintf(m_probeAlias, "%s", new_text);
}

void ViewSuperuser::EntryProbeAliasInsert(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path)
{
    if(GTK_IS_ENTRY(editable))
    {
        g_signal_connect(G_OBJECT(editable), "insert_text", G_CALLBACK(on_entry_insert_item), this);
    }
}

void ViewSuperuser::EntryProbeAliasLengthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));

    m_strProbeAlias = g_strdup(gtk_entry_get_text(GTK_ENTRY(editable)));
    strcat((char*)m_strProbeAlias.c_str(), new_text);

    if(old_text_length + new_text_length >10) {
        gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
        return;
    }
}