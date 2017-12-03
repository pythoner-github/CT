/*
 * Printgeneral.cpp
 *
 *  Created on: 2012-8-3
 *      Author: yangyaoqing
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include <cups/ppd.h>
#include "Printgeneral.h"
#include "Printmain.h"
#include "Printfunctions.h"

GtkWidget *Printgeneral::generalTab = NULL;
GtkWidget *Printgeneral::printerFrame = NULL;
GtkWidget *Printgeneral::printerFrameCombo = NULL;
GtkWidget *Printgeneral::printerFrameInfoText = NULL;
GtkWidget *Printgeneral::printerFrameLocationText = NULL;
GtkWidget *Printgeneral::printerNumCopiesFrame = NULL;
GtkWidget *Printgeneral::printerNumField = NULL;
GtkWidget *Printgeneral::collateCheck = NULL;
GtkWidget *Printgeneral::printerMediaFrame = NULL;
GtkWidget *Printgeneral::printerMediaSizeCombo = NULL;
GtkWidget *Printgeneral::printerMediaTypeCombo = NULL;
GtkWidget *Printgeneral::printerMediaSourceCombo = NULL;
GtkWidget *Printgeneral::MediaTypeFrame = NULL;
GtkWidget *Printgeneral::MediaSizeFrame = NULL;
GtkWidget *Printgeneral::MediaSourceFrame = NULL;
GtkWidget *Printgeneral::printerDuplexFrame = NULL;
GSList *Printgeneral::DuplexButtons = NULL;

Printgeneral* Printgeneral::m_ptrInstance = NULL;
Printgeneral* Printgeneral::GetInstance()
{
	if(m_ptrInstance == NULL)
	m_ptrInstance = new Printgeneral;
	return m_ptrInstance;
}

Printgeneral::Printgeneral(void)
{

}

Printgeneral::~Printgeneral(void)
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

void Printgeneral::GeneralTabSetDefaults(void)
{
	GList *mediaSize = NULL, *mediaType = NULL, *mediaSource = NULL;
	int i1, flg;
	ppd_option_t *option;
	ppd_choice_t *choice;
	//char  buf[128];

	/* PrinterName */
	gtk_list_select_item(GTK_LIST(GTK_COMBO(printerFrameCombo)->list) , PrinterChoice);
	/* PrinterInfo */
	//gtk_label_set_text(GTK_LABEL(printerFrameInfoText), _(PrinterInfos[PrinterChoice]));
	/* PrinterLocation */
	//snprintf(buf, 128, "%s: %s", _("Location"), _(PrinterLocations[PrinterChoice]));
	//gtk_label_set_text(GTK_LABEL(printerFrameLocationText), buf);
	/* Number of Copies */
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(printerNumField), (gfloat)NumberOfCopies);
	/* Collate Copies */
	if(collate)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(collateCheck), 1);
	}
	else
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(collateCheck), 0);
	}

	/* Media Types */
	if((ppdFindOption(printerPPD, "PageSize") == NULL) &&
	   (ppdFindOption(printerPPD, "MediaType") == NULL) &&
	   (ppdFindOption(printerPPD, "InputSlot") == NULL) &&
	   (ppdFindOption(printerPPD, "PageRegion") == NULL))
		gtk_widget_hide(printerMediaFrame);
	else
	{
		gtk_widget_show(printerMediaFrame);

		/* Look if PageSize or PageRegion is marked! */
		flg = 0;
		if((option = ppdFindOption(printerPPD, "PageSize")) !=  NULL)
		{
			for(i1 = option->num_choices, choice = option->choices; i1>0; i1--, choice++)
			{
				if(choice -> marked)
					flg = 1;
			}
		}

		if(flg == 0) { /* PageSize should win over PageRegion(PPD-Specs) */
			if((option = ppdFindOption(printerPPD, "PageRegion")) !=  NULL)
			{
				for(i1 = option->num_choices, choice = option->choices; i1>0; i1--, choice++)
				{
					if(choice -> marked)
						flg = 2;
				}
			}
		}

		if(flg != 2) /* PageSize or nothing is marked, use PageSize */
		{
			option = ppdFindOption(printerPPD, "PageSize");
			for(i1 = option->num_choices, choice = option->choices; i1>0; i1--, choice++)
			{
				mediaSize = g_list_append(mediaSize, _(choice->text));
				if(choice -> marked)
					MediaSizeSelected = option->num_choices-i1;
			}
			gtk_combo_set_popdown_strings(GTK_COMBO(printerMediaSizeCombo), mediaSize);
			gtk_list_select_item(GTK_LIST(GTK_COMBO(printerMediaSizeCombo)->list), MediaSizeSelected);
			gtk_entry_set_position(GTK_ENTRY(GTK_COMBO(printerMediaSizeCombo)->entry), 0);
			gtk_widget_show(MediaSizeFrame);
		}
		else
		{
			if(flg == 2) /* PageRegion marked, PageSize not */
			{
				option = ppdFindOption(printerPPD, "PageRegion");
				for(i1 = option->num_choices, choice = option->choices; i1>0; i1--, choice++)
				{
					mediaSize = g_list_append(mediaSize, _(choice->text));
					if(choice -> marked)
						MediaSizeSelected = option->num_choices - i1;
				}
				gtk_combo_set_popdown_strings(GTK_COMBO(printerMediaSizeCombo), mediaSize);
				gtk_list_select_item(GTK_LIST(GTK_COMBO(printerMediaSizeCombo)->list), MediaSizeSelected);
				gtk_entry_set_position(GTK_ENTRY(GTK_COMBO(printerMediaSizeCombo)->entry), 0);
				gtk_widget_show(MediaSizeFrame);
			} else { /* We dont have PageSize and we dont have PageRegion, broken PPD! */
				gtk_widget_hide(MediaSizeFrame);
			}
		}
		if((option = ppdFindOption(printerPPD, "MediaType")) != NULL)
		{
			for(i1 = option->num_choices, choice=option->choices; i1>0; i1--, choice++)
			{
				mediaType = g_list_append(mediaType, _(choice->text));
				if(choice -> marked)
					MediaTypeSelected = option->num_choices - i1;
			}
			gtk_combo_set_popdown_strings(GTK_COMBO(printerMediaTypeCombo), mediaType);
			gtk_list_select_item(GTK_LIST(GTK_COMBO(printerMediaTypeCombo)->list), MediaTypeSelected);
			gtk_entry_set_position(GTK_ENTRY(GTK_COMBO(printerMediaTypeCombo)->entry), 0);
			gtk_widget_show(MediaTypeFrame);
		}
		else
		{
			gtk_widget_hide(MediaTypeFrame);
		}
		if((option = ppdFindOption(printerPPD, "InputSlot")) != NULL)
		{
			for(i1 = option->num_choices, choice = option->choices; i1>0; i1--, choice++)
			{
				mediaSource = g_list_append(mediaSource, _(choice->text));
				if(choice -> marked)
					MediaSourceSelected = option->num_choices-i1;
			}
			gtk_combo_set_popdown_strings(GTK_COMBO(printerMediaSourceCombo), mediaSource);
			gtk_list_select_item(GTK_LIST(GTK_COMBO(printerMediaSourceCombo)->list), MediaSourceSelected);
			gtk_entry_set_position(GTK_ENTRY(GTK_COMBO(printerMediaSourceCombo)->entry), 0);
			gtk_widget_show(MediaSourceFrame);
		}
		else
		{
			gtk_widget_hide(MediaSourceFrame);
		}
	}

