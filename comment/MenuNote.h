#ifndef MENU_NOTE_H
#define MENU_NOTE_H


class MenuNote
{
	public:
		MenuNote();
		~MenuNote() {}
		void Hide(void);
		void Show(void);

        GtkWidget* Create(void);
        void Focus(void);
        GtkWidget * create_abdo_item();
        GtkWidget * create_uro_item();
        GtkWidget * create_car_item();
        GtkWidget * create_ob_item();
        GtkWidget * create_gyn_item();
        GtkWidget * create_sp_item();
        GtkWidget * create_vs_item();
        GtkWidget * create_ortho_item();
        GtkWidget * create_tendon_item();
        GtkWidget * create_current_item();
        void UpdateLabel(void);
        void DepartmentName(char department[256], int index);

    private:
        enum {
            NAME_COLUMN,
            INDEX_COLUMN,
            N_COLUMNS
        };

        GtkWidget *m_treeView_comment;
        GtkWidget *m_fixed;
        GtkWidget *m_treeview;
        GtkWidget *m_treeview_item_current_comment;
        GtkWidget *m_treeview_item_abdo_comment;
        GtkWidget *m_treeview_item_uro_comment;
        GtkWidget *m_treeview_item_ob_comment;
        GtkWidget *m_treeview_item_car_comment;
        GtkWidget *m_treeview_item_gyn_comment;
        GtkWidget *m_treeview_item_vs_comment;
        GtkWidget *m_treeview_item_ortho_comment;
        GtkWidget *m_treeview_item_sp_comment;
        GtkWidget *m_treeview_item_tendon_comment;

        GtkWidget *scrolledwindow_item_current_comment;
        GtkWidget *scrolledwindow_item_abdo_comment;
        GtkWidget *scrolledwindow_item_uro_comment;
        GtkWidget *scrolledwindow_item_ob_comment;
        GtkWidget *scrolledwindow_item_car_comment;
        GtkWidget *scrolledwindow_item_gyn_comment;
        GtkWidget *scrolledwindow_item_sp_comment;
        GtkWidget *scrolledwindow_item_vs_comment;
        GtkWidget *scrolledwindow_item_ortho_comment;
        GtkWidget *scrolledwindow_item_tendon_comment;
        
        GtkCellRenderer *m_cellrenderer_comment_text;

        GtkWidget * m_table;
        GtkWidget * m_comboDept;
        GtkTreeModel * m_modelDept;
        GtkTreeModel* CreateDeptModel(void);
        
        GtkWidget* create_item_list(int index , char *department);
        void UpdateScaleModel(void);
        void HideMenu(void);
        void ShowMenu(int index, char *department);
        GtkTreeModel* create_item_note_model(int index, char *department);
        // signal handle
        void BtnComboDeptChanged(GtkComboBox *widget);
        static void HandleComboDeptChanged(GtkComboBox *widget, MenuNote *data) { data->BtnComboDeptChanged(widget); }
        void add_columns_comment(GtkTreeView *treeview);

        void LoadNoteToList(GtkTreeView *treeview);
        void AddColums(GtkTreeView *treeview);

        //signal handle
        //void TreeSelectionChanged(GtkTreeSelection *selection);
        void TreeBtnPress(GtkWidget *widget, GdkEventButton *event);

        //signal connect 
        //static void HandleSelectionChanged(GtkTreeSelection *selection, MenuNote *data) { data->TreeSelectionChanged(selection); }
        static gboolean HandleTreeBtnPress(GtkWidget *widget, GdkEventButton *event, MenuNote *data) { data->TreeBtnPress(widget, event); return FALSE; }
};

extern MenuNote g_menuNote;

#endif
