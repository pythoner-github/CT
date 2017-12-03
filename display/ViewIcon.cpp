#include <gtk/gtk.h>

#include "Def.h"
#include "gui_global.h"
#include "gui_func.h"
#include "ViewIcon.h"
#include "Battery.h"

namespace {
const char* ScanIconPath[5] = {
    "res/icon/scan-00.png", 
    "res/icon/scan-25.png", 
    "res/icon/scan-50.png", 
    "res/icon/scan-75.png", 
    "res/icon/scan-100.png",
};

const char* NetworkIconPath[2] = {
    "res/icon/network-offline.png",
    "res/icon/network-idle.png",
};

const char* SoundIconPath[2] = {
    "res/icon/audio-muted.png",
    "res/icon/audio-on.png",
};

const char* ChargeIconPath[7] = {
    "res/icon/charge0.jpg", 
    "res/icon/charge1.jpg", 
    "res/icon/charge2.jpg", 
    "res/icon/charge3.jpg", 
    "res/icon/charge4.jpg", 
    "res/icon/charge5.jpg", 
    "res/icon/charge.jpg", 
};

const char* ReplayIconPath = "res/icon/replay.png";
const char* FlashIconPath = "res/icon/flashkey.png";
const char* CdromIconPath = "res/icon/cdrom.png";
//const char* PrinterIconPath = "res/icon/printer.png";
}

ViewIcon* ViewIcon::m_ptrInstance = NULL;

ViewIcon::ViewIcon(void)
{
    m_countScanIcon = 0;
    m_timeout = 0;
}

ViewIcon::~ViewIcon()
{
    if (m_ptrInstance != NULL)
	delete m_ptrInstance;
}

ViewIcon* ViewIcon::GetInstance(void)
{
    if (m_ptrInstance == NULL)
	    m_ptrInstance = new ViewIcon;
    return m_ptrInstance;
}

void ViewIcon::Create(void)
{
	const int num = 6;
    m_table = gtk_table_new(1, num, TRUE);
//    gtk_widget_set_size_request (m_table, 180, 32);
    gtk_widget_set_size_request (m_table, 26*num, 24);
    gtk_table_set_col_spacings (GTK_TABLE (m_table), 5);

    m_network = create_pixmap (NetworkIconPath[0]);
    gtk_table_attach_defaults (GTK_TABLE (m_table), m_network, 0, 1, 0, 1);

    m_sound = create_pixmap (SoundIconPath[0]);
    gtk_table_attach_defaults (GTK_TABLE (m_table), m_sound, 1, 2, 0, 1);

    m_replay = create_pixmap (NULL); //(ScanIconPath[0]);
    gtk_table_attach_defaults (GTK_TABLE (m_table), m_replay, 2, 3, 0, 1);

    m_udisk = create_pixmap (NULL); //(FlashIconPath);
    gtk_table_attach_defaults (GTK_TABLE (m_table), m_udisk, 3, 4, 0, 1);

    m_cdrom = create_pixmap (NULL); //(CdromIconPath);
    gtk_table_attach_defaults (GTK_TABLE (m_table), m_cdrom, 4, 5, 0, 1);

    m_charge = create_pixmap (NULL); //(CdromIconPath);
    gtk_table_attach_defaults (GTK_TABLE (m_table), m_charge, 5, 6, 0, 1);
#if 0
    m_printer = create_pixmap(NULL);
    gtk_table_attach_defaults (GTK_TABLE (m_table), m_printer, 5, 6, 0, 1);
#endif

#ifdef EMP_355
    InitCharge();
#endif

}

GtkWidget * ViewIcon::GetIconArea(void)
{
    return m_table;
}

void ViewIcon::ScanIcon(const char *iconName)
{
    gtk_image_set_from_file(GTK_IMAGE(m_replay), iconName);
}

gboolean ScrollScanIcon(gpointer data)
{
    char path[256];
    ViewIcon *tmp;
    tmp = (ViewIcon *)data;
    int count = tmp->GetCountScanIcon();
    switch (count) {
    case 0:
        sprintf(path, "%s/%s", CFG_RES_PATH, ScanIconPath[0]);
        break;
    case 1:
        sprintf(path, "%s/%s", CFG_RES_PATH, ScanIconPath[1]);
        break;
    case 2:
        sprintf(path, "%s/%s", CFG_RES_PATH, ScanIconPath[2]);
        break;
    case 3:
        sprintf(path, "%s/%s", CFG_RES_PATH, ScanIconPath[3]);
        break;
    case 4:
        sprintf(path, "%s/%s", CFG_RES_PATH, ScanIconPath[4]);
        break;
    }
    tmp->ScanIcon(path);

    if (count < 4)
	count++;
    else
	count = 0;

    tmp->SetCountScanIcon(count);
    return TRUE;
}

