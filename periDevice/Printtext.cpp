/*
 * Printtext.cpp
 *
 *  Created on: 2012-8-6
 *      Author: yangyaoqing
 */

#include <stdlib.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include <string.h>
#include "Printtext.h"
#include "Printdefaults.h"
#include "Printmain.h"
#include "Printfunctions.h"

GtkWidget *Printtext::textSizesFrame = NULL;
GtkWidget *Printtext::textMarginsFrame = NULL;
GtkWidget *Printtext::textPrettyFrame = NULL;
GtkWidget *Printtext::textTab = NULL;
GtkWidget *Printtext::textCPIField = NULL;
GtkWidget *Printtext::textLPIField = NULL;
GtkWidget *Printtext::textCPPField = NULL;

GtkWidget *Printtext::wrapCheck = NULL;
GtkWidget *Printtext::textLeftField = NULL;
GtkWidget *Printtext::textRightField = NULL;
GtkWidget *Printtext::textTopField = NULL;
GtkWidget *Printtext::textBottomField = NULL;
GtkWidget *Printtext::textLeftField_I = NULL;
GtkWidget *Printtext::textRightField_I = NULL;
GtkWidget *Printtext::textTopField_I = NULL;
GtkWidget *Printtext::textBottomField_I = NULL;
GtkWidget *Printtext::textLeftField_C = NULL;
GtkWidget *Printtext::textRightField_C = NULL;
GtkWidget *Printtext::textTopField_C = NULL;
GtkWidget *Printtext::textBottomField_C = NULL;
GtkWidget *Printtext::prettyCheck = NULL;

Printtext* Printtext::m_ptrInstance = NULL;
Printtext* Printtext::GetInstance()
{
	if(m_ptrInstance == NULL)
	m_ptrInstance = new Printtext;
	return m_ptrInstance;
}

Printtext::Printtext(void)
{

}

Printtext::~Printtext(void)
{
	if(m_ptrInstance !=NULL)
		delete m_ptrInstance;
}

void Printtext::TextTabSetDefaults(void)
{
	char tmp[MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+5];

	/* Sizes */
	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%u", cpivalue);
	gtk_entry_set_text(GTK_ENTRY (textCPIField), tmp);
	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%u", lpivalue);
	gtk_entry_set_text(GTK_ENTRY (textLPIField), tmp);
	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%u", cppvalue);
	gtk_entry_set_text(GTK_ENTRY (textCPPField), tmp);

	/* Margins */
	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%u", topmargin);
	gtk_entry_set_text(GTK_ENTRY(textTopField), tmp);
	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%.3f", topmargin*0.01389);
	gtk_label_set_text(GTK_LABEL(textTopField_I), tmp);
	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%.2f", topmargin*0.35);
	gtk_label_set_text(GTK_LABEL(textTopField_C), tmp);

	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%u", bottommargin);
	gtk_entry_set_text(GTK_ENTRY(textBottomField), tmp);
	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%.3f", bottommargin*0.01389);
	gtk_label_set_text(GTK_LABEL(textBottomField_I), tmp);
	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%.2f", bottommargin*0.35);
	gtk_label_set_text(GTK_LABEL(textBottomField_C), tmp);

	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%u", leftmargin);
	gtk_entry_set_text(GTK_ENTRY(textLeftField), tmp);
	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%.3f", leftmargin*0.01389);
	gtk_label_set_text(GTK_LABEL(textLeftField_I), tmp);
	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%.2f", leftmargin*0.35);
	gtk_label_set_text(GTK_LABEL(textLeftField_C), tmp);

	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%u", rightmargin);
	gtk_entry_set_text(GTK_ENTRY(textRightField), tmp);
	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%.3f", rightmargin*0.01389);
	gtk_label_set_text(GTK_LABEL(textRightField_I), tmp);
	snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+MAX_MARGIN_DIGITS+4, "%.2f", rightmargin*0.35);
	gtk_label_set_text(GTK_LABEL(textRightField_C), tmp);

	/* Pretty Print */
	if(prettyprint)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prettyCheck), 1);
	}
	else
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prettyCheck), 0);
	}

	/* Wrap Text */
	if(textwrap)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wrapCheck), 1);
	}
	else
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wrapCheck), 0);
	}
}

