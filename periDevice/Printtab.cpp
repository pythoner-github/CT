/*
 * Printtab.cpp
 *
 *  Created on: 2012-8-6
 *      Author: yangyaoqing
 */
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "Printdefaults.h"
#include "Printtab.h"
#include "Printmain.h"
#include "Printfunctions.h"
#include "Printhpgl2.h"
#include "Printimage.h"
#include "Printoutput.h"
#include "Printppd.h"
#include "Printtext.h"

GtkWidget *Printtab::prefsFrame = NULL;
GtkWidget *Printtab::gtklpViewable = NULL;
GtkWidget *Printtab::pathesFrame = NULL;
GtkWidget *Printtab::gtklpTab = NULL;

GtkWidget *Printtab::saveSizePosCheck = NULL;
GtkWidget *Printtab::saveAnzCheck = NULL;
GtkWidget *Printtab::gtklpViewOut = NULL;
GtkWidget *Printtab::gtklpViewText = NULL;
GtkWidget *Printtab::gtklpViewImage = NULL;
GtkWidget *Printtab::gtklpViewHpgl2 = NULL;
GtkWidget *Printtab::gtklpViewSpecial = NULL;
GtkWidget *Printtab::gtklpViewPPD = NULL;
GtkWidget *Printtab::remPrinterCheck = NULL;
GtkWidget *Printtab::remTabCheck = NULL;
GtkWidget *Printtab::saveExitCheck = NULL;
//GtkWidget *Printtab::jobIDOnExitCheck = NULL;
GtkWidget *Printtab::wantconstCheck = NULL;
//GtkWidget *Printtab::clearFileListCheck = NULL;

Printtab* Printtab::m_ptrInstance = NULL;
Printtab* Printtab::GetInstance()
{
	if(m_ptrInstance == NULL)
	m_ptrInstance = new Printtab;
	return m_ptrInstance;
}

Printtab::Printtab(void)
{

}

Printtab::~Printtab(void)
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

void Printtab::GtklpTabSetDefaults(void)
{
#if 0
	/* Viewable sections */
	if(viewable[0])
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewOut), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewOut), 0);

	if(viewable[1])
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewText), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewText), 0);

	if(viewable[2])
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewImage), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewImage), 0);

	if(viewable[3])
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewHpgl2), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewHpgl2), 0);

	if(viewable[5])
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewPPD), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtklpViewPPD), 0);
	if(printerPPD == NULL) {
		gtk_widget_set_sensitive(gtklpViewPPD, FALSE);
		gtk_widget_hide(Printppd::ppdTab);
		wantconst = 0;
	}
	else
	{
		gtk_widget_set_sensitive(gtklpViewPPD, TRUE);
		gtk_widget_show(Printppd::ppdTab);
	}
#endif
	/* Prefs */
	if(saveanz)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(saveAnzCheck), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(saveAnzCheck), 0);
	if(saveonexit)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(saveExitCheck), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(saveExitCheck), 0);
	if(rememberprinter)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(remPrinterCheck), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(remPrinterCheck), 0);
	if(remembertab)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(remTabCheck), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(remTabCheck), 0);
	if(saveonexit == 0)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(saveAnzCheck), FALSE);
	}
}

void Printtab::ViewChangedOut(GtkWidget *button, gpointer data)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
	{
		viewable[0] = 1;
		gtk_widget_hide(tabs);
		gtk_widget_show(Printoutput::outputTab);
		gtk_widget_show(tabs);
	}
	else
	{
		viewable[0] = 0;
		gtk_widget_hide(Printoutput::outputTab);
	}
	return;
}

void Printtab::ViewChangedText(GtkWidget *button, gpointer data)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
	{
		viewable[1] = 1;
		gtk_widget_hide(tabs);
		gtk_widget_show(Printtext::textTab);
		gtk_widget_show(tabs);
	}
	else
	{
		viewable[1] = 0;
		gtk_widget_hide(Printtext::textTab);
	}
	return;
}

void Printtab::ViewChangedImage(GtkWidget *button, gpointer data)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
	{
		viewable[2] = 1;
		gtk_widget_hide(tabs);
		gtk_widget_show(Printimage::imageTab);
		gtk_widget_show(tabs);
	}
	else
	{
		viewable[2] = 0;
		gtk_widget_hide(Printimage::imageTab);
	}
	return;
}

