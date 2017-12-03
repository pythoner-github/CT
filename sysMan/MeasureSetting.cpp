/*
 * =====================================================================================
 *
 *       Filename:  MeasureSetting.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18/4/2016 04:09:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhang jingxia 
 *        Company:  
 *
 * =====================================================================================
 */

#include <dirent.h>
#include "MeasureSetting.h"
#include "Def.h"
#include "gui_global.h"
#include "gui_func.h"
#include "KeyValueOpr.h"
#include "KeyDef.h"
#include "MeasureDef.h"
#include "MeaCalcFun.h"
#include "ViewMain.h"
#include "ViewDialog.h"
#include "KeyFunc.h"
#include "FileMan.h"
#include "PeripheralMan.h"
#include "MenuMeasure.h"
#include "ConfigToHost.h"
#include "UserSelect.h"
#include "TopArea.h"
#include "CalcSetting.h"
using std::vector;
#ifdef EMP_322
extern const char *examType_calc[];
#endif
//MeasureSetting g_menuCalcExamType;
MeasureSetting* MeasureSetting::m_ptrInstance = NULL;

MeasureSetting::MeasureSetting()
{
}

MeasureSetting::~MeasureSetting()
{
   if (m_ptrInstance != NULL)
      delete m_ptrInstance;

}

MeasureSetting* MeasureSetting::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new MeasureSetting;

    return m_ptrInstance;
}

GtkWidget* MeasureSetting::GetWindow(void)
{
    return m_win_parent;
}


void MeasureSetting::CreateDefineItem_calc(vector<string>& vecExamItem_calc)
{  

    char probelist[256];
    char useritem[256];
    char department[256];
    char firstgenitem[256];
    char src_group[256];
    char userselect[256];
    char path_userselect[256];
    sprintf(path_userselect, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini_userselect(path_userselect);
    ExamItem examitem;
    vector<string> useritemgroup;
    useritemgroup = examitem.GetDefaultUserGroup();

    int group_length(0);
    group_length = useritemgroup.size();
    for (int i= 0 ; i <  group_length; i++)
	{
		if (useritemgroup[i].length() != 0)
		{
			sprintf(src_group ,"%s", useritemgroup[i].c_str());
			examitem.GetUserItem(src_group, userselect, probelist, useritem, department, firstgenitem);
			string username=useritem;
			vecExamItem_calc.push_back(username);
		}
    }
}

void MeasureSetting::CreateItemListForMeasure(vector<string>& vecItemCalc)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
         sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_ITEM_PATH);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_ITEM_FILE, username.c_str(), ".ini");
    }

    IniFile ini(path);
    IniFile *ptrIni= &ini;
    int number;
    number = ptrIni->ReadInt("Measure", "Number");
    for(int i=1;i<=number;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt("Measure", CalcNumber);
        string item_name;
        item_name= ItemMenuTransEnglish(item_num);
        vecItemCalc.push_back(item_name.c_str());
    }
}



int MeasureSetting::ItemNameTransEtype(char *select_name)
{
    int select_num = -1;
    for(int i=0; i < ABD_MEA_START;i++)
    {
        if(strcmp(select_name, _(BasicInfo[i].title))==0 )
        {
            select_num = BasicInfo[i].item;
            return select_num;
        }
    }
   
	return select_num;
}

string MeasureSetting::ItemMenuTransEnglish(int item_num)
{
    string item_name;
    if((item_num <ABD_MEA_START)&&(item_num >=0))
    {
        for(int i=0; i < ABD_MEA_START; i++)
        {
            if(item_num == BasicInfo[i].item)
            {
                item_name = BasicInfo[i].title;
            }
        }
    }
    return item_name;
}

void MeasureSetting::CreateItemListForMeasureed(char *probe_exam,vector<string>& vecItemCalc1)
{   
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    int number;
    number = ptrIni->ReadInt(probe_exam, "Number");

    if(number ==0)
        return;

    for(int i=1;i<=number;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(probe_exam, CalcNumber);
        string item_name;
        item_name= ItemMenuTransEnglish(item_num);
        vecItemCalc1.push_back(item_name);
    }
}