#if 0
	/* Duplex */
	if((option = ppdFindOption(printerPPD, "Duplex")) ==  NULL)
	{
		//gtk_widget_hide(printerDuplexFrame);
	}
	else
	{
		choice = ppdFindChoice(option, "None");
		if(choice != NULL)
		{
			if(choice->marked)
			{
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_slist_nth_data(DuplexButtons, 2)), TRUE);
			}
			else
			{
				choice = ppdFindChoice(option, "DuplexNoTumble");
				if(choice != NULL)
				{
					if(choice->marked)
					{
						gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_slist_nth_data(DuplexButtons, 1)), TRUE);
					}
					else
					{
						choice = ppdFindChoice(option, "DuplexTumble");
						if(choice != NULL)
						{
							if(choice->marked)
								gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(g_slist_nth_data(DuplexButtons, 0)), TRUE);
						}
					}
				}
			}
		}
		gtk_widget_show(printerDuplexFrame);
	}
#endif
}

void Printgeneral::NumOfCopiesChanged(GtkWidget *widget)
{
	gtk_spin_button_update(GTK_SPIN_BUTTON(printerNumField));
	NumberOfCopies = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(printerNumField));
}

void Printgeneral::WantCollateCheck(GtkWidget *widget)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(collateCheck)))
	{
		collate = 0;
	}
	else
	{
		collate = 1;
	}
}

