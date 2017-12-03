#include "DicomLocalSetting.h"
#include "gui_global.h"
#include "gui_func.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include "DCMMan.h"
#include "SysDicomSetting.h"
#include "ViewDialog.h"
#include "NetworkMan.h"
#include "ViewSystem.h"

DicomLocalSetting* DicomLocalSetting::m_ptrInstance = NULL;

DicomLocalSetting::DicomLocalSetting()
{
}

DicomLocalSetting::~DicomLocalSetting()
{
}

DicomLocalSetting* DicomLocalSetting::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new DicomLocalSetting;

    return m_ptrInstance;
}

GtkWidget* DicomLocalSetting::CreateDicomWindow(GtkWidget *parent)
{
    GtkWidget *fixed_local;
    GtkWidget *frame_network;
    GtkWidget *table_network;
    GtkWidget *label_network;

    GtkWidget *label_network_ip;
    GtkWidget *ip_style;
    GtkWidget *label_network_mask;
    GtkWidget *mask_style;
    GtkWidget *label_network_gateway;
    GtkWidget *gateway_style;

    GtkWidget *button_setting;

    GtkWidget *frame_host;
    GtkWidget *label_host;
    GtkWidget *table_host;
    GtkWidget *label_host_port;
    GtkWidget *label_host_ae;

    fixed_local = gtk_fixed_new ();
    gtk_widget_show (fixed_local);

    frame_network = gtk_frame_new (NULL);
    gtk_widget_show (frame_network);
    gtk_fixed_put (GTK_FIXED (fixed_local), frame_network, 20, 10);
    gtk_widget_set_size_request (frame_network, 330, 180+100);
    gtk_frame_set_label_align (GTK_FRAME (frame_network), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_network), GTK_SHADOW_IN);

    label_network = gtk_label_new (_("<b>NetWork Property</b>"));
    gtk_widget_show (label_network);
    gtk_frame_set_label_widget (GTK_FRAME (frame_network), label_network);
    gtk_label_set_use_markup (GTK_LABEL (label_network), TRUE);

    table_network = gtk_table_new (6, 5, FALSE);
    gtk_widget_show (table_network);
    gtk_container_add (GTK_CONTAINER (frame_network), table_network);
    gtk_container_set_border_width (GTK_CONTAINER (table_network), 8);
    gtk_table_set_row_spacings (GTK_TABLE (table_network), 5);

    m_entry_network_ip = gtk_entry_new ();
    gtk_widget_show (m_entry_network_ip);
    //gtk_table_attach_defaults (GTK_TABLE (table_network), m_entry_network_ip, 1, 3, 0, 1);
    gtk_table_attach (GTK_TABLE (table_network), m_entry_network_ip, 1, 3, 0, 1,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

    gtk_widget_set_size_request (m_entry_network_ip, -1, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_network_ip), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_network_ip), 9679);

    ip_style = gtk_label_new (_("<b>(xxx.xxx.xxx.xxx)</b>"));
    gtk_widget_show (ip_style);
    //gtk_table_attach_defaults (GTK_TABLE (table_network), label_network_ip, 0, 1, 0, 1);
    gtk_table_attach (GTK_TABLE (table_network), ip_style, 1, 3, 1, 2,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (ip_style), TRUE);

    m_entry_network_mask = gtk_entry_new ();
    gtk_widget_show (m_entry_network_mask);

    //gtk_table_attach_defaults(GTK_TABLE (table_network), m_entry_network_mask, 1, 3, 1, 2);
    gtk_table_attach (GTK_TABLE (table_network), m_entry_network_mask, 1, 3, 2, 3,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

    gtk_widget_set_size_request (m_entry_network_mask, -1, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_network_mask), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_network_mask), 9679);
#if 1
    mask_style = gtk_label_new (_("<b>(xxx.xxx.xxx.xxx)</b>"));
    gtk_widget_show (mask_style);
    //gtk_table_attach_defaults (GTK_TABLE (table_network), label_network_ip, 0, 1, 0, 1);
    gtk_table_attach (GTK_TABLE (table_network), mask_style, 1, 3,3 , 4,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (mask_style), TRUE);
#endif
    m_entry_network_gateway = gtk_entry_new ();
    gtk_widget_show (m_entry_network_gateway);
    //gtk_table_attach_defaults (GTK_TABLE (table_network), m_entry_network_gateway, 1, 3, 2, 3);
    gtk_table_attach (GTK_TABLE (table_network), m_entry_network_gateway, 1,3 , 4, 5,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

    gtk_widget_set_size_request (m_entry_network_gateway, -1, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_network_gateway), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_network_gateway), 9679);