void MeasureSetting::CreateItemList_Calc2(char *probe_exam,vector<int>& vecItemCalc1)
{   
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    int number;
    number = ptrIni->ReadInt(probe_exam, "Number");

    if(number ==0)
        return;

    for(int i=1;i<=number;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(probe_exam, CalcNumber);
        vecItemCalc1.push_back(item_num);
    }
}

GtkTreeModel* MeasureSetting::create_item_calc_model1()
{
    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));

    if(!exam_type_name)
        return NULL;
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
	int sequence = GetSequence(exam_type);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sequence_calc), sequence);

    vector<string> vecItemCalc1;
    vecItemCalc1.clear();
    CreateItemListForMeasureed(exam_type, vecItemCalc1);
    int item_size(0);
    item_size = vecItemCalc1.size();

    if(vecItemCalc1.empty())
        return NULL;

    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(1,
            G_TYPE_STRING);

    for (int i = 0; i<item_size; i++) 
    {
        gtk_tree_store_append(store, &iter, NULL);
        gtk_tree_store_set(store, &iter, 
                0,  _(vecItemCalc1[i].c_str()),
                -1);
next:
        continue;
    }
    return GTK_TREE_MODEL(store);
}


GtkTreeModel* MeasureSetting::create_item_calc_model2()
{
    
    vector<string> vecItemCalc;
    vecItemCalc.clear();
    CreateItemListForMeasure(vecItemCalc);
    int item_size(0);
    item_size = vecItemCalc.size();

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));

    if(!exam_type_name)
        return NULL;
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);

    vector<string> vecItemCalc1;
    vecItemCalc1.clear();
    CreateItemListForMeasureed(exam_type, vecItemCalc1);
    int item_size1(0);
    item_size1 = vecItemCalc1.size();
    int pos_num=0;
    if(!vecItemCalc1.empty())
    {

        for (int i = 0; i<item_size1; i++) 
        {
            vector<string>::iterator pos;
            pos = find(vecItemCalc.begin(), vecItemCalc.end(), vecItemCalc1[i]);
          
            if(pos != vecItemCalc.end())
            {
                vecItemCalc.erase(pos);
                pos_num++;
            } else if(pos == vecItemCalc.end())
            {    //vecItemCalc.pop_back();
               // pos_num++;
            }
        }  
    } 
    if(vecItemCalc.empty())
        return NULL;

    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(1,
            G_TYPE_STRING);

    for (int i = 0; i<(item_size-pos_num); i++) 
    {
        gtk_tree_store_append(store, &iter, NULL);
        gtk_tree_store_set(store, &iter, 
                0,  _(vecItemCalc[i].c_str()),
                -1);
next:
        continue;
    }

    return GTK_TREE_MODEL(store);
}


void MeasureSetting::CreateItemListForDeleteMeasureed(char *select_name, string probe_exam,vector<int>& vecItemCalc1)
{   

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    const char *probeExam = probe_exam.c_str();

    int number;
    number = ptrIni->ReadInt(probeExam, "Number");

    if(number ==0)
        return;

    for(int i=1;i<=number;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);

        int item_num = ptrIni->ReadInt(probeExam, CalcNumber);
        string item_name;
        item_name= ItemMenuTransEnglish(item_num);
        if(strcmp(select_name, _(item_name.c_str()))!=0)
        {
            vecItemCalc1.push_back(item_num);
        }
    }
}

void MeasureSetting::add_columns_calc(GtkTreeView *treeview)
{
    gint col_offset;
    GtkTreeViewColumn *column;

    m_cellrenderer_calc_text = gtk_cell_renderer_text_new();
    col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
            -1, "", m_cellrenderer_calc_text, "text", 0,NULL);
    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
}
void MeasureSetting::add_columns_calc1(GtkTreeView *treeview)
{
    gint col_offset;
    GtkTreeViewColumn *column;

    m_cellrenderer_calc_text1 = gtk_cell_renderer_text_new();
    col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
            -1, "", m_cellrenderer_calc_text1, "text", 0,NULL);
    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
}


void MeasureSetting::ExamCalcChanged(GtkComboBox *widget)
{
    ChangeModel2();

    // update measur sequence
    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));
    if(!exam_type_name)
        return;
   
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    int sequence = GetSequence(exam_type);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sequence_calc), sequence);   
}