void Printtext::TextSizesChanged(gpointer data)
{
	char tmp[MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS+1];
	unsigned i1, flg = 0;

	tmp[0] = (uintptr_t)NULL;

	if(strcmp((char *)data, "CPI") == 0)
	{
		strncpy(tmp, gtk_editable_get_chars(GTK_EDITABLE(textCPIField), 0, -1), (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS);
	}
	else if(strcmp((char *)data, "LPI") == 0)
	{
		strncpy(tmp, gtk_editable_get_chars(GTK_EDITABLE(textLPIField), 0, -1), (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS);
	}
	else if(strcmp((char *)data, "CPP") == 0)
	{
		strncpy(tmp, gtk_editable_get_chars(GTK_EDITABLE(textCPPField), 0, -1), (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS);
	}

	for(i1 = 0; i1<strlen(tmp);i1++)
	{
		if(isdigit(tmp[i1])  ==  0)
		{
			if(strcmp((char *)data, "CPI") == 0)
		{
			snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS, "%u", cpivalue);
			gtk_entry_set_text(GTK_ENTRY(textCPIField), tmp);
			gtk_editable_set_position(GTK_EDITABLE(textCPIField), -1);
		}
		else if(strcmp((char *)data, "LPI") == 0)
		{
			snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS, "%u", lpivalue);
			gtk_entry_set_text(GTK_ENTRY(textLPIField), tmp);
			gtk_editable_set_position(GTK_EDITABLE(textLPIField), -1);
		}
		else if(strcmp((char *)data, "CPP") == 0)
		{
			snprintf(tmp, (size_t)MAX_CPI_DIGITS+MAX_LPI_DIGITS+MAX_CPP_DIGITS, "%u", cppvalue);
			gtk_entry_set_text(GTK_ENTRY(textCPPField), tmp);
			gtk_editable_set_position(GTK_EDITABLE(textCPPField), -1);
		}
		flg = 1;
		break;
		}
	}

	if((flg == 0) && tmp[0] !=  (uintptr_t)NULL)
	{
		if(strcmp((char *)data, "CPI") == 0)
		{
			cpivalue = atoi(tmp);
		}
		else if(strcmp((char *)data, "LPI") == 0)
		{
			lpivalue = atoi(tmp);
		}
		else if(strcmp((char *)data, "CPP") == 0)
		{
			cppvalue = atoi(tmp);
		}
	}
}

void Printtext::TextSizes(GtkWidget *container)
{
	GtkWidget *mainHBox, *vbox, *hbox1, *hbox2, *hbox3;
	GtkWidget *labelCPI, *buttonCPI1;
	GtkWidget *labelLPI, *buttonLPI1;
	GtkWidget *labelCPP, *buttonCPP1;

	mainHBox  =  gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	textSizesFrame = gtk_frame_new(_("Sizes and Spacings"));
	gtk_box_pack_start(GTK_BOX(mainHBox), textSizesFrame, TRUE, TRUE, FRAME_SPACING_V);

	/* Box */
	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(textSizesFrame), vbox);
	gtk_widget_show(vbox);

	/* Box */
	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 0);
	gtk_widget_show(hbox1);

	/* Label CPI */
	labelCPI = gtk_label_new(_("Chars per Inch"));
	gtk_misc_set_alignment(GTK_MISC(labelCPI), 0, 0.5);
	gtk_widget_set_usize(GTK_WIDGET(labelCPI), TEXTSIZE_FIELDLENGTH, -1);
	gtk_box_pack_start(GTK_BOX(hbox1), labelCPI, FALSE, FALSE, 0);
	gtk_widget_show(labelCPI);

	/* TextBox CPI */
	textCPIField = gtk_entry_new_with_max_length (MAX_CPI_DIGITS);
	gtk_widget_set_usize(textCPIField, MAXTEXTPERFIELDDIGITS, -1);
	gtk_entry_set_text(GTK_ENTRY (textCPIField), "");
	gtk_box_pack_start(GTK_BOX(hbox1), textCPIField, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(textCPIField);

	/* Buttons CPI */
	buttonCPI1 = gtk_button_new_with_label(_("Default"));
	Button_pad(buttonCPI1);
	gtk_box_pack_end(GTK_BOX(hbox1), buttonCPI1, FALSE, FALSE, 0);
	gtk_widget_show(buttonCPI1);

	/* Box */
	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);
	gtk_widget_show(hbox2);

	/* Label LPI */
	labelLPI = gtk_label_new(_("Lines per Inch"));
	gtk_misc_set_alignment(GTK_MISC(labelLPI), 0, 0.5);
	gtk_widget_set_usize(GTK_WIDGET(labelLPI), TEXTSIZE_FIELDLENGTH, -1);
	gtk_box_pack_start(GTK_BOX(hbox2), labelLPI, FALSE, FALSE, 0);
	gtk_widget_show(labelLPI);

	/* TextBox LPI */
	textLPIField = gtk_entry_new_with_max_length (MAX_LPI_DIGITS);
	gtk_widget_set_usize(textLPIField, MAXTEXTPERFIELDDIGITS, -1);
	gtk_entry_set_text(GTK_ENTRY (textLPIField), "");
	gtk_box_pack_start(GTK_BOX(hbox2), textLPIField, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(textLPIField);

	/* Buttons LPI */
	buttonLPI1 = gtk_button_new_with_label(_("Default"));
	Button_pad(buttonLPI1);
	gtk_box_pack_end(GTK_BOX(hbox2), buttonLPI1, FALSE, FALSE, 0);
	gtk_widget_show(buttonLPI1);

	/* Box */
	hbox3 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox3, FALSE, FALSE, 0);
	gtk_widget_show(hbox3);

	/* Label CPP */
	labelCPP = gtk_label_new(_("Columns per Page"));
	gtk_misc_set_alignment(GTK_MISC(labelCPP), 0, 0.5);
	gtk_widget_set_usize(GTK_WIDGET(labelCPP), TEXTSIZE_FIELDLENGTH, -1);
	gtk_box_pack_start(GTK_BOX(hbox3), labelCPP, FALSE, FALSE, 0);
	gtk_widget_show(labelCPP);

	/* TextBox CPP */
	textCPPField = gtk_entry_new_with_max_length (MAX_CPP_DIGITS);
	gtk_widget_set_usize(textCPPField, MAXTEXTPERFIELDDIGITS, -1);
	gtk_entry_set_text(GTK_ENTRY (textCPPField), "");
	gtk_box_pack_start(GTK_BOX(hbox3), textCPPField, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(textCPPField);

	/* Buttons CPI */
	buttonCPP1 = gtk_button_new_with_label(_("Default"));
	Button_pad(buttonCPP1);
	gtk_box_pack_end(GTK_BOX(hbox3), buttonCPP1, FALSE, FALSE, 0);
	gtk_widget_show(buttonCPP1);

	/* Signals */
	g_signal_connect(G_OBJECT(GTK_ENTRY(textCPIField)), "changed", G_CALLBACK(button_textsizeschange), (void *)"CPI");
    g_signal_connect(G_OBJECT(GTK_ENTRY(textLPIField)), "changed", G_CALLBACK(button_textsizeschange), (void *)"LPI");
	g_signal_connect(G_OBJECT(GTK_ENTRY(textCPPField)), "changed", G_CALLBACK(button_textsizeschange), (void *)"CPP");
    g_signal_connect(G_OBJECT(buttonCPI1), "pressed", G_CALLBACK(button_reset), (void *)"CPIDefault");
    g_signal_connect(G_OBJECT(buttonLPI1), "pressed", G_CALLBACK(button_reset), (void *)"LPIDefault");
	g_signal_connect(G_OBJECT(buttonCPP1), "pressed", G_CALLBACK(button_reset), (void *)"CPPDefault");
}

