#ifndef TREE_MENU_H
#define TREE_MENU_H

#include <gtk/gtk.h>
#include <vector>
using namespace std;

enum RendererType
{
    TEXT_ONLY, //GtkCellRendererText
    TOGGLE,   //GtkCellRendererToggle
};

enum EMeasureMode { MEA_2D, MEA_D, MEA_M, MEA_UNKNOWN };
enum EMeasureSequenceType { MEA_SEQ_NONE, MEA_SEQ_REPEAT, MEA_SEQ_NEXT };

//typedef void (*pFuncNone)(void);
typedef void (*pFuncNone)(int);
typedef void (*pFuncInt)(int);
typedef void (*pFuncChar)(char *);
typedef void (*pFuncTemp)(int ,char *);
union PtrFunc
{
    pFuncNone ptrNone;
    pFuncInt ptrInt;
    pFuncChar ptrChar;
    pFuncTemp ptrTemp;
};

struct GUIMenu
{
    const char *name;
    int etype;
    RendererType type;
    bool value;
    PtrFunc pf;
    GUIMenu *child;
};

struct GUIMenuNew
{
    const char *name;
    int etype;
    EMeasureMode mode;
    RendererType type;
    bool value;
    PtrFunc pf;
    vector<GUIMenuNew> child;
};

class TreeMenu
{
public:

    TreeMenu();
    ~TreeMenu(){};

    GtkWidget* GetTreeView(void);
    GtkWidget* Create(GUIMenu *pItem);
    GtkWidget* Create(GUIMenu *pItem, GUIMenu *pUserDefine);
    GtkWidget* Create(vector<GUIMenuNew> &vecItems);
    void Update(GtkWidget *treeView, GUIMenu *pItem);
    void SelectIter(GtkWidget *treeView, int parent, int child, bool select);
    void ChangeModel(GtkWidget *treeview, GUIMenu *pItem, GUIMenu *pUserDefine);
    void ChangeModel(GtkWidget *treeview, GUIMenu *pItem);
    void ChangeModel(GtkWidget *treeview, vector<GUIMenuNew> &vecItems);
    void Update(GtkWidget *treeView, GUIMenu *pItem, GUIMenu *pUserDefine);
    void tree_auto_scroll(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data);
    void SelectFirst(GtkWidget *tree_view);
    void SelectNext(GtkWidget *tree_view, EMeasureSequenceType sequence);
    void ExpandMenu(GtkWidget *tree_view);
    gboolean TreeViewTooltip(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip);
    /**
     *@return etype of current measure item.
     */
    int SetCompleteMeasureFlag(GtkWidget *tree_view, int &parentEtype);
    void Update(GtkWidget *treeView, vector<GUIMenuNew> &vecItems);

private:
    enum {
        NAME_COLUMN,
        ETYPE_COLUMN,
        RENDERER_TYPE_COLUMN,
        VALUE_COLUMN,
        PFUNC_COLUMN,
        N_COLUMNS
    };

    enum { NAME_COLUMN_WIDTH = 140, RENDERER_TYPE_COLUMN_WIDTH = 10 };

    GtkWidget *m_treeView;
    bool m_showTooltip;
    GtkTreeIter m_iterBak;
    void AddColumns(GtkTreeView *treeView);
    void AddColumnsNew(GtkTreeView *treeView);
    GtkTreeModel * CreateModel(GUIMenu *pItem);
    GtkTreeModel* CreateModel(GUIMenu *pItem, GUIMenu *pUserDefine);
    GtkTreeModel* CreateModel(vector<GUIMenuNew> &vecItems);

    void TreeViewExpanded(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path);
    void TreeViewCollapse(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path);
    bool TreeViewTestRowExpandBefore(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path);
// signal handle
    void TreeRowActivated(GtkTreeView *treeView, GtkTreePath *path, GtkTreeViewColumn *column);
    void TreeSelectionChanged(GtkTreeSelection *selection);
    gboolean BtnExamTypeClicked(GtkWidget *widget, GdkEventButton *event);
    gboolean TreeViewClicked(GtkWidget *widget, GdkEventButton *event);
    gboolean TreeViewMouseMove(GtkWidget *widget, GdkEventMotion *event);
    bool IsSetParentCompleteMeasureFlag(GtkTreeModel *tree_model, GtkTreeIter *iter);
    void SelectItemNew(GtkWidget *tree_view, GtkTreeModel *model, GtkTreeSelection *selection, GtkTreeIter *iter);

    // signal connect
    static void HandleExamTypeBtnClicked(GtkWidget *widget, GdkEventButton *event, TreeMenu *data) { data->BtnExamTypeClicked(widget, event); }

    static void HandleTreeRowActivated(GtkTreeView *treeView, GtkTreePath *path, GtkTreeViewColumn *column, TreeMenu *data)
    {
        data->TreeRowActivated(treeView, path, column);
    }
    static void HandleTreeSelectionChanged(GtkTreeSelection *selection, TreeMenu *data)
    {
        data->TreeSelectionChanged(selection);
    }
    static void HandleTreeViewExpanded(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path, TreeMenu *data)
    {
        data->TreeViewExpanded(treeview, iter, path);
    }
    static void HandleTreeViewCollapse(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path, TreeMenu *data)
    {
        data->TreeViewCollapse(treeview, iter, path);
    }
    static bool HandleTreeViewExpandBefore(GtkTreeView *treeview, GtkTreeIter *iter, GtkTreePath *path, TreeMenu *data)
    {
        return data->TreeViewTestRowExpandBefore(treeview, iter, path);
    }

    static gboolean HandleTreeViewClicked(GtkWidget *widget, GdkEventButton *event, TreeMenu *data)
    {
        return data->TreeViewClicked(widget, event);
    }

    static gboolean HandleTreeViewMouseMove(GtkWidget *widget, GdkEventMotion *event, TreeMenu *data)
    {
        return data->TreeViewMouseMove(widget, event);
    }

    static gboolean HandleTreeViewTooltip(GtkWidget  *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, TreeMenu *data)
    {
        return data->TreeViewTooltip(widget, x, y, keyboard_mode, tooltip);
    }

    static gboolean TimeoutMoveCursor(gpointer data);
};
#endif