/*
 * Printlib.cpp
 *
 *  Created on: 2012-8-6
 *      Author: yangyaoqing
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <cups/cups.h>
#include <cups/http.h>
#include <cups/language.h>
#include <X11/Xlib.h>
#include "Printlib.h"
#include "Printdefaults.h"
#include "KeyDef.h"
#include "KeyValueOpr.h"
#include "ViewDialog.h"

#define GTKPRINTERACCEPTDIR "accept"
#define GTKPRINTERDENYDIR "deny"
//int Printlib::DEBUG = 1;
char Printlib::PrinterNames[MAX_PRT][DEF_PRN_LEN+1] = {};
int Printlib::PrinterChoice = 0;
int Printlib::PrinterNum = 0;
int Printlib::lastPrinterChoice = 0;
char Printlib::ServerName[MAX_SERVER_NAME+1] = {};
char Printlib::PrinterBannerStart[MAX_PRT][MAXLINE+1] = {};
char Printlib::PrinterBannerEnd[MAX_PRT][MAXLINE+1] = {};
char Printlib::DEF_PRN[DEF_PRN_LEN+1] = {};
char Printlib::PrinterInfos[MAX_PRT][MAXLINE+1] = {};
char Printlib::PrinterLocations[MAX_PRT][MAXLINE+1] = {};
char Printlib::ExtraOpt[MAX_EXTRAOPT+1] = {};
char Printlib::GTKLPRC[MAXPATH+1] = {};
char Printlib::HELPURL[MAXPATH+1] = {};
char Printlib::JobName[MAX_JOB_NAME_LENGTH+1] = {};
char Printlib::Range[MAXRANGEENTRY+1] = {};
char Printlib::confdir[MAXPATH+1] = {};
char Printlib::fileOpenPath[MAXPATH+1] = {};
char Printlib::filesToPrintAnz = {0};
char *Printlib::PPDfilename = NULL;
unsigned int Printlib::cppvalue = 0;
unsigned int Printlib::lpivalue = 0;
unsigned int Printlib::cpivalue = 0;
unsigned int Printlib::hpgl2pen = 0;
unsigned int  Printlib::topmargin = 0;
unsigned int Printlib::bottommargin = 0;
unsigned int Printlib::leftmargin = 0;
unsigned int Printlib::rightmargin = 0;
int Printlib::NumberOfCopies = 0;
int Printlib::collate = 0;
int Printlib::MediaSizeSelected = 0;
int Printlib::MediaTypeSelected = 0;
int Printlib::MediaSourceSelected = 0;
int Printlib::PPDopen = 0;
int Printlib::ReallyPrint = 0;
int Printlib::builtin = 0;
int Printlib::conflict_active = 0;
int Printlib::clearfilelist = 0;
int Printlib::cmdNumCopies = 0;
int Printlib::saveanz = 0;
int Printlib::oldnumcopies = 0;
int Printlib::ignore_stdin = 0;
int Printlib::imageScalingType = 0;
int Printlib::jobidonexit = 0;
int Printlib::nox = 0;
int Printlib::num_commandline_opts = 0;
int Printlib::prettyprint = 0;
int Printlib::rangeType = 0;
int Printlib::rememberprinter = 0;
int Printlib::remembertab = 0;
int Printlib::saveonexit = 0;
int Printlib::mirroroutput = 0;
int Printlib::sheetsType = 0;
int Printlib::tabtoopen = 0;
int Printlib::wantconst = 0;
int Printlib::wantLandscape = 0;
int Printlib::wantRange = 0;
int Printlib::wantReverseOutputOrder = 0;
int Printlib::wantraw = 0;
int Printlib::viewable[6]= {};
double Printlib::brightness = 0;
double Printlib::ggamma = 0;
double Printlib::hue = 0;
double Printlib::imageScalingNomeValue = 0;
double Printlib::imageScalingScalingValue = 0;
double Printlib::imageScalingNaturalValue = 0;
double Printlib::imageScalingPPIValue = 0;
double Printlib::sat = 0;
int Printlib::textwrap = 0;
int Printlib::mainWindowHeight = 0;
int Printlib::mainWindowWidth = 0;
int Printlib::mainWindowX = 0;
int Printlib::mainWindowY = 0;
int Printlib::wantSaveSizePos = 0;
char Printlib::nupLayout[5] = {};
char Printlib::borderType[13] = {};
cups_option_t *Printlib::commandline_opts = NULL;
ppd_file_t *Printlib::printerPPD = NULL;
GtkWidget *Printlib::tabs = NULL;
GtkWidget *Printlib::dialog = NULL;
GtkWidget *Printlib::mainWindow = NULL;

http_t *Printlib::cupsHttp;
cups_lang_t *Printlib::cupsLanguage;
GtkObject *Printlib::brightAdj = NULL;
GtkObject *Printlib::gammaAdj = NULL;

Printlib* Printlib::m_ptrInstance = NULL;
Printlib* Printlib::GetInstance()
{
	if(m_ptrInstance == NULL)
	m_ptrInstance = new Printlib;
	return m_ptrInstance;
}

Printlib::Printlib(void)
{

}

Printlib::~Printlib(void)
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

void Printlib::Quick_message(gchar *message, int which)
{
	GtkWidget *label, *pixmapwid, *vbox, *hbox;
	GtkStyle *style;

	if(nox == 1) {
		g_print("%s\n", message);
		return;
	}
	while (gtk_events_pending())
		gtk_main_iteration();

	dialog = gtk_dialog_new_with_buttons((const char *)"", GTK_WINDOW(mainWindow), (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
			GTK_STOCK_OK, 0, NULL);

	vbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, MESSAGE_SPACING_V);

	switch(which)
	{
		case 1:		/*--- Info ---*/
			gtk_window_set_title(GTK_WINDOW(dialog), _("Message"));
			pixmapwid = gtk_image_new_from_stock(GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
			gtk_box_pack_start(GTK_BOX(hbox), pixmapwid, FALSE, FALSE, MESSAGE_SPACING_H);
		break;
		case 2:		/*--- Error ---*/
			gtk_window_set_title(GTK_WINDOW(dialog), _("Error"));
			pixmapwid = gtk_image_new_from_stock(GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_DIALOG);
			gtk_box_pack_start(GTK_BOX(hbox), pixmapwid, FALSE, FALSE, MESSAGE_SPACING_H);
		break;
		case 3:         /*--- MyFault ---*/
			gtk_window_set_title(GTK_WINDOW(dialog), _("Fehlerteufel"));
			style  =  gtk_widget_get_style(dialog);
			gtk_box_pack_start(GTK_BOX(hbox), pixmapwid, FALSE, FALSE, MESSAGE_SPACING_H);
		break;
		case 4:         /*--- Warning ---*/
			gtk_window_set_title(GTK_WINDOW(dialog), _("Warning!"));
			pixmapwid = gtk_image_new_from_stock(GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_DIALOG);
			gtk_box_pack_start(GTK_BOX(hbox), pixmapwid, FALSE, FALSE, MESSAGE_SPACING_H);
		break;
		default:
		break;
	}

	label  =  gtk_label_new (message);
	gtk_box_pack_end(GTK_BOX(hbox), label, FALSE, FALSE, MESSAGE_SPACING_H);

	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), vbox);

	gtk_widget_show_all(dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void Printlib::GetPrinters(int loop)
{
	int i1;
	cups_dest_t *dests;
	ipp_t *request, *response;
	ipp_attribute_t *attr;
	char URI[MAX_URI+1];
	char tmpprn[DEF_PRN_LEN+1];
	unsigned droppedPrinters;
	char TMPPATH[MAXPATH+1];
	DIR *testdir;
	FILE *testfile;
	char allowdenyexists;
	char ad;
	double progFrac = 1.0, progress = 1.0;
	droppedPrinters = 0;
	allowdenyexists = -1;
	ad = 1;
	PrinterChoice = -1;
	lastPrinterChoice = -1;

	PrinterNum = cupsGetDests(&dests);
	if(PrinterNum > MAX_PRT)
	{
		ExitOnError(_("You have installed too many printers or templates !"), (char *)"", -3);
	}
	PrinterNum--;

	snprintf(TMPPATH, (size_t)MAXPATH, "%s/%s/", DEF_GTKLPRC, GTKPRINTERACCEPTDIR);
	testdir = opendir(TMPPATH);
	if (testdir != (DIR *)NULL) {
		allowdenyexists = 0;
		ad = -1;
		closedir(testdir);
	} else {
		snprintf(TMPPATH, (size_t)MAXPATH, "%s/%s/", DEF_GTKLPRC, GTKPRINTERDENYDIR);
		testdir = opendir(TMPPATH);
		if (testdir != (DIR *)NULL) {
			allowdenyexists = 1;
			ad = 1;
			closedir(testdir);
		}	else {
			allowdenyexists = 2;
			ad = 1;
		}
	}

	progFrac = 0.8/PrinterNum;
	progress = 0.1;

	for(i1 = 0;i1 <= PrinterNum; i1++)
	{
		if(dests[i1].instance == (char *)NULL)
			snprintf(PrinterNames[i1-droppedPrinters], (size_t)DEF_PRN_LEN, "%s", dests[i1].name);
		else
			snprintf(PrinterNames[i1-droppedPrinters], (size_t)DEF_PRN_LEN, "%s/%s", dests[i1].name, dests[i1].instance);

		if(allowdenyexists == 0) {
			snprintf(TMPPATH, (size_t)MAXPATH, "%s/%s/%s", DEF_GTKLPRC, GTKPRINTERACCEPTDIR, PrinterNames[i1-droppedPrinters]);
    	    //printf("jhuang : path = %s\n", TMPPATH);

        testfile = fopen(TMPPATH, "r");
		if(testfile == (FILE *)NULL) {
				ad = -1;
		} else {
			fclose(testfile);
			snprintf(TMPPATH, (size_t)MAXPATH, "%s/%s/%s/%s", getenv("HOME"), GTKLPRC_USER, GTKPRINTERDENYDIR, PrinterNames[i1-droppedPrinters]);
    	    //printf("jhuang : path = %s\n", TMPPATH);
            testfile = fopen(TMPPATH, "r");
			if(testfile != (FILE *)NULL) {
				fclose(testfile);
				ad = -1;
			} else {
				ad = 1;
			}
		}
	}

	if(allowdenyexists == 1) {
		snprintf(TMPPATH, (size_t)MAXPATH, "%s/%s/%s", DEF_GTKLPRC, GTKPRINTERDENYDIR, PrinterNames[i1-droppedPrinters]);
    	    //printf("jhuang : path = %s\n", TMPPATH);

		testfile = fopen(TMPPATH, "r");
		if(testfile != (FILE *)NULL) {
			fclose(testfile);
			ad = -1;
		} else {
			snprintf(TMPPATH, (size_t)MAXPATH, "%s/%s/%s/%s", getenv("HOME"), GTKLPRC_USER, GTKPRINTERDENYDIR, PrinterNames[i1-droppedPrinters]);
    	    //printf("jhuang : path = %s\n", TMPPATH);

            testfile = fopen(TMPPATH, "r");
			if(testfile != (FILE *)NULL) {
				fclose(testfile);
				ad = -1;
			} else {
				ad = 1;
			}
		}
	}

	if(ad == -1) {
		droppedPrinters++;
	} else {
		if((dests[i1].is_default == 1) & (DEF_PRN[0] == (uintptr_t)NULL))
		{
			if(dests[i1].instance == (uintptr_t)NULL) {
				strncpy(DEF_PRN, dests[i1].name, (size_t)DEF_PRN_LEN);
			}
			else {
				snprintf(DEF_PRN, (size_t)DEF_PRN_LEN, "%s/%s", dests[i1].name, dests[i1].instance);
			}
		}
		if(DEF_PRN[0] != (uintptr_t)NULL)
		{
			if(dests[i1].instance != NULL)
				snprintf(tmpprn, (size_t)DEF_PRN_LEN, "%s/%s", dests[i1].name, dests[i1].instance);
			else
				snprintf(tmpprn, (size_t)DEF_PRN_LEN, "%s", dests[i1].name);
			if(strcmp(DEF_PRN, tmpprn) == 0)
				PrinterChoice = i1-droppedPrinters;
		}
		snprintf(URI, (size_t)MAX_URI, "ipp://%s/printers/%s", ServerName, dests[i1].name);
		request = ippNew();
		request->request.op.operation_id = IPP_GET_PRINTER_ATTRIBUTES;
		request->request.op.request_id = 1;
		ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_CHARSET, "attributes-charset", NULL, cupsLangEncoding(cupsLanguage));
		ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_LANGUAGE, "attributes-natural-language", NULL, cupsLanguage->language);
		ippAddString(request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri", NULL, URI);
		if((response = cupsDoRequest(cupsHttp, request, "/")) == NULL)
		{
			;
		}
		if((attr = ippFindAttribute(response, "job-sheets-default", IPP_TAG_ZERO)) != NULL)
		{
			strncpy(PrinterBannerStart[i1-droppedPrinters], attr->values[0].string.text, (size_t)MAXLINE);
			strncpy(PrinterBannerEnd[i1-droppedPrinters], attr->values[1].string.text, (size_t)MAXLINE);
		}
		else
		{
			strncpy(PrinterBannerStart[i1-droppedPrinters], "none", (size_t)MAXLINE);
			strncpy(PrinterBannerEnd[i1-droppedPrinters], "none", (size_t)MAXLINE);
		}
		if((attr = ippFindAttribute(response, "printer-location", IPP_TAG_ZERO)) != NULL)
		{
			strncpy(PrinterLocations[i1-droppedPrinters], attr->values[0].string.text, (size_t)MAXLINE);
		}
		else
		{

		}
		if((attr = ippFindAttribute(response, "printer-info", IPP_TAG_ZERO)) != NULL)
		{
			strncpy(PrinterInfos[i1-droppedPrinters], attr->values[0].string.text, (size_t)MAXLINE);
		}
		else
		{
			;
		}
		ippDelete(response);
		}
    }
	//extern GtkWidget *GetMainWindow(void);
	if(DEF_PRN[0] == (uintptr_t)NULL)
	{
		strncpy(DEF_PRN, PrinterNames[0], (size_t)DEF_PRN_LEN);
		PrinterChoice = 0;
	}
	if(PrinterChoice == -1)
	{
		if(loop == 1)
		{
			Gtklp_fatal_end(_("The given printer does not exist!"));
		}
		DEF_PRN[0] = (uintptr_t)NULL;
		GetPrinters(loop+1);
	}

	PrinterNum -= droppedPrinters;
}