void MeasureSetting::MeasureSequenceChanged(GtkComboBox *widget)
{
    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));
    if(!exam_type_name)
        return;
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    
    int number = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_sequence_calc));
    
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path1);
    IniFile *ptrIni = &ini; 
    ptrIni->WriteInt(exam_type, "Sequence", number);
    ptrIni->SyncConfigFile();
}


void MeasureSetting::ChangeModel2(void)
{
    GtkTreeModel *model1 = create_item_calc_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc1), model1);

    GtkTreeModel *model = create_item_calc_model2();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc), model);

}


void MeasureSetting::ChangeModel(void)
{
    GtkTreeModel *model = create_item_calc_model2();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc), model);

}

void MeasureSetting::ChangeExamBox(char *check_part)
{
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_calc), check_part);
}

void MeasureSetting::ChangeExamBoxDelete(void)
{
    vector<string> vecExamItem_calc;
    vecExamItem_calc.clear();
    //系统默认的检查部位
    for (int i=0; i<= EXAM_NUM; i++)
    {
#ifdef EMP_322
        vecExamItem_calc.push_back(examType_calc[i]);
#else
        vecExamItem_calc.push_back(examType[i]);
#endif
    }
    //用户自定义的检查部位
    CreateDefineItem_calc(vecExamItem_calc);
    int exam_size(0);
    exam_size = vecExamItem_calc.size();
    for(int i = exam_size; i >= 0; i--)
    {
        gtk_combo_box_remove_text(GTK_COMBO_BOX (m_combobox_exam_calc), i);
    }

    for (int i = 0; i <exam_size; i++) 
    {
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_calc), _(vecExamItem_calc[i].c_str()));
    }
}

void MeasureSetting::ChangeExamBoxToDefault(void)
{
    //gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_exam_calc), 0);
}

void MeasureSetting::ChangeModelAndLight(const char *name)
{
    GtkTreeModel *model = create_item_calc_model2();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc), model);

    GtkTreeSelection *selection;
    GtkTreeIter iter_tmp;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc)); 
    iter_tmp= InsertUniqueCalc(model, name);
    gtk_tree_selection_select_iter(selection, &iter_tmp);

    GtkTreePath *path_scroll = gtk_tree_model_get_path(model, &iter_tmp);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_calc), path_scroll, NULL, FALSE, 1.0, 1.0);
    gtk_tree_path_free (path_scroll);
}


