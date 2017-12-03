// -*- c++ -*-
#ifndef _VIEW_DIALOG_H_
#define _VIEW_DIALOG_H_

#include <gtk/gtk.h>
#include "FakeXEvent.h" 

typedef int (*pFuncDialog)(gpointer data);

class ViewDialog: public FakeXEvent
{
public:
    enum EDialogType {INFO, WARNING, QUESTION, ERROR, PROGRESS, PRG_CANCEL, FILENAME};
    pFuncDialog m_ptrFunc;

    ~ViewDialog();
    static ViewDialog* GetInstance();

    void Create(GtkWindow *parent, EDialogType type, const char *info, pFuncDialog ptrFunc, pFuncDialog ptrFuncCancel=NULL);
    void Destroy(void);
    void SetProgressBar(double fraction);
    void SetText(const char *info);
      bool Exist(void);
	
private:
    ViewDialog();
    static ViewDialog* m_ptrInstance;

    void KeyEvent(unsigned char keyValue);

    GtkWidget *m_window;
    pFuncDialog m_ptrFuncCancel;
    GtkWidget *m_label_text;
    GtkWidget *m_progress_bar;
    GtkWidget *m_entry;
    EDialogType m_type;
	bool m_preCursor;

    // signal handle
    gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
    void BtnOkClicked(GtkButton *button);
    void BtnCancelClicked(GtkButton *button);
    void BtnCloseClicked(GtkButton *button);
    void BtnPrgCancelClicked(GtkButton *button);
    void EntryInsertText(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    
// signal connect
    static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, ViewDialog *data) { return data->WindowDeleteEvent(widget, event); }
    static void on_button_ok_clicked(GtkButton *button, ViewDialog *data)
	{ data->BtnOkClicked(button); }
    static void on_button_cancel_clicked(GtkButton *button, ViewDialog *data)
	{ data->BtnCancelClicked(button); }
    static void on_button_close_clicked(GtkButton *button, ViewDialog *data)
	{ data->BtnCloseClicked(button); }
    static void on_button_prg_cancel_clicked(GtkButton *button, ViewDialog *data)
	{ data->BtnPrgCancelClicked(button); }
    static void on_entry_insert_text(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewDialog *data)
	{ data->EntryInsertText(editable, new_text, new_text_length, position); }

};

/*
 *  Used for show info when something is processing, e.g. copying, sending. It is different from ViewDialog. In the normal case, using ViewDialog 
 *  to show info.
 */
class ViewHintDialog : public FakeXEvent
{
public:
    ~ViewHintDialog();
    static ViewHintDialog* GetInstance();

    void Create(GtkWindow *parent, const char *info);
    void Destroy(void);
    bool Exist(void);
private:
    ViewHintDialog();
    static ViewHintDialog* m_ptrInstance;
    
    GtkWidget *m_window;
};

#endif
