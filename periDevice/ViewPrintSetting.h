#ifndef _VIEWPRINTSETTING_H_
#define _VIEWPRINTSETTING_H_

#include <gtk/gtk.h>
#include "FakeXEvent.h"

class ViewPrintSetting : public FakeXEvent
{
public:
    ViewPrintSetting();
    ~ViewPrintSetting();
    static ViewPrintSetting* GetInstance();
    void CreateWindow(GtkWindow *parent);
    bool SonyURI(void);
private:
    void KeyEvent(unsigned char keyValue);
    static ViewPrintSetting* m_ptrInstance;
    void UpdateSetting(void);

    GtkWidget *m_window;
    GtkWidget *m_spinbuttonCopies;
    GtkWidget *m_spinbuttonGamma;
    GtkWidget *m_spinbuttonBrightness;
    GtkWidget *m_comboboxMedia;
    GtkWidget *m_comboboxLandscade;
    GtkWidget *m_comboboxPageSize;
    GtkWidget *m_comboboxSonyGamma;

// signal handle
    void BtnOkClicked(GtkButton *button);
    void BtnCancelClicked(GtkButton *button);
    void BtnDefaultClicked(GtkButton *button);
// signal connect
    static void on_button_ok_clicked(GtkButton *button, ViewPrintSetting *data) { data->BtnOkClicked(button); }
    static void on_button_cancel_clicked(GtkButton *button, ViewPrintSetting *data) { data->BtnCancelClicked(button); }
    static void on_button_default_clicked(GtkButton *button, ViewPrintSetting *data) { data->BtnDefaultClicked(button); }
};

#endif /* _VIEWPRINTSETTING_H_ */