GtkWidget* MeasureSetting::CreateMeasureWindow(GtkWidget *parent)
{
    m_win_parent = parent;

    fixed_calc = gtk_fixed_new ();
    gtk_widget_show (fixed_calc);
    
    GtkWidget *label_exam_calc = gtk_label_new (_("Exam Type:"));
    gtk_misc_set_alignment (GTK_MISC(label_exam_calc), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_exam_calc), TRUE);
    gtk_widget_show (label_exam_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), label_exam_calc, 10, 10);//90-15, 15);
    gtk_widget_set_size_request (label_exam_calc, 100+17, 30);

    m_combobox_exam_calc = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_exam_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_combobox_exam_calc, 140, 10);
    gtk_widget_set_size_request (m_combobox_exam_calc, 180, 30);
    vector<string> vecExamItem_calc;
    vecExamItem_calc.clear();
   //系统默认的检查部位
    for (int i=0; i<= EXAM_NUM; i++)
    {
#ifdef EMP_322
        vecExamItem_calc.push_back(examType_calc[i]);
#else
        vecExamItem_calc.push_back(examType[i]);
#endif
    }
    //用户自定义的检查部位
    CreateDefineItem_calc(vecExamItem_calc);
    int exam_size(0);
    exam_size = vecExamItem_calc.size();

    for (int i = 0; i <exam_size; i++) 
	{
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_calc), _(vecExamItem_calc[i].c_str()));
    }
    for (int i = 0; i <exam_size; i++) 
	{
		string curExamType;
		TopArea::GetInstance()->GetCheckPart(curExamType);
		if (strcmp(curExamType.c_str(), _(vecExamItem_calc[i].c_str())) == 0)
		{
			gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_exam_calc), i);
			break;
        }
    }
    g_signal_connect(m_combobox_exam_calc, "changed", G_CALLBACK(HandleExamCalcChanged), this);

    GtkWidget *label_department_calc = gtk_label_new (_("Available Items:"));
    gtk_misc_set_alignment (GTK_MISC(label_department_calc), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_department_calc), TRUE);
    gtk_widget_show (label_department_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), label_department_calc, 10, 35);
    gtk_widget_set_size_request (label_department_calc, 150, 30);


    scrolledwindow_item_calc = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_item_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), scrolledwindow_item_calc, 15, 80);
    gtk_widget_set_size_request (scrolledwindow_item_calc, 240, 340);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_calc), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

     GtkTreeModel *model = create_item_calc_model2();
      m_treeview_item_calc = gtk_tree_view_new_with_model(model);


    add_columns_calc(GTK_TREE_VIEW(m_treeview_item_calc));

    gtk_widget_modify_base(m_treeview_item_calc, GTK_STATE_NORMAL, g_deep);

    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_calc), m_treeview_item_calc);

    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);
    gtk_widget_show (m_treeview_item_calc);


     GtkWidget *label_sequence_calc = gtk_label_new (_("Measure Sequence:"));
    gtk_misc_set_alignment (GTK_MISC(label_sequence_calc), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_sequence_calc), TRUE);
    gtk_widget_show (label_sequence_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), label_sequence_calc, 400, 10);
    gtk_widget_set_size_request (label_sequence_calc, 110+15+10, 30);

    m_combobox_sequence_calc = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_sequence_calc);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_combobox_sequence_calc, 540, 15);
    gtk_widget_set_size_request (m_combobox_sequence_calc, 180-75+10, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_sequence_calc), _("None"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_sequence_calc), _("Repeat"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_sequence_calc), _("Next"));
	//////////////////////////////
    ExamItem exam;
	char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path11);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
	string examType = exam.ReadDefaultProbeDefaultItemName(&ini1);
	char exam_type[256];
    exam.TransItemNameEng(examType.c_str(), exam_type);
    int sequence = GetSequence(exam_type);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sequence_calc), sequence);
    g_signal_connect(m_combobox_sequence_calc, "changed", G_CALLBACK(HandleMeasureSequenceChanged), this);


    GtkWidget *label_department_calc_select = gtk_label_new (_("Selected Items:"));
    gtk_misc_set_alignment (GTK_MISC(label_department_calc_select), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_department_calc_select), TRUE);
    gtk_widget_show (label_department_calc_select);
    gtk_fixed_put (GTK_FIXED (fixed_calc), label_department_calc_select, 400, 40);
    gtk_widget_set_size_request (label_department_calc_select, 150, 30);


    scrolledwindow_item_calc1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_item_calc1);
    gtk_fixed_put (GTK_FIXED (fixed_calc), scrolledwindow_item_calc1, 400, 80);
    gtk_widget_set_size_request (scrolledwindow_item_calc1, 140+100+15, 340);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_calc1), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    GtkTreeModel *model1 = create_item_calc_model1();
    m_treeview_item_calc1 = gtk_tree_view_new_with_model(model1);
    add_columns_calc1(GTK_TREE_VIEW(m_treeview_item_calc1));

    gtk_widget_modify_base(m_treeview_item_calc1, GTK_STATE_NORMAL, g_deep);

    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_calc1), m_treeview_item_calc1);

    GtkTreeSelection *select1;
    select1 = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc1));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);
    gtk_widget_show (m_treeview_item_calc1);


    m_button_select_one = gtk_button_new_with_mnemonic (">");
    gtk_widget_show (m_button_select_one);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_select_one, 290, 170);
    gtk_widget_set_size_request (m_button_select_one, 80, 30);
    g_signal_connect(m_button_select_one, "clicked", G_CALLBACK(HandleButtonSelectOneCalcClicked), this);

    m_button_select_all = gtk_button_new_with_mnemonic (">>");
    gtk_widget_show (m_button_select_all);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_select_all, 290, 220);
    gtk_widget_set_size_request (m_button_select_all, 80, 30);
    g_signal_connect(m_button_select_all, "clicked", G_CALLBACK(HandleButtonSelectAllCalcClicked), this);


    m_button_calc_delete = gtk_button_new_with_mnemonic ("<");
    gtk_widget_show (m_button_calc_delete);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_delete, 290, 270);
    gtk_widget_set_size_request (m_button_calc_delete, 80, 30);
    g_signal_connect(m_button_calc_delete, "clicked", G_CALLBACK(HandleButtonBackOneClicked), this);

    m_button_calc_delete_all = gtk_button_new_with_mnemonic ("<<");
    gtk_widget_show (m_button_calc_delete_all);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_delete_all,  290, 320);
    gtk_widget_set_size_request (m_button_calc_delete_all, 80, 30);
    g_signal_connect(m_button_calc_delete_all, "clicked", G_CALLBACK(HandleButtonBackAllClicked), this);

   
    m_button_calc_up = gtk_button_new_with_mnemonic (_("Up"));
    gtk_widget_show (m_button_calc_up);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_up, 680, 220);
    gtk_widget_set_size_request (m_button_calc_up, 80, 30);
    g_signal_connect(m_button_calc_up, "clicked", G_CALLBACK(HandleButtonUpClicked), this);

    m_button_calc_down = gtk_button_new_with_mnemonic (_("Down"));
    gtk_widget_show (m_button_calc_down);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_down, 680, 270);
    gtk_widget_set_size_request (m_button_calc_down, 80, 30);
    g_signal_connect(m_button_calc_down, "clicked", G_CALLBACK(HandleButtonDownClicked), this);


    m_button_calc_default = gtk_button_new_with_mnemonic (_("Default Factory"));
    gtk_widget_show (m_button_calc_default);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_button_calc_default, 540, 430);
    gtk_widget_set_size_request (m_button_calc_default, 160, 30);
    g_signal_connect(m_button_calc_default, "clicked", G_CALLBACK(HandleButtonDefaultClicked), this);

    return fixed_calc;
}

