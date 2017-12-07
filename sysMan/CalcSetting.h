#ifndef __CALC_SETTING_H__
#define __CALC_SETTING_H__

#include <gtk/gtk.h>
#include "calcPeople/MeasureDef.h"
#include "display/FakeXEvent.h"
#include <vector>
#include "probe/ExamItem.h"

#ifdef VET
#ifdef EMP_355
static const int EXAM_NUM = 5;
#else
static const int EXAM_NUM = 9;
#endif
static const int SECTION_NUM =22;
static const int ETYPE_NUM =35;
static const int MAX_METHOD = 17;
#else
#ifdef EMP_322
static const int EXAM_NUM = 18;
static const int SECTION_NUM =12;
static const int ETYPE_NUM =28;
static const int MAX_METHOD = 15;
#else
static const int EXAM_NUM = 23;
static const int SECTION_NUM =18;
static const int ETYPE_NUM =35;
static const int MAX_METHOD = 17;
#endif
#endif
struct CustomEtype
{
   std::string name;
   int etype;
};

struct CustomTypeAndMethod
{
    int type;
    std::string name;
    int etype;
};

class CustomCalc:public FakeXEvent
{

    public:
        enum
        {
            N_COMBO_COL1,
            N_COMBO_TOTAL
        };
        CustomCalc();
        ~CustomCalc();
        static CustomCalc* GetInstance();
        GtkWidget* CreateImportRenameSettingWin(GtkWidget *parent);
        GtkWidget* CreateExportCalcSettingWin(GtkWidget *parent);
        GtkWidget* CreateCalcSettingWin(GtkWidget *parent);
        void DestroyWin(void);

        void DelayDestroyWin(void);
        GtkWidget *GetWindow(){return m_window;}
        void SetProgressBar(double fraction);
        void ExportLoadInfoNotice(char *result);
        void ExportRightInfoNotice(char *result);
        void ExportErrorInfoNotice(char *result);
        void HideOKAndCancelClicked();
        void OKAndCancelClicked();
        bool RenameCompare(char * name_copy);
        void ImportSuccess(void);
        bool ImportCopy(int j);
        void ImportRenameCopy(string item_name);
        void ButtonImportNameOK();
        void ImportWrite(string item_name, int &item_num);
    private:
        GtkWidget *m_entry_name;
        GtkWidget *m_entry_export_name;
        GtkWidget *m_combobox_type;
        GtkWidget *m_combobox_method;
        GtkWidget *label_type;
        GtkWidget *label_method;
        GtkWidget *m_window;
        GtkWidget *fixed1;
        GtkWidget *m_frame_new_notice;
        GtkWidget *m_label_notice;
        GtkWidget *fixed_new_notice;
        GtkWidget *m_label_notice1;
        GtkWidget *m_label_notice2;
        GtkWidget *m_label_notice3;
        GtkWidget *m_progress_bar;
        GtkWidget *button_right;
        GtkWidget *img_right;
        GtkWidget *button_error;
        GtkWidget *img_error;
        GtkWidget *button_load;
        GtkWidget *img_load;
        GtkWidget *button_ok;
        GtkWidget *button_cancel;

        GtkTreeModel *CreateComboModel(char name[][20], int n);
        void KeyEvent(unsigned char keyValue);
        static CustomCalc* m_ptrInstance;
        void ComboboxTypeChanged(GtkComboBox *combobox);
        void ButtonRenameCancelClicked(GtkButton *button);
        void ButtonCancelClicked(GtkButton *button);
        void ButtonOKClicked(GtkButton *button);
        void ButtonExportNameOKClicked(GtkButton *button);
    //      void ButtonImportNameOK();
        void EntryItemInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
        void EntryItemFocusIn(GtkEditable *editable, GdkEventFocus *event);

       static void HandleButtonRenameCancelClicked(GtkButton *button, CustomCalc *data)
        {
            if (data)
                data->ButtonRenameCancelClicked(button);
        }

