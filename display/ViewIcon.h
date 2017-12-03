// -*- c++ -*-
#ifndef _VIEW_ICON_H_
#define _VIEW_ICON_H_

#include <gtk/gtk.h>
#include <string>

using std::string;

gboolean ScrollScanIcon(gpointer data);

class ViewIcon {
public:
    ~ViewIcon();
    static ViewIcon* GetInstance(void);
    GtkWidget * GetIconArea(void);
    void Create(void);

    void Network(bool on);
    void Sound(bool on);
    void Replay(bool on);
    void Udisk(bool on);
    void Cdrom(bool on);
    void Printer(bool on);
    void Charge(int data);

    void ScanIcon(const char *iconName);
    int GetCountScanIcon(void) { return m_countScanIcon; }
    void SetCountScanIcon(int count) { m_countScanIcon = count; }
    void InitCharge();
    void UpdateCharge();
private:
    ViewIcon(void);
    static ViewIcon* m_ptrInstance;

    GtkWidget *m_table;

    GtkWidget *m_network;
    GtkWidget *m_sound;
    GtkWidget *m_replay;
    GtkWidget *m_udisk;
    GtkWidget *m_cdrom;
    //GtkWidget *m_printer;
    GtkWidget *m_charge;

    int m_countScanIcon;
    int m_timeout;
};
#endif