const gchar* MeasureSetting::GetExamName(void)
{
    return gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));
}

int MeasureSetting::GetSequence(const char *exam_type)
{
	ExamItem exam;
	char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path11);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
	string examType = exam.ReadDefaultProbeDefaultItemName(&ini1);
	char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path1);
    IniFile *ptrIni_calc = &ini;
	int squence = ptrIni_calc->ReadInt(exam_type, "Sequence");
	return squence;
}

GtkTreeIter MeasureSetting::InsertUniqueCalc(GtkTreeModel *model, const char *str)
{
    GtkTreeIter tmp_iter;
    char *strtmp = NULL;
    gboolean has_node = gtk_tree_model_get_iter_first(model, &tmp_iter);
    while (has_node == TRUE) 
    {
        gtk_tree_model_get(model, &tmp_iter, 0, &strtmp, -1);
        if (strcmp(strtmp, str) == 0)
            return tmp_iter;
        else
            has_node = gtk_tree_model_iter_next(model, &tmp_iter);
    }
    return tmp_iter;
}


void MeasureSetting::ButtonSelectOneCalcClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc)); 
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::ERROR, 
                _("Please select a item before move!"), NULL); 
        return;
    }

    char* select_name;
    gtk_tree_model_get(model, &iter, 0, &select_name, -1);
    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));

    if(!exam_type_name)
        return;
    char exam_type[256];
	ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    vector<string> vecItem_Calc;
    vecItem_Calc.clear();
    CreateItemListForMeasureed(exam_type, vecItem_Calc);

    int item_length(0);
    item_length = vecItem_Calc.size();
    if(!vecItem_Calc.empty())
    {
        for(int i=0; i<item_length; i++)
        {
            if( strcmp(select_name, _(vecItem_Calc[i].c_str())) == 0)
            {
                GtkTreeModel *model_tmp;
                GtkTreeSelection *selection;
                GtkTreeIter iter_tmp;
                selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc1)); 
                model_tmp= gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc1));
                 //高亮要插入的词条，并更新滚动条
                iter_tmp= InsertUniqueCalc(model_tmp, select_name);
                gtk_tree_selection_select_iter(selection, &iter_tmp);
            
                GtkTreePath *path_scroll = gtk_tree_model_get_path(model_tmp, &iter_tmp);
                gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_calc1), path_scroll, NULL, FALSE, 1.0, 1.0);
                gtk_tree_path_free (path_scroll);
                
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::INFO, _(" Item has existed. Please select item again!"), NULL);
                return;
            }
        }
    }
  
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path1);
    IniFile *ptrIni = &ini; 
    int number;
    char SelectNum[256];
    number = ptrIni->ReadInt(exam_type, "Number");
    
    sprintf(SelectNum, "Calc%d",number+1);
    int select_num = ItemNameTransEtype(select_name);
	if (select_num == -1)
	{
		PRINTF("Fail to ItemNameTransEtype, not exist the etype!\n");
		return;
	}
   
    ptrIni->WriteInt(exam_type, SelectNum, select_num);
    ptrIni->WriteInt(exam_type, "Number", number+1);
    ptrIni->SyncConfigFile();

    GtkTreeModel *new_model1 = create_item_calc_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc1), new_model1);
    //高亮插入的词条，并更新滚动条
    GtkTreeSelection *new_selection;
    GtkTreeIter iter_new;
    new_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc1)); 
    iter_new= InsertUniqueCalc(new_model1, select_name);
    gtk_tree_selection_select_iter(new_selection, &iter_new);

    GtkTreePath *path_scroll = gtk_tree_model_get_path(new_model1, &iter_new);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_calc1), path_scroll, NULL, TRUE, 1.0, 1.0);
    gtk_tree_path_free (path_scroll);

    //更新departent列表
    GtkTreeStore *calculate_store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc)));
    gtk_tree_store_remove(calculate_store, &iter);

    if (g_menuMeasure.GetExamItem() == exam_type)
          g_menuMeasure.ChangeExamItem(exam_type);
}


