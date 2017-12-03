/*
 * Printmain.cpp
 *
 *  Created on: 2012-8-3
 *      Author: yangyaoqing
 */
#include <stdio.h>
#include <cups/cups.h>
#include <cups/language.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <locale.h>
#include <libgen.h>
#include <gui_func.h>
#include "Printmain.h"
#include "Printfunctions.h"
#include "KeyValueOpr.h"
#include "ViewReport.h"
#include "ViewDialog.h"

Printmain* Printmain::m_ptrInstance = NULL;
Printmain* Printmain::GetInstance()
{
	if(m_ptrInstance == NULL)
	m_ptrInstance = new Printmain;
	return m_ptrInstance;
}

Printmain::Printmain(void)
{
}

Printmain::~Printmain(void)
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

void Printmain::ButtonPrint(GtkButton *button)
{
	Printfunctions::GetInstance()->PrintFunc(NULL);
}

void Printmain::ButtonReset(GtkButton *button)
{
	Printfunctions::GetInstance()->ResetFunc();
}
void Printmain::ButtonSave(GtkButton *button)
{
	Printfunctions::GetInstance()->SaveGtkLPPrefs(button);
}

void Printmain::ButtonAbbruch(GtkButton *button)
{
	if(GTK_IS_WIDGET(mainWindow)){
		g_keyInterface.Pop();
		gtk_widget_destroy(mainWindow);
	}
}

void Printmain::ButtonDestroy(GtkButton *button)
{
	if(GTK_IS_WIDGET(mainWindow)){
	    g_keyInterface.Pop();
	    gtk_widget_destroy(mainWindow);
	}
}

void Printmain::ViewPrintWindowCreat(GtkWindow *parent)
{
	GtkWidget *mainVBox = NULL;
	GtkWidget *buttonBox, *buttonPrint, *buttonAbbruch, *buttonReset, *buttonSave;

	num_commandline_opts = 0;
	commandline_opts = NULL;
	DEF_PRN[0] = (uintptr_t)NULL;
	confdir[0] = (uintptr_t)NULL;
	ServerName[0] =(uintptr_t)NULL;
	ReallyPrint = 1;
	cmdNumCopies = -1;
	builtin = 0;
	tabtoopen = 1;

	mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);//0
	gtk_window_set_position(GTK_WINDOW(mainWindow), GTK_WIN_POS_CENTER_ALWAYS);//2
	//gtk_widget_set_usize(mainWindow, WAIT_WIN_SIZE, -1);
	gtk_window_set_title(GTK_WINDOW (mainWindow), _("PRINTER SETTING"));
	gtk_widget_set_size_request (mainWindow, 800, 485);
	gtk_window_set_modal (GTK_WINDOW(mainWindow), TRUE);//3
	gtk_window_set_resizable (GTK_WINDOW (mainWindow), FALSE);//4
	gtk_window_set_transient_for(GTK_WINDOW(mainWindow), parent);//5
	gtk_window_set_decorated (GTK_WINDOW (mainWindow), TRUE);//6
	g_signal_connect (G_OBJECT (mainWindow), "delete_event", G_CALLBACK (button_destroy_clicked), this);
	gtk_container_set_border_width (GTK_CONTAINER (mainWindow), 10);
	gtk_widget_show_all(mainWindow);
	mainVBox = gtk_vbox_new(FALSE, BUTTON_SPACING_H);
	gtk_container_add(GTK_CONTAINER(mainWindow), mainVBox);

	cupsLanguage = cupsLangDefault();

	Printfunctions::GetInstance()->GetPaths();//USER CONFIGURE
	//保存文件打开的路径
	fileOpenPath[0] = (uintptr_t)NULL;
	getcwd(fileOpenPath, MAXPATH);
	strncat(fileOpenPath,"/", (size_t)MAXPATH);
	Printfunctions::GetInstance()->GetOptions(NULL, DEF_PRN, 2);
	GetPrinters(0);
	if(DEF_PRN[0] == (uintptr_t)NULL)
	{
		gtk_widget_destroy(mainWindow);
		return
		ViewDialog::GetInstance()->Create(GTK_WINDOW(parent), ViewDialog::INFO, _("No printer found or specified."), NULL);
	}
	Printfunctions::GetInstance()->GetPPDOpts(DEF_PRN);
	Printfunctions::GetInstance()->GetOptions(NULL, DEF_PRN,-1);

	/* Tab */
	tabs = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(mainVBox), tabs, TRUE, TRUE, 0);
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(tabs), GTK_POS_TOP );
	buttonBox=gtk_hbox_new(TRUE, BUTTON_SPACING_H);
	gtk_box_pack_end(GTK_BOX(mainVBox), buttonBox, FALSE, FALSE, 0);

	buttonReset = gtk_button_new_from_stock(GTK_STOCK_REVERT_TO_SAVED);
	Button_pad(buttonReset);
	g_signal_connect (G_OBJECT (buttonReset), "clicked", G_CALLBACK (button_reset_clicked), this);
	gtk_box_pack_start(GTK_BOX(buttonBox), buttonReset, FALSE, TRUE, 0);

	buttonSave = gtk_button_new_from_stock(GTK_STOCK_SAVE);
	Button_pad(buttonSave);
	g_signal_connect (G_OBJECT (buttonSave), "clicked", G_CALLBACK (button_save_clicked), this);
	gtk_box_pack_start(GTK_BOX(buttonBox), buttonSave, FALSE, TRUE, 0);

	buttonAbbruch = gtk_button_new_from_stock(GTK_STOCK_QUIT);
	Button_pad(buttonAbbruch);
	g_signal_connect (G_OBJECT(buttonAbbruch), "clicked", G_CALLBACK (button_abbruch_clicked), this);
	gtk_box_pack_start(GTK_BOX(buttonBox), buttonAbbruch, FALSE, TRUE, 0);

	Printfunctions::GetInstance()->CreateWidgets();
	Printfunctions::GetInstance()->ShowWidgets();

	gtk_notebook_set_current_page(GTK_NOTEBOOK(tabs), tabtoopen);
	gtk_widget_show_all(buttonBox);

	g_keyInterface.Push(this);
	gtk_widget_show_all(mainWindow);

	conflict_active = 1;
	if(printerPPD != NULL) {
		Printfunctions::GetInstance()->Constraints(ppdConflicts(printerPPD));
	}
}

void Printmain::GetPrintOptions(void)
{
	num_commandline_opts = 0;
	commandline_opts = NULL;
	DEF_PRN[0] = (uintptr_t)NULL;
	confdir[0] = (uintptr_t)NULL;
	ServerName[0] =(uintptr_t)NULL;
	ReallyPrint = 1;
	cmdNumCopies = -1;
	builtin = 0;
	tabtoopen = 1;
	cupsLanguage=cupsLangDefault();
	Printfunctions::GetInstance()->GetPaths();//SAVE USER CONFIGURE
	fileOpenPath[0] = (uintptr_t)NULL;
	getcwd(fileOpenPath, MAXPATH);
	strncat(fileOpenPath,"/", (size_t)MAXPATH);
	Printfunctions::GetInstance()->GetOptions(NULL, DEF_PRN, 2);
	GetPrinters(0);
	Printfunctions::GetInstance()->GetPPDOpts(DEF_PRN);
	Printfunctions::GetInstance()->GetOptions(NULL, DEF_PRN, -1);
	Printfunctions::GetInstance()->CreateWidgets();
	Printfunctions::GetInstance()->ShowWidgets();

	conflict_active = 1;
	if(printerPPD != NULL) {
		Printfunctions::GetInstance()->Constraints(ppdConflicts(printerPPD));
	}
}
