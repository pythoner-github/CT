#ifndef VIEW_DICOM_SUPERVISE_H
#define VIEW_DICOM_SUPERVISE_H

#include <gtk/gtk.h>
#include <vector>
#include <string>
#include "FakeXEvent.h"

class ViewDicomSupervise:public FakeXEvent
{
    public:
        ~ViewDicomSupervise();
        static ViewDicomSupervise* GetInstance();
        void Authenticate(void);
        //GtkWidget* CreateWindow(GtkWidget *parent);
        void CreateWindow();
        void DestroyWindow(void);
    private:
        ViewDicomSupervise();
        static ViewDicomSupervise* m_ptrInstance;

        static std::string noticeInfo;
        static std::string noticeInfo1;
        static const unsigned int AUTHEN_NUM = 5;
        GtkWidget *m_window;
        GtkWidget *m_labelExport;
        GtkWidget *m_labelRegister;
        // anthen
        std::vector<unsigned char> m_vecAuthenInfo;
        bool m_statusAuthen;
        int m_timer;
        GtkWidget *m_entry_key;
        bool m_flagDICOM;
        void ConfirmAuthen(void);
        void Exit(void);

        virtual void KeyEvent(unsigned char keyValue);
        gboolean IsAuthenValid(void);
        gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
        void BtnExportClicked(GtkButton *button);
        void BtnRegisterClicked(GtkButton *button);
        void BtnExitClicked(GtkButton *button);

        static gboolean HandleAuthen(gpointer data) { return m_ptrInstance->IsAuthenValid(); }
    static gboolean HandleWindowDeleteEvent(GtkWidget *widget, GdkEvent *event, ViewDicomSupervise *data) { return data->WindowDeleteEvent(widget, event); }
    static void HandleBtnExportClicked(GtkButton *widget, ViewDicomSupervise *data) { return data->BtnExportClicked(widget); }
    static void HandleBtnRegisterClicked(GtkButton *widget, ViewDicomSupervise *data) { return data->BtnRegisterClicked(widget); }
    static void HandleBtnExitClicked(GtkButton *widget, ViewDicomSupervise *data) { return data->BtnExitClicked(widget); }
};
#endif