        static void HandleButtonCancelClicked(GtkButton *button, CustomCalc *data)
        {
            if (data)
                data->ButtonCancelClicked(button);
        }
        static void HandleButtonOKClicked(GtkButton *button, CustomCalc *data)
        {
            if (data)
                data->ButtonOKClicked(button);
        }

        static void HandleButtonExportNameOKClicked(GtkButton *button, CustomCalc *data)
        {
            if (data)
                data->ButtonExportNameOKClicked(button);
        }

  /*      static void HandleButtonImportNameOKClicked(GtkButton *button, CustomCalc *data)
        {
            if (data)
                data->ButtonImportNameOKClicked(button);
        }
*/
        static void on_entry_focus_in(GtkEditable *editable, GdkEventFocus *event, CustomCalc *data)
        { data->EntryItemFocusIn(editable, event); }

        //signal handle
        gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);

        //signal
        static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, CustomCalc *data) { return data->WindowDeleteEvent(widget, event); }

        static void HandleComboboxTypeChanged(GtkComboBox *combobox, CustomCalc *data)
        {
            if (data)
                data->ComboboxTypeChanged(combobox);
        }

};

enum {
    NAME_COLUMN,
    N_COLUMNS
};

class CalcSetting
{
    public:
        CalcSetting();
        ~CalcSetting();
        GtkWidget* GetWindow(void);
        GtkWidget* CreateExportCalcWindow(GtkWidget *parent);
        GtkWidget* CreateCalcWindow(GtkWidget *parent);
        GtkWidget *m_win_parent;
        GtkWidget* m_treeView_all;
        static CalcSetting* GetInstance();
        GtkWidget *m_treeview_item_calc;
        GtkWidget *m_treeview_item_calc1;
        GtkCellRenderer *m_cellrenderer_calc_text;
        GtkCellRenderer *m_cellrenderer_calc_text1;
        GtkWidget *m_combobox_department_calc;
        GtkWidget *m_combobox_sequence_calc;
        void ChangeModel(void);
        void ChangeModel2(void);
        void ChangeExamBox(char *check_part);
        void ChangeExamBoxDelete(void);
        void ChangeExamBoxToDefault(void);
        void ChangeModelAndLight(const char *name);
        void GetCalcListEtype(char *exam_type, vector<int> &vecItemCalc);
        int GetCalcListNum(char *exam_type);
        int GetMeasureSequence(const char *exam_type);
        void GetCustomCalcMeasure(int Etype, int &measure_type, string &calc_name);
        int GetCalcMaxEtype();
        void GetDepartmentForCustomMeasure(int Etype, string &department);
        vector<string> GetSelectedVec() {return m_vecPath;}
        vector<string> GetSelectedVec1() {return m_vecPath1;}
        gboolean GetSelectPath(void);
        const gchar* GetExamName(void);
        const gchar* GetDepartmentName(void);
        string ItemMenuTransEnglish(int item_num);
        string CustomItemTransName(int item_num);
        void DeleteItem(void);
        void AddItem(void);
        void UpdateAllCalc(void);
        void ClearAllCalc(void);

    private:
        GtkWidget *fixed_calc;
        GtkWidget *m_combobox_exam_calc;
        GtkWidget *scrolledwindow_item_calc1;
        GtkWidget *scrolledwindow_item_calc;
        GtkWidget* m_button_select_one;
        GtkWidget *m_button_select_all;
        GtkWidget *m_button_calc_add;
        GtkWidget *m_button_calc_delete;
        GtkWidget *m_button_calc_delete_all;
        GtkWidget *m_button_calc_delete_select;
        GtkWidget *m_button_calc_up;
        GtkWidget *m_button_calc_down;
        GtkWidget *m_button_calc_export;
        GtkWidget *m_button_calc_import;
        GtkWidget *m_button_calc_default;

        static CalcSetting* m_ptrInstance;
        CustomCalc m_custom_calc_win;
        GtkWidget* m_section_view;
        GtkWidget* m_section_combobox;
        vector<string> m_vecPath;
        vector<string> m_vecPath1;

