// -*- c++ -*-
#ifndef VIEWPROBE_H
#define VIEWPROBE_H

#include <gtk/gtk.h>
#include "ProbeSelect.h"
#include "FakeXEvent.h"

class ViewProbe : public FakeXEvent
{
public:
    ~ViewProbe();
    static ViewProbe* GetInstance();
    bool Create(void);
    bool Create(int socket);
    void Destroy(void);
    void ReadProbe(void);
    void ReadOneProbe(int socket);
    const gchar* GetUserName(void);
    string GetItemNameUserDef() { return m_itemNameUserDef; }
    int GetProbeIndex() { return m_probeIndex; }
    int GetCurExamItemIndex() { return m_examItemIndex; }

private:
    enum
    {
        PROBE_INDEX,
        EXAM_COLUMN,
        N_COLUMNS
    };

    static ViewProbe* m_ptrInstance;

    ProbeSelect m_pKps;
    int m_probeIndex;
    int m_itemIndex;
    int m_probe_index;
    GtkWidget *m_window;
    vector<ExamItem::EItem>* m_itemList;
    vector<string>* probeItemName;
    vector<string> m_vecUserItemName;
    char *m_userItemName;
    string m_itemNameUserDef;
    GtkWidget *m_combobox_user_select;
    ViewProbe();
    void KeyEvent(unsigned char keyValue);
    GtkWidget *treeview0;
    GtkWidget *treeview1;
    GtkWidget *treeview2;
    GtkWidget *treeview3;
    int m_preSocketIndex;
    int m_examItemIndex;
    int m_treeViewIndex;
    gchar *m_tree_view_path;
    char m_treeViewPath[32];

    void CreateWindow(ProbeSocket::ProbePara* para, vector<ExamItem::EItem>* itemList, int maxSocket);
    GtkWidget* CreateProbe(const char *probe_name, const char probeType, 
                           char exam_type[][50], int probe_index);
    int CreateAllExamType(const char *model, vector<ExamItem::EItem> item, char exam_type[][50]);
    void GetUserItemNameOfProbe(const char* model);
    //void GetUserItemNameOfProbe(const char* model, vector<string> &useritemname);
    int CreateExamType(vector<ExamItem::EItem> item, char exam_type[][50]);
    GtkTreeModel* CreateModel(char exam_type[][50], int probe_index);
    void AddColumn(GtkTreeView *treeView);
    GtkWidget* CreateTreeView(char exam_type[][50], int probe_index);
    void GetCurExamItemIndex(char exam_type[][50], int probe_index);

    // signal handle
    gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
    void BtnCancelClicked(GtkButton *button);
    // void BtnOKClicked(GtkButton *button);
    void TreeSelectionChanged(GtkTreeSelection *treeselection);
    void TreeViewFocusOut(GtkWidget *widget, GdkEventFocus *event);
    void ProbeUserChanged(GtkWidget *widget);
    void TreeViewBtnClicked(GtkWidget *widget, GdkEventButton *event);


    // signal connect
    static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, ViewProbe *data) { return data->WindowDeleteEvent(widget, event); }
    // static void on_button_ok_clicked(GtkButton *button, ViewProbe *data) { data->BtnOKClicked(button); }
    static void on_button_cancel_clicked(GtkButton *button, ViewProbe *data) { data->BtnCancelClicked(button); }
    static gboolean HandleTreeViewFocusOut(GtkWidget *widget, GdkEventFocus *event, ViewProbe *data) {
        data->TreeViewFocusOut(widget, event);
        return FALSE;
    }
    static void HandleTreeSelectionChanged(GtkTreeSelection *treeselection, ViewProbe *data) {
        data->TreeSelectionChanged(treeselection);
    }
    static void HandleProbeUserChanged(GtkWidget *widget, ViewProbe *data) {data->ProbeUserChanged(widget); }

    static gboolean HandleTreeViewButtonClick(GtkWidget *widget, GdkEventButton *event, ViewProbe *data) {
            data->TreeViewBtnClicked(widget, event);
			return TRUE;
    }
};
#endif
