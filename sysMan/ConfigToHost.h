#ifndef _CONFIGTOHOST_H_
#define _CONFIGTOHOST_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

class FakeXEvent;
class ConfigToHost: public FakeXEvent
{
    public:
        ~ConfigToHost();
        static ConfigToHost* GetInstance();
        void CreateWindow(GtkWindow *parent);
        void CreateCalcImportWindow(GtkWindow *parent);	
        GtkWidget* GetWindow() {return m_window;}
        vector<string> GetSelectedVec() {return m_vecPath;}
        void ClearData() { m_vecPath.clear();}
        void DestroyWindow(void);
        void SetProgressBar(double fraction);
        void ExportLoadInfoNotice(char *result);
        void ExportRightInfoNotice(char *result);
        void ExportErrorInfoNotice(char *result);
        void DeleteUdiskFile();
        void HideOKAndCancelClicked(); 
        void OKAndCancelClicked(); 

    private:
        enum{
            COL_CHECKED,
            COL_NAME,
            NUM_COLS
        };

        ConfigToHost(void);
        static ConfigToHost* m_ptrInstance;
        void KeyEvent(unsigned char KeyValue);
      
        GtkWidget *m_window;

        GtkWidget *m_window1;
        GtkWidget *m_treeRoot;
        GtkWidget *m_treeBranch;
        GtkWidget *m_chkbtnDel;
        GtkWidget *btnOK;
        GtkWidget *btnCancel; 
        GList *m_listBranch;
        GtkWidget *label_calc_notice1;
        vector<string> m_vecPath;
        GtkWidget *m_progress_bar;
        GtkWidget *img_right;
        GtkWidget *img_error;
        GtkWidget *img_load;
        GtkWidget* create_calc_treeview();
        GtkWidget* create_treeview(gint type);	
        GtkTreeModel* create_root_model(void);
        GtkTreeModel* create_calc_root_model();
        void UpdateRootModel(void);
        void UpdateCalcRootModel(void); 
        void UpdateBranchModel(gint rows);
        GtkTreeModel* LoadBranchModel(gchar *branch);
        void SetAllToggleValue(GtkTreeModel *model, gboolean value);
        gboolean GetAllSelectPath(void);
        gboolean GetAllCalcSelectPath(void);

        gboolean CheckBranchStauts(void);

        //signal handle
        gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
        //	void BtnSelectAllClicked(GtkButton *button);
        //	void BtnDeselectClicked(GtkButton *button);
        void BtnOKClicked(GtkButton *button);
        void BtnCalcImportOKClicked(GtkButton *button); 
        void BtnCancelClicked(GtkButton *button);
        void ToggleData(GtkCellRendererToggle *cell, gchar *path_str);
        void RootSelectionChanged(GtkTreeSelection *selection);
        void RootCalcSelectionChanged(GtkTreeSelection *selection);

        //signal connect 
        static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, ConfigToHost* data) { return data->WindowDeleteEvent(widget, event); }
        static void on_data_toggled(GtkCellRendererToggle *cell, gchar *path_str, ConfigToHost* data) { data->ToggleData(cell, path_str); }

        static void on_root_calc_selection_changed(GtkTreeSelection *selction, ConfigToHost* data) { data->RootCalcSelectionChanged(selction); }

        static void on_root_selection_changed(GtkTreeSelection *selction, ConfigToHost* data) { data->RootSelectionChanged(selction); }
        static void on_button_ok_clicked(GtkButton *button, ConfigToHost* data) { data->BtnOKClicked(button); }
        static void on_button_calc_ok_clicked(GtkButton *button, ConfigToHost* data) { data->BtnCalcImportOKClicked(button); }
        static void on_button_cancel_clicked(GtkButton *button, ConfigToHost* data) { data->BtnCancelClicked(button); }
        //	static void on_button_select_all_clicked(GtkButton *button, ConfigToHost* data) { data->BtnSelectAllClicked(button); }
        //	static void on_button_deselect_clicked(GtkButton *button, ConfigToHost* data) { data->BtnSelectAllClicked(button); }
};

#endif
