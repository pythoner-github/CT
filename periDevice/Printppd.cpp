/*
 * Printppd.cpp
 *
 *  Created on: 2012-8-13
 *      Author: yangyaoqing
 */
#include <string.h>
#include <gtk/gtk.h>
#include <cups/ppd.h>
#include "Printppd.h"
#include "Printdefaults.h"
#include "Printfunctions.h"

GtkWidget *Printppd::ppdTab = NULL;
GtkWidget *Printppd::ppdFrame = NULL;

int Printppd::ppd_entry_count = 0;
GtkWidget *Printppd::FrameHBox = NULL;
GtkWidget *Printppd::FrameVBox = NULL;

Printppd* Printppd::m_ptrInstance = NULL;
Printppd* Printppd::GetInstance()
{
	if(m_ptrInstance == NULL)
	m_ptrInstance = new Printppd;
	return m_ptrInstance;
}

Printppd::Printppd()
{

}

Printppd::~Printppd(void)
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

int Printppd::TogglePPDOption(ppd_group_t *group, int ct, int nummer, int enable)
{
	int i1,i2;
	ppd_option_t *option;
	ppd_choice_t *choice;
	ppd_group_t *subgroup;

	for(i1 = group->num_options, option = group->options; i1*ct>0; i1--, option++)
	{
		if((strcmp(option->keyword, "PageSize") != 0) &&
		   (strcmp(option->keyword, "InputSlot") != 0) &&
		   (strcmp(option->keyword, "MediaType") != 0) &&
		   //(strcmp(option->keyword, "Duplex") != 0) &&
			 (strcmp(option->keyword, "Copies") != 0) &&
		   (strcmp(option->keyword, "Collate") != 0) &&
			(strcmp(option->keyword, "PageRegion") != 0)) /* PageRegion seems to be the same as PageSize in Cups */
		{
			for(i2 = option->num_choices, choice = option->choices; i2*ct>0; i2--, choice++)
			{
				if(nummer == ct)
				{
					g_print("\nChanged(enable = %i): %i: %s(%s) = %s(%s)\n",
					       enable, ct, option->keyword, option->text, choice->choice, choice->text);
					if(enable == 1)
					{
						Printfunctions::GetInstance()->Constraints(ppdMarkOption(printerPPD, option->keyword, choice->choice));
					}
					else
					{
						choice->marked = 0;
						Printfunctions::GetInstance()->Constraints(ppdConflicts(printerPPD));
					}
					ct = -1;
				}
				else
				{
					ct++;
				}
			}
		}
	}

	for(i1 = group->num_subgroups, subgroup = group->subgroups; i1*ct>0; i1--, subgroup++)
		ct = TogglePPDOption(group, ct, nummer, enable);

	return(ct);
}

void Printppd::EnablePPDOption(int nummer)
{
	int i1;
	int ct;
	ppd_group_t *group;

	g_print("Enable %i\n", nummer);

	ct = 1;
	for(i1 = printerPPD->num_groups, group = printerPPD->groups; i1*ct>0; i1--, group++)
		ct = TogglePPDOption(group, ct, nummer, 1);
}

void Printppd::DisablePPDOption(int nummer)
{
	int i1;
	int ct;
	ppd_group_t *group;

	g_print("Disable %i\n", nummer);

	ct = 1;
	for(i1 = printerPPD->num_groups, group = printerPPD->groups; i1*ct>0; i1--, group++)
		ct = TogglePPDOption(group, ct, nummer, 0);
}

void Printppd::PPD_Multi_changed(GtkWidget  *widget, int  data)
{
	//printf("Printppd::PPD_Multi_changed\n");
	//g_print("PPD_Multi changed %s -> %i\n", gtk_widget_get_name(widget), ppd_entry_count);//data
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		EnablePPDOption(ppd_entry_count);//data
	else
		DisablePPDOption(ppd_entry_count);//data
}


