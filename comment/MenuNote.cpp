#include <gtk/gtk.h>
#include "Def.h"
#include "gui_global.h"
#include "gui_func.h"
#include "display/TopArea.h"
#include "MenuArea.h"
#include "MenuNote.h"
#include "NoteArea.h"
#include "ExamItem.h"
#include "ProbeSelect.h"
#include "ViewSystem.h"
#include "MenuProjectMode.h"
#include "ViewSuperuser.h"

MenuNote g_menuNote;


MenuNote::MenuNote(void)
{

    m_table = 0;
    m_modelDept = 0;
}

void MenuNote::Hide(void)
{
   gtk_combo_box_popdown(GTK_COMBO_BOX(m_comboDept));
   gtk_widget_hide_all(m_table);
    //	gtk_widget_hide_all(m_fixed);
}

void MenuNote::Show(void)
{
UpdateLabel();
 gtk_widget_show_all(m_table);


 //根据不同的探头部位设置不同科别
	int index = 0;
	ExamItem::EItem item = ProbeSelect::GetItemIndex();
   char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    string current_item;
    string current_probe;
    char current_exam[256];
    ExamItem exam;
    current_item = exam.ReadDefaultProbeDefaultItemName(&ini);
    current_probe = exam.ReadDefaultProbe(&ini);

    exam.TransItemNameEng(current_item.c_str(), current_exam);

    string username;
    username = exam.ReadDefaultUserSelect(&ini);
    char path2[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path2, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    }
    else
    {
        sprintf(path2, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
    }
    IniFile new_ini(path2);
    IniFile *ptrIni= &new_ini;


   /* char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    IniFile new_ini(path1);
    IniFile *ptrIni= &new_ini;*/
    int number;
    number = ptrIni->ReadInt((current_probe + "-" + current_exam).c_str(), "Number");
    if (number)
    {
        index=0;
    }
   else
    {
#ifdef VET
        index = 1;
#else
        switch(item)
        {
            case ExamItem::ABDO_ADULT:
            case ExamItem::ABDO_LIVER:
            case ExamItem::ABDO_KID:
            case ExamItem::CAR_KID:
            case ExamItem::USER1:
            case ExamItem::USER2:
            case ExamItem::USER3:
            case ExamItem::USER4:
                index = 1;
                break;
            case ExamItem::KIDNEY:
            case ExamItem::BLADDER:
                index = 2;
                break;
            case ExamItem::CAR_ADULT:
                index = 3;
                break;
            case ExamItem::EARLY_PREG:
            case ExamItem::LATER_PREG:
                index = 4;
                break;
            case ExamItem::GYN:
                index = 5;
                break;
            case ExamItem::SMALL_PART:
            case ExamItem::GLANDS:
            case ExamItem::THYROID:
            case ExamItem::EYE:
                index = 6;
                break;
#ifdef EMP_322
            case ExamItem::HIP_JOINT:
            case ExamItem::JOINT_CAVITY:
            case ExamItem::MENISCUS:
            case ExamItem::SPINE:
                index = 7;
                break;

#else
            case ExamItem::CAROTID:
            case ExamItem::JUGULAR:
            case ExamItem::PERI_ARTERY:
            case ExamItem::PERI_VEIN:
                index = 7;
                break;
            case ExamItem::HIP_JOINT:
            case ExamItem::JOINT_CAVITY:
            case ExamItem::MENISCUS:
            case ExamItem::SPINE:
                index = 8;
                break;
#endif
            default:
               index = 1;
               break;
        }
#endif
    }

   gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboDept), index);
   char department[256];
   if(index ==0)
   {
       char path[256];
       sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
       IniFile ini(path);
       string current_item;
       string current_probe;
       char current_exam[256];
       ExamItem exam;
       current_item = exam.ReadDefaultProbeDefaultItemName(&ini);
       current_probe = exam.ReadDefaultProbe(&ini);
       exam.TransItemNameEng(current_item.c_str(), current_exam);

       strcpy(department,(current_probe + "-" + current_exam).c_str());
   }
   else
   {
       DepartmentName(department, index);

   }

   HideMenu();
   ShowMenu(index, department);


}
void MenuNote::ShowMenu(int index, char *department)
{
#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&g_menuProjectMode.ShowStatus())
    {
        MenuArea::GetInstance()->ShowProjectModeMenu();
        return;
    }
#endif
    GtkTreeModel *model = create_item_note_model(index, department);

    if(index==0)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_current_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_current_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_current_comment);
        gtk_widget_show(m_treeview_item_current_comment);
        gtk_widget_show(scrolledwindow_item_current_comment);
    }
    else if(index==1)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_abdo_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_abdo_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_abdo_comment);
        gtk_widget_show(m_treeview_item_abdo_comment);
        gtk_widget_show(scrolledwindow_item_abdo_comment);
    }

    else if(index==2)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_uro_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_uro_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_uro_comment);
        gtk_widget_show(m_treeview_item_uro_comment);
        gtk_widget_show(scrolledwindow_item_uro_comment);
    }
    else if(index==3)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_car_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_car_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_car_comment);
        gtk_widget_show(m_treeview_item_car_comment);
        gtk_widget_show(scrolledwindow_item_car_comment);
    }
    else if(index==4)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_ob_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_ob_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_ob_comment);
        gtk_widget_show(m_treeview_item_ob_comment);
        gtk_widget_show(scrolledwindow_item_ob_comment);
    }
