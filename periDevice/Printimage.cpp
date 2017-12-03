/*
 * Printimage.cpp
 *
 *  Created on: 2012-8-6
 *      Author: yangyaoqing
 */
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "Printimage.h"
#include "Printdefaults.h"
#include "Printfunctions.h"

GtkObject *Printimage::imageHUEAdj = NULL;
GtkObject *Printimage::imageSatAdj = NULL;
GtkWidget *Printimage::imageScalingAllFrame = NULL;
GtkWidget *Printimage::imageHUEFrame = NULL;
GtkWidget *Printimage::imageSatFrame = NULL;
GtkWidget *Printimage::imageTab = NULL;

GtkObject *Printimage::imageScaleAllAdj = NULL;
GtkWidget *Printimage::imageHUEButton = NULL;
GtkWidget *Printimage::imageSatButton = NULL;
GtkWidget *Printimage::scaleTypeCombo = NULL;
GtkWidget *Printimage::imageScaleAllScale = NULL;

Printimage* Printimage::m_ptrInstance = NULL;
Printimage* Printimage::GetInstance()
{
	if(m_ptrInstance == NULL)
	m_ptrInstance = new Printimage;
	return m_ptrInstance;
}

Printimage::Printimage(void)
{
}

Printimage::~Printimage(void)
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

void Printimage::ImageTabSetDefaults(void)
{
	/* Image Scaling */
	gtk_list_select_item(GTK_LIST(GTK_COMBO(scaleTypeCombo)->list), imageScalingType);
	/* HUE */
	gtk_adjustment_set_value(GTK_ADJUSTMENT(imageHUEAdj), hue);
	/* Saturation */
	gtk_adjustment_set_value(GTK_ADJUSTMENT(imageSatAdj), sat);
}

void Printimage::ImageScalingAdjChanged(GtkWidget *widget, gpointer data)
{
	switch(imageScalingType)
	{
	case 0:   /* None */
    	imageScalingNomeValue = GTK_ADJUSTMENT(imageScaleAllAdj)->value;
		break;
    case 1:   /* Sheet */
    	imageScalingScalingValue = GTK_ADJUSTMENT(imageScaleAllAdj)->value;
    	break;
    case 2:   /* Image */
    	imageScalingNaturalValue = GTK_ADJUSTMENT(imageScaleAllAdj)->value;
    	break;
    case 3:   /* PPI */
    	imageScalingPPIValue = GTK_ADJUSTMENT(imageScaleAllAdj)->value;
    	break;
	}
}