void Printppd::AddPPDGroup(ppd_group_t *group)
{
    int			 i1, i2;
    int			 marked_choice, default_choice;
    int 		 morethanone;
	ppd_option_t *option;
	ppd_choice_t *choice;
	ppd_group_t *subgroup;
    GtkWidget		*option_box, *label = NULL, *group_frame, *group_box;
    GtkWidget		*ppdWidget, *frame = NULL, *vbox = NULL;
	GList *list;
	unsigned thiswidth, maxwidth;

	morethanone = 0;
	maxwidth = 0;

	/* Option types: PPD_UI_BOOLEAN PPD_UI_PICKONE PPD_UI_PICKMANY */
    group_frame = gtk_frame_new(_(group->text));//general?
    gtk_box_pack_start(GTK_BOX(FrameVBox), group_frame, FALSE, FALSE, FRAME_SPACING_V);
    gtk_widget_show(group_frame);

    group_box = gtk_vbox_new(FALSE, 1);
    gtk_container_add(GTK_CONTAINER(group_frame), group_box);
    gtk_container_set_border_width(GTK_CONTAINER(group_box), VBOX_BORDER);
    gtk_widget_show(group_box);

    for(i1 = 0, option = group->options; i1 < group->num_options; i1++, option++)
    {
    	if((strcmp(option->keyword, "PageSize") != 0) &&
		   (strcmp(option->keyword, "InputSlot") != 0) &&
		   (strcmp(option->keyword, "MediaType") != 0) &&
		   //(strcmp(option->keyword, "Duplex") != 0) &&
		   (strcmp(option->keyword, "Copies") != 0) &&
		   (strcmp(option->keyword, "Collate") != 0) &&
		   (strcmp(option->keyword, "PageRegion") != 0)) /* PageRegion seems to be the same as PageSize in Cups */
    	{
    		morethanone = 1;
    		option_box = gtk_hbox_new(FALSE, 8);
    		gtk_box_pack_start(GTK_BOX(group_box), option_box, FALSE, FALSE, 1);
    		gtk_widget_show(option_box);

    		list = NULL;
    		marked_choice = -1;
    		default_choice = -1;

    		for(i2 = 0, choice = option->choices; i2 < option->num_choices; i2++, choice++)
    		{
#if 0
    			printf("option->choices->marked:%c,---ption->choices->choice:%s,---option->choices->text:%s, ---option->choices->code:%s,\
    					---option->num_choices:%i\n",option->choices->marked, option->choices->choice,option->choices->text,
    					option->choices->code, option->num_choices);
#endif
    			ppd_entry_count++;
    			switch(option->ui)
    			{
    			case PPD_UI_BOOLEAN:
    			case PPD_UI_PICKONE:
    				if(choice->text == NULL) {
    					list = g_list_append(list, _(choice->choice));
    					thiswidth = gdk_string_width(gtk_style_get_font(gtk_widget_get_style(option_box)), _(choice->choice));
    				} else {
    					list = g_list_append(list, _(choice->text));
    					// The default font is used in every widget!
    					thiswidth = gdk_string_width(gtk_style_get_font(gtk_widget_get_style(option_box)), _(choice->text));
    				}
    				if(thiswidth > maxwidth)
    					maxwidth = thiswidth;

    				if (strcmp(_(choice->text), _(option->defchoice)) == 0)
    					default_choice = i2;

    				if (choice->marked)
    					marked_choice = i2;
    				break;
    			case PPD_UI_PICKMANY:
    				if((strcmp(choice->text, "None") == 0) ||
    						(strcmp(choice->choice, "None") == 0))
    				break;

    				if (vbox == NULL)
						{
							vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
							gtk_container_set_border_width(GTK_CONTAINER(vbox), VBOX_BORDER);
							gtk_widget_show(vbox);
						}
    				if(choice->text == NULL)
    					ppdWidget = gtk_check_button_new_with_label(choice->choice);
    				else
    					ppdWidget = gtk_check_button_new_with_label(choice->text);

    				if(choice->marked)
    					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ppdWidget), TRUE);
    				gtk_box_pack_start(GTK_BOX(vbox), ppdWidget, FALSE, FALSE, 0);
    				g_signal_connect(G_OBJECT(ppdWidget), "toggled", G_CALLBACK(button_multi_change), this);//(int *) ppd_entry_count);
    				gtk_widget_show(ppdWidget);
    				break;
				default:
				break;
				}
    		}

    		switch(option->ui)
			{
			case PPD_UI_BOOLEAN:
			case PPD_UI_PICKONE:
#if 1
				if(strcmp(_(option->text), "Double-Sided Printing") != 0){
					label = gtk_label_new(_(option->text));
					gtk_box_pack_start(GTK_BOX(option_box), label, TRUE, TRUE, 1);
					gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
					gtk_widget_show(label);

					ppdWidget = gtk_combo_new();

					if(list == (GList *) NULL)
						list = g_list_append(list, (void *)"");

					gtk_combo_set_popdown_strings (GTK_COMBO(ppdWidget), list);
					gtk_combo_set_value_in_list(GTK_COMBO(ppdWidget), TRUE, FALSE);
					gtk_list_set_selection_mode(GTK_LIST(GTK_COMBO(ppdWidget)->list), GTK_SELECTION_SINGLE);
					gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(ppdWidget)->entry), FALSE);
					if(default_choice >=  0)
						gtk_list_select_item(GTK_LIST(GTK_COMBO(ppdWidget)->list), default_choice);

					if(marked_choice >=  0)
						gtk_list_select_item(GTK_LIST(GTK_COMBO(ppdWidget)->list), marked_choice);
					gtk_box_pack_start(GTK_BOX(option_box), ppdWidget, FALSE, FALSE, 0);
					g_signal_connect(G_OBJECT(GTK_COMBO(ppdWidget)->entry), "changed", G_CALLBACK(button_dropdown_change), _(option->keyword));//(0)_(option->keyword);
					gtk_widget_set_usize(GTK_WIDGET(GTK_ENTRY(GTK_COMBO(ppdWidget)->entry)), Max(maxwidth+COMBO_EXTRA_SPACE, COMBO_MIN_SPACE), -1);
					gtk_widget_show(ppdWidget);
				}
				break;
