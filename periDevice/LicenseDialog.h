#ifndef LICENSEDIALOG_H
#define LICENSEDIALOG_H
#include <gtk/gtk.h>
#include "display/FakeXEvent.h"

class CLicenseDialog : FakeXEvent
{
    public:
        enum WarnType {NODEVICE, EXPIRE, NOAUTHORIZE};
        static CLicenseDialog *GetInstance();
        virtual ~CLicenseDialog(){}

        void Create(WarnType type, bool isInitial);

    private:
        CLicenseDialog();
        static void ButtonOkClicked(GtkButton *button, gpointer data);
        static void ButtonCloseClicked(GtkButton *button, gpointer data);
        void KeyEvent(unsigned char keyValue);

        static CLicenseDialog *m_ptrInstance;
        GtkWidget *m_window;
        GtkWidget *label_text;
        GtkWidget *button_ok;
        GtkWidget *button_close;
        bool m_isInitial;
        bool m_preCursor;
};
#endif //LICENSEDIALOG_H