/*
 * Printfunctions.cpp
 *
 *  Created on: 2012-8-3
 *      Author: yangyaoqing
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <cups/cups.h>
#include <cups/http.h>
#include <cups/language.h>
#include <cups/ppd.h>

#include "Printdefaults.h"
#include "Printlib.h"
#include "Printfunctions.h"
#include "Printgeneral.h"
#include "Printmain.h"
#include "Printtab.h"
#include "Printhpgl2.h"
#include "Printimage.h"
#include "Printoutput.h"
#include "Printppd.h"
#include "Printtext.h"
#include "PrintScreen.h"
#include "ViewDialog.h"
GtkWidget *Printfunctions::button_inst_save = NULL;
GtkWidget *Printfunctions::button_inst_remove = NULL;
GtkWidget *Printfunctions::instwin = NULL;

Printfunctions* Printfunctions::m_ptrInstance = NULL;
Printfunctions *Printfunctions::GetInstance()
{
	if(m_ptrInstance == NULL)
		m_ptrInstance = new Printfunctions;
	return m_ptrInstance;
}

Printfunctions::Printfunctions(void)
{

}

Printfunctions::~Printfunctions(void)
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

int Printfunctions::Teststdin()
{
	fd_set rfds;
	struct timeval tv;
	int retval;
	int ret;

	if(ignore_stdin)
		return(0);

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);

	tv.tv_sec = 0;
	tv.tv_usec = WAITFORSTDIN;

	retval = select(1, &rfds, NULL, NULL, &tv);

	if (retval)
	{
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	return(ret);
}

void Printfunctions::GetPaths(void)
{
	char *z1;

	GTKLPRC[0] = (uintptr_t)NULL;
	if(confdir[0] == (uintptr_t)NULL)
	{
		if(getenv("HOME")==(uintptr_t)NULL)
		{
			;
		}
		else
		{
			//snprintf((char*)confdir, (size_t)MAXPATH, "%s/%s", getenv("HOME"), GTKLPRC_USER);
			snprintf((char*)confdir, (size_t)MAXPATH, "/%s", GTKLPRC_USER);
		}
	}
	else
	{
		z1 = (char*)confdir;
		z1 += strlen((char*)confdir);
		z1--;
		if(*z1 == '/')
			*z1 = (uintptr_t)NULL;
	}
	snprintf((char*)GTKLPRC, (size_t)MAXPATH, "%s/%s", confdir, GTKLPRC_GLOBAL);
}

void Printfunctions::GetOptions(GtkWindow *parent, char *printername, int which)
{
	int i1, hasrc, nohome;
	int needNew = 0;
	int whichfile = 0;
	char printpath[MAXPATH + 1], globalpath[MAXPATH + 1];
	char printpath_g[MAXPATH + 1], globalpath_g[MAXPATH + 1];
	char tmp[MAXLINE + 1];
	char iprintername[DEF_PRN_LEN + 1];
	FILE *file = NULL;
	int num_printer;
	cups_dest_t *all_printer;
	cups_dest_t *act_printer;
	char *instance;
	cups_option_t *option;
	char *z1;
	int screenHeight, screenWidth;
	num_printer = 0;
	all_printer = NULL;
	act_printer = NULL;
	instance = NULL;

	z1 = strchr(printername, '/');
	if(z1 !=  NULL) {
		*z1 = (uintptr_t)NULL;
		snprintf(iprintername, (size_t)DEF_PRN_LEN, "%s\\%s", printername, z1 + 1);
		*z1 = '/';
		z1 ++;
		z1 = strchr(z1, '/');
		if(z1 !=  NULL)
		{
		}
	}
	else
	{
		strncpy(iprintername, printername, (size_t)DEF_PRN_LEN);
	}

	/* Check for user-settings */
	if(!builtin)
	{
		nohome = 0;
		printpath[0] = (uintptr_t)NULL;
		globalpath[0] = (uintptr_t)NULL;
		printpath_g[0] = (uintptr_t)NULL;
		globalpath_g[0] = (uintptr_t)NULL;

		if(getenv("HOME") == (uintptr_t)NULL)
		{
			nohome = 1;
		}

		strncpy(printpath_g, DEF_GTKLPRC, (size_t)MAXPATH);
		strncat(printpath_g, "/", (size_t)MAXPATH);
		strncat(printpath_g, iprintername, (size_t)MAXPATH);
		strncpy(globalpath_g, DEF_GTKLPRC, (size_t)MAXPATH);
		strncat(globalpath_g, "/", (size_t)MAXPATH);
		strncat(globalpath_g, GTKLPRC_GLOBAL, (size_t)MAXPATH);
	}
	else
	{
		nohome = 1;
	}

	if(nohome == 0)
	{
		snprintf(printpath, (size_t)MAXPATH, "%s/%s", confdir, iprintername);
		snprintf(globalpath, (size_t)MAXPATH, "%s/%s", confdir, GTKLPRC_GLOBAL);
	}
	else
	{
		strncpy(printpath, printpath_g, (size_t)MAXPATH);
		strncpy(globalpath, globalpath_g, (size_t)MAXPATH);
	}

	if(which <=  1)
	{
		strncpy(nupLayout, "lrtb", (size_t)4);
		strncpy(borderType, "none", (size_t)12);
		NumberOfCopies = 1;
		wantLandscape = 0;
		wantRange = 0;
		wantReverseOutputOrder = 0;
		Range[0] = (uintptr_t)NULL;
		rangeType = 2;
		mirroroutput = 0;
		sheetsType = 1;
		brightness = 100;
		ggamma = 1000;
		cpivalue = 10;
		lpivalue = 6;
		cppvalue = 1;
		topmargin = 0;
		bottommargin = 0;
		leftmargin = 0;
		rightmargin = 0;
		prettyprint = 0;
		collate = 0;
		textwrap = 0;
		imageScalingType = 0;
		imageScalingNomeValue = 0;//ad
		imageScalingScalingValue = 100;
		imageScalingNaturalValue = 100;
		imageScalingPPIValue = 300;
		hue = 0;
		sat = 100;
		Printhpgl2::hpgl2black = 0;
		Printhpgl2::hpgl2fit = 0;
		hpgl2pen = 1000;
		wantraw = 0;
		ExtraOpt[0] = (uintptr_t)NULL;
		viewable[0] = 1;
		viewable[1] = 1;
		viewable[2] = 1;
		viewable[3] = 1;
		viewable[4] = 1;
		viewable[5] = 1;
		hasrc = 0;

		if(!builtin)
		{
			file = fopen(printpath, "r");
			if(file == (FILE*)NULL)
			{
				file = fopen(printpath_g, "r");
				if(file == (FILE*)NULL)
				{
					;
				}
				else
				{
					hasrc = 1;
					whichfile = 2;
				}
			}
			else
			{
				hasrc = 1;
				whichfile = 1;
			}
		}
		else
		{

		}

		num_printer = cupsGetDests(&all_printer);
		instance = strchr(printername, '/');
		if(instance !=  NULL)
		{
			*instance = (uintptr_t)NULL;
			act_printer = cupsGetDest(printername, instance + 1, num_printer, all_printer);
			*instance = '/';
		}
		else
		{
			act_printer = cupsGetDest(printername, NULL, num_printer, all_printer);
		}

		if(act_printer == NULL)
		{
			//Printmain::GetInstance()->ButtonDestroy(NULL);
			//ViewDialog::GetInstance()->Create(GTK_WINDOW(parent), ViewDialog::INFO, _("No printer found or specified."), NULL);
			return;
		}

		for(i1 = num_commandline_opts, option = commandline_opts; i1>0; option ++, i1--){
			act_printer->num_options = cupsAddOption(option->name, option->value, act_printer->num_options, &act_printer->options);
		}

		if(cmdNumCopies > 0) {
			NumberOfCopies = cmdNumCopies;
		}
		else {
			if(cupsGetOption("copies", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
				NumberOfCopies = atoi(cupsGetOption("copies", act_printer->num_options, act_printer->options));
			oldnumcopies = NumberOfCopies;
		}

		if(cupsGetOption("collate", act_printer->num_options, act_printer->options) !=  NULL) {
			if(strcasecmp(cupsGetOption("collate", act_printer->num_options, act_printer->options), "true") == 0)
				collate = 1;
			else
				collate = 0;
		} else if (ppdFindMarkedChoice(printerPPD, "Collate") !=  NULL) {
			if(strcasecmp(ppdFindMarkedChoice(printerPPD, "Collate")->choice, "true") == 0)
				collate = 1;
			else
				collate = 0;
		}

		if(cupsGetOption("landscape", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			if(strcasecmp(cupsGetOption("landscape", act_printer->num_options, act_printer->options), "true") == 0)
				wantLandscape = 1;
		}

		if(cupsGetOption("page-ranges", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			wantRange = 1;
			strncpy(Range, cupsGetOption("page-ranges", act_printer->num_options, act_printer->options), (size_t)MAXRANGEENTRY);
		}

		if(cupsGetOption("outputorder", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			if(strcasecmp(cupsGetOption("outputorder", act_printer->num_options, act_printer->options), "normal") == 0)
				wantReverseOutputOrder = 0;
			if(strcasecmp(cupsGetOption("outputorder", act_printer->num_options, act_printer->options), "reverse") == 0)
				wantReverseOutputOrder = 1;
		}

		if(cupsGetOption("page-set", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			if(strcasecmp(cupsGetOption("page-set", act_printer->num_options, act_printer->options), "even") == 0)
				rangeType = 1;
			if(strcasecmp(cupsGetOption("page-set", act_printer->num_options, act_printer->options), "odd") == 0)
				rangeType = 0;
		}

		if(cupsGetOption("number-up", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			sheetsType = atoi(cupsGetOption("number-up", act_printer->num_options, act_printer->options));
			if((sheetsType !=  1) & (sheetsType !=  2) & (sheetsType !=  3) & (sheetsType !=  4) & (sheetsType !=  6) & (sheetsType !=  9) & (sheetsType !=  16))
				sheetsType = 1;
		}

		if(cupsGetOption("number-up-layout", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			strncpy(nupLayout, cupsGetOption("number-up-layout", act_printer->num_options, act_printer->options), (size_t)4);
		}

		if(cupsGetOption("mirror", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			if(strcasecmp(cupsGetOption("mirror", act_printer->num_options, act_printer->options), "true") == 0)
				mirroroutput = 1;
		}

		if(cupsGetOption("page-border", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			strncpy(borderType, cupsGetOption("page-border", act_printer->num_options, act_printer->options), (size_t)12);
		}

		if(cupsGetOption("brightness", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
			brightness = atof(cupsGetOption("brightness", act_printer->num_options, act_printer->options));

		if(cupsGetOption("gamma", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
			ggamma = atof(cupsGetOption("gamma", act_printer->num_options, act_printer->options));

		if(cupsGetOption("cpi", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
			cpivalue = (unsigned)atol(cupsGetOption("cpi", act_printer->num_options, act_printer->options));

		if(cupsGetOption("lpi", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
			lpivalue = (unsigned)atol(cupsGetOption("lpi", act_printer->num_options, act_printer->options));

		if(cupsGetOption("columns", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
			cppvalue = (unsigned)atol(cupsGetOption("columns", act_printer->num_options, act_printer->options));

		if(cupsGetOption("page-top", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
			topmargin = (unsigned)atol(cupsGetOption("page-top", act_printer->num_options, act_printer->options));

		if(cupsGetOption("page-bottom", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
			bottommargin = (unsigned)atol(cupsGetOption("page-bottom", act_printer->num_options, act_printer->options));

		if(cupsGetOption("page-left", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
			leftmargin = (unsigned)atol(cupsGetOption("page-left", act_printer->num_options, act_printer->options));

		if(cupsGetOption("page-right", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
			rightmargin = (unsigned)atol(cupsGetOption("page-right", act_printer->num_options, act_printer->options));

		if(cupsGetOption("prettyprint", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			if(strcasecmp(cupsGetOption("prettyprint", act_printer->num_options, act_printer->options), "true") == 0)
				prettyprint = 1;
		}

		if(cupsGetOption("wrap", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			if(strcasecmp(cupsGetOption("wrap", act_printer->num_options, act_printer->options), "true") == 0)
				textwrap = 1;
		}

		if(cupsGetOption("nome", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			imageScalingNomeValue = atof(cupsGetOption("nome", act_printer->num_options, act_printer->options));
			imageScalingType = 0;
		}

		if(cupsGetOption("scaling", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			imageScalingScalingValue = atof(cupsGetOption("scaling", act_printer->num_options, act_printer->options));
			imageScalingType = 1;
		}

		if(cupsGetOption("natural-scaling", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			imageScalingNaturalValue = atof(cupsGetOption("natural-scaling", act_printer->num_options, act_printer->options));
			imageScalingType = 2;
		}

		if(cupsGetOption("ppi", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			imageScalingPPIValue = atof(cupsGetOption("ppi", act_printer->num_options, act_printer->options));
			imageScalingType = 3;
		}

		if(cupsGetOption("hue", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
			hue = atof(cupsGetOption("hue", act_printer->num_options, act_printer->options));

		if(cupsGetOption("saturation", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
			sat = atof(cupsGetOption("saturation", act_printer->num_options, act_printer->options));

		if(cupsGetOption("blackplot", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			if(strcasecmp(cupsGetOption("blackplot", act_printer->num_options, act_printer->options), "true") == 0)
				Printhpgl2::hpgl2black = 1;
		}

		if(cupsGetOption("fitplot", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			if(strcasecmp(cupsGetOption("fitplot", act_printer->num_options, act_printer->options), "true") == 0)
				Printhpgl2::hpgl2fit = 1;
		}

		if(cupsGetOption("penwidth", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL)
			hpgl2pen = (unsigned)atol(cupsGetOption("penwidth", act_printer->num_options, act_printer->options));

		if(cupsGetOption("job-sheets", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			strncpy(tmp, cupsGetOption("job-sheets", act_printer->num_options, act_printer->options), (size_t)MAXLINE);
			z1 = strchr(tmp, ',');
			if(z1 == (uintptr_t)NULL) {
				strncpy(PrinterBannerStart[PrinterChoice], tmp, (size_t)MAXLINE);
			}
			else {
				*z1 = (uintptr_t)NULL;
				z1 ++;
				strncpy(PrinterBannerStart[PrinterChoice], tmp, (size_t)MAXLINE);
				strncpy(PrinterBannerEnd[PrinterChoice], z1, (size_t)MAXLINE);
			}
		}

		if(cupsGetOption("raw", act_printer->num_options, act_printer->options) !=  (uintptr_t)NULL) {
			if(strcasecmp(cupsGetOption("raw", act_printer->num_options, act_printer->options), "true") == 0)
				wantraw = 1;
		}

		if((file !=  NULL) && (hasrc == 1))
		{
			tmp[0] = (uintptr_t)NULL;
			i1 = Freadline(file, tmp, MAXLINE);
			if((JobName[0] == (uintptr_t)NULL) && (tmp[0] !=  '-'))
				strncpy(JobName, _(tmp), (size_t)MAX_JOB_NAME_LENGTH);

			tmp[0] = (uintptr_t)NULL;
			i1 = Freadline(file, tmp, MAXLINE);
			if(tmp[0] !=  '-')
				strncpy(ExtraOpt, _(tmp), (size_t)MAX_EXTRAOPT);
			else
				ExtraOpt[0] = (uintptr_t)NULL;

			fscanf(file, "%i %i %i %i %i %i\n", &viewable[0], &viewable[1], &viewable[2], &viewable[3], &viewable[4], &viewable[5]);

			tmp[0] = (uintptr_t)NULL;
			Freadline(file, tmp, MAXLINE);
			tmp[0] = (uintptr_t)NULL;
			Freadline(file, tmp, MAXLINE);

			fclose(file);

			if(needNew == 1)
			{
				if(whichfile == 1)
				{
					i1 = unlink(printpath);
					if(i1 == -1)
					{
						;
					}
				}
				if(whichfile == 2)
				{
					i1 = unlink(printpath_g);
					if(i1 == -1)
					{
						;
					}
				}
				Printgeneral::GetInstance()->PrinterChanged(PrinterNames[PrinterChoice]);
				return;
			}
		}
	}
	hasrc = 0;

	if(which >= 1)
	{
		saveonexit = 1;
		saveanz = 1;
		rememberprinter = 0;
		remembertab = 0;
		clearfilelist = 1;
		tabtoopen = 0;
		jobidonexit = 0;
		wantconst = 0;
		mainWindowHeight = 0;
		mainWindowWidth = 0;
		mainWindowX = 0;
		mainWindowY = 0;
		//wantSaveSizePos = 0;

		file = fopen(globalpath, "r");
		if(file == (FILE*)NULL)
		{
		file = fopen(globalpath_g, "r");
			if(file == (FILE*)NULL)
			{
				;
			}
			else
			{
				hasrc = 1;
			}
		}
		else
		{
			hasrc = 1;
		}
		if(hasrc == 1)
		{
			tmp[0] = (uintptr_t)NULL;
			Freadline(file, tmp, MAXLINE);	/* last Printer */

			fscanf(file, "%i\n", &rememberprinter);
			if((rememberprinter == 1) && DEF_PRN[0] == (uintptr_t)NULL)
			{
				strncpy(DEF_PRN, tmp, (size_t)DEF_PRN_LEN);
			}
			fscanf(file, "%i\n", &remembertab);
			fscanf(file, "%i\n", &tabtoopen);
			if(remembertab == 0)
				tabtoopen = 0;
			fscanf(file, "%i\n", &saveonexit);
			fscanf(file, "%i\n", &saveanz);
			fscanf(file, "%i\n", &clearfilelist);
			fscanf(file, "%i\n", &jobidonexit);
			tmp[0] = (uintptr_t)NULL;
			Freadline(file, tmp, MAXLINE);
			tmp[0] = (uintptr_t)NULL;
			Freadline(file, tmp, MAXLINE);
			strncpy(HELPURL, tmp, (size_t)MAXPATH);
			tmp[0] = (uintptr_t)NULL;
			Freadline(file, tmp, MAXLINE);
			fscanf(file, "%i\n", &wantconst);

			if(fscanf(file, "%i %i %i %i %i\n", &wantSaveSizePos, &mainWindowX, &mainWindowY,
					&mainWindowWidth, &mainWindowHeight) == EOF) {	/* Old style conf */
				wantSaveSizePos = 0;
				mainWindowHeight = 0;
				mainWindowWidth = 0;
				mainWindowX = 0;
				mainWindowY = 0;
			} else {
				/* validate Size and Pos settings, ignore on too small screens */
				if(nox == 0) {
					screenHeight = gdk_screen_height();
					screenWidth = gdk_screen_width();

					if(mainWindowX < 0)
						mainWindowX = 0;
					if(mainWindowY < 0)
						mainWindowY = 0;

					if(mainWindowX + mainWindowWidth < WINDOWINSCREENBORDER || mainWindowY +
							mainWindowHeight < WINDOWINSCREENBORDER || screenWidth-mainWindowX < WINDOWINSCREENBORDER
							|| screenHeight-mainWindowY < WINDOWINSCREENBORDER) {
						mainWindowHeight = 0;
						mainWindowWidth = 0;
						mainWindowX = 0;
						mainWindowY = 0;
					}
				}
			}
			fclose(file);
	        }
	}
}

int Printfunctions::AddAllOptionsHelp(ppd_group_t *group, cups_option_t **optionsptr, int num_options)
{
	int i1, i2;
	ppd_option_t *option;
	ppd_choice_t *choice;
	ppd_group_t *subgroup;

	for(i1 = group->num_options, option = group->options; i1>0; i1--, option ++)
	{
		for(i2 = option->num_choices, choice = option->choices; i2>0; i2--, choice ++)
		{
			if(choice->marked) {
				if(strncasecmp(option->keyword, "copies", (size_t)6) !=  0)
					num_options = cupsAddOption(option->keyword, choice->choice, num_options, optionsptr);
			}
		}
	}

	for(i1 = group->num_subgroups, subgroup = group->subgroups; i1>0; i1--, subgroup ++)
  		num_options = AddAllOptionsHelp(group, optionsptr, num_options);

	return(num_options);
}

int Printfunctions::AddAllOptions(cups_option_t **optionsptr, int num_options)
{
	int i1;
	ppd_group_t *group;
	char tmp[MAXOPTLEN + 1];
	cups_option_t *option;

	option = NULL;

	if(printerPPD !=  NULL) {
		for(i1 = printerPPD->num_groups, group = printerPPD->groups; i1>0; i1--, group++)
			num_options = AddAllOptionsHelp(group, optionsptr, num_options);
	}

	for(i1 = num_commandline_opts, option = commandline_opts; i1>0; option ++, i1--) {
		num_options = cupsAddOption(option->name, option->value, num_options, optionsptr);
	}

	/* General */
	if(NumberOfCopies > 0) {
		snprintf(tmp, (size_t)MAXOPTLEN, "%i", NumberOfCopies);
		num_options = cupsAddOption("copies", tmp, num_options, optionsptr);
	}

	if(collate == 1)
	  num_options = cupsAddOption("collate", "true", num_options, optionsptr);
	else
	  num_options = cupsAddOption("collate", "false", num_options, optionsptr);

	/* Output */
	if(wantRange == 1 && Range[0] !=  (uintptr_t)NULL)
		num_options = cupsAddOption("page-ranges", Range, num_options, optionsptr);
	if(wantReverseOutputOrder == 1)
		num_options = cupsAddOption("outputorder", "reverse", num_options, optionsptr);
	else
		num_options = cupsAddOption("outputorder", "normal", num_options, optionsptr);
	if(rangeType == 1)
		num_options = cupsAddOption("page-set", "even", num_options, optionsptr);
	if(rangeType == 0)
		num_options = cupsAddOption("page-set", "odd", num_options, optionsptr);
	if(sheetsType <=  16) {
		snprintf(tmp, (size_t)MAXOPTLEN, "%i", sheetsType);
		num_options = cupsAddOption("number-up", tmp, num_options, optionsptr);
	}
	if(strcmp(nupLayout, "lrtb") !=  0)
		num_options = cupsAddOption("number-up-layout", nupLayout, num_options, optionsptr);

	if(mirroroutput == 1)
		num_options = cupsParseOptions("mirror", num_options, optionsptr);

	if(strcmp(borderType, "none") !=  0)
		num_options = cupsAddOption("page-border", borderType, num_options, optionsptr);
	if(wantLandscape == 1)
		num_options = cupsParseOptions("landscape", num_options, optionsptr);
	if(1){//(brightAdj != NULL) {
		if(GTK_ADJUSTMENT(brightAdj)->value != 100) {//??????????????????????????????????????????????????????????
			snprintf(tmp, (size_t)MAXOPTLEN, "%.0f", GTK_ADJUSTMENT(brightAdj)->value);
			num_options = cupsAddOption("brightness", tmp, num_options, optionsptr);
		}
		if(GTK_ADJUSTMENT(gammaAdj)->value !=  1000) {
			snprintf(tmp, (size_t)MAXOPTLEN, "%.0f", GTK_ADJUSTMENT(gammaAdj)->value);
			num_options = cupsAddOption("gamma", tmp, num_options, optionsptr);
		}
	} else {
		snprintf(tmp, (size_t)MAXOPTLEN, "%.0f", brightness);
		num_options = cupsAddOption("brightness", tmp, num_options, optionsptr);
		snprintf(tmp, (size_t)MAXOPTLEN, "%.0f", ggamma);
		num_options = cupsAddOption("gamma", tmp, num_options, optionsptr);
	}

	if(cpivalue !=  10) {
		snprintf(tmp, (size_t)MAXOPTLEN, "%u", cpivalue);
		num_options = cupsAddOption("cpi", tmp, num_options, optionsptr);
	}
	if(lpivalue !=  6) {
		snprintf(tmp, (size_t)MAXOPTLEN, "%u", lpivalue);
		num_options = cupsAddOption("lpi", tmp, num_options, optionsptr);
	}
	if(cppvalue !=  1) {
		snprintf(tmp, (size_t)MAXOPTLEN, "%u", cppvalue);
		num_options = cupsAddOption("columns", tmp, num_options, optionsptr);
	}
	if(topmargin !=  0) {
		snprintf(tmp, (size_t)MAXOPTLEN, "%u", topmargin);
		num_options = cupsAddOption("page-top", tmp, num_options, optionsptr);
	}
	if(bottommargin !=  0) {
		snprintf(tmp, (size_t)MAXOPTLEN, "%u", bottommargin);
		num_options = cupsAddOption("page-bottom", tmp, num_options, optionsptr);
	}
	if(leftmargin !=  0) {
		snprintf(tmp, (size_t)MAXOPTLEN, "%u", leftmargin);
		num_options = cupsAddOption("page-left", tmp, num_options, optionsptr);
	}
	if(rightmargin !=  0) {
		snprintf(tmp, (size_t)MAXOPTLEN, "%u", rightmargin);
		num_options = cupsAddOption("page-right", tmp, num_options, optionsptr);
	}
	if(prettyprint == 1)
		num_options = cupsParseOptions("prettyprint", num_options, optionsptr);
	if(textwrap == 1)
		num_options = cupsParseOptions("wrap = true", num_options, optionsptr);
	else
		num_options = cupsParseOptions("wrap = false", num_options, optionsptr);

	switch(imageScalingType) {
		case 0:	/* None */
			snprintf(tmp, (size_t)MAXOPTLEN, "%.0f", imageScalingNomeValue);
			num_options = cupsAddOption("nome", tmp, num_options, optionsptr);
		break;
		case 1:	/* Sheet */
			snprintf(tmp, (size_t)MAXOPTLEN, "%.0f", imageScalingScalingValue);
			num_options = cupsAddOption("scaling", tmp, num_options, optionsptr);
		break;
		case 2:	/* Image */
			snprintf(tmp, (size_t)MAXOPTLEN, "%.0f", imageScalingNaturalValue);
			num_options = cupsAddOption("natural-scaling", tmp, num_options, optionsptr);
		break;
		case 3:	/* PPI */
			snprintf(tmp, (size_t)MAXOPTLEN, "%.0f", imageScalingPPIValue);
			num_options = cupsAddOption("ppi", tmp, num_options, optionsptr);
		break;
	}
	if(1){//(Printimage::imageHUEAdj != NULL) {//?????????
		if(GTK_ADJUSTMENT(Printimage::imageHUEAdj)->value !=  0) {
			snprintf(tmp, (size_t)MAXOPTLEN, "%.0f", GTK_ADJUSTMENT(Printimage::imageHUEAdj)->value);
			num_options = cupsAddOption("hue", tmp, num_options, optionsptr);
		}
		if(GTK_ADJUSTMENT(Printimage::imageSatAdj)->value !=  100) {
			snprintf(tmp, (size_t)MAXOPTLEN, "%.0f", GTK_ADJUSTMENT(Printimage::imageSatAdj)->value);
			num_options = cupsAddOption("saturation", tmp, num_options, optionsptr);
		}
	}
	else {
		snprintf(tmp, (size_t)MAXOPTLEN, "%.0f", hue);
		num_options = cupsAddOption("hue", tmp, num_options, optionsptr);
		snprintf(tmp, (size_t)MAXOPTLEN, "%.0f", sat);
		num_options = cupsAddOption("saturation", tmp, num_options, optionsptr);
	}

	/* HPGL2 */
	if(Printhpgl2::hpgl2black == 1)
		num_options = cupsParseOptions("blackplot", num_options, optionsptr);
	if(Printhpgl2::hpgl2fit == 1)
		num_options = cupsParseOptions("fitplot", num_options, optionsptr);
	if(hpgl2pen !=  1000) {
		snprintf(tmp, (size_t)MAXOPTLEN, "%u", hpgl2pen);
		num_options = cupsAddOption("penwidth", tmp, num_options, optionsptr);
	}

	return(num_options);
}

void Printfunctions::SetPrinterAsDefault(char *printername)
{
	char *instance;
	cups_dest_t *dests;
	cups_dest_t *dest;
	int num_dests;
	int i1;

	num_dests = 0;
	dest = NULL;
	dests = NULL;

	instance = strchr(printername, '/');
	if(instance !=  NULL)
		*instance = (uintptr_t)NULL;

	num_dests = cupsGetDests(&dests);

	if(instance !=  0)
		dest = cupsGetDest(printername, instance + 1, num_dests, dests);
	else
		dest = cupsGetDest(printername, NULL, num_dests, dests);

	if(dest !=  NULL) {
		for(i1 = 0; i1<num_dests; i1 ++)
			dests[i1].is_default = 0;

		dest->is_default = 1;
		cupsSetDests(num_dests, dests);
	}

	if(instance !=  NULL)
		*instance = '/';
}

int Printfunctions::SaveOptions(char *printername, int onlyglobal)
{
	int i1;
	int num_options = 0;
	cups_option_t *options;
	cups_option_t **optionsptr = NULL;
	char printpath[MAXPATH  +  1], globalpath[MAXPATH  +  1];
	FILE *file;
	int num_printer;
	cups_dest_t *all_printer;
	cups_dest_t *act_printer;
	char *instance;
	char iprintername[DEF_PRN_LEN  +  1];
	char *z1;
	int newinstance = 0;
	char tmp[MAXOPTLEN  +  1];

	if(onlyglobal == 0) {
		options = (cups_option_t *)0;
		num_printer = 0;
		all_printer = NULL;
		act_printer = NULL;
		instance = NULL;
		optionsptr = &options;
	}

	if(builtin)
	{
		return(0);
	}

	if(onlyglobal == 0) {
		z1 = strchr(printername, '/');
		if(z1 !=  NULL) {
			*z1 = (uintptr_t)NULL;
			snprintf(iprintername, (size_t)DEF_PRN_LEN, "%s\\%s", printername, z1 + 1);
			*z1 = '/';
			z1++;
			z1 = strchr(z1, '/');
			if(z1 !=  NULL) {
				;
				}
		}
		else
		{
			strncpy(iprintername, printername, (size_t)DEF_PRN_LEN);
		}
	}

	printpath[0] = (uintptr_t)NULL;
	globalpath[0] = (uintptr_t)NULL;

	if(getenv("HOME") == (uintptr_t)NULL)
	{
		return(-1);
	}

	snprintf(printpath, (size_t)MAXPATH, "%s/", confdir);

	/* try to mkdir this */
	i1 = mkdir(printpath, 0777);//S_IREAD | S_IWRITE | S_IEXEC);

	strncpy(globalpath, printpath, (size_t)MAXPATH);
	strncat(globalpath, GTKLPRC_GLOBAL, (size_t)MAXPATH);

	if(onlyglobal == 0) {
		strncat(printpath, iprintername, (size_t)MAXPATH);
        
		file = fopen(printpath, "wr");
		if(file == (FILE*)NULL)
		{
			Quick_message(_("Error saving user-settings!"), 2);
			return(-1);
		}
		if(JobName[0] == (uintptr_t)NULL)
			fprintf(file, "-\n");
		else
			fprintf(file, "%s\n", JobName);
		if(ExtraOpt[0] == (uintptr_t)NULL)
			fprintf(file, "-\n");
		else
			fprintf(file, "%s\n", ExtraOpt);
		fprintf(file, "%i %i %i %i %i %i\n", viewable[0], viewable[1], viewable[2],
				viewable[3], viewable[4], viewable[5]);
		fclose(file);

		num_printer = cupsGetDests(&all_printer);
		instance = strchr(printername, '/');
		if(instance !=  NULL) {
			*instance = (uintptr_t)NULL;
			act_printer = cupsGetDest(printername, instance + 1, num_printer, all_printer);
			*instance = '/';
		}
		else {
			act_printer = cupsGetDest(printername, NULL, num_printer, all_printer);
		}

		if(act_printer == NULL) {
			if(instance == NULL) {
				;
			} else {		/* New Instance */
				*instance = (uintptr_t)NULL;
				num_printer = cupsAddDest(printername, instance + 1, num_printer, &all_printer);
				if(num_printer >=  MAX_PRT) {
					Quick_message(_("Too many printers!"), 2);
					*instance = '/';
					return(-1);
				}
				act_printer = cupsGetDest(printername, instance + 1, num_printer, all_printer);
				if(act_printer == NULL) {
					Quick_message(_("Unable to add instance!"), 2);
					*instance = '/';
					return(-1);
				}
				*instance = '/';
				newinstance = 1;
			}
		}

		num_options = AddAllOptions(optionsptr, num_options);

		/* But if we don`t want to save the number of copies on exit... */
		if(saveanz == 0) {
			if(NumberOfCopies > 0) {
    		snprintf(tmp, (size_t)MAXOPTLEN, "%i", oldnumcopies);
    		num_options = cupsAddOption("copies", tmp, num_options, optionsptr);
			}
		}

		act_printer->num_options = num_options;
		act_printer->options = options;

		cupsSetDests(num_printer, all_printer);

		cupsFreeOptions(num_options, options);

		/* Set Permissions to read/write only by user */
		//if(chmod(printpath, S_IRUSR | S_IWUSR) == -1) {
		//	if(DEBUG)
		//		printf("Unable to chmod %s: %s\n", printpath, strerror(errno));
		//}
	}

	/* Global settings */

	file = fopen(globalpath, "w");
	if(file == (FILE*)NULL)
	{
		Quick_message(_("Error saving user-settings!"), 2);
		return(-1);
	}
	fprintf(file, "%s\n", printername);
	fprintf(file, "%i\n", rememberprinter);
	fprintf(file, "%i\n", remembertab);
	fprintf(file, "%i\n", gtk_notebook_get_current_page(GTK_NOTEBOOK(tabs)));
	fprintf(file, "%i\n", saveonexit);
	fprintf(file, "%i\n", saveanz);
	fprintf(file, "%i\n", clearfilelist);
	fprintf(file, "%i\n", jobidonexit);
	fprintf(file, "%s\n", HELPURL);
	fprintf(file, "%i\n", wantconst);

	/* save window state if wished */
	if(mainWindow !=  NULL){
		gtk_window_get_position(GTK_WINDOW(mainWindow), &mainWindowX, &mainWindowY);//
		gtk_window_get_size(GTK_WINDOW(mainWindow), &mainWindowWidth, &mainWindowHeight);//
	}

	fprintf(file, "%i %i %i %i %i\n", wantSaveSizePos, mainWindowX, mainWindowY, mainWindowWidth, mainWindowHeight);
	fclose(file);

	/* Set Permissions to read/write only by user */
	if(chmod(globalpath, S_IRUSR | S_IWUSR) == -1) {
		;
	}

	if(rememberprinter == 1)
		SetPrinterAsDefault(printername);

	return(newinstance);
}

void Printfunctions::SetDefaults(void)
{
	Printgeneral::GetInstance()->GeneralTabSetDefaults();
	Printoutput::GetInstance()->OutputTabSetDefaults();
	Printtext::GetInstance()->TextTabSetDefaults();
	Printimage::GetInstance()->ImageTabSetDefaults();
	Printhpgl2::GetInstance()->Hpgl2TabSetDefaults();
	Printtab::GetInstance()->GtklpTabSetDefaults();
	Printppd::GetInstance()->PpdTabSetDefaults();
}

int Printfunctions::Freadline(FILE *file, char *String, unsigned howmuch)
{
	int i1, i2;
	unsigned ct;
	i1 = 0;
	i2 = 0;
	ct = 0;

	while(TRUE)
	{
		i1 = fgetc(file);
		if((i1 !=  EOF) && (i1 !=  10))
		{
			*String++ = (char)i1;
		}
		else
		{
			*String = (uintptr_t)NULL;
			break;
		}
		i2++;
		if(ct < howmuch) {
			ct++;
		}
		else {
			*String = (uintptr_t)NULL;
			return(i2);
		}
	}
	if(i1 == EOF)
		i2 = EOF;
	return(i2);
}

int Printfunctions::GetPPDOpts(char *printername)
{
	int num_printer, i1;
	cups_dest_t *all_printer;
	cups_dest_t *act_printer;
	char *instance;
	cups_option_t *option;

	all_printer = NULL;
	act_printer = NULL;
	num_printer = 0;

	if(PPDopen == 1)
	{
		unlink(PPDfilename);
		ppdClose(printerPPD);
		PPDopen = 0;
		printerPPD = NULL;
	}

	num_printer = cupsGetDests(&all_printer);
	instance = strchr(printername, '/');
	if(instance !=  NULL)
	{
		*instance = (uintptr_t)NULL;
		act_printer = cupsGetDest(printername, instance + 1, num_printer, all_printer);
	}
	else
	{
		act_printer = cupsGetDest(printername, NULL, num_printer, all_printer);
	}

	if(act_printer == NULL)
	{
		;
	}

	PPDfilename = (char *)cupsGetPPD(printername);
	if(PPDfilename == (char *)NULL)
	{
		return(0);
	}

	printerPPD = ppdOpenFile(PPDfilename);
	if(printerPPD == NULL)
	{
		unlink(PPDfilename);
		if(instance !=  NULL)
        *instance = '/';
		return(0);
	}

	ppdMarkDefaults(printerPPD);

	for(i1 = num_commandline_opts, option = commandline_opts; i1>0; option++, i1--)
	{
		act_printer->num_options = cupsAddOption(option->name, option->value, act_printer->num_options, &act_printer->options);
	}

	cupsMarkOptions(printerPPD, act_printer->num_options, act_printer->options);

	PPDopen = 1;

	if(instance !=  NULL)
        *instance = '/';
	return(0);
}

void Printfunctions::ResetButton(gpointer data)
{
	char tmp[MAX_CPI_DIGITS + MAX_LPI_DIGITS + MAX_CPP_DIGITS + 1];

	if(strcmp((char *)data, "Brightness") == 0)
	{
		gtk_adjustment_set_value(GTK_ADJUSTMENT(brightAdj), brightness);
		return;
	}

	if(strcmp((char *)data, "BrightnessDefault") == 0)
	{
		gtk_adjustment_set_value(GTK_ADJUSTMENT(brightAdj), 100);
		return;
	}

	if(strcmp((char *)data, "Gamma") == 0)
	{
		gtk_adjustment_set_value(GTK_ADJUSTMENT(gammaAdj), ggamma);
		return;
	}

	if(strcmp((char *)data, "GammaDefault") == 0)
	{
		gtk_adjustment_set_value(GTK_ADJUSTMENT(gammaAdj), 1000);
		return;
	}

	if(strcmp((char *)data, "CPI") == 0)
	{
		snprintf(tmp, (size_t)MAX_CPI_DIGITS + MAX_LPI_DIGITS + MAX_CPP_DIGITS, "%u", cpivalue);
		gtk_entry_set_text(GTK_ENTRY (Printtext::textCPIField), tmp);
		return;
	}

	if(strcmp((char *)data, "CPIDefault") == 0)
	{
		gtk_entry_set_text(GTK_ENTRY (Printtext::textCPIField), "10");
		return;
	}

	if(strcmp((char *)data, "LPI") == 0)
	{
		snprintf(tmp, (size_t)MAX_CPI_DIGITS + MAX_LPI_DIGITS + MAX_CPP_DIGITS, "%u", lpivalue);
		gtk_entry_set_text(GTK_ENTRY (Printtext::textLPIField), tmp);
		return;
	}

	if(strcmp((char *)data, "LPIDefault") == 0)
	{
		gtk_entry_set_text(GTK_ENTRY (Printtext::textLPIField), "6");
		return;
	}

	if(strcmp((char *)data, "CPP") == 0)
	{
		snprintf(tmp, (size_t)MAX_CPI_DIGITS + MAX_LPI_DIGITS + MAX_CPP_DIGITS, "%u", cppvalue);
		gtk_entry_set_text(GTK_ENTRY (Printtext::textCPPField), tmp);
		return;
	}

	if(strcmp((char *)data, "CPPDefault") == 0)
	{
		gtk_entry_set_text(GTK_ENTRY (Printtext::textCPPField), "1");
		return;
	}

	if(strcmp((char *)data, "HUE") == 0)
	{
		gtk_adjustment_set_value(GTK_ADJUSTMENT(Printimage::imageHUEAdj), hue);
		return;
	}

	if(strcmp((char *)data, "HUEDefault") == 0)
	{
		gtk_adjustment_set_value(GTK_ADJUSTMENT(Printimage::imageHUEAdj), 0);
		return;
	}

	if(strcmp((char *)data, "Sat") == 0)
	{
		gtk_adjustment_set_value(GTK_ADJUSTMENT(Printimage::imageSatAdj), sat);
		return;
	}

	if(strcmp((char *)data, "SatDefault") == 0)
	{
		gtk_adjustment_set_value(GTK_ADJUSTMENT(Printimage::imageSatAdj), 100);
		return;
	}

	if(strcmp((char *)data, "HPGL2PenDefault") == 0)
	{
		gtk_entry_set_text(GTK_ENTRY (Printhpgl2::hpgl2PenField), "1000");
		return;
	}

}

void Printfunctions::CreateWidgets(void)
{
	Printgeneral::GetInstance()->GeneralTab();
	gtk_widget_show(Printgeneral::generalTab);
	Printoutput::GetInstance()->OutputTab();
	gtk_widget_show(Printoutput::outputTab);
	Printtext::GetInstance()->TextTab();
	gtk_widget_show(Printtext::textTab);
	Printimage::GetInstance()->ImageTab();
	gtk_widget_show(Printimage::imageTab);
	Printhpgl2::GetInstance()->Hpgl2Tab();
	gtk_widget_show(Printhpgl2::hpgl2Tab);
	Printppd::GetInstance()->PPDTab();
	gtk_widget_show(Printppd::ppdTab);
	Printtab::GetInstance()->GtkPrinterTab();
	gtk_widget_show(Printtab::gtklpTab);
}

void Printfunctions::ShowWidgets(void)
{
	//sleep(SLEEPBEFORE);
	SetDefaults();

	/* Tabs */
	if(printerPPD == NULL)
		viewable[5] = 0;
	if(!viewable[0])
		gtk_widget_hide(Printoutput::outputTab);
	if(!viewable[1])
		gtk_widget_hide(Printtext::textTab);
	if(!viewable[3])
		gtk_widget_hide(Printhpgl2::hpgl2Tab);
	if(!viewable[5])
		gtk_widget_hide(Printppd::ppdTab);

	/* Frames in general */
	gtk_widget_show(Printgeneral::printerFrame);
	gtk_widget_show(Printgeneral::printerNumCopiesFrame);
	if((ppdFindOption(printerPPD, "PageSize") !=  NULL) || (ppdFindOption(printerPPD, "MediaType") !=  NULL) ||
			(ppdFindOption(printerPPD, "InputSlot") !=  NULL) || (ppdFindOption(printerPPD, "PageRegion") !=  NULL))
		gtk_widget_show(Printgeneral::printerMediaFrame);

	/* Frames in output */
	gtk_widget_show(Printoutput::sheetsFrame);
	gtk_widget_show(Printoutput::brightFrame);
	gtk_widget_show(Printoutput::gammaFrame);

	/* Frames in text */
	gtk_widget_show(Printtext::textSizesFrame);
	gtk_widget_show(Printtext::textMarginsFrame);
	gtk_widget_show(Printtext::textPrettyFrame);

	/* Frames in image */
	gtk_widget_show(Printimage::imageScalingAllFrame);
	gtk_widget_show(Printimage::imageHUEFrame);
	gtk_widget_show(Printimage::imageSatFrame);

	/* Frames in hpgl2 */
	gtk_widget_show(Printhpgl2::hpgl2OptFrame);
	gtk_widget_show(Printhpgl2::hpgl2PenFrame);

	/* Frames in tab */
	//gtk_widget_show(Printtab::gtklpViewable);
	gtk_widget_show(Printtab::prefsFrame);
}

void Printfunctions::SaveFunc( GtkWidget *widget, gpointer instance)
{
	char tmp[DEF_PRN_LEN + 1];
	char *z1;
	int i1, so;
	GList *printerList = NULL;
	int merk;

	z1 = strchr(PrinterNames[PrinterChoice], '/');
	if(z1 !=  NULL)
		*z1 = (uintptr_t)NULL;

	if(strlen((char *)gtk_entry_get_text(GTK_ENTRY(instance))) !=  0) {
		snprintf(tmp, (size_t)DEF_PRN_LEN, "%s/%s", PrinterNames[PrinterChoice], (char *)gtk_entry_get_text(GTK_ENTRY(instance)));
		if(z1 !=  NULL)
			*z1 = '/';

		so = SaveOptions(tmp, 0);
		if(so == 1) {
			GetPrinters(0);
			merk = 0;
			for(i1 = 0; i1<= PrinterNum; i1++) {
				printerList = g_list_append(printerList, _(PrinterNames[i1]));
				if(strcmp(PrinterNames[i1], tmp) == 0)
					merk = i1;
			}
			gtk_combo_set_popdown_strings (GTK_COMBO(Printgeneral::printerFrameCombo), printerList);
			gtk_list_select_item(GTK_LIST(GTK_COMBO(Printgeneral::printerFrameCombo)->list), merk);
		}
		if(so >=  0) {
			gtk_widget_destroy(instwin);
		}
	}
	else {
		if(SaveOptions(PrinterNames[PrinterChoice], 0) >=  0) {
			Quick_message(_("Options saved!"), 1);
			gtk_widget_destroy(instwin);
		}
		if(z1 !=  NULL)
			*z1 = '/';
	}
}

void Printfunctions::ResetFunc(void)
{
	GetOptions(NULL, PrinterNames[PrinterChoice], 2);
	Printgeneral::GetInstance()->PrinterChanged(PrinterNames[PrinterChoice]);
}

void Printfunctions::PrintFunc(const char *path)
{
	FILE *spool;
	char tmp[MAXOPTLEN + 1];
	char tmppath[MAXPATH + 1];
	char *z1;
	int i1;
	int num_options, jobid;
	cups_option_t *options;
	cups_option_t **optionsptr;
	const char *files = (char *)malloc(MAXPRINTFILES + 1);
	char tmpprn[DEF_PRN_LEN  +  1];
	char *spoolfile;
	PrintScreen *Ps = new PrintScreen;

	files = path;
    //printf("file name %s\n", path);

	num_options = 0;
	options = (cups_option_t *)0;
	optionsptr = &options;
	num_options = AddAllOptions(optionsptr, num_options);

	snprintf(tmpprn, (size_t)DEF_PRN_LEN, "%s", PrinterNames[PrinterChoice]);
	z1 = strchr(tmpprn, '/');
	if(z1 !=  NULL)
		*z1 = (uintptr_t)NULL;

    if(ReallyPrint == 1)
	{
 		z1 = strrchr((char *)files, '/');
		if(z1 == (char *)NULL)
			z1 = (char *)files;
		else
			z1++;
		jobid = cupsPrintFile(tmpprn, files, z1, num_options, options);
		if(jobid <=  0)
			g_print("\n%s\n\n", _("Unable to print, \nunknown file format!"));
		if(jobidonexit)
			g_print("%s: %i\n", _("JobID"), jobid);
		cupsFreeOptions(num_options, options);
	}
#if 0
	if((ReallyPrint == 1) && (Teststdin() == 0))
	{
 		z1 = strrchr((char *)files, '/');
		if(z1 == (char *)NULL)
			z1 = (char *)files;
		else
			z1++;
		jobid = cupsPrintFile(tmpprn, files, z1, num_options, options);
		if(jobid <=  0)
			g_print("\n%s\n\n", _("Unable to print, \nunknown file format!"));
		if(jobidonexit)
			g_print("%s: %i\n", _("JobID"), jobid);
		cupsFreeOptions(num_options, options);
	}

	if((ReallyPrint == 1) && (Teststdin() == 1))
	{
 		spoolfile = (char *)cupsTempFile2(tmppath, sizeof(tmppath));

		if(spoolfile == (uintptr_t)NULL)
		{
			g_print("\n%s\n\n", _("Unable to print!"));
		}
	else
	{
		spool = fopen(spoolfile, "w");
		if(spool == (FILE *)NULL)
		{
			g_print("\n%s\n\n", _("Unable to print!"));
		}
		else
		{
			while ((i1 = fread(tmp, 1, sizeof(tmp), stdin)) > 0)
			fwrite(tmp, 1, i1, spool);
			i1 = ftell(spool);
			fclose(spool);
			if(i1 == 0)
			{
				;
			}

			if(JobName[0] !=  (uintptr_t)NULL)
			{
 				jobid = cupsPrintFile(tmpprn, spoolfile, JobName, num_options, options);
            }
			else
			{
 				jobid = cupsPrintFile(tmpprn, spoolfile, _("(stdin)"), num_options, options);
			}

			if(jobid <=  0)
			{
				g_print("\n%s\n\n", _("Unable to print!"));
			}
			if(jobidonexit)
				g_print("%s: %i\n", _("JobID"), jobid);

			cupsFreeOptions(num_options, options);
			unlink(tmppath);
			}
		}
	}
#endif
	if(ReallyPrint == 0)	/* Not really Print, only list ! */
	{
		for(i1 = num_options-1; i1 >= 0; i1--) {
			g_print(" -o %s = %s", options[i1].name, options[i1].value);
		}
		g_print(" -P%s", PrinterNames[PrinterChoice]);
		if(JobName[0] !=  (uintptr_t)NULL)
			g_print(" -J \"%s\"", JobName);
		cupsFreeOptions(num_options, options);
	}
	delete Ps;
	//free(files);
}

void Printfunctions::PrintPPDoptsHelp(ppd_group_t *group)
{
	int i1, i2;
	ppd_option_t *option;
	ppd_choice_t *choice;
	ppd_group_t *subgroup;

	for(i1 = group->num_options, option = group->options; i1>0; i1--, option++)
	{
		for(i2 = option->num_choices, choice = option->choices; i2>0; i2--, choice++)
		{
			if(choice->marked)
			{
				g_print("[%s = %s] ", option->keyword, choice->choice);
			}
			else
			{
				g_print("%s ", choice->choice);
			}
			if(option->conflicted)
				g_print("CONFLICT: %s %s \n", option->text, choice->text);
		}
		g_print("\n");
	}

	for(i1 = group->num_subgroups, subgroup = group->subgroups; i1>0; i1--, subgroup ++)
		PrintPPDoptsHelp(subgroup);
}

void Printfunctions::Constraints(int prob)
{
	//printf("Printfunctions::Constraints(int prob)\n");
	unsigned int   i, j, k, count;
	ppd_const_t *c;
	ppd_group_t *g, *sg;
	ppd_option_t  *o1, *o2;
	ppd_choice_t  *c1, *c2;
	GtkWidget *constwin;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *table;
	GtkWidget *label;

	if((prob <=  0) || (printerPPD == NULL) || (conflict_active == 0) || (wantconst == 0))
		return;

	count = 0;
	constwin = gtk_dialog_new_with_buttons((_("Conflicts")), GTK_WINDOW(mainWindow),
			(GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT), NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(constwin), 0);

	hbox = gtk_hbox_new(FALSE, INFRAME_SPACING_H);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(constwin)->vbox), hbox);
	gtk_widget_show(hbox);

	vbox = gtk_vbox_new(FALSE, INFRAME_SPACING_V);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, INFRAME_SPACING_H);
	gtk_widget_show(vbox);

	table = gtk_table_new(3, 7, FALSE);
	gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, INFRAME_SPACING_V);
	gtk_widget_show(table);

	for (i = printerPPD->num_groups, g = printerPPD->groups; i > 0; i--, g++) {
		for (j = g->num_options, o1 = g->options; j > 0; j--, o1++)
			o1->conflicted = 0;
		for (j = g->num_subgroups, sg = g->subgroups; j > 0; j--, sg++) {
			for (k = sg->num_options, o1 = sg->options; k > 0; k--, o1++)
				o1->conflicted = 0;
		}
	}

	for (i = printerPPD->num_consts, c = printerPPD->consts; i > 0; i--, c++) {
		o1 = ppdFindOption(printerPPD, c->option1);
		if (o1 == NULL)
			continue;
		else if (c->choice1[0] !=  '\0') {
			c1 = ppdFindChoice(o1, c->choice1);
		}
		else {
			for (j = o1->num_choices, c1 = o1->choices; j > 0; j--, c1++) {
				if (c1->marked)
					break;
			}
			if (j == 0 || strcasecmp(c1->choice, "none") == 0 || strcasecmp(c1->choice, "off") == 0 || strcasecmp(c1->choice, "false") == 0)
				c1 = NULL;
		}

		o2 = ppdFindOption(printerPPD, c->option2);
		if (o2 == NULL)
			continue;
		else if (c->choice2[0] !=  '\0') {
			c2 = ppdFindChoice(o2, c->choice2);
		}
		else {
			for (j = o2->num_choices, c2 = o2->choices; j > 0; j--, c2++) {
				if (c2->marked)
					break;
			}
			if (j == 0 || strcasecmp(c2->choice, "none") == 0 || strcasecmp(c2->choice, "off") == 0 || strcasecmp(c2->choice, "false") == 0)
				c2 = NULL;
		}

		if ((c1 !=  NULL) && (c1->marked) && (c2 !=  NULL) && (c2->marked))	{
			gtk_table_resize(GTK_TABLE(table), count + 1, 7);

			label = gtk_label_new(_(o1->text));
			gtk_widget_show(label);
			gtk_table_attach(GTK_TABLE(table), label, 0, 1, count, count + 1, (GtkAttachOptions)0, (GtkAttachOptions)0, 0, 0);

			label = gtk_label_new(_(c1->text));
			gtk_widget_show(label);
			gtk_table_attach(GTK_TABLE(table), label, 2, 3, count, count + 1, (GtkAttachOptions)0, (GtkAttachOptions)0, 0, 0);

			label = gtk_label_new(_(o2->text));
			gtk_widget_show(label);
			gtk_table_attach(GTK_TABLE(table), label, 4, 5, count, count + 1, (GtkAttachOptions)0, (GtkAttachOptions)0, 0, 0);

			label = gtk_label_new(_(c2->text));
			gtk_widget_show(label);
			gtk_table_attach(GTK_TABLE(table), label, 6, 7, count, count + 1, (GtkAttachOptions)0, (GtkAttachOptions)0, 0, 0);

			o1->conflicted = 1;
			o2->conflicted = 1;
			count++;
		}
	}
	gtk_dialog_run(GTK_DIALOG(constwin));
	gtk_widget_destroy(constwin);
}

void Printfunctions::PPD_DropDown_changed(GtkWidget *widget, gpointer data)
{
	ppd_choice_t *choice;
	ppd_option_t *option;
	int i1;
	char *tmp[3] = {(char *)"PageSize", (char *)"MediaType", (char *)"InputSlot"};

	for(int i = 0; i < 3; i++){
		option = ppdFindOption(printerPPD, (const char *) tmp[i]);
		if(option != NULL)
			for(i1 = option->num_choices, choice = option->choices; i1 > 0; i1--, choice++)
			{
				if(strcmp(_(choice->text), gtk_entry_get_text(GTK_ENTRY(widget))) == 0)
				{
					Constraints(ppdMarkOption(printerPPD, option->keyword, choice->choice));
					i1 = 0;
				}
			}
	}
}

void Printfunctions::SaveGtkLPPrefs(GtkButton *widget)
{
	if(SaveOptions(PrinterNames[PrinterChoice], 0) >= 0);
		//Quick_message(_("Options saved!"),1);
}

void Printfunctions::InstWinEntryChanged(GtkWidget *widget)
{
	int i1;
	char tmp[DEF_PRN_LEN + 1];
	char *z1;

	if(strlen((char *)gtk_entry_get_text(GTK_ENTRY(widget))) == 0) {
		gtk_button_set_label(GTK_BUTTON(button_inst_save), GTK_STOCK_SAVE);
		gtk_widget_set_sensitive ((GtkWidget *) button_inst_remove, FALSE);
		return;
	}

	z1 = strchr(PrinterNames[PrinterChoice], '/');
	if(z1 !=  NULL)
		*z1 = (uintptr_t)NULL;
	snprintf(tmp, (size_t)DEF_PRN_LEN, "%s/%s", PrinterNames[PrinterChoice], (char *)gtk_entry_get_text(GTK_ENTRY(widget)));
	if(z1 !=  NULL)
		 *z1 = '/';

	for(i1 = 0; i1 <= PrinterNum; i1++) {
		if(strcmp(tmp, PrinterNames[i1]) == 0) {
			gtk_button_set_label(GTK_BUTTON(button_inst_save), GTK_STOCK_SAVE);
			gtk_widget_set_sensitive ((GtkWidget *) button_inst_remove, TRUE);
			return;
		}
	}

	gtk_button_set_label(GTK_BUTTON(button_inst_save), GTK_STOCK_ADD);
	gtk_widget_set_sensitive ((GtkWidget *) button_inst_remove, FALSE);
}

void Printfunctions::ButtonWinEntryChange(GtkWidget *widget)
{
	InstWinEntryChanged(widget);
}


bool Printfunctions::PrinterforPC(void)
{}