void Printtab::ViewChangedHpgl2(GtkWidget *button, gpointer data)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
	{
		viewable[3] = 1;
		gtk_widget_hide(tabs);
		gtk_widget_show(Printhpgl2::hpgl2Tab);
		gtk_widget_show(tabs);
	}
	else
	{
		viewable[3] = 0;
		gtk_widget_hide(Printhpgl2::hpgl2Tab);
	}
	return;
}

void Printtab::ViewChangedPPD(GtkWidget *button, gpointer data)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
	{
		viewable[5] = 1;
		gtk_widget_hide(tabs);
		gtk_widget_show(Printppd::ppdTab);
		gtk_widget_show(tabs);
	}
	else
	{
		viewable[5] = 0;
		gtk_widget_hide(Printppd::ppdTab);
	}
	return;
}

void Printtab::GtklpViewable(GtkWidget *container)
{
	GtkWidget *mainHBox, *hbox, *vbox;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	gtklpViewable = gtk_frame_new(_("Look"));
	gtk_box_pack_start(GTK_BOX(mainHBox), gtklpViewable, TRUE, TRUE, FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(gtklpViewable), hbox);
	gtk_widget_show(hbox);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(vbox);

	/* Buttons */
	gtklpViewOut = gtk_check_button_new_with_label(_("Show Output"));
	gtk_box_pack_start(GTK_BOX(vbox), gtklpViewOut, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(gtklpViewOut), "toggled", G_CALLBACK(button_pview_changeout), this);
	gtk_widget_show(gtklpViewOut);

	gtklpViewText = gtk_check_button_new_with_label(_("Show Text"));
	gtk_box_pack_start(GTK_BOX(vbox), gtklpViewText, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(gtklpViewText), "toggled", G_CALLBACK(button_pview_changetext), this );
	gtk_widget_show(gtklpViewText);

	gtklpViewImage = gtk_check_button_new_with_label(_("Show Image"));
	gtk_box_pack_start(GTK_BOX(vbox), gtklpViewImage, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(gtklpViewImage), "toggled", G_CALLBACK(button_pview_changeimage), this);
	gtk_widget_show(gtklpViewImage);

	gtklpViewHpgl2 = gtk_check_button_new_with_label(_("Show HP-GL/2"));
	gtk_box_pack_start(GTK_BOX(vbox), gtklpViewHpgl2, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(gtklpViewHpgl2), "toggled", G_CALLBACK(button_pview_changehpgl2), this);
	gtk_widget_show(gtklpViewHpgl2);

	gtklpViewPPD = gtk_check_button_new_with_label(_("Show PPD"));
	gtk_box_pack_start(GTK_BOX(vbox), gtklpViewPPD, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(gtklpViewPPD), "toggled", G_CALLBACK(button_pview_changeppd), this);
	gtk_widget_show(gtklpViewPPD);
}

void Printtab::PrefsChanged(gpointer data)
{
	if(strcmp((char *)data, "Rem") == 0)
	{
		if(rememberprinter == 0)
			rememberprinter = 1;
		else
			rememberprinter = 0;
		return;
	}
	if(strcmp((char *)data, "Tab") == 0)
	{
		if(remembertab == 0)
			remembertab = 1;
		else
			remembertab = 0;
		return;
	}
	if(strcmp((char *)data, "Save") == 0)
	{
		if(saveonexit == 0) {
			saveonexit = 1;
			gtk_widget_set_sensitive(GTK_WIDGET(saveAnzCheck), TRUE);
		}
		else {
			saveonexit = 0;
			gtk_widget_set_sensitive(GTK_WIDGET(saveAnzCheck), FALSE);
		}
		return;
	}

	if(strcmp((char *)data, "CFL") == 0)
	{
		if(clearfilelist == 0)
			clearfilelist = 1;
		else
			clearfilelist = 0;
		return;
	}

	if(strcmp((char *)data, "JOE") == 0)
	{
		if(jobidonexit == 0)
			jobidonexit = 1;
		else
			jobidonexit = 0;
		return;
	}

	if(strcmp((char *)data, "CONS") == 0)
	{
		if(wantconst == 0)
			wantconst = 1;
		else
			wantconst = 0;
		return;
	}

	if(strcmp((char *)data, "Anz") == 0)
	{
		if(saveanz == 0)
			saveanz = 1;
		else
			saveanz = 0;
		return;
	}

	if(strcmp((char *)data, "SSP") == 0)
	{
		if(wantSaveSizePos == 0)
			wantSaveSizePos = 1;
		else
			wantSaveSizePos = 0;

		return;
	}
}

