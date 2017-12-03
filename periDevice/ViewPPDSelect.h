#ifndef _VIEWPPDSELECT_H_
#define _VIEWPPDSELECT_H_

#include <gtk/gtk.h>

class FakeXEvent;
class ViewPPDSelect : public FakeXEvent
{
public:
    ~ViewPPDSelect();
    static ViewPPDSelect* GetInstance();
    void CreateWindow(GtkWindow *parent);
    void DestroyWindow(void);
private:
    ViewPPDSelect(void);
    static ViewPPDSelect* m_ptrInstance;
    void KeyEvent(unsigned char KeyValue);
    GtkWidget* m_window;
    GtkTreeSelection *m_selected_ppd;

    enum {
        NAME,
        PATH,
		SIZE,
		MODIFIED,
        NUM_COLS
    };

    void update_tree_model(void);
    GtkTreeModel* create_tree_model(void);
    GtkWidget* create_treeview(void);

// signal handle
    gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
    void BtnOpenClicked(GtkButton *button);
    void BtnCancelClicked(GtkButton *button);

// signal connect
    static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, ViewPPDSelect *data) { return data->WindowDeleteEvent(widget, event); }
    static void on_button_open_clicked(GtkButton *button, ViewPPDSelect *data) { data->BtnOpenClicked(button); }
    static void on_button_cancel_clicked(GtkButton *button, ViewPPDSelect *data) { data->BtnCancelClicked(button); }
};

#endif /* _VIEWPPDSELECT_H_ */