#ifndef VET
    else if(index==5)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_gyn_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_gyn_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_gyn_comment);
        gtk_widget_show(m_treeview_item_gyn_comment);
        gtk_widget_show(scrolledwindow_item_gyn_comment);
    }
#endif
#ifdef VET
    else if(index==5)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_sp_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_sp_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_sp_comment);
        gtk_widget_show(m_treeview_item_sp_comment);
        gtk_widget_show(scrolledwindow_item_sp_comment);
    }
#else
    else if(index==6)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_sp_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_sp_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_sp_comment);
        gtk_widget_show(m_treeview_item_sp_comment);
        gtk_widget_show(scrolledwindow_item_sp_comment);
    }

#endif

#ifdef EMP_322
    else if(index==7)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_ortho_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_ortho_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_ortho_comment);
        gtk_widget_show(m_treeview_item_ortho_comment);
        gtk_widget_show(scrolledwindow_item_ortho_comment);
    }

#else
#ifdef VET
    else if(index==6)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_vs_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_vs_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_vs_comment);
        gtk_widget_show(m_treeview_item_vs_comment);
        gtk_widget_show(scrolledwindow_item_vs_comment);
    }

    else if(index==7)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_ortho_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_ortho_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_ortho_comment);
        gtk_widget_show(m_treeview_item_ortho_comment);
        gtk_widget_show(scrolledwindow_item_ortho_comment);
    }

#else
    else if(index==7)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_vs_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_vs_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_vs_comment);
        gtk_widget_show(m_treeview_item_vs_comment);
        gtk_widget_show(scrolledwindow_item_vs_comment);
    }

    else if(index==8)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_ortho_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_ortho_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_ortho_comment);
        gtk_widget_show(m_treeview_item_ortho_comment);
        gtk_widget_show(scrolledwindow_item_ortho_comment);
    }
#endif
#endif

#ifdef VET
 else if(index==8)
    {
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_tendon_comment), NULL);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_tendon_comment), model);
        g_object_unref(model);
        gtk_widget_queue_draw(m_treeview_item_tendon_comment);
        gtk_widget_show(m_treeview_item_tendon_comment);
        gtk_widget_show(scrolledwindow_item_tendon_comment);
    }

#endif


}

void MenuNote::UpdateScaleModel(void)
{

    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;
    string current_item;
    current_item = exam.ReadDefaultProbeDefaultItemName(&ini);

    GtkTreeIter iter;
    GtkListStore *store = GTK_LIST_STORE(m_modelDept);

    gtk_tree_model_get_iter_first(m_modelDept, &iter);
    gtk_list_store_set(store, &iter, 0, _(current_item.c_str()), -1);   //probe_exam_string.c_str(), -1);
    gtk_tree_model_iter_next(m_modelDept, &iter);
    gtk_list_store_set(store, &iter, 0, _("Abdomen"), -1);
    gtk_tree_model_iter_next(m_modelDept, &iter);
    gtk_list_store_set(store, &iter, 0, _("Urology"), -1);
    gtk_tree_model_iter_next(m_modelDept, &iter);
    gtk_list_store_set(store, &iter, 0, _("Cardiac"), -1);
    gtk_tree_model_iter_next(m_modelDept, &iter);
    gtk_list_store_set(store, &iter, 0, _("Obstetrics"), -1);
#ifndef VET
    gtk_tree_model_iter_next(m_modelDept, &iter);
    gtk_list_store_set(store, &iter, 0, _("Gynecology"), -1);
#endif
    gtk_tree_model_iter_next(m_modelDept, &iter);
    gtk_list_store_set(store, &iter, 0, _("Small Part"), -1);
#ifndef EMP_322
    gtk_tree_model_iter_next(m_modelDept, &iter);
    gtk_list_store_set(store, &iter, 0, _("Vascular"), -1);
#endif
    gtk_tree_model_iter_next(m_modelDept, &iter);
    gtk_list_store_set(store, &iter, 0, _("Orthopedic"), -1);
#ifdef VET
    gtk_tree_model_iter_next(m_modelDept, &iter);
    gtk_list_store_set(store, &iter, 0, _("Tendon"), -1);

#endif
}