void Printimage::ImageScalingAllChanged(GtkWidget *widget, gpointer data)
{
	char tmp[MAXLINE+1];

	snprintf(tmp, (size_t)MAXLINE, "%s", gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(scaleTypeCombo)->entry)));

	if(strlen(tmp) == 0) {
		return;
	}

	if(strcmp(tmp, _("none"))  ==  0)
		imageScalingType = 0;
	else if(strcmp(tmp, _("Sheet relativ"))  ==  0)
		imageScalingType = 1;
	else if(strcmp(tmp, _("Image relativ"))  ==  0)
		imageScalingType = 2;
	else if(strcmp(tmp, _("PPI"))  ==  0)
		imageScalingType = 3;
	else
	{
		;
	}
	switch(imageScalingType)
	{
	case 0:		/* None */
		gtk_widget_set_sensitive(GTK_WIDGET(imageScaleAllScale), TRUE);
		GTK_ADJUSTMENT(imageScaleAllAdj)->lower = 0;
		GTK_ADJUSTMENT(imageScaleAllAdj)->upper = 800;
		GTK_ADJUSTMENT(imageScaleAllAdj)->step_increment = IMAGE_SCALE_STEP;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_increment = IMAGE_SCALE_PAGE;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_size = 0;

		GTK_ADJUSTMENT(imageScaleAllAdj)->value = imageScalingNomeValue;
		gtk_adjustment_value_changed(GTK_ADJUSTMENT(imageScaleAllAdj));

		//GTK_ADJUSTMENT(imageScaleAllAdj)->value = 0;
		//gtk_adjustment_value_changed(GTK_ADJUSTMENT(imageScaleAllAdj));
		//gtk_widget_set_sensitive(GTK_WIDGET(imageScaleAllScale), FALSE);
		break;
	case 1:		/* Sheet */
		gtk_widget_set_sensitive(GTK_WIDGET(imageScaleAllScale), TRUE);
		GTK_ADJUSTMENT(imageScaleAllAdj)->lower = 1;
		GTK_ADJUSTMENT(imageScaleAllAdj)->upper = 800;
		GTK_ADJUSTMENT(imageScaleAllAdj)->step_increment = IMAGE_SCALE_STEP;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_increment = IMAGE_SCALE_PAGE;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_size = 0;

		GTK_ADJUSTMENT(imageScaleAllAdj)->value = imageScalingScalingValue;
		gtk_adjustment_value_changed(GTK_ADJUSTMENT(imageScaleAllAdj));
		break;
	case 2: 	/* Image */
		gtk_widget_set_sensitive(GTK_WIDGET(imageScaleAllScale), TRUE);
		GTK_ADJUSTMENT(imageScaleAllAdj)->lower = 1;
		GTK_ADJUSTMENT(imageScaleAllAdj)->upper = 800;
		GTK_ADJUSTMENT(imageScaleAllAdj)->step_increment = IMAGE_SCALE_STEP;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_increment = IMAGE_SCALE_PAGE;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_size = 0;
		GTK_ADJUSTMENT(imageScaleAllAdj)->value = imageScalingNaturalValue;
		gtk_adjustment_value_changed(GTK_ADJUSTMENT(imageScaleAllAdj));
		break;
	case 3: 	/* PPI */
		gtk_widget_set_sensitive(GTK_WIDGET(imageScaleAllScale), TRUE);
		GTK_ADJUSTMENT(imageScaleAllAdj)->lower = 1;
		GTK_ADJUSTMENT(imageScaleAllAdj)->upper = 1200;
		GTK_ADJUSTMENT(imageScaleAllAdj)->step_increment = IMAGE_PPI_STEP;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_increment = IMAGE_PPI_PAGE;
		GTK_ADJUSTMENT(imageScaleAllAdj)->page_size = 0;
		GTK_ADJUSTMENT(imageScaleAllAdj)->value = imageScalingPPIValue;
		gtk_adjustment_value_changed(GTK_ADJUSTMENT(imageScaleAllAdj));
		break;
	}
}

void Printimage::ImageScalingAllFrame(GtkWidget *container)
{
	GtkWidget *mainHBox, *hbox, *vbox, *hbox1;
	GList *STList = NULL;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	imageScalingAllFrame = gtk_frame_new(_("Scaling"));
	gtk_box_pack_start(GTK_BOX(mainHBox), imageScalingAllFrame, TRUE, TRUE, 0);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(imageScalingAllFrame), vbox);
	gtk_widget_show(vbox);

	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
	gtk_widget_show(hbox);

	hbox1 = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, INFRAME_SPACING_V);
	gtk_widget_show(hbox1);

	/* Type-Combo-Box */
	scaleTypeCombo = gtk_combo_new();
	STList = g_list_append(STList, _("none"));
	STList = g_list_append(STList, _("Sheet relativ"));
	STList = g_list_append(STList, _("Image relativ"));
	STList = g_list_append(STList, _("PPI"));

	gtk_combo_set_popdown_strings (GTK_COMBO(scaleTypeCombo), STList);
	gtk_combo_set_value_in_list(GTK_COMBO(scaleTypeCombo), TRUE, FALSE);
	gtk_list_set_selection_mode(GTK_LIST(GTK_COMBO(scaleTypeCombo)->list), GTK_SELECTION_SINGLE);

	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(scaleTypeCombo)->entry), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox), scaleTypeCombo, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(scaleTypeCombo);

	/* Slider */
	imageScaleAllAdj = gtk_adjustment_new(0, 1, 1, 1, 1, 0);
	imageScaleAllScale = gtk_hscale_new(GTK_ADJUSTMENT(imageScaleAllAdj));
	gtk_scale_set_digits(GTK_SCALE(imageScaleAllScale), IMAGE_SCALE_NATURAL_DIGIT);
	gtk_scale_set_draw_value(GTK_SCALE(imageScaleAllScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(imageScaleAllScale), GTK_POS_TOP);
	gtk_range_set_update_policy(GTK_RANGE(imageScaleAllScale), GTK_UPDATE_DISCONTINUOUS);
	gtk_box_pack_start(GTK_BOX(hbox1), imageScaleAllScale, TRUE, TRUE, INFRAME_SPACING_V);
	gtk_widget_show(imageScaleAllScale);

	g_signal_connect(G_OBJECT(GTK_COMBO(scaleTypeCombo)->entry), "changed", G_CALLBACK(button_imagescalingallchanged), this);
	g_signal_connect(G_OBJECT(imageScaleAllAdj), "value-changed", G_CALLBACK(button_imageScalingadjchanged), this);
}