void MeasureSetting::ButtonSelectAllCalcClicked(GtkButton *button)
{    
    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));

    if(!exam_type_name)
        return;

	// if m_treeview_item_calc is empty
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc));
	if (gtk_tree_model_iter_n_children(model, NULL) == 0)
	{
		PRINTF("=========== tree view department items is empty!\n");
		return;
	}
    
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
      char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini11(path11);
  
    string username;
    username = exam.ReadDefaultUserSelect(&ini11);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini; 
    
    int Num = ptrIni->ReadInt(exam_type, "Number");
    
    vector<string> vecSelect_Calc;
    vecSelect_Calc.clear();
    CreateItemListForMeasureed(exam_type, vecSelect_Calc);

    int select_length(0);
    select_length = vecSelect_Calc.size();

     char path12[256];
     if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path12, "%s%s", CFG_RES_PATH, MEASURE_ITEM_PATH);
    }
    else
    {
        sprintf(path12, "%s%s%s%s", CFG_RES_PATH, MEASURE_ITEM_FILE, username.c_str(), ".ini");
    }
   

    IniFile new_ini(path12);
    IniFile *new_ptrIni= &new_ini;
    int number = new_ptrIni->ReadInt("Measure", "Number");
    
    for(int i=1;i<=number;i++)
    {
    int writeCalc=1;
    char CalcNumber[256];
    sprintf(CalcNumber, "Calc%d", i);
    int item_num = new_ptrIni->ReadInt("Measure", CalcNumber);
    string item_name;
    item_name= ItemMenuTransEnglish(item_num);
    if(!vecSelect_Calc.empty())
    {
        for(int j=0; j<select_length; j++)
        {
            if(strcmp(_(item_name.c_str()), _(vecSelect_Calc[j].c_str())) ==0)
            {
                writeCalc=0;
            }
            }
        }
        
        if(writeCalc)
        {
            char CalcNum[256];
            sprintf(CalcNum, "Calc%d", ++Num);
            ptrIni->WriteInt(exam_type, CalcNum, item_num);
        }

    }
    ptrIni->WriteInt(exam_type, "Number", Num);
    ptrIni->SyncConfigFile();
    
    //更新Exam Item和 Department 列表
    ChangeModel2();
	
	if (g_menuMeasure.GetExamItem() == exam_type)
		g_menuMeasure.ChangeExamItem(exam_type);
   
}