void MenuNote::UpdateLabel(void)
{
    UpdateScaleModel();

}

GtkWidget* MenuNote::Create(void)
{

    m_table = gtk_table_new(18, 6, TRUE);
    gtk_table_set_row_spacing(GTK_TABLE(m_table), 0, 5);

    m_modelDept = CreateDeptModel();
    m_comboDept = create_combobox(0, 0, "text", m_modelDept);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_comboDept, 1, 5, 0, 1);


    g_signal_connect(m_comboDept, "changed", G_CALLBACK(HandleComboDeptChanged), this);

    GtkWidget *MenuCurrent = create_current_item();
    gtk_table_attach_defaults(GTK_TABLE(m_table), MenuCurrent, 0, 6, 1, 18);

    GtkWidget *MenuAbdo = create_abdo_item();
    gtk_table_attach_defaults(GTK_TABLE(m_table), MenuAbdo, 0, 6, 1, 18);

    GtkWidget *MenuUR = create_uro_item();
    gtk_table_attach_defaults(GTK_TABLE(m_table), MenuUR, 0, 6, 1, 18);

    GtkWidget *MenuCar = create_car_item();
    gtk_table_attach_defaults(GTK_TABLE(m_table), MenuCar, 0, 6, 1, 18);

    GtkWidget *MenuOb = create_ob_item();
    gtk_table_attach_defaults(GTK_TABLE(m_table), MenuOb, 0, 6, 1, 18);
#ifndef VET
    GtkWidget *MenuGYN = create_gyn_item();
    gtk_table_attach_defaults(GTK_TABLE(m_table), MenuGYN, 0, 6, 1, 18);
#endif
    GtkWidget *MenuSP = create_sp_item();
    gtk_table_attach_defaults(GTK_TABLE(m_table), MenuSP, 0, 6, 1, 18);
#ifndef EMP_322
    GtkWidget *MenuVS = create_vs_item();
    gtk_table_attach_defaults(GTK_TABLE(m_table), MenuVS, 0, 6, 1, 18);
#endif
    GtkWidget *MenuOrtho = create_ortho_item();
    gtk_table_attach_defaults(GTK_TABLE(m_table), MenuOrtho, 0, 6, 1, 18);
#ifdef VET
    GtkWidget *MenuTendon = create_tendon_item();
    gtk_table_attach_defaults(GTK_TABLE(m_table), MenuTendon, 0, 6, 1, 18);
#endif
    return m_table;

}

GtkWidget* MenuNote::create_item_list(int index , char *department)
{
    /* creating a model */
    GtkTreeModel *model;
    model = create_item_note_model(index, department);

    /* creating the view */
    m_treeView_comment = gtk_tree_view_new_with_model(model);
   add_columns_comment(GTK_TREE_VIEW(m_treeView_comment));

    /* set view property */
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(m_treeView_comment), FALSE);
    gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(m_treeView_comment), TRUE);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(m_treeView_comment), FALSE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(m_treeView_comment), TRUE);

    /* selection handling */
    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeView_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);
     gtk_widget_add_events(GTK_WIDGET(m_treeView_comment), (gtk_widget_get_events(GTK_WIDGET(m_treeView_comment)) | GDK_BUTTON_RELEASE_MASK));
    gtk_tree_view_collapse_all(GTK_TREE_VIEW(m_treeView_comment));
    return m_treeView_comment;
}


GtkWidget * MenuNote::create_current_item()
{

     char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;
    string current_item;
    string current_probe;
    char current_exam[256];
    current_item = exam.ReadDefaultProbeDefaultItemName(&ini);
    current_probe = exam.ReadDefaultProbe(&ini);
    exam.TransItemNameEng(current_item.c_str(), current_exam);
    string probe_exam_string= current_probe + "-" + current_exam;
    char department[256];
    strcpy(department, probe_exam_string.c_str());

    scrolledwindow_item_current_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_current_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_item_current_comment), GTK_SHADOW_IN);

    m_treeview_item_current_comment=create_item_list(0, department);
    //gtk_widget_modify_base(m_treeview_item_current_comment, GTK_STATE_NORMAL, g_deep);


	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeview_item_current_comment), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(m_treeview_item_current_comment), TRUE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeview_item_current_comment), FALSE);
	gtk_widget_modify_base (m_treeview_item_current_comment, GTK_STATE_NORMAL, g_deep);
	gtk_widget_modify_base (m_treeview_item_current_comment, GTK_STATE_INSENSITIVE, g_deep);
	gtk_widget_modify_text (m_treeview_item_current_comment, GTK_STATE_INSENSITIVE, g_white);
	gtk_widget_modify_text (m_treeview_item_current_comment, GTK_STATE_ACTIVE, g_white);


	g_signal_connect (G_OBJECT (m_treeview_item_current_comment), "button-press-event", G_CALLBACK (HandleTreeBtnPress), NULL);



    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_current_comment), m_treeview_item_current_comment);

    gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_item_current_comment));
    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_current_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    return  scrolledwindow_item_current_comment;
}

