#ifndef _VIEW_TEMPLET_H_
#define _VIEW_TEMPLET_H_

#include "FakeXEvent.h"
#include <sqlite3.h>

#define TEMPLET_EN_DB "./res/glossary_en.db"
#define TEMPLET_CH_DB "./res/glossary_ch.db"
static const unsigned int maxNodeLen = 60;

class ViewTemplet:public FakeXEvent
{
public:
    ~ViewTemplet();
    static ViewTemplet* GetInstance();
    void CreateWindow(void);
    void DestroyWindow(void);
    bool DefaultFactory(void);
 void tree_auto_scroll(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data);
    

private:
    ViewTemplet();

    GtkTreeModel* CreateTreeModel(void);
	void KeyEvent(unsigned char keyValue);
    void AddTreeViewColumn(GtkTreeView *treeview);
    bool OpenDB(void);
    bool CloseDB(void);
    bool UniqueItem(const char *str1, const char *str2 = "%", const char *str3 = "%", const char *str4 = "%");
    GtkTreeIter InsertUnique(GtkTreeModel *model, GtkTreeIter *iter, const char *str);
    void InitTreeView(void);
    static ViewTemplet* m_ptrInstance;
    static sqlite3 *m_db_templet;

    GtkWidget *m_window;
    GtkWidget *m_treeView;
    int m_flagDepth;//复制时结点的深度标志
    // char m_copyNode[4][maxNodeLen];//复制结点时所选结点的值(f1-f4)
    // char m_parentNode[maxNodeLen];//复制时所选结点的父结点

    std::string m_copyNode[4];	// 复制结点时所选结点的值(f1-f4)
    std::string m_parentNode;	// 复制时所选结点的父结点
    int m_nodeHeight;

    GtkCellRenderer *m_renderer;
    GtkWidget *m_textview_indication;
    GtkTextBuffer *m_buffer_indication;
    GtkWidget *m_textview_comments;
    GtkTextBuffer *m_buffer_comments;

    GtkWidget *m_button_insert;
    GtkWidget *m_button_delete;
    GtkWidget *m_button_copy;
    GtkWidget *m_button_paste;
    GtkWidget *m_button_transfer;
    GtkWidget *m_button_exit;
    GtkWidget *m_button_edit;
    GtkWidget *m_button_save;
    GtkWidget *m_button_default;
    GtkWidget *m_button_return;

    GtkTreeIter m_topIter;//根结点

// signal handle
    void BtnExitClicked(GtkButton *button);
    void TreeSelectionChanged(GtkTreeSelection *selection);
    void InsertClicked(GtkButton *button);
    void DeleteClicked(GtkButton *button);
    void CopyClicked(GtkButton *button);
    void PasteClicked(GtkButton *button);
    void TransferClicked(GtkButton *button);
    void ExitClicked(GtkButton *button);
    void EditClicked(GtkButton *button);
    void SaveClicked(GtkButton *button);
    void DefaultClicked(GtkButton *button);
    void ReturnClicked(GtkButton *button);
    void CellRendererEdited(GtkCellRendererText *renderer, gchar *path, gchar *new_text);
  bool TempletTestRowExpandBefore(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path);
   gboolean BtnTempletClicked(GtkWidget *widget, GdkEventButton *event);

// signal connect
static void HandleTempletBtnClicked(GtkWidget *widget, GdkEventButton *event, ViewTemplet *data) { data->BtnTempletClicked(widget, event); }
  static bool HandleTempletExpandBefore(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path, ViewTemplet *data)
    { 
        return data->TempletTestRowExpandBefore(treeview, iter, path);
    }

    static void HandleTreeSelectionChanged(GtkTreeSelection *selection, ViewTemplet *data) { data->TreeSelectionChanged(selection); }
    static void HandleInsertClicked(GtkButton *button, ViewTemplet *data) { data->InsertClicked(button); }
    static void HandleDeleteClicked(GtkButton *button, ViewTemplet *data) { data->DeleteClicked(button); }
    static void HandleCopyClicked(GtkButton *button, ViewTemplet *data) { data->CopyClicked(button); }
    static void HandlePasteClicked(GtkButton *button, ViewTemplet *data) { data->PasteClicked(button); }
    static void HandleTransferClicked(GtkButton *button, ViewTemplet *data) { data->TransferClicked(button); }
    static void HandleExitClicked(GtkButton *button, ViewTemplet *data) { data->ExitClicked(button); }
    static void HandleEditClicked(GtkButton *button, ViewTemplet *data) { data->EditClicked(button); }
    static void HandleSaveClicked(GtkButton *button, ViewTemplet *data) { data->SaveClicked(button); }
    static void HandleDefaultClicked(GtkButton *button, ViewTemplet *data) { data->DefaultClicked(button); }
    static void HandleReturnClicked(GtkButton *button, ViewTemplet *data) { data->ReturnClicked(button); }
    static void HandleCellRendererEdited(GtkCellRendererText *renderer, gchar *path, gchar *new_text, ViewTemplet *data) { data->CellRendererEdited(renderer, path, new_text); }
};

#endif