void MeasureSetting::ButtonBackOneClicked(GtkButton *button)
{
 GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc1)); 
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc1));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::ERROR, 
                _("Please select a item before move!"), NULL); 
        return;
    }
  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);

    char* select_name;
    gtk_tree_model_get(model, &iter, 0, &select_name, -1);

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));

    if(!exam_type_name)
        return;

    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
     char path11[256];
        sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path11);
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);
        char path1[256];
        if(strcmp(username.c_str(), "System Default") == 0)
        {
            sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
        }
        else
        {
            sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
        }
        IniFile ini(path1);
        IniFile *ptrIni_calc = &ini; 

     vector<int> vecDelete_Calc;
    vecDelete_Calc.clear();
    CreateItemListForDeleteMeasureed(select_name, exam_type, vecDelete_Calc); 

    int item_length(0);
    item_length = vecDelete_Calc.size();
	int squence_copy;
	squence_copy= ptrIni_calc->ReadInt(exam_type, "Sequence");
    ptrIni_calc->RemoveGroup(exam_type);
	ptrIni_calc->SyncConfigFile();

    for(int i=0;i<item_length;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i+1);
        ptrIni_calc->WriteInt(exam_type, CalcNumber, vecDelete_Calc[i]);
    }
    
    ptrIni_calc->WriteInt(exam_type, "Number", item_length);
	ptrIni_calc->WriteInt(exam_type, "Sequence", squence_copy);
    ptrIni_calc->SyncConfigFile();

    GtkTreeModel *new_model1 = create_item_calc_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc1), new_model1);
    //设置光标，更新滚动条的值
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_calc1), path, NULL, TRUE);
    gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_calc1)));
    gtk_tree_path_free (path);

    ChangeModel();

	if (g_menuMeasure.GetExamItem() == exam_type)
		g_menuMeasure.ChangeExamItem(exam_type);
}


void MeasureSetting::ButtonBackAllClicked(GtkButton *button)
{

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));
    if(!exam_type_name)
        return;

    char exam_type[256];
	ExamItem exam;
	exam.TransItemName(exam_type_name, exam_type);
	char path[256];
	sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
	IniFile ini1(path);
	string username;
	username = exam.ReadDefaultUserSelect(&ini1);
	char path1[256];
	if(strcmp(username.c_str(), "System Default") == 0)
	{
		sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
	}
	else
	{
		sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
	}
	IniFile ini(path1);
	IniFile *ptrIni = &ini; 

	int squence_copy;
	squence_copy= ptrIni->ReadInt(exam_type, "Sequence");

	ptrIni->RemoveGroup(exam_type);

	ptrIni->WriteInt(exam_type, "Number", 0);
	ptrIni->WriteInt(exam_type, "Sequence", squence_copy);
	ptrIni->SyncConfigFile();

	//更新列表
	ChangeModel2();

	if (g_menuMeasure.GetExamItem() == exam_type)
		g_menuMeasure.ChangeExamItem(exam_type);
}


void MeasureSetting::ButtonDownClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc1)); 
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc1));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::ERROR, 
                _("Please select a item before down!"), NULL); 
        return;
    }
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    char *path_string = gtk_tree_path_to_string(path);
    int path_num = atoi(path_string);

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    vector<int> vecDelete_Calc;
    vecDelete_Calc.clear();
    CreateItemList_Calc2(exam_type, vecDelete_Calc); 

    int item_length(0);
    item_length = vecDelete_Calc.size();


    int path_total = item_length-1;
    if(path_num != path_total)
    {
        char path11[256];
        sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path11);
        ExamItem exam;
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);
        char path1[256];
        if(strcmp(username.c_str(), "System Default") == 0)
        {
            sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
        }
        else
        {
            sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
        }
        IniFile ini(path1);
        IniFile *ptrIni_calc = &ini; 

		int squence_copy;
		squence_copy= ptrIni_calc->ReadInt(exam_type, "Sequence");
		ptrIni_calc->RemoveGroup(exam_type);
        ptrIni_calc->SyncConfigFile();

        swap(vecDelete_Calc[path_num], vecDelete_Calc[path_num+1]);

        for(int i=0;i<item_length;i++)
        {
            char CalcNumber[256];
            sprintf(CalcNumber, "Calc%d", i+1);
            ptrIni_calc->WriteInt(exam_type, CalcNumber, vecDelete_Calc[i]);
        }

        ptrIni_calc->WriteInt(exam_type, "Number", item_length);
        ptrIni_calc->WriteInt(exam_type, "Sequence", squence_copy);
        ptrIni_calc->SyncConfigFile();

        //更新树
        GtkTreeModel *new_model1 = create_item_calc_model1();
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc1), new_model1);

        g_free(path_string);
        gtk_tree_path_next(path);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_calc1), path, NULL, TRUE);
        if(item_length-path_num >2)
            gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_calc1), path, NULL, TRUE, 1.0, 1.0);//移动后的选中词条置底
        else
            gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_calc1)));//保证在最下端的词条移动后能够正常显示
        gtk_tree_path_free (path);
    }

	if (g_menuMeasure.GetExamItem() == exam_type)
		g_menuMeasure.ChangeExamItem(exam_type);
}