void Printlib::Gtklp_fatal_end(char *text)
{
	cupsLangFlush();
	cupsLanguage  =  NULL;
	httpClose(cupsHttp);
	cupsHttp  =  NULL;
	//printf("\n%s\n\n", text);
	exit(-3);
}

int Printlib::Max(int a, int b)
{
	if(a > b)
		return(a);
	else
		return(b);
}

void Printlib::Button_pad(GtkWidget  *widget)
{
	GtkWidget *lab  =  GTK_BIN(widget)->child;
	if (GTK_IS_LABEL(lab))
		gtk_misc_set_padding(GTK_MISC(lab), BUTTON_PAD, 0);
}

void Printlib::ExitOnError(char *gerror1, char *gerror2, int exitcode)
{
	GtkWidget *exitErrorDialog;
	GtkWidget *hbox, *label, *pixmapwid;
	char tmplabel[MAXLINE+1];

	if(nox == 0) {
		exitErrorDialog = gtk_dialog_new_with_buttons(_("Error"), GTK_WINDOW(mainWindow),
				(GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT), GTK_STOCK_OK, 0, NULL);
		hbox = gtk_hbox_new(0, INFRAME_SPACING_H);
		gtk_container_add (GTK_CONTAINER (GTK_DIALOG(exitErrorDialog)->vbox), hbox);
		gtk_widget_show(hbox);

		pixmapwid = gtk_image_new_from_stock(GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_DIALOG);
		gtk_box_pack_start(GTK_BOX(hbox), pixmapwid, FALSE, FALSE, MESSAGE_SPACING_H);
		gtk_widget_show(pixmapwid);

		if(strlen(gerror2) == 0)
			snprintf(tmplabel, (size_t)MAXLINE, gerror1);
		else
			snprintf(tmplabel, (size_t)MAXLINE, gerror1, gerror2);
		label = gtk_label_new(tmplabel);
		gtk_box_pack_end(GTK_BOX(hbox), label, FALSE, FALSE, FRAME_SPACING_V);
		gtk_widget_show(label);
		gtk_dialog_run(GTK_DIALOG(exitErrorDialog));

	} else {
		if(strlen(gerror2) == 0)
			g_warning(gerror1);
		else
			g_warning(gerror1, gerror2);
	}
}

#if 1
void Printlib::BtnCancelClicked(GtkButton *button)
{
    g_keyInterface.Pop();
    gtk_widget_destroy(mainWindow);
}

void Printlib::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);

    switch(keyValue) {
    case KEY_ESC:
	BtnCancelClicked(NULL);
	break;
    default:
	break;
    }
}
#endif
