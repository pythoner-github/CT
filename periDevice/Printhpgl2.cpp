/*
 * Printhpgl2.cpp
 *
 *  Created on: 2012-8-6
 *      Author: yangyaoqing
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include "Printhpgl2.h"
#include "Printdefaults.h"
#include "Printmain.h"
#include "Printfunctions.h"

int Printhpgl2::hpgl2black = 0;
int Printhpgl2::hpgl2fit = 0;
GtkWidget *Printhpgl2::hpgl2PenFrame = NULL;
GtkWidget *Printhpgl2::hpgl2OptFrame = NULL;
GtkWidget *Printhpgl2::hpgl2Tab = NULL;
GtkWidget *Printhpgl2::hpgl2PenField = NULL;

GtkWidget *Printhpgl2::hpgl2BlackCheck = NULL;
GtkWidget *Printhpgl2::hpgl2FitCheck = NULL;

Printhpgl2* Printhpgl2::m_ptrInstance = NULL;
Printhpgl2* Printhpgl2::GetInstance()
{
	if(m_ptrInstance == NULL)
	m_ptrInstance = new Printhpgl2;
	return m_ptrInstance;
}

Printhpgl2::Printhpgl2(void)
{

}

Printhpgl2::~Printhpgl2(void)
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

void Printhpgl2::Hpgl2TabSetDefaults(void)
{
	char tmp[MAX_HPGL_PEN_DIGIT + 1];

	/* Black */
	if(hpgl2black)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hpgl2BlackCheck), 1);
	}
	else
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hpgl2BlackCheck), 0);
	}
	/* Fit */
	if(hpgl2fit)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hpgl2FitCheck), 1);
	}
	else
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hpgl2FitCheck), 0);
	}
	/* Pen */
	snprintf(tmp, (size_t)MAX_HPGL_PEN_DIGIT, "%u", hpgl2pen);
	gtk_entry_set_text(GTK_ENTRY (hpgl2PenField), tmp);
}

void Printhpgl2::Hpgl2CheckChanged(gpointer data)
{
	if(strcmp((char *)data, "Black") == 0)
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(hpgl2BlackCheck)))
		{
			hpgl2black = 0;
		}
		else
		{
			hpgl2black = 1;
		}
		return;
	}

	if(strcmp((char *)data, "Fit") == 0)
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(hpgl2FitCheck)))
		{
			hpgl2fit=0;
		}
		else
		{
			hpgl2fit=1;
		}
		return;
	}
}

void Printhpgl2::Hpgl2OptFrame(GtkWidget *container)
{
	GtkWidget *mainHBox, *hbox, *vbox;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	hpgl2OptFrame = gtk_frame_new(_("Options"));
	gtk_box_pack_start(GTK_BOX(mainHBox), hpgl2OptFrame, TRUE, TRUE, FRAME_SPACING_V);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(hpgl2OptFrame), hbox);
	gtk_widget_show(hbox);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(vbox);

	hpgl2BlackCheck  =  gtk_check_button_new_with_label(_("Printing in Black"));
	gtk_box_pack_start(GTK_BOX(vbox), hpgl2BlackCheck, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(hpgl2BlackCheck), "pressed", G_CALLBACK(on_button_printhpgl2clicked), (void *)"Black");
	gtk_widget_show(hpgl2BlackCheck);

	hpgl2FitCheck = gtk_check_button_new_with_label(_("Fit on Page"));
	gtk_box_pack_start(GTK_BOX(vbox), hpgl2FitCheck, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(hpgl2FitCheck), "pressed", G_CALLBACK(on_button_printhpgl2clicked), (void *)"Fit");
	gtk_widget_show(hpgl2FitCheck);
}

void Printhpgl2::Hpgl2PenChanged(GtkWidget *widget)
{
	char tmp[MAX_HPGL_PEN_DIGIT + 1];
	unsigned i1, flg = 0;

	strncpy(tmp, gtk_editable_get_chars(GTK_EDITABLE(hpgl2PenField), 0, -1), (size_t)MAX_HPGL_PEN_DIGIT);

	for(i1 = 0; i1<strlen(tmp); i1 ++ )
	{
		if(isdigit(tmp[i1])  ==  0)
		{
			snprintf(tmp, (size_t)MAX_HPGL_PEN_DIGIT, "%u", hpgl2pen);
			gtk_entry_set_text(GTK_ENTRY(hpgl2PenField), tmp);
			gtk_editable_set_position(GTK_EDITABLE(hpgl2PenField), -1);
			flg = 1;
			break;
		}
	}

	if((flg == 0) && tmp[0] !=  (uintptr_t)NULL)
	{
		hpgl2pen = atoi(tmp);
	}
}

void Printhpgl2::Hpgl2PenFrame(GtkWidget *container)
{
	GtkWidget *mainHBox, *hbox, *label, *button1;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	hpgl2PenFrame = gtk_frame_new(_("Pen Width"));
	gtk_box_pack_start(GTK_BOX(mainHBox), hpgl2PenFrame, TRUE, TRUE, FRAME_SPACING_V);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(hpgl2PenFrame), hbox);
	gtk_widget_show(hbox);

	hpgl2PenField = gtk_entry_new_with_max_length (MAX_HPGL_PEN_DIGIT);
	gtk_widget_set_usize(hpgl2PenField, MAX_HPGL_PEN_FIELD_LENGTH, -1);
	gtk_entry_set_text(GTK_ENTRY (hpgl2PenField), "");
	gtk_box_pack_start(GTK_BOX(hbox), hpgl2PenField, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(hpgl2PenField);

	label = gtk_label_new(_("micrometers"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	button1 = gtk_button_new_with_label(_("Default"));
	Button_pad(button1);
	gtk_box_pack_end(GTK_BOX(hbox), button1, FALSE, FALSE, 0);
	gtk_widget_show(button1);

	g_signal_connect(G_OBJECT(GTK_ENTRY(hpgl2PenField)), "changed", GTK_SIGNAL_FUNC(on_button_hpgl2penchange), this);
	g_signal_connect(G_OBJECT(button1), "pressed", GTK_SIGNAL_FUNC(on_button_hpgl2resetbutton), (void *)"HPGL2PenDefault");
}

void Printhpgl2::Hpgl2Tab(void)
{
	hpgl2Tab = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hpgl2Tab), FRAME_BORDER);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), hpgl2Tab, gtk_label_new(_("HP-GL/2")));

	Hpgl2OptFrame(hpgl2Tab);
	Hpgl2PenFrame(hpgl2Tab);
}

void Printhpgl2::ButtonPrinthpgl2Clicked(GtkWidget *button, gpointer data)
{
	Hpgl2CheckChanged(data);
}

void Printhpgl2::ButtonHpgl2PenFrame(GtkWidget *button, GtkWidget *widget)
{
	Hpgl2PenFrame(widget);
}
void Printhpgl2::ButtonHpgl2PenChanged(GtkWidget *button, GtkWidget *widget)
{
	Hpgl2PenChanged(widget);
}

void Printhpgl2::ButtonHpgl2ResetButton(gpointer data)
{
	Printfunctions::GetInstance()->ResetButton(data);
}
