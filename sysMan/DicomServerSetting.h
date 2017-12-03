#ifndef _DICOM_SERVER_SETTING_H_
#define _DICOM_SERVER_SETTING_H_

#include <gtk/gtk.h>
#include "FakeXEvent.h"
#include <string.h>
#include <stdio.h>
#include "DCMMan.h"
#include <vector>


class DicomServerSetting
{
    public:
        
        static DicomServerSetting* GetInstance();
        GtkWidget* CreateDicomWindow(GtkWidget *parent);
        
        enum{
            COL_DEVICE,
            COL_IP,
            NUM_COLS
        };

        void init_server_setting(void);

        vector<string> m_group_device_server;
        vector<string> m_group_ip_server;
    private:
        DicomServerSetting();
        ~DicomServerSetting();

        static DicomServerSetting* m_ptrInstance;
        
        //server
        GtkWidget *m_entry_device;
        GtkWidget *m_entry_ip;
        GtkWidget *m_treeview;

        GtkWidget* create_server_treeview();
        GtkTreeModel* create_device_model();

        void ButtonPingClicked(GtkButton *button);
        void ButtonAddClicked(GtkButton *button);
        void ButtonDeleteClicked(GtkButton *button);
        
        static void HandleButtonPingClicked(GtkButton *button, DicomServerSetting *data)
        {
            if (data)
                data->ButtonPingClicked(button);
        }
        
        static void HandleButtonAddClicked(GtkButton *button, DicomServerSetting *data)
        {
            if (data)
                data->ButtonAddClicked(button);
        }
        
        static void HandleButtonDeleteClicked(GtkButton *button, DicomServerSetting *data)
        {
            if (data)
                data->ButtonDeleteClicked(button);
        }

        static void GetSingleServerAttribute(string device, string ip, void *data);

        static gboolean PingTimeout(gpointer data);
};
#endif