void Printtext::TextMarginsChanged(gpointer data)
{
	char tmp[MAX_MARGIN_DIGITS+5];
	unsigned i1, flg = 0;

	tmp[0] = (uintptr_t)NULL;

	if(strcmp((const char *)data, (const char *)"Top") == 0)
	{
		strncpy(tmp, gtk_editable_get_chars(GTK_EDITABLE(textTopField), 0, -1), (size_t)MAX_MARGIN_DIGITS+4);
	}
	else if(strcmp((const char *)data, (const char *)"Bottom") == 0)
	{
		strncpy(tmp, gtk_editable_get_chars(GTK_EDITABLE(textBottomField), 0, -1), (size_t)MAX_MARGIN_DIGITS+4);
	}
	else if(strcmp((const char *)data, (const char *)"Left") == 0)
	{
		strncpy(tmp, gtk_editable_get_chars(GTK_EDITABLE(textLeftField), 0, -1), (size_t)MAX_MARGIN_DIGITS+4);
	}
	else if(strcmp((const char *)data, (const char *)"Right") == 0)
	{
		strncpy(tmp, gtk_editable_get_chars(GTK_EDITABLE(textRightField), 0, -1), (size_t)MAX_MARGIN_DIGITS+4);
	}
	for(i1 = 0; i1<strlen(tmp);i1++)
	{
		if(isdigit(tmp[i1])  ==  0)
		{
			if(strcmp((const char *)data, (const char *)"Top") == 0)
			{
				snprintf(tmp, (size_t)MAX_MARGIN_DIGITS+4, "%u", topmargin);
				gtk_entry_set_text(GTK_ENTRY(textTopField), tmp);
				gtk_editable_set_position(GTK_EDITABLE(textTopField), -1);
			}
			else if(strcmp((const char *)data, (const char *)"Bottom") == 0)
			{
				snprintf(tmp, (size_t)MAX_MARGIN_DIGITS+4, "%u", bottommargin);
				gtk_entry_set_text(GTK_ENTRY(textBottomField), tmp);
				gtk_editable_set_position(GTK_EDITABLE(textBottomField), -1);
			}
			else if(strcmp((const char *)data, (const char *)"Left") == 0)
			{
				snprintf(tmp, (size_t)MAX_MARGIN_DIGITS+4, "%u", leftmargin);
				gtk_entry_set_text(GTK_ENTRY(textLeftField), tmp);
				gtk_editable_set_position(GTK_EDITABLE(textRightField), -1);
			}
			else if(strcmp((const char *)data, (const char *)"Right") == 0)
			{
				snprintf(tmp, (size_t)MAX_MARGIN_DIGITS+4, "%u", rightmargin);
				gtk_entry_set_text(GTK_ENTRY(textRightField), tmp);
				gtk_editable_set_position(GTK_EDITABLE(textRightField), -1);
			}
			flg = 1;
			break;
		}
	}

	if((flg == 0) && tmp[0] !=  (uintptr_t)NULL)
	{
		if(strcmp((const char *)data, (const char *)"Top") == 0)
		{
			topmargin = atoi(tmp);
			snprintf(tmp, (size_t)MAX_MARGIN_DIGITS+4, "%.3f", topmargin*0.01389);
			gtk_label_set_text(GTK_LABEL(textTopField_I), tmp);
			snprintf(tmp, (size_t)MAX_MARGIN_DIGITS+4, "%.2f", topmargin*0.35);
			gtk_label_set_text(GTK_LABEL(textTopField_C), tmp);
		}
		else if(strcmp((const char *)data, (const char *)"Bottom") == 0)
		{
			bottommargin = atoi(tmp);
			snprintf(tmp, (size_t)MAX_MARGIN_DIGITS+4, "%.3f", bottommargin * 0.01389);
			gtk_label_set_text(GTK_LABEL(textBottomField_I), tmp);
			snprintf(tmp, (size_t)MAX_MARGIN_DIGITS+4, "%.2f", bottommargin * 0.35);
			gtk_label_set_text(GTK_LABEL(textBottomField_C), tmp);
		}
		else if(strcmp((const char *)data, (const char *)"Left") == 0)
		{
			leftmargin = atoi(tmp);
			snprintf(tmp, (size_t)MAX_MARGIN_DIGITS+4, "%.3f", leftmargin * 0.01389);
			gtk_label_set_text(GTK_LABEL(textLeftField_I), tmp);
			snprintf(tmp, (size_t)MAX_MARGIN_DIGITS+4, "%.2f", leftmargin * 0.35);
			gtk_label_set_text(GTK_LABEL(textLeftField_C), tmp);
		}
		else if(strcmp((const char *)data, (const char *)"Right") == 0)
		{
			rightmargin = atoi(tmp);
			snprintf(tmp, (size_t)MAX_MARGIN_DIGITS+4, "%.3f", rightmargin * 0.01389);
			gtk_label_set_text(GTK_LABEL(textRightField_I), tmp);
			snprintf(tmp, (size_t)MAX_MARGIN_DIGITS+4, "%.2f", rightmargin * 0.35);
			gtk_label_set_text(GTK_LABEL(textRightField_C), tmp);
		}
	}
}

