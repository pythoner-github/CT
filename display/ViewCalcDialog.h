// -*- c++ -*-
#ifndef _VIEWCALCDIALOG_H_
#define _VIEWCALCDIALOG_H_

#include <gtk/gtk.h>
#include "FakeXEvent.h" 

class ViewCalcDialog : public FakeXEvent
{
public:
    enum ECalcDialogType {CAV, RA};

    ~ViewCalcDialog();
    static ViewCalcDialog* GetInstance();

    void Create(ECalcDialogType type);
    void Destroy(void);
private:
    ViewCalcDialog();
    static ViewCalcDialog* m_ptrInstance;

    GtkWidget *m_window;
    GtkWidget *m_entry_manual_input;
    GtkWidget *m_radiobutton_top;
    GtkWidget *m_radiobutton_bottom;
    GtkWidget *m_radiobutton_manual;

    GtkWidget *m_radiobutton_5;
    GtkWidget *m_radiobutton_10;
    GtkWidget *m_radiobutton_15;
    GtkWidget *m_radiobutton_20;

// signal handle
    gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
    void BtnOkClicked(GtkButton *button);
    void BtnCancelClicked(GtkButton *button);
    void RadioManualToggled(GtkToggleButton *togglebutton);
    void EntryNumInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);

// signal connect
    static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, ViewCalcDialog *data) { return data->WindowDeleteEvent(widget, event); }
    static void on_button_ok_clicked(GtkButton *button, ViewCalcDialog *data)
    	{ data->BtnOkClicked(button); }
    static void on_button_cancel_clicked(GtkButton *button, ViewCalcDialog *data)
    	{ data->BtnCancelClicked(button); }
    static void on_radio_manual_toggled(GtkToggleButton *togglebutton, ViewCalcDialog *data)
	{ data->RadioManualToggled(togglebutton); }
    static void on_entry_manual_input(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewCalcDialog *data)
	{ data->EntryNumInsert(editable, new_text, new_text_length, position); }
};

#endif /* _VIEWCALCDIALOG_H_ */
