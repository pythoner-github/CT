/*
 * Printoutput.cpp
 *
 *  Created on: 2012-8-6
 *      Author: yangyaoqing
 */
#include <stdio.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include "Printoutput.h"
#include "Printmain.h"
#include "Printfunctions.h"

//GtkObject *Printoutput::brightAdj  =  NULL;
//GtkObject *Printoutput::gammaAdj = NULL;
GtkWidget *Printoutput::rangesFrame = NULL;
GtkWidget *Printoutput::brightFrame = NULL;
GtkWidget *Printoutput::gammaFrame = NULL;
GtkWidget *Printoutput::sheetsFrame = NULL;
GtkWidget *Printoutput::outputTab = NULL;

GSList *Printoutput::sheetsButtons = NULL;
GSList *Printoutput::nupLayButtons = NULL;
GSList *Printoutput::borderButtons = NULL;
GSList *Printoutput::orientButtons = NULL;
GtkWidget *Printoutput::nupCombo = NULL;
GtkWidget *Printoutput::nupStyleButton = NULL;
int Printoutput::nupStyle = 0;
GtkWidget *Printoutput::nupPixMapWid = NULL;
GSList *Printoutput::rangeButtons = NULL;
GtkWidget *Printoutput::rangeCheck = NULL;
GtkWidget *Printoutput::rangeText = (GtkWidget  *)NULL;
GtkWidget *Printoutput::landscapeCheck = NULL;
GtkWidget *Printoutput::mirrorCheck = NULL;
GtkWidget *Printoutput::outputOrderCheck = NULL;

Printoutput* Printoutput::m_ptrInstance = NULL;
Printoutput* Printoutput::GetInstance()
{
	if(m_ptrInstance == NULL)
	m_ptrInstance = new Printoutput;
	return m_ptrInstance;
}

Printoutput::Printoutput(void)
{

}

Printoutput::~Printoutput(void)
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

void Printoutput::OutputTabSetDefaults(void)
{
	if(mirroroutput == 0)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mirrorCheck), 0);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mirrorCheck), 1);

	/* Sheets Type */
	nupStyle = 0;

	/* Landscape */
	if(wantLandscape)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_slist_nth_data(orientButtons, 0)), TRUE);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_slist_nth_data(orientButtons, 1)), TRUE);

	gtk_adjustment_set_value(GTK_ADJUSTMENT(brightAdj), (int)brightness);
	gtk_adjustment_set_value(GTK_ADJUSTMENT(gammaAdj), ggamma);
}

void Printoutput::OutputOrderCheckChanged(GtkWidget *widget, gpointer data)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(outputOrderCheck)))
	{
		wantReverseOutputOrder = 0;
	}
	else
	{
		wantReverseOutputOrder = 1;
	}
}

void Printoutput::LandscapeCheckChanged(gpointer data)
{
	if(strcmp((char *)data, "portrait") == 0)
	{
		wantLandscape = 0;
	}
	else
	{
		wantLandscape = 1;
	}
}

void Printoutput::NupStyleChange(gpointer data)
{
	nupStyle += atoi((char *)data);
	if(nupStyle<0)
		nupStyle = 7;
	if(nupStyle > 7)
		nupStyle = 0;
}

void Printoutput::WantMirrorCheck(GtkWidget *widget)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mirrorCheck)))
	{
		mirroroutput = 0;
	}
	else
	{
		mirroroutput = 1;
	}
}


void Printoutput::SheetUsageFrame(GtkWidget *container)
{
	GtkWidget *mainHBox, *hbox2, *vbox;

	mainHBox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, 0);
	gtk_widget_show(mainHBox);
	sheetsFrame = gtk_frame_new(_("Sheet Usage"));
	gtk_box_pack_start(GTK_BOX(mainHBox), sheetsFrame, TRUE, TRUE, FRAME_SPACING_V);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_add(GTK_CONTAINER(sheetsFrame), vbox);
	gtk_widget_show(vbox);

    hbox2 = gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, TRUE,0);
    gtk_widget_show(hbox2);

 	mirrorCheck = gtk_check_button_new_with_label(_("Mirror Output"));
	gtk_box_pack_start(GTK_BOX(hbox2), mirrorCheck, FALSE, FALSE, FRAME_SPACING_H);
	gtk_widget_show(mirrorCheck);
   g_signal_connect(G_OBJECT(mirrorCheck), "pressed", G_CALLBACK(on_button_wantmirrorcheck), this);
}