void MeasureSetting::ButtonUpClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_calc1)); 
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_calc1));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_win_parent), ViewDialog::ERROR, 
                _("Please select a item before up!"), NULL); //请先选择待插入结点的父结点
        return;
    }
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    char *path_string = gtk_tree_path_to_string(path);
    int path_num = atoi(path_string);
    PRINTF("---path_string:%s %d\n",path_string,path_num);

    if(path_num == 0)
        return;


    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_calc));
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);

    vector<int> vecDelete_Calc;
    vecDelete_Calc.clear();
    CreateItemList_Calc2(exam_type, vecDelete_Calc); 

    int item_length(0);
    item_length = vecDelete_Calc.size();


    char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path11);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path1);
    IniFile *ptrIni_calc = &ini; 

	int squence_copy;
	squence_copy= ptrIni_calc->ReadInt(exam_type, "Sequence");
    ptrIni_calc->RemoveGroup(exam_type);
    ptrIni_calc->SyncConfigFile();

    swap(vecDelete_Calc[path_num], vecDelete_Calc[path_num-1]);

    for(int i=0;i<item_length;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i+1);
        ptrIni_calc->WriteInt(exam_type, CalcNumber, vecDelete_Calc[i]);
    }

    ptrIni_calc->WriteInt(exam_type, "Number", item_length);
	ptrIni_calc->WriteInt(exam_type, "Sequence", squence_copy);
    ptrIni_calc->SyncConfigFile();

    GtkTreeModel *new_model1 = create_item_calc_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_calc1), new_model1);

    g_free(path_string);
    if(gtk_tree_path_prev(path))
    {
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_calc1), path, NULL, TRUE);
        if(item_length-path_num >12)
            gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_calc1), path, NULL, TRUE, 0.0, 1.0);// 移动后选中词条置顶
        else
            gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_calc1)));//保证在最上端的词条移动时能够正确显示
    }
    gtk_tree_path_free (path);

	if (g_menuMeasure.GetExamItem() == exam_type)
		g_menuMeasure.ChangeExamItem(exam_type);
}
 
void MeasureSetting::ButtonDefaultClicked(GtkButton *button)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);

    char path[256];
    char userselectname2[256];
    char path3[256];
    char userselectname3[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
        sprintf(path3, "%s%s", CFG_RES_PATH, MEASURE_ITEM_PATH);

    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
        sprintf(path3, "%s%s%s%s", CFG_RES_PATH, MEASURE_ITEM_FILE, username.c_str(), ".ini");

    }
    FileMan f;

    // 计算预设已选所用
    sprintf(userselectname2, "%s%s", CFG_RES_PATH, DEFAULT_MEASURE_FILE);
    f.CopyFile(userselectname2, path);

    //计算预设词库所用
    sprintf(userselectname3, "%s%s", CFG_RES_PATH, DEFAULT_MEASURE_ITEM_FILE );
    f.CopyFile(userselectname3, path3);

    //更新Exam Item 和Departmen 列表
    ChangeModel2();

	g_menuMeasure.ChangeAllMeasureItems();
}

//获得exam_type检查部位下所有测量项的item，push到vector中
void MeasureSetting::GetMeasureListEtype(char *exam_type, vector<int> & vecItemCalc)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    int number;
    number = ptrIni->ReadInt(exam_type, "Number");

    if(number ==0)
        return;

    for(int i=1;i<=number;i++)
    {
        char CalcNumber[256];
        sprintf(CalcNumber, "Calc%d", i);
        int item_num = ptrIni->ReadInt(exam_type, CalcNumber);
        vecItemCalc.push_back(item_num);
    }
}

//获得exam_type检查部位下的测量项总数
int MeasureSetting::GetMeasureListNum(char *exam_type)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    return  ptrIni->ReadInt(exam_type, "Number");

}

//获得exam_type测量的排序方法 0:none   1:repeat   2:next
int MeasureSetting::GetMeasureSequence(const char *exam_type)
{
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, MEASURE_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    return  ptrIni->ReadInt(exam_type, "Sequence");

}