        int GetSequence(const char *exam_type);

        int  create_exam_calc_model(vector<string> &vecExamItem_calc);
        void add_columns_calc(GtkTreeView *treeview);
        void add_columns_calc1(GtkTreeView *treeview);
        GtkTreeModel* create_item_calc_model1();
        void CreateDefaultItemList_Calc(char *department,vector<string>& vecItemCalc);
        void CreateItemList_Calc(char *department,vector<string>& vecItemCalc);
        //GtkTreeModel* create_item_calc_model();
        GtkTreeModel* create_item_calc_model2();
        void CreateItemList_Calc1(char *probe_exam,vector<string>& vecItemCalc1);
        void CreateItemList_Calc2(char *probe_exam,vector<int>& vecItemCalc1);
        void CreateItemList_Delete_Calc1(char *select_name, string probe_exam,vector<int>& vecItemCalc1);
        void CreateItemList_Delete_Calc(char *select_name, char *department, vector<int>& vecDeleteCalc, IniFile *ini);
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
        void ButtonAddClicked(GtkButton *button);
        void ButtonDeleteSelectClicked(GtkButton *button);
        void ButtonDeleteAllClicked(GtkButton *button);
        void ButtonUpClicked(GtkButton *button);
        void ButtonDownClicked(GtkButton *button);
        void ButtonExportClicked(GtkButton *button);
        void ButtonImportClicked(GtkButton *button);
        void ButtonDefaultClicked(GtkButton *button);
        void MeasureSequenceChanged(GtkComboBox *widget);
        void DepartmentCalcChanged(GtkComboBox *widget);
        static void HandleDepartmentCalcChanged(GtkComboBox *widget, CalcSetting *data) {data->DepartmentCalcChanged(widget); }

        static void HandleMeasureSequenceChanged(GtkComboBox *widget, CalcSetting *data) {data->MeasureSequenceChanged(widget); }

        static void HandleExamCalcChanged(GtkComboBox *widget, CalcSetting *data) {data->ExamCalcChanged(widget); }

        static void HandleButtonSelectOneCalcClicked(GtkButton *button, CalcSetting *data)
        {
            if (data)
                data->ButtonSelectOneCalcClicked(button);
        }
        static void HandleButtonSelectAllCalcClicked(GtkButton *button, CalcSetting *data)
        {
            if (data)
                data->ButtonSelectAllCalcClicked(button);
        }

        static void HandleButtonDeleteSelectClicked(GtkButton *button, CalcSetting *data)
        {
            if (data)
                data->ButtonDeleteSelectClicked(button);
        }
        static void HandleButtonAddClicked(GtkButton *button, CalcSetting *data)
        {
            if (data)
                data->ButtonAddClicked(button);
        }

        static void HandleButtonBackOneClicked(GtkButton *button, CalcSetting *data)
        {
            if (data)
                data->ButtonBackOneClicked(button);
        }

        static void HandleButtonBackAllClicked(GtkButton *button, CalcSetting *data)
        {
            if (data)
                data->ButtonBackAllClicked(button);
        }

        static void HandleButtonDownClicked(GtkButton *button, CalcSetting *data)
        {
            if (data)
                data->ButtonDownClicked(button);
        }
        static void HandleButtonUpClicked(GtkButton *button, CalcSetting *data)
        {
            if (data)
                data->ButtonUpClicked(button);
        }

        static void HandleButtonExportClicked(GtkButton *button, CalcSetting *data)
        {
            if (data)
                data->ButtonExportClicked(button);
        }
        static void HandleButtonImportClicked(GtkButton *button, CalcSetting *data)
        {
            if (data)
                data->ButtonImportClicked(button);
        }

        static void HandleButtonDefaultClicked(GtkButton *button, CalcSetting *data)
        {
            if (data)
                data->ButtonDefaultClicked(button);
        }

};
#endif