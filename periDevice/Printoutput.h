/*
 * Printoutput.h
 *
 *  Created on: 2012-8-6
 *      Author: yangyaoqing
 */

#ifndef PRINTOUTPUT_H_
#define PRINTOUTPUT_H_

#include <glib.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include "Printlib.h"

class Printoutput : public Printlib
{
public:
	static Printoutput *GetInstance();
	Printoutput(void);
	~Printoutput(void);
	static GtkWidget *rangesFrame;
	static GtkWidget *brightFrame;
	static GtkWidget *gammaFrame;
	static GtkWidget *sheetsFrame;
	static GtkWidget *outputTab;

	void OutputTabSetDefaults(void);
	void OutputTab(void);

private:
	static Printoutput *m_ptrInstance;
	static 	GSList *sheetsButtons, *nupLayButtons, *borderButtons, *orientButtons;
	static 	GtkWidget *nupCombo;
	static GtkWidget *nupStyleButton;
	static int nupStyle;
	static GtkWidget *nupPixMapWid;
	static GSList *rangeButtons;
	static GtkWidget  *rangeCheck;
	static GtkWidget  *rangeText;
	static GtkWidget *landscapeCheck;
	static GtkWidget *mirrorCheck;
	static GtkWidget *outputOrderCheck;

	void OutputOrderCheckChanged(GtkWidget *widget, gpointer data);
	void LandscapeCheckChanged(gpointer data);
	void NupStyleChange(gpointer data);
	void WantMirrorCheck(GtkWidget *widget);
	void SheetUsageFrame(GtkWidget *container);
	void BrightFrame(GtkWidget *container);
	void GammaFrame(GtkWidget *container);

	void ButtonNupStyleChange(gpointer data);
	void ButtonResetButton(gpointer data);
	static void on_button_nupstylechange(GtkWidget *button, gpointer dat, Printoutput *data) {data->ButtonNupStyleChange(dat);}
	static void on_button_resetbutton(GtkWidget *button, gpointer dat, Printoutput *data) {data->ButtonResetButton(dat);}
	static void on_button_wantmirrorcheck(GtkWidget *button, GtkWidget *widget, Printoutput *data)
	{data->WantMirrorCheck(widget);}
	static void on_button_landscapecheckchanged(GtkWidget *button, gpointer *dat, Printoutput *data)
	{data->LandscapeCheckChanged(dat);}

};

#endif /* PRINTOUTPUT_H_ */