GtkWidget * MenuNote::create_abdo_item()
{
    scrolledwindow_item_abdo_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_abdo_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_item_abdo_comment), GTK_SHADOW_IN);
    m_treeview_item_abdo_comment= create_item_list(1, "Abdomen");

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeview_item_abdo_comment), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(m_treeview_item_abdo_comment), TRUE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeview_item_abdo_comment), FALSE);
	gtk_widget_modify_base (m_treeview_item_abdo_comment, GTK_STATE_NORMAL, g_deep);
	gtk_widget_modify_base (m_treeview_item_abdo_comment, GTK_STATE_INSENSITIVE, g_deep);
	gtk_widget_modify_text (m_treeview_item_abdo_comment, GTK_STATE_INSENSITIVE, g_white);
	gtk_widget_modify_text (m_treeview_item_abdo_comment, GTK_STATE_ACTIVE, g_white);


	g_signal_connect (G_OBJECT (m_treeview_item_abdo_comment), "button-press-event", G_CALLBACK (HandleTreeBtnPress), NULL);


    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_abdo_comment), m_treeview_item_abdo_comment);


    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_abdo_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    return  scrolledwindow_item_abdo_comment;
}


GtkWidget * MenuNote::create_uro_item()
{
     scrolledwindow_item_uro_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_uro_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_item_uro_comment), GTK_SHADOW_IN);
    m_treeview_item_uro_comment = create_item_list(2, "Urology");

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeview_item_uro_comment), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(m_treeview_item_uro_comment), TRUE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeview_item_uro_comment), FALSE);
	gtk_widget_modify_base (m_treeview_item_uro_comment, GTK_STATE_NORMAL, g_deep);
	gtk_widget_modify_base (m_treeview_item_uro_comment, GTK_STATE_INSENSITIVE, g_deep);
	gtk_widget_modify_text (m_treeview_item_uro_comment, GTK_STATE_INSENSITIVE, g_white);
	gtk_widget_modify_text (m_treeview_item_uro_comment, GTK_STATE_ACTIVE, g_white);


	g_signal_connect (G_OBJECT (m_treeview_item_uro_comment), "button-press-event", G_CALLBACK (HandleTreeBtnPress), NULL);


    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_uro_comment), m_treeview_item_uro_comment);


    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_uro_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    return  scrolledwindow_item_uro_comment;
}


GtkWidget * MenuNote::create_car_item()
{

  scrolledwindow_item_car_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_car_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_item_car_comment), GTK_SHADOW_IN);
    m_treeview_item_car_comment = create_item_list(3, "Cardiac");

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeview_item_car_comment), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(m_treeview_item_car_comment), TRUE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeview_item_car_comment), FALSE);
	gtk_widget_modify_base (m_treeview_item_car_comment, GTK_STATE_NORMAL, g_deep);
	gtk_widget_modify_base (m_treeview_item_car_comment, GTK_STATE_INSENSITIVE, g_deep);
	gtk_widget_modify_text (m_treeview_item_car_comment, GTK_STATE_INSENSITIVE, g_white);
	gtk_widget_modify_text (m_treeview_item_car_comment, GTK_STATE_ACTIVE, g_white);


	g_signal_connect (G_OBJECT (m_treeview_item_car_comment), "button-press-event", G_CALLBACK (HandleTreeBtnPress), NULL);


    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_car_comment), m_treeview_item_car_comment);


    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_car_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    return  scrolledwindow_item_car_comment;

}
GtkWidget * MenuNote::create_ob_item()
{
  scrolledwindow_item_ob_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_ob_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_item_ob_comment), GTK_SHADOW_IN);
    m_treeview_item_ob_comment = create_item_list(4, "Obstetrics");

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeview_item_ob_comment), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(m_treeview_item_ob_comment), TRUE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeview_item_ob_comment), FALSE);
	gtk_widget_modify_base (m_treeview_item_ob_comment, GTK_STATE_NORMAL, g_deep);
	gtk_widget_modify_base (m_treeview_item_ob_comment, GTK_STATE_INSENSITIVE, g_deep);
	gtk_widget_modify_text (m_treeview_item_ob_comment, GTK_STATE_INSENSITIVE, g_white);
	gtk_widget_modify_text (m_treeview_item_ob_comment, GTK_STATE_ACTIVE, g_white);


	g_signal_connect (G_OBJECT (m_treeview_item_ob_comment), "button-press-event", G_CALLBACK (HandleTreeBtnPress), NULL);


    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_ob_comment), m_treeview_item_ob_comment);


    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_ob_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    return  scrolledwindow_item_ob_comment;
}