void Printgeneral::PrinterChanged(gpointer data)
{
	int i1, strangeerrortrap, ret, merkconflict;
	char tmp[DEF_PRN_LEN+1];

	snprintf(tmp, (size_t)DEF_PRN_LEN, "%s", gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(printerFrameCombo)->entry)));

	if(tmp[0] == (uintptr_t)NULL) {
		return;
	}

	merkconflict = conflict_active;
	conflict_active = 0;
	i1 = 0;
	strangeerrortrap = 0;
	do
	{
		if(strcmp(_(PrinterNames[i1]), tmp) ==  0)
		{
			lastPrinterChoice = PrinterChoice;
			PrinterChoice = i1;
			strangeerrortrap = 1;
		}
		i1++;
	}
	while(PrinterNames[i1][0] != (uintptr_t)NULL);

	if(strangeerrortrap ==  0)
	{
		;
	}

	Printfunctions::GetInstance()->GetOptions(NULL, tmp, -1);
	ret = Printfunctions::GetInstance()->GetPPDOpts(tmp);
	if(ret ==  -2)		/* Not Authorized */
	{
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(printerFrameCombo)->entry), PrinterNames[lastPrinterChoice]);
	}
	if(ret ==  0)  /* OK, printer has ppd */
	{
		Printfunctions::GetInstance()->GetOptions(NULL, tmp, 0);
		gtk_widget_hide(tabs);
		Printfunctions::GetInstance()->SetDefaults();
		gtk_widget_show(tabs);
	}
	conflict_active = merkconflict;
	if(printerPPD != NULL)
		Printfunctions::GetInstance()->Constraints(ppdConflicts(printerPPD));
}