#endif
			case PPD_UI_PICKMANY:
				frame = gtk_frame_new(_(option->text));
				gtk_container_set_border_width(GTK_CONTAINER(frame), 5);
				gtk_box_pack_start(GTK_BOX(option_box), frame, TRUE, TRUE, 1);
				gtk_widget_show(frame);
				gtk_container_add(GTK_CONTAINER(frame), vbox);
				vbox = NULL;
				break;
			default:
				break;
			}
    	}
    }

    if(morethanone == 0) {
		gtk_widget_destroy(group_frame);
		return;
	}

	for(i1 = group->num_subgroups, subgroup = group->subgroups; i1>0; i1--, subgroup++)
		AddPPDGroup(subgroup);
}

void Printppd::PpdTabSetDefaults(void)
{
	int i1;
	ppd_group_t *group;

	gtk_widget_destroy(FrameVBox);
	FrameVBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(FrameHBox), FrameVBox, TRUE, TRUE, FRAME_SPACING_V);
	gtk_widget_show(FrameVBox);

	ppd_entry_count = 0;

	if(printerPPD != NULL)
	{
		for(i1 = printerPPD->num_groups, group = printerPPD->groups; i1>0; i1--, group++)
			AddPPDGroup(group);
	}
}

void Printppd::PPDFrame(GtkWidget *container)
{
	GtkWidget *frameScrolled;

	frameScrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(frameScrolled), GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);//GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(container), frameScrolled, TRUE, TRUE, 0);
	gtk_widget_show(frameScrolled);

	FrameHBox = gtk_hbox_new(FALSE, 0);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(frameScrolled), FrameHBox);
	gtk_widget_show(FrameHBox);

	FrameVBox = gtk_vbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(FrameHBox), FrameVBox, FALSE, FALSE, FRAME_SPACING_V);
	gtk_widget_show(FrameVBox);
}

void Printppd::PPDTab(void)
{
	ppdTab = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(ppdTab), FRAME_BORDER);
	gtk_notebook_append_page(GTK_NOTEBOOK(tabs), ppdTab, gtk_label_new(_("PPD")));
	PPDFrame(ppdTab);
}

void Printppd::PPD_DropDown_changed(GtkWidget *widget, gpointer data)
{
	//printf("Printppd::PPD_DropDown_change*****\n");
	ppd_choice_t *choice;
	ppd_option_t *option;
	int i0, i1;
    ppd_group_t *group;

    //printf("data:%s\n", (char *)data);
	if(printerPPD != NULL)
	{
		//printf("*********0\n");
		//for(i0 = printerPPD->num_groups, group = printerPPD->groups; i0>0; i0--, group++)
		//{
			//printf("********i1-%d\n", i0);
			if(0) AddPPDGroup(group->subgroups);
			//option = group->options;
			//option = ppdFindOption(printerPPD, option->keyword);
			option = ppdFindOption(printerPPD, (const char *) data);
			//printf("*********1\n");
			if(option != NULL){
				for(i1 = option->num_choices, choice = option->choices; i1 > 0; i1--, choice++)
				{
					//printf("**********2--i1:%d--choice->text:%s\n", i1, choice->text);
					if(strcmp(_(choice->text), gtk_entry_get_text(GTK_ENTRY(widget))) == 0)
					{
						//printf("**********3--choice->text:%s\n", choice->text);
						Printfunctions::GetInstance()->Constraints(ppdMarkOption(printerPPD, option->keyword, choice->choice));
						i1 = 0;
					}
				}
			}

		//}
	}
}