void Printtab::ConstButton(GtkWidget *widget)
{
	if(wantconst == 0) {
		wantconst = 1;
		Printfunctions::GetInstance()->Constraints(ppdConflicts(printerPPD));
		wantconst = 0;
	}
	else {
		Printfunctions::GetInstance()->Constraints(ppdConflicts(printerPPD));
	}
}

void Printtab::PrefsFrame(GtkWidget *container)
{
	GtkWidget *mainHBox, *hbox, *vbox, *constbox;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	prefsFrame = gtk_frame_new(_("Options"));
	gtk_box_pack_start(GTK_BOX(mainHBox), prefsFrame, TRUE, TRUE, FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(prefsFrame), hbox);
	gtk_widget_show(hbox);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(vbox);

	/* Buttons */
	remPrinterCheck = gtk_check_button_new_with_label(_("Remember last printer"));
	gtk_box_pack_start(GTK_BOX(vbox), remPrinterCheck, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(remPrinterCheck), "pressed", G_CALLBACK(button_prefs_change), (void *)"Rem");
	gtk_widget_show(remPrinterCheck);

	remTabCheck = gtk_check_button_new_with_label(_("Remember last Tab"));
	gtk_box_pack_start(GTK_BOX(vbox), remTabCheck, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(remTabCheck), "pressed", G_CALLBACK(button_prefs_change), (void *)"Tab");
	gtk_widget_show(remTabCheck);

	saveExitCheck = gtk_check_button_new_with_label(_("Save all preferences on exit"));
	gtk_box_pack_start(GTK_BOX(vbox), saveExitCheck, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(saveExitCheck), "pressed", G_CALLBACK(button_prefs_change), (void *)"Save");
	gtk_widget_show(saveExitCheck);

	saveAnzCheck = gtk_check_button_new_with_label(_("Save number of copies on exit"));
	gtk_box_pack_start(GTK_BOX(vbox), saveAnzCheck, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(saveAnzCheck), "pressed", G_CALLBACK(button_prefs_change), (void *)"Anz");
	gtk_widget_show(saveAnzCheck);

//	clearFileListCheck = gtk_check_button_new_with_label(_("Clear file list on print"));
//	gtk_box_pack_start(GTK_BOX(vbox), clearFileListCheck, FALSE, FALSE, 0);
//	g_signal_connect(G_OBJECT(clearFileListCheck), "pressed", G_CALLBACK(button_prefs_change), (void *)"CFL");
//	gtk_widget_show(clearFileListCheck);

	constbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), constbox, TRUE, TRUE, 0);
	gtk_widget_show(constbox);

}

GtkWidget* Printtab::GtklpTabFrame(GtkWidget *container)
{
	GtkWidget *frameScrolled;
	GtkWidget *hbox, *vbox;

	frameScrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(frameScrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(container), frameScrolled, TRUE, TRUE, 0);
	gtk_widget_show(frameScrolled);

	hbox=gtk_hbox_new(FALSE, 0);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(frameScrolled), hbox);
	gtk_widget_show(hbox);

	vbox=gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(vbox);

	return(vbox);
}

void Printtab::GtkPrinterTab(void)
{
	gtklpTab=gtk_vbox_new(FALSE, 0);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), gtklpTab, gtk_label_new(_("PrintConfig")));

	GtkWidget *frame;
	frame = GtklpTabFrame(gtklpTab);
	PrefsFrame(frame);
}

void Printtab::ButtonPrefsChange(GtkWidget *button, gpointer data)
{
	PrefsChanged(data);
}

void Printtab::ButtonConstButton(GtkWidget *button, GtkWidget *widget)
{
	ConstButton(widget);
}