#if 1
    gateway_style = gtk_label_new (_("<b>(xxx.xxx.xxx.xxx)</b>"));
    gtk_widget_show (gateway_style);
    //gtk_table_attach_defaults (GTK_TABLE (table_network), label_network_ip, 0, 1, 0, 1);
    gtk_table_attach (GTK_TABLE (table_network), gateway_style, 1, 3,5, 6,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (gateway_style), TRUE);
#endif
    label_network_ip = gtk_label_new (_("<b>IP Address:     </b>"));
    gtk_widget_show (label_network_ip);
    //gtk_table_attach_defaults (GTK_TABLE (table_network), label_network_ip, 0, 1, 0, 1);
    gtk_table_attach (GTK_TABLE (table_network), label_network_ip, 0, 1, 0, 1,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_network_ip), TRUE);

    label_network_mask = gtk_label_new (_("<b>Netmask :</b>"));
    gtk_widget_show (label_network_mask);
    //gtk_table_attach_defaults (GTK_TABLE (table_network), label_network_mask, 0, 1, 1, 2);
    gtk_table_attach (GTK_TABLE (table_network), label_network_mask, 0, 1, 2, 3,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

    gtk_label_set_use_markup (GTK_LABEL (label_network_mask), TRUE);

    label_network_gateway = gtk_label_new (_("<b>Gateway :</b>"));
    gtk_widget_show (label_network_gateway);
    //gtk_table_attach_defaults (GTK_TABLE (table_network), label_network_gateway, 0, 1, 2, 3);
    gtk_table_attach (GTK_TABLE (table_network), label_network_gateway, 0, 1, 4, 5,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

    gtk_label_set_use_markup (GTK_LABEL (label_network_gateway), TRUE);

    button_setting = gtk_button_new_with_mnemonic (_("Setting"));
    gtk_widget_show (button_setting);
    gtk_fixed_put (GTK_FIXED (fixed_local), button_setting, 420+10, 230+10);
    gtk_widget_set_size_request (button_setting, 85, 35);
    //gtk_table_attach_defaults (GTK_TABLE (table_network), button_setting, 2,3 ,3, 4);
    g_signal_connect(button_setting, "clicked", G_CALLBACK(HandleButtonSettingClicked), this);
    
    //Host
    frame_host = gtk_frame_new (NULL);
    gtk_widget_show (frame_host);
    // gtk_fixed_put (GTK_FIXED (fixed_dicom), frame_host, 400, 30);
    gtk_fixed_put (GTK_FIXED (fixed_local), frame_host, 370+20, 10);

    gtk_widget_set_size_request (frame_host, 375, 180);
    gtk_frame_set_label_align (GTK_FRAME (frame_host), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_host), GTK_SHADOW_IN);

    label_host = gtk_label_new (_("<b>Host</b>"));
    gtk_widget_show (label_host);
    gtk_frame_set_label_widget (GTK_FRAME (frame_host), label_host);
    gtk_label_set_use_markup (GTK_LABEL (label_host), TRUE);

    table_host = gtk_table_new (3, 3, TRUE);
    gtk_widget_show (table_host);
    gtk_container_add (GTK_CONTAINER (frame_host), table_host);
    gtk_container_set_border_width (GTK_CONTAINER (table_host), 8);
    gtk_table_set_row_spacings (GTK_TABLE (table_host), 5);

    label_host_ae = gtk_label_new (_("<b>AE Title :</b>"));
    gtk_widget_show (label_host_ae);
    gtk_table_attach (GTK_TABLE (table_host), label_host_ae, 0, 1, 0, 1,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_host_ae), TRUE);

    m_entry_host_ae = gtk_entry_new ();
    gtk_widget_show (m_entry_host_ae);
    gtk_table_attach (GTK_TABLE (table_host), m_entry_host_ae, 1,2 , 0, 1,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_widget_set_size_request (m_entry_host_ae, -1, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_host_ae), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_host_ae), 9679);

    label_host_port = gtk_label_new (_("<b>Port :</b>"));
    gtk_widget_show (label_host_port);
    gtk_table_attach (GTK_TABLE (table_host), label_host_port, 0, 1, 1, 2,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_host_port), TRUE);

    m_entry_host_port = gtk_entry_new ();
    gtk_widget_show (m_entry_host_port);
    gtk_table_attach (GTK_TABLE (table_host), m_entry_host_port, 1, 2, 1, 2,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_widget_set_size_request (m_entry_host_port, -1, 30);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_host_port), 15);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_host_port), 9679);
    g_signal_connect(m_entry_host_port, "key-press-event", G_CALLBACK(HandleKeyFilter), this);

    return fixed_local;

}