void Printgeneral::PrinterMedia(GtkWidget *container)
{
	GtkWidget *mainHBox, *vbox;
	GtkWidget *labelSize, *labelType, *labelSource;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	printerMediaFrame = gtk_frame_new(_("Media Selections"));
	gtk_box_pack_start(GTK_BOX(mainHBox), printerMediaFrame, TRUE, TRUE, FRAME_SPACING_V);

	/* Box */
	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 8);
	gtk_container_add(GTK_CONTAINER(printerMediaFrame), vbox);
	gtk_widget_show(vbox);

	/* MediaSize */
	MediaSizeFrame = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(vbox), MediaSizeFrame);

	labelSize = gtk_label_new(_("Media Size"));
	gtk_misc_set_alignment(GTK_MISC(labelSize), 0, 0.5);
	gtk_widget_set_usize(GTK_WIDGET(labelSize), MEDIA_FIELDLENGTH, -1);
	gtk_box_pack_start(GTK_BOX(MediaSizeFrame), labelSize, FALSE, FALSE, 0);
	gtk_widget_show(labelSize);

	printerMediaSizeCombo = gtk_combo_new();
	gtk_list_set_selection_mode(GTK_LIST(GTK_COMBO(printerMediaSizeCombo)->list), GTK_SELECTION_SINGLE);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(printerMediaSizeCombo)->entry), FALSE);
	gtk_box_pack_start(GTK_BOX(MediaSizeFrame), printerMediaSizeCombo, FALSE, FALSE, 0);
	gtk_widget_show(printerMediaSizeCombo);

	/* MediaType */
	MediaTypeFrame = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(vbox), MediaTypeFrame);

	labelType = gtk_label_new(_("Media Type"));
	gtk_misc_set_alignment(GTK_MISC(labelType), 0, 0.5);
	gtk_widget_set_usize(GTK_WIDGET(labelType), MEDIA_FIELDLENGTH, -1);
	gtk_box_pack_start(GTK_BOX(MediaTypeFrame), labelType, FALSE, FALSE, 0);
	gtk_widget_show(labelType);

	printerMediaTypeCombo = gtk_combo_new();
	gtk_list_set_selection_mode(GTK_LIST(GTK_COMBO(printerMediaTypeCombo)->list), GTK_SELECTION_SINGLE);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(printerMediaTypeCombo)->entry), FALSE);
	gtk_box_pack_start(GTK_BOX(MediaTypeFrame), printerMediaTypeCombo, FALSE,  FALSE, 0);
	gtk_widget_show(printerMediaTypeCombo);

	/* MediaSource */
	MediaSourceFrame = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(vbox), MediaSourceFrame);

	labelSource = gtk_label_new(_("Media Source"));
	gtk_misc_set_alignment(GTK_MISC(labelSource), 0, 0.5);
	gtk_widget_set_usize(GTK_WIDGET(labelSource), MEDIA_FIELDLENGTH, -1);
	gtk_box_pack_start(GTK_BOX(MediaSourceFrame), labelSource, FALSE, FALSE, 0);
	gtk_widget_show(labelSource);

	printerMediaSourceCombo = gtk_combo_new();
	gtk_list_set_selection_mode(GTK_LIST(GTK_COMBO(printerMediaSourceCombo)->list), GTK_SELECTION_SINGLE);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(printerMediaSourceCombo)->entry), FALSE);
	gtk_box_pack_start(GTK_BOX(MediaSourceFrame), printerMediaSourceCombo, FALSE, FALSE, 0);
	gtk_widget_show(printerMediaSourceCombo);

	g_signal_connect(G_OBJECT(GTK_COMBO(printerMediaSizeCombo)->entry), "changed", G_CALLBACK(button_ppd_drop_down_change), this);//(void *)"PageSize");
	g_signal_connect(G_OBJECT(GTK_COMBO(printerMediaTypeCombo)->entry), "changed", G_CALLBACK(button_ppd_drop_down_change), this);//(void *)"MediaType");
	g_signal_connect(G_OBJECT(GTK_COMBO(printerMediaSourceCombo)->entry), "changed", G_CALLBACK(button_ppd_drop_down_change), this);//(void *)"InputSlot");
}

void Printgeneral::PrinterNumCopies(GtkWidget *container)
{
	GtkWidget *mainHBox, *hbox;
	GtkObject *adjust;

	mainHBox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, 0);
	gtk_widget_show(mainHBox);

	printerNumCopiesFrame = gtk_frame_new(_("Number of Copies"));
	gtk_box_pack_start(GTK_BOX(mainHBox), printerNumCopiesFrame, TRUE, TRUE, FRAME_SPACING_V);

	/* Box */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), INFRAME_SPACING_H);
	gtk_container_add(GTK_CONTAINER(printerNumCopiesFrame), hbox);
	gtk_widget_show(hbox);

	/* Spinbutton */
    adjust = gtk_adjustment_new(1.0, 1.0, 999, 1.0, 1.0, 0);
	printerNumField = gtk_spin_button_new(GTK_ADJUSTMENT(adjust), 1.0, 0);
	gtk_widget_set_usize(printerNumField, 3 * 16, -1);
	gtk_box_pack_start(GTK_BOX(hbox), printerNumField, FALSE, FALSE, 0);
	gtk_widget_show(printerNumField);

	/* collate Check */
	collateCheck = gtk_check_button_new_with_label(_("Collate Copies"));
	gtk_box_pack_end(GTK_BOX(hbox), collateCheck, FALSE, FALSE, 0);
	gtk_widget_show(collateCheck);

	/* Signals */
	g_signal_connect(G_OBJECT(GTK_SPIN_BUTTON(printerNumField)), "value-changed", G_CALLBACK(button_numofcopies_change), this);
	g_signal_connect(G_OBJECT(collateCheck), "pressed", G_CALLBACK(button_wantcollate_check), this);
}

