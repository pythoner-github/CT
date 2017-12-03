#ifndef VIEW_SUPER_USER_H
#define VIEW_SUPER_USER_H

#include <vector>
#include <string>
#include "display/FakeXEvent.h"

class ViewSuperuser:public FakeXEvent
{
public:
	~ViewSuperuser();
	static ViewSuperuser* GetInstance();
    void Authenticate(void);
    void CreateWindow(void);
    void CreateDemoWindow(void);
    void DestroyWindow(void);
    bool GetDemoStatus() { return !m_statusDemo; }

#ifdef EMP_PROJECT
    bool GetProjectModeStatus(void) { return m_statusProjectMode; }
#endif

   void ExitDemo();

#ifdef TRANSDUCER
    int GetPressValue() { return m_tranPressCorrect;}
#endif

    enum {EPM_COLUMN, EPA_COLUMN, END_COLUMN};
private:
	ViewSuperuser();

	static ViewSuperuser* m_ptrInstance;
    static const unsigned int AUTHEN_NUM = 11;
    static const unsigned int DEMO_NUM = 5;

#if defined(EMP_360)
    //static const int WPORBE_NUM = 6;
    static const int WPORBE_NUM = 8;
#elif defined(EMP_161)
    static const int WPORBE_NUM = 5; //4;
#elif (defined(EMP_322) || defined(EMP_313))
    static const int WPORBE_NUM = 9; //7;
#elif defined(EMP_430)
    static const int WPORBE_NUM = 5;
#elif defined(EMP_440)
    static const int WPORBE_NUM = 4;
#elif (defined(EMP_355))
#ifdef VET
    static const int WPORBE_NUM = 15;
#else
    static const int WPORBE_NUM = 14; //10;//11;
#endif
#else
#ifdef VET
    static const int WPORBE_NUM = 11;
#else
    static const int WPORBE_NUM = 11;
#endif
#endif

    static std::string m_probeName[WPORBE_NUM];

    GtkWidget *m_window;
    GtkWidget *m_labelHint;
    GtkWidget *m_treeviewProbe;
    GtkWidget *m_notebook;

    // anthen
    std::vector<unsigned char> m_vecAuthenInfo;
    bool m_statusAuthen;
    bool m_statusDemo;

#ifdef EMP_PROJECT
    bool m_statusProjectMode;
#endif

    int m_timer;

    //probe
    int m_probeType;
    GtkWidget *m_btn_edit;
    GtkWidget *m_treeView;
    GtkCellRenderer *m_render;
    char m_probeModel[32];
    std::string m_strProbeAlias;
    char m_probeAlias[256];
    //transducer press value correct
    int m_tranPressCorrect;
	GtkWidget* create_probe_treeview(void);
	GtkTreeModel* create_probe_model(void);
    virtual void KeyEvent(unsigned char keyValue);

    // probe
	gboolean UpdateProbeStatus(const char* socket, const char* status);
    gboolean IsAuthenValid(void);
    gboolean WriteProbe(void);
    bool ConfirmAuthen(void);
    bool ConfirmDemo(void);
    void Exit(void);
    GtkWidget* CreateNoteProbe(void);
    GtkWidget* CreateNoteMonitor(void);

#ifdef EMP_PROJECT
    GtkWidget* CreateNoteProject(void);
#endif

// signal handle
    gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
	void MachineChanged(GtkComboBox *combobox);
	void ProbeChanged(GtkComboBox *combobox);
	void BtnWriteClicked(GtkButton *button);
	void BtnExitClicked(GtkButton *button);
	void ChgAperture(GtkComboBox *combobox);

#ifdef TRANSDUCER
    void SpinbuttonPressAdjust(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
#endif

    //demo
    void BtnEnterDemoClicked(GtkButton *button);
	void BtnChgMonitorClicked(GtkButton *button);
	void BtnChgMonitorClicked2(GtkButton *button);

    void BtnEditClicked(GtkButton *button);
    void BtnFactoryClicked(GtkButton *button);
    void TreeSelectionChanged(GtkTreeSelection *selection);
	void EditedCellChanged(GtkCellRendererText *cell, gchar *path_string, gchar *new_text);
    void EntryProbeAliasInsert(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path);
    void EntryProbeAliasLengthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);

    // signal connect
    static gboolean HandleWindowDeleteEvent(GtkWidget *widget, GdkEvent *event, ViewSuperuser *data) { return data->WindowDeleteEvent(widget, event); }
    static void HandleMachineChanged(GtkComboBox *widget, ViewSuperuser *data) { return data->MachineChanged(widget); }
    static void HandleProbeChanged(GtkComboBox *widget, ViewSuperuser *data) { return data->ProbeChanged(widget); }
    static void HandleBtnWriteClicked(GtkButton *widget, ViewSuperuser *data) { return data->BtnWriteClicked(widget); }
    static void HandleBtnExitClicked(GtkButton *widget, ViewSuperuser *data) { return data->BtnExitClicked(widget); }
    static void HandleChgAperture(GtkComboBox *widget, ViewSuperuser *data) { return data->ChgAperture(widget); }
    static void HandleBtnChgMonitorClicked(GtkButton *widget, ViewSuperuser *data) { return data->BtnChgMonitorClicked(widget); }
    static void HandleBtnChgMonitorClicked2(GtkButton *widget, ViewSuperuser *data) { return data->BtnChgMonitorClicked2(widget); }

#ifdef TRANSDUCER
    static void on_spinbutton_press_adjust(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSuperuser *data)
    { data->SpinbuttonPressAdjust(editable, new_text, new_text_length, position); }
#endif

    //demo
    static void HandleBtnEnterDemoClicked(GtkButton *widget, ViewSuperuser *data) { return data->BtnEnterDemoClicked(widget); }

    static gboolean HandleAuthen(gpointer data) { return m_ptrInstance->IsAuthenValid(); }
    static gboolean HandleWriteProbe(gpointer data) { return m_ptrInstance->WriteProbe(); }

#ifdef EMP_PROJECT
    //ProjectMode
    static void HandleProjectModeClicked(GtkButton *widget, ViewSuperuser *data) { return data->BtnProjectModeClicked(widget); }

public:
    void BtnProjectModeClicked(GtkButton *button);
    #endif

    static void HandleTreeSelectionChanged(GtkTreeSelection *selection, ViewSuperuser *data) { data->TreeSelectionChanged(selection); }
    static void on_button_edit_clicked(GtkButton *button, ViewSuperuser *data) { data->BtnEditClicked(button); }
    static void on_button_factory_clicked(GtkButton *button, ViewSuperuser *data) { data->BtnFactoryClicked(button); }
	static void on_edited_cell_callback(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, ViewSuperuser *data){
		data->EditedCellChanged(cell, path_string, new_text);
	}
    static void  on_entry_probe_alias_insert(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path, ViewSuperuser *data)
    { data->EntryProbeAliasInsert(cell, editable, path); }

    static void on_entry_insert_item(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSuperuser *data)
    { data->EntryProbeAliasLengthInsert(editable, new_text, new_text_length, position); }
};

#endif