void Printimage::ImageHUEFrame(GtkWidget *container)
{
	GtkWidget *mainHBox, *hbox1, *hbox2, *vbox, *imageHUEScale, *button1, *button2;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	imageHUEFrame = gtk_frame_new(_("Color HUE rotation"));
	gtk_box_pack_start(GTK_BOX(mainHBox), imageHUEFrame, TRUE, TRUE, FRAME_SPACING_V);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_add(GTK_CONTAINER(imageHUEFrame), vbox);
	gtk_widget_show(vbox);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox1);

	imageHUEAdj = gtk_adjustment_new(0, -360, 360, HUE_STEP, HUE_PAGE, 0);
	imageHUEScale = gtk_hscale_new(GTK_ADJUSTMENT(imageHUEAdj));
	gtk_scale_set_digits(GTK_SCALE(imageHUEScale), HUE_DIGIT);
	gtk_scale_set_draw_value(GTK_SCALE(imageHUEScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(imageHUEScale), GTK_POS_TOP);
	gtk_range_set_update_policy(GTK_RANGE(imageHUEScale), GTK_UPDATE_DISCONTINUOUS);
	gtk_box_pack_start(GTK_BOX(hbox1), imageHUEScale, TRUE, TRUE, INFRAME_SPACING_H);
	gtk_widget_show(imageHUEScale);

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

	g_signal_connect(G_OBJECT(button1), "pressed", G_CALLBACK(button_reset), (void *)"HUEDefault");
	g_signal_connect(G_OBJECT(button2), "pressed", G_CALLBACK(button_reset), (void *)"HUE");
}

void Printimage::ImageSatFrame(GtkWidget *container)
{
	GtkWidget *mainHBox, *hbox1, *hbox2, *vbox, *imageSatScale, *button1, *button2;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);
	imageSatFrame = gtk_frame_new(_("Color saturation"));
	gtk_box_pack_start(GTK_BOX(mainHBox), imageSatFrame, TRUE, TRUE, FRAME_SPACING_V);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_add(GTK_CONTAINER(imageSatFrame), vbox);
	gtk_widget_show(vbox);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(hbox1);

	imageSatAdj = gtk_adjustment_new(0, 0, 200, SAT_STEP, HUE_PAGE, 0);
	imageSatScale = gtk_hscale_new(GTK_ADJUSTMENT(imageSatAdj));
	gtk_scale_set_digits(GTK_SCALE(imageSatScale), SAT_DIGIT);
	gtk_scale_set_draw_value(GTK_SCALE(imageSatScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(imageSatScale), GTK_POS_TOP);
	gtk_range_set_update_policy(GTK_RANGE(imageSatScale), GTK_UPDATE_DISCONTINUOUS);
	gtk_box_pack_start(GTK_BOX(hbox1), imageSatScale, TRUE, TRUE, INFRAME_SPACING_H);
	gtk_widget_show(imageSatScale);

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

	g_signal_connect(G_OBJECT(button1), "pressed", G_CALLBACK(button_reset), (void *)"SatDefault");
	g_signal_connect(G_OBJECT(button2), "pressed", G_CALLBACK(button_reset), (void *)"Sat");
}

void Printimage::ImageTab(void)
{
    GtkWidget  *hbox;

	imageTab = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(imageTab), FRAME_BORDER);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), imageTab, gtk_label_new(_("Image")));

	hbox  =  gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(imageTab), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	ImageScalingAllFrame(hbox);
	ImageHUEFrame(imageTab);
	ImageSatFrame(imageTab);
}

void Printimage::ButtonImageScalingAllChanged(GtkWidget *button, GtkWidget *widget, gpointer data)
{
	ImageScalingAllChanged(widget, data);
}

void Printimage::ButtonImageScalingAdjChanged(GtkWidget *button, GtkWidget *widget)
{
	ImageScalingAdjChanged(widget, NULL);
}

void Printimage::ButtonReset(GtkButton *button, gpointer data)
{
	Printfunctions::GetInstance()->ResetButton(data);
}