void Printgeneral::PrinterFrame(GtkWidget *container)
{
	GtkWidget *mainHBox, *hbox, *vbox;
	GList *printerList = NULL;
	int i1;

	mainHBox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_box_pack_start(GTK_BOX(container), mainHBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(mainHBox);

	printerFrame = gtk_frame_new(_("Printer"));
	gtk_box_pack_start(GTK_BOX(mainHBox), printerFrame, TRUE, TRUE, FRAME_SPACING_V);

	/* Printer Box */
	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), VBOX_BORDER);
	gtk_container_add(GTK_CONTAINER(printerFrame), hbox);
	gtk_widget_show(hbox);

	printerFrameCombo = gtk_combo_new();

	for(i1 = 0; i1 <= PrinterNum; i1++)
	{
		printerList = g_list_append(printerList, _(PrinterNames[i1]));
	}
	gtk_combo_set_popdown_strings(GTK_COMBO(printerFrameCombo), printerList);
	gtk_combo_set_value_in_list(GTK_COMBO(printerFrameCombo), TRUE, FALSE);
	gtk_list_set_selection_mode(GTK_LIST(GTK_COMBO(printerFrameCombo)->list), GTK_SELECTION_SINGLE);

	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(printerFrameCombo)->entry), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox), printerFrameCombo, FALSE, FALSE, 0);
	gtk_widget_show(printerFrameCombo);

	/* InfoBoxen */
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_box_set_spacing(GTK_BOX(vbox), 0);
	gtk_box_pack_end(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
	gtk_widget_show(vbox);

	/* Information */
	printerFrameInfoText = gtk_label_new("");
	gtk_label_set_line_wrap(GTK_LABEL(printerFrameInfoText), TRUE);
	gtk_label_set_justify(GTK_LABEL(printerFrameInfoText), GTK_JUSTIFY_LEFT);
	gtk_widget_set_usize(printerFrameInfoText, PRINTER_INFO_FIELD_LENGTH, -1);
	gtk_box_pack_start(GTK_BOX(vbox), printerFrameInfoText, FALSE, FALSE, 0);
	gtk_widget_show(printerFrameInfoText);

	/* Location */
	printerFrameLocationText = gtk_label_new("");
	gtk_label_set_line_wrap(GTK_LABEL(printerFrameLocationText), TRUE);
	gtk_label_set_justify(GTK_LABEL(printerFrameLocationText), GTK_JUSTIFY_LEFT);
	gtk_widget_set_usize(printerFrameLocationText, PRINTER_INFO_FIELD_LENGTH, -1);
	gtk_box_pack_start(GTK_BOX(vbox), printerFrameLocationText, FALSE, FALSE, 0);
	gtk_widget_show(printerFrameLocationText);

	/* Signals */
	g_signal_connect(G_OBJECT(GTK_COMBO(printerFrameCombo)->entry), "changed", G_CALLBACK(button_print_change), this);
}

void Printgeneral::GeneralTab(void)
{
	generalTab = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(generalTab), FRAME_BORDER);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), generalTab, gtk_label_new(_("General")));

	PrinterFrame(generalTab);
	PrinterNumCopies(generalTab);
	PrinterMedia(generalTab);
}

void Printgeneral::ButtonPrinterChange(GtkWidget *button, gpointer data)
{
	PrinterChanged(data);
}

void Printgeneral::ButtonPPDDropDownChanged(GtkWidget *button, gpointer data)
{
	Printfunctions::GetInstance()->PPD_DropDown_changed(button, data);
}

void Printgeneral::ButtonNumOfCopiesChanged(GtkWidget * button,  GtkWidget *widget)
{
	NumOfCopiesChanged(widget);
}

void Printgeneral::ButtonWantCollateCheck(GtkWidget * button,  GtkWidget *widget)
{
	WantCollateCheck(widget);
}