#define MARGIN_LABEL_SIZE 60
void Printtext::MarginSizes(GtkWidget *container)
{
	GtkWidget *mainHBox, *vbox, *hbox1, *hbox2, *hbox3, *hbox4;
	GtkWidget *labelTop, *labelBottom, *labelLeft, *labelRight;
	GtkWidget *labelTop_P, *labelBottom_P, *labelLeft_P, *labelRight_P;
	GtkWidget *labelTop_I, *labelBottom_I, *labelLeft_I, *labelRight_I;
	GtkWidget *labelTop_C, *labelBottom_C, *labelLeft_C, *labelRight_C;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	textMarginsFrame = gtk_frame_new(_("Margins"));
	gtk_box_pack_start(GTK_BOX(mainHBox), textMarginsFrame, TRUE, TRUE, FRAME_SPACING_V);

	/* Box */
	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(textMarginsFrame), vbox);
	gtk_widget_show(vbox);

	/* Box */
	hbox1 = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 0);
	gtk_widget_show(hbox1);

	/* Label Top */
	labelTop = gtk_label_new(_("Top"));
	gtk_misc_set_alignment(GTK_MISC(labelTop), 0, 0.5);
	gtk_widget_set_usize(GTK_WIDGET(labelTop), MARGIN_LABEL_SIZE, -1);
	gtk_box_pack_start(GTK_BOX(hbox1), labelTop, FALSE, FALSE, 0);
	gtk_widget_show(labelTop);

	/* TextBox Top */
	textTopField = gtk_entry_new_with_max_length (MAX_MARGIN_DIGITS);
	gtk_widget_set_usize(textTopField, MAXMARGIN_ENTRY_LENGTH, -1);
	gtk_entry_set_text(GTK_ENTRY (textTopField), "");
	gtk_box_pack_start(GTK_BOX(hbox1), textTopField, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(textTopField);

	/* Label Top-Points */
	labelTop_P = gtk_label_new(MARGINS_POINTS);
	gtk_misc_set_alignment(GTK_MISC(labelTop_P), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox1), labelTop_P, FALSE, FALSE, 0);
	gtk_widget_show(labelTop_P);

	/* Label Top-inch1 */
	textTopField_I = gtk_label_new("");
	gtk_misc_set_alignment(GTK_MISC(textTopField_I), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox1), textTopField_I, FALSE, FALSE, 0);
	gtk_widget_show(textTopField_I);

	/* Label Top-inch2 */
	labelTop_I = gtk_label_new(MARGINS_INCH);
	gtk_misc_set_alignment(GTK_MISC(labelTop_I), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox1), labelTop_I, FALSE, FALSE, 0);
	gtk_widget_show(labelTop_I);

	/* Label Top-cm1 */
	textTopField_C = gtk_label_new("");
	gtk_misc_set_alignment(GTK_MISC(textTopField_C), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox1), textTopField_C, FALSE, FALSE, 0);
	gtk_widget_show(textTopField_C);

	/* Label Top-cm2 */
	labelTop_C = gtk_label_new(MARGINS_MM);
	gtk_misc_set_alignment(GTK_MISC(labelTop_C), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox1), labelTop_C, FALSE, FALSE, 0);
	gtk_widget_show(labelTop_C);

	/* Box */
	hbox2 = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);
	gtk_widget_show(hbox2);

	/* Label Bottom */
	labelBottom = gtk_label_new(_("Bottom"));
	gtk_misc_set_alignment(GTK_MISC(labelBottom), 0, 0.5);
	gtk_widget_set_usize(GTK_WIDGET(labelBottom), MARGIN_LABEL_SIZE, -1);
	gtk_box_pack_start(GTK_BOX(hbox2), labelBottom, FALSE, FALSE, 0);
	gtk_widget_show(labelBottom);

	/* TextBox Bottom */
	textBottomField = gtk_entry_new_with_max_length (MAX_MARGIN_DIGITS);
	gtk_widget_set_usize(textBottomField, MAXMARGIN_ENTRY_LENGTH, -1);
	gtk_entry_set_text(GTK_ENTRY (textBottomField), "");
	gtk_box_pack_start(GTK_BOX(hbox2), textBottomField, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(textBottomField);

	/* Label Bottom-Points */
	labelBottom_P = gtk_label_new(MARGINS_POINTS);
	gtk_misc_set_alignment(GTK_MISC(labelBottom_P), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox2), labelBottom_P, FALSE, FALSE, 0);
	gtk_widget_show(labelBottom_P);

	/* Label Bottom-inch1 */
	textBottomField_I = gtk_label_new("");
	gtk_misc_set_alignment(GTK_MISC(textBottomField_I), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox2), textBottomField_I, FALSE, FALSE, 0);
	gtk_widget_show(textBottomField_I);

	/* Label Bottom-inch2 */
	labelBottom_I = gtk_label_new(MARGINS_INCH);
	gtk_misc_set_alignment(GTK_MISC(labelBottom_I), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox2), labelBottom_I, FALSE, FALSE, 0);
	gtk_widget_show(labelBottom_I);

	/* Label Bottom-cm1 */
	textBottomField_C = gtk_label_new("");
	gtk_misc_set_alignment(GTK_MISC(textBottomField_C), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox2), textBottomField_C, FALSE, FALSE, 0);
	gtk_widget_show(textBottomField_C);

	/* Label Bottom-cm2 */
	labelBottom_C = gtk_label_new(MARGINS_MM);
	gtk_misc_set_alignment(GTK_MISC(labelBottom_C), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox2), labelBottom_C, FALSE, FALSE, 0);
	gtk_widget_show(labelBottom_C);

	/* Box */
	hbox3 = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox3, FALSE, FALSE, 0);
	gtk_widget_show(hbox3);

	/* Label Left */
	labelLeft = gtk_label_new(_("Left"));
	gtk_misc_set_alignment(GTK_MISC(labelLeft), 0, 0.5);
	gtk_widget_set_usize(GTK_WIDGET(labelLeft), MARGIN_LABEL_SIZE, -1);
	gtk_box_pack_start(GTK_BOX(hbox3), labelLeft, FALSE, FALSE, 0);
	gtk_widget_show(labelLeft);

	/* TextBox Left */
	textLeftField = gtk_entry_new_with_max_length (MAX_MARGIN_DIGITS);
	gtk_widget_set_usize(textLeftField, MAXMARGIN_ENTRY_LENGTH, -1);
	gtk_entry_set_text(GTK_ENTRY (textLeftField), "");
	gtk_box_pack_start(GTK_BOX(hbox3), textLeftField, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(textLeftField);

	/* Label Left-Points */
	labelLeft_P = gtk_label_new(MARGINS_POINTS);
	gtk_misc_set_alignment(GTK_MISC(labelLeft_P), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox3), labelLeft_P, FALSE, FALSE, 0);
	gtk_widget_show(labelLeft_P);

	/* Label Left-inch1 */
	textLeftField_I = gtk_label_new("");
	gtk_misc_set_alignment(GTK_MISC(textLeftField_I), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox3), textLeftField_I, FALSE, FALSE, 0);
	gtk_widget_show(textLeftField_I);

	/* Label Left-inch2 */
	labelLeft_I = gtk_label_new(MARGINS_INCH);
	gtk_misc_set_alignment(GTK_MISC(labelLeft_I), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox3), labelLeft_I, FALSE, FALSE, 0);
	gtk_widget_show(labelLeft_I);

	/* Label Left-cm1 */
	textLeftField_C = gtk_label_new("");
	gtk_misc_set_alignment(GTK_MISC(textLeftField_C), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox3), textLeftField_C, FALSE, FALSE, 0);
	gtk_widget_show(textLeftField_C);

	/* Label Left-cm2 */
	labelLeft_C = gtk_label_new(MARGINS_MM);
	gtk_misc_set_alignment(GTK_MISC(labelLeft_C), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox3), labelLeft_C, FALSE, FALSE, 0);
	gtk_widget_show(labelLeft_C);

	/* Box */
	hbox3 = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(vbox), hbox3, FALSE, FALSE, 0);
	gtk_widget_show(hbox3);

	/* Label Right */
	labelRight = gtk_label_new(_("Right"));
	gtk_misc_set_alignment(GTK_MISC(labelRight), 0, 0.5);
	gtk_widget_set_usize(GTK_WIDGET(labelRight), MARGIN_LABEL_SIZE, -1);
	gtk_box_pack_start(GTK_BOX(hbox3), labelRight, FALSE, FALSE, 0);
	gtk_widget_show(labelRight);

	/* TextBox Right */
	textRightField = gtk_entry_new_with_max_length (MAX_MARGIN_DIGITS);
	gtk_widget_set_usize(textRightField, MAXMARGIN_ENTRY_LENGTH, -1);
	gtk_entry_set_text(GTK_ENTRY (textRightField), "");
	gtk_box_pack_start(GTK_BOX(hbox3), textRightField, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(textRightField);

	/* Label Right-Points */
	labelRight_P = gtk_label_new(MARGINS_POINTS);
	gtk_misc_set_alignment(GTK_MISC(labelRight_P), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox3), labelRight_P, FALSE, FALSE, 0);
	gtk_widget_show(labelRight_P);

	/* Label Right-inch1 */
	textRightField_I = gtk_label_new("");
	gtk_misc_set_alignment(GTK_MISC(textRightField_I), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox3), textRightField_I, FALSE, FALSE, 0);
	gtk_widget_show(textRightField_I);

	/* Label Right-inch2 */
	labelRight_I = gtk_label_new(MARGINS_INCH);
	gtk_misc_set_alignment(GTK_MISC(labelRight_I), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox3), labelRight_I, FALSE, FALSE, 0);
	gtk_widget_show(labelRight_I);

	/* Label Right-cm1 */
	textRightField_C = gtk_label_new("");
	gtk_misc_set_alignment(GTK_MISC(textRightField_C), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox3), textRightField_C, FALSE, FALSE, 0);
	gtk_widget_show(textRightField_C);

	/* Label Right-cm2 */
	labelRight_C = gtk_label_new(MARGINS_MM);
	gtk_misc_set_alignment(GTK_MISC(labelRight_C), 0, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox3), labelRight_C, FALSE, FALSE, 0);
	gtk_widget_show(labelRight_C);

	/* Box */
	hbox4 = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox4, FALSE, FALSE, 0);
	gtk_widget_show(hbox4);

	g_signal_connect(G_OBJECT(GTK_ENTRY(textTopField)), "changed", G_CALLBACK(button_textmarginschanged), (void *)"Top");
	g_signal_connect(G_OBJECT(GTK_ENTRY(textBottomField)), "changed", G_CALLBACK(button_textmarginschanged), (void *)"Bottom");
	g_signal_connect(G_OBJECT(GTK_ENTRY(textLeftField)), "changed", G_CALLBACK(button_textmarginschanged), (void *)"Left");
	g_signal_connect(G_OBJECT(GTK_ENTRY(textRightField)), "changed", G_CALLBACK(button_textmarginschanged), (void *)"Right");
}

