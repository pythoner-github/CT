#include <stdio.h>
#include <stdlib.h>
#include "DicomSend.h"
#include <glib/gi18n.h>

#define SCALE_WIDTH  1.25
#define SCALE_HEIGHT 1.33

DicomSend *DicomSend::m_pInstance = NULL;

DicomSend::DicomSend()
{
	m_menu = gtk_menu_new();
}
void DicomSend::destroyItem()
{
	int i, total;


	//gtk_widget_destroy(m_menu);
	total = m_vecItem.size();
	for(i=0; i<total; i++)
		gtk_widget_destroy(m_vecItem[i]);
	m_vecItem.clear();

}
DicomSend::~DicomSend()
{
	int i, total;
	if(NULL != m_pInstance)
	{
		delete m_pInstance;

	}

	gtk_widget_destroy(m_menu);
	total = m_vecItem.size();
	for(i=0; i<total; i++)
		gtk_widget_destroy(m_vecItem[i]);
	m_vecItem.clear();

}

DicomSend *DicomSend::GetInstance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new DicomSend();
	}
	return m_pInstance;
}

void DicomSend::createLabelButton(GtkWidget *fixedWin)
{
	GtkWidget *labelExport;
	GtkWidget *imgExport;
	GtkWidget *btnExport;

 	labelExport = gtk_label_new_with_mnemonic (_("Export Data"));
        imgExport = gtk_image_new_from_stock (GTK_STOCK_SAVE, GTK_ICON_SIZE_BUTTON);
        btnExport = create_button_icon(labelExport, imgExport);
        gtk_fixed_put (GTK_FIXED (fixedWin), btnExport, 20*SCALE_WIDTH, (550-30)*SCALE_HEIGHT);
       g_signal_connect(G_OBJECT(btnExport), "event", G_CALLBACK(DicomSend::HandleDicomBtnExportEvent), this);


}
GtkWidget* DicomSend::create_button_icon(GtkWidget *label, GtkWidget *icon_image)
{
#if 0
    int size =0;
        if(CDCMRegister::GetMe()->IsAuthorize())
        size = 20;
    else
        size = 0;
#endif
int size =0;
size = 20;
    GtkWidget *button = gtk_button_new();
    gtk_widget_set_size_request (button, 120*SCALE_WIDTH, (70-size)*SCALE_HEIGHT);

    GtkWidget *alignment = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_container_add (GTK_CONTAINER (button), alignment);

    GtkWidget *hbox = gtk_hbox_new (FALSE, 2);
    gtk_container_add (GTK_CONTAINER (alignment), hbox);
    gtk_box_pack_start (GTK_BOX (hbox), icon_image, FALSE, FALSE, 0);
	if (label)
	    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);

	gtk_button_set_focus_on_click(GTK_BUTTON(button), FALSE);
    gtk_widget_show_all(button);

    return button;
}
void DicomSend::createPopup(char *label,HandleMenuItemCDActivate_t HandleMenuItemFlashActivate)
{
	m_itemFlash = gtk_menu_item_new_with_label(_(label));
	gtk_menu_shell_append(GTK_MENU_SHELL(m_menu), m_itemFlash);
	g_signal_connect(G_OBJECT (m_itemFlash), "activate", G_CALLBACK(HandleMenuItemFlashActivate), this);
	gtk_widget_show(m_itemFlash);
	m_vecItem.push_back(m_itemFlash);


}
int DicomSend::BtnExportEvent(GtkWidget *widget, GdkEvent *event)
{
	if (event->type == GDK_BUTTON_PRESS) {
		GdkEventButton *bevent = (GdkEventButton *) event;
		gtk_menu_popup (GTK_MENU (m_menu), NULL, NULL, NULL, NULL,
				bevent->button, bevent->time);

		return TRUE;
	}
	return FALSE;
}
 int DicomSend::HandleDicomBtnExportEvent(GtkWidget *widget, GdkEvent *event, DicomSend *data)
{
	return DicomSend::GetInstance()->BtnExportEvent(widget, event);
 }