void Printoutput::BrightFrame(GtkWidget *container)
{
	GtkWidget *mainHBox, *hbox1, *hbox2, *vbox, *brightScale, *button1, *button2;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	brightFrame = gtk_frame_new(_("Brightness"));
	gtk_box_pack_start(GTK_BOX(mainHBox), brightFrame, TRUE, TRUE, FRAME_SPACING_V);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_add(GTK_CONTAINER(brightFrame), vbox);
	gtk_widget_show(vbox);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox1);

	brightAdj = gtk_adjustment_new(0, BRIGHT_MIN, BRIGHT_MAX, BRIGHT_STEP, BRIGHT_PAGE, 0);
	brightScale = gtk_hscale_new(GTK_ADJUSTMENT(brightAdj));
	gtk_scale_set_digits(GTK_SCALE(brightScale), BRIGHT_DIGIT);
	gtk_scale_set_draw_value(GTK_SCALE(brightScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(brightScale), GTK_POS_TOP);
	gtk_range_set_update_policy(GTK_RANGE(brightScale), GTK_UPDATE_DISCONTINUOUS);
	gtk_box_pack_start(GTK_BOX(hbox1), brightScale, TRUE, TRUE, 0);
	gtk_widget_show(brightScale);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox2);

	button1 = gtk_button_new_with_label(_("Default"));
	Button_pad(button1);
	gtk_box_pack_end(GTK_BOX(hbox2), button1, FALSE, FALSE, BUTTON_SPACING_H);
	gtk_widget_show(button1);

	button2 = gtk_button_new_with_label(_("Reset"));
	Button_pad(button2);
	gtk_box_pack_end(GTK_BOX(hbox2), button2, FALSE, FALSE, 0);
	gtk_widget_show(button2);

	g_signal_connect(G_OBJECT(button1), "pressed", G_CALLBACK(on_button_resetbutton), (void *)"BrightnessDefault");
	g_signal_connect(G_OBJECT(button2), "pressed", G_CALLBACK(on_button_resetbutton), (void *)"Brightness");
}

void Printoutput::GammaFrame(GtkWidget *container)
{
	GtkWidget *mainHBox, *hbox1, *hbox2, *vbox, *gammaScale, *button1, *button2;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	gammaFrame = gtk_frame_new(_("Gamma correction"));
	gtk_box_pack_start(GTK_BOX(mainHBox), gammaFrame, TRUE, TRUE, FRAME_SPACING_V);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_add(GTK_CONTAINER(gammaFrame), vbox);
	gtk_widget_show(vbox);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox1);

	gammaAdj = gtk_adjustment_new(0, GAMMA_MIN, GAMMA_MAX, GAMMA_STEP, GAMMA_PAGE, 0);
	gammaScale = gtk_hscale_new(GTK_ADJUSTMENT(gammaAdj));
	gtk_scale_set_digits(GTK_SCALE(gammaScale), GAMMA_DIGIT);
	gtk_scale_set_draw_value(GTK_SCALE(gammaScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(gammaScale), GTK_POS_TOP);
	gtk_range_set_update_policy(GTK_RANGE(gammaScale), GTK_UPDATE_DISCONTINUOUS);
	gtk_box_pack_start(GTK_BOX(hbox1), gammaScale, TRUE, TRUE, 0);
	gtk_widget_show(gammaScale);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox2);

	button1 = gtk_button_new_with_label(_("Default"));
	Button_pad(button1);
	gtk_box_pack_end(GTK_BOX(hbox2), button1, FALSE, FALSE, BUTTON_SPACING_H);
	gtk_widget_show(button1);

	button2 = gtk_button_new_with_label(_("Reset"));
	Button_pad(button2);
	gtk_box_pack_end(GTK_BOX(hbox2), button2, FALSE, FALSE, 0);
	gtk_widget_show(button2);

	g_signal_connect(G_OBJECT(button1), "pressed", G_CALLBACK(on_button_resetbutton), (void *)"GammaDefault");
	g_signal_connect(G_OBJECT(button2), "pressed", G_CALLBACK(on_button_resetbutton), (void *)"Gamma");
}

void Printoutput::OutputTab(void)
{
	outputTab = gtk_vbox_new(FALSE, 0);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), outputTab, gtk_label_new(_("Output")));

	SheetUsageFrame(outputTab);
	BrightFrame(outputTab);
	GammaFrame(outputTab);
}

void Printoutput::ButtonNupStyleChange(gpointer data)
{
	NupStyleChange(data);
}

void Printoutput::ButtonResetButton(gpointer data)
{
	Printfunctions::GetInstance()->ResetButton(data);
}