void Printtext::WantPrettyCheck(gpointer data)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prettyCheck)))
	{
			prettyprint = 0;
	}
	else
	{
			prettyprint = 1;
	}
}

void Printtext::WantWrapCheck(GtkWidget *widget, gpointer data)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wrapCheck)))
	{
		textwrap = 0;
	}
	else
	{
		textwrap = 1;
	}
}

void Printtext::TextForm(GtkWidget *container)
{
	GtkWidget *mainHBox, *vbox, *hbox;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	textPrettyFrame = gtk_frame_new(_("Text formatting"));
	gtk_box_pack_start(GTK_BOX(mainHBox), textPrettyFrame, TRUE, TRUE, FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(textPrettyFrame), hbox);
	gtk_widget_show(hbox);

	/* Box */
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
	gtk_widget_show(vbox);

	/* Check-Buttons */
	prettyCheck = gtk_check_button_new_with_label(_("Use Pretty Print"));
	gtk_box_pack_start(GTK_BOX(vbox), prettyCheck, FALSE, FALSE, 0);
	gtk_widget_show(prettyCheck);

	wrapCheck = gtk_check_button_new_with_label(_("Wrap text"));
	gtk_box_pack_start(GTK_BOX(vbox), wrapCheck, FALSE, FALSE, 0);
	gtk_widget_show(wrapCheck);
	g_signal_connect(G_OBJECT(prettyCheck), "pressed", G_CALLBACK(button_wantprettycheck), this);
	g_signal_connect(G_OBJECT(wrapCheck), "pressed", G_CALLBACK(button_wantprettycheck), this);
}


void Printtext::TextTab(void)
{
	textTab = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(textTab), FRAME_BORDER);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), textTab, gtk_label_new(_("Text")));

	TextSizes(textTab);
	MarginSizes(textTab);
	TextForm(textTab);
}

void Printtext::ButtonTextSizesChange(GtkWidget *button, gpointer data)
{
	TextSizesChanged(data);
}

void Printtext::ButtonReset(GtkWidget *button, gpointer data)
{
	Printfunctions::GetInstance()->ResetButton(data);
}

void Printtext::ButtonTextMarginsChanged(GtkWidget *button, gpointer data)
{
	TextMarginsChanged(data);
}

void Printtext::ButtonWantPrettyCheck(GtkWidget *button,  gpointer data)
{
	WantPrettyCheck(data);
}
