#ifndef _VIEWPRINTADD_H_
#define _VIEWPRINTADD_H_

#include <gtk/gtk.h>
#include "FakeXEvent.h"
#include <string>

class ViewPrinterAdd : public FakeXEvent
{
public:
    ~ViewPrinterAdd();
    static ViewPrinterAdd* GetInstance();
    void CreateWindow(GtkWindow *parent);
    void DestroyWindow(void);
    void SetPPDName(const char *name, const char* path);
private:
    ViewPrinterAdd();
    static ViewPrinterAdd* m_ptrInstance;
    void KeyEvent(unsigned char KeyValue);
    GtkWidget* m_window;
    GtkWidget* m_entry_printer_name;
    GtkWidget *m_labelPPDName;

    std::string m_ppdFileName;
    std::string m_uriName;

// signal handle
    gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
    void BtnAddClicked(GtkButton *button);
    void BtnCancelClicked(GtkButton *button);
    void BtnPPDNameClicked(GtkButton *button);
// signal connect
    static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, ViewPrinterAdd *data) { return data->WindowDeleteEvent(widget, event); }
    static void on_button_add_clicked(GtkButton *button, ViewPrinterAdd *data) { data->BtnAddClicked(button); }
    static void on_button_cancel_clicked(GtkButton *button, ViewPrinterAdd *data) { data->BtnCancelClicked(button); }
    static void on_button_ppd_file_clicked(GtkButton *button, ViewPrinterAdd *data) { data->BtnPPDNameClicked(button); }
};

#endif /* _VIEWPRINTADD_H_ */