GtkWidget * MenuNote::create_gyn_item()
{
    scrolledwindow_item_gyn_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_gyn_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_item_gyn_comment), GTK_SHADOW_IN);
     m_treeview_item_gyn_comment = create_item_list(5, "Gynecology");

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeview_item_gyn_comment), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(m_treeview_item_gyn_comment), TRUE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeview_item_gyn_comment), FALSE);
	gtk_widget_modify_base (m_treeview_item_gyn_comment, GTK_STATE_NORMAL, g_deep);
	gtk_widget_modify_base (m_treeview_item_gyn_comment, GTK_STATE_INSENSITIVE, g_deep);
	gtk_widget_modify_text (m_treeview_item_gyn_comment, GTK_STATE_INSENSITIVE, g_white);
	gtk_widget_modify_text (m_treeview_item_gyn_comment, GTK_STATE_ACTIVE, g_white);


	g_signal_connect (G_OBJECT (m_treeview_item_gyn_comment), "button-press-event", G_CALLBACK (HandleTreeBtnPress), NULL);


    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_gyn_comment), m_treeview_item_gyn_comment);


    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_gyn_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    return  scrolledwindow_item_gyn_comment;


}
GtkWidget * MenuNote::create_sp_item()
{
  scrolledwindow_item_sp_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_sp_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_item_sp_comment), GTK_SHADOW_IN);
#ifdef VET
  m_treeview_item_sp_comment =  create_item_list(5, "Small Part");
#else
   m_treeview_item_sp_comment =  create_item_list(6, "Small Part");
#endif
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeview_item_sp_comment), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(m_treeview_item_sp_comment), TRUE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeview_item_sp_comment), FALSE);
	gtk_widget_modify_base (m_treeview_item_sp_comment, GTK_STATE_NORMAL, g_deep);
	gtk_widget_modify_base (m_treeview_item_sp_comment, GTK_STATE_INSENSITIVE, g_deep);
	gtk_widget_modify_text (m_treeview_item_sp_comment, GTK_STATE_INSENSITIVE, g_white);
	gtk_widget_modify_text (m_treeview_item_sp_comment, GTK_STATE_ACTIVE, g_white);


	g_signal_connect (G_OBJECT (m_treeview_item_sp_comment), "button-press-event", G_CALLBACK (HandleTreeBtnPress), NULL);


    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_sp_comment), m_treeview_item_sp_comment);

    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_sp_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    return  scrolledwindow_item_sp_comment;


}

#ifdef EMP_322
GtkWidget * MenuNote::create_ortho_item()
{
  scrolledwindow_item_ortho_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_ortho_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_item_ortho_comment), GTK_SHADOW_IN);
     m_treeview_item_ortho_comment = create_item_list(7, "Orthopedic");

	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeview_item_ortho_comment), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(m_treeview_item_ortho_comment), TRUE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeview_item_ortho_comment), FALSE);
	gtk_widget_modify_base (m_treeview_item_ortho_comment, GTK_STATE_NORMAL, g_deep);
	gtk_widget_modify_base (m_treeview_item_ortho_comment, GTK_STATE_INSENSITIVE, g_deep);
	gtk_widget_modify_text (m_treeview_item_ortho_comment, GTK_STATE_INSENSITIVE, g_white);
	gtk_widget_modify_text (m_treeview_item_ortho_comment, GTK_STATE_ACTIVE, g_white);


	g_signal_connect (G_OBJECT (m_treeview_item_ortho_comment), "button-press-event", G_CALLBACK (HandleTreeBtnPress), NULL);


    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_ortho_comment), m_treeview_item_ortho_comment);


    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_ortho_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    return  scrolledwindow_item_ortho_comment;


}

#else
GtkWidget * MenuNote::create_vs_item()
{
  scrolledwindow_item_vs_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_vs_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_item_vs_comment), GTK_SHADOW_IN);