void DicomLocalSetting::init_local_setting(void)
{
    SysDicomSetting *sysDicomSetting = new SysDicomSetting;
    string local_ip, local_netmask, local_gateway, local_AE;
    int host_port;
    string host_ae;
    char tmp[256];

    sysDicomSetting->GetLocalIP(local_ip);
    gtk_entry_set_text(GTK_ENTRY(m_entry_network_ip), local_ip.c_str());
    sysDicomSetting->GetLocalNetMask(local_netmask);
    gtk_entry_set_text(GTK_ENTRY(m_entry_network_mask), local_netmask.c_str());
    sysDicomSetting->GetLocalGateWay(local_gateway);
    gtk_entry_set_text(GTK_ENTRY(m_entry_network_gateway), local_gateway.c_str());

    host_port = CDCMMan::GetMe()->GetLocalPort();
    sprintf(tmp,"%d",host_port);
    gtk_entry_set_text(GTK_ENTRY(m_entry_host_port), tmp);
    host_ae = CDCMMan::GetMe()->GetLocalAE();
    gtk_entry_set_text(GTK_ENTRY(m_entry_host_ae), host_ae.c_str());
    delete sysDicomSetting;
}

void DicomLocalSetting::ButtonSettingClicked(GtkButton *button)
{
    char info[256];
    const char *local_ip, *local_mask, *local_gateway, *local_ae, *host_port, *host_ae;

    local_ip = (const char *)gtk_entry_get_text(GTK_ENTRY(m_entry_network_ip));
    local_mask = (const char *)gtk_entry_get_text(GTK_ENTRY(m_entry_network_mask));
    local_gateway = (const char *)gtk_entry_get_text(GTK_ENTRY(m_entry_network_gateway));
    host_port = (const char *)gtk_entry_get_text(GTK_ENTRY(m_entry_host_port));
    host_ae = (const char *)gtk_entry_get_text(GTK_ENTRY(m_entry_host_ae));

    printf("------Setting:%s %s %s %s %s\n", local_ip,local_mask,local_gateway,host_port,host_ae);

    //if(host_ae[0] == '\0' || host_port[0] == '\0' || local_ip[0] == '\0' || local_mask[0] == '\0' || local_ae[0] == '\0')
    if (strcmp(local_ip,"")==0 ||strcmp(local_mask,"")==0 ||strcmp(local_gateway,"")==0 ||strcmp(host_port,"")==0 ||strcmp(host_ae,"")==0)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), ViewDialog::ERROR, _("Local information must be not empty!"), NULL);
        return ;
    }

    CNetworkMan localIP;
    if(localIP.SetLocalIP(local_ip, local_gateway, local_mask))
    {
        CDCMMan::GetMe()->SetLocalAE(host_ae);
        CDCMMan::GetMe()->SetLocalPort(atoi(host_port));
        sprintf(info, _("Sucess to set the local network!"));

        SysDicomSetting sysDicomSetting;
        sysDicomSetting.SetLocalIP(local_ip);
        sysDicomSetting.SetLocalNetMask(local_mask);
        sysDicomSetting.SetLocalGateWay(local_gateway);

        sysDicomSetting.SyncFile();

    }
    else
    {
        PRINTF("Fail to set the local network!\n");
        sprintf(info, _("Fail to set the local network!"));
    }
   
    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), 
            ViewDialog::INFO, 
            info,
            NULL);
    
    }

gboolean DicomLocalSetting::KeyFilter(GtkWidget *entry, GdkEventKey *event)
{
        switch(event->keyval)
        {
                case GDK_0:
                case GDK_1:
                case GDK_2:
                case GDK_3:
                case GDK_4:
                case GDK_5:
                case GDK_6:
                case GDK_7:
                case GDK_8:
                case GDK_9:
                case GDK_KP_0:
                case GDK_KP_1:
                case GDK_KP_2:
                case GDK_KP_3:
                case GDK_KP_4:
                case GDK_KP_5:
                case GDK_KP_6:
                case GDK_KP_7:
                case GDK_KP_8:
                case GDK_KP_9:
                case GDK_BackSpace:
                case GDK_Left:
                case GDK_Up:
                case GDK_Right:
                case GDK_Down:
                case GDK_Delete:
                case GDK_Home:
                case GDK_End:
				case GDK_Tab:
                        return FALSE;
                default:
                        return TRUE;
        }
}
