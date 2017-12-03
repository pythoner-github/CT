#ifndef _DICOM_LOCAL_SETTING_H_
#define _DICOM_LOCAL_SETTING_H_

#include <gtk/gtk.h>
#include "FakeXEvent.h"
#include <string.h>
#include <stdio.h>

class DicomLocalSetting
{
    public:

        static DicomLocalSetting* GetInstance();
        GtkWidget* CreateDicomWindow(GtkWidget *parent);
        
        void init_local_setting(void);
    private:
        DicomLocalSetting();
        ~DicomLocalSetting();

        static DicomLocalSetting* m_ptrInstance;
        
        //Local
        GtkWidget *m_entry_network_ip;
        GtkWidget *m_entry_network_mask;
        GtkWidget *m_entry_network_gateway;

        GtkWidget *m_entry_host_port;
        GtkWidget *m_entry_host_ae;


        void ButtonSettingClicked(GtkButton *button);
        gboolean KeyFilter(GtkWidget *entry, GdkEventKey *event);

        static void HandleButtonSettingClicked(GtkButton *button, DicomLocalSetting *data)
        {
            if (data)
                data->ButtonSettingClicked(button);
        }
        
        static gboolean HandleKeyFilter(GtkWidget *entry, GdkEventKey *event, gpointer data)
        {
            ((DicomLocalSetting *)data)->KeyFilter(entry, event);
        }
};
#endif