#ifdef VET
    m_treeview_item_vs_comment = create_item_list(6, "Vascular");
#else
    m_treeview_item_vs_comment = create_item_list(7, "Vascular");
#endif
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeview_item_vs_comment), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(m_treeview_item_vs_comment), TRUE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeview_item_vs_comment), FALSE);
	gtk_widget_modify_base (m_treeview_item_vs_comment, GTK_STATE_NORMAL, g_deep);
	gtk_widget_modify_base (m_treeview_item_vs_comment, GTK_STATE_INSENSITIVE, g_deep);
	gtk_widget_modify_text (m_treeview_item_vs_comment, GTK_STATE_INSENSITIVE, g_white);
	gtk_widget_modify_text (m_treeview_item_vs_comment, GTK_STATE_ACTIVE, g_white);


	g_signal_connect (G_OBJECT (m_treeview_item_vs_comment), "button-press-event", G_CALLBACK (HandleTreeBtnPress), NULL);

    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_vs_comment), m_treeview_item_vs_comment);


    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_vs_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    return  scrolledwindow_item_vs_comment;


}
GtkWidget * MenuNote::create_ortho_item()
{
  scrolledwindow_item_ortho_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_ortho_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_item_ortho_comment), GTK_SHADOW_IN);
#ifdef VET
     m_treeview_item_ortho_comment = create_item_list(7, "Orthopedic");
#else
     m_treeview_item_ortho_comment = create_item_list(8, "Orthopedic");
#endif
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeview_item_ortho_comment), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(m_treeview_item_ortho_comment), TRUE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeview_item_ortho_comment), FALSE);
	gtk_widget_modify_base (m_treeview_item_ortho_comment, GTK_STATE_NORMAL, g_deep);
	gtk_widget_modify_base (m_treeview_item_ortho_comment, GTK_STATE_INSENSITIVE, g_deep);
	gtk_widget_modify_text (m_treeview_item_ortho_comment, GTK_STATE_INSENSITIVE, g_white);
	gtk_widget_modify_text (m_treeview_item_ortho_comment, GTK_STATE_ACTIVE, g_white);


	g_signal_connect (G_OBJECT (m_treeview_item_ortho_comment), "button-press-event", G_CALLBACK (HandleTreeBtnPress), NULL);


    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_ortho_comment), m_treeview_item_ortho_comment);


    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_ortho_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    return  scrolledwindow_item_ortho_comment;


}
#endif

#ifdef VET
GtkWidget * MenuNote::create_tendon_item()
{
  scrolledwindow_item_tendon_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_tendon_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_item_tendon_comment), GTK_SHADOW_IN);
    m_treeview_item_tendon_comment = create_item_list(8, "Tendon");
    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeview_item_tendon_comment), FALSE);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(m_treeview_item_tendon_comment), TRUE);
	gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeview_item_tendon_comment), FALSE);
	gtk_widget_modify_base (m_treeview_item_tendon_comment, GTK_STATE_NORMAL, g_deep);
	gtk_widget_modify_base (m_treeview_item_tendon_comment, GTK_STATE_INSENSITIVE, g_deep);
	gtk_widget_modify_text (m_treeview_item_tendon_comment, GTK_STATE_INSENSITIVE, g_white);
	gtk_widget_modify_text (m_treeview_item_tendon_comment, GTK_STATE_ACTIVE, g_white);


	g_signal_connect (G_OBJECT (m_treeview_item_tendon_comment), "button-press-event", G_CALLBACK (HandleTreeBtnPress), NULL);


    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_tendon_comment), m_treeview_item_tendon_comment);


    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_tendon_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    return  scrolledwindow_item_tendon_comment;


}
#endif
void MenuNote::add_columns_comment(GtkTreeView *treeview)
{
	gint col_offset;
	GtkTreeViewColumn *column;

	m_cellrenderer_comment_text = gtk_cell_renderer_text_new();
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
			-1, "", m_cellrenderer_comment_text, "text", 0,NULL);
	column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
	gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);

}


GtkTreeModel* MenuNote::CreateDeptModel(void)
{

     char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;

    string current_item;
    current_item = exam.ReadDefaultProbeDefaultItemName(&ini);
    GtkListStore *liststore;
    GtkTreeIter iter;
    liststore = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _(current_item.c_str()), -1);//probe_exam_string.c_str(), -1);
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _("Abdomen"), -1);
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _("Urology"), -1);
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _("Cardiac"), -1);
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _("Obstetrics"), -1);
#ifndef VET
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _("Gynecology"), -1);
#endif
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _("Small Part"), -1);
#ifndef EMP_322
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _("Vascular"), -1);
#endif
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _("Orthopedic"), -1);
#ifdef VET
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, _("Tendon"), -1);

