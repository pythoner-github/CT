/*
 * =====================================================================================
 *
 *       Filename:  MeasureSetting.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18/4/2016
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhang jingxia 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef __MEASURE_SETTING_H__
#define __MEASURE_SETTING_H__

#include <gtk/gtk.h>
#include "MeasureDef.h"
#include "FakeXEvent.h"
#include <vector>
#include "ExamItem.h"

class MeasureSetting
{
    public:
        MeasureSetting();
        ~MeasureSetting();
        GtkWidget* GetWindow(void);
        GtkWidget* CreateExportCalcWindow(GtkWidget *parent);
        GtkWidget* CreateMeasureWindow(GtkWidget *parent);
        GtkWidget *m_win_parent;
        GtkWidget* m_treeView_all;
        static MeasureSetting* GetInstance();
        GtkWidget *m_treeview_item_calc;
        GtkWidget *m_treeview_item_calc1;
        GtkCellRenderer *m_cellrenderer_calc_text;
        GtkCellRenderer *m_cellrenderer_calc_text1;
        GtkWidget *m_combobox_sequence_calc;
        void ChangeModel(void);
        void ChangeModel2(void);
        void ChangeExamBox(char *check_part);
        void ChangeExamBoxDelete(void);
        void ChangeExamBoxToDefault(void);
        void ChangeModelAndLight(const char *name);
        void GetMeasureListEtype(char *exam_type, vector<int> &vecItemCalc);
        int GetMeasureListNum(char *exam_type);
        int GetMeasureSequence(const char *exam_type);
        int GetCalcMaxEtype();
        const gchar* GetExamName(void);
        string ItemMenuTransEnglish(int item_num);


    private:
        GtkWidget *fixed_calc;
        GtkWidget *m_combobox_exam_calc;
        GtkWidget *scrolledwindow_item_calc1;
        GtkWidget *scrolledwindow_item_calc;
        GtkWidget* m_button_select_one;
        GtkWidget *m_button_select_all;
        GtkWidget *m_button_calc_delete;
        GtkWidget *m_button_calc_delete_all;
        GtkWidget *m_button_calc_delete_select;
        GtkWidget *m_button_calc_up;
        GtkWidget *m_button_calc_down;
        GtkWidget *m_button_calc_default;

        static MeasureSetting* m_ptrInstance;

		int GetSequence(const char *exam_type);

        int  create_exam_calc_model(vector<string> &vecExamItem_calc);
        void add_columns_calc(GtkTreeView *treeview);
        void add_columns_calc1(GtkTreeView *treeview);
        GtkTreeModel* create_item_calc_model1();
        void CreateItemListForMeasure(vector<string>& vecItemCalc);
        //GtkTreeModel* create_item_calc_model();
        GtkTreeModel* create_item_calc_model2();
        void CreateItemListForMeasureed(char *probe_exam,vector<string>& vecItemCalc1);
        void CreateItemList_Calc2(char *probe_exam,vector<int>& vecItemCalc1);
        void CreateItemListForDeleteMeasureed(char *select_name, string probe_exam,vector<int>& vecItemCalc1);
        GtkTreeIter InsertUniqueCalc(GtkTreeModel *model, const char *str);
        void CreateDefineItem_calc(vector<string>& vecExamItem_calc);
        int  create_exam_calc_model();
        int ItemNameTransEtype(char *select_name);
        // signal handle
        void ExamCalcChanged(GtkComboBox *widget);
        void ButtonBackOneClicked(GtkButton *button);
        void ButtonBackAllClicked(GtkButton *button);
        void ButtonSelectOneCalcClicked(GtkButton *button);
        void ButtonSelectAllCalcClicked(GtkButton *button);
        void ButtonDeleteAllClicked(GtkButton *button);
        void ButtonUpClicked(GtkButton *button);
        void ButtonDownClicked(GtkButton *button);
        void ButtonDefaultClicked(GtkButton *button);
        void MeasureSequenceChanged(GtkComboBox *widget);

        static void HandleMeasureSequenceChanged(GtkComboBox *widget, MeasureSetting *data) {data->MeasureSequenceChanged(widget); }

        static void HandleExamCalcChanged(GtkComboBox *widget, MeasureSetting *data) {data->ExamCalcChanged(widget); }

        static void HandleButtonSelectOneCalcClicked(GtkButton *button, MeasureSetting *data)
        {
            if (data)
                data->ButtonSelectOneCalcClicked(button);
        }
        static void HandleButtonSelectAllCalcClicked(GtkButton *button, MeasureSetting *data)
        {
            if (data)
                data->ButtonSelectAllCalcClicked(button);
        }

              
        static void HandleButtonBackOneClicked(GtkButton *button, MeasureSetting *data)
        {
            if (data)
                data->ButtonBackOneClicked(button);
        }

        static void HandleButtonBackAllClicked(GtkButton *button, MeasureSetting *data)
        {
            if (data)
                data->ButtonBackAllClicked(button);
        }

        static void HandleButtonDownClicked(GtkButton *button, MeasureSetting *data)
        {
            if (data)
                data->ButtonDownClicked(button);
        }
        static void HandleButtonUpClicked(GtkButton *button, MeasureSetting *data)
        {
            if (data)
                data->ButtonUpClicked(button);
        }

      
        static void HandleButtonDefaultClicked(GtkButton *button, MeasureSetting *data)
        {
            if (data)
                data->ButtonDefaultClicked(button);
        }

};
#endif