void ViewIcon::Replay(bool on)
{
    if (on) {
        if (m_timeout > 0) {
            g_source_remove(m_timeout);
            m_timeout = 0;
        }
        ScanIcon(ReplayIconPath);
    }
    else {
        if(m_timeout == 0)
            m_timeout = g_timeout_add(500, ScrollScanIcon, this);
    }
}

void ViewIcon::Network(bool on)
{
    char path[256];
    if (on)
        sprintf(path, "%s/%s", CFG_RES_PATH, NetworkIconPath[1]);
    else
        sprintf(path, "%s/%s", CFG_RES_PATH, NetworkIconPath[0]);
    gtk_image_set_from_file(GTK_IMAGE(m_network), path);
}

void ViewIcon::Sound(bool on)
{
    char path[256];
    if (on)
        sprintf(path, "%s/%s", CFG_RES_PATH, SoundIconPath[1]);
    else
        sprintf(path, "%s/%s", CFG_RES_PATH, SoundIconPath[0]);
    gtk_image_set_from_file(GTK_IMAGE(m_sound), path);
}

void ViewIcon::Udisk(bool on)
{
    char path[256];
    if (on) {
        sprintf(path, "%s/%s", CFG_RES_PATH, FlashIconPath);
        gtk_image_set_from_file(GTK_IMAGE(m_udisk), path);
    }
    else 
        gtk_image_set_from_file(GTK_IMAGE(m_udisk), NULL);
}

void ViewIcon::Cdrom(bool on)
{
    char path[256];
    if (on) {
        sprintf(path, "%s/%s", CFG_RES_PATH, CdromIconPath);
        gtk_image_set_from_file(GTK_IMAGE(m_cdrom), path);
    }
    else 
        gtk_image_set_from_file(GTK_IMAGE(m_cdrom), NULL);

}

void ViewIcon::Printer(bool on)
{
#if 0
    char path[256];
    if (on) {
        sprintf(path, "%s/%s", CFG_RES_PATH, PrinterIconPath);
        gtk_image_set_from_file(GTK_IMAGE(m_printer), path);
    }
    else 
        gtk_image_set_from_file(GTK_IMAGE(m_printer), NULL);
#endif
}

void ViewIcon::Charge(int data)
{   
    char path[256];
    int value = 80 / 4;       
    int count = 0;           
    if (data < 10)          
        count = 0;
    else if (data > 90)    
        count = 5;        
    else
        count = 1 + (data - 10) / value;      
    PRINTF("--count =%d\n", count);          
    switch (count) {                        
        case 0:
            sprintf(path, "%s/%s", CFG_RES_PATH, ChargeIconPath[0]);     
            break;                                                      
        case 1:
            sprintf(path, "%s/%s", CFG_RES_PATH, ChargeIconPath[1]);   
            break;                                                    
        case 2:
            sprintf(path, "%s/%s", CFG_RES_PATH, ChargeIconPath[2]); 
            break;                                                  
        case 3:
            sprintf(path, "%s/%s", CFG_RES_PATH, ChargeIconPath[3]);   
            break;                                                    
        case 4:
            sprintf(path, "%s/%s", CFG_RES_PATH, ChargeIconPath[4]); 
            break;                                                  
        case 5:
            sprintf(path, "%s/%s", CFG_RES_PATH, ChargeIconPath[5]); 
            break;   

    }           

    gtk_image_set_from_file(GTK_IMAGE(m_charge), path);
}

void ViewIcon::UpdateCharge()
{
    Battery battery;
    Charge(battery.GetCapacity());   
}


gboolean CallBackUpdateCharge(gpointer data)
{
    ViewIcon *tmp;
    tmp = (ViewIcon *)data;
    tmp->UpdateCharge();
    return true;
}

void ViewIcon::InitCharge()
{
    UpdateCharge();
    g_timeout_add( 6000, CallBackUpdateCharge, this );
}