#endif

    return GTK_TREE_MODEL(liststore);
}

void MenuNote::BtnComboDeptChanged(GtkComboBox *widget)
{
    int index = gtk_combo_box_get_active(widget);
    char department[256];
    if(index ==0)
    {
        char path[256];
        sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini(path);
        string current_item;
        string current_probe;
        char current_exam[256];
        ExamItem exam;
        current_item = exam.ReadDefaultProbeDefaultItemName(&ini);
        current_probe = exam.ReadDefaultProbe(&ini);
        exam.TransItemNameEng(current_item.c_str(), current_exam);

        strcpy(department,(current_probe + "-" + current_exam).c_str());
    }
    else
    {
    DepartmentName(department, index);

    }

    	HideMenu();
    ShowMenu(index, department);
}

void MenuNote::HideMenu(void)
{
    gtk_widget_hide(scrolledwindow_item_current_comment);
    gtk_widget_hide(scrolledwindow_item_abdo_comment);
    gtk_widget_hide(scrolledwindow_item_uro_comment);
    gtk_widget_hide(scrolledwindow_item_car_comment);
    gtk_widget_hide(scrolledwindow_item_ob_comment);
    gtk_widget_hide(scrolledwindow_item_gyn_comment);
    gtk_widget_hide(scrolledwindow_item_sp_comment);
#ifndef EMP_322
    gtk_widget_hide(scrolledwindow_item_vs_comment);
#endif
    gtk_widget_hide(scrolledwindow_item_ortho_comment);
#ifdef VET
    gtk_widget_hide(scrolledwindow_item_tendon_comment);

#endif

}
GtkTreeModel* MenuNote::create_item_note_model(int index, char *department)
{
    char path[256];
    if (index ==0)
    {
        char path1[256];
        sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path1);
        ExamItem exam;
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);
        if(strcmp(username.c_str(), "System Default") == 0)
        {
            sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
        }
        else
        {
            sprintf(path, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
        }
    }
    else
    {
        sprintf(path, "%s%s", CFG_RES_PATH, NOTE_FILE);
    }
    if(!g_file_test(path, G_FILE_TEST_EXISTS))
    {
        FILE *file = fopen(path, "w+");
        if(file == NULL)
            PRINTF("open %s file is error\n", path);
        return NULL;
    }

    IniFile ini(path);
    IniFile *ptrIni= &ini;
    int number;
    number = ptrIni->ReadInt(department, "Number");
    vector<ExamPara> vecItemComment;
    vecItemComment.clear();
    for(int i=1;i<=number;i++)
    {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i);

        ExamPara exampara;
        exampara.dept_name="";
        exampara.name = ptrIni->ReadString(department, NoteNumber);
        exampara.index=ExamItem::USERNAME;
        vecItemComment.push_back(exampara);
    }
    int item_size(0);
    item_size = vecItemComment.size();

    if(vecItemComment.empty())
        return NULL;

    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(N_COLUMNS,
            G_TYPE_STRING,
            G_TYPE_INT);

    vector<ExamPara>::iterator iterItem;
    for (iterItem = vecItemComment.begin(); iterItem != vecItemComment.end(); iterItem++) {


        gtk_tree_store_append(store, &iter, NULL);
        if(index == 0)
        {
            gtk_tree_store_set(store, &iter,
                    NAME_COLUMN, iterItem->name.c_str(),
                    INDEX_COLUMN, iterItem->index,
                    -1);
        }
        else
        {
            gtk_tree_store_set(store, &iter,
                    NAME_COLUMN, _(iterItem->name.c_str()),
                    INDEX_COLUMN, iterItem->index,
                    -1);
        }


    }

    return GTK_TREE_MODEL(store);
}

void MenuNote::DepartmentName(char department[256], int index)
{
    if(index == 1)
    {
        strcpy(department, "Abdomen");
    }
    else if(index == 2)
    {
        strcpy(department, "Urology");
    }
    else if(index == 3)
    {
        strcpy(department, "Cardiac");
    }
    else if(index == 4)
    {
        strcpy(department, "Obstetrics");
    }
#ifdef EMP_322
 else if(index == 5)
    {
        strcpy(department, "Gynecology");
    }
    else if(index == 6)
    {
        strcpy(department, "Small Part");
    }
       else
    {
        strcpy(department, "Orthopedic");
    }


#else
#ifdef VET

    else if(index == 5)
    {
        strcpy(department, "Small Part");
    }
    else if(index == 6)
    {
        strcpy(department, "Vascular");
    }
    else if(index ==7)
    {
        strcpy(department, "Orthopedic");
    }
    else
    {
        strcpy(department, "Tendon");
    }

#else
    else if(index == 5)
    {
        strcpy(department, "Gynecology");
    }
    else if(index == 6)
    {
        strcpy(department, "Small Part");
    }
    else if(index == 7)
    {
        strcpy(department, "Vascular");
    }
    else
    {
        strcpy(department, "Orthopedic");
    }
#endif
#endif

}

