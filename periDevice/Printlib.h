/*
 * Printlib.h
 *
 *  Created on: 2012-8-6
 *      Author: yangyaoqing
 */

#ifndef PRINTLIB_H_
#define PRINTLIB_H_
#include <glib.h>
#include <gtk/gtk.h>
#ifndef _LOCALE_H
#define _LOCALE_H
#endif
#include <cups/language.h>
#include <cups/http.h>
#include <cups/cups.h>
#include <cups/ppd.h>

#include "Printdefaults.h"
#include "FakeXEvent.h"

#define MAX_PRT 5

class Printlib : public FakeXEvent
{
public:
	//static int DEBUG;
	static char PrinterNames[MAX_PRT][DEF_PRN_LEN+1];
	static int PrinterChoice;
	static int PrinterNum;
	static int lastPrinterChoice;
	static char ServerName[MAX_SERVER_NAME+1];
	static char PrinterBannerStart[MAX_PRT][MAXLINE+1];
	static char PrinterBannerEnd[MAX_PRT][MAXLINE+1];
	static char DEF_PRN[DEF_PRN_LEN+1];
	static char PrinterInfos[MAX_PRT][MAXLINE+1];
	static char PrinterLocations[MAX_PRT][MAXLINE+1];
	static char ExtraOpt[MAX_EXTRAOPT+1];
	static char GTKLPRC[MAXPATH+1];
	static char HELPURL[MAXPATH+1];
	static char JobName[MAX_JOB_NAME_LENGTH+1];
	static char Range[MAXRANGEENTRY+1];
	static char confdir[MAXPATH+1];
	static char fileOpenPath[MAXPATH+1];
	static char filesToPrintAnz;
	static char *PPDfilename;
	static unsigned int cppvalue,lpivalue,cpivalue;
	static unsigned int hpgl2pen;
	static unsigned int topmargin, bottommargin, leftmargin, rightmargin;
	static int NumberOfCopies;
	static int collate;
	static int MediaSizeSelected;
	static int MediaTypeSelected;
	static int MediaSourceSelected;
	static int PPDopen;
	static int ReallyPrint;
	static int builtin;
	static int conflict_active;
	static int clearfilelist;
	static int cmdNumCopies;
	static int saveanz;
	static int oldnumcopies;
	static int ignore_stdin;
	static int imageScalingType;
	static int jobidonexit;
	static int nox;
	static int num_commandline_opts;
	static int prettyprint;
	static int rangeType;
	static int rememberprinter;
	static int remembertab;
	static int saveonexit;
	static int mirroroutput;
	static int sheetsType;
	static int tabtoopen;
	static int wantconst;
	static int wantLandscape;
	static int wantRange;
	static int wantReverseOutputOrder;
	static int wantraw;
	static int viewable[6];
	static double brightness;
	static double ggamma;
	static double hue;
	static double imageScalingNomeValue;
	static double imageScalingScalingValue;
	static double imageScalingNaturalValue;
	static double imageScalingPPIValue;
	static double sat;
	static int textwrap;
	static int mainWindowHeight, mainWindowWidth, mainWindowX, mainWindowY, wantSaveSizePos;
	static char nupLayout[5];
	static char borderType[13];
	static cups_option_t *commandline_opts;
	static ppd_file_t *printerPPD;
	static GtkWidget *tabs;
	static GtkWidget *dialog;
	static GtkWidget *mainWindow;
	static cups_lang_t *cupsLanguage;
	static http_t *cupsHttp;
	static GtkObject *brightAdj;
	static GtkObject *gammaAdj;

	static Printlib* GetInstance();
	Printlib(void);
	~Printlib(void);
	void BtnCancelClicked(GtkButton *button);
    void KeyEvent(unsigned char KeyValue);
	void Button_pad(GtkWidget  *widget);
	void Quick_message(gchar *message, int which);
	void GetPrinters(int loop);
	void ExitOnError(char *gerror1, char *gerror2, int exitcode);
	const char *GetPass(const char *prompt);
	int Max(int a, int b);

private:
	static Printlib* m_ptrInstance;

	void Gtklp_fatal_end(char *text);
};


#endif /* PRINTLIB_H_ */