void MenuNote::Focus(void)
{
    gtk_widget_set_sensitive(m_treeview_item_current_comment, TRUE);
    gtk_widget_set_sensitive(m_treeview_item_abdo_comment, TRUE);
    gtk_widget_set_sensitive(m_treeview_item_uro_comment, TRUE);
    gtk_widget_set_sensitive(m_treeview_item_car_comment, TRUE);
    gtk_widget_set_sensitive(m_treeview_item_ob_comment, TRUE);
    gtk_widget_set_sensitive(m_treeview_item_sp_comment, TRUE);
#ifdef VET
  gtk_widget_set_sensitive(m_treeview_item_tendon_comment, TRUE);
#else
    gtk_widget_set_sensitive(m_treeview_item_gyn_comment, TRUE);
#endif
    gtk_widget_set_sensitive(m_treeview_item_ortho_comment, TRUE);
#ifndef EMP_322
    gtk_widget_set_sensitive(m_treeview_item_vs_comment, TRUE);
#endif

    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboDept));
    if(index == 0)
    {
        gtk_widget_grab_focus(m_treeview_item_current_comment);
    }
   else if(index == 1)
    {
        gtk_widget_grab_focus(m_treeview_item_abdo_comment);
    }
   else if(index == 2)
    {
        gtk_widget_grab_focus(m_treeview_item_uro_comment);
    }
    else if(index == 3)
    {
        gtk_widget_grab_focus(m_treeview_item_car_comment);
    }
    else if(index == 4)
    {
        gtk_widget_grab_focus(m_treeview_item_ob_comment);
    }
#ifdef VET
  else if(index == 5)
    {
        gtk_widget_grab_focus(m_treeview_item_sp_comment);
    }

#else
    else if(index == 5)
    {
        gtk_widget_grab_focus(m_treeview_item_gyn_comment);
    }
    else if(index == 6)
    {
        gtk_widget_grab_focus(m_treeview_item_sp_comment);
    }
#endif
#ifdef EMP_322
      else if(index == 7)
    {
        gtk_widget_grab_focus(m_treeview_item_ortho_comment);
    }

#else
#ifdef VET
    else if(index == 6)
    {
        gtk_widget_grab_focus(m_treeview_item_vs_comment);
    }
    else if(index == 7)
    {
        gtk_widget_grab_focus(m_treeview_item_ortho_comment);
    }
    else
    {
     gtk_widget_grab_focus(m_treeview_item_tendon_comment);
    }

#else
    else if(index == 7)
    {
        gtk_widget_grab_focus(m_treeview_item_vs_comment);
    }
    else
    {
        gtk_widget_grab_focus(m_treeview_item_ortho_comment);
    }
#endif
#endif
}


void MenuNote::TreeBtnPress(GtkWidget *widget, GdkEventButton *event)
{


	GtkTreeView *tree = GTK_TREE_VIEW(widget);
	if (event->window == gtk_tree_view_get_bin_window (tree) && event->type == GDK_BUTTON_PRESS && event->button == 1)
	{
		int x, y;
		gtk_tree_view_convert_widget_to_bin_window_coords (tree, event->x, event->y, &x, &y);
	//	PRINTF("Button preess at: (%f, %f) for widget, (%d, %d) for bin_window\n", event->x, event->y, x, y);

		GtkTreeIter iter;
		GtkTreePath *path;
		GtkTreeModel *model = gtk_tree_view_get_model (tree);
		if (gtk_tree_view_get_path_at_pos (tree, x, y, &path, NULL, NULL, NULL))
		{
		//	PRINTF("Row exist: path is %s\n", gtk_tree_path_to_string(path));
			if (gtk_tree_model_get_iter (model, &iter, path))
			{
				gchar *text;
				gtk_tree_model_get (model, &iter, NAME_COLUMN, &text, -1);
				NoteArea::GetInstance()->Focus();
			//	gtk_widget_set_sensitive(widget, FALSE);
				NoteArea::GetInstance()->SetNewText(text);
				g_free (text);
			}
			gtk_tree_path_free (path);
		}
	}
